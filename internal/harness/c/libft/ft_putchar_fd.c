#include "libft_test.h"

/* wrote captures standard output around a call and compares the bytes. */
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
		ft_putchar_fd('a', 1);
		wrote("a", 1, "ft_putchar_fd('a', 1)");
	}
	CASE(2)
	{
		/* Exactly one byte, with no terminator and no newline added. */
		t_capture_start(1);
		ft_putchar_fd('\0', 1);
		wrote("\0", 1, "ft_putchar_fd('\\0', 1)");
	}
	CASE(3)
	{
		/* Several calls append rather than overwrite. */
		t_capture_start(1);
		ft_putchar_fd('4', 1);
		ft_putchar_fd('2', 1);
		ft_putchar_fd('\n', 1);
		wrote("42\n", 3, "three calls to ft_putchar_fd");
	}
	return (t_finish());
}
