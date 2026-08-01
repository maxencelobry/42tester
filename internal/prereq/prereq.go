// Package prereq implements the two checks the moulinette runs before any
// test: the expected files are there, and no forbidden function is called.
package prereq

import (
	"context"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
	"runtime"
	"sort"
	"strings"
	"time"

	"tester42/internal/result"
	"tester42/internal/spec"
)

// checkName is the label the moulinette report uses for this line. The report
// has exactly two prerequisites, so everything about the submission being
// complete is folded in here rather than added as a third.
const checkName = "Expected files"

// ExpectedFiles reports whether every mandatory file is present and every
// function the subject names is declared in the header.
//
// The subject gives the prototypes, so a function that is missing from the
// header is not a detail to discover later through a compiler error: the
// submission does not match what was asked.
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
			Name:   checkName,
			Status: result.KO,
			Detail: "missing file(s): " + strings.Join(missing, ", "),
		}
	}

	return result.Check{Name: checkName, Status: result.OK}
}

// MissingFunctions maps each expected function that is not usable yet to the
// reason why, ready to be shown as that function's own failure.
//
// It deliberately does not abort the run. Students write these one at a time,
// so a function that does not exist yet belongs at its own place in the
// sequence, after everything before it has been graded, rather than as a wall
// of names before any test has run.
func MissingFunctions(dir, nm string, objects []string, p *spec.Project) map[string]string {
	out := map[string]string{}
	if len(p.MandatoryFuncs) == 0 {
		return out
	}

	var headers string
	for _, h := range p.Headers {
		if data, err := os.ReadFile(filepath.Join(dir, h)); err == nil {
			headers += string(data)
		}
	}

	var defined map[string]bool
	if nm != "" && len(objects) > 0 {
		if d, _, err := symbols(nm, objects); err == nil {
			defined = d
		}
	}

	for _, name := range p.MandatoryFuncs {
		// The name has to be followed by an opening parenthesis, so a
		// mention in a comment or a differently spelled function does not
		// count as a declaration.
		declared := regexp.MustCompile(`\b` + regexp.QuoteMeta(name) + `\s*\(`).MatchString(headers)
		switch {
		case !declared:
			out[name] = fmt.Sprintf("%s is not declared in %s.\nThe subject gives the prototype; add it to the header.",
				name, strings.Join(p.Headers, " or "))
		case defined != nil && !defined[name]:
			out[name] = fmt.Sprintf("%s is declared but never defined: nothing in %s provides it.\nThe source file is empty, or it is not built into the library.",
				name, p.Library)
		}
	}
	return out
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
