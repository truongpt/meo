# Design Improvements

Architectural and design-level issues that block extensibility and self-hosting.

## D1: No IR Between Parse and Codegen [Critical] ✅ RESOLVED

**Problem:** AST is built then immediately consumed and freed per top-level statement. No multi-pass analysis possible.

**Fix:** `ParseProc` now collects ASTs into `ParseParameter.ast_list[]`. `codegen_gen_all()` processes all collected ASTs after parsing completes.

## D2: AST Module IS the Codegen Engine [Critical] ✅ RESOLVED

**Problem:** `ast_gen()` in `ast.c` called `Gen*` functions directly. AST module did code generation traversal.

**Fix:** `src/codegen.c` contains all codegen traversal. `src/ast.c` is pure AST data structures (create, free).

## D3: 10 Files per New Operator [High] ✅ RESOLVED

**Problem:** Adding an operator required touching 10 files across 4 modules.

**Fix:** Operator descriptor table (`src/op_table.c`). Pratt parser replaces 8 hardcoded precedence functions. Codegen dispatch table maps AstType → Gen function. Adding `%` now requires 6 edits instead of 10.

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

## D8: Parser Directly Drives Codegen [High] ✅ RESOLVED

**Problem:** Parser called `ast_gen()` inside its main loop. Parser responsible for both building AST and triggering codegen.

**Fix:** D1 resolved this. Parser collects ASTs into `ast_list[]`. Separate `codegen_gen_all()` pass processes them.

## D9: `var_id` in Token Leaks Codegen into Lexer [Medium]

**Problem:** `Token.var_id` at `inc/meo.h:28` is set during parsing and used in codegen. The Token struct carries information from downstream modules backward.

**Impact:** Lexer/parsing/codegen boundaries are blurred.

**Fix:** Remove `var_id` from Token. Resolve variable IDs in the symbol table lookup, not in the token.

## D10: No Multi-File Compilation [Critical for Self-Hosting]

**Problem:** `src/meo.c` processes exactly one input file. No `#include`, no separate compilation, no linker interface.

**Impact:** Cannot compile meo's own multi-file source.

**Fix:** This is Phase 3 (preprocessor) + Phase 5 (bootstrap) of the self-hosting roadmap.

## D11: Scope Management is Manual and Fragile [Medium]

**Problem:** `var_level++` / `var_level--` scattered across `src/parse.c`. No guard pattern. One missed decrement corrupts all scope tracking.

**Impact:** Subtle scope bugs. Hard to maintain.

**Fix:** Scope guard struct that increments on enter, decrements on destroy (RAII-style in C via cleanup macros or explicit calls).

## D12: Mock Duplication Across Test Suites [Low]

**Problem:** `test/parse/mock_lex.cpp` and `test/ast/mock_lex.cpp` are byte-for-byte identical. Mock vtable fills differ across test targets.

**Impact:** Tests drift out of sync. Maintenance burden.

**Fix:** Shared mock library. Auto-generate vtable mocks from `GenFuncTable` definition.

## D13: Global Mutable State in All Modules [Medium]

**Problem:** `g_lex_prm[10]`, `g_parse_prm[10]`, `g_gen_prm[10]`, `cur_reg`, `var_on_stack`, `allocated_stack` — all file-scope globals.

**Impact:** Cannot compile multiple files in parallel. Cannot test in parallel.

**Fix:** Per-instance state passed via context pointers. No globals.

## D14: No Type System [Critical for Self-Hosting]

**Problem:** Types tracked as `int32_t type` fields but no type checking, no implicit conversions, no type inference. `int` is the only real type.

**Impact:** Cannot compile code that uses `char`, `struct`, arrays, pointers correctly.

**Fix:** This is Phase 2 (Type System) of the self-hosting roadmap.

## D15: Interpreter Path is Dead Code [Low] ✅ RESOLVED

**Problem:** `is_interpret` branch was commented out. Architecturally abandoned.

**Fix:** D2 resolved this. `is_interpret` removed from codegen path.

---

## Summary

| Severity | Count | Resolved |
|----------|-------|----------|
| Critical | 4 | 2 (D1, D2) |
| High | 3 | 2 (D3, D8) |
| Medium | 5 | 0 |
| Low | 3 | 1 (D15) |
| **Total** | **15** | **5** |

### Remaining Issues by Priority

1. **D4** — Register allocator with spill [Critical] — blocks complex expressions
2. **D6** — Error recovery [High] — blocks usability
3. **D14** — Type system [Critical] — blocks self-hosting Phase 2
4. **D10** — Multi-file compilation [Critical] — blocks self-hosting Phase 3
5. **D7** — Hash-based symbol table [Medium] — performance
6. **D9** — Remove var_id from Token [Medium] — cleanliness
7. **D11** — Scope guard [Medium] — correctness
8. **D5** — Type-safe module interfaces [Medium] — cleanliness
9. **D13** — Eliminate global state [Medium] — parallelism
10. **D12** — Shared test mocks [Low] — maintenance
