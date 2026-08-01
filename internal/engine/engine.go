// Package engine ties everything together: prepare, check, build, run.
package engine

import (
	"fmt"
	"path/filepath"
	"runtime"
	"sync"
	"time"

	"tester42/internal/build"
	"tester42/internal/platform"
	"tester42/internal/prereq"
	"tester42/internal/result"
	"tester42/internal/runner"
	"tester42/internal/spec"
)

// Options controls a run.
type Options struct {
	// Extra runs the groups the real moulinette does not have.
	Extra bool
	// StrictAlloc fails a test when a function allocates more than it needs.
	// The moulinette does not check this, so it is off by default.
	StrictAlloc bool
	// Leaks turns on the malloc interposer. Unavailable on Windows.
	Leaks bool
	// Sanitize builds the harness with the address/UB sanitizers.
	Sanitize bool
	// Timeout bounds one test invocation.
	Timeout time.Duration
	// Jobs is how many groups are compiled and run at once.
	Jobs int
	// RunAll keeps going after a failure instead of stopping there. The
	// moulinette stops, so that is the default; this is for when you would
	// rather see everything that is broken in one pass.
	RunAll bool
	// Progress is called as groups finish, for a live display.
	Progress func(done, total int, group string)
}

// Run tests one submission and returns the report.
func Run(project *spec.Project, dir string, opts Options) (*result.Report, []string, error) {
	rep := &result.Report{Project: project}

	tool, err := platform.Detect(opts.Sanitize)
	if err != nil {
		return nil, nil, err
	}
	notes := tool.Notes

	if runtime.GOOS == "windows" && !tool.SupportsPOSIX() {
		return nil, notes, fmt.Errorf(
			"this compiler cannot build POSIX code (read/write/ssize_t).\n"+
				"On Windows, run the tester inside WSL: wsl 42tester %s", dir)
	}
	if opts.Leaks && runtime.GOOS == "windows" {
		opts.Leaks = false
		notes = append(notes, "leak tracking is not available on Windows; run under WSL for it")
	}
	if runtime.GOOS == "windows" && project.ID == "printf" {
		// The tests use the system printf as the reference. That is exactly
		// right on the machines the moulinette runs on, but Windows prints
		// pointers as 0000000000000001 where Linux prints 0x1 and macOS
		// prints 0x1, so the pointer group cannot be trusted here.
		notes = append(notes, "on Windows the C library prints %p as 0000000000000001 instead of 0x1, "+
			"so ft_printf_pointer will fail even on correct code; check that group on Linux or macOS")
	}

	// Expected files is checked before anything is compiled, exactly like
	// the moulinette: there is no point building a submission that is
	// missing its header.
	rep.Prerequisites = append(rep.Prerequisites, prereq.ExpectedFiles(dir, project))
	if !rep.PrerequisitesOK() {
		rep.Aborted = true
		rep.AbortReason = rep.Prerequisites[0].Detail
		rep.Prerequisites = append(rep.Prerequisites,
			result.Check{Name: "Allowed functions", Status: result.Skipped})
		return rep, notes, nil
	}

	b, err := build.New(tool, project, dir, opts.Sanitize)
	if err != nil {
		return nil, notes, err
	}
	defer b.Cleanup()

	if opts.Leaks {
		b.Defines = append(b.Defines, "-DT_LEAKS=1")
		if runtime.GOOS == "linux" {
			b.LinkFlags = append(b.LinkFlags, "-ldl")
		}
	}
	if opts.StrictAlloc {
		b.Defines = append(b.Defines, "-DT_STRICT_ALLOC=1")
	}

	makeLog, err := b.BuildLibrary()
	if err != nil {
		rep.Aborted = true
		rep.AbortReason = err.Error()
		if makeLog != "" {
			rep.AbortReason += "\n" + makeLog
		}
		rep.Prerequisites = append(rep.Prerequisites,
			result.Check{Name: "Allowed functions", Status: result.Skipped})
		return rep, notes, nil
	}

	// Some students put the list functions behind a `bonus` rule. Building it
	// when it exists is harmless and saves them a confusing failure; the
	// functions are required either way.
	b.BuildBonus()

	// Both remaining checks read the archive, so they can only run once
	// `make` has produced it.
	objects := objectsToInspect(b)

	// Functions that are not written yet fail at their own place in the
	// sequence rather than all at once up front, so a run always ends on the
	// next thing to do.
	missing := prereq.MissingFunctions(dir, tool.NM, objects, project)

	rep.Prerequisites = append(rep.Prerequisites,
		prereq.AllowedFunctions(tool.NM, objects, project))

	// The report's own groups first, so its section order is preserved, then
	// what the subject demands beyond it.
	groups := append([]spec.Group{}, project.Groups...)
	groups = append(groups, project.RequiredGroups...)
	if opts.Extra {
		groups = append(groups, project.ExtraGroups...)
	}

	rep.Groups = make([]*result.Group, len(groups))
	for i, g := range groups {
		rep.Groups[i] = &result.Group{Spec: g}
	}

	if !opts.RunAll {
		runUntilFailure(b, rep, missing, opts)
		return rep, notes, nil
	}

	jobs := opts.Jobs
	if jobs <= 0 {
		jobs = runtime.NumCPU()
	}
	sem := make(chan struct{}, jobs)
	var wg sync.WaitGroup
	var mu sync.Mutex
	done := 0

	for i := range rep.Groups {
		wg.Add(1)
		go func(i int) {
			defer wg.Done()
			sem <- struct{}{}
			defer func() { <-sem }()

			rg := rep.Groups[i]
			if reason, ok := missing[rg.Spec.Name]; ok {
				markNotWritten(rg, reason)
			} else {
				runGroup(b, rg, opts.Timeout, b.Run)
			}

			mu.Lock()
			done++
			if opts.Progress != nil {
				opts.Progress(done, len(rep.Groups), rg.Spec.Name)
			}
			mu.Unlock()
		}(i)
	}
	wg.Wait()

	return rep, notes, nil
}

