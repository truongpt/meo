# Improvement Backlog

Issues found by code review, organized by severity. Fix these before adding new features.

## Critical (7)

- [ ] **C1** `strncmp` with `sizeof` in keyword matching — `src/lex.c:233-248`. Prefix match is broken, `into` matches `int`. Use `strcmp` instead.
- [ ] **C2** `push_back` is `char`, stores EOF (-1) ambiguously — `src/lex.c:19`. Change to `int push_back` with a `has_pushback` flag.
- [ ] **C3** `x86_64_var` heap buffer overflow — `src/x86_64_asm.c:246-250`. Uses `sizeof(var)` (pointer size) instead of `strlen(var)`. Fix: `strlen(var) + strlen("(%%rip)") + 1`.
- [ ] **C4** `x86_64_var` label memory leaked — `src/x86_64_asm.c:247-250`. `malloc`'d string never freed. Use arena/pool.
- [ ] **C5** `x86_64_local_var` same malloc leak — `src/x86_64_asm.c:263-265`.
- [ ] **C6** Register allocator not stack-aware — `src/x86_64_asm.c:103-105`. Caller-saved registers clobbered across `call` instructions.
- [ ] **C7** `is_op_reg` buffer over-read — `src/x86_64_asm.c:162`. `sizeof(*op_reg)` reads 8 bytes from short strings. Use `strcmp`.

## High (12)

- [ ] **H1** Lexer doesn't handle `\r` — `src/lex.c:295-306`. Add `c == '\r'` to whitespace skip.
- [ ] **H2** `!` without `=` sets no token — `src/lex.c:138-145`. Uninitialized token type.
- [ ] **H3** `read_string` union field confusion — `src/lex.c:270-272`. Use `t->str[i]` consistently.
- [ ] **H4** `read_number` no overflow check — `src/lex.c:276-288`. Silent overflow on large numbers.
- [ ] **H5** `ast_create_node` memcpy without NUL — `src/ast.c:258`. Use `strncpy` with size limit.
- [ ] **H6** `sprintf` to `id_str` can overflow — `src/ast.c:261`. Use `snprintf`.
- [ ] **H7** No `-i` argc check — `src/meo.c:30-32`. `argv[2]` out of bounds if only `-i` given.
- [ ] **H8** `system()` called even on parse error — `src/meo.c:61`. Check return values.
- [ ] **H9** Wrong callee-saved registers saved — `src/x86_64_asm.c:590-605`. Saves `r10-r15` (caller-saved) instead of `rbx, r12-r15` (callee-saved).
- [ ] **H10** Stack misalignment before `call` — `src/x86_64_asm.c:534-541`. 56 bytes of pushes + local vars may not be 16-byte aligned.
- [ ] **H11** `set_log_level` vs `set_mlog_level` name mismatch — `src/log.h:26` vs `src/log.c:77`.
- [ ] **H12** `x86_64_store` returns malloc'd pointer without tracking — `src/x86_64_asm.c:560-570`. Memory leak.

## Medium (12)

- [ ] **M1** `LexClose` double-close possible — `src/lex.c:74-87`.
- [ ] **M2** `ParseOpen` memset after avail set — `src/parse.c:72-81`. Reorder.
- [ ] **M3** `symtable_find` redundant checks — `src/symtable.c:49-65`. Simplify to `strcmp`.
- [ ] **M4** `symtable_remove` is a stub — `src/symtable.c:43-47`. Implement or use stack.
- [ ] **M5** `ast_map` never cleared on scope exit — `src/ast.c:662-666`.
- [ ] **M6** `read_string` no escape sequences — `src/lex.c:257-273`.
- [ ] **M7** `read_string` no EOF check — `src/lex.c:262`. Infinite loop on unterminated string.
- [ ] **M8** `read_identifier` no `_` support — `src/lex.c:222-224`. Non-standard.
- [ ] **M9** `factor` dereference is no-op — `src/parse.c:609-618`. `&` and `*` silently discarded.
- [ ] **M10** `x86_64_local_var` sprintf overflow — `src/x86_64_asm.c:262-264`. Use `snprintf`.
- [ ] **M11** Division by zero not checked — `src/x86_64_asm.c:290-298`.
- [ ] **M12** Include guards placed after `#include` — multiple headers.

## Low (15)

- [ ] **L1** Magic numbers — `8`, `16`, `10` used without names.
- [ ] **L2** Label buffers too small — `src/ast.c:293,327,364,394`. Use `snprintf`.
- [ ] **L3** Dead code: `TokenLongType`, `GenArm` — never used.
- [ ] **L4** No `const` correctness — `char*` where `const char*` needed.
- [ ] **L5** `g_token_str` fragile sync with enum — `src/log.c:38-75`.
- [ ] **L6** `MeoHandle` initializer missing third NULL — `src/meo.c:39`.
- [ ] **L7** `mlog` format not `const char*` — `src/log.h:27`.
- [ ] **L8** `tok2str` negative value accidental behavior — `src/log.c:82-88`.
- [ ] **L9** `ast_compile_pre_func` NULL leaf reliance — `src/ast.c:386-388`.
- [ ] **L10** `symtable_clear_level` doesn't compact — `src/symtable.c:122-138`.
- [ ] **L11** Inconsistent header include order.
- [ ] **L12** `x86_64_or/and` temp register exhaustion — `src/x86_64_asm.c:358-393`.
- [ ] **L13** No variadic argument support (6+ args).
- [ ] **L14** `reg_free` const-correctness — `src/x86_64_asm.c:551-553`.
- [ ] **L15** `x86_64_reg_backup/restore` uses wrong registers — caller vs callee saved.

## Summary

| Severity | Count |
|----------|-------|
| Critical | 7 |
| High | 12 |
| Medium | 12 |
| Low | 15 |
| **Total** | **46** |

### Recommended Fix Order

1. **C3, C7** — buffer overflow/over-read (crash risks)
2. **C4, C5** — memory leaks
3. **C1** — keyword matching bug (correctness)
4. **C2** — push_back portability
5. **H9, H10** — register saving and stack alignment (code generation correctness)
6. **H1, H8, H7** — basic robustness
7. **M6, M7, M8** — lexer completeness
8. Then tackle remaining items by phase
