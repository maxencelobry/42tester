package spec

// Printf mirrors docs/moulinette/printf.md: 6 groups, 63 cases.
//
// Note the real report has no group for %d, %i or %u even though the subject
// requires them. Those live in ExtraGroups and only run with --extra, so the
// default output stays a faithful copy of the moulinette.
func Printf() *Project {
	return &Project{
		ID:         "printf",
		Aliases:    []string{"ft_printf", "Printf"},
		Assignment: "Common_Core-Project-C-Printf",
		ExpectedFiles: []string{
			"Makefile",
			"ft_printf.h",
		},
		AllowedFuncs: []string{
			"malloc", "free", "write",
			"va_start", "va_arg", "va_copy", "va_end",
		},
		MandatoryFuncs: []string{"ft_printf"},
		Library:        "libftprintf.a",
		Headers:        []string{"ft_printf.h"},
		Groups: []Group{
			simple("ft_printf_char", 10, "printf/char.c"),
			simple("ft_printf_string", 10, "printf/string.c"),
			simple("ft_printf_pointer", 8, "printf/pointer.c"),
			simple("ft_printf_hex_lower", 15, "printf/hex_lower.c"),
			simple("ft_printf_hex_upper", 15, "printf/hex_upper.c"),
			simple("ft_printf_percent", 5, "printf/percent.c"),
		},
		ExtraGroups: []Group{
			simple("ft_printf_int", 15, "printf/int.c"),
			simple("ft_printf_unsigned", 12, "printf/unsigned.c"),
			simple("ft_printf_mixed", 10, "printf/mixed.c"),
		},
	}
}
