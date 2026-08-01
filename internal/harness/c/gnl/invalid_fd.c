#include "gnl_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* read() fails on every one of these, so the answer is NULL and the
		 * function must not leak whatever it had started building. */
		line(-1, NULL, "get_next_line(-1)");
		line(-42, NULL, "get_next_line(-42)");
		line(4242, NULL, "get_next_line(4242) on a descriptor that was never opened");

		/* A descriptor that was valid and then closed. */
		int	fd = open_text("content\n");
		if (fd >= 0)
		{
			t_close(fd);
			g_open_fds[g_file_count - 1] = -1;
			line(fd, NULL, "get_next_line on a closed descriptor");
		}
		close_all();
		t_leaks_check();
	}
	return (t_finish());
}
