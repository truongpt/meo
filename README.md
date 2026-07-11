# Meo - A Subset C Compiler

**meo** is "meo" (cat) in Vietnamese, but now it is a subset C compiler built to study compiler construction.

## Branches

- `master` — main development branch (vibe coding with [Opencode](https://opencode.ai))
- `manual` — manual study notes and experiments

## References

- [Compiler Design in C - Allen I. Holub](https://holub.com/goodies/compiler/compilerDesignInC.pdf)
- [Dragon Book](https://www.amazon.com/Compilers-Principles-Techniques-Tools-2nd/dp/0321486811)
- [A Compiler Writing Journey](https://github.com/DoctorWkt/acwj)

## Design

- Four-pass compiler architecture (refered from [Compiler design in C](https://holub.com/goodies/compiler/compilerDesignInC.pdf)).
- Modular design: lexical analyzer, parser, code generation are independent modules.
- Backend abstraction via function pointer table (`GenFuncTable`), currently supporting x86-64.

## Build

```bash
cmake -S . -B build
cmake --build build -j$(nproc)
ctest --test-dir build
```

## Current Status

- Supports: `int`, `void`, functions (up to 6 params), recursion, global/local variables with scoping, if/else, while, for, basic arithmetic/relational/logical/bitwise operators.
- Compiles all [sample](sample) programs.
- 4/5 test suites pass (1 pre-existing integration failure).

## Self-Hosting Roadmap

The next goal is to make meo compile itself. See [self_hosting.md](self_hosting.md) for the full roadmap.

### What's Missing (Priority Order)

1. **Foundation** — `char`/`char*`, comments, unary operators, `break`/`continue`, arrays, `%`, `++`/`--`, compound assignment, pointer code gen
2. **Type System** — `struct`, `enum`, `typedef`, `static`, `extern`, function prototypes
3. **Preprocessor** — `#include`, `#define`, `#ifdef`
4. **Standard Library** — `malloc`, `printf`, `fopen`, `strlen`, etc.
5. **Bootstrap** — simplify source, first self-compilation, iterate

## Progress

See [100days.md](100days.md) for daily progress log.
