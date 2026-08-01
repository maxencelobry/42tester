#include "libft_test.h"

static int	sign(int v)
{
	return ((v > 0) - (v < 0));
}

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		EQ_INT(sign(ft_memcmp("tripouille", "tripouille", 10)), 0);
	}
	CASE(2)
	{
		EQ_INT(sign(ft_memcmp("abc", "abd", 3)), -1);
		EQ_INT(sign(ft_memcmp("abd", "abc", 3)), 1);
	}
	CASE(3)
	{
		EQ_INT(sign(ft_memcmp("abc", "xyz", 0)), 0);
		EQ_INT(sign(ft_memcmp("abcX", "abcY", 3)), 0);
	}
	CASE(4)
	{
		/* A NUL byte does not end the comparison. */
		const char	a[6] = {'a', 0, 'b', 0, 'c', 0};
		const char	b[6] = {'a', 0, 'b', 0, 'd', 0};
		EQ_INT(sign(ft_memcmp(a, b, 6)), -1);
		EQ_INT(sign(ft_memcmp(a, b, 4)), 0);
	}
	CASE(5)
	{
		const unsigned char	a[2] = {0x80, 0};
		const unsigned char	b[2] = {0x01, 0};
		EQ_INT(sign(ft_memcmp(a, b, 1)), 1);
		EQ_INT(sign(ft_memcmp(b, a, 1)), -1);
	}
	return (t_finish());
}
