// Package detect figures out which 42 project a directory holds, so students
// can just run the tester with a path and no flags.
package detect

import (
	"os"
	"path/filepath"
	"regexp"
	"sort"
	"strings"

	"tester42/internal/spec"
)

// Match is a candidate project found in a directory.
type Match struct {
	Project *spec.Project
	// Dir is where the submission lives, which may be a subdirectory of the
	// path the user pointed at.
	Dir string
	// Score is the confidence; higher wins.
	Score int
	// Reasons lists the evidence, shown when the guess is wrong.
	Reasons []string
}

// signal is one piece of evidence for a project.
type signal struct {
	file   string
	points int
}

var signals = map[string][]signal{
	// get_next_line first: its files are unmistakable, and a submission that
	// has them is never anything else.
	"gnl": {
		{"get_next_line.c", 4},
		{"get_next_line.h", 4},
		{"get_next_line_utils.c", 3},
		{"get_next_line_bonus.c", 1},
	},
	"printf": {
		{"ft_printf.c", 5},
		{"ft_printf.h", 4},
		{"ft_printf_utils.c", 2},
	},
	"libft": {
		{"libft.h", 4},
		{"ft_strlcpy.c", 2},
		{"ft_split.c", 2},
		{"ft_substr.c", 2},
		{"ft_lstnew.c", 1},
		{"ft_lstnew_bonus.c", 1},
	},
}

// nameRule finds the archive a Makefile builds, which is the strongest hint
// of all: NAME = libft.a versus NAME = libftprintf.a.
var nameRule = regexp.MustCompile(`(?m)^\s*NAME\s*[:?]?=\s*(\S+)`)

var archiveOwner = map[string]string{
	"libft.a":       "libft",
	"libftprintf.a": "printf",
}

// Scan looks for projects in root and, failing that, in its subdirectories.
// It returns matches sorted best first.
func Scan(root string) []Match {
	if m := scanDir(root); len(m) > 0 {
		return m
	}

	var all []Match
	entries, err := os.ReadDir(root)
	if err != nil {
		return nil
	}
	for _, e := range entries {
		if !e.IsDir() || strings.HasPrefix(e.Name(), ".") {
			continue
		}
		all = append(all, scanDir(filepath.Join(root, e.Name()))...)
	}
	sort.SliceStable(all, func(i, j int) bool { return all[i].Score > all[j].Score })
	return all
}

// minScore is the confidence below which we refuse to guess. It is set so a
// lone libft.h or a lone get_next_line.c is enough, but a stray ft_split.c
// copied into an unrelated folder is not.
const minScore = 4

func scanDir(dir string) []Match {
	scores := map[string]int{}
	reasons := map[string][]string{}

	for id, sigs := range signals {
		for _, s := range sigs {
			if exists(filepath.Join(dir, s.file)) {
				scores[id] += s.points
				reasons[id] = append(reasons[id], s.file)
			}
		}
	}

	if name, ok := makefileName(dir); ok {
		if id, known := archiveOwner[name]; known {
			scores[id] += 5
			reasons[id] = append(reasons[id], "Makefile builds "+name)
		}
	}

	// A printf submission often vendors a libft; the printf signals are
	// specific enough to win, so drop the weaker libft guess to avoid
	// reporting the same directory twice.
	if scores["printf"] >= minScore && scores["libft"] > 0 {
		delete(scores, "libft")
	}

	var out []Match
	for id, score := range scores {
		if score < minScore {
			continue
		}
		p, err := spec.Lookup(id)
		if err != nil {
			continue
		}
		out = append(out, Match{Project: p, Dir: dir, Score: score, Reasons: reasons[id]})
	}
	sort.SliceStable(out, func(i, j int) bool { return out[i].Score > out[j].Score })
	return out
}

// makefileName extracts the NAME variable from a Makefile.
func makefileName(dir string) (string, bool) {
	for _, f := range []string{"Makefile", "makefile", "GNUmakefile"} {
		data, err := os.ReadFile(filepath.Join(dir, f))
		if err != nil {
			continue
		}
		if m := nameRule.FindSubmatch(data); m != nil {
			return strings.TrimSpace(string(m[1])), true
		}
	}
	return "", false
}

func exists(path string) bool {
	_, err := os.Stat(path)
	return err == nil
}
