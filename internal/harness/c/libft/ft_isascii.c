#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		for (int c = 0; c <= 127; c++)
			classify("ft_isascii", c, ft_isascii(c), 1);
	}
	CASE(2)
	{
		for (int c = 128; c <= 255; c++)
			classify("ft_isascii", c, ft_isascii(c), 0);
	}
	CASE(3)
	{
		/* Negative values are outside ASCII; a signed-char bug shows here. */
		int	negatives[] = {-1, -42, -128, -256};
		for (size_t i = 0; i < sizeof(negatives) / sizeof(*negatives); i++)
			classify("ft_isascii", negatives[i], ft_isascii(negatives[i]), 0);
	}
	CASE(4)
	{
		classify("ft_isascii", 0, ft_isascii(0), 1);
		classify("ft_isascii", 127, ft_isascii(127), 1);
		classify("ft_isascii", 128, ft_isascii(128), 0);
	}
	CASE(5)
	{
		for (int c = -256; c <= 512; c++)
			classify("ft_isascii", c, ft_isascii(c), isascii(c) != 0);
	}
	return (t_finish());
}
