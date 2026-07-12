# Meo Compiler Architecture

A subset C compiler targeting x86-64, using TDD with Catch2 and CMake.

## Compilation Pipeline

```
Source File (.c)
      │
      ▼
┌─────────────┐
│    Lexer     │  lex.c ─ single-character lookahead tokenizer
│  LexProc()   │  Produces: Token stream
└──────┬──────┘
       │
       ▼
┌─────────────┐
│   Parser     │  parse.c ─ recursive descent + Pratt expression parser
│  ParseProc()  │  Consumes: Token stream via LexProc
└──────┬──────┘  Produces: AST roots in parse_prm->ast_list[]
       │         Side effects: SymbolTable populated, scope managed
       ▼
┌─────────────┐
│  Codegen     │  codegen.c ─ recursive AST walk
│codegen_gen() │  Consumes: All ASTs from ast_list[]
└──────┬──────┘  Calls: Gen* functions → vtable → x86_64 backend
       │         Side effects: AT&T x86-64 assembly written to out.s
       ▼
┌─────────────┐
│   Assembler  │  system("cc -no-pie -o a.out out.s")
│   + Linker   │  Delegates to GCC
└──────┬──────┘
       │
       ▼
  Executable (a.out)
```

## Module Map

```
inc/                          src/
─────────                     ─────────
meo.h          ───────────►   meo.c            Main entry, CLI, pipeline orchestration
  Token struct                  lex.c            Lexer implementation
  TokenType enum                parse.c          Parser (Pratt + recursive descent)
  Size constants                parse_internal.h ParseParameter struct (parser state)
                                ast.c            AST node construction and destruction
lex.h           ───────────►   codegen.c        Code generation traversal
                                codegen.h        Codegen public API
parse.h         ───────────►
                                gen.c            Backend abstraction (vtable trampoline)
gen.h           ───────────►   gen_internal.h   GenFuncTable vtable definition
                                x86_64_asm.c     x86-64 backend implementation
symtable.h      ───────────►   symtable.c       Symbol table (flat array, level scoping)
                                op_table.c       Operator descriptor table + token names
op_table.h      ───────────►
                                log.c            Leveled logging with ANSI colors
error_code.h    ───────────►   log.h            Log levels, mlog(), tok2str()
```

## Key Data Structures

### Token (`inc/meo.h`)
```c
typedef struct Token {
    int32_t tok;            // TokenType enum value
    union {
        int32_t value;      // TokenNumber: numeric value
        struct {
            char id_str[32]; // TokenIdentifier: identifier string
            bool left_value;
        };
        char str[100];      // TokenString: string literal
    };
    int32_t var_id;         // Set during parsing, used in codegen
} Token;
```

### AstNode (`src/ast.h`)
```c
typedef struct AstNode {
    int32_t type;           // AstType enum (31 types)
    union {
        int32_t value;      // AstNumber
        char id_str[32];    // AstIdentifier, AstLeftVar
        char str[100];      // AstString
        char exit_label[10];// AstReturn (set during codegen pre-pass)
    };
    int32_t var_type;       // AstVarGlobal or AstVarLocal
    int32_t var_level;      // Scope level
    int32_t arg_order;      // Argument position
    struct AstNode* left;
    struct AstNode* mid;    // Used by AstIf, AstFunc
    struct AstNode* right;
} AstNode;
```

### OpDesc (`inc/op_table.h`)
```c
typedef struct OpDesc {
    enum TokenType tok;     // Source token
    int32_t    ast;         // Corresponding AST type
    int        prec;        // Precedence (1=lowest, 9=highest)
    bool       right_assoc;// Associativity
    const char name[4];     // Display string
} OpDesc;
```

### GenFuncTable (`src/gen_internal.h`)
Virtual dispatch table with 35 function pointers for backend operations:
arithmetic, comparison, logical, jumps, function prologue/epilogue, memory access.

### ParseParameter (`src/parse_internal.h`)
Parser runtime context: lex/gen handles, symbol table, current token, scope level, variable label map, AST list.

### SymbolTable (`src/symtable.c`)
Flat array of 1024 entries. Level-based scoping via `var_level` counter. Lookup walks from current level down to 0.

## AST Node Types (31)

