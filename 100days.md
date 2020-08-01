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
- Add and excute code gen function.
- Improve test parse by adding gen code mock
- Now, **meo** has full of modules of pass-2 : lexial analyzer + parser + gen code :laughing:
- Still ton of unknowledge & unclear about compiler :worried: -> just keeping SMALL step over and over by #100DaysOfCode.
- Next todo list.
  - Fix lexical unit test fail :collision: wtf
  - Add intergration test
  - Add code to gen current result to x86-64 asm.
