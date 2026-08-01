#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		char	dst[32];
		memset(dst, 0, 32);
		ft_memcpy(dst, "tripouille", 10);
		EQ_MEM(dst, "tripouille", 10);
	}
	CASE(2)
	{
		/* A NUL in the middle must not stop the copy: this is memcpy, not
		 * strcpy. */
		char		dst[16];
		const char	src[8] = {'a', 0, 'b', 0, 'c', 0, 'd', 0};
		memset(dst, '#', 16);
		ft_memcpy(dst, src, 8);
		EQ_MEM(dst, src, 8);
		t_assert(dst[8] == '#', "ft_memcpy wrote past the %d bytes it was given", 8);
	}
	CASE(3)
	{
		char	dst[8] = "abcdefg";
		ft_memcpy(dst, "XYZ", 0);
		EQ_STR(dst, "abcdefg");
	}
	CASE(4)
	{
		char	dst[16];
		EQ_PTR(ft_memcpy(dst, "hello", 5), dst);
	}
	CASE(5)
	{
		/* Copying nothing between two null pointers is legal and the
		 * moulinette checks it. */
		ft_memcpy(NULL, NULL, 0);

		char	dst[300];
		char	src[300];
		for (int i = 0; i < 300; i++)
			src[i] = (char)(i * 7 + 1);
		memset(dst, 0, 300);
		ft_memcpy(dst, src, 300);
		EQ_MEM(dst, src, 300);
	}
	return (t_finish());
}
