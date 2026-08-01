/* Formats that combine several conversions. The moulinette tests each
 * conversion on its own, so this file only runs with --extra. */

#include "printf_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
		CMP("%s is %d years old", "42", 42);
	CASE(2)
		CMP("%c%s%d%%", 'a', "bc", 42);
	CASE(3)
	{
		int	local = 1;
		CMP("%p %s %d %x", (void *)&local, "mix", -1, 255u);
	}
	CASE(4)
		CMP("no conversion at all");
	CASE(5)
		CMP("");
	CASE(6)
	{
		/* A conversion right at each end of the format. */
		CMP("%dmiddle%d", 1, 2);
		CMP("%s", "only");
	}
	CASE(7)
		CMP("%c%c%c%c%c%c%c%c%c%c", 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j');
	CASE(8)
		CMP("%x%X%u%d%i", 255u, 255u, 255u, 255, 255);
	CASE(9)
	{
		CMP("%s%c%s%c", "a", '\n', "b", '\n');
		CMP("tab\there %d", 42);
	}
	CASE(10)
	{
		/* A long format with many conversions, to catch a buffer that is
		 * flushed too late or not at all. */
		for (int i = 0; i < 20; i++)
			CMP("%d-%s-%c-%x|", i, "x", 'y', (unsigned)i);
	}
	return (t_finish());
}
