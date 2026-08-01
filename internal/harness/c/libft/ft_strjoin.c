#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		char	*p = ft_strjoin("tri", "pouille");
		NOT_NULL(p);
		EQ_STR(p, "tripouille");
		free(p);
		t_leaks_check();
	}
	CASE(2)
	{
		char	*p = ft_strjoin("", "pouille");
		NOT_NULL(p);
		EQ_STR(p, "pouille");
		free(p);
		t_leaks_check();
	}
	CASE(3)
	{
		char	*p = ft_strjoin("tri", "");
		NOT_NULL(p);
		EQ_STR(p, "tri");
		free(p);
		t_leaks_check();
	}
	CASE(4)
	{
		char	*p = ft_strjoin("", "");
		NOT_NULL(p);
		EQ_STR(p, "");
		free(p);
		t_leaks_check();
	}
	CASE(5)
	{
		/* Exactly both lengths plus one terminator, and a fresh block that
		 * does not alias either input. */
		char	*p = ft_strjoin("42", "42");
		NOT_NULL(p);
		ALLOC_SIZE(p, 5);
		free(p);

		char	a[8] = "abc";
		char	b[8] = "def";
		p = ft_strjoin(a, b);
		NOT_NULL(p);
		if (p)
		{
			a[0] = 'X';
			b[0] = 'Y';
			EQ_STR(p, "abcdef");
			free(p);
		}
		t_leaks_check();
	}
	return (t_finish());
}
