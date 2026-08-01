#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		for (int c = '0'; c <= '9'; c++)
			classify("ft_isdigit", c, ft_isdigit(c), 1);
	}
	CASE(2)
	{
		const char	*letters = "abcxyzABCXYZ";
		for (int i = 0; letters[i]; i++)
			classify("ft_isdigit", letters[i], ft_isdigit(letters[i]), 0);
	}
	CASE(3)
	{
		int	edges[] = {'0' - 1, '9' + 1};
		for (size_t i = 0; i < sizeof(edges) / sizeof(*edges); i++)
			classify("ft_isdigit", edges[i], ft_isdigit(edges[i]), 0);
	}
	CASE(4)
	{
		/* A digit as a number rather than a character is a common mix-up. */
		for (int c = 0; c <= 9; c++)
			classify("ft_isdigit", c, ft_isdigit(c), 0);
	}
	CASE(5)
	{
		for (int c = -1; c <= 255; c++)
			classify("ft_isdigit", c, ft_isdigit(c), isdigit(c) != 0);
	}
	return (t_finish());
}
