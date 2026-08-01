#include "gnl_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		int	fd = open_text("short\nthis one is quite a bit longer\nx\n");
		line(fd, "short\n", "line 1");
		line(fd, "this one is quite a bit longer\n", "line 2");
		line(fd, "x\n", "line 3");
		line(fd, NULL, "line 4");
		close_all();
	}
	CASE(2)
	{
		/* An empty line is a line: it is just a newline on its own. */
		int	fd = open_text("first\n\nthird\n");
		line(fd, "first\n", "line 1");
		line(fd, "\n", "the empty line");
		line(fd, "third\n", "line 3");
		line(fd, NULL, "line 4");
		close_all();
	}
	CASE(3)
	{
		int	fd = open_text("a\nbb\nccccccccccccccccccccccccccccc\ndd\ne\n");
		line(fd, "a\n", "line 1");
		line(fd, "bb\n", "line 2");
		line(fd, "ccccccccccccccccccccccccccccc\n", "line 3");
		line(fd, "dd\n", "line 4");
		line(fd, "e\n", "line 5");
		line(fd, NULL, "line 6");
		close_all();
	}
	CASE(4)
	{
		/* Long and short alternating, which stresses whatever is kept
		 * between calls. */
		char	*big = repeat('L', 4096, 1);
		char	*all;
		int		fd;

		if (big == NULL)
			t_fail("internal: out of memory");
		else
		{
			all = malloc(strlen(big) * 2 + 16);
			if (all == NULL)
				t_fail("internal: out of memory");
			else
			{
				strcpy(all, "s\n");
				strcat(all, big);
				strcat(all, "s\n");
				strcat(all, big);
				fd = open_text(all);
				line(fd, "s\n", "short 1");
				line(fd, big, "long 1");
				line(fd, "s\n", "short 2");
				line(fd, big, "long 2");
				line(fd, NULL, "after all");
				free(all);
				close_all();
			}
			free(big);
		}
	}
	CASE(5)
	{
		/* The last line has no newline, so it comes back without one. */
		int	fd = open_text("one\ntwo\nlast without newline");
		line(fd, "one\n", "line 1");
		line(fd, "two\n", "line 2");
		line(fd, "last without newline", "the last line");
		line(fd, NULL, "after it");
		close_all();
	}
	return (t_finish());
}
