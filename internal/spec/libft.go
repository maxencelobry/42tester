package spec

// Libft mirrors docs/moulinette/libft.md: 43 groups, 232 cases.
//
// The lst* groups appear in the same flow as the rest in the real report,
// but they only exist once the bonus part is turned in, so they are flagged
// as bonus and skipped when the archive has no ft_lst* symbols.
func Libft() *Project {
	return &Project{
		ID:         "libft",
		Aliases:    []string{"Libft", "ft_libft"},
		Assignment: "Common_Core-Project-C-Libft",
		ExpectedFiles: []string{
			"Makefile",
			"libft.h",
		},
		BonusFiles:   []string{"ft_lstnew_bonus.c", "ft_lstnew.c"},
		AllowedFuncs: []string{"malloc", "free", "write"},
		Library:      "libft.a",
		Headers:      []string{"libft.h"},
		Groups: []Group{
			simple("ft_isalpha", 5, "libft/ft_isalpha.c"),
			simple("ft_isdigit", 5, "libft/ft_isdigit.c"),
			simple("ft_isalnum", 5, "libft/ft_isalnum.c"),
			simple("ft_isascii", 5, "libft/ft_isascii.c"),
			simple("ft_isprint", 5, "libft/ft_isprint.c"),
			simple("ft_strlen", 5, "libft/ft_strlen.c"),
			simple("ft_memset", 5, "libft/ft_memset.c"),
			simple("ft_bzero", 3, "libft/ft_bzero.c"),
			simple("ft_memcpy", 5, "libft/ft_memcpy.c"),
			simple("ft_memmove", 5, "libft/ft_memmove.c"),
			simple("ft_strlcpy", 5, "libft/ft_strlcpy.c"),
			simple("ft_strlcat", 5, "libft/ft_strlcat.c"),
			simple("ft_toupper", 5, "libft/ft_toupper.c"),
			simple("ft_tolower", 5, "libft/ft_tolower.c"),
			simple("ft_strchr", 5, "libft/ft_strchr.c"),
			simple("ft_strrchr", 5, "libft/ft_strrchr.c"),
			simple("ft_strncmp", 5, "libft/ft_strncmp.c"),
			simple("ft_memchr", 5, "libft/ft_memchr.c"),
			simple("ft_memcmp", 5, "libft/ft_memcmp.c"),
			simple("ft_strnstr", 5, "libft/ft_strnstr.c"),
			simple("ft_atoi", 10, "libft/ft_atoi.c"),
			simple("ft_calloc", 5, "libft/ft_calloc.c"),
			simple("ft_strdup", 5, "libft/ft_strdup.c"),
			simple("ft_substr", 10, "libft/ft_substr.c"),
			simple("ft_strjoin", 5, "libft/ft_strjoin.c"),
			simple("ft_strtrim", 5, "libft/ft_strtrim.c"),
			simple("ft_split", 5, "libft/ft_split.c"),
			simple("ft_itoa", 10, "libft/ft_itoa.c"),
			simple("ft_strmapi", 5, "libft/ft_strmapi.c"),
			simple("ft_striteri", 5, "libft/ft_striteri.c"),
			simple("ft_putchar_fd", 3, "libft/ft_putchar_fd.c"),
			simple("ft_putstr_fd", 3, "libft/ft_putstr_fd.c"),
			simple("ft_putendl_fd", 3, "libft/ft_putendl_fd.c"),
			simple("ft_putnbr_fd", 5, "libft/ft_putnbr_fd.c"),
			bonus("ft_lstnew", 5, "libft/ft_lstnew.c"),
			bonus("ft_lstadd_front", 5, "libft/ft_lstadd_front.c"),
			bonus("ft_lstsize", 5, "libft/ft_lstsize.c"),
			bonus("ft_lstlast", 5, "libft/ft_lstlast.c"),
			bonus("ft_lstadd_back", 5, "libft/ft_lstadd_back.c"),
			bonus("ft_lstdelone", 5, "libft/ft_lstdelone.c"),
			bonus("ft_lstclear", 5, "libft/ft_lstclear.c"),
			bonus("ft_lstiter", 5, "libft/ft_lstiter.c"),
			bonus("ft_lstmap", 5, "libft/ft_lstmap.c"),
		},
	}
}

func bonus(name string, cases int, source string) Group {
	g := simple(name, cases, source)
	g.Bonus = true
	return g
}
