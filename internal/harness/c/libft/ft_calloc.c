#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* Every byte must be zero, which is what separates calloc from
		 * malloc. */
		unsigned char	*p = ft_calloc(64, 1);
		NOT_NULL(p);
		if (p)
		{
			for (int i = 0; i < 64; i++)
				t_assert(p[i] == 0, "ft_calloc left byte %d set to %d", i, p[i]);
			free(p);
		}
		t_leaks_check();
	}
	CASE(2)
	{
		/* The block is count * size bytes, not count bytes. */
		int	*p = ft_calloc(10, sizeof(int));
		NOT_NULL(p);
		if (p)
		{
			for (int i = 0; i < 10; i++)
				t_assert(p[i] == 0, "ft_calloc left element %d set to %d", i, p[i]);
			ALLOC_SIZE(p, 10 * sizeof(int));
			memset(p, 0xff, 10 * sizeof(int));
			free(p);
		}
		t_leaks_check();
	}
	CASE(3)
	{
		/* Zero bytes still gives a pointer that can be freed. */
		void	*p = ft_calloc(0, 0);
		NOT_NULL(p);
		free(p);
		p = ft_calloc(0, 42);
		NOT_NULL(p);
		free(p);
		t_leaks_check();
	}
	CASE(4)
	{
		char	*p = ft_calloc(1024, 1);
		NOT_NULL(p);
		if (p)
		{
			for (int i = 0; i < 1024; i++)
				t_assert(p[i] == 0, "ft_calloc left byte %d set to %d", i, p[i]);
			free(p);
		}
		t_leaks_check();
	}
	CASE(5)
	{
		/* A request this large cannot succeed; returning a pointer would
		 * mean the multiplication overflowed. */
		void	*p = ft_calloc((size_t)-1, (size_t)-1);
		EQ_PTR(p, NULL);
		free(p);
		t_leaks_check();
	}
	return (t_finish());
}
