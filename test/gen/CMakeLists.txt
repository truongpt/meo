add_executable(gen_test
    gen_test.cpp
    mock_arch.cpp
    main.cpp
    ${CMAKE_SOURCE_DIR}/src/gen.c
    ${CMAKE_SOURCE_DIR}/src/log.c
)

target_include_directories(gen_test PRIVATE
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/test/inc
)

target_compile_options(gen_test PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-g -O0 -fpermissive -Wnarrowing -Wno-write-strings>
)

add_test(NAME gen_test COMMAND gen_test)
