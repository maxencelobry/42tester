#include "gnl_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* Calling again well past the end must keep returning NULL rather
		 * than repeating the last line or crashing. */
		int	fd = open_text("one\n");
		line(fd, "one\n", "the only line");
		for (int i = 0; i < 10; i++)
			line(fd, NULL, "a call past the end");
		close_all();
	}
	CASE(2)
	{
		/* A line whose length is exactly the buffer size, with the newline
		 * landing on the boundary. */
		char	*want = repeat('a', BUFFER_SIZE > 0 ? BUFFER_SIZE : 1, 1);
		int		fd;

		if (want == NULL)
			t_fail("internal: out of memory");
		else
		{
			fd = open_text(want);
			line(fd, want, "a line exactly BUFFER_SIZE long");
			line(fd, NULL, "after it");
			free(want);
			close_all();
		}
	}
	CASE(3)
	{
		/* One byte short of the buffer, and one byte over. */
		size_t	n = BUFFER_SIZE > 1 ? (size_t)BUFFER_SIZE : 2;
		char	*shorter = repeat('b', n - 1, 1);
		char	*longer = repeat('c', n + 1, 1);
		int		fd;

		if (shorter == NULL || longer == NULL)
			t_fail("internal: out of memory");
		else
		{
			fd = open_text(shorter);
			line(fd, shorter, "a line one byte shorter than the buffer");
			close_all();
			fd = open_text(longer);
			line(fd, longer, "a line one byte longer than the buffer");
			close_all();
		}
		free(shorter);
		free(longer);
	}
	CASE(4)
	{
		/* Content that has no newline at all and is larger than the buffer. */
		char	*want = repeat('d', 3000, 0);
		int		fd;

		if (want == NULL)
			t_fail("internal: out of memory");
		else
		{
			fd = open_text(want);
			line(fd, want, "3000 characters with no newline");
			line(fd, NULL, "after it");
			free(want);
			close_all();
		}
	}
	CASE(5)
	{
		/* Bytes that are not text must pass through untouched, as long as
		 * they are not NUL: a line is delimited by \n and nothing else. */
		const char	data[] = "\x01\x02\x03\n\xfe\xff\n";
		int			fd = open_content(data, sizeof(data) - 1);

		line(fd, "\x01\x02\x03\n", "a line of control bytes");
		line(fd, "\xfe\xff\n", "a line of high bytes");
		line(fd, NULL, "after them");
		close_all();
	}
	return (t_finish());
}
