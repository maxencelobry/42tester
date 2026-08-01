#include "libft_test.h"

/* sign reduces a comparison result to -1, 0 or 1: the standard only promises
 * the sign, so comparing exact values would fail correct implementations. */
static int	sign(int v)
{
	return ((v > 0) - (v < 0));
}

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		EQ_INT(sign(ft_strncmp("tripouille", "tripouille", 10)), 0);
		EQ_INT(sign(ft_strncmp("", "", 42)), 0);
	}
	CASE(2)
	{
		EQ_INT(sign(ft_strncmp("abc", "abd", 3)), -1);
		EQ_INT(sign(ft_strncmp("abd", "abc", 3)), 1);
	}
	CASE(3)
	{
		/* n = 0 compares nothing, so the strings are equal. */
		EQ_INT(sign(ft_strncmp("abc", "xyz", 0)), 0);
	}
	CASE(4)
	{
		/* The comparison stops before the difference. */
		EQ_INT(sign(ft_strncmp("abcX", "abcY", 3)), 0);
		EQ_INT(sign(ft_strncmp("abcX", "abcY", 4)), -1);
		/* And it stops at the terminator even if n is larger. */
		EQ_INT(sign(ft_strncmp("abc", "abc", 100)), 0);
		EQ_INT(sign(ft_strncmp("abc", "abcd", 100)), -1);
	}
	CASE(5)
	{
		/* Bytes are compared as unsigned char: a signed comparison gets the
		 * sign backwards here. */
		EQ_INT(sign(ft_strncmp("\200", "\001", 1)), 1);
		EQ_INT(sign(ft_strncmp("\001", "\200", 1)), -1);
		EQ_INT(sign(ft_strncmp("test\200", "test\0", 6)), 1);
	}
	return (t_finish());
}
