#include "gnl_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* No newline anywhere: the whole file is one line, returned as is. */
		int	fd = open_text("no newline here");
		line(fd, "no newline here", "the only line");
		line(fd, NULL, "after it");
		close_all();
	}
	CASE(2)
	{
		int	fd = open_text("first\nsecond\nthird");
		line(fd, "first\n", "line 1");
		line(fd, "second\n", "line 2");
		line(fd, "third", "the last line, with no newline");
		line(fd, NULL, "after it");
		close_all();
	}
	CASE(3)
	{
		/* A single character, then end of file. */
		int	fd = open_text("x");
		line(fd, "x", "the only character");
		line(fd, NULL, "after it");
		line(fd, NULL, "and again");
		close_all();
	}
	return (t_finish());
}
