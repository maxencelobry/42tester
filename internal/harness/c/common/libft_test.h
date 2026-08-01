/* Shared prelude for the libft tests: the student's header plus ours. */
#ifndef LIBFT_TEST_H
# define LIBFT_TEST_H

# include "libft.h"
# include "tester.h"

# include <ctype.h>
# include <stdlib.h>
# include <string.h>
# include <limits.h>
# include <stdio.h>

/* classify compares the return value of a character classification function.
 *
 * The subject pins these to 1 and 0, not merely to non-zero: "the return
 * value must be 1 if the character matches the tested class, 0 if the
 * character does not match". The libc is free to return anything non-zero,
 * so it is used only to decide which of the two is expected.
 */
static T_UNUSED void	classify(const char *fn, int c, int got, int matches)
{
	char	g[16];
	char	w[16];

	if (got == (matches ? 1 : 0))
		return ;
	snprintf(g, sizeof(g), "%d", got);
	snprintf(w, sizeof(w), "%d", matches ? 1 : 0);
	if (c >= 32 && c < 127)
		t_input("%d ('%c')", c, c);
	else
		t_input("%d", c);
	t_fail_cmp(fn, w, g);
}

#endif
