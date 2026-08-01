# 42tester

Know what the moulinette will say, before you push.

A tester for the first three C projects of the 42 common core — **libft**,
**ft_printf**, **get_next_line** — that writes its report in the moulinette's
own format. When your code passes everything, the file it produces is
byte-for-byte identical to the real one.

```bash
42tester .
```

That is the whole interface. It works out which project is in the directory.

---

## Status: young, read this before trusting it

This was written with Claude, in one sitting, and has not yet been run on the
author's own projects. Being specific about that matters more than the label,
because you are going to decide whether to submit based on what it tells you.

**What is checked.** Reference implementations of all three projects were
written and graded with it: libft 222/222, get_next_line 44/44, ft_printf
55/63 — the eight being a Windows `%p` quirk described under
[Limits](#limits-stated-plainly). Deliberately broken versions were then
checked to fail in the right places, including a segfault, an infinite loop
and a forbidden `printf`. The passing report is diffed against three real
moulinette reports by a test in the repository.

**What is not.** It has never run on macOS or Linux — only on Windows, which
is the one platform nobody at 42 is graded on. It has never seen a real
student submission. `--leaks` has never been executed at all.

So: use it to find bugs, not to conclude you have none. A failure points at
something worth looking at. A clean run is encouraging, not a guarantee.
Issues are welcome, particularly anything that turns out to be this tool's
fault rather than your code's.

---

## Quick start

```bash
# macOS (Apple silicon)
curl -L https://github.com/maxencelobry/42tester/releases/latest/download/42tester-macos-arm64 -o 42tester
chmod +x 42tester && sudo mv 42tester /usr/local/bin/

cd ~/libft && 42tester .
```

Or build it — Go 1.24 and a C compiler, nothing else:

```bash
git clone https://github.com/maxencelobry/42tester && cd 42tester && make
```

## What you get

```
detected libft in . (libft.h, Makefile builds libft.a)

Common_Core-Project-C-Libft
  ✓ Expected files
  ✓ Allowed functions

  ✗ ft_itoa                9/10
      ✗ ft_itoa_test__#5
        ft_itoa(-2147483648)
          expected: "-2147483648"
          got:      "-("

  FAIL  138/139

  report: moulinette_report.md
```

Like the moulinette, it walks the subject in order and **stops at the first
function that fails** — there is no point grading the rest until that one is
fixed. Every failure names the call and puts expected against got, not
`3.KO`. Pass `--all` to see everything that is broken in one pass instead.

## Why not the tester you already know

|  | 42tester | The usual ones |
|---|---|---|
| Install | one binary | clone a repo per project, or a Python venv |
| Report | the moulinette's format, verified identical | a line of `1.OK 2.KO` |
| A segfault | costs you that one test case | kills the rest of the run |
| An infinite loop | reported as a timeout | hangs |
| Forbidden functions | read from your compiled `.a` with `nm` | not checked |
| Your directory | never written to except the report | build artefacts left behind |

## Usage

```bash
42tester .                    # test whatever is here, stop at the first failure
42tester . --all              # do not stop, show everything that is broken
42tester ~/ft_printf -v       # show every test, not just failures
42tester . --leaks            # also hunt memory leaks
42tester -p gnl ~/some/dir    # force the project if detection is wrong
42tester ~/42                 # a folder of projects: tests all of them
```

| Flag | |
|---|---|
| `-v` | List every test, not just the failures |
| `-p <project>` | Force `libft`, `printf` or `gnl` |
| `--all` | Keep going after a failure instead of stopping there like the moulinette |
| `-o <file>` | Where to write the report (default `moulinette_report.md`) |
| `--no-report` | Do not write the report file |
| `--json <file>` | Machine-readable report, for CI |
| `--leaks` | Track memory leaks (Linux and macOS) |
| `--sanitize` | Build with the address and undefined-behaviour sanitizers |
| `--strict-alloc` | Fail functions that allocate more than they need |
| `--extra` | Run tests the moulinette does not have |
| `--timeout` | Time limit for one test (default `5s`) |
| `-j` | Groups to build and run at once |

Exit code is 0 when everything passes, so it drops straight into CI.

## What it checks

Two prerequisites first, the same ones the moulinette starts with:

- **Expected files** — the header, the Makefile, at least one source.
- **Allowed functions** — every external symbol in your archive, read with
  `nm`. Because it reads the compiled code and not the source, a forbidden
  `printf` cannot hide behind a macro or a typedef.

Then the tests, one executable per group and one process per case.

| Project | Groups | Cases |
|---|---|---|
| libft | 43 | 222 |
| ft_printf | 6 | 63 |
| get_next_line | 13 | 44 |

They run in the subject's order, not alphabetically — for **libft** that is
the 23 libc functions of part 1, then the 11 additional ones of part 2, then
the 9 list functions of the bonus, 43 in all. That order is the one the real
reports use, which is what lets the output be diffed against them.

**ft_printf** is compared against your system's `printf` — the bytes written
*and* the return value. Using the C library as the reference is what makes
platform-specific answers like `%p` of `NULL` right by construction, instead
of hard-coding one platform's answer.

**get_next_line** is compiled once per `BUFFER_SIZE` the moulinette uses: 0, 1
and 1000. `BUFFER_SIZE` 0 has to return `NULL`, not spin forever.

**libft** covers all 43 functions including the bonus, which is skipped
automatically if you have not turned it in.

### `--extra`

Tests the real reports do not contain, but that the subject demands or an
evaluator will try by hand:

- `ft_printf`: `%d`, `%i`, `%u`, and formats mixing several conversions
- `get_next_line`: several file descriptors read in turn, binary content,
  `BUFFER_SIZE 9999`

Off by default, so the report stays a faithful copy.

### `--strict-alloc`

Fails a function that asks for more memory than it needs — `ft_substr`
allocating the whole string when it returns four characters. The moulinette
tolerates this, so it is off by default. Turn it on to be harder on yourself
than the correction is.

## Limits, stated plainly

**The failure layout is approximate.** The reports this was built from are
all-green, so the exact markup the moulinette uses for a failing test is not
known. What it does is: name the call, show expected against actual, and stop
the run there. This tool does the same — that is what `--stop` is — but the
precise wording and spacing of the `KO ❌` block are its own. The *passing*
layout is exact and enforced by a test. If you have a real report with
failures in it, open an issue with it and this gets pinned down.

**No `%d`, `%i` or `%u` group in ft_printf.** The real report has groups only
for `%c %s %p %x %X %%`, so that is what the default run has. The subject
still requires the others — use `--extra`.

**Windows.** Everything works except two things, both because the Microsoft C
library differs from the ones at 42:

- `%p` prints `0000000000000001` where Linux and macOS print `0x1`, so
  `ft_printf_pointer` fails on correct code. The tool warns you when it does.
- `--leaks` needs `dlsym` and is a no-op.

Use WSL or a Mac for those two. The other 321 of the 329 cases are
trustworthy — libft and get_next_line are unaffected.

## How it works

```
cmd/42tester          the command line
internal/spec         each project: files, allowed functions, test groups
internal/detect       working out which project a directory holds
internal/prereq       expected files, allowed functions
internal/build        working copy, make, one executable per group
internal/runner       running cases, timeouts, crash attribution
internal/report       moulinette markdown, terminal output, JSON
internal/harness/c    the C test suite, embedded in the binary
docs/moulinette       the real reports this is measured against
```

The C suite is embedded with `go:embed`, which is why there is one file to
download. Your submission is copied to a temporary directory before `make`
runs, and the test binaries run from a scratch directory of their own, so
neither a broken Makefile nor a crashing test can leave anything in your repo.

Each group is a separate executable and each case a separate process. When a
case dies without reporting, the runner re-runs it alone to find out whether
it really crashed or was just downstream of one — which is how you get
`3/5` instead of `run died`.

The report format is not maintained by hand. `internal/report/markdown_test.go`
renders an all-passing report for each project and diffs it against
`docs/moulinette/*.md`. The odd blank lines in the renderer are load-bearing;
tidying them fails the build.

## Contributing

Test cases are plain C in `internal/harness/c/`. To add one, write it and bump
`Cases` for that group in `internal/spec/`. Anything the real moulinette does
not run belongs in `ExtraGroups`, not `Groups` — the default report has to
stay a copy.

```bash
make test    # includes the report-fidelity check
make vet
```

## Credits

The test cases owe a lot to [Tripouille's
testers](https://github.com/Tripouille/libftTester), in particular the
allocation-size check and a number of edge cases for `ft_strlcat` and
`ft_split`.
