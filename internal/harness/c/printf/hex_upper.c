#include "printf_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
		CMP("%X", 0u);
	CASE(2)
		CMP("%X", 1u);
	CASE(3)
	{
		for (unsigned n = 1; n <= 9; n++)
			CMP("%X", n);
	}
	CASE(4)
	{
		/* A to F, the whole point of the uppercase conversion. */
		for (unsigned n = 10; n <= 15; n++)
			CMP("%X", n);
	}
	CASE(5)
		CMP("%X", 16u);
	CASE(6)
		CMP("%X", 255u);
	CASE(7)
		CMP("%X", 256u);
	CASE(8)
		CMP("%X", 4095u);
	CASE(9)
		CMP("%X", 65535u);
	CASE(10)
	{
		CMP("%X", (unsigned)-1);
		CMP("%X", UINT_MAX);
	}
	CASE(11)
		CMP("%X", (unsigned)INT_MAX);
	CASE(12)
		CMP("%X %X %X", 10u, 11u, 12u);
	CASE(13)
		CMP("before %X after", 0xABCDEFu);
	CASE(14)
	{
		CMP("%X", 0xDEADBEEFu);
		CMP("%X", 0x12345678u);
	}
	CASE(15)
	{
		/* Lower and upper side by side: only the letters may differ. */
		CMP("%x %X", 0xabcdefu, 0xabcdefu);
		unsigned	n = 1;
		for (int i = 0; i < 8; i++)
		{
			CMP("%X", n);
			n *= 16;
		}
	}
	return (t_finish());
}
