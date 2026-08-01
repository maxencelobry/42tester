/* Shared prelude for the get_next_line tests.
 *
 * Each case writes its input to a real file and reads it back through a real
 * file descriptor, because that is the only thing the subject promises will
 * work. Temporary files are removed when the descriptor is closed.
 */

#ifndef GNL_TEST_H
# define GNL_TEST_H

# include "get_next_line.h"
# include "tester.h"

# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# ifdef _WIN32
#  include <io.h>
#  include <process.h>
#  define t_open   _open
#  define t_close  _close
#  define t_dup    _dup
#  define t_dup2   _dup2
#  define t_getpid _getpid
#  define T_OPEN_FLAGS (O_RDONLY | O_BINARY)
# else
#  include <unistd.h>
#  define t_open   open
#  define t_close  close
#  define t_dup    dup
#  define t_dup2   dup2
#  define t_getpid getpid
#  define T_OPEN_FLAGS (O_RDONLY)
# endif

# define GNL_MAX_FILES 16

static char	g_paths[GNL_MAX_FILES][160];
static int	g_open_fds[GNL_MAX_FILES];
static int	g_file_count;

/* open_content writes len bytes to a scratch file and returns a descriptor
 * open on it, or -1 if that failed. */
static T_UNUSED int	open_content(const void *data, size_t len)
{
	FILE	*f;
	int		fd;
	int		slot = g_file_count;

	if (slot >= GNL_MAX_FILES)
	{
		t_fail("internal: too many scratch files in one case");
		return (-1);
	}
	snprintf(g_paths[slot], sizeof(g_paths[slot]), ".42tester_gnl_%d_%d",
		(int)t_getpid(), slot);
	f = fopen(g_paths[slot], "wb");
	if (f == NULL)
	{
		t_fail("internal: could not create a scratch file");
		return (-1);
	}
	if (len > 0)
		fwrite(data, 1, len, f);
	fclose(f);

	fd = t_open(g_paths[slot], T_OPEN_FLAGS);
	if (fd < 0)
	{
		remove(g_paths[slot]);
		t_fail("internal: could not open the scratch file");
		return (-1);
	}
	g_open_fds[slot] = fd;
	g_file_count++;
	return (fd);
}

/* open_text is the common case: a NUL-terminated string. */
static T_UNUSED int	open_text(const char *text)
{
	return (open_content(text, strlen(text)));
}

static T_UNUSED void	close_all(void)
{
	for (int i = 0; i < g_file_count; i++)
	{
		if (g_open_fds[i] >= 0)
			t_close(g_open_fds[i]);
		remove(g_paths[i]);
	}
	g_file_count = 0;
}

/* line checks the next line against want; a NULL want means end of file. */
static T_UNUSED void	line(int fd, const char *want, const char *what)
{
	char	*got;

	if (fd < 0)
		return ;
	got = get_next_line(fd);
	if (want == NULL)
	{
		if (got != NULL)
			t_fail("%s returned %s, expected NULL (end of file)", what, t_showz(got));
	}
	else if (got == NULL)
		t_fail("%s returned NULL, expected %s", what, t_showz(want));
	else if (strcmp(got, want) != 0)
		t_fail("%s returned %s, expected %s", what, t_showz(got), t_showz(want));
	free(got);
}

/* repeat builds a string of n copies of c followed by an optional newline.
 * The caller frees it. */
static T_UNUSED char	*repeat(char c, size_t n, int newline)
{
	char	*s = malloc(n + 2);

	if (s == NULL)
		return (NULL);
	memset(s, c, n);
	if (newline)
		s[n] = '\n';
	s[n + (newline ? 1 : 0)] = '\0';
	return (s);
}

#endif
