#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		char	*p = ft_substr("tripouille", 3, 4);
		NOT_NULL(p);
		EQ_STR(p, "poui");
		free(p);
		t_leaks_check();
	}
	CASE(2)
	{
		char	*p = ft_substr("tripouille", 0, 4);
		NOT_NULL(p);
		EQ_STR(p, "trip");
		free(p);
		t_leaks_check();
	}
	CASE(3)
	{
		/* A start past the end gives an empty string, not NULL, and must not
		 * read out of bounds. */
		char	*p = ft_substr("tripouille", 42, 5);
		NOT_NULL(p);
		EQ_STR(p, "");
		free(p);
		t_leaks_check();
	}
	CASE(4)
	{
		/* len larger than what is left is clipped to the end. */
		char	*p = ft_substr("tripouille", 6, 100);
		NOT_NULL(p);
		EQ_STR(p, "ille");
		free(p);
		t_leaks_check();
	}
	CASE(5)
	{
		char	*p = ft_substr("tripouille", 3, 0);
		NOT_NULL(p);
		EQ_STR(p, "");
		free(p);
		t_leaks_check();
	}
	CASE(6)
	{
		char	*p = ft_substr("", 0, 5);
		NOT_NULL(p);
		EQ_STR(p, "");
		free(p);
		t_leaks_check();
	}
	CASE(7)
	{
		/* Clipping must shrink the allocation too, not just the string. */
		char	*p = ft_substr("tripouille", 6, 100);
		NOT_NULL(p);
		ALLOC_SIZE(p, 5);
		free(p);
		t_leaks_check();
	}
	CASE(8)
	{
		char	*p = ft_substr("tripouille", 9, 1);
		NOT_NULL(p);
		EQ_STR(p, "e");
		free(p);

		p = ft_substr("tripouille", 10, 1);
		NOT_NULL(p);
		EQ_STR(p, "");
		free(p);
		t_leaks_check();
	}
	CASE(9)
	{
		/* A huge len must not turn into a huge malloc. */
		char	*p = ft_substr("42", 0, (size_t)-1);
		NOT_NULL(p);
		EQ_STR(p, "42");
		ALLOC_SIZE(p, 3);
		free(p);
		t_leaks_check();
	}
	CASE(10)
	{
		/* The result is a fresh block, independent from the source. */
		char	src[16] = "tripouille";
		char	*p = ft_substr(src, 0, 10);
		NOT_NULL(p);
		if (p)
		{
			t_assert((void *)p != (void *)src,
				"ft_substr returned a pointer into the source string");
			src[0] = 'X';
			EQ_STR(p, "tripouille");
			free(p);
		}
		t_leaks_check();
	}
	return (t_finish());
}
