#include "lst_test.h"

static int	g_del_calls;

/* upper returns a fresh uppercase copy, which is what f is meant to do:
 * produce content the new list owns. */
static void	*upper(void *content)
{
	char	*copy = strdup((char *)content);

	if (copy == NULL)
		return (NULL);
	for (char *p = copy; *p; p++)
		if (*p >= 'a' && *p <= 'z')
			*p = (char)(*p - 32);
	return (copy);
}

static void	del(void *content)
{
	g_del_calls++;
	free(content);
}

static void	free_mapped(t_list *lst)
{
	while (lst != NULL)
	{
		t_list	*next = lst->next;
		free(lst->content);
		free(lst);
		lst = next;
	}
}

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		void	*items[] = {(void *)"a", (void *)"b", (void *)"c"};
		t_list	*lst = chain(items, 3);
		t_list	*mapped = ft_lstmap(lst, upper, del);

		NOT_NULL(mapped);
		if (mapped && length(mapped) == 3)
		{
			EQ_STR((char *)mapped->content, "A");
			EQ_STR((char *)mapped->next->content, "B");
			EQ_STR((char *)mapped->next->next->content, "C");
		}
		free_mapped(mapped);
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(2)
	{
		/* The new list has the same length as the old one. */
		void	*items[] = {(void *)"x", (void *)"y", (void *)"z", (void *)"w"};
		t_list	*lst = chain(items, 4);
		t_list	*mapped = ft_lstmap(lst, upper, del);

		EQ_INT(length(mapped), 4);
		free_mapped(mapped);
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(3)
	{
		/* The source list is left untouched, nodes and contents alike. */
		char	a[8] = "hello";
		void	*items[] = {a};
		t_list	*lst = chain(items, 1);
		t_list	*head = lst;
		t_list	*mapped = ft_lstmap(lst, upper, del);

		EQ_PTR(lst, head);
		EQ_STR(a, "hello");
		if (mapped)
			t_assert(mapped != lst, "ft_lstmap returned the list it was given");
		free_mapped(mapped);
		free_nodes(lst);
		t_leaks_check();
	}
	CASE(4)
	{
		/* Nothing to map gives nothing back. */
		EQ_PTR(ft_lstmap(NULL, upper, del), NULL);
		t_leaks_check();
	}
	CASE(5)
	{
		/* The mapped contents are separate blocks, so freeing one list does
		 * not disturb the other. */
		char	a[8] = "one";
		char	b[8] = "two";
		void	*items[] = {a, b};
		t_list	*lst = chain(items, 2);
		t_list	*mapped = ft_lstmap(lst, upper, del);

		NOT_NULL(mapped);
		if (mapped && length(mapped) == 2)
		{
			t_assert(mapped->content != (void *)a,
				"ft_lstmap reused the content of the source list");
			free_mapped(mapped);
			EQ_STR(a, "one");
			EQ_STR(b, "two");
		}
		else
			free_mapped(mapped);
		free_nodes(lst);
		t_leaks_check();
	}
	return (t_finish());
}
