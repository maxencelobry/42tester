#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		const char	*s = "tripouille";
		EQ_PTR(ft_strchr(s, 'p'), s + 3);
	}
	CASE(2)
	{
		/* The first match wins, not the last. */
		const char	*s = "abcabc";
		EQ_PTR(ft_strchr(s, 'b'), s + 1);
		EQ_PTR(ft_strchr(s, 'a'), s + 0);
	}
	CASE(3)
	{
		/* Looking for the terminator returns a pointer to it. */
		const char	*s = "hello";
		const char	*empty = "";
		EQ_PTR(ft_strchr(s, '\0'), s + 5);
		EQ_PTR(ft_strchr(empty, '\0'), empty);
	}
	CASE(4)
	{
		EQ_PTR(ft_strchr("tripouille", 'z'), NULL);
		EQ_PTR(ft_strchr("", 'a'), NULL);
	}
	CASE(5)
	{
		/* The character is an int but compared as a char, so 'a' + 256 is
		 * still 'a'. */
		const char	*s = "tripouille";
		EQ_PTR(ft_strchr(s, 'p' + 256), s + 3);
		EQ_PTR(ft_strchr(s, (char)-1), NULL);
	}
	return (t_finish());
}
