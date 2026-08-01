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
		t_capture_start(1);
		ft_putstr_fd("tripouille", 1);
		wrote("tripouille", 10, "ft_putstr_fd(\"tripouille\", 1)");
	}
	CASE(2)
	{
		/* An empty string writes nothing at all, not even a terminator. */
		t_capture_start(1);
		ft_putstr_fd("", 1);
		wrote("", 0, "ft_putstr_fd(\"\", 1)");
	}
	CASE(3)
	{
		/* No newline is added, and characters inside the string are kept. */
		t_capture_start(1);
		ft_putstr_fd("42\nis\tthe answer", 1);
		wrote("42\nis\tthe answer", 16, "ft_putstr_fd with control characters");
	}
	return (t_finish());
}
