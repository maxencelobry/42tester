#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		for (int c = 'a'; c <= 'z'; c++)
			EQ_INT(ft_toupper(c), c - 32);
	}
	CASE(2)
	{
		for (int c = 'A'; c <= 'Z'; c++)
			EQ_INT(ft_toupper(c), c);
	}
	CASE(3)
	{
		/* Everything that is not a lowercase letter comes back unchanged. */
		const char	*others = "0123456789 \t\n!@#[]{}";
		for (int i = 0; others[i]; i++)
			EQ_INT(ft_toupper(others[i]), others[i]);
	}
	CASE(4)
	{
		int	edges[] = {'a' - 1, 'z' + 1};
		for (size_t i = 0; i < sizeof(edges) / sizeof(*edges); i++)
			EQ_INT(ft_toupper(edges[i]), edges[i]);
	}
	CASE(5)
	{
		for (int c = 0; c <= 255; c++)
			t_assert(ft_toupper(c) == toupper(c),
				"ft_toupper(%d) returned %d, expected %d", c, ft_toupper(c), toupper(c));
	}
	return (t_finish());
}
