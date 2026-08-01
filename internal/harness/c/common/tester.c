#include "tester.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifdef _WIN32
# include <io.h>
# include <process.h>
# define t_dup   _dup
# define t_dup2  _dup2
# define t_close _close
# define t_getpid _getpid
#else
# include <unistd.h>
# define t_dup   dup
# define t_dup2  dup2
# define t_close close
# define t_getpid getpid
#endif

#if defined(T_STRICT_ALLOC)
# if defined(__APPLE__)
#  include <malloc/malloc.h>
#  define T_BLOCK_SIZE(p) malloc_size(p)
# elif defined(_WIN32)
#  include <malloc.h>
#  define T_BLOCK_SIZE(p) _msize((void *)(p))
# elif defined(__linux__)
#  include <malloc.h>
#  define T_BLOCK_SIZE(p) malloc_usable_size((void *)(p))
# endif
#endif

/* ------------------------------------------------------------------------ */
/* state                                                                     */
/* ------------------------------------------------------------------------ */

/* t_only is the single case to run, or 0 for all of them. */
static int	t_only = 0;
static int	t_current = 0;
static int	t_failed = 0;
static int	t_any_failure = 0;
static char	t_msg[T_MSG_MAX];
static char	t_expected[T_MSG_MAX];
static char	t_got[T_MSG_MAX];

/* ------------------------------------------------------------------------ */
/* reporting                                                                 */
/* ------------------------------------------------------------------------ */

/* emit writes one protocol line, escaping the detail so it stays on a single
 * line. It uses write() rather than fprintf so it is safe from a signal
 * handler. */
static void	emit(int n, const char *status, const char *detail)
{
	char	line[T_MSG_MAX * 2 + 64];
	size_t	len = 0;
	int		head;

	head = snprintf(line, sizeof(line), "@@CASE %d %s ", n, status);
	if (head < 0)
		return ;
	len = (size_t)head;
	for (size_t i = 0; detail && detail[i] && len + 3 < sizeof(line); i++)
	{
		if (detail[i] == '\n')
		{
			line[len++] = '\\';
			line[len++] = 'n';
		}
		else if (detail[i] == '\t')
		{
			line[len++] = '\\';
			line[len++] = 't';
		}
		else if (detail[i] == '\\')
		{
			line[len++] = '\\';
			line[len++] = '\\';
		}
		else if ((unsigned char)detail[i] >= ' ' || detail[i] == ' ')
			line[len++] = detail[i];
	}
	line[len++] = '\n';
	if (write(2, line, len) < 0)
		(void)0;
}

/* on_signal reports the case that was running and leaves immediately: the
 * process state is not trustworthy after a segfault. */
static void	on_signal(int sig)
{
	const char	*name = "the test crashed";

	if (sig == SIGSEGV)
		name = "segmentation fault: the function read or wrote outside the memory it was given";
	else if (sig == SIGABRT)
		name = "aborted: usually a double free or a corrupted heap";
	else if (sig == SIGFPE)
		name = "arithmetic error: division by zero or overflow";
#ifdef SIGBUS
	else if (sig == SIGBUS)
		name = "bus error: misaligned or invalid memory access";
#endif
	if (t_current > 0)
		emit(t_current, "CRASH", name);
	_Exit(3);
}

void	t_init(int argc, char **argv)
{
	if (argc > 1)
		t_only = atoi(argv[1]);

	signal(SIGSEGV, on_signal);
	signal(SIGABRT, on_signal);
	signal(SIGFPE, on_signal);
#ifdef SIGBUS
	signal(SIGBUS, on_signal);
#endif
	setvbuf(stderr, NULL, _IONBF, 0);
}

int	t_case_begin(int n)
{
	if (t_only != 0 && t_only != n)
		return (0);
	t_current = n;
	t_failed = 0;
	t_msg[0] = '\0';
	t_expected[0] = '\0';
	t_got[0] = '\0';
	t_leaks_reset();
	return (1);
}

int	t_case_end(void)
{
	if (t_failed)
	{
		t_any_failure = 1;
		emit(t_current, "KO", t_msg);
		/* The comparison, when there was one, follows on its own lines so
		 * the report can lay expected against got. */
		if (t_expected[0] != '\0' || t_got[0] != '\0')
		{
			emit(t_current, "EXPECTED", t_expected);
			emit(t_current, "GOT", t_got);
		}
	}
	else
		emit(t_current, "OK", "");
	t_current = 0;
	return (0);
}

int	t_finish(void)
{
	return (t_any_failure ? 1 : 0);
}

/* record keeps the first failure of a case: later ones are usually knock-on
 * effects and would bury the real cause. */
static void	record(const char *fmt, va_list ap)
{
	if (t_failed)
		return ;
	t_failed = 1;
	vsnprintf(t_msg, sizeof(t_msg), fmt, ap);
}

void	t_fail(const char *fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	record(fmt, ap);
	va_end(ap);
}

void	t_assert(int cond, const char *fmt, ...)
{
	va_list	ap;

	if (cond)
		return ;
	va_start(ap, fmt);
	record(fmt, ap);
	va_end(ap);
}

