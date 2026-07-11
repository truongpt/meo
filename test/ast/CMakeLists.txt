add_executable(ast_test
    ast_test.cpp
    main.cpp
    mock_lex.cpp
    ${CMAKE_SOURCE_DIR}/src/ast.c
    ${CMAKE_SOURCE_DIR}/src/symtable.c
    ${CMAKE_SOURCE_DIR}/src/gen.c
    ${CMAKE_SOURCE_DIR}/src/parse.c
    ${CMAKE_SOURCE_DIR}/src/x86_64_asm.c
    ${CMAKE_SOURCE_DIR}/src/log.c
)

target_include_directories(ast_test PRIVATE
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/test/inc
)

target_compile_options(ast_test PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-g -O0 -fpermissive -Wnarrowing>
)

add_test(NAME ast_test COMMAND ast_test)
