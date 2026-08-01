#include "gnl_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* A line far longer than any buffer must still come back whole. */
		char	*want = repeat('a', 10000, 1);
		int		fd;

		if (want == NULL)
			t_fail("internal: out of memory");
		else
		{
			fd = open_text(want);
			line(fd, want, "a 10000 character line");
			line(fd, NULL, "after it");
			free(want);
			close_all();
		}
	}
	CASE(2)
	{
		/* The same without a trailing newline. */
		char	*want = repeat('b', 10000, 0);
		int		fd;

		if (want == NULL)
			t_fail("internal: out of memory");
		else
		{
			fd = open_text(want);
			line(fd, want, "a 10000 character line with no newline");
			line(fd, NULL, "after it");
			free(want);
			close_all();
		}
	}
	CASE(3)
	{
		char	*first = repeat('x', 5000, 1);
		char	*second = repeat('y', 5000, 1);
		char	*both;
		int		fd;

		if (first == NULL || second == NULL)
			t_fail("internal: out of memory");
		else
		{
			both = malloc(strlen(first) + strlen(second) + 1);
			if (both == NULL)
				t_fail("internal: out of memory");
			else
			{
				strcpy(both, first);
				strcat(both, second);
				fd = open_text(both);
				line(fd, first, "the first long line");
				line(fd, second, "the second long line");
				line(fd, NULL, "after both");
				free(both);
				close_all();
			}
		}
		free(first);
		free(second);
	}
	CASE(4)
	{
		/* A long line followed by a short one: the leftover buffer must be
		 * handled correctly. */
		char	*big = repeat('z', 8192, 1);
		char	*all;
		int		fd;

		if (big == NULL)
			t_fail("internal: out of memory");
		else
		{
			all = malloc(strlen(big) + 8);
			if (all == NULL)
				t_fail("internal: out of memory");
			else
			{
				strcpy(all, big);
				strcat(all, "tiny\n");
				fd = open_text(all);
				line(fd, big, "the long line");
				line(fd, "tiny\n", "the short line after it");
				line(fd, NULL, "after both");
				free(all);
				close_all();
			}
		}
		free(big);
	}
	CASE(5)
	{
		/* Large enough that a naive implementation reallocating one byte at
		 * a time will time out. */
		char	*want = repeat('q', 100000, 1);
		int		fd;

		if (want == NULL)
			t_fail("internal: out of memory");
		else
		{
			fd = open_text(want);
			line(fd, want, "a 100000 character line");
			line(fd, NULL, "after it");
			free(want);
			close_all();
		}
	}
	return (t_finish());
}
