// Package build turns a student submission plus the embedded harness into
// one executable per test group.
package build

import (
	"context"
	"fmt"
	"io"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"time"

	"tester42/internal/harness"
	"tester42/internal/platform"
	"tester42/internal/spec"
)

// Builder compiles a submission in an isolated working copy.
//
// The student's own directory is never written to: `make` can generate object
// files, and running it in place would dirty their repository.
type Builder struct {
	Tool    *platform.Toolchain
	Project *spec.Project

	// Root is the temporary directory holding everything we produce.
	Root string
	// Src is the working copy of the submission.
	Src string
	// Harness is the extracted C test suite.
	Harness string
	// Bin is where group executables land.
	Bin string
	// Run is the working directory of the test binaries. Tests create scratch
	// files there, and a crashing test leaves them behind, so it is kept well
	// away from anything the student owns.
	Run string

	// Sanitize adds the detected sanitizer flags to the harness build.
	Sanitize bool
	// Defines are extra -D flags applied to every group, used to switch on
	// optional harness features such as leak tracking.
	Defines []string
	// Timeout bounds each compiler and `make` invocation.
	Timeout time.Duration
	// LinkFlags are appended last, e.g. -ldl for the malloc interposer.
	LinkFlags []string
}

// New prepares a working copy of submission and extracts the harness.
func New(tool *platform.Toolchain, project *spec.Project, submission string, sanitize bool) (*Builder, error) {
	root, err := os.MkdirTemp("", "42tester-")
	if err != nil {
		return nil, fmt.Errorf("create work dir: %w", err)
	}

	b := &Builder{
		Tool:     tool,
		Project:  project,
		Root:     root,
		Src:      filepath.Join(root, "src"),
		Harness:  filepath.Join(root, "harness"),
		Bin:      filepath.Join(root, "bin"),
		Run:      filepath.Join(root, "run"),
		Sanitize: sanitize,
		Timeout:  2 * time.Minute,
	}

	for _, d := range []string{b.Src, b.Harness, b.Bin, b.Run} {
		if err := os.MkdirAll(d, 0o755); err != nil {
			b.Cleanup()
			return nil, err
		}
	}
	if err := copyTree(submission, b.Src); err != nil {
		b.Cleanup()
		return nil, fmt.Errorf("copy submission: %w", err)
	}
	if err := harness.Extract(b.Harness); err != nil {
		b.Cleanup()
		return nil, fmt.Errorf("extract harness: %w", err)
	}
	return b, nil
}

// Cleanup removes the working directory.
func (b *Builder) Cleanup() {
	if b.Root != "" {
		os.RemoveAll(b.Root)
	}
}

// LibraryPath is where the student's archive is expected after `make`.
func (b *Builder) LibraryPath() string {
	if b.Project.Library == "" {
		return ""
	}
	return filepath.Join(b.Src, b.Project.Library)
}

// Make runs the student's Makefile with the given targets and returns its
// combined output.
func (b *Builder) Make(targets ...string) (string, error) {
	if b.Tool.Make == "" {
		return "", fmt.Errorf("make is not installed")
	}
	ctx, cancel := context.WithTimeout(context.Background(), b.Timeout)
	defer cancel()

	cmd := exec.CommandContext(ctx, b.Tool.Make, targets...)
	cmd.Dir = b.Src
	out, err := cmd.CombinedOutput()
	if ctx.Err() == context.DeadlineExceeded {
		return string(out), fmt.Errorf("make timed out after %s", b.Timeout)
	}
	return string(out), err
}

// BuildLibrary runs `make` and checks the expected archive was produced.
// Projects without a library (get_next_line) are a no-op.
func (b *Builder) BuildLibrary() (string, error) {
	if b.Project.Library == "" {
		return "", nil
	}
	out, err := b.Make()
	if err != nil {
		return out, fmt.Errorf("make failed: %w", err)
	}
	if _, err := os.Stat(b.LibraryPath()); err != nil {
		return out, fmt.Errorf("make did not produce %s", b.Project.Library)
	}
	return out, nil
}

// BuildBonus runs `make bonus` when the target exists. A missing target is
// not an error: it just means the bonus was not turned in.
func (b *Builder) BuildBonus() bool {
	if b.Project.Library == "" || b.Tool.Make == "" {
		return false
	}
	if !b.HasMakeTarget("bonus") {
		return false
	}
	_, err := b.Make("bonus")
	return err == nil
}

// HasMakeTarget looks for a rule in the Makefile. Parsing the file is cruder
// than asking make, but `make -n bonus` on a Makefile without that rule
// prints an error we would have to parse anyway.
func (b *Builder) HasMakeTarget(target string) bool {
	for _, name := range []string{"Makefile", "makefile", "GNUmakefile"} {
		data, err := os.ReadFile(filepath.Join(b.Src, name))
		if err != nil {
			continue
		}
		for _, line := range strings.Split(string(data), "\n") {
			line = strings.TrimSpace(line)
			if strings.HasPrefix(line, target+":") || strings.HasPrefix(line, target+" :") {
				return true
			}
		}
	}
	return false
}

