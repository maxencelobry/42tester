/* %u is required by the subject but has no group in the moulinette reports,
 * so this file only runs with --extra. */

#include "printf_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
		CMP("%u", 0u);
	CASE(2)
		CMP("%u", 42u);
	CASE(3)
	{
		/* Negative values arrive as their unsigned reading. */
		CMP("%u", (unsigned)-1);
		CMP("%u", (unsigned)-42);
	}
	CASE(4)
		CMP("%u", UINT_MAX);
	CASE(5)
		CMP("%u", (unsigned)INT_MAX);
	CASE(6)
	{
		/* One past INT_MAX is where a signed implementation breaks. */
		CMP("%u", 2147483648u);
	}
	CASE(7)
	{
		for (unsigned n = 0; n <= 9; n++)
			CMP("%u", n);
	}
	CASE(8)
		CMP("%u %u %u", 1u, 2u, 3u);
	CASE(9)
		CMP("before %u after", 4242u);
	CASE(10)
	{
		unsigned	n = 1;
		for (int i = 0; i < 10; i++)
		{
			CMP("%u", n);
			n *= 10;
		}
	}
	CASE(11)
		CMP("%u%u", 0u, 0u);
	CASE(12)
		CMP("%u%%%u", 4u, 2u);
	return (t_finish());
}
