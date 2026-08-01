#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		for (int c = 'a'; c <= 'z'; c++)
			t_assert(ft_isalpha(c) != 0, "ft_isalpha('%c') returned 0", c);
	}
	CASE(2)
	{
		for (int c = 'A'; c <= 'Z'; c++)
			t_assert(ft_isalpha(c) != 0, "ft_isalpha('%c') returned 0", c);
	}
	CASE(3)
	{
		const char	*others = "0123456789 \t\n!@#$%^&*()[]{}";
		for (int i = 0; others[i]; i++)
			t_assert(ft_isalpha(others[i]) == 0,
				"ft_isalpha('%c') returned non-zero", others[i]);
	}
	CASE(4)
	{
		/* The characters just outside each range are where off-by-one
		 * comparisons show up. */
		int	edges[] = {'a' - 1, 'z' + 1, 'A' - 1, 'Z' + 1};
		for (size_t i = 0; i < sizeof(edges) / sizeof(*edges); i++)
			t_assert(ft_isalpha(edges[i]) == 0,
				"ft_isalpha(%d) returned non-zero", edges[i]);
	}
	CASE(5)
	{
		for (int c = -1; c <= 255; c++)
			t_assert(!!ft_isalpha(c) == !!isalpha(c),
				"ft_isalpha(%d) disagrees with isalpha(%d)", c, c);
	}
	return (t_finish());
}
