/* Compiled with no -D BUFFER_SIZE at all.
 *
 * The subject requires the project to build both ways, so get_next_line.h
 * has to define a default of its own. If it does not, this group fails to
 * compile, which is the whole point of it. Nothing here may mention
 * BUFFER_SIZE: the tests have to work whatever the header chose.
 */

#include "gnl_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		int	fd = open_text("one\ntwo\nthree\n");
		line(fd, "one\n", "line 1 with the header's own buffer size");
		line(fd, "two\n", "line 2");
		line(fd, "three\n", "line 3");
		line(fd, NULL, "after the last line");
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
		/* Longer than any plausible default, so the growing path runs too. */
		char	*big = repeat('d', 50000, 1);
		int		fd;

		if (big == NULL)
			t_fail("internal: out of memory");
		else
		{
			fd = open_text(big);
			line(fd, big, "a 50000 character line");
			line(fd, NULL, "after it");
			free(big);
			close_all();
		}
	}
	return (t_finish());
}
