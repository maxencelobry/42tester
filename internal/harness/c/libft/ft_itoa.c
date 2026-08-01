#include "libft_test.h"

/* expect renders n with the libc and compares, which keeps the cases short
 * and makes the reference obvious. */
static void	expect(int n)
{
	char	want[16];
	char	call[32];
	char	shown[64];
	char	*got;

	sprintf(want, "%d", n);
	snprintf(call, sizeof(call), "ft_itoa(%d)", n);
	got = ft_itoa(n);
	if (got == NULL || strcmp(got, want) != 0)
	{
		snprintf(shown, sizeof(shown), "%s", t_showz(got));
		t_fail_cmp(call, t_showz(want), shown);
	}
	free(got);
}

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		expect(0);
		t_leaks_check();
	}
	CASE(2)
	{
		expect(42);
		expect(1234);
		t_leaks_check();
	}
	CASE(3)
	{
		expect(-42);
		expect(-1234);
		t_leaks_check();
	}
	CASE(4)
	{
		expect(INT_MAX);
		t_leaks_check();
	}
	CASE(5)
	{
		/* INT_MIN cannot be negated inside an int, which is where naive
		 * implementations break. */
		expect(INT_MIN);
		t_leaks_check();
	}
	CASE(6)
	{
		for (int n = 0; n <= 9; n++)
			expect(n);
		t_leaks_check();
	}
	CASE(7)
	{
		for (int n = -9; n <= -1; n++)
			expect(n);
		t_leaks_check();
	}
	CASE(8)
	{
		/* The block holds the digits and the terminator, nothing more. */
		char	*p = ft_itoa(-42);
		NOT_NULL(p);
		ALLOC_SIZE(p, 4);
		free(p);

		p = ft_itoa(0);
		NOT_NULL(p);
		ALLOC_SIZE(p, 2);
		free(p);
		t_leaks_check();
	}
	CASE(9)
	{
		for (int n = -1000; n <= 1000; n += 37)
			expect(n);
		t_leaks_check();
	}
	CASE(10)
	{
		/* Powers of ten and the values just below them exercise every
		 * digit-count boundary. */
		int	powers[] = {10, 100, 1000, 10000, 100000, 1000000,
			10000000, 100000000, 1000000000};
		for (size_t i = 0; i < sizeof(powers) / sizeof(*powers); i++)
		{
			expect(powers[i]);
			expect(-powers[i]);
			expect(powers[i] - 1);
		}
		t_leaks_check();
	}
	return (t_finish());
}
