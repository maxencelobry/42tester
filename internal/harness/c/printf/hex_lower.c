#include "printf_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
		CMP("%x", 0u);
	CASE(2)
		CMP("%x", 1u);
	CASE(3)
	{
		for (unsigned n = 1; n <= 9; n++)
			CMP("%x", n);
	}
	CASE(4)
	{
		/* Ten to fifteen are where the letters start. */
		for (unsigned n = 10; n <= 15; n++)
			CMP("%x", n);
	}
	CASE(5)
		CMP("%x", 16u);
	CASE(6)
		CMP("%x", 255u);
	CASE(7)
		CMP("%x", 256u);
	CASE(8)
		CMP("%x", 4095u);
	CASE(9)
		CMP("%x", 65535u);
	CASE(10)
	{
		/* The argument is unsigned, so this is 0xffffffff and not -1. */
		CMP("%x", (unsigned)-1);
		CMP("%x", UINT_MAX);
	}
	CASE(11)
		CMP("%x", (unsigned)INT_MAX);
	CASE(12)
		CMP("%x %x %x", 1u, 2u, 3u);
	CASE(13)
		CMP("before %x after", 0xabcdefu);
	CASE(14)
	{
		CMP("%x", 0xdeadbeefu);
		CMP("%x", 0x12345678u);
	}
	CASE(15)
	{
		/* Every power of sixteen changes the digit count. */
		unsigned	n = 1;
		for (int i = 0; i < 8; i++)
		{
			CMP("%x", n);
			n *= 16;
		}
	}
	return (t_finish());
}
