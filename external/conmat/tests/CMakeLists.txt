# Create test executable
add_executable(test_conmat
  test_conmat.cpp
)

target_link_libraries(test_conmat PUBLIC
  conmat::conmat
)

# Add test to CTest
add_test(NAME conmat_tests COMMAND test_conmat)
