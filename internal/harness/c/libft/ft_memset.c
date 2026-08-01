#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		char	got[32];
		char	want[32];
		memset(want, 0, 32);
		ft_memset(got, 0, 32);
		EQ_MEM(got, want, 32);
	}
	CASE(2)
	{
		/* Only the first n bytes may change; the tail guards against an
		 * off-by-one in the loop. */
		char	got[32];
		char	want[32];
		memset(got, '-', 32);
		memset(want, '-', 32);
		ft_memset(got, 'A', 10);
		memset(want, 'A', 10);
		EQ_MEM(got, want, 32);
	}
	CASE(3)
	{
		char	got[8] = "abcdefg";
		ft_memset(got, 'Z', 0);
		EQ_STR(got, "abcdefg");
	}
	CASE(4)
	{
		char	buf[16];
		EQ_PTR(ft_memset(buf, 0, 16), buf);
	}
	CASE(5)
	{
		/* The value is taken as an unsigned char, so 256 wraps to 0 and
		 * -1 becomes 0xff. */
		unsigned char	got[16];
		unsigned char	want[16];
		ft_memset(got, 256, 16);
		memset(want, 256, 16);
		EQ_MEM(got, want, 16);
		ft_memset(got, -1, 16);
		memset(want, -1, 16);
		EQ_MEM(got, want, 16);
	}
	return (t_finish());
}
