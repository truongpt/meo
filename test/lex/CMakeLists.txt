add_executable(lex_test
    lex_test.cpp
    main.cpp
    ${CMAKE_SOURCE_DIR}/src/lex.c
    ${CMAKE_SOURCE_DIR}/src/log.c
    ${CMAKE_SOURCE_DIR}/src/op_table.c
)

target_include_directories(lex_test PRIVATE
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/test/inc
)

target_compile_options(lex_test PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-g -O0 -fpermissive -Wnarrowing -Wno-write-strings>
)

add_test(NAME lex_test COMMAND lex_test)
