#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* The return value is the length of src, not what was copied. */
		char	dst[32];
		memset(dst, '#', 32);
		EQ_SIZE(ft_strlcpy(dst, "tripouille", 32), 10);
		EQ_STR(dst, "tripouille");
	}
	CASE(2)
	{
		/* size 0 means "touch nothing", but still report src's length. */
		char	dst[16] = "untouched";
		EQ_SIZE(ft_strlcpy(dst, "hello", 0), 5);
		EQ_STR(dst, "untouched");
	}
	CASE(3)
	{
		/* size 1 leaves room for the terminator only. */
		char	dst[16];
		memset(dst, '#', 16);
		EQ_SIZE(ft_strlcpy(dst, "hello", 1), 5);
		EQ_STR(dst, "");
	}
	CASE(4)
	{
		/* Truncation still terminates the string. */
		char	dst[16];
		memset(dst, '#', 16);
		EQ_SIZE(ft_strlcpy(dst, "tripouille", 5), 10);
		EQ_STR(dst, "trip");
		t_assert(dst[5] == '#', "ft_strlcpy wrote past the size it was given");
	}
	CASE(5)
	{
		char	dst[16];
		memset(dst, '#', 16);
		EQ_SIZE(ft_strlcpy(dst, "", 10), 0);
		EQ_STR(dst, "");

		char	exact[6];
		memset(exact, '#', 6);
		EQ_SIZE(ft_strlcpy(exact, "hello", 6), 5);
		EQ_STR(exact, "hello");
	}
	return (t_finish());
}
