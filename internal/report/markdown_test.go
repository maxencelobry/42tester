package report

import (
	"os"
	"path/filepath"
	"strings"
	"testing"

	"tester42/internal/result"
	"tester42/internal/spec"
)

// TestMarkdownMatchesRealReports is the guard that keeps this tool honest: a
// submission that passes everything must produce a file byte-identical to the
// real moulinette report kept in docs/moulinette. Any change to the renderer
// that breaks that is a regression, however tidy it looks.
func TestMarkdownMatchesRealReports(t *testing.T) {
	cases := []struct {
		project *spec.Project
		golden  string
	}{
		{spec.Libft(), "libft.md"},
		{spec.Printf(), "printf.md"},
		{spec.GetNextLine(), "get_next_line.md"},
	}

	for _, tc := range cases {
		t.Run(tc.project.ID, func(t *testing.T) {
			want, err := os.ReadFile(filepath.Join("..", "..", "docs", "moulinette", tc.golden))
			if err != nil {
				t.Fatalf("read golden report: %v", err)
			}

			got := Markdown(allPassing(tc.project), MarkdownOptions{Details: true})
			if got != normalize(string(want)) {
				t.Errorf("report does not match %s\n%s", tc.golden, firstDiff(normalize(string(want)), got))
			}
		})
	}
}

// TestTotalsMatchRealReports checks the case counts we transcribed are still
// the ones the real reports show.
func TestTotalsMatchRealReports(t *testing.T) {
	// Summed from the "**Valid tests:**" lines of docs/moulinette/*.md.
	want := map[string]int{"libft": 222, "printf": 63, "gnl": 44}

	for _, p := range spec.All() {
		if got := spec.TotalCases(p.Groups); got != want[p.ID] {
			t.Errorf("%s: %d test cases, expected %d", p.ID, got, want[p.ID])
		}
	}
}

// allPassing builds the report of a flawless submission.
func allPassing(p *spec.Project) *result.Report {
	rep := &result.Report{
		Project: p,
		Prerequisites: []result.Check{
			{Name: "Expected files", Status: result.OK},
			{Name: "Allowed functions", Status: result.OK},
		},
	}
	for _, g := range p.Groups {
		rg := &result.Group{Spec: g, Compilation: result.OK}
		// A stale compile log must not leak into a passing report.
		rg.CompileLog = "warning: something the compiler grumbled about"
		for i := 1; i <= g.Cases; i++ {
			rg.Cases = append(rg.Cases, result.Case{
				Number: i,
				Name:   g.TestName(i),
				Status: result.OK,
			})
		}
		rep.Groups = append(rep.Groups, rg)
	}
	return rep
}

// normalize strips carriage returns so the comparison survives a checkout
// with Windows line endings.
func normalize(s string) string {
	return strings.ReplaceAll(s, "\r\n", "\n")
}

// firstDiff points at the first line that differs, which is far easier to
// read than a full dump of two thousand lines.
func firstDiff(want, got string) string {
	wl := strings.Split(want, "\n")
	gl := strings.Split(got, "\n")
	for i := 0; i < len(wl) && i < len(gl); i++ {
		if wl[i] != gl[i] {
			return "line " + itoa(i+1) + ":\n  want: " + quote(wl[i]) + "\n  got:  " + quote(gl[i])
		}
	}
	return "line counts differ: want " + itoa(len(wl)) + ", got " + itoa(len(gl))
}

func quote(s string) string { return "\"" + s + "\"" }

func itoa(n int) string {
	if n == 0 {
		return "0"
	}
	var b [20]byte
	i := len(b)
	for n > 0 {
		i--
		b[i] = byte('0' + n%10)
		n /= 10
	}
	return string(b[i:])
}
