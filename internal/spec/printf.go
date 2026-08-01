package spec

// Printf mirrors docs/moulinette/printf.md: 6 groups, 63 cases.
//
// The subject asks for the conversions cspdiuxX%, but the report has groups
// only for c, s, p, x, X and %. The missing three are in RequiredGroups: the
// six report groups still render byte for byte, and %d, %i and %u are still
// graded.
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
		// The subject requires cspdiuxX%, so %d, %i and %u are graded even
		// though the report we have shows no group for them.
		RequiredGroups: []Group{
			simple("ft_printf_int", 15, "printf/int.c"),
			simple("ft_printf_unsigned", 12, "printf/unsigned.c"),
		},
		ExtraGroups: []Group{
			simple("ft_printf_mixed", 10, "printf/mixed.c"),
		},
	}
}
