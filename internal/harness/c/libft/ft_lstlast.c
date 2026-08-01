#include "lst_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
		EQ_PTR(ft_lstlast(NULL), NULL);
	CASE(2)
	{
		/* A single node is its own last. */
		void	*items[] = {(void *)"a"};
		t_list	*lst = chain(items, 1);

		EQ_PTR(ft_lstlast(lst), lst);
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(3)
	{
		void	*items[] = {(void *)"a", (void *)"b", (void *)"c"};
		t_list	*lst = chain(items, 3);

		EQ_PTR(ft_lstlast(lst), lst->next->next);
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(4)
	{
		void	*items[] = {(void *)"a", (void *)"b", (void *)"c"};
		t_list	*lst = chain(items, 3);
		t_list	*last = ft_lstlast(lst);

		NOT_NULL(last);
		if (last)
		{
			EQ_STR((char *)last->content, "c");
			EQ_PTR(last->next, NULL);
		}
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(5)
	{
		/* The answer follows the list when it grows. */
		void	*items[] = {(void *)"a", (void *)"b"};
		t_list	*lst = chain(items, 2);
		t_list	*added = node("c");

		lst->next->next = added;
		EQ_PTR(ft_lstlast(lst), added);
		EQ_INT(length(lst), 3);
		free_nodes(lst);
		t_leaks_check();
	}
	return (t_finish());
}
