/* get_next_line must work on descriptor 0 like any other. The test puts a
 * real file behind it rather than relying on the parent to pipe anything in. */

#include "gnl_test.h"

/* redirect_stdin points descriptor 0 at fd and returns the saved original. */
static int	redirect_stdin(int fd)
{
	int	saved;

	if (fd < 0)
		return (-1);
	saved = t_dup(0);
	t_dup2(fd, 0);
	return (saved);
}

static void	restore_stdin(int saved)
{
	if (saved < 0)
		return ;
	t_dup2(saved, 0);
	t_close(saved);
}

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		int	fd = open_text("from stdin\n");
		int	saved = redirect_stdin(fd);

		line(0, "from stdin\n", "get_next_line(0)");
		line(0, NULL, "after it");
		restore_stdin(saved);
		close_all();
	}
	CASE(2)
	{
		int	fd = open_text("one\ntwo\nthree\n");
		int	saved = redirect_stdin(fd);

		line(0, "one\n", "line 1 from stdin");
		line(0, "two\n", "line 2 from stdin");
		line(0, "three\n", "line 3 from stdin");
		line(0, NULL, "line 4 from stdin");
		restore_stdin(saved);
		close_all();
	}
	CASE(3)
	{
		/* Nothing on stdin means NULL, not a hang. */
		int	fd = open_text("");
		int	saved = redirect_stdin(fd);

		line(0, NULL, "get_next_line(0) on empty input");
		restore_stdin(saved);
		close_all();
	}
	return (t_finish());
}
