#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		char	*p = ft_strdup("tripouille");
		NOT_NULL(p);
		EQ_STR(p, "tripouille");
		free(p);
		t_leaks_check();
	}
	CASE(2)
	{
		char	*p = ft_strdup("");
		NOT_NULL(p);
		EQ_STR(p, "");
		free(p);
		t_leaks_check();
	}
	CASE(3)
	{
		/* The copy must be independent: writing to it must not touch the
		 * original. */
		char	src[16] = "hello";
		char	*p = ft_strdup(src);
		NOT_NULL(p);
		if (p)
		{
			t_assert(p != src, "ft_strdup returned the pointer it was given");
			p[0] = 'H';
			EQ_STR(src, "hello");
			free(p);
		}
		t_leaks_check();
	}
	CASE(4)
	{
		char	big[513];
		memset(big, 'a', 512);
		big[512] = '\0';
		char	*p = ft_strdup(big);
		NOT_NULL(p);
		EQ_STR(p, big);
		free(p);
		t_leaks_check();
	}
	CASE(5)
	{
		/* The block is exactly the string plus its terminator. */
		char	*p = ft_strdup("42");
		NOT_NULL(p);
		ALLOC_SIZE(p, 3);
		free(p);
		t_leaks_check();
	}
	return (t_finish());
}
