#include "gnl_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* The newline is part of the line that is returned. */
		int	fd = open_text("hello\n");
		line(fd, "hello\n", "get_next_line on \"hello\\n\"");
		close_all();
	}
	CASE(2)
	{
		int	fd = open_text("one\ntwo\n");
		line(fd, "one\n", "first line");
		line(fd, "two\n", "second line");
		close_all();
	}
	CASE(3)
	{
		int	fd = open_text("one\ntwo\nthree\n");
		line(fd, "one\n", "first line");
		line(fd, "two\n", "second line");
		line(fd, "three\n", "third line");
		line(fd, NULL, "after the last line");
		close_all();
	}
	CASE(4)
	{
		int	fd = open_text("a\nbb\nccc\ndddd\neeeee\n");
		line(fd, "a\n", "line 1");
		line(fd, "bb\n", "line 2");
		line(fd, "ccc\n", "line 3");
		line(fd, "dddd\n", "line 4");
		line(fd, "eeeee\n", "line 5");
		line(fd, NULL, "line 6");
		close_all();
	}
	CASE(5)
	{
		/* Once the file is exhausted it stays exhausted. */
		int	fd = open_text("only\n");
		line(fd, "only\n", "the only line");
		line(fd, NULL, "first call past the end");
		line(fd, NULL, "second call past the end");
		line(fd, NULL, "third call past the end");
		close_all();
	}
	return (t_finish());
}
