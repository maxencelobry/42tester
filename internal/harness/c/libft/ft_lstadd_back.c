#include "lst_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* Adding to an empty list makes the new node the whole list. */
		t_list	*lst = NULL;
		t_list	*n = node("a");

		ft_lstadd_back(&lst, n);
		EQ_PTR(lst, n);
		if (lst)
			EQ_PTR(lst->next, NULL);
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(2)
	{
		void	*items[] = {(void *)"a"};
		t_list	*lst = chain(items, 1);
		t_list	*head = lst;
		t_list	*n = node("b");

		ft_lstadd_back(&lst, n);
		EQ_PTR(lst, head);
		EQ_PTR(lst->next, n);
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(3)
	{
		/* Three additions come out in the order they were made. */
		t_list	*lst = NULL;

		ft_lstadd_back(&lst, node("a"));
		ft_lstadd_back(&lst, node("b"));
		ft_lstadd_back(&lst, node("c"));
		EQ_INT(length(lst), 3);
		if (length(lst) == 3)
		{
			EQ_STR((char *)lst->content, "a");
			EQ_STR((char *)lst->next->content, "b");
			EQ_STR((char *)lst->next->next->content, "c");
			EQ_PTR(lst->next->next->next, NULL);
		}
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(4)
	{
		void	*items[] = {(void *)"a", (void *)"b", (void *)"c"};
		t_list	*lst = chain(items, 3);

		ft_lstadd_back(&lst, node("d"));
		EQ_INT(length(lst), 4);
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(5)
	{
		/* The head keeps its identity and its content. */
		void	*items[] = {(void *)"a", (void *)"b"};
		t_list	*lst = chain(items, 2);
		t_list	*head = lst;

		ft_lstadd_back(&lst, node("c"));
		EQ_PTR(lst, head);
		EQ_STR((char *)lst->content, "a");
		free_nodes(lst);
		t_leaks_check();
	}
	return (t_finish());
}
