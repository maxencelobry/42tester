#include "printf_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
		CMP("%%");
	CASE(2)
		CMP("%%%%");
	CASE(3)
	{
		CMP("100%%");
		CMP("%% done");
	}
	CASE(4)
	{
		/* A percent between conversions must not swallow its neighbours. */
		CMP("%c%%%c", 'a', 'b');
		CMP("%s%%%s", "x", "y");
	}
	CASE(5)
	{
		CMP("%%%%%%%%");
		CMP("a%%b%%c%%d");
		CMP("%%s");
	}
	return (t_finish());
}
