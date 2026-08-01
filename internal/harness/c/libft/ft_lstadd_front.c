#include "lst_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* Adding to an empty list makes the new node the whole list. */
		t_list	*lst = NULL;
		t_list	*n = node("a");

		ft_lstadd_front(&lst, n);
		EQ_PTR(lst, n);
		if (lst)
			EQ_PTR(lst->next, NULL);
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(2)
	{
		void	*items[] = {(void *)"b", (void *)"c"};
		t_list	*lst = chain(items, 2);
		t_list	*old_head = lst;
		t_list	*n = node("a");

		ft_lstadd_front(&lst, n);
		EQ_PTR(lst, n);
		if (lst)
			EQ_PTR(lst->next, old_head);
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(3)
	{
		/* Three additions come out in reverse order. */
		t_list	*lst = NULL;

		ft_lstadd_front(&lst, node("c"));
		ft_lstadd_front(&lst, node("b"));
		ft_lstadd_front(&lst, node("a"));
		EQ_INT(length(lst), 3);
		if (length(lst) == 3)
		{
			EQ_STR((char *)lst->content, "a");
			EQ_STR((char *)lst->next->content, "b");
			EQ_STR((char *)lst->next->next->content, "c");
		}
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(4)
	{
		/* The list grows by exactly one node. */
		void	*items[] = {(void *)"x", (void *)"y", (void *)"z"};
		t_list	*lst = chain(items, 3);

		ft_lstadd_front(&lst, node("w"));
		EQ_INT(length(lst), 4);
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(5)
	{
		/* The tail is untouched: the last node still ends the list. */
		void	*items[] = {(void *)"b"};
		t_list	*lst = chain(items, 1);
		t_list	*tail = lst;

		ft_lstadd_front(&lst, node("a"));
		EQ_PTR(tail->next, NULL);
		EQ_STR((char *)tail->content, "b");
		free_nodes(lst);
		t_leaks_check();
	}
	return (t_finish());
}
