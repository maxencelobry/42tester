#include "lst_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		char	*content = strdup("tripouille");
		t_list	*n = ft_lstnew(content);
		NOT_NULL(n);
		if (n)
		{
			EQ_PTR(n->content, content);
			EQ_PTR(n->next, NULL);
			free(n);
		}
		free(content);
		t_leaks_check();
	}
	CASE(2)
	{
		/* A NULL content is a valid node, not a reason to return NULL. */
		t_list	*n = ft_lstnew(NULL);
		NOT_NULL(n);
		if (n)
		{
			EQ_PTR(n->content, NULL);
			EQ_PTR(n->next, NULL);
			free(n);
		}
		t_leaks_check();
	}
	CASE(3)
	{
		t_list	*n = ft_lstnew(NULL);
		NOT_NULL(n);
		ALLOC_SIZE(n, sizeof(t_list));
		free(n);
		t_leaks_check();
	}
	CASE(4)
	{
		/* Two calls give two independent nodes. */
		t_list	*a = ft_lstnew(NULL);
		t_list	*b = ft_lstnew(NULL);
		NOT_NULL(a);
		NOT_NULL(b);
		if (a && b)
			t_assert(a != b, "ft_lstnew returned the same node twice");
		free(a);
		free(b);
		t_leaks_check();
	}
	CASE(5)
	{
		/* The content is stored as given, never copied. */
		char	buf[16] = "hello";
		t_list	*n = ft_lstnew(buf);
		NOT_NULL(n);
		if (n)
		{
			EQ_PTR(n->content, buf);
			buf[0] = 'H';
			EQ_STR((char *)n->content, "Hello");
			free(n);
		}
		t_leaks_check();
	}
	return (t_finish());
}
