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

set(TEST_SUPPORT_SOURCES
    src/utils/minunit.c
)

set(AC_PUBLIC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(AC_WARNING_FLAGS
    $<$<C_COMPILER_ID:GNU,Clang>:-Wall>
    $<$<C_COMPILER_ID:GNU,Clang>:-Wextra>
    $<$<C_COMPILER_ID:GNU,Clang>:-Wpedantic>
)

function(ac_configure_static_library target)
    target_include_directories(${target} PUBLIC ${AC_PUBLIC_INCLUDE_DIR})
    target_compile_options(${target} PRIVATE ${AC_WARNING_FLAGS})
    if(ENABLE_COVERAGE AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE --coverage -O0 -g)
        target_link_options(${target} PRIVATE --coverage)
    endif()
endfunction()

add_library(algorithms_c_utils STATIC ${UTILITY_SOURCES})
ac_configure_static_library(algorithms_c_utils)

add_library(algorithms_c_algorithms STATIC ${ALGORITHMS_SOURCES})
ac_configure_static_library(algorithms_c_algorithms)

target_link_libraries(algorithms_c_algorithms
    PUBLIC algorithms_c_utils
)

add_library(algorithms_c_structures STATIC ${STRUCTURES_SOURCES})
ac_configure_static_library(algorithms_c_structures)

target_link_libraries(algorithms_c_structures
    PUBLIC algorithms_c_utils
)

add_library(algorithms_c INTERFACE)
target_link_libraries(algorithms_c
    INTERFACE
        algorithms_c_algorithms
        algorithms_c_structures
        algorithms_c_utils
)
target_include_directories(algorithms_c INTERFACE ${AC_PUBLIC_INCLUDE_DIR})
add_library(AlgorithmsC::algorithms_c ALIAS algorithms_c)

set(UNIT_TEST_TARGETS
    array_count_sort
    vector
    stack
    queue
    sorting
    binary_search
)

set(UNIT_TEST_EXECUTABLES)

foreach(test_name IN LISTS UNIT_TEST_TARGETS)
    set(target "algorithms_c_${test_name}_tests")
    add_executable(${target}
        tests/unit/test_${test_name}.c
        ${TEST_SUPPORT_SOURCES}
    )
    target_link_libraries(${target} PRIVATE algorithms_c)
    target_compile_options(${target} PRIVATE ${AC_WARNING_FLAGS})
    if(ENABLE_COVERAGE AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE --coverage -O0 -g)
        target_link_options(${target} PRIVATE --coverage)
    endif()
    list(APPEND UNIT_TEST_EXECUTABLES ${target})
    add_test(NAME ${test_name} COMMAND ${target})
endforeach()

add_executable(sorting_stress tests/stress/sorting_stress.c)
target_link_libraries(sorting_stress PRIVATE algorithms_c)
target_compile_options(sorting_stress PRIVATE ${AC_WARNING_FLAGS})
if(ENABLE_COVERAGE AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(sorting_stress PRIVATE --coverage -O0 -g)
    target_link_options(sorting_stress PRIVATE --coverage)
endif()

add_custom_target(stress DEPENDS sorting_stress)

if(ENABLE_COVERAGE)
    find_program(GCOVR_PATH gcovr)
    if(GCOVR_PATH)
        add_custom_target(coverage-report
            COMMAND
                ${CMAKE_COMMAND} -E env
                CTEST_OUTPUT_ON_FAILURE=1
                ${CMAKE_CTEST_COMMAND}
                --force-new-ctest-process
                --output-on-failure
            COMMAND ${GCOVR_PATH}
                --root ${CMAKE_SOURCE_DIR}
                --object-directory ${CMAKE_BINARY_DIR}
                --xml coverage.xml
                --html --html-details coverage.html
                --txt coverage.txt
                --txt-metric branch
                --print-summary
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS ${UNIT_TEST_EXECUTABLES}
        )
    else()
        add_custom_target(coverage-report
            COMMAND ${CMAKE_COMMAND} -E echo "gcovr not found"
            COMMAND ${CMAKE_COMMAND} -E false
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
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
    add_custom_target(clang-tidy
        COMMAND ${CMAKE_COMMAND} -E echo "clang-tidy not found"
    )
endif()

install(
    TARGETS
        algorithms_c
        algorithms_c_algorithms
        algorithms_c_structures
        algorithms_c_utils
    EXPORT AlgorithmsCTargets
)

install(DIRECTORY include/ DESTINATION include)
