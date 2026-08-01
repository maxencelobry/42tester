#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		char	*p = ft_strtrim("   tripouille   ", " ");
		NOT_NULL(p);
		EQ_STR(p, "tripouille");
		free(p);
		t_leaks_check();
	}
	CASE(2)
	{
		/* Nothing to trim still returns a copy, never the original pointer. */
		char	*p = ft_strtrim("tripouille", "xyz");
		NOT_NULL(p);
		EQ_STR(p, "tripouille");
		free(p);
		t_leaks_check();
	}
	CASE(3)
	{
		/* Every character is in the set, so nothing is left. */
		char	*p = ft_strtrim("aaabbb", "ab");
		NOT_NULL(p);
		EQ_STR(p, "");
		free(p);
		t_leaks_check();
	}
	CASE(4)
	{
		char	*p = ft_strtrim("  tripouille  ", "");
		NOT_NULL(p);
		EQ_STR(p, "  tripouille  ");
		free(p);

		p = ft_strtrim("", " ");
		NOT_NULL(p);
		EQ_STR(p, "");
		free(p);
		t_leaks_check();
	}
	CASE(5)
	{
		/* Several characters in the set, and only the ends are touched. */
		char	*p = ft_strtrim("\t\n 42 is here \n\t", " \t\n");
		NOT_NULL(p);
		EQ_STR(p, "42 is here");
		ALLOC_SIZE(p, 11);
		free(p);
		t_leaks_check();
	}
	return (t_finish());
}
