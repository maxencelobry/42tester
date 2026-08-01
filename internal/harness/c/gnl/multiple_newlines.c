#include "gnl_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* Three newlines are three empty lines, not one. */
		int	fd = open_text("\n\n\n");
		line(fd, "\n", "empty line 1");
		line(fd, "\n", "empty line 2");
		line(fd, "\n", "empty line 3");
		line(fd, NULL, "after them");
		close_all();
	}
	CASE(2)
	{
		int	fd = open_text("a\n\n\nb\n");
		line(fd, "a\n", "line 1");
		line(fd, "\n", "empty line 2");
		line(fd, "\n", "empty line 3");
		line(fd, "b\n", "line 4");
		line(fd, NULL, "line 5");
		close_all();
	}
	CASE(3)
	{
		/* Newlines at the very start. */
		int	fd = open_text("\n\ncontent\n");
		line(fd, "\n", "empty line 1");
		line(fd, "\n", "empty line 2");
		line(fd, "content\n", "the content line");
		line(fd, NULL, "after it");
		close_all();
	}
	CASE(4)
	{
		/* And newlines at the very end. */
		int	fd = open_text("content\n\n\n");
		line(fd, "content\n", "the content line");
		line(fd, "\n", "empty line 2");
		line(fd, "\n", "empty line 3");
		line(fd, NULL, "after them");
		close_all();
	}
	CASE(5)
	{
		int	fd = open_text("\n\n\n\n\n\n\n\n\n\n");
		for (int i = 1; i <= 10; i++)
			line(fd, "\n", "one of ten empty lines");
		line(fd, NULL, "after all ten");
		close_all();
	}
	return (t_finish());
}
