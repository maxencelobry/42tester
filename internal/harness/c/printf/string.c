#include "printf_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
		CMP("%s", "tripouille");
	CASE(2)
		CMP("%s", "");
	CASE(3)
	{
		/* A null pointer prints whatever the libc prints for it, which is
		 * "(null)" on Linux. */
		CMP("%s", (char *)NULL);
		CMP("[%s]", (char *)NULL);
	}
	CASE(4)
	{
		char	big[1025];
		memset(big, 'a', 1024);
		big[1024] = '\0';
		CMP("%s", big);
	}
	CASE(5)
	{
		CMP("%s", "line one\nline two");
		CMP("%s", "tab\there");
	}
	CASE(6)
		CMP("%s %s %s", "one", "two", "three");
	CASE(7)
		CMP("before %s after", "middle");
	CASE(8)
	{
		/* A percent inside the argument is data, not a conversion. */
		CMP("%s", "100%% sure");
		CMP("%s", "%s%d%c");
	}
	CASE(9)
	{
		/* Every byte above 127 must survive unchanged. */
		char	all[256];
		for (int i = 1; i < 256; i++)
			all[i - 1] = (char)i;
		all[255] = '\0';
		CMP("%s", all);
	}
	CASE(10)
	{
		CMP("%s%s%s%s", "a", "", "b", "");
		CMP("%s", " ");
	}
	return (t_finish());
}
