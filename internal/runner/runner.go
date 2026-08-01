// Package runner executes the compiled test binaries and turns their output
// into cases.
package runner

import (
	"bytes"
	"context"
	"errors"
	"os/exec"
	"strconv"
	"strings"
	"time"

	"tester42/internal/result"
	"tester42/internal/spec"
)

// Runner executes one group at a time.
type Runner struct {
	// Timeout bounds a single invocation, whether it runs the whole group or
	// one case.
	Timeout time.Duration
	// Dir is the working directory of the test binaries. Tests write scratch
	// files relative to it, so it must not be the user's own directory: a
	// crash leaves those files behind.
	Dir string
	// Env is passed to the test binaries, used for sanitizer options.
	Env []string
}

// New returns a runner with sensible defaults.
func New(timeout time.Duration, dir string) *Runner {
	if timeout <= 0 {
		timeout = 5 * time.Second
	}
	return &Runner{Timeout: timeout, Dir: dir}
}

// RunGroup runs every case of g and returns them in order.
//
// The binary is first asked to run all cases at once, which is fast. If it
// dies or hangs partway, the remaining cases are re-run one process each so
// a single segfault costs one case instead of the whole group.
func (r *Runner) RunGroup(exe string, g spec.Group) []result.Case {
	cases := make([]result.Case, g.Cases)
	for i := range cases {
		cases[i] = result.Case{Number: i + 1, Name: g.TestName(i + 1)}
	}

	reported, timedOut := r.invoke(exe)
	for i := range cases {
		if c, ok := reported[i+1]; ok {
			cases[i].Status = c.status
			cases[i].Detail = c.detail
			cases[i].Expected = c.expected
			cases[i].Got = c.got
		} else {
			cases[i].Status = result.Crash
			if timedOut {
				cases[i].Status = result.Timeout
			}
		}
	}

	// Anything the batch run failed to report gets its own process, so we
	// learn whether it really crashed or was merely downstream of one.
	for i := range cases {
		if cases[i].Status != result.Crash && cases[i].Status != result.Timeout {
			continue
		}
		single, hung := r.invoke(exe, strconv.Itoa(i+1))
		if c, ok := single[i+1]; ok {
			cases[i].Status = c.status
			cases[i].Detail = c.detail
			cases[i].Expected = c.expected
			cases[i].Got = c.got
			continue
		}
		if hung {
			cases[i].Status = result.Timeout
			cases[i].Detail = "the test did not finish within " + r.Timeout.String() + " (infinite loop?)"
		} else {
			cases[i].Status = result.Crash
			cases[i].Detail = "the process died without reaching the end of the test (segfault, abort or bus error)"
		}
	}

	return cases
}

type reportedCase struct {
	status   result.Status
	detail   string
	expected string
	got      string
}

// invoke runs the binary and parses the protocol lines it writes to stderr.
// Test output goes to stdout, which is why the protocol does not share it.
func (r *Runner) invoke(exe string, args ...string) (map[int]reportedCase, bool) {
	ctx, cancel := context.WithTimeout(context.Background(), r.Timeout)
	defer cancel()

	cmd := exec.CommandContext(ctx, exe, args...)
	cmd.Dir = r.Dir
	var stderr bytes.Buffer
	cmd.Stderr = &stderr
	cmd.Stdout = nil
	if len(r.Env) > 0 {
		cmd.Env = r.Env
	}

	err := cmd.Run()
	timedOut := errors.Is(ctx.Err(), context.DeadlineExceeded)
	_ = err // a non-zero exit is expected whenever a case fails

	return parse(stderr.String()), timedOut
}

// parse reads the protocol lines the harness writes to stderr:
//
//	@@CASE <n> <OK|KO|CRASH> <one-line explanation>
//	@@CASE <n> EXPECTED <value>
//	@@CASE <n> GOT <value>
//
// The last two are optional and only follow a failing comparison.
func parse(out string) map[int]reportedCase {
	cases := map[int]reportedCase{}
	for _, line := range strings.Split(out, "\n") {
		line = strings.TrimRight(line, "\r")
		if !strings.HasPrefix(line, "@@CASE ") {
			continue
		}
		parts := strings.SplitN(line[len("@@CASE "):], " ", 3)
		if len(parts) < 2 {
			continue
		}
		n, err := strconv.Atoi(parts[0])
		if err != nil {
			continue
		}
		var value string
		if len(parts) == 3 {
			value = unescape(parts[2])
		}

		c := cases[n]
		switch parts[1] {
		case "EXPECTED":
			c.expected = value
		case "GOT":
			c.got = value
		default:
			c.status = statusOf(parts[1])
			c.detail = value
		}
		cases[n] = c
	}
	return cases
}

func statusOf(s string) result.Status {
	switch s {
	case "OK":
		return result.OK
	case "CRASH":
		return result.Crash
	case "TIMEOUT":
		return result.Timeout
	default:
		return result.KO
	}
}

// unescape reverses the C side's encoding, which keeps every detail on one
// line so the protocol stays line-oriented.
func unescape(s string) string {
	var b strings.Builder
	for i := 0; i < len(s); i++ {
		if s[i] != '\\' || i+1 >= len(s) {
			b.WriteByte(s[i])
			continue
		}
		i++
		switch s[i] {
		case 'n':
			b.WriteByte('\n')
		case 't':
			b.WriteByte('\t')
		case '\\':
			b.WriteByte('\\')
		default:
			b.WriteByte(s[i])
		}
	}
	return b.String()
}
