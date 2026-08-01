package report

import (
	"fmt"
	"io"
	"strings"

	"tester42/internal/result"
)

// TerminalOptions tunes the human-readable output.
type TerminalOptions struct {
	// Color enables ANSI escapes.
	Color bool
	// Verbose prints every case; otherwise only failures are listed.
	Verbose bool
}

const (
	reset  = "\033[0m"
	green  = "\033[32m"
	red    = "\033[31m"
	yellow = "\033[33m"
	dim    = "\033[2m"
	bold   = "\033[1m"
)

// Terminal writes a compact summary meant to be read while fixing code.
func Terminal(w io.Writer, r *result.Report, opts TerminalOptions) {
	c := func(color, s string) string {
		if !opts.Color {
			return s
		}
		return color + s + reset
	}

	fmt.Fprintf(w, "\n%s\n", c(bold, r.Project.Assignment))

	for _, chk := range r.Prerequisites {
		fmt.Fprintf(w, "  %s %s\n", mark(chk.Status, opts.Color), chk.Name)
		if chk.Detail != "" && !chk.Status.Passed() {
			fmt.Fprint(w, indent(chk.Detail, "      "))
		}
	}

	if r.Aborted {
		fmt.Fprintf(w, "\n%s %s\n", c(red, "aborted:"), r.AbortReason)
		return
	}

	fmt.Fprintln(w)
	for _, g := range r.Groups {
		valid, total := g.Valid(), len(g.Cases)
		if g.Compilation == result.Skipped && total == 0 {
			continue
		}

		if skipped(g) {
			if opts.Verbose {
				fmt.Fprintf(w, "  %s %-22s %s\n", mark(result.Skipped, opts.Color), g.Spec.Name,
					c(dim, "not turned in"))
			}
			continue
		}

		line := fmt.Sprintf("  %s %-22s %d/%d", mark(groupStatus(g), opts.Color), g.Spec.Name, valid, total)
		switch {
		case g.Compilation != result.OK && g.Compilation != result.Skipped:
			fmt.Fprintf(w, "  %s %-22s %s\n", mark(g.Compilation, opts.Color), g.Spec.Name, c(red, "compilation failed"))
			fmt.Fprint(w, indent(trimLog(g.CompileLog), "      "))
			continue
		case valid == total:
			if opts.Verbose {
				fmt.Fprintln(w, line)
			}
		default:
			fmt.Fprintln(w, line)
		}

		for _, cs := range g.Cases {
			if cs.Status.Passed() && !opts.Verbose {
				continue
			}
			fmt.Fprintf(w, "      %s %s\n", mark(cs.Status, opts.Color), cs.Name)
			if e := cs.Explain(); e != "" {
				fmt.Fprint(w, indent(e, "        "))
			}
		}
	}

	valid, total := r.Totals()
	fmt.Fprintln(w)
	if r.Success() {
		fmt.Fprintf(w, "  %s  %d/%d\n\n", c(green+bold, "PASS"), valid, total)
		return
	}
	fmt.Fprintf(w, "  %s  %d/%d", c(red+bold, "FAIL"), valid, total)
	// Passing every test while failing a prerequisite is the confusing case:
	// say plainly that the moulinette stops there.
	if !r.PrerequisitesOK() && valid == total {
		fmt.Fprintf(w, "  %s", c(red, "— every test passes, but a prerequisite failed, which is a 0"))
	}
	fmt.Fprint(w, "\n\n")
}

func groupStatus(g *result.Group) result.Status {
	if g.Compilation != result.OK && g.Compilation != result.Skipped {
		return g.Compilation
	}
	// A group nobody ran is skipped, not passing. Reporting it as a tick
	// next to "0/5" reads as five failures that somehow succeeded.
	if skipped(g) {
		return result.Skipped
	}
	worst := result.OK
	for _, c := range g.Cases {
		if c.Status != result.OK && c.Status != result.Skipped {
			worst = c.Status
		}
	}
	return worst
}

// skipped reports whether no case of the group was actually run.
func skipped(g *result.Group) bool {
	for _, c := range g.Cases {
		if c.Status != result.Skipped {
			return false
		}
	}
	return true
}

func mark(s result.Status, color bool) string {
	sym, col := "✓", green
	switch s {
	case result.KO:
		sym, col = "✗", red
	case result.Crash:
		sym, col = "!", red
	case result.Timeout:
		sym, col = "⏱", yellow
	case result.Skipped:
		sym, col = "-", dim
	}
	if !color {
		return sym
	}
	return col + sym + reset
}

func indent(s, prefix string) string {
	s = strings.TrimRight(s, "\n")
	if s == "" {
		return ""
	}
	lines := strings.Split(s, "\n")
	for i, l := range lines {
		lines[i] = prefix + l
	}
	return strings.Join(lines, "\n") + "\n"
}

// trimLog keeps compiler output readable by showing only the first errors.
func trimLog(log string) string {
	lines := strings.Split(strings.TrimRight(log, "\n"), "\n")
	if len(lines) > 12 {
		lines = append(lines[:12], fmt.Sprintf("... (%d more lines)", len(lines)-12))
	}
	return strings.Join(lines, "\n")
}
