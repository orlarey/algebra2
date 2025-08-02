# Helper function to create test executables
function(add_algebra_test test_name)
    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} algebra)
    
    # Add the test to CTest
    add_test(NAME ${test_name} COMMAND ${test_name})
    
    # Set test properties (optional)
    set_tests_properties(${test_name} PROPERTIES
        TIMEOUT 30
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()

# Create all test executables
add_algebra_test(test_tree)
add_algebra_test(test_hashcons)
add_algebra_test(test_abs)
add_algebra_test(test_string)
add_algebra_test(test_generic)
add_algebra_test(test_variables)
add_algebra_test(test_fixpoint)

# Optional: Create a custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS test_tree test_hashcons test_abs test_string test_generic test_variables test_fixpoint
    COMMENT "Running all algebra tests"
)