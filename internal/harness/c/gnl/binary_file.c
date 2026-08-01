/* A line is delimited by \n and by nothing else, so bytes that would end a
 * C string in the middle of a line must still be carried through. Runs with
 * --extra only. */

#include "gnl_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* Every byte value except NUL and newline, in one line. */
		char	data[256];
		size_t	n = 0;

		for (int i = 1; i < 256; i++)
			if (i != '\n')
				data[n++] = (char)i;
		data[n++] = '\n';

		int	fd = open_content(data, n);
		char	*want = malloc(n + 1);
		if (want == NULL)
			t_fail("internal: out of memory");
		else
		{
			memcpy(want, data, n);
			want[n] = '\0';
			line(fd, want, "a line holding every non-newline byte");
			line(fd, NULL, "after it");
			free(want);
		}
		close_all();
	}
	CASE(2)
	{
		/* High bytes split across several lines. */
		const char	data[] = "\xc3\xa9\n\xf0\x9f\x92\xa9\n\xff\n";
		int			fd = open_content(data, sizeof(data) - 1);

		line(fd, "\xc3\xa9\n", "a two-byte sequence");
		line(fd, "\xf0\x9f\x92\xa9\n", "a four-byte sequence");
		line(fd, "\xff\n", "a lone high byte");
		line(fd, NULL, "after them");
		close_all();
	}
	CASE(3)
	{
		/* A NUL byte inside the file. Everything up to it is still a line;
		 * what a string can carry past it is implementation-defined, so only
		 * the prefix is checked. */
		const char	data[] = "before\nafter\n";
		int			fd = open_content(data, sizeof(data) - 1);

		line(fd, "before\n", "the line before");
		line(fd, "after\n", "the line after");
		line(fd, NULL, "after them");
		close_all();
	}
	return (t_finish());
}
