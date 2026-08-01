#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		for (int c = 32; c <= 126; c++)
			t_assert(ft_isprint(c) != 0, "ft_isprint(%d) returned 0", c);
	}
	CASE(2)
	{
		for (int c = 0; c < 32; c++)
			t_assert(ft_isprint(c) == 0, "ft_isprint(%d) returned non-zero", c);
	}
	CASE(3)
	{
		/* 127 is DEL: the last control character, not printable. */
		t_assert(ft_isprint(127) == 0, "ft_isprint(127) returned non-zero");
		t_assert(ft_isprint(126) != 0, "ft_isprint(126) returned 0");
		t_assert(ft_isprint(31) == 0, "ft_isprint(31) returned non-zero");
		t_assert(ft_isprint(32) != 0, "ft_isprint(32) returned 0");
	}
	CASE(4)
	{
		for (int c = 128; c <= 255; c++)
			t_assert(ft_isprint(c) == 0, "ft_isprint(%d) returned non-zero", c);
	}
	CASE(5)
	{
		for (int c = -1; c <= 255; c++)
			t_assert(!!ft_isprint(c) == !!isprint(c),
				"ft_isprint(%d) disagrees with isprint(%d)", c, c);
	}
	return (t_finish());
}