// Relinks reports whether a second `make` rebuilds anything, which the
// moulinette penalises.
func (b *Builder) Relinks() (bool, string) {
	out, err := b.Make()
	if err != nil {
		return false, out
	}
	lower := strings.ToLower(out)
	quiet := strings.Contains(lower, "nothing to be done") ||
		strings.Contains(lower, "is up to date") ||
		strings.TrimSpace(out) == ""
	return !quiet, out
}

// CompileGroup builds the executable for one test group and returns its path
// along with the compiler output.
func (b *Builder) CompileGroup(g spec.Group) (string, string, error) {
	exe := filepath.Join(b.Bin, sanitizeName(g.Name)+b.Tool.ExeSuffix)

	args := []string{"-Wall", "-Wextra", "-std=gnu99", "-D_GNU_SOURCE"}
	if b.Sanitize {
		args = append(args, b.Tool.Sanitizers...)
	}
	args = append(args, b.Defines...)
	args = append(args, g.CFlags...)
	// get_next_line sources need a BUFFER_SIZE even in groups that do not
	// vary it; the subject's default is unspecified, 42 is the usual value.
	if b.Project.ID == "gnl" && !hasDefine(g.CFlags, "BUFFER_SIZE") {
		args = append(args, "-DBUFFER_SIZE=42")
	}
	args = append(args,
		"-I", b.Src,
		"-I", filepath.Join(b.Harness, "common"),
		"-o", exe,
		filepath.Join(b.Harness, filepath.FromSlash(g.Source)),
		filepath.Join(b.Harness, "common", "tester.c"),
	)
	// Sources are compiled straight in when there is no archive to link.
	for _, s := range b.Project.Sources {
		args = append(args, filepath.Join(b.Src, s))
	}
	if lib := b.LibraryPath(); lib != "" {
		args = append(args, lib)
	}
	args = append(args, b.LinkFlags...)

	ctx, cancel := context.WithTimeout(context.Background(), b.Timeout)
	defer cancel()

	cmd := exec.CommandContext(ctx, b.Tool.CC, args...)
	cmd.Dir = b.Src
	out, err := cmd.CombinedOutput()
	if ctx.Err() == context.DeadlineExceeded {
		return "", string(out), fmt.Errorf("compilation timed out")
	}
	if err != nil {
		return "", string(out), err
	}
	return exe, string(out), nil
}

// CompileObject builds a single source file to an object, without linking.
// It exists so nm has something to inspect for projects that ship no archive.
func (b *Builder) CompileObject(source, out string) (string, string, error) {
	args := []string{"-Wall", "-Wextra", "-std=gnu99", "-D_GNU_SOURCE"}
	if b.Project.ID == "gnl" {
		args = append(args, "-DBUFFER_SIZE=42")
	}
	args = append(args, "-I", b.Src, "-c", filepath.Join(b.Src, source), "-o", out)

	ctx, cancel := context.WithTimeout(context.Background(), b.Timeout)
	defer cancel()

	cmd := exec.CommandContext(ctx, b.Tool.CC, args...)
	cmd.Dir = b.Src
	log, err := cmd.CombinedOutput()
	return out, string(log), err
}

func hasDefine(flags []string, name string) bool {
	for _, f := range flags {
		if strings.HasPrefix(f, "-D"+name+"=") || f == "-D"+name {
			return true
		}
	}
	return false
}

// sanitizeName keeps group names usable as file names.
func sanitizeName(s string) string {
	return strings.Map(func(r rune) rune {
		switch {
		case r >= 'a' && r <= 'z', r >= 'A' && r <= 'Z', r >= '0' && r <= '9', r == '_', r == '-':
			return r
		}
		return '_'
	}, s)
}

// copyTree copies src into dst, skipping build artefacts and the git
// directory so the working copy stays small and clean.
func copyTree(src, dst string) error {
	return filepath.WalkDir(src, func(path string, d os.DirEntry, err error) error {
		if err != nil {
			return err
		}
		rel, err := filepath.Rel(src, path)
		if err != nil {
			return err
		}
		if rel == "." {
			return nil
		}
		if d.IsDir() {
			switch d.Name() {
			case ".git", "node_modules", ".vscode":
				return filepath.SkipDir
			}
			return os.MkdirAll(filepath.Join(dst, rel), 0o755)
		}
		switch filepath.Ext(rel) {
		case ".o", ".a", ".so", ".dylib":
			// Stale artefacts would let a broken Makefile look like it works.
			return nil
		}
		return copyFile(path, filepath.Join(dst, rel))
	})
}

func copyFile(src, dst string) error {
	in, err := os.Open(src)
	if err != nil {
		return err
	}
	defer in.Close()

	if err := os.MkdirAll(filepath.Dir(dst), 0o755); err != nil {
		return err
	}
	out, err := os.Create(dst)
	if err != nil {
		return err
	}
	defer out.Close()

	if _, err := io.Copy(out, in); err != nil {
		return err
	}
	return out.Close()
}
