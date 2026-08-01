// Package report renders a result.Report in the formats students care about:
// a byte-for-byte copy of the moulinette markdown, a readable terminal
// summary, and machine-readable JSON.
package report

import (
	"fmt"
	"strings"

	"tester42/internal/result"
)

// disclaimer is copied verbatim from the real reports.
const disclaimer = "⚠️ Disclaimer: some tests are generated randomly so may be different from one attempt to another."

// MarkdownOptions tunes the markdown output.
type MarkdownOptions struct {
	// Details appends an explanation block under each failing test. The real
	// moulinette reports we have are all-green, so its failure layout is
	// unknown; keep this off when diffing our output against a real report.
	Details bool
}

// Markdown renders the report exactly like docs/moulinette/*.md.
//
// The blank-line counts here are not cosmetic: they were transcribed from the
// real reports so a diff against one comes out empty. Do not "tidy" them.
func Markdown(r *result.Report, opts MarkdownOptions) string {
	var b strings.Builder

	prereqMark := "✅"
	if !r.PrerequisitesOK() {
		prereqMark = "❌"
	}

	fmt.Fprintf(&b, "# Moulinette report for %s\n", r.Project.Assignment)
	b.WriteString("\n\n\n\n")
	fmt.Fprintf(&b, "## %s\n", r.Project.Assignment)
	b.WriteString("\n")
	fmt.Fprintf(&b, "### %s Prerequisites\n", prereqMark)
	b.WriteString("\n")

	for _, c := range r.Prerequisites {
		// The trailing space after the bold name is present in the original.
		fmt.Fprintf(&b, "**%s** \n: %s\n\n", c.Name, c.Status)
		if opts.Details && c.Detail != "" {
			b.WriteString(detailBlock(c.Detail))
		}
	}

	b.WriteString("\n### 📋 Test results\n\n")
	b.WriteString(disclaimer)
	b.WriteString("\n\n")

	for _, g := range r.Groups {
		fmt.Fprintf(&b, "#### %s\n\n", g.Spec.Name)
		fmt.Fprintf(&b, "**Compilation:** %s\n\n\n", g.Compilation)
		// Only a failure is worth explaining. A successful build can still
		// print warnings, and dumping those here would make the report
		// differ from the real one for no reason.
		if opts.Details && g.Compilation != result.OK && g.CompileLog != "" {
			b.WriteString(detailBlock(g.CompileLog))
		}
		for _, c := range g.Cases {
			fmt.Fprintf(&b, "**%s:** %s\n", c.Name, c.Status)
			if opts.Details && c.Detail != "" {
				b.WriteString(detailBlock(c.Detail))
			}
			b.WriteString("\n\n")
		}
		fmt.Fprintf(&b, "\n**Valid tests:** %d\n", g.Valid())
	}

	mark := "✅"
	if !r.Success() {
		mark = "❌"
	}
	fmt.Fprintf(&b, "\n\n%s **Assignment:** %s\n", mark, r.Project.Assignment)

	return b.String()
}

// detailBlock renders a failure explanation as a fenced code block. This
// layout is our own: the real moulinette's failure format is not documented
// in the reports we have.
func detailBlock(detail string) string {
	detail = strings.TrimRight(detail, "\n")
	return "\n```\n" + detail + "\n```\n"
}
