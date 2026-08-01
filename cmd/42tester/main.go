// Command 42tester grades libft, ft_printf and get_next_line submissions and
// prints a report in the moulinette's own format.
//
// Point it at a directory and it works out which project is in there:
//
//	42tester .
package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"runtime"
	"strings"
	"time"

	"tester42/internal/detect"
	"tester42/internal/engine"
	"tester42/internal/report"
	"tester42/internal/result"
	"tester42/internal/spec"
)

// version is overridden at build time with -ldflags.
var version = "dev"

type flags struct {
	project     string
	markdown    string
	jsonOut     string
	verbose     bool
	extra       bool
	strictAlloc bool
	leaks       bool
	sanitize    bool
	noColor     bool
	noMarkdown  bool
	runAll      bool
	noBonus     bool
	timeout     time.Duration
	jobs        int
	showVersion bool
}

func main() {
	var f flags
	flag.StringVar(&f.project, "p", "", "force the project (libft, printf, gnl) instead of detecting it")
	flag.StringVar(&f.markdown, "o", "moulinette_report.md", "where to write the moulinette-format report")
	flag.StringVar(&f.jsonOut, "json", "", "also write a machine-readable report to this file")
	flag.BoolVar(&f.verbose, "v", false, "list every test, not just the failures")
	flag.BoolVar(&f.extra, "extra", false, "run the extra tests the moulinette does not have (%d, %i, %u, multiple fd, ...)")
	flag.BoolVar(&f.strictAlloc, "strict-alloc", false, "fail tests that allocate more memory than needed")
	flag.BoolVar(&f.leaks, "leaks", false, "track memory leaks (Linux and macOS)")
	flag.BoolVar(&f.sanitize, "sanitize", false, "build with the address and undefined-behaviour sanitizers")
	flag.BoolVar(&f.noColor, "no-color", false, "disable coloured output")
	flag.BoolVar(&f.noMarkdown, "no-report", false, "skip writing the markdown report")
	flag.BoolVar(&f.runAll, "all", false, "keep going after a failure instead of stopping there like the moulinette")
	flag.BoolVar(&f.noBonus, "no-bonus", false, "do not fail when the bonus is missing; skip those tests instead")
	flag.DurationVar(&f.timeout, "timeout", 5*time.Second, "time limit for a single test")
	flag.IntVar(&f.jobs, "j", 0, "how many groups to build and run at once (default: number of cores)")
	flag.BoolVar(&f.showVersion, "version", false, "print the version and exit")

	flag.Usage = usage
	flag.Parse()

	if f.showVersion {
		fmt.Printf("42tester %s (%s/%s)\n", version, runtime.GOOS, runtime.GOARCH)
		return
	}

	dir := "."
	if flag.NArg() > 0 {
		dir = flag.Arg(0)
	}
	abs, err := filepath.Abs(dir)
	if err != nil {
		fail("%v", err)
	}
	if st, err := os.Stat(abs); err != nil || !st.IsDir() {
		fail("%s is not a directory", dir)
	}

	targets, err := resolve(abs, f.project)
	if err != nil {
		fail("%v", err)
	}

	failed := false
	for i, t := range targets {
		if i > 0 {
			fmt.Println(strings.Repeat("─", 60))
		}
		if !runOne(t, f) {
			failed = true
		}
	}
	if failed {
		os.Exit(1)
	}
}

// target is one submission to grade.
type target struct {
	project *spec.Project
	dir     string
	// why explains the detection, printed so a wrong guess is obvious.
	why string
}

// resolve decides what to test: either what the user forced with -p, or
// whatever the directory looks like.
func resolve(dir, forced string) ([]target, error) {
	if forced != "" {
		p, err := spec.Lookup(forced)
		if err != nil {
			return nil, err
		}
		return []target{{project: p, dir: dir}}, nil
	}

	matches := detect.Scan(dir)
	if len(matches) == 0 {
		return nil, fmt.Errorf(
			"no 42 project found in %s\n"+
				"expected one of: libft.h, ft_printf.c, get_next_line.c\n"+
				"force it with -p libft | -p printf | -p gnl", dir)
	}

	// Several matches in the same directory means the guess is ambiguous;
	// several matches in different directories means the user pointed at a
	// folder holding all their projects, which we happily test in one go.
	sameDir := true
	for _, m := range matches[1:] {
		if m.Dir != matches[0].Dir {
			sameDir = false
			break
		}
	}
	if sameDir && len(matches) > 1 {
		var names []string
		for _, m := range matches {
			names = append(names, m.Project.ID)
		}
		return nil, fmt.Errorf("%s looks like several projects at once (%s); pick one with -p",
			dir, strings.Join(names, ", "))
	}

	var out []target
	for _, m := range matches {
		out = append(out, target{
			project: m.Project,
			dir:     m.Dir,
			why:     strings.Join(m.Reasons, ", "),
		})
	}
	return out, nil
}