/* The message is the call alone; the reader composes the sentence from it
 * and the two values, so there is one source of truth for each piece. */
void	t_fail_cmp(const char *call, const char *expected, const char *got)
{
	if (t_failed)
		return ;
	t_failed = 1;
	snprintf(t_msg, sizeof(t_msg), "%s", call);
	snprintf(t_expected, sizeof(t_expected), "%s", expected);
	snprintf(t_got, sizeof(t_got), "%s", got);
}

/* cmp_ll and friends render both sides before handing them over, because
 * t_show rotates a small pool of buffers and the caller must not hold two
 * live results across another call. */
void	t_eq_ll(long long got, long long want, const char *what)
{
	char	g[32];
	char	w[32];

	if (got == want)
		return ;
	snprintf(g, sizeof(g), "%lld", got);
	snprintf(w, sizeof(w), "%lld", want);
	t_fail_cmp(what, w, g);
}

void	t_eq_ull(unsigned long long got, unsigned long long want, const char *what)
{
	char	g[32];
	char	w[32];

	if (got == want)
		return ;
	snprintf(g, sizeof(g), "%llu", got);
	snprintf(w, sizeof(w), "%llu", want);
	t_fail_cmp(what, w, g);
}

void	t_eq_str(const char *got, const char *want, const char *what)
{
	char	g[T_MSG_MAX];

	if (got == want)
		return ;
	if (got != NULL && want != NULL && strcmp(got, want) == 0)
		return ;
	snprintf(g, sizeof(g), "%s", t_showz(got));
	t_fail_cmp(what, t_showz(want), g);
}

void	t_eq_mem(const void *got, const void *want, size_t n, const char *what)
{
	char	g[T_MSG_MAX];

	if (got == NULL || want == NULL)
	{
		if (got == want)
			return ;
	}
	else if (memcmp(got, want, n) == 0)
		return ;
	snprintf(g, sizeof(g), "%s", t_show(got, n));
	t_fail_cmp(what, t_show(want, n), g);
}

void	t_eq_ptr(const void *got, const void *want, const char *what)
{
	char	g[32];
	char	w[32];

	if (got == want)
		return ;
	snprintf(g, sizeof(g), "%p", got);
	snprintf(w, sizeof(w), "%p", want);
	t_fail_cmp(what, w, g);
}

void	t_not_null(const void *got, const char *what)
{
	if (got == NULL)
		t_fail_cmp(what, "a valid pointer", "NULL");
}

/* ------------------------------------------------------------------------ */
/* rendering values                                                          */
/* ------------------------------------------------------------------------ */

#define T_SHOW_SLOTS 4
#define T_SHOW_MAX   200

const char	*t_show(const void *data, size_t len)
{
	static char		bufs[T_SHOW_SLOTS][T_SHOW_MAX * 4 + 16];
	static unsigned	slot = 0;
	const unsigned char	*p = data;
	char			*out;
	size_t			i, o = 0;
	int				truncated = 0;

	if (p == NULL)
		return ("NULL");
	out = bufs[slot++ % T_SHOW_SLOTS];
	if (len > T_SHOW_MAX)
	{
		len = T_SHOW_MAX;
		truncated = 1;
	}
	out[o++] = '"';
	for (i = 0; i < len; i++)
	{
		if (p[i] == '\n')
			o += (size_t)sprintf(out + o, "\\n");
		else if (p[i] == '\t')
			o += (size_t)sprintf(out + o, "\\t");
		else if (p[i] == '"')
			o += (size_t)sprintf(out + o, "\\\"");
		else if (p[i] == '\\')
			o += (size_t)sprintf(out + o, "\\\\");
		else if (p[i] >= 32 && p[i] < 127)
			out[o++] = (char)p[i];
		else
			o += (size_t)sprintf(out + o, "\\x%02x", p[i]);
	}
	out[o++] = '"';
	if (truncated)
		o += (size_t)sprintf(out + o, "...");
	out[o] = '\0';
	return (out);
}

const char	*t_showz(const char *s)
{
	if (s == NULL)
		return ("NULL");
	return (t_show(s, strlen(s)));
}

/* ------------------------------------------------------------------------ */
/* capturing a file descriptor                                               */
/* ------------------------------------------------------------------------ */

static FILE	*cap_file = NULL;
static int	cap_fd = -1;
static int	cap_saved = -1;
static char	cap_path[128];

void	t_capture_start(int fd)
{
	static unsigned	counter = 0;

	snprintf(cap_path, sizeof(cap_path), ".42tester_cap_%d_%u",
		(int)t_getpid(), counter++);
	cap_file = fopen(cap_path, "w+b");
	if (cap_file == NULL)
	{
		t_fail("internal: could not create a capture file");
		return ;
	}
	fflush(NULL);
	cap_saved = t_dup(fd);
	cap_fd = fd;
	t_dup2(fileno(cap_file), fd);
}

