#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		for (int c = 32; c <= 126; c++)
			classify("ft_isprint", c, ft_isprint(c), 1);
	}
	CASE(2)
	{
		for (int c = 0; c < 32; c++)
			classify("ft_isprint", c, ft_isprint(c), 0);
	}
	CASE(3)
	{
		/* 127 is DEL: the last control character, not printable. */
		classify("ft_isprint", 127, ft_isprint(127), 0);
		classify("ft_isprint", 126, ft_isprint(126), 1);
		classify("ft_isprint", 31, ft_isprint(31), 0);
		classify("ft_isprint", 32, ft_isprint(32), 1);
	}
	CASE(4)
	{
		for (int c = 128; c <= 255; c++)
			classify("ft_isprint", c, ft_isprint(c), 0);
	}
	CASE(5)
	{
		for (int c = -1; c <= 255; c++)
			classify("ft_isprint", c, ft_isprint(c), isprint(c) != 0);
	}
	return (t_finish());
}
