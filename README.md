# Meo subset c compiler

## Let's start
- **meo** is :cat: in Vietnamese, but now  it is subset c compliler
- The project is mainly used to study compiler.
- I use [100DaysOfCode](https://github.com/kallaway/100-days-of-code) strategy, that will make [small step](100days.md) everyday by [rule](https://github.com/kallaway/100-days-of-code/blob/master/rules.md)
- I study base on following.
  - Reading [Compiler design in C - Allen I.Holub](https://holub.com/goodies/compiler/compilerDesignInC.pdf)
  - Reading [Dragon book](https://www.amazon.com/Compilers-Principles-Techniques-Tools-2nd/dp/0321486811)
  - Reference [A Compiler Writing Journey](https://github.com/DoctorWkt/acwj)

## Design
- Using design as [four-pass compiler](system_struct.png), which is refered from [Compiler design in C - Allen I.Holub](https://holub.com/goodies/compiler/compilerDesignInC.pdf)
- Each module lexical analyzer, parser, code generation are designed as independent module, it can be easy to replace without any effect other part.
- Each module is designed as multi instance, I plan supporting paralell compiler to increase performance.

## Implementation
- First step: only supporting X86-64 + pass 2 (lexical analyzer, parser, code generation).
- Using back end of GCC to generate executed code.
- Using TDD with [catch2](https://github.com/catchorg/Catch2).
- Using [Google coding convention](https://google.github.io/styleguide/cppguide.html).

## Current status.
- Can compile all files in [sample](sample), ex [fibonacci.c](sample/fibonacci.c)
- Can compiler recursion, as the fibonacci recursion version [recursion.c](sample/recursion.c)

## Todo list
- [x] Support function.
- [x] Function call.
- [x] Global variable & local variable.
- [ ] Structure type.
- [ ] Pointer.
- [ ] Function pointer.
- [ ] All [operator](https://en.cppreference.com/w/c/language/operator_precedence).
- [ ] v..v...
- [ ] Self compile.
