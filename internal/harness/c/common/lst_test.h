/* Helpers for the ft_lst* tests.
 *
 * Lists are built by hand rather than with ft_lstnew, so a broken ft_lstnew
 * fails its own group instead of every list group at once.
 */

#ifndef LST_TEST_H
# define LST_TEST_H

# include "libft_test.h"

static T_UNUSED t_list	*node(void *content)
{
	t_list	*n = malloc(sizeof(t_list));

	if (n == NULL)
		return (NULL);
	n->content = content;
	n->next = NULL;
	return (n);
}

/* chain builds a list of n nodes whose contents are the given pointers. */
static T_UNUSED t_list	*chain(void **contents, int n)
{
	t_list	*head = NULL;
	t_list	*tail = NULL;

	for (int i = 0; i < n; i++)
	{
		t_list	*cur = node(contents[i]);
		if (cur == NULL)
			return (head);
		if (head == NULL)
			head = cur;
		else
			tail->next = cur;
		tail = cur;
	}
	return (head);
}

static T_UNUSED int	length(t_list *lst)
{
	int	n = 0;

	while (lst != NULL)
	{
		n++;
		lst = lst->next;
	}
	return (n);
}

/* free_nodes releases the nodes only, leaving their contents alone. */
static T_UNUSED void	free_nodes(t_list *lst)
{
	while (lst != NULL)
	{
		t_list	*next = lst->next;
		free(lst);
		lst = next;
	}
}

#endif
