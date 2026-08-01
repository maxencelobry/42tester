#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		const char	*hay = "tripouille is here";
		EQ_PTR(ft_strnstr(hay, "is", 20), hay + 11);
		EQ_PTR(ft_strnstr(hay, "tri", 20), hay + 0);
	}
	CASE(2)
	{
		/* An empty needle matches at the very start, whatever len is. */
		const char	*hay = "tripouille";
		EQ_PTR(ft_strnstr(hay, "", 10), hay);
		EQ_PTR(ft_strnstr(hay, "", 0), hay);
	}
	CASE(3)
	{
		EQ_PTR(ft_strnstr("tripouille", "zz", 10), NULL);
		EQ_PTR(ft_strnstr("", "a", 10), NULL);
	}
	CASE(4)
	{
		/* "cde" sits at indices 2 to 4, so it needs len to reach 5. A match
		 * that would run past len does not count. */
		const char	*hay = "abcdef";
		EQ_PTR(ft_strnstr(hay, "cde", 6), hay + 2);
		EQ_PTR(ft_strnstr(hay, "cde", 5), hay + 2);
		EQ_PTR(ft_strnstr(hay, "cde", 4), NULL);
		EQ_PTR(ft_strnstr(hay, "cde", 3), NULL);
	}
	CASE(5)
	{
		/* len 0 finds nothing but the empty needle, and the search must not
		 * read past the end of the haystack. */
		EQ_PTR(ft_strnstr("abc", "a", 0), NULL);
		EQ_PTR(ft_strnstr("abc", "abcd", 100), NULL);

		const char	*hay = "aaabaaa";
		EQ_PTR(ft_strnstr(hay, "ab", 7), hay + 2);
	}
	return (t_finish());
}
