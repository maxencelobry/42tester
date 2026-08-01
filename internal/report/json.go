package report

import (
	"encoding/json"

	"tester42/internal/result"
)

type jsonReport struct {
	Assignment    string      `json:"assignment"`
	Project       string      `json:"project"`
	Success       bool        `json:"success"`
	Aborted       bool        `json:"aborted,omitempty"`
	AbortReason   string      `json:"abort_reason,omitempty"`
	ValidTests    int         `json:"valid_tests"`
	TotalTests    int         `json:"total_tests"`
	Prerequisites []jsonCheck `json:"prerequisites"`
	Groups        []jsonGroup `json:"groups"`
}

type jsonCheck struct {
	Name   string `json:"name"`
	Status string `json:"status"`
	Detail string `json:"detail,omitempty"`
}

type jsonGroup struct {
	Name        string     `json:"name"`
	Compilation string     `json:"compilation"`
	CompileLog  string     `json:"compile_log,omitempty"`
	Valid       int        `json:"valid_tests"`
	Cases       []jsonCase `json:"cases"`
}

type jsonCase struct {
	Name   string `json:"name"`
	Status string `json:"status"`
	Detail string `json:"detail,omitempty"`
}

// statusKey is the stable machine-readable form, unlike Status.String which
// carries the report's emoji.
func statusKey(s result.Status) string {
	switch s {
	case result.OK:
		return "ok"
	case result.KO:
		return "ko"
	case result.Crash:
		return "crash"
	case result.Timeout:
		return "timeout"
	case result.Skipped:
		return "skipped"
	}
	return "unknown"
}

// JSON renders the report for CI or for a web front-end.
func JSON(r *result.Report) ([]byte, error) {
	valid, total := r.Totals()
	out := jsonReport{
		Assignment:  r.Project.Assignment,
		Project:     r.Project.ID,
		Success:     r.Success(),
		Aborted:     r.Aborted,
		AbortReason: r.AbortReason,
		ValidTests:  valid,
		TotalTests:  total,
	}
	for _, c := range r.Prerequisites {
		out.Prerequisites = append(out.Prerequisites, jsonCheck{c.Name, statusKey(c.Status), c.Detail})
	}
	for _, g := range r.Groups {
		jg := jsonGroup{
			Name:        g.Spec.Name,
			Compilation: statusKey(g.Compilation),
			Valid:       g.Valid(),
		}
		if g.Compilation != result.OK {
			jg.CompileLog = g.CompileLog
		}
		for _, c := range g.Cases {
			jg.Cases = append(jg.Cases, jsonCase{c.Name, statusKey(c.Status), c.Detail})
		}
		out.Groups = append(out.Groups, jg)
	}
	return json.MarshalIndent(out, "", "  ")
}
