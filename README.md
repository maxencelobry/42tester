# 42tester

A moulinette-style tester for the first three C projects of the 42 common
core: **libft**, **ft_printf** and **get_next_line**.

It produces a report in the moulinette's own format. When everything passes,
the file it writes is byte-for-byte identical to the real one — that is
enforced by a test, not by hand.

```
42tester .
```

One binary. No Python, no virtualenv, no cloning a different repository per
project. The whole C test suite is embedded in the executable.

## Install

Grab the binary for your machine from the releases page, or build it:

```bash
make
```

Go 1.24 or later, and a C compiler. That is all.

## Use

Point it at a directory and it works out what is in there:

```bash
42tester .
```

```
detected libft in . (libft.h, Makefile builds libft.a)

Common_Core-Project-C-Libft
  ✓ Expected files
  ✓ Allowed functions

  ✗ ft_split               3/5
      ✗ ft_split_test__#2
        count(tab) returned 3, expected 2
      ✗ ft_split_test__#4
        ft_split("", ' ') returned NULL

  FAIL  218/222

  report: moulinette_report.md
```

Point it at a folder holding several projects and it tests all of them in one
go. If the guess is wrong, force it:

```bash
42tester -p printf ~/ft_printf
```

### Flags

| Flag | What it does |
|---|---|
| `-v` | List every test, not just the failures |
| `-p <project>` | Force `libft`, `printf` or `gnl` instead of detecting |
| `-o <file>` | Where to write the report (default `moulinette_report.md`) |
| `--no-report` | Do not write the markdown report at all |
| `--json <file>` | Also write a machine-readable report, for CI |
| `--leaks` | Track memory leaks (Linux and macOS) |
| `--sanitize` | Build with the address and undefined-behaviour sanitizers |
| `--strict-alloc` | Fail tests that allocate more memory than they need |
| `--extra` | Run the tests the moulinette does not have (see below) |
| `--timeout` | Time limit for one test (default 5s) |
| `-j` | How many groups to build and run at once |

## What it checks

Before any test, the same two things the moulinette checks first:

- **Expected files** — the header, the Makefile and at least one source.
- **Allowed functions** — every external symbol in your `.a`, read with `nm`.
  Reading the compiled archive rather than the source is what makes this
  trustworthy: a forbidden `printf` cannot hide behind a macro.

Then one executable per test group, and one process per test case. A segfault
in `ft_split` costs you that case, not the rest of the run. Infinite loops are
caught by the timeout and reported as such rather than hanging the tester.

| Project | Groups | Cases |
|---|---|---|
| libft | 43 | 222 |
| ft_printf | 6 | 63 |
| get_next_line | 13 | 44 |

`ft_printf` is graded against your system's `printf`, output **and** return
value. Using the C library as the reference is what makes platform-specific
answers such as `%p` of `NULL` correct by construction.

`get_next_line` is compiled once per `BUFFER_SIZE` the moulinette tries: 0, 1
and 1000. `BUFFER_SIZE` 0 has to return `NULL`, not spin.

### The extra tests

`--extra` adds tests the real moulinette reports do not contain, but that the
subject requires or that an evaluator will try by hand:

- `ft_printf`: `%d`, `%i`, `%u`, and formats mixing several conversions
- `get_next_line`: several descriptors read in turn, binary content,
  `BUFFER_SIZE` 9999

They are off by default so the report stays a faithful copy of the
moulinette's.

## Known limits

**Windows.** Everything works except two things, both because the Microsoft C
library differs from the one on the 42 machines:

- `%p` prints `0000000000000001` where Linux and macOS print `0x1`, so
  `ft_printf_pointer` fails even on correct code. The tester says so when it
  runs.
- Leak tracking needs `dlsym`, so `--leaks` is a no-op.

Use WSL or a Mac for those. Everything else is trustworthy.

**Failure formatting.** The reports this was built from are all-green, so the
moulinette's own layout for a failure is unknown. What you get for a failing
test (`KO ❌` plus an explanation block) is this tool's invention. The passing
layout is exact.

**Allocation sizes.** `--strict-alloc` fails a function that asks for more
memory than it needs — `ft_substr` allocating the whole string when it only
returns four characters, say. The moulinette tolerates that, so it is off by
default. Turn it on if you want to be harder on yourself than the correction
is.

## How it is put together

```
cmd/42tester          the command line
internal/spec         what each project is: files, allowed functions, groups
internal/detect       working out which project a directory holds
internal/prereq       expected files, allowed functions
internal/build        working copy, make, one executable per group
internal/runner       running cases, timeouts, crash attribution
internal/report       moulinette markdown, terminal output, JSON
internal/harness/c    the C test suite, embedded in the binary
docs/moulinette       the real reports this is measured against
```

Your directory is never written to except for the report: everything is
compiled in a temporary copy, so a broken Makefile cannot leave object files
behind in your repository.

## Prior art

The test cases owe a lot to [Tripouille's
testers](https://github.com/Tripouille/libftTester), in particular the
allocation-size check and a number of edge cases for `ft_strlcat` and
`ft_split`.
