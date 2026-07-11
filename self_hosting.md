# Self-Hosting Roadmap

Goal: make meo compile itself.

## Phase 1: Foundation

- [ ] `char` / `char*` types
- [ ] `//` and `/* */` comments
- [ ] Unary operators (`-x`, `!x`, `~x`)
- [ ] `sizeof` operator
- [ ] `break` / `continue`
- [ ] Modulo `%`
- [ ] Increment/decrement (`++`/`--`)
- [ ] Compound assignment (`+=`, `-=`, `*=`, `/=`, etc.)
- [ ] Shift operators (`<<`, `>>`)
- [ ] Arrays (declaration, indexing)
- [ ] Fix pointer code gen (`&` address-of, `*` dereference)

## Phase 2: Type System

- [ ] `struct`
- [ ] `enum`
- [ ] `typedef`
- [ ] `static`
- [ ] `extern`
- [ ] `const`
- [ ] Function prototypes / forward declarations

## Phase 3: Preprocessor

- [ ] `#include`
- [ ] `#define` (object-like macros)
- [ ] `#define` (function-like macros)
- [ ] `#ifdef` / `#ifndef` / `#endif`
- [ ] `#if` / `#elif` / `#else`
- [ ] `#undef`
- [ ] `#pragma once` (or header guards)

## Phase 4: Standard Library Interface

- [ ] `malloc`, `free`, `realloc`
- [ ] `printf`, `fprintf`, `sprintf`, `snprintf`
- [ ] `fopen`, `fclose`, `fgetc`, `fseek`
- [ ] `strlen`, `strcmp`, `strncmp`, `strcpy`
- [ ] `memset`, `memcpy`, `memmove`
- [ ] `exit`, `abort`
- [ ] `va_list`, `va_start`, `va_end`
- [ ] `EOF`, `NULL`, `sizeof`
- [ ] `int32_t`, `uint8_t`, `bool`, `true`, `false`

## Phase 5: Bootstrap

- [ ] Simplify meo source to avoid unimplemented features
- [ ] Flatten headers into single translation unit
- [ ] Replace `struct` with raw offsets where possible
- [ ] First self-compilation attempt
- [ ] Fix issues and iterate
- [ ] Achieve full self-hosting

## Status

| Phase | Status |
|-------|--------|
| Phase 1: Foundation | Not started |
| Phase 2: Type System | Not started |
| Phase 3: Preprocessor | Not started |
| Phase 4: Stdlib Interface | Not started |
| Phase 5: Bootstrap | Not started |