// runOne grades a single submission and reports whether it passed.
func runOne(t target, f flags) bool {
	rel := relativeTo(t.dir)
	if t.why != "" {
		fmt.Printf("detected %s in %s (%s)\n", t.project.ID, rel, t.why)
	} else {
		fmt.Printf("testing %s in %s\n", t.project.ID, rel)
	}

	opts := engine.Options{
		Extra:       f.extra,
		StrictAlloc: f.strictAlloc,
		Leaks:       f.leaks,
		Sanitize:    f.sanitize,
		Timeout:     f.timeout,
		Jobs:        f.jobs,
		RunAll:      f.runAll,
		NoBonus:     f.noBonus,
		Progress: func(done, n int, group string) {
			fmt.Printf("\r\033[K  %d/%d  %s", done, n, group)
		},
	}

	rep, notes, err := engine.Run(t.project, t.dir, opts)
	fmt.Print("\r\033[K")
	if err != nil {
		fmt.Fprintf(os.Stderr, "error: %v\n", err)
		return false
	}

	for _, n := range notes {
		fmt.Printf("  note: %s\n", n)
	}

	report.Terminal(os.Stdout, rep, report.TerminalOptions{
		Color:   !f.noColor && os.Getenv("NO_COLOR") == "",
		Verbose: f.verbose,
	})

	writeArtifacts(rep, t, f)
	return rep.Success()
}

// writeArtifacts saves the markdown and JSON reports next to the submission.
func writeArtifacts(rep *result.Report, t target, f flags) {
	if !f.noMarkdown && f.markdown != "" {
		path := f.markdown
		if !filepath.IsAbs(path) {
			path = filepath.Join(t.dir, path)
		}
		md := report.Markdown(rep, report.MarkdownOptions{Details: true})
		if err := os.WriteFile(path, []byte(md), 0o644); err != nil {
			fmt.Fprintf(os.Stderr, "  could not write %s: %v\n", path, err)
		} else {
			fmt.Printf("  report: %s\n\n", relativeTo(path))
		}
	}
	if f.jsonOut != "" {
		path := f.jsonOut
		if !filepath.IsAbs(path) {
			path = filepath.Join(t.dir, path)
		}
		data, err := report.JSON(rep)
		if err == nil {
			err = os.WriteFile(path, append(data, '\n'), 0o644)
		}
		if err != nil {
			fmt.Fprintf(os.Stderr, "  could not write %s: %v\n", path, err)
		}
	}
}

// relativeTo shortens a path against the working directory when that helps.
func relativeTo(path string) string {
	wd, err := os.Getwd()
	if err != nil {
		return path
	}
	rel, err := filepath.Rel(wd, path)
	if err != nil || strings.HasPrefix(rel, "..") {
		return path
	}
	if rel == "." {
		return "."
	}
	return rel
}

func usage() {
	fmt.Fprintf(os.Stderr, `42tester %s — moulinette-style tester for libft, ft_printf and get_next_line

Usage:
  42tester [flags] [directory]

The directory defaults to the current one and the project is detected from
its contents. Point it at a folder holding several projects and all of them
are tested.

Examples:
  42tester .                 test whatever is here
  42tester ~/libft -v        show every test, not just failures
  42tester . --leaks         also look for memory leaks
  42tester . --extra         add the tests the moulinette does not run

Flags:
`, version)
	flag.PrintDefaults()
}

func fail(format string, args ...any) {
	fmt.Fprintf(os.Stderr, "42tester: "+format+"\n", args...)
	os.Exit(2)
}
