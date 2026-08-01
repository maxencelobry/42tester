#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		char	dst[32];
		memset(dst, 0, 32);
		ft_memmove(dst, "tripouille", 10);
		EQ_MEM(dst, "tripouille", 10);
	}
	CASE(2)
	{
		/* Destination after source: a naive forward copy corrupts this. */
		char	got[16] = "0123456789";
		char	want[16] = "0123456789";
		ft_memmove(got + 2, got, 5);
		memmove(want + 2, want, 5);
		EQ_STR(got, want);
	}
	CASE(3)
	{
		/* Destination before source: a naive backward copy corrupts this. */
		char	got[16] = "0123456789";
		char	want[16] = "0123456789";
		ft_memmove(got, got + 2, 5);
		memmove(want, want + 2, 5);
		EQ_STR(got, want);
	}
	CASE(4)
	{
		char	buf[8] = "abcdefg";
		ft_memmove(buf, buf + 1, 0);
		EQ_STR(buf, "abcdefg");
		EQ_PTR(ft_memmove(buf, buf, 3), buf);
	}
	CASE(5)
	{
		char	got[512];
		char	want[512];
		for (int i = 0; i < 512; i++)
			got[i] = want[i] = (char)(i % 251);
		ft_memmove(got + 100, got, 300);
		memmove(want + 100, want, 300);
		EQ_MEM(got, want, 512);
	}
	return (t_finish());
}
