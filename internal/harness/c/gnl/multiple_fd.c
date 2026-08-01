/* Reading several descriptors in turn without losing anyone's place. The
 * moulinette reports we copied have no group for it, so this runs with
 * --extra only; the bonus part of the subject does require it. */

#include "gnl_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		int	a = open_text("a1\na2\n");
		int	b = open_text("b1\nb2\n");

		line(a, "a1\n", "first line of file A");
		line(b, "b1\n", "first line of file B");
		line(a, "a2\n", "second line of file A");
		line(b, "b2\n", "second line of file B");
		line(a, NULL, "end of file A");
		line(b, NULL, "end of file B");
		close_all();
	}
	CASE(2)
	{
		int	a = open_text("a1\na2\na3\n");
		int	b = open_text("b1\n");
		int	c = open_text("c1\nc2\n");

		line(a, "a1\n", "A line 1");
		line(b, "b1\n", "B line 1");
		line(c, "c1\n", "C line 1");
		line(b, NULL, "end of B");
		line(a, "a2\n", "A line 2");
		line(c, "c2\n", "C line 2");
		line(a, "a3\n", "A line 3");
		line(c, NULL, "end of C");
		line(a, NULL, "end of A");
		close_all();
	}
	CASE(3)
	{
		/* One descriptor finishing must not disturb the others. */
		int	a = open_text("only\n");
		int	b = open_text("one\ntwo\nthree\n");

		line(a, "only\n", "the only line of A");
		line(a, NULL, "end of A");
		line(b, "one\n", "B line 1");
		line(a, NULL, "end of A again");
		line(b, "two\n", "B line 2");
		line(b, "three\n", "B line 3");
		line(b, NULL, "end of B");
		close_all();
	}
	CASE(4)
	{
		/* Lines long enough that each descriptor is mid-buffer when the
		 * other one is read. */
		char	*big = repeat('m', 5000, 1);
		int		a;
		int		b;

		if (big == NULL)
			t_fail("internal: out of memory");
		else
		{
			a = open_text(big);
			b = open_text(big);
			line(a, big, "the long line of A");
			line(b, big, "the long line of B");
			line(a, NULL, "end of A");
			line(b, NULL, "end of B");
			free(big);
			close_all();
		}
	}
	return (t_finish());
}
