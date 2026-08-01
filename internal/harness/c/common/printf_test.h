/* Shared prelude for the ft_printf tests.
 *
 * Every case runs the same format through ft_printf and through the system
 * printf, then compares both the bytes written and the return value. Using
 * the libc as the reference is what makes platform-specific answers such as
 * "%p" of NULL correct by construction.
 */

#ifndef PRINTF_TEST_H
# define PRINTF_TEST_H

# include "ft_printf.h"
# include "tester.h"

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <limits.h>
# include <stdint.h>

static T_UNUSED void	t_cmp_printf(const char *expr, int got, char *gs, size_t gl,
				int want, char *ws, size_t wl)
{
	if (gs == NULL || ws == NULL)
	{
		t_fail("internal: could not capture the output of ft_printf(%s)", expr);
		free(gs);
		free(ws);
		return ;
	}
	if (gl != wl || memcmp(gs, ws, gl) != 0)
	{
		char	call[256];
		char	shown[512];

		snprintf(call, sizeof(call), "ft_printf(%s), the output", expr);
		snprintf(shown, sizeof(shown), "%s", t_show(gs, gl));
		t_fail_cmp(call, t_show(ws, wl), shown);
	}
	else if (got != want)
	{
		char	call[256];
		char	g[32];
		char	w[32];

		snprintf(call, sizeof(call),
			"ft_printf(%s) printed the right thing, but its return value", expr);
		snprintf(g, sizeof(g), "%d", got);
		snprintf(w, sizeof(w), "%d", want);
		t_fail_cmp(call, w, g);
	}
	free(gs);
	free(ws);
}

/* CMP runs one format both ways. The trailing fflush matters: stdout is a
 * file while captured, so it is fully buffered. */
# define CMP(...)												\
	do {														\
		int		t_got_;											\
		int		t_want_;										\
		size_t	t_gl_;											\
		size_t	t_wl_;											\
		char	*t_gs_;											\
		char	*t_ws_;											\
																\
		t_capture_start(1);										\
		t_got_ = ft_printf(__VA_ARGS__);						\
		t_gs_ = t_capture_stop(&t_gl_);							\
		t_capture_start(1);										\
		t_want_ = printf(__VA_ARGS__);							\
		fflush(stdout);											\
		t_ws_ = t_capture_stop(&t_wl_);							\
		t_cmp_printf(#__VA_ARGS__, t_got_, t_gs_, t_gl_,		\
			t_want_, t_ws_, t_wl_);								\
	} while (0)

#endif
