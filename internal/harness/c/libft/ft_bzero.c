#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		char	got[32];
		char	want[32];
		memset(got, 'x', 32);
		memset(want, 0, 32);
		ft_bzero(got, 32);
		EQ_MEM(got, want, 32);
	}
	CASE(2)
	{
		char	got[16];
		char	want[16];
		memset(got, 'x', 16);
		memset(want, 'x', 16);
		memset(want, 0, 5);
		ft_bzero(got, 5);
		EQ_MEM(got, want, 16);
	}
	CASE(3)
	{
		char	got[8] = "abcdefg";
		ft_bzero(got, 0);
		EQ_STR(got, "abcdefg");
	}
	return (t_finish());
}
