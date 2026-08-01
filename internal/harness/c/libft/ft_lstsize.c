#include "lst_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
		EQ_INT(ft_lstsize(NULL), 0);
	CASE(2)
	{
		void	*items[] = {(void *)"a"};
		t_list	*lst = chain(items, 1);

		EQ_INT(ft_lstsize(lst), 1);
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(3)
	{
		void	*items[] = {(void *)"a", (void *)"b", (void *)"c"};
		t_list	*lst = chain(items, 3);

		EQ_INT(ft_lstsize(lst), 3);
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(4)
	{
		/* A node whose content is NULL still counts. */
		void	*items[] = {NULL, NULL, NULL, NULL};
		t_list	*lst = chain(items, 4);

		EQ_INT(ft_lstsize(lst), 4);
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(5)
	{
		/* Counting must not stop early on a long list, and must not modify
		 * it either. */
		void	*items[64];
		for (int i = 0; i < 64; i++)
			items[i] = (void *)"x";
		t_list	*lst = chain(items, 64);

		EQ_INT(ft_lstsize(lst), 64);
		EQ_INT(ft_lstsize(lst), 64);
		EQ_INT(length(lst), 64);
		free_nodes(lst);
		t_leaks_check();
	}
	return (t_finish());
}
