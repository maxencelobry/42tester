#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* Normal append: the result is the length the string would have had. */
		char	dst[32];
		memset(dst, 0, 32);
		strcpy(dst, "foo");
		EQ_SIZE(ft_strlcat(dst, "bar", 32), 6);
		EQ_STR(dst, "foobar");
	}
	CASE(2)
	{
		/* size 0: nothing is written, and dst is treated as length 0. */
		char	dst[32];
		memset(dst, 0, 32);
		strcpy(dst, "foo");
		EQ_SIZE(ft_strlcat(dst, "bar", 0), 3);
		EQ_STR(dst, "foo");
	}
	CASE(3)
	{
		/* size smaller than the existing dst: the result is size + strlen(src),
		 * and dst is left alone. */
		char	dst[32];
		memset(dst, 0, 32);
		strcpy(dst, "foobar");
		EQ_SIZE(ft_strlcat(dst, "baz", 3), 6);
		EQ_STR(dst, "foobar");
	}
	CASE(4)
	{
		/* Truncation: only what fits is appended, terminator included. */
		char	dst[32];
		memset(dst, 0, 32);
		strcpy(dst, "foo");
		EQ_SIZE(ft_strlcat(dst, "barbaz", 8), 9);
		EQ_STR(dst, "foobarb");
	}
	CASE(5)
	{
		char	dst[32];
		memset(dst, 0, 32);
		strcpy(dst, "foo");
		EQ_SIZE(ft_strlcat(dst, "", 32), 3);
		EQ_STR(dst, "foo");

		/* Exactly enough room for the terminator and no more. */
		memset(dst, 0, 32);
		strcpy(dst, "abc");
		EQ_SIZE(ft_strlcat(dst, "de", 6), 5);
		EQ_STR(dst, "abcde");

		memset(dst, 0, 32);
		strcpy(dst, "abc");
		EQ_SIZE(ft_strlcat(dst, "de", 5), 5);
		EQ_STR(dst, "abcd");
	}
	return (t_finish());
}
