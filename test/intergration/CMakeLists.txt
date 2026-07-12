add_executable(intergration_test
    parse_lex_test.cpp
    mock_arch.cpp
    main.cpp
    ${CMAKE_SOURCE_DIR}/src/parse.c
    ${CMAKE_SOURCE_DIR}/src/gen.c
    ${CMAKE_SOURCE_DIR}/src/ast.c
    ${CMAKE_SOURCE_DIR}/src/codegen.c
    ${CMAKE_SOURCE_DIR}/src/symtable.c
    ${CMAKE_SOURCE_DIR}/src/log.c
    ${CMAKE_SOURCE_DIR}/src/lex.c
)

target_include_directories(intergration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/test/inc
)

target_compile_options(intergration_test PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-g -O0 -fpermissive -Wnarrowing -Wno-write-strings>
)

add_test(NAME intergration_test COMMAND intergration_test)
