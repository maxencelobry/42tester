#include "libft_test.h"

/* count walks the NULL-terminated array the function returned. */
static int	count(char **tab)
{
	int	n = 0;

	if (tab == NULL)
		return (-1);
	while (tab[n] != NULL)
		n++;
	return (n);
}

static void	free_tab(char **tab)
{
	if (tab == NULL)
		return ;
	for (int i = 0; tab[i] != NULL; i++)
		free(tab[i]);
	free(tab);
}

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		char	**tab = ft_split("hello world 42", ' ');
		NOT_NULL(tab);
		if (tab)
		{
			EQ_INT(count(tab), 3);
			if (count(tab) == 3)
			{
				EQ_STR(tab[0], "hello");
				EQ_STR(tab[1], "world");
				EQ_STR(tab[2], "42");
			}
			free_tab(tab);
		}
		t_leaks_check();
	}
	CASE(2)
	{
		/* Repeated, leading and trailing separators produce no empty words. */
		char	**tab = ft_split("  hello   world  ", ' ');
		NOT_NULL(tab);
		if (tab)
		{
			EQ_INT(count(tab), 2);
			if (count(tab) == 2)
			{
				EQ_STR(tab[0], "hello");
				EQ_STR(tab[1], "world");
			}
			free_tab(tab);
		}
		t_leaks_check();
	}
	CASE(3)
	{
		char	**tab = ft_split("tripouille", ' ');
		NOT_NULL(tab);
		if (tab)
		{
			EQ_INT(count(tab), 1);
			if (count(tab) == 1)
				EQ_STR(tab[0], "tripouille");
			free_tab(tab);
		}
		t_leaks_check();
	}
	CASE(4)
	{
		/* An empty input gives an array holding only the NULL terminator. */
		char	**tab = ft_split("", ' ');
		NOT_NULL(tab);
		if (tab)
		{
			EQ_INT(count(tab), 0);
			free_tab(tab);
		}
		t_leaks_check();
	}
	CASE(5)
	{
		char	**tab = ft_split("     ", ' ');
		NOT_NULL(tab);
		if (tab)
		{
			EQ_INT(count(tab), 0);
			free_tab(tab);
		}

		/* Runs of separators of different lengths, a classic source of
		 * empty words and off-by-one word counts. */
		tab = ft_split("--1-2--3---4----5-----42", '-');
		NOT_NULL(tab);
		if (tab)
		{
			EQ_INT(count(tab), 6);
			if (count(tab) == 6)
			{
				EQ_STR(tab[0], "1");
				EQ_STR(tab[4], "5");
				EQ_STR(tab[5], "42");
			}
			free_tab(tab);
		}
		t_leaks_check();
	}
	return (t_finish());
}
