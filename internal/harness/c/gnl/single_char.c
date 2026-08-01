#include "gnl_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		int	fd = open_text("a");
		line(fd, "a", "a lone character with no newline");
		line(fd, NULL, "after it");
		close_all();
	}
	CASE(2)
	{
		int	fd = open_text("a\n");
		line(fd, "a\n", "a lone character with a newline");
		line(fd, NULL, "after it");
		close_all();
	}
	CASE(3)
	{
		/* A file holding nothing but a newline is one empty line. */
		int	fd = open_text("\n");
		line(fd, "\n", "a lone newline");
		line(fd, NULL, "after it");
		close_all();
	}
	return (t_finish());
}
