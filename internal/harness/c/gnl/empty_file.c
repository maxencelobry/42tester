#include "gnl_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* Nothing to read means NULL straight away, and every time after. */
		int	fd = open_text("");
		line(fd, NULL, "the first call on an empty file");
		line(fd, NULL, "the second call");
		line(fd, NULL, "the third call");
		close_all();
	}
	return (t_finish());
}
