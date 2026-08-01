#include "printf_test.h"

static int	g_static;

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		int	local = 42;
		CMP("%p", (void *)&local);
	}
	CASE(2)
	{
		/* The libc decides what a null pointer looks like; matching it is
		 * the point. */
		CMP("%p", (void *)0);
		CMP("[%p]", (void *)0);
	}
	CASE(3)
	{
		void	*heap = malloc(16);
		CMP("%p", heap);
		free(heap);
	}
	CASE(4)
	{
		int		a = 1;
		int		b = 2;
		CMP("%p %p", (void *)&a, (void *)&b);
	}
	CASE(5)
		CMP("pointer: %p end", (void *)&g_static);
	CASE(6)
	{
		/* A small non-zero value exercises the leading digits. */
		CMP("%p", (void *)1);
		CMP("%p", (void *)15);
		CMP("%p", (void *)16);
	}
	CASE(7)
	{
		CMP("%p", (void *)255);
		CMP("%p", (void *)4096);
		CMP("%p", (void *)0xdeadbeef);
	}
	CASE(8)
	{
		/* The widest value a pointer can hold. */
		CMP("%p", (void *)(unsigned long)-1);
		CMP("%p%p", (void *)1, (void *)2);
	}
	return (t_finish());
}
