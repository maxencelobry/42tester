#include "lst_test.h"

static int	g_del_calls;

static void	del(void *content)
{
	g_del_calls++;
	free(content);
}

/* build_owned makes a list whose contents are allocated, so del has
 * something real to free. */
static t_list	*build_owned(int n)
{
	t_list	*lst = NULL;
	t_list	*tail = NULL;

	for (int i = 0; i < n; i++)
	{
		t_list	*cur = node(strdup("content"));
		if (lst == NULL)
			lst = cur;
		else
			tail->next = cur;
		tail = cur;
	}
	return (lst);
}

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* The whole list goes and the caller pointer is set to NULL. */
		t_list	*lst = build_owned(3);

		g_del_calls = 0;
		ft_lstclear(&lst, del);
		EQ_INT(g_del_calls, 3);
		EQ_PTR(lst, NULL);
		t_leaks_check();
	}
	CASE(2)
	{
		t_list	*lst = build_owned(10);

		g_del_calls = 0;
		ft_lstclear(&lst, del);
		EQ_INT(g_del_calls, 10);
		EQ_PTR(lst, NULL);
		t_leaks_check();
	}
	CASE(3)
	{
		/* An already empty list is a no-op, not a crash. */
		t_list	*lst = NULL;

		g_del_calls = 0;
		ft_lstclear(&lst, del);
		EQ_INT(g_del_calls, 0);
		EQ_PTR(lst, NULL);
		t_leaks_check();
	}
	CASE(4)
	{
		t_list	*lst = build_owned(1);

		g_del_calls = 0;
		ft_lstclear(&lst, del);
		EQ_INT(g_del_calls, 1);
		EQ_PTR(lst, NULL);
		t_leaks_check();
	}
	CASE(5)
	{
		/* Clearing from the middle leaves the head intact. */
		t_list	*lst = build_owned(3);
		t_list	*second = lst->next;

		g_del_calls = 0;
		ft_lstclear(&second, del);
		EQ_INT(g_del_calls, 2);
		EQ_PTR(second, NULL);
		lst->next = NULL;
		EQ_INT(length(lst), 1);
		ft_lstclear(&lst, del);
		t_leaks_check();
	}
	return (t_finish());
}
