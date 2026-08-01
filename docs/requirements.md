# Where each rule comes from

The subjects are copyrighted by 42 and are not reproduced here. This file
records, in its own words, the requirements that shaped the tests, with the
section to check if you want to verify a claim against your own copy.

## libft

**Structure — §4, table of contents.** The subject has one mandatory part
containing three sections: Part 1 (libc functions), Part 2 (additional
functions), Part 3 (linked list). There is no bonus part, and the word never
appears. All 43 functions are therefore graded, and `internal/spec/libft.go`
lists them in that order.

**Classification functions — §4.2.** `isalpha`, `isdigit`, `isalnum`,
`isascii` and `isprint` must return 1 on a match and 0 otherwise. Not
"non-zero": the exact value is pinned. This is why `classify()` in
`internal/harness/c/common/libft_test.h` compares against 1 and 0 rather
than against the libc's return value, which is 1024 on glibc for a letter.

**calloc with a zero argument — §4.2.** The subject overrides the man page:
if either argument is zero, the function still returns a pointer that can be
passed to `free`. `ft_calloc` case 3 checks that.

**ft_substr — §4.3.** The start index is an `unsigned int` and the length a
`size_t`; the result is a fresh allocation of at most that length.

**Technical rules — §4.1.** Global variables forbidden, helper functions
static, every file at the root, no unused files, `-Wall -Wextra -Werror`,
`ar` to build the archive, `libft.a` at the root.

## ft_printf

**Conversions — §4.** The nine conversions `c s p d i u x X %` are all
required. The moulinette report we mirror has groups for only six of them,
which is why `%d`, `%i` and `%u` live in `RequiredGroups`: they are graded,
without disturbing the six groups that must render exactly like the report.

**Reference — §4.** The function is compared against the system `printf`,
which is what the harness does rather than hard-coding expected strings.

**Buffer management — §4.** Explicitly out of scope; nothing here tests it.

## get_next_line

**Return value — §4.** The line, including its terminating newline, except
at end of file when the file does not end with one. `NULL` when there is
nothing left to read or on error.

**BUFFER_SIZE — §4.** Defined at compile time with `-D BUFFER_SIZE=n`, and
the evaluators change it. The project must build **with and without** that
flag, so the header carries a default of its own. That is what the
`default_buffer_size` group checks.

**Standard input — §4.** Reading from a file and from standard input must
both work, hence the `stdin_test` group.

**Undefined behaviour — §4.** The subject calls reading a binary file
undefined, and likewise a file that changed since the last call. The binary
test therefore sits in `ExtraGroups` and never runs by default.

**Forbidden — §4.** `lseek` and using your own libft. Both are caught by the
allowed-functions check, which reads the compiled objects with `nm`.

## What is not sourced

Everything else in the harness is a judgement call: which edge cases are
worth testing, how many cases per group, and the whole failure layout. The
moulinette's own rendering of a failing test is unknown, as
[the README](../README.md) says.
