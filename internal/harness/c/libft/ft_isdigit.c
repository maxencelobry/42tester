#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		for (int c = '0'; c <= '9'; c++)
			t_assert(ft_isdigit(c) != 0, "ft_isdigit('%c') returned 0", c);
	}
	CASE(2)
	{
		const char	*letters = "abcxyzABCXYZ";
		for (int i = 0; letters[i]; i++)
			t_assert(ft_isdigit(letters[i]) == 0,
				"ft_isdigit('%c') returned non-zero", letters[i]);
	}
	CASE(3)
	{
		int	edges[] = {'0' - 1, '9' + 1};
		for (size_t i = 0; i < sizeof(edges) / sizeof(*edges); i++)
			t_assert(ft_isdigit(edges[i]) == 0,
				"ft_isdigit(%d) returned non-zero", edges[i]);
	}
	CASE(4)
	{
		/* A digit as a number rather than a character is a common mix-up. */
		for (int c = 0; c <= 9; c++)
			t_assert(ft_isdigit(c) == 0, "ft_isdigit(%d) returned non-zero", c);
	}
	CASE(5)
	{
		for (int c = -1; c <= 255; c++)
			t_assert(!!ft_isdigit(c) == !!isdigit(c),
				"ft_isdigit(%d) disagrees with isdigit(%d)", c, c);
	}
	return (t_finish());
}
