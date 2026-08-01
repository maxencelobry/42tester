package spec

// GetNextLine mirrors docs/moulinette/get_next_line.md: 13 groups, 44 cases.
//
// The three buffer_size_* groups are the same source compiled three times
// with a different -D BUFFER_SIZE, which is why they share a report prefix.
func GetNextLine() *Project {
	return &Project{
		ID:         "gnl",
		Aliases:    []string{"get_next_line", "getnextline", "GetNextLine"},
		Assignment: "Common_Core-Project-C-GetNextLine",
		ExpectedFiles: []string{
			"get_next_line.c",
			"get_next_line_utils.c",
			"get_next_line.h",
		},
		AllowedFuncs: []string{"read", "malloc", "free"},
		Sources:      []string{"get_next_line.c", "get_next_line_utils.c"},
		Headers:      []string{"get_next_line.h"},
		Groups: []Group{
			// The heading already ends in _test, so the label is not doubled.
			{Name: "simple_test", Prefix: "simple", Cases: 5, Source: "gnl/simple.c"},
			simple("long_lines", 5, "gnl/long_lines.c"),
			simple("mixed_lines", 5, "gnl/mixed_lines.c"),
			simple("no_newline", 3, "gnl/no_newline.c"),
			simple("single_char", 3, "gnl/single_char.c"),
			simple("empty_file", 1, "gnl/empty_file.c"),
			simple("multiple_newlines", 5, "gnl/multiple_newlines.c"),
			simple("edge_cases", 5, "gnl/edge_cases.c"),
			bufSize("buffer_size_0", 0, 2),
			bufSize("buffer_size_1", 1, 3),
			bufSize("buffer_size_1000", 1000, 3),
			{Name: "stdin_test", Prefix: "stdin", Cases: 3, Source: "gnl/stdin.c"},
			simple("invalid_fd", 1, "gnl/invalid_fd.c"),
		},
		ExtraGroups: []Group{
			simple("multiple_fd", 4, "gnl/multiple_fd.c"),
			simple("binary_file", 3, "gnl/binary_file.c"),
			bufSize("buffer_size_9999", 9999, 3),
		},
	}
}

func bufSize(name string, size, cases int) Group {
	return Group{
		Name:   name,
		Prefix: "buffer_size",
		Cases:  cases,
		Source: "gnl/buffer_size.c",
		CFlags: []string{"-DBUFFER_SIZE=" + itoa(size), "-DGNL_BUFFER_SIZE=" + itoa(size)},
	}
}

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
