# Meo subset c compiler

## Let's start
- **meo** is :cat: in Vietnamese, but now  it is subset c compliler
- The project is mainly used to study compiler.
- I use [100DaysOfCode](https://github.com/kallaway/100-days-of-code) strategy, that will make [small step](100days.md) everyday by [rule](https://github.com/kallaway/100-days-of-code/blob/master/rules.md)
- I start by following to [Compiler design in C - Allen I.Holub](https://holub.com/goodies/compiler/compilerDesignInC.pdf) and refer [A Compiler Writing Journey](https://github.com/DoctorWkt/acwj)

## Milestone
- [x] Run basic math operator +,-,*,/ on X86_64 GNU/Linux -> [v0.1](https://github.com/truongpt/meo/releases/tag/v0.1)
- [ ] Support variable
- [ ] Support IF condition
- [ ] Support WHILE condition
- [ ] Support FOR condition
- [ ] Support function call condition
- [ ] Todo and todo as :ocean: ... :satisfied:
- [ ] Self compile

## Design
- Using design as [four-pass compiler](system_struct.png), which is refered from [Compiler design in C - Allen I.Holub](https://holub.com/goodies/compiler/compilerDesignInC.pdf)
- Each module lexical analyzer, parser, code generation are designed as independent module, it can be easy to replace without any effect other part.

## Implementation
- First step: only supporting X86-64 + pass 2 (lexical analyzer, parser, code generation).
- Using back end of GCC to generate executed code.
- Using TDD with [catch2](https://github.com/catchorg/Catch2).
- Using [Google coding convention](https://google.github.io/styleguide/cppguide.html).
