include("${PROJECT_SOURCE_DIR}/etc/Ext_GTest.cmake")

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/libs/include")

macro(add_test_exe test_name)
  add_executable("${test_name}" "${test_name}.cpp")
  set_target_properties(SPHLib PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
  target_link_libraries("${test_name}" PUBLIC SPHLib gtest_main gtest)
  add_test(${test_name} ${test_name})
endmacro(add_test_exe)

add_test_exe(NSearchCorrectness)
#add_test_exe(PBDSolverCorrectness)