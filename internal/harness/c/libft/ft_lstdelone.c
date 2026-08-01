#include "lst_test.h"

static int		g_del_calls;
static void		*g_del_last;

static void	del(void *content)
{
	g_del_calls++;
	g_del_last = content;
	free(content);
}

static void	del_noop(void *content)
{
	g_del_calls++;
	g_del_last = content;
}

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* del receives the content pointer, exactly once. */
		char	*content = strdup("tripouille");
		t_list	*n = node(content);

		g_del_calls = 0;
		g_del_last = NULL;
		ft_lstdelone(n, del);
		EQ_INT(g_del_calls, 1);
		EQ_PTR(g_del_last, content);
		t_leaks_check();
	}
	CASE(2)
	{
		/* The node itself is freed too: with leak tracking on, anything
		 * still held here is a leak. */
		t_list	*n = node(strdup("x"));

		g_del_calls = 0;
		ft_lstdelone(n, del);
		EQ_INT(g_del_calls, 1);
		t_leaks_check();
	}
	CASE(3)
	{
		/* Only this node goes: the rest of the list survives. */
		void	*items[] = {(void *)"a", (void *)"b"};
		t_list	*lst = chain(items, 2);
		t_list	*second = lst->next;

		g_del_calls = 0;
		ft_lstdelone(lst, del_noop);
		EQ_INT(g_del_calls, 1);
		EQ_STR((char *)second->content, "b");
		EQ_PTR(second->next, NULL);
		free_nodes(second);
		t_leaks_check();
	}
	CASE(4)
	{
		/* A NULL content is still handed to del. */
		t_list	*n = node(NULL);

		g_del_calls = 0;
		g_del_last = (void *)1;
		ft_lstdelone(n, del_noop);
		EQ_INT(g_del_calls, 1);
		EQ_PTR(g_del_last, NULL);
		t_leaks_check();
	}
	CASE(5)
	{
		/* Deleting several nodes one by one leaves nothing behind. */
		g_del_calls = 0;
		for (int i = 0; i < 5; i++)
			ft_lstdelone(node(strdup("x")), del);
		EQ_INT(g_del_calls, 5);
		t_leaks_check();
	}
	return (t_finish());
}
