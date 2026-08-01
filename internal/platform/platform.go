// Package platform locates the C toolchain and decides which extras (address
// sanitizer, leak detection) are usable on the machine we run on.
package platform

import (
	"context"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
	"time"
)

// Toolchain is the resolved set of external tools the tester needs.
type Toolchain struct {
	// CC is the C compiler used to build the harness and link the student's
	// code. The student's own Makefile picks its own compiler.
	CC string
	// NM lists symbols in an archive; empty when unavailable, in which case
	// the allowed-functions check degrades to a source scan.
	NM string
	// Make runs the student's Makefile; empty when unavailable.
	Make string
	// ExeSuffix is ".exe" on Windows.
	ExeSuffix string
	// Sanitizers holds -fsanitize flags that actually work here.
	Sanitizers []string
	// LeakTool is "lsan" (bundled with ASan), "leaks" (macOS), or "" when
	// leak detection is not available.
	LeakTool string
	// Notes are warnings worth showing the student once.
	Notes []string
}

// Detect resolves the toolchain, probing the compiler for the features we
// would like to use rather than guessing from runtime.GOOS alone.
func Detect(wantSanitizers bool) (*Toolchain, error) {
	t := &Toolchain{ExeSuffix: exeSuffix()}

	t.CC = firstInPath(envOr("CC", ""), "cc", "gcc", "clang")
	if t.CC == "" {
		return nil, fmt.Errorf("no C compiler found: install gcc or clang, or set CC")
	}
	t.NM = firstInPath(envOr("NM", ""), "nm", "llvm-nm", "gcc-nm")
	t.Make = firstInPath(envOr("MAKE", ""), "make", "gmake", "mingw32-make")

	if t.NM == "" {
		t.Notes = append(t.Notes, "nm not found: the allowed-functions check falls back to scanning sources, which is less reliable")
	}
	if t.Make == "" {
		t.Notes = append(t.Notes, "make not found: projects with a Makefile cannot be built")
	}

	if wantSanitizers {
		t.detectSanitizers()
	}
	return t, nil
}

// detectSanitizers compiles a throwaway program to find out what the local
// compiler really supports. mingw, for instance, advertises the flag but
// fails to link.
func (t *Toolchain) detectSanitizers() {
	if t.probeCompile("-fsanitize=address,undefined", "-fno-omit-frame-pointer") {
		t.Sanitizers = []string{"-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-g"}
	} else if t.probeCompile("-fsanitize=undefined") {
		t.Sanitizers = []string{"-fsanitize=undefined", "-g"}
		t.Notes = append(t.Notes, "address sanitizer unavailable here: out-of-bounds writes may go unnoticed")
	} else {
		t.Notes = append(t.Notes, "no sanitizer available: memory errors will only show up as crashes")
		return
	}

	switch {
	case runtime.GOOS == "linux" && len(t.Sanitizers) > 0 && strings.Contains(t.Sanitizers[0], "address"):
		// LeakSanitizer ships with ASan on Linux and is on by default.
		t.LeakTool = "lsan"
	case runtime.GOOS == "darwin":
		// LeakSanitizer is not available on Apple silicon; the `leaks`
		// command that ships with Xcode is the working alternative.
		if firstInPath("", "leaks") != "" {
			t.LeakTool = "leaks"
		} else {
			t.Notes = append(t.Notes, "`leaks` not found: install the Xcode command line tools for leak detection")
		}
	}
}

// probeCompile reports whether the compiler can build and link a trivial
// program with the given flags.
func (t *Toolchain) probeCompile(flags ...string) bool {
	dir, err := os.MkdirTemp("", "42tester-probe-")
	if err != nil {
		return false
	}
	defer os.RemoveAll(dir)

	src := filepath.Join(dir, "probe.c")
	if err := os.WriteFile(src, []byte("int main(void){return 0;}\n"), 0o644); err != nil {
		return false
	}
	out := filepath.Join(dir, "probe"+t.ExeSuffix)

	ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
	defer cancel()

	args := append(append([]string{}, flags...), "-o", out, src)
	return exec.CommandContext(ctx, t.CC, args...).Run() == nil
}

// SupportsPOSIX reports whether the compiler can build code using read(),
// write() and ssize_t. On Windows this is what tells us whether msys2/mingw
// is good enough or whether the student should run under WSL.
func (t *Toolchain) SupportsPOSIX() bool {
	dir, err := os.MkdirTemp("", "42tester-posix-")
	if err != nil {
		return false
	}
	defer os.RemoveAll(dir)

	const src = `#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
int main(void){ char b[4]; ssize_t n = read(0, b, 1); (void)n; return 0; }
`
	f := filepath.Join(dir, "posix.c")
	if err := os.WriteFile(f, []byte(src), 0o644); err != nil {
		return false
	}
	out := filepath.Join(dir, "posix"+t.ExeSuffix)

	ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
	defer cancel()
	return exec.CommandContext(ctx, t.CC, "-o", out, f).Run() == nil
}

func exeSuffix() string {
	if runtime.GOOS == "windows" {
		return ".exe"
	}
	return ""
}

func envOr(key, def string) string {
	if v := os.Getenv(key); v != "" {
		return v
	}
	return def
}

// firstInPath returns the path of the first named binary that exists.
func firstInPath(names ...string) string {
	for _, n := range names {
		if n == "" {
			continue
		}
		if p, err := exec.LookPath(n); err == nil {
			return p
		}
	}
	return ""
}
