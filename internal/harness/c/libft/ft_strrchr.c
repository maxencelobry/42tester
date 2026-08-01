#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* "tripouille" has an 'i' at 2 and at 6; the later one wins. */
		const char	*s = "tripouille";
		EQ_PTR(ft_strrchr(s, 'i'), s + 6);
	}
	CASE(2)
	{
		/* The last match wins. */
		const char	*s = "abcabc";
		EQ_PTR(ft_strrchr(s, 'a'), s + 3);
		EQ_PTR(ft_strrchr(s, 'c'), s + 5);
	}
	CASE(3)
	{
		const char	*s = "hello";
		EQ_PTR(ft_strrchr(s, '\0'), s + 5);
		t_assert(ft_strrchr("", 0) != NULL, "ft_strrchr(\"\", 0) returned NULL");
	}
	CASE(4)
	{
		EQ_PTR(ft_strrchr("tripouille", 'z'), NULL);
		EQ_PTR(ft_strrchr("", 'a'), NULL);
	}
	CASE(5)
	{
		const char	*s = "tripouille";
		EQ_PTR(ft_strrchr(s, 'i' + 256), s + 6);
		EQ_PTR(ft_strrchr(s, 't'), s + 0);
	}
	return (t_finish());
}
