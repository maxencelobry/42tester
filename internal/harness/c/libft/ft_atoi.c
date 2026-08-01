#include "libft_test.h"

int	main(int argc, char **argv)
{
	t_init(argc, argv);

	CASE(1)
	{
		EQ_INT(ft_atoi("42"), 42);
		EQ_INT(ft_atoi("0"), 0);
		EQ_INT(ft_atoi("7"), 7);
	}
	CASE(2)
	{
		EQ_INT(ft_atoi("-42"), -42);
		EQ_INT(ft_atoi("-0"), 0);
	}
	CASE(3)
	{
		/* Only these six characters count as leading whitespace. */
		EQ_INT(ft_atoi("   42"), 42);
		EQ_INT(ft_atoi("\t\n\v\f\r 42"), 42);
		EQ_INT(ft_atoi("\t\n\v\f\r -42"), -42);
	}
	CASE(4)
	{
		EQ_INT(ft_atoi("+42"), 42);
		EQ_INT(ft_atoi("  +42"), 42);
	}
	CASE(5)
	{
		/* Parsing stops at the first character that is not a digit. */
		EQ_INT(ft_atoi("42abc"), 42);
		EQ_INT(ft_atoi("42 43"), 42);
		EQ_INT(ft_atoi("4.2"), 4);
		EQ_INT(ft_atoi("42\n"), 42);
	}
	CASE(6)
		EQ_INT(ft_atoi("2147483647"), INT_MAX);
	CASE(7)
		EQ_INT(ft_atoi("-2147483648"), INT_MIN);
	CASE(8)
	{
		/* Nothing to parse means zero, not garbage. */
		EQ_INT(ft_atoi(""), 0);
		EQ_INT(ft_atoi("abc"), 0);
		EQ_INT(ft_atoi("   "), 0);
		EQ_INT(ft_atoi("-"), 0);
		EQ_INT(ft_atoi("+"), 0);
	}
	CASE(9)
	{
		EQ_INT(ft_atoi("000042"), 42);
		EQ_INT(ft_atoi("-000042"), -42);
		EQ_INT(ft_atoi("0000000000000000042"), 42);
	}
	CASE(10)
	{
		/* A second sign is not part of the number. */
		EQ_INT(ft_atoi("+-42"), 0);
		EQ_INT(ft_atoi("--42"), 0);
		EQ_INT(ft_atoi("+ 42"), 0);
		/* Whitespace after the sign is not allowed either. */
		EQ_INT(ft_atoi("- 42"), 0);
		/* And whitespace in the middle ends the number. */
		EQ_INT(ft_atoi("4 2"), 4);
	}
	return (t_finish());
}
