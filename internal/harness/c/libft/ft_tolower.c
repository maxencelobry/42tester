#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		for (int c = 'A'; c <= 'Z'; c++)
			EQ_INT(ft_tolower(c), c + 32);
	}
	CASE(2)
	{
		for (int c = 'a'; c <= 'z'; c++)
			EQ_INT(ft_tolower(c), c);
	}
	CASE(3)
	{
		const char	*others = "0123456789 \t\n!@#[]{}";
		for (int i = 0; others[i]; i++)
			EQ_INT(ft_tolower(others[i]), others[i]);
	}
	CASE(4)
	{
		int	edges[] = {'A' - 1, 'Z' + 1};
		for (size_t i = 0; i < sizeof(edges) / sizeof(*edges); i++)
			EQ_INT(ft_tolower(edges[i]), edges[i]);
	}
	CASE(5)
	{
		for (int c = 0; c <= 255; c++)
			t_assert(ft_tolower(c) == tolower(c),
				"ft_tolower(%d) returned %d, expected %d", c, ft_tolower(c), tolower(c));
	}
	return (t_finish());
}
