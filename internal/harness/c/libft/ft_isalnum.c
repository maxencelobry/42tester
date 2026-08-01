#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		for (int c = '0'; c <= '9'; c++)
			t_assert(ft_isalnum(c) != 0, "ft_isalnum('%c') returned 0", c);
	}
	CASE(2)
	{
		for (int c = 'a'; c <= 'z'; c++)
			t_assert(ft_isalnum(c) != 0, "ft_isalnum('%c') returned 0", c);
		for (int c = 'A'; c <= 'Z'; c++)
			t_assert(ft_isalnum(c) != 0, "ft_isalnum('%c') returned 0", c);
	}
	CASE(3)
	{
		const char	*others = " \t\n!@#$%^&*()[]{}_-+=";
		for (int i = 0; others[i]; i++)
			t_assert(ft_isalnum(others[i]) == 0,
				"ft_isalnum('%c') returned non-zero", others[i]);
	}
	CASE(4)
	{
		int	edges[] = {'0' - 1, '9' + 1, 'a' - 1, 'z' + 1, 'A' - 1, 'Z' + 1};
		for (size_t i = 0; i < sizeof(edges) / sizeof(*edges); i++)
			t_assert(ft_isalnum(edges[i]) == 0,
				"ft_isalnum(%d) returned non-zero", edges[i]);
	}
	CASE(5)
	{
		for (int c = -1; c <= 255; c++)
			t_assert(!!ft_isalnum(c) == !!isalnum(c),
				"ft_isalnum(%d) disagrees with isalnum(%d)", c, c);
	}
	return (t_finish());
}
