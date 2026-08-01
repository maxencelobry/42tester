/* Minimal test framework for the 42 testers.
 *
 * A test file declares numbered cases; the Go side either runs them all in
 * one process or, after a crash, re-runs a single one by passing its number
 * on the command line. Results go to stderr as "@@CASE <n> <status> <detail>"
 * so stdout stays free for the functions under test, several of which write
 * to it on purpose.
 */

#ifndef TESTER_H
# define TESTER_H

# include <stddef.h>

/* Helpers shared through a header are not used by every test file, and a
 * warning about that would end up in the student's report. */
# if defined(__GNUC__) || defined(__clang__)
#  define T_UNUSED __attribute__((unused))
# else
#  define T_UNUSED
# endif

/* ---- lifecycle ---------------------------------------------------------- */

void	t_init(int argc, char **argv);
int		t_case_begin(int n);
int		t_case_end(void);
int		t_finish(void);

/* CASE runs its body once, then reports. Failures inside the body are
 * recorded rather than thrown, so one case can check several things.       */
# define CASE(n) for (int t_once_ = t_case_begin(n); t_once_; t_once_ = t_case_end())

/* ---- assertions --------------------------------------------------------- */

void	t_fail(const char *fmt, ...);
void	t_assert(int cond, const char *fmt, ...);
void	t_eq_ll(long long got, long long want, const char *what);
void	t_eq_ull(unsigned long long got, unsigned long long want, const char *what);
void	t_eq_str(const char *got, const char *want, const char *what);
void	t_eq_mem(const void *got, const void *want, size_t n, const char *what);
void	t_eq_ptr(const void *got, const void *want, const char *what);
void	t_not_null(const void *got, const char *what);

# define EQ_INT(got, want)		t_eq_ll((long long)(got), (long long)(want), #got)
# define EQ_SIZE(got, want)		t_eq_ull((unsigned long long)(got), (unsigned long long)(want), #got)
# define EQ_STR(got, want)		t_eq_str((got), (want), #got)
# define EQ_MEM(got, want, n)	t_eq_mem((got), (want), (n), #got)
# define EQ_PTR(got, want)		t_eq_ptr((got), (want), #got)
# define NOT_NULL(got)			t_not_null((got), #got)
# define ASSERT(cond)			t_assert(!!(cond), "%s is false", #cond)

/* ---- capturing what a function writes ----------------------------------- */

/* Redirects a file descriptor to a temporary file. t_capture_stop restores
 * it and returns everything written, which the caller frees. */
void	t_capture_start(int fd);
char	*t_capture_stop(size_t *len);

/* ---- allocation size ---------------------------------------------------- */

/* Checks a block is exactly the size it should be. Only enforced when the
 * tester is built with -DT_STRICT_ALLOC, because the moulinette tolerates
 * over-allocation and we default to matching it. */
void	t_alloc_size(const void *p, size_t want, const char *what);
# define ALLOC_SIZE(p, want) t_alloc_size((p), (size_t)(want), #p)

/* ---- leaks -------------------------------------------------------------- */

/* Only active with -DT_LEAKS. t_leaks_check fails the current case if
 * anything allocated since the last reset is still held. */
void	t_leaks_reset(void);
void	t_leaks_check(void);

/* ---- helpers ------------------------------------------------------------ */

/* Renders bytes as a readable, quoted string in a rotating static buffer.
 * NULL becomes "(null)". Long inputs are truncated. */
const char	*t_show(const void *data, size_t len);
/* Same, for a NUL-terminated string. */
const char	*t_showz(const char *s);

#endif
