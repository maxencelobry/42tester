#include "libft_test.h"

/* to_upper ignores the index; index_digit uses it. Between them they show
 * whether both arguments reach the callback. */
static char	to_upper(unsigned int i, char c)
{
	(void)i;
	if (c >= 'a' && c <= 'z')
		return ((char)(c - 32));
	return (c);
}

static char	index_digit(unsigned int i, char c)
{
	(void)c;
	return ((char)('0' + (i % 10)));
}

static char	unchanged(unsigned int i, char c)
{
	(void)i;
	return (c);
}

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		char	*p = ft_strmapi("tripouille", to_upper);
		NOT_NULL(p);
		EQ_STR(p, "TRIPOUILLE");
		free(p);
		t_leaks_check();
	}
	CASE(2)
	{
		/* The index starts at 0 and counts every character. */
		char	*p = ft_strmapi("abcdefghijkl", index_digit);
		NOT_NULL(p);
		EQ_STR(p, "012345678901");
		free(p);
		t_leaks_check();
	}
	CASE(3)
	{
		char	*p = ft_strmapi("", to_upper);
		NOT_NULL(p);
		EQ_STR(p, "");
		free(p);
		t_leaks_check();
	}
	CASE(4)
	{
		/* The source is left alone and the result is a new block. */
		char	src[16] = "hello";
		char	*p = ft_strmapi(src, to_upper);
		NOT_NULL(p);
		if (p)
		{
			EQ_STR(src, "hello");
			t_assert(p != src, "ft_strmapi returned the string it was given");
			free(p);
		}
		t_leaks_check();
	}
	CASE(5)
	{
		char	*p = ft_strmapi("42", unchanged);
		NOT_NULL(p);
		EQ_STR(p, "42");
		ALLOC_SIZE(p, 3);
		free(p);
		t_leaks_check();
	}
	return (t_finish());
}
