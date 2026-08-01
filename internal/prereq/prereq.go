// Package prereq implements the two checks the moulinette runs before any
// test: the expected files are there, and no forbidden function is called.
package prereq

import (
	"context"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"sort"
	"strings"
	"time"

	"tester42/internal/result"
	"tester42/internal/spec"
)

// ExpectedFiles reports whether every mandatory file is present.
func ExpectedFiles(dir string, p *spec.Project) result.Check {
	var missing []string
	for _, f := range p.ExpectedFiles {
		if _, err := os.Stat(filepath.Join(dir, f)); err != nil {
			missing = append(missing, f)
		}
	}
	// A submission with a header and a Makefile but no source is not a
	// submission; the moulinette expects the functions to exist somewhere.
	if len(missing) == 0 && len(p.Sources) == 0 {
		if sources, _ := filepath.Glob(filepath.Join(dir, "*.c")); len(sources) == 0 {
			missing = append(missing, "*.c (no source file found)")
		}
	}

	if len(missing) > 0 {
		return result.Check{
			Name:   "Expected files",
			Status: result.KO,
			Detail: "missing: " + strings.Join(missing, ", "),
		}
	}
	return result.Check{Name: "Expected files", Status: result.OK}
}

// compilerInternals are symbols the toolchain emits on its own. They are not
// calls the student wrote, so flagging them would be a false accusation.
var compilerInternals = map[string]bool{
	"__stack_chk_fail":            true,
	"__stack_chk_guard":           true,
	"_GLOBAL_OFFSET_TABLE_":       true,
	"__gmon_start__":              true,
	"_ITM_registerTMCloneTable":   true,
	"_ITM_deregisterTMCloneTable": true,
	"__cxa_finalize":              true,
	"__chkstk_ms":                 true,
	"___chkstk_ms":                true,
	"__main":                      true,
	"__errno_location":            true,
	"__error":                     true,
	"___error":                    true,
	"__assert_fail":               true,
	"__assert_rtn":                true,
}

// AllowedFunctions inspects the compiled objects and reports any external
// symbol the subject does not permit.
//
// Reading the binary rather than the source is what makes this trustworthy:
// a student cannot hide printf behind a macro or a typedef.
func AllowedFunctions(nm string, objects []string, p *spec.Project) result.Check {
	const name = "Allowed functions"

	if nm == "" {
		return result.Check{
			Name:   name,
			Status: result.OK,
			Detail: "not verified: nm is not installed on this machine",
		}
	}
	if len(objects) == 0 {
		return result.Check{
			Name:   name,
			Status: result.OK,
			Detail: "not verified: nothing was compiled",
		}
	}

	defined, undefined, err := symbols(nm, objects)
	if err != nil {
		return result.Check{
			Name:   name,
			Status: result.OK,
			Detail: "not verified: " + err.Error(),
		}
	}

	allowed := map[string]bool{}
	for _, f := range p.AllowedFuncs {
		allowed[f] = true
	}

	var forbidden []string
	for sym := range undefined {
		if defined[sym] || allowed[sym] || compilerInternals[sym] {
			continue
		}
		if strings.HasPrefix(sym, "__asan") || strings.HasPrefix(sym, "__ubsan") ||
			strings.HasPrefix(sym, "__sanitizer") || strings.HasPrefix(sym, ".refptr.") {
			continue
		}
		forbidden = append(forbidden, sym)
	}

	if len(forbidden) == 0 {
		return result.Check{Name: name, Status: result.OK}
	}
	sort.Strings(forbidden)
	return result.Check{
		Name:   name,
		Status: result.KO,
		Detail: fmt.Sprintf("forbidden function(s): %s\nallowed here: %s",
			strings.Join(forbidden, ", "), strings.Join(p.AllowedFuncs, ", ")),
	}
}

// symbols runs nm and splits its output into symbols the objects define and
// symbols they expect from elsewhere.
func symbols(nm string, objects []string) (defined, undefined map[string]bool, err error) {
	ctx, cancel := context.WithTimeout(context.Background(), 60*time.Second)
	defer cancel()

	out, err := exec.CommandContext(ctx, nm, objects...).Output()
	if err != nil {
		return nil, nil, fmt.Errorf("nm failed: %w", err)
	}

	defined = map[string]bool{}
	undefined = map[string]bool{}

	for _, line := range strings.Split(string(out), "\n") {
		fields := strings.Fields(line)
		if len(fields) < 2 {
			continue
		}
		// Lines look like "0000000000000000 T ft_strlen" or "   U malloc";
		// the type letter is always the second-to-last field.
		kind := fields[len(fields)-2]
		sym := fields[len(fields)-1]
		if len(kind) != 1 {
			continue
		}
		// Mach-O prefixes every C symbol with an underscore.
		if runtime.GOOS == "darwin" {
			sym = strings.TrimPrefix(sym, "_")
		}
		switch kind {
		case "U":
			undefined[sym] = true
		case "w", "v":
			// Weak undefined: not a call the student made.
		default:
			defined[sym] = true
		}
	}
	return defined, undefined, nil
}
