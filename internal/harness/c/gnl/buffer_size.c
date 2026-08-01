/* Compiled once per BUFFER_SIZE the moulinette tries. The Go side passes
 * -DBUFFER_SIZE, so the cases below adapt to whatever it is. */

#include "gnl_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

#if BUFFER_SIZE <= 0

	/* With a buffer of zero bytes there is nothing read() can ever deliver,
	 * so the only correct answer is NULL, and it must not loop forever. */
	CASE(1)
	{
		int	fd = open_text("some content\nand more\n");
		line(fd, NULL, "get_next_line with BUFFER_SIZE 0");
		close_all();
	}
	CASE(2)
	{
		int	fd = open_text("");
		line(fd, NULL, "get_next_line on an empty file with BUFFER_SIZE 0");
		close_all();
	}

#else

	CASE(1)
	{
		int	fd = open_text("one\ntwo\nthree\n");
		line(fd, "one\n", "line 1");
		line(fd, "two\n", "line 2");
		line(fd, "three\n", "line 3");
		line(fd, NULL, "line 4");
		close_all();
	}
	CASE(2)
	{
		/* No trailing newline, so the last line comes back bare. */
		int	fd = open_text("alpha\nbeta\ngamma");
		line(fd, "alpha\n", "line 1");
		line(fd, "beta\n", "line 2");
		line(fd, "gamma", "the last line");
		line(fd, NULL, "after it");
		close_all();
	}
	CASE(3)
	{
		/* A line several buffers long, plus one that fits in a single
		 * buffer, so both paths are exercised at this size. */
		char	*big = repeat('z', (size_t)BUFFER_SIZE * 3 + 7, 1);
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
				strcat(all, "end\n");
				fd = open_text(all);
				line(fd, big, "the long line");
				line(fd, "end\n", "the short line after it");
				line(fd, NULL, "after both");
				free(all);
				close_all();
			}
			free(big);
		}
	}

#endif

	return (t_finish());
}
