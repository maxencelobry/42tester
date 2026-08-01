#include "libft_test.h"

static void	wrote(const char *want, size_t want_len, const char *what)
{
	size_t	len;
	char	*got = t_capture_stop(&len);

	if (got == NULL)
	{
		t_fail("internal: could not read back what %s wrote", what);
		return ;
	}
	if (len != want_len || memcmp(got, want, len) != 0)
		t_fail("%s wrote %s, expected %s", what, t_show(got, len), t_show(want, want_len));
	free(got);
}

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		/* Same as putstr, with exactly one newline appended. */
		t_capture_start(1);
		ft_putendl_fd("tripouille", 1);
		wrote("tripouille\n", 11, "ft_putendl_fd(\"tripouille\", 1)");
	}
	CASE(2)
	{
		/* An empty string still gets its newline. */
		t_capture_start(1);
		ft_putendl_fd("", 1);
		wrote("\n", 1, "ft_putendl_fd(\"\", 1)");
	}
	CASE(3)
	{
		/* A string that already ends in a newline gets a second one. */
		t_capture_start(1);
		ft_putendl_fd("42\n", 1);
		wrote("42\n\n", 4, "ft_putendl_fd(\"42\\n\", 1)");
	}
	return (t_finish());
}
