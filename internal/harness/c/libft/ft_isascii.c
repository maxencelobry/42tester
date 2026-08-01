#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		for (int c = 0; c <= 127; c++)
			t_assert(ft_isascii(c) != 0, "ft_isascii(%d) returned 0", c);
	}
	CASE(2)
	{
		for (int c = 128; c <= 255; c++)
			t_assert(ft_isascii(c) == 0, "ft_isascii(%d) returned non-zero", c);
	}
	CASE(3)
	{
		/* Negative values are outside ASCII; a signed-char bug shows here. */
		int	negatives[] = {-1, -42, -128, -256};
		for (size_t i = 0; i < sizeof(negatives) / sizeof(*negatives); i++)
			t_assert(ft_isascii(negatives[i]) == 0,
				"ft_isascii(%d) returned non-zero", negatives[i]);
	}
	CASE(4)
	{
		t_assert(ft_isascii(0) != 0, "ft_isascii(0) returned 0");
		t_assert(ft_isascii(127) != 0, "ft_isascii(127) returned 0");
		t_assert(ft_isascii(128) == 0, "ft_isascii(128) returned non-zero");
	}
	CASE(5)
	{
		for (int c = -256; c <= 512; c++)
			t_assert(!!ft_isascii(c) == !!isascii(c),
				"ft_isascii(%d) disagrees with isascii(%d)", c, c);
	}
	return (t_finish());
}
