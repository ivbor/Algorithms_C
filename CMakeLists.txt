cmake_minimum_required(VERSION 3.20)

project(AlgorithmsC VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
enable_testing()

option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

set(ALGORITHMS_SOURCES
    src/algorithms/array_count_sort.c
    src/algorithms/binary_search.c
    src/algorithms/compare.c
    src/algorithms/counting_sort.c
    src/algorithms/insertion_sort.c
    src/algorithms/merge_sort.c
    src/algorithms/quick_sort.c
)

set(STRUCTURES_SOURCES
    src/structures/matrix_view.c
    src/structures/queue.c
    src/structures/stack.c
    src/structures/vector.c
)

set(UTILITY_SOURCES
    src/utils/logger.c
)

add_library(algorithms_c STATIC
    ${ALGORITHMS_SOURCES}
    ${STRUCTURES_SOURCES}
    ${UTILITY_SOURCES}
)

target_include_directories(algorithms_c PUBLIC include)

target_compile_options(algorithms_c PRIVATE
    $<$<C_COMPILER_ID:GNU,Clang>:-Wall>
    $<$<C_COMPILER_ID:GNU,Clang>:-Wextra>
    $<$<C_COMPILER_ID:GNU,Clang>:-Wpedantic>
)

if(ENABLE_COVERAGE)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(algorithms_c PRIVATE --coverage -O0 -g)
        target_link_options(algorithms_c PRIVATE --coverage)
    endif()
endif()

set(TEST_SOURCES
    tests/helpers/all_tests.c
    tests/unit/test_array_count_sort.c
    tests/unit/test_vector.c
    tests/unit/test_stack.c
    tests/unit/test_queue.c
    tests/unit/test_sorting.c
    tests/unit/test_binary_search.c
    src/utils/minunit.c
)

add_executable(algorithms_c_tests ${TEST_SOURCES})

target_link_libraries(algorithms_c_tests PRIVATE algorithms_c)

if(ENABLE_COVERAGE AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(algorithms_c_tests PRIVATE --coverage -O0 -g)
    target_link_options(algorithms_c_tests PRIVATE --coverage)
endif()

add_test(NAME unit_tests COMMAND algorithms_c_tests)

add_executable(sorting_stress tests/stress/sorting_stress.c)

target_link_libraries(sorting_stress PRIVATE algorithms_c)

add_custom_target(stress DEPENDS sorting_stress)

if(ENABLE_COVERAGE)
    find_program(GCOVR_PATH gcovr)
    if(GCOVR_PATH)
        add_custom_target(coverage-report
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND ${GCOVR_PATH}
                --root ${CMAKE_SOURCE_DIR}
                --object-directory ${CMAKE_BINARY_DIR}
                --xml coverage.xml
                --html --html-details coverage.html
                --branches
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS algorithms_c_tests
        )
    else()
        add_custom_target(coverage-report
            COMMAND ${CMAKE_COMMAND} -E echo "gcovr not found"
        )
    endif()
else()
    add_custom_target(coverage-report
        COMMAND ${CMAKE_COMMAND} -E echo "Reconfigure with -DENABLE_COVERAGE=ON"
    )
endif()

find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
set(CLANG_TIDY_SOURCES
    ${ALGORITHMS_SOURCES}
    ${STRUCTURES_SOURCES}
    ${UTILITY_SOURCES}
)
if(CLANG_TIDY_EXECUTABLE)
    add_custom_target(clang-tidy
        COMMAND ${CLANG_TIDY_EXECUTABLE}
            -p ${CMAKE_BINARY_DIR}
            ${CLANG_TIDY_SOURCES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
else()
    add_custom_target(clang-tidy
        COMMAND ${CMAKE_COMMAND} -E echo "clang-tidy not found"
    )
endif()

install(TARGETS algorithms_c EXPORT AlgorithmsCTargets)
install(DIRECTORY include/ DESTINATION include)