char	*t_capture_stop(size_t *len)
{
	long	size;
	char	*data;
	size_t	got;

	if (len)
		*len = 0;
	if (cap_file == NULL)
		return (NULL);
	fflush(NULL);
	t_dup2(cap_saved, cap_fd);
	t_close(cap_saved);
	cap_saved = -1;
	cap_fd = -1;

	fflush(cap_file);
	fseek(cap_file, 0, SEEK_END);
	size = ftell(cap_file);
	if (size < 0)
		size = 0;
	rewind(cap_file);

	data = malloc((size_t)size + 1);
	if (data == NULL)
	{
		fclose(cap_file);
		remove(cap_path);
		cap_file = NULL;
		return (NULL);
	}
	got = fread(data, 1, (size_t)size, cap_file);
	data[got] = '\0';
	if (len)
		*len = got;

	fclose(cap_file);
	remove(cap_path);
	cap_file = NULL;
	return (data);
}

/* ------------------------------------------------------------------------ */
/* allocation size                                                           */
/* ------------------------------------------------------------------------ */

void	t_alloc_size(const void *p, size_t want, const char *what)
{
#ifdef T_BLOCK_SIZE
	size_t	got;
	void	*reference;
	size_t	expected;

	if (p == NULL)
	{
		t_fail("%s is NULL, expected a block of %zu bytes", what, want);
		return ;
	}
	/* The allocator rounds sizes up, so comparing against a block we ask for
	 * ourselves is the only fair test. */
	reference = malloc(want);
	if (reference == NULL)
		return ;
	expected = T_BLOCK_SIZE(reference);
	got = T_BLOCK_SIZE(p);
	free(reference);
	if (got != expected)
		t_fail("%s was allocated with %zu usable bytes, expected %zu (for %zu requested)",
			what, got, expected, want);
#else
	(void)p;
	(void)want;
	(void)what;
#endif
}

/* ------------------------------------------------------------------------ */
/* leak tracking                                                             */
/* ------------------------------------------------------------------------ */

#if defined(T_LEAKS) && !defined(_WIN32)

# include <dlfcn.h>

/* The table is a fixed array on purpose: allocating inside malloc would
 * recurse. 4096 live blocks is far more than any libft test needs. */
# define T_LEAK_SLOTS 4096

typedef struct s_block
{
	void	*p;
	size_t	size;
}	t_block;

static t_block	leak_table[T_LEAK_SLOTS];
static size_t	leak_count = 0;
static int		leak_tracking = 0;

static void	*(*real_malloc)(size_t) = NULL;
static void	(*real_free)(void *) = NULL;

/* Early allocations happen before dlsym is usable (dlsym itself may
 * allocate), so they are served from a small static arena and never freed. */
static char		bootstrap[65536];
static size_t	bootstrap_used = 0;

static int	in_bootstrap(void *p)
{
	return ((char *)p >= bootstrap && (char *)p < bootstrap + sizeof(bootstrap));
}

static void	resolve_real(void)
{
	static int	resolving = 0;

	if (real_malloc != NULL || resolving)
		return ;
	resolving = 1;
	real_malloc = (void *(*)(size_t))dlsym(RTLD_NEXT, "malloc");
	real_free = (void (*)(void *))dlsym(RTLD_NEXT, "free");
	resolving = 0;
}

void	*malloc(size_t size)
{
	void	*p;

	resolve_real();
	if (real_malloc == NULL)
	{
		if (bootstrap_used + size > sizeof(bootstrap))
			return (NULL);
		p = bootstrap + bootstrap_used;
		bootstrap_used += (size + 15) & ~(size_t)15;
		return (p);
	}
	p = real_malloc(size);
	if (p != NULL && leak_tracking && leak_count < T_LEAK_SLOTS)
	{
		leak_table[leak_count].p = p;
		leak_table[leak_count].size = size;
		leak_count++;
	}
	return (p);
}

void	free(void *p)
{
	size_t	i;

	if (p == NULL || in_bootstrap(p))
		return ;
	for (i = 0; i < leak_count; i++)
	{
		if (leak_table[i].p == p)
		{
			leak_table[i] = leak_table[leak_count - 1];
			leak_count--;
			break ;
		}
	}
	resolve_real();
	if (real_free != NULL)
		real_free(p);
}

void	*calloc(size_t n, size_t size)
{
	void	*p;

	if (size != 0 && n > (size_t)-1 / size)
		return (NULL);
	p = malloc(n * size);
	if (p != NULL)
		memset(p, 0, n * size);
	return (p);
}

void	t_leaks_reset(void)
{
	leak_count = 0;
	leak_tracking = 1;
}

void	t_leaks_check(void)
{
	size_t	total = 0;
	size_t	i;

	if (leak_count == 0)
		return ;
	for (i = 0; i < leak_count; i++)
		total += leak_table[i].size;
	t_fail("leak: %zu block(s) still allocated, %zu bytes in total",
		leak_count, total);
	leak_count = 0;
}

#else

void	t_leaks_reset(void)
{
}

void	t_leaks_check(void)
{
}

#endif