| Category | Types |
|----------|-------|
| Arithmetic | `AstPlus`, `AstMinus`, `AstMul`, `AstDiv` |
| Comparison | `AstLT`, `AstLE`, `AstGT`, `AstGE`, `AstEQ`, `AstNE` |
| Bitwise/Logical | `AstBitAnd`, `AstBitOr`, `AstBitXor`, `AstAnd`, `AstOr` |
| Values | `AstNumber`, `AstString`, `AstIdentifier`, `AstLeftVar` |
| Declarations | `AstDeclare`, `AstIntType`, `AstVoidType` |
| Assignment | `AstAssign` |
| Control Flow | `AstIf`, `AstWhile` |
| Functions | `AstFunc`, `AstFuncArg`, `AstFuncCall`, `AstArgPass`, `AstReturn` |
| Sequencing | `AstLink` |

### Node Tree Semantics
- `AstLink(left, right)` — statement sequence (linked list)
- `AstIf(cond, true_stmt, false_stmt)` — uses left/mid/right
- `AstFunc(name, args, body)` — uses left/mid/right
- `AstWhile(cond, body)` — left=cond, right=body
- `AstDeclare(type, name, var_type)` — left=type, right=name

## x86-64 Backend

### Register Allocation
- 6 scratch registers: `%r10-%r15`
- Linear stack allocator: `reg_alloc()` / `reg_free()`
- No spill to stack, no liveness analysis
- Crashes on register exhaustion (>6 live values)

### System V AMD64 ABI
- Args: `%rdi`, `%rsi`, `%rdx`, `%rcx`, `%r8`, `%r9`
- Return: `%rax`
- Callee-saved: `%rbx`, `%r12-%r15` (currently saves wrong set)

### Generated Assembly Pattern
```asm
# Function prologue
pushq %r10..%r15       # (bug: should save callee-saved)
pushq %rbp
movq  %rsp, %rbp
subq  $N, %rsp         # N = local_var_size, 16-byte aligned

# Function body
...

# Function epilogue
.Lexit:
addq  $N, %rsp
popq  %rbp
popq  %r10..%r15
ret
```

## Supported C Subset

| Feature | Status |
|---------|--------|
| `int` and `void` types | Working |
| Global variables | Working |
| Local variables with init | Working |
| Functions (up to 6 params) | Working |
| Recursive calls | Working |
| `if`/`else` | Working |
| `while` loops | Working |
| `for` loops (desugared) | Working |
| Nested scopes `{ }` | Working |
| `return` statements | Working |
| `+`, `-`, `*`, `/` | Working |
| `<`, `<=`, `>`, `>=`, `==`, `!=` | Working |
| `&`, `\|`, `^` (bitwise) | Working |
| `&&`, `\|\|` (non-short-circuit) | Working |
| `=` assignment | Working |
| String literals | Working |
| `char` / `char*` | Not supported |
| `long`, `float`, `double` | Not supported |
| `struct`, `union`, `enum` | Not supported |
| `typedef` | Not supported |
| Arrays | Not supported |
| Pointer arithmetic | Parsing only, codegen incomplete |
| Preprocessor `#include` | Not supported |
| Multi-file compilation | Not supported |

## Design Issues (from design.md)

| Issue | Severity | Status |
|-------|----------|--------|
| D1: No IR between parse and codegen | Critical | Resolved |
| D2: AST module is codegen engine | Critical | Resolved |
| D3: 10 files per new operator | High | Resolved |
| D4: 6 global registers, no spill | Critical | Open |
| D5: void* everywhere | Medium | Open |
| D6: exit(1) on every error | High | Open |
| D7: Symbol table O(n) scan | Medium | Open |
| D8: Parser directly drives codegen | High | Resolved |
| D9: var_id leaks codegen into token | Medium | Open |
| D10: No multi-file compilation | Critical | Open |
| D11: Manual scope management | Medium | Open |
| D12: Mock duplication | Low | Open |
| D13: Global mutable state | Medium | Open |
| D14: No type system | Critical | Open |
| D15: Dead interpreter code | Low | Resolved |

## Build Commands

```bash
./build.sh              # Build (auto-detects CMakeLists.txt changes)
ctest --test-dir build  # Run tests (4/5 pass, 1 pre-existing SIGILL)
./run_samples.sh        # Run all 16 sample programs
```
