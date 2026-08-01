#include "printf_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
		CMP("%c", 'a');
	CASE(2)
	{
		CMP("%c", 'A');
		CMP("%c", 'z');
		CMP("%c", '0');
	}
	CASE(3)
		CMP("before %c after", 'X');
	CASE(4)
	{
		/* A NUL is one written byte like any other, and counts as one. */
		CMP("%c", '\0');
		CMP("[%c]", '\0');
	}
	CASE(5)
	{
		CMP("%c", '\n');
		CMP("%c", '\t');
		CMP("%c%c", '\r', '\v');
	}
	CASE(6)
	{
		/* The argument is an int narrowed to unsigned char. */
		CMP("%c", 200);
		CMP("%c", 255);
		CMP("%c", 128);
	}
	CASE(7)
		CMP("%c%c%c%c%c", 'h', 'e', 'l', 'l', 'o');
	CASE(8)
	{
		CMP("%cstart", 'X');
		CMP("end%c", 'X');
	}
	CASE(9)
	{
		for (int c = 32; c < 127; c++)
			CMP("%c", c);
	}
	CASE(10)
	{
		CMP("%c and a literal %% sign", '?');
		CMP("%c%c", ' ', ' ');
	}
	return (t_finish());
}
