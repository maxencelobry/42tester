#include "lst_test.h"

static int		g_calls;
static char		g_seen[64];
static int		g_seen_len;

static void	record(void *content)
{
	g_calls++;
	if (content != NULL && g_seen_len < (int)sizeof(g_seen) - 1)
		g_seen[g_seen_len++] = ((char *)content)[0];
}

static void	shout(void *content)
{
	char	*s = content;

	while (*s)
	{
		if (*s >= 'a' && *s <= 'z')
			*s = (char)(*s - 32);
		s++;
	}
}

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* The callback runs once per node. */
		void	*items[] = {(void *)"a", (void *)"b", (void *)"c"};
		t_list	*lst = chain(items, 3);

		g_calls = 0;
		g_seen_len = 0;
		ft_lstiter(lst, record);
		EQ_INT(g_calls, 3);
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(2)
	{
		/* And it runs from the head to the tail, in that order. */
		void	*items[] = {(void *)"a", (void *)"b", (void *)"c"};
		t_list	*lst = chain(items, 3);

		g_calls = 0;
		g_seen_len = 0;
		memset(g_seen, 0, sizeof(g_seen));
		ft_lstiter(lst, record);
		EQ_STR(g_seen, "abc");
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(3)
	{
		/* The callback gets the content, not the node, so writing through
		 * it changes the list. */
		char	a[8] = "hello";
		char	b[8] = "world";
		void	*items[] = {a, b};
		t_list	*lst = chain(items, 2);

		ft_lstiter(lst, shout);
		EQ_STR(a, "HELLO");
		EQ_STR(b, "WORLD");
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(4)
	{
		/* An empty list means no calls and no crash. */
		g_calls = 0;
		ft_lstiter(NULL, record);
		EQ_INT(g_calls, 0);
		t_leaks_check();
	}
	CASE(5)
	{
		/* The list itself is left exactly as it was. */
		void	*items[] = {(void *)"a", (void *)"b", (void *)"c", (void *)"d"};
		t_list	*lst = chain(items, 4);
		t_list	*head = lst;

		g_calls = 0;
		ft_lstiter(lst, record);
		EQ_INT(g_calls, 4);
		EQ_PTR(lst, head);
		EQ_INT(length(lst), 4);
		free_nodes(lst);
		t_leaks_check();
	}
	return (t_finish());
}
