#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		for (int c = '0'; c <= '9'; c++)
			classify("ft_isalnum", c, ft_isalnum(c), 1);
	}
	CASE(2)
	{
		for (int c = 'a'; c <= 'z'; c++)
			classify("ft_isalnum", c, ft_isalnum(c), 1);
		for (int c = 'A'; c <= 'Z'; c++)
			classify("ft_isalnum", c, ft_isalnum(c), 1);
	}
	CASE(3)
	{
		const char	*others = " \t\n!@#$%^&*()[]{}_-+=";
		for (int i = 0; others[i]; i++)
			classify("ft_isalnum", others[i], ft_isalnum(others[i]), 0);
	}
	CASE(4)
	{
		int	edges[] = {'0' - 1, '9' + 1, 'a' - 1, 'z' + 1, 'A' - 1, 'Z' + 1};
		for (size_t i = 0; i < sizeof(edges) / sizeof(*edges); i++)
			classify("ft_isalnum", edges[i], ft_isalnum(edges[i]), 0);
	}
	CASE(5)
	{
		for (int c = -1; c <= 255; c++)
			classify("ft_isalnum", c, ft_isalnum(c), isalnum(c) != 0);
	}
	return (t_finish());
}
