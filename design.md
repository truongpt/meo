# Design Improvements

Architectural and design-level issues that block extensibility and self-hosting.

## D1: No IR Between Parse and Codegen [Critical] ✅ RESOLVED

**Problem:** AST is built then immediately consumed and freed per top-level statement (`src/parse.c:116-121`). No multi-pass analysis possible.

**Impact:** Cannot do type checking, optimization, or cross-function analysis. Interpreter path is dead code (`src/ast.c:638-641`).

**Fix:** Introduce a retained IR phase. Parse builds AST, AST persists across the full translation unit, then a separate codegen pass walks it.

**Status:** Done. `ParseProc` now collects ASTs into `ParseParameter.ast_list[]`. `codegen_gen_all()` processes all collected ASTs after parsing completes.

## D2: AST Module IS the Codegen Engine [Critical] ✅ RESOLVED

**Problem:** `ast_gen()` at `src/ast.c:636-649` takes `ParseParameter*` and calls `Gen*` functions directly. The AST module does code generation traversal.

**Impact:** AST cannot be used independently. No clean separation between "what the program says" and "how to execute it."

**Fix:** Move `ast_compile()` out of `ast.c` into a dedicated `codegen.c` that walks the AST and calls `Gen*`.

**Status:** Done. `src/codegen.c` contains all codegen traversal. `src/ast.c` now contains only pure AST data structures (create, free).

## D3: 10 Files per New Operator [High]

**Problem:** Adding `%` requires touching: `meo.h` (enum), `lex.c` (tokenize), `ast.h` (AST type), `ast.c` (map + gen), `gen.h` (API), `gen.c` (trampoline), `gen_internal.h` (vtable), `x86_64_asm.c` (implement), `log.c` (string table).

**Impact:** Slow iteration. Easy to miss a file. `g_token_str` table silently desyncs from enum.

**Fix:** Use an operator descriptor table:
```c
typedef struct {
    TokenType tok;
    AstType   ast;
    int       prec;
    bool      right_assoc;
    GenFunc   gen_func;
    char*     name;
} OpDesc;
```
Single table drives lexing, parsing, codegen, and debug output.

## D4: 6 Global Registers, No Spill [Critical]

**Problem:** `cur_reg` is a file-scope global (`src/x86_64_asm.c:103`). 6 registers total. No spill to stack. Nested expressions or function calls with >3 args overflow.

**Impact:** Any non-trivial expression crashes the compiler.

**Fix:** Per-function register state. Implement spill slots on the stack when register pressure exceeds capacity.

## D5: `void*` Everywhere Destroys Type Safety [Medium]

**Problem:** `ParseOpen(void** prm, void* scan_prm, void* gen_prm)` — every module parameter is `void*`, requiring blind casts.

**Impact:** Cannot catch type mismatches at compile time. Swapping modules is fragile.

**Fix:** Use forward-declared opaque types:
```c
typedef struct LexContext LexContext;
typedef struct GenContext GenContext;
```

## D6: `exit(1)` on Every Error [High]

**Problem:** 17+ call sites use `exit(1)` after logging. No error recovery, no error counting. Cannot report multiple errors.

**Impact:** Useless for real development. First syntax error kills the process.

**Fix:** Return error codes from every function. Parser recovers by synchronizing to `;` or `}` and continues. Accumulate errors, report count at end.

## D7: Symbol Table is O(n) Linear Scan [Medium]

**Problem:** `symtable_find` at `src/symtable.c:49-65` scans all entries. `symtable_clear_level` marks level=-1 but never reclaims space (`cur_pos` only grows).

**Impact:** Slow on large programs. Symbol table fills up permanently with dead entries.

**Fix:** Hash table for lookup. Stack-based scope: `cur_pos` decrements on scope exit.

## D8: Parser Directly Drives Codegen [High]

**Problem:** `src/parse.c:119`: `ast_gen(parse_prm, node)` is called inside the parser's main loop. Parser is responsible for both building AST and triggering codegen.

**Impact:** Cannot parse without codegen. Cannot separate compilation passes.

**Fix:** Parser returns a completed AST. A separate driver calls codegen.

## D9: `var_id` in Token Leaks Codegen into Lexer [Medium]

**Problem:** `Token.var_id` at `inc/meo.h:28` is set during parsing (`src/parse.c:151`) and used in codegen. The Token struct carries information from downstream modules backward.

**Impact:** Lexer/parsing/codegen boundaries are blurred.

**Fix:** Remove `var_id` from Token. Resolve variable IDs in the symbol table lookup, not in the token.

## D10: No Multi-File Compilation [Critical for Self-Hosting]

**Problem:** `src/meo.c` processes exactly one input file. No `#include`, no separate compilation, no linker interface.

**Impact:** Cannot compile meo's own multi-file source.

**Fix:** This is Phase 3 (preprocessor) + Phase 5 (bootstrap) of the self-hosting roadmap.

## D11: Scope Management is Manual and Fragile [Medium]

**Problem:** `var_level++` / `var_level--` scattered across `src/parse.c:234,272,288,309`. No guard pattern. One missed decrement corrupts all scope tracking.

**Impact:** Subtle scope bugs. Hard to maintain.

**Fix:** Scope guard struct that increments on enter, decrements on destroy (RAII-style in C via cleanup macros or explicit calls).

## D12: Mock Duplication Across Test Suites [Low]

**Problem:** `test/parse/mock_lex.cpp` and `test/ast/mock_lex.cpp` are byte-for-byte identical. `test/parse/mock_arch.cpp` fills 5 of 35 vtable slots; `test/gen/mock_arch.cpp` fills different slots.

**Impact:** Tests drift out of sync. Maintenance burden.

**Fix:** Shared mock library. Auto-generate vtable mocks from `GenFuncTable` definition.

## D13: Global Mutable State in All Modules [Medium]

**Problem:** `g_lex_prm[10]`, `g_parse_prm[10]`, `g_gen_prm[10]`, `cur_reg`, `var_on_stack`, `allocated_stack` — all file-scope globals.

**Impact:** Cannot compile multiple files in parallel. Cannot test in parallel.

**Fix:** Per-instance state passed via context pointers. No globals.

## D14: No Type System [Critical for Self-Hosting]

**Problem:** Types tracked as `int32_t type` fields but no type checking, no implicit conversions, no type inference. `src/ast.c:378-383` hardcodes `int` as only type.

**Impact:** Cannot compile code that uses `char`, `struct`, arrays, pointers correctly.

**Fix:** This is Phase 2 (Type System) of the self-hosting roadmap.

## D15: Interpreter Path is Dead Code [Low]

**Problem:** `is_interpret` branch at `src/ast.c:638-641` is commented out. Architecturally abandoned because fused parse-codegen pipeline makes it impractical.

**Impact:** Dead code. Confusing.

**Fix:** Remove or implement after D2 is resolved.

---

## Summary

| Severity | Count |
|----------|-------|
| Critical | 4 |
| High | 3 |
| Medium | 5 |
| Low | 3 |
| **Total** | **15** |

### Recommended Refactor Order

1. **D2** — Extract codegen from AST module (unblocks everything else)
2. **D1** — Retain AST across full translation unit
3. **D3** — Operator descriptor table (makes new features 1 file, not 10)
4. **D6** — Error recovery (makes compiler usable)
5. **D4** — Register allocator with spill
6. **D7** — Hash-based symbol table
7. Then tackle D8-D15 as needed for self-hosting
