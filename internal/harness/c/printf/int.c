/* %d and %i are required by the subject but have no group in the moulinette
 * reports we copied, so this file only runs with --extra. */

#include "printf_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
		CMP("%d", 0);
	CASE(2)
		CMP("%d", 42);
	CASE(3)
		CMP("%d", -42);
	CASE(4)
		CMP("%d", INT_MAX);
	CASE(5)
	{
		/* INT_MIN cannot be negated inside an int. */
		CMP("%d", INT_MIN);
	}
	CASE(6)
	{
		for (int n = 0; n <= 9; n++)
			CMP("%d", n);
	}
	CASE(7)
	{
		for (int n = -9; n <= -1; n++)
			CMP("%d", n);
	}
	CASE(8)
		CMP("%i", 0);
	CASE(9)
		CMP("%i", -1234);
	CASE(10)
		CMP("%i", INT_MIN);
	CASE(11)
		CMP("%d %i %d", 1, -2, 3);
	CASE(12)
		CMP("before %d after", -7);
	CASE(13)
	{
		int	powers[] = {10, 100, 1000, 10000, 100000,
			1000000, 10000000, 100000000, 1000000000};
		for (size_t i = 0; i < sizeof(powers) / sizeof(*powers); i++)
			CMP("%d", powers[i]);
	}
	CASE(14)
	{
		for (int n = -1000; n <= 1000; n += 173)
			CMP("%d", n);
	}
	CASE(15)
	{
		CMP("%d%d%d", -1, 0, 1);
		CMP("%d%%%d", 4, 2);
	}
	return (t_finish());
}
