#include "libft_test.h"

static void	expect(int n)
{
	char	want[T_DECIMAL_CHARS(int)];
	char	shown[T_DECIMAL_CHARS(int) + 32];
	size_t	len;
	char	*got;

	snprintf(want, sizeof(want), "%d", n);
	t_capture_start(1);
	ft_putnbr_fd(n, 1);
	got = t_capture_stop(&len);
	if (got == NULL)
	{
		t_fail("internal: could not read back what ft_putnbr_fd(%d, 1) wrote", n);
		return ;
	}
	if (len != strlen(want) || memcmp(got, want, len) != 0)
	{
		t_input("%d", n);
		snprintf(shown, sizeof(shown), "%s", t_show(got, len));
		t_fail_cmp("ft_putnbr_fd(n, 1) wrote", t_showz(want), shown);
	}
	free(got);
}

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
		expect(0);
	CASE(2)
	{
		expect(42);
		expect(7);
		expect(1234567);
	}
	CASE(3)
	{
		expect(-42);
		expect(-1);
		expect(-1234567);
	}
	CASE(4)
		expect(INT_MAX);
	CASE(5)
	{
		/* INT_MIN cannot be negated inside an int, which is where naive
		 * implementations print garbage or loop forever. */
		expect(INT_MIN);
	}
	return (t_finish());
}