// runUntilFailure walks the groups in subject order and stops at the first
// test that does not pass, dropping everything after it. That is what the
// moulinette shows: the run ends where the problem is.
//
// Compilation still happens in parallel up front, because it is the slow
// part and it does not change where the first failure lands.
func runUntilFailure(b *build.Builder, rep *result.Report, missing map[string]string, opts Options) {
	compiled := compileAll(b, rep, missing, opts)

	for i, rg := range rep.Groups {
		if rg.Compilation == result.Skipped {
			continue
		}
		if rg.Compilation != result.OK {
			rep.Groups = rep.Groups[:i+1]
			return
		}

		rg.Cases = runner.New(opts.Timeout, b.Run).RunGroup(compiled[i], rg.Spec)
		if opts.Progress != nil {
			opts.Progress(i+1, len(rep.Groups), rg.Spec.Name)
		}

		for j, c := range rg.Cases {
			if c.Status.Passed() || c.Status == result.Skipped {
				continue
			}
			// Keep the failing case: it is the one to look at.
			rg.Cases = rg.Cases[:j+1]
			rep.Groups = rep.Groups[:i+1]
			return
		}
	}
}

// compileAll builds every group at once and returns the executables, indexed
// like rep.Groups. A group that fails to build has its status set here and an
// empty path.
func compileAll(b *build.Builder, rep *result.Report, missing map[string]string, opts Options) []string {
	jobs := opts.Jobs
	if jobs <= 0 {
		jobs = runtime.NumCPU()
	}
	exes := make([]string, len(rep.Groups))
	sem := make(chan struct{}, jobs)
	var wg sync.WaitGroup
	var mu sync.Mutex
	built := 0

	for i := range rep.Groups {
		rg := rep.Groups[i]
		if reason, ok := missing[rg.Spec.Name]; ok {
			markNotWritten(rg, reason)
			continue
		}
		wg.Add(1)
		go func(i int, rg *result.Group) {
			defer wg.Done()
			sem <- struct{}{}
			defer func() { <-sem }()

			exe, log, err := b.CompileGroup(rg.Spec)
			rg.CompileLog = log
			if err != nil {
				rg.Compilation = result.KO
				rg.Cases = nil
			} else {
				rg.Compilation = result.OK
				exes[i] = exe
			}

			mu.Lock()
			built++
			if opts.Progress != nil {
				opts.Progress(built, len(rep.Groups), "compiling "+rg.Spec.Name)
			}
			mu.Unlock()
		}(i, rg)
	}
	wg.Wait()
	return exes
}

// runGroup compiles and executes a single group.
func runGroup(b *build.Builder, rg *result.Group, timeout time.Duration, runDir string) {
	exe, log, err := b.CompileGroup(rg.Spec)
	if err != nil {
		rg.Compilation = result.KO
		rg.CompileLog = log
		rg.Cases = skippedCases(rg.Spec)
		for i := range rg.Cases {
			rg.Cases[i].Status = result.KO
			rg.Cases[i].Detail = "not run: the test could not be compiled against this submission"
		}
		return
	}
	rg.Compilation = result.OK
	rg.CompileLog = log
	rg.Cases = runner.New(timeout, runDir).RunGroup(exe, rg.Spec)
}

func skippedCases(g spec.Group) []result.Case {
	cases := make([]result.Case, g.Cases)
	for i := range cases {
		cases[i] = result.Case{Number: i + 1, Name: g.TestName(i + 1), Status: result.Skipped}
	}
	return cases
}

// objectsToInspect returns what nm should read: the archive when there is
// one, the compiled sources otherwise.
func objectsToInspect(b *build.Builder) []string {
	if lib := b.LibraryPath(); lib != "" {
		return []string{lib}
	}
	var objs []string
	for _, s := range b.Project.Sources {
		obj := filepath.Join(b.Src, s[:len(s)-len(filepath.Ext(s))]+".o")
		if _, _, err := b.CompileObject(s, obj); err == nil {
			objs = append(objs, obj)
		}
	}
	return objs
}

// markNotWritten fails a group whose function does not exist yet, without
// trying to compile a test that cannot possibly link.
func markNotWritten(rg *result.Group, reason string) {
	rg.Compilation = result.KO
	rg.CompileLog = reason
	rg.Cases = skippedCases(rg.Spec)
	for i := range rg.Cases {
		rg.Cases[i].Status = result.KO
		rg.Cases[i].Detail = reason
	}
}
