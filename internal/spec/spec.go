// Package spec describes what each 42 project is made of: the files the
// moulinette expects, the functions it allows, and the exact list of test
// groups it runs.
//
// The group names and case counts are transcribed from real moulinette
// reports (see docs/moulinette/). Keeping them identical is the whole point:
// a student comparing our output to the real one should not see a difference
// in structure, only in which lines are green.
package spec

import "fmt"

// Group is one "#### <name>" section of the report.
type Group struct {
	// Name is the section heading, e.g. "ft_split" or "buffer_size_1000".
	Name string
	// Prefix builds the individual test labels: "<Prefix>_test__#<n>".
	// It usually equals Name, but the moulinette diverges for a few groups
	// (group "simple_test" emits "simple_test__#1", group "buffer_size_0"
	// emits "buffer_size_test__#1"), so it is stored explicitly.
	Prefix string
	// Cases is how many tests the real moulinette reports for this group.
	Cases int
	// Source is the harness file, relative to the embedded harness root.
	Source string
	// CFlags are extra compiler flags for this group only (get_next_line
	// compiles the same sources once per BUFFER_SIZE).
	CFlags []string
	// NoBufferSize compiles get_next_line without any -D BUFFER_SIZE at all.
	// The subject requires the project to build both ways, so the header has
	// to carry a default of its own.
	NoBufferSize bool
}

// TestName returns the label of case n (1-based) as the moulinette writes it.
func (g Group) TestName(n int) string {
	return fmt.Sprintf("%s_test__#%d", g.Prefix, n)
}

// Project is one assignment.
type Project struct {
	// ID is what the user types: "libft", "printf", "gnl".
	ID string
	// Aliases are alternative spellings accepted on the command line.
	Aliases []string
	// Assignment is the moulinette's name for the project, reproduced
	// verbatim in the report header and footer.
	Assignment string
	// ExpectedFiles must all be present in the submission.
	ExpectedFiles []string
	// AllowedFuncs is the set of external symbols the subject permits.
	AllowedFuncs []string
	// MandatoryFuncs must each be declared in the header and defined in the
	// compiled code. The subject gives their exact names, so a missing
	// prototype or an empty source file is a failure and not a detail.
	MandatoryFuncs []string
	// Library is the archive `make` is expected to produce, if any.
	Library string
	// Sources lists source files linked directly instead of via a library
	// (get_next_line has no Makefile in the mandatory part).
	Sources []string
	// Headers are include files needed to compile the harness.
	Headers []string
	// Groups are the sections of the real moulinette report, in its order.
	// Rendering these alone has to reproduce that report exactly, which is
	// what internal/report/markdown_test.go checks.
	Groups []Group
	// RequiredGroups cover things the subject demands that the report we
	// have shows no group for. They run by default: a tester that skips a
	// mandatory conversion is broken, whatever the report looks like.
	RequiredGroups []Group
	// ExtraGroups are genuinely optional: behaviour the subject calls
	// undefined, or that only an evaluator would try. --extra turns them on.
	ExtraGroups []Group
}

// TotalCases counts every test case across the given groups.
func TotalCases(groups []Group) int {
	n := 0
	for _, g := range groups {
		n += g.Cases
	}
	return n
}

// All returns every supported project, in curriculum order.
func All() []*Project {
	return []*Project{Libft(), Printf(), GetNextLine()}
}

// Lookup resolves a user-supplied project name.
func Lookup(name string) (*Project, error) {
	for _, p := range All() {
		if p.ID == name {
			return p, nil
		}
		for _, a := range p.Aliases {
			if a == name {
				return p, nil
			}
		}
	}
	return nil, fmt.Errorf("unknown project %q (known: libft, printf, gnl)", name)
}

// FunctionNames collects the names of the groups, which for libft are the
// functions themselves.
func FunctionNames(groups []Group) []string {
	names := make([]string, 0, len(groups))
	for _, g := range groups {
		names = append(names, g.Name)
	}
	return names
}

// simple is the common case where the report prefix equals the group name.
func simple(name string, cases int, source string) Group {
	return Group{Name: name, Prefix: name, Cases: cases, Source: source}
}
