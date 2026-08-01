#include "libft_test.h"

static unsigned	g_calls;
static unsigned	g_last_index;

static void	upper_in_place(unsigned int i, char *c)
{
	(void)i;
	if (*c >= 'a' && *c <= 'z')
		*c = (char)(*c - 32);
}

static void	write_index(unsigned int i, char *c)
{
	*c = (char)('0' + (i % 10));
}

static void	counter(unsigned int i, char *c)
{
	(void)c;
	g_calls++;
	g_last_index = i;
}

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* The string is modified where it stands: there is no return value. */
		char	s[16] = "tripouille";
		ft_striteri(s, upper_in_place);
		EQ_STR(s, "TRIPOUILLE");
	}
	CASE(2)
	{
		char	s[16] = "abcdefghijkl";
		ft_striteri(s, write_index);
		EQ_STR(s, "012345678901");
	}
	CASE(3)
	{
		/* An empty string means the callback is never called. */
		char	s[4] = "";
		g_calls = 0;
		ft_striteri(s, counter);
		EQ_INT(g_calls, 0);
		EQ_STR(s, "");
	}
	CASE(4)
	{
		/* The callback runs once per character, and not for the terminator. */
		char	s[16] = "hello";
		g_calls = 0;
		g_last_index = 999;
		ft_striteri(s, counter);
		EQ_INT(g_calls, 5);
		EQ_INT(g_last_index, 4);
	}
	CASE(5)
	{
		char	s[8] = "aBcDe";
		ft_striteri(s, upper_in_place);
		EQ_STR(s, "ABCDE");
		t_leaks_check();
	}
	return (t_finish());
}
