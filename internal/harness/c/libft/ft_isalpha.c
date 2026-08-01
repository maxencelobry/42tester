#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		for (int c = 'a'; c <= 'z'; c++)
			classify("ft_isalpha", c, ft_isalpha(c), 1);
	}
	CASE(2)
	{
		for (int c = 'A'; c <= 'Z'; c++)
			classify("ft_isalpha", c, ft_isalpha(c), 1);
	}
	CASE(3)
	{
		const char	*others = "0123456789 \t\n!@#$%^&*()[]{}";
		for (int i = 0; others[i]; i++)
			classify("ft_isalpha", others[i], ft_isalpha(others[i]), 0);
	}
	CASE(4)
	{
		/* The characters just outside each range are where off-by-one
		 * comparisons show up. */
		int	edges[] = {'a' - 1, 'z' + 1, 'A' - 1, 'Z' + 1};
		for (size_t i = 0; i < sizeof(edges) / sizeof(*edges); i++)
			classify("ft_isalpha", edges[i], ft_isalpha(edges[i]), 0);
	}
	CASE(5)
	{
		for (int c = -1; c <= 255; c++)
			classify("ft_isalpha", c, ft_isalpha(c), isalpha(c) != 0);
	}
	return (t_finish());
}
