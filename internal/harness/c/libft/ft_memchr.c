#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		const char	*s = "tripouille";
		EQ_PTR(ft_memchr(s, 'p', 10), s + 3);
		EQ_PTR(ft_memchr(s, 't', 10), s + 0);
	}
	CASE(2)
	{
		EQ_PTR(ft_memchr("tripouille", 'z', 10), NULL);
	}
	CASE(3)
	{
		/* n = 0 means nothing is looked at, even if the byte is there. */
		EQ_PTR(ft_memchr("abc", 'a', 0), NULL);
		/* And the search stops exactly at n. */
		EQ_PTR(ft_memchr("abc", 'c', 2), NULL);
	}
	CASE(4)
	{
		/* Unlike strchr, this one keeps looking past a NUL byte. */
		const char	buf[8] = {'a', 0, 'b', 0, 'c', 0, 'd', 0};
		EQ_PTR(ft_memchr(buf, 'c', 8), buf + 4);
		EQ_PTR(ft_memchr(buf, 0, 8), buf + 1);
	}
	CASE(5)
	{
		const unsigned char	buf[4] = {0x80, 0xff, 0x01, 0x00};
		EQ_PTR(ft_memchr(buf, 0x80, 4), buf + 0);
		EQ_PTR(ft_memchr(buf, 0xff, 4), buf + 1);
		/* The value is taken modulo 256, so 'a' + 256 finds 'a'. */
		const char	*s = "abc";
		EQ_PTR(ft_memchr(s, 'b' + 256, 3), s + 1);
	}
	return (t_finish());
}
