cmake_minimum_required(VERSION 3.10)
project(meo LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(meo
    src/meo.c
    src/lex.c
    src/parse.c
    src/symtable.c
    src/ast.c
    src/codegen.c
    src/gen.c
    src/x86_64_asm.c
    src/log.c
)

target_include_directories(meo PRIVATE inc src)

target_compile_options(meo PRIVATE -Wall -Werror)

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
