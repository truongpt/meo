# Progress of 100DaysOfCode
### Day 1
- Create this repo
- Learn how to start the project.

### Day 2
- Add templace code + makefile
- Using catch2 framework for testing.
- Study lexical analyzer.
  - https://github.com/DoctorWkt/acwj/tree/master/01_Scanner
  - https://holub.com/goodies/compiler/compilerDesignInC.pdf

### Day 3
- Read "1.3 A Recursive-Descent Expression Compiler" of Compiler design in C
- Add test case to confirm basic arithmetic token of Lexial Analyzer.
- Lexial Analyzer support basic arithmetic token -> current test case PASS ^^
- TODO: study BNF form ...

### Day 4
- Add some test case for lexial analyzer
- Read "1.2 Representing Computer Languages" of Compiler design in C

### Day 5,6
- Read "1.2 Representing Computer Languages" of Compiler design in C
- Study parser by "1.3.2 The Basic Parser" & "1.3.3 Improving the Parser" of Compiler design in C
- Prototype parser

### Day 7
- Study parser by "1.3.2 The Basic Parser" & "1.3.3 Improving the Parser" of Compiler design in C
- Prototype bare-bone of parser

## Day 8
- Study more about parser, a lot of theory :-(
  - https://medium.com/basecs/leveling-up-ones-parsing-game-with-asts-d7a6fc2400ff
  - https://github.com/DoctorWkt/acwj/tree/master/02_Parser
  - "1.3.2 The Basic Parser" & "1.3.3 Improving the Parser" of Compiler design in C
  
## Day 9
- Continue study parse by "Compiler design in C" book.
- Add new test case for basic arithmetic expression with tentative parse API. These all of them are FAIL -> But it is OK because of TDD, add test first, API processing will be implemented later ^^

## Day 10
- Continue study parse by "Compiler design in C" book.
- Scanning overview [Dragon book](https://en.wikipedia.org/wiki/Compilers:_Principles,_Techniques,_and_Tools)
- Add some new test case for parser.
- Prototype code generate to combine with parse to try some arithmetic expression ->　learning by doing.
- Currently result, basic arithmetic expression is correctly parser but -> :bug: "Segmentation fault (core dumped)" -> that is task for Day 11.
```
    $ echo "(1+2)*3*(4+5)*6;" > s.c
    $ cat s.c
    (1+2)*3*(4+5)*6;
    $ ./meo s.c
    r0 = 1
    r1 = 2
    r0 += r1
    r1 = 3
    r0 *= r1
    r1 = 4
    r2 = 5
    r1 += r2
    r0 *= r1
    r1 = 6
    r0 *= r1
    value = 486
    Number or identifier expected
    Missing semicolon
    Segmentation fault (core dumped)
```

## Day 11, 12
- Fix :bug: "Segmentation fault (core dumped)"
- Add and excute test code-gen function.
- Improve test parse by adding gen code mock
- Now, **meo** has full modules of pass-2 : lexial analyzer + parser + gen code :laughing:
- Still ton of unknowledge & unclear about compiler :worried: -> just keep SMALL step over and over by #100DaysOfCode.
- Next todo list.
  - Fix lexical unit test fail :collision: wtf
  - Add intergration test
  - Add code to gen current result to x86-64 asm.

## Day 13
- Fix lexical unit test fail.
- Study X86-64 instruction
  - https://www.systems.ethz.ch/sites/default/files/file/COURSES/2014%20FALL%20COURSES/2014_Fall_SPCA/lectures/x86_64_asm_cheat_sheet.pdf
  - https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf

## Day 14
- Support subtract, div operator.
- Add gen asm on x86-64 of current result.
- Using backend of gcc to generate asm to machine instruction.
- Achieve the first milestone :satisfied:
```
    $ cat sample/s4.c
    (2+3)/2*(7-5)+9/3;
    $ ./meo sample/s4.c
    $ ./a.out
    $ echo $?
    7

    $
    $ cat sample/s5.c
    (2+1)/2-(7-5)+9/3;
    $ ./meo sample/s5.c
    $ ./a.out
    $ echo $?
    2
```

## Day 15
- Consider next step, it can be compile sample [s.c](sample/s.c)
- Continue study chapper 2 "Input and Lexical Analysis" of "Compiler design in C" book.
- Add new test case for lexical analyzer

## Day 16
- Refine and add parser some identifier at lexical analyzer but ... -> :sleeping:
```
-------------------------------------------------------------------------------
lex test token: int type, L&R bracket, return
-------------------------------------------------------------------------------
lex_test.cpp:233
...............................................................................

lex_test.cpp:258: FAILED:
  REQUIRE( T.tok == t.tok )
with expansion:
  33759920 (0x20322b0) == 5

===============================================================================
test cases:   7 |   6 passed | 1 failed
assertions: 107 | 106 passed | 1 failed
```

## Day 17
- Fix error of lexical analyzer to support more token to cover next step.
- Study symbol table, parser ... It become more complicated.

## Day 18
- Reading about parser.
  - Compiler design in C
  - Compilers: Principles, Techniques, and Tools. [a.k.a Dragon book]
- Consider design Astract Syntax Tree
  - https://medium.com/basecs/leveling-up-ones-parsing-game-with-asts-d7a6fc2400ff

## Day 19
- Reset from zero, refine knowlege.
  -  Reading chapter 2 of Dragon book.

## Day 20
- Reading "2.3. Syntax-Directed Translation" of Dragon book.
- Consider design AST, add template code.

## Day 21
- Implement AST bit by bit.

## Day 22
- Continue study theory about parse.
- Refactoring to integrate AST -> code gen.
- Refactoring test to maping with new API of code gen.
- Consider how to support variable

## Day 23
- Add print to statement + interpreter support one.
- Implementing symbol table to support variable.

## Day 24
- Add simple version of symbol table (poor performance)
- Support scan & parse Equal, Identifier token
- Todo:
  - Gen code for Equal, Identifier to support simple version of variable.

## Day 25
- Struggling how to process Identifier with specific type and arithmetic operator. -> Need refactor AST.

## Day 26
- Supporting variable ...

## Day 27
- Just finish firs step to support variable in case interpreter.

## Day 28
- Add option to select interpreter or compiler.
- Tentative add x86-64 call printf to display result.
  - https://montcs.bloomu.edu/~bobmon/Code/Asm.and.C/hello-asms.html
  - https://stackoverflow.com/questions/38335212/calling-printf-in-x86-64-using-gnu-assembler/38335743

## Day 29
- Even still having alot of NG design and mess code, but currently variable is supported also compiler and interpreter.

## Day 30
- Add log function.
- Maintain test and consider add integration test.
- Consider refine current result.

## Day 31
- Add more mock function of ARCH to test parse.

## Day 32
- Remove interpreter, consider later if interest.
- Refine lexical analyzer.
- Add intergration test, with pseudo ASM code.

## Day 33
- Consider support COMPARISONS.
- Studying "5 Syntax-Directed Translation" of Dragon book.

## Day 34
- Update parse to support COMPARISONS (ex: relational operators <, <=, >, >=)
- Study by reading https://github.com/rui314/9cc

## Day 35
- Add gen ASM for relational operators <, <=, >, >=
- Support equal op ==, !=
- Slightly refactor parse syntax.
- Add parse token: if, else, while, do, for.

## Day 36
- Study how to parse IF statement?

## Day 37
- Tag v0.2 https://github.com/truongpt/meo/releases/tag/v0.2

## Day 38
- Add compiler option to prevent stupid mistake.
- Refactoring parser to prepare support IF statement.

## Day 39
- Support parse If statements.
- TODO: code gen for If statements.

## Day 40
- code gen for If statements.
- TODO: fix case if (1) {};

## Day 41
- Fix case if (1) {};
- Support WHILE statement.

## Day 42
- Support FOR statement.
- Support initialize when declare variable.
- Fix issue of Assignment operator.

## Day 43
- Support compile function step 1.

## Day 44
- Study how to return, how to pass parameter to function.
- prototype to confirm return executing.

## Day 45
- Update test case to compliane with C syntax.

## Day 46
- Fix memory leak -> pass valgrind test.
- Token analizer + Parser for bitwise + logic operator.

## Day 47
- Gen asm for bitwise + logic operator.
- Token analizer for string -> prepare for calling printf :D

## Day 48
- Consider how to implement function call.

## Day 49
- Parse function call.
- TODO: gen ASM of function call.

## Day 50
- Support simple version function call, support max 6 input parameters.
- Remove tentative print statement, because now it can be link libc to call printf function.

## Day 51
- Study how to support local variable.

### Day 52
- Reading "2.7 Symbol Tables" p.85 Dragon book.
- How to manage local variable and global variable? need multi symbol table?
- How to manage local variable following to scope.
- How to generate asm code for local variable ?

### Day 53
- **Done**
  - Global variable: + slightly update, meo supported global variable.
  - Local variable: need alot of work.
    - Add member of AstNode to separate global & local variable.
    - Update symbol table.
      - Manage local variable following to specific scope.
      - Mapping variable name with symbol from asm code.
- **Todo**
  - Consider how to calculate necessary stack size of function.
  - Support local variable.
  - Initialize global variable.

### Day 54
- **Done**
  - Add level for symbol table to manage specific scope.
  - Generate asm code for symbol variable.
    - error -> debugging.

- **Todo**
  - How to manage local variable at gen code? need more mapping table or shared currently symbol table? good design?
    - Don't use symbol table, because target is difference.
  - Consider how to calculate necessary stack size of function.
  - Support local variable.
  - Initialize global variable.
  - Maintain test? don't following to TDD :/

- **Note**
  - Support complete input parameter, now only input variable but not checking it is invalid or not?
  - What is good design?
  - Parkinson law? only 2x25 minutes/day for meo
  - Need improve processing declare varialbe. now it is stupid 

### Day 55
- Fix mistake local variable, current asm is using 64bit register -> stack area for each buffer must be 8.?

### Day 56
- **Done**
  - Terminated processing of the function, ret -> todo end label.
  - Fix problem not goto exit label in case IF statements.
  - Refactoring variable declare

- **Todo**
  - Reduce redundance ASM code in case variable. Notice for compare instruction.

### Day 57
- **Done**
  - Fix problem variable at pre condition of FOR loop, handle it as local variable.
  - Allocated stack size base on local variable.
- **Todo**
  - stack size = alignment of 16? need investigate

### Day 58
- **Done**
  - Support call function without input -> function(void).
  - Support received return value of function.
- **Doing**
  - Implementing funciton with non-void argument.

### Day 59
- **Done**
  -  Support function (just < 6 input parameters).
- **Todo**
  - Support pass expression to function -> re-chec ast_compile_arg
- **Node**
  - Careful of left var vs right var.

### Day 60
- **Done**
  - Support pass expression to function.
- **Todo**
  - How to backup register when calling function.

### Day 61
- **Done**
  - Fix problem because lack free register when return operator.
- **Todo**
  - Study call convention of x86-64.
  - Free register in case unary tree, example int main() {5;6;}
- **Node**
  - https://stackoverflow.com/questions/18024672/what-registers-are-preserved-through-a-linux-x86-64-function-call

### Day 62
- Study x86: https://stackoverflow.com/tags/x86/info
- Consider improve ASM code, current it seems stupid.
- Consider how to release register in case unary tree.