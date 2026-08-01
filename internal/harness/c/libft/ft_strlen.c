#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
		EQ_SIZE(ft_strlen(""), 0);
	CASE(2)
		EQ_SIZE(ft_strlen("tripouille"), 10);
	CASE(3)
	{
		/* A string long enough that the count cannot be a lucky guess. */
		const char	*s = "42 is the answer to everything, obviously";
		EQ_SIZE(ft_strlen(s), strlen(s));
	}
	CASE(4)
	{
		char	buf[64];
		memset(buf, 'a', sizeof(buf));
		buf[63] = '\0';
		EQ_SIZE(ft_strlen(buf), 63);
		buf[0] = '\0';
		EQ_SIZE(ft_strlen(buf), 0);
	}
	CASE(5)
	{
		char	buf[257];
		for (size_t n = 0; n <= 256; n++)
		{
			memset(buf, 'x', n);
			buf[n] = '\0';
			EQ_SIZE(ft_strlen(buf), strlen(buf));
		}
	}
	return (t_finish());
}
