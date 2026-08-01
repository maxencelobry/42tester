// Package result holds the outcome model shared by the runner and the
// report writers.
package result

import "tester42/internal/spec"

// Status is the outcome of a single check, compilation or test case.
type Status int

const (
	// OK is a pass.
	OK Status = iota
	// KO is a wrong result: the function returned or wrote something we did
	// not expect.
	KO
	// Crash means the process died on a signal (segfault, abort, bus error).
	Crash
	// Timeout means the case did not finish in time, usually an infinite loop.
	Timeout
	// Skipped means the case was not run, e.g. bonus not turned in.
	Skipped
)

// Passed reports whether the status counts towards "Valid tests".
func (s Status) Passed() bool { return s == OK }

// String renders the status the way the moulinette report does.
func (s Status) String() string {
	switch s {
	case OK:
		return "OK 🔥"
	case KO:
		return "KO ❌"
	case Crash:
		return "CRASH 💥"
	case Timeout:
		return "TIMEOUT ⏱️"
	case Skipped:
		return "SKIPPED ⏭️"
	}
	return "?"
}

// Case is one "**<name>_test__#<n>:**" line.
type Case struct {
	Number int
	Name   string
	Status Status
	// Detail explains the failure. For a comparison it is the call that was
	// made; otherwise it is a full sentence. Empty when the case passed.
	Detail string
	// Input is what the case was fed, when the call alone does not say it:
	// the contents of a file, the string a helper was built from.
	Input string
	// Expected and Got hold the two sides of a comparison, so the report can
	// lay them out one under the other. Both are empty for failures that are
	// not comparisons, such as a crash.
	Expected string
	Got      string
}

// Comparison reports whether the failure has an expected/got pair to show.
func (c Case) Comparison() bool {
	return c.Expected != "" || c.Got != ""
}

// Explain renders the failure as the moulinette-style block: the call, then
// the two values one under the other. Returns "" for a passing case.
func (c Case) Explain() string {
	if c.Status.Passed() || c.Status == Skipped {
		return ""
	}
	if !c.Comparison() {
		if c.Input == "" {
			return c.Detail
		}
		return c.Detail + "\n  input:    " + c.Input
	}
	out := c.Detail
	if c.Input != "" {
		out += "\n  input:    " + c.Input
	}
	return out + "\n  expected: " + c.Expected + "\n  got:      " + c.Got
}

// Group is one "#### <name>" section.
type Group struct {
	Spec spec.Group
	// Compilation is the status of building the harness for this group.
	Compilation Status
	// CompileLog is the compiler's stderr when Compilation failed.
	CompileLog string
	Cases      []Case
}

// Valid counts the passing cases, which is what "**Valid tests:**" shows.
func (g *Group) Valid() int {
	n := 0
	for _, c := range g.Cases {
		if c.Status.Passed() {
			n++
		}
	}
	return n
}

// Check is a prerequisite line such as "Expected files".
type Check struct {
	Name   string
	Status Status
	Detail string
}

// Report is everything needed to render the moulinette output.
type Report struct {
	Project *spec.Project
	// Prerequisites are rendered before the test results. A failing
	// prerequisite stops the run, exactly like the real moulinette.
	Prerequisites []Check
	Groups        []*Group
	// Aborted is set when a prerequisite or the library build failed, in
	// which case no group was run.
	Aborted bool
	// AbortReason explains the abort in the terminal output.
	AbortReason string
}

// PrerequisitesOK reports whether every prerequisite passed.
func (r *Report) PrerequisitesOK() bool {
	for _, c := range r.Prerequisites {
		if !c.Status.Passed() {
			return false
		}
	}
	return true
}

// Totals returns the number of passing cases and the number run.
func (r *Report) Totals() (valid, total int) {
	for _, g := range r.Groups {
		for _, c := range g.Cases {
			if c.Status == Skipped {
				continue
			}
			total++
			if c.Status.Passed() {
				valid++
			}
		}
	}
	return valid, total
}

// Success reports whether the whole assignment passed.
func (r *Report) Success() bool {
	if r.Aborted || !r.PrerequisitesOK() {
		return false
	}
	for _, g := range r.Groups {
		if g.Compilation != OK && g.Compilation != Skipped {
			return false
		}
		for _, c := range g.Cases {
			if c.Status != OK && c.Status != Skipped {
				return false
			}
		}
	}
	return true
}
