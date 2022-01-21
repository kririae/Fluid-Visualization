# Add external library
list(APPEND CMAKE_MODULE_PATH /usr/lib/cmake/OpenVDB)

find_package(OpenVDB REQUIRED)
set(OPENVDB_BUILD_CORE ON)
set(OPENVDB_BUILD_VDB_PRINT OFF)
set(USE_TBB OFF)

include_directories("${PROJECT_SOURCE_DIR}/libs/include")
add_executable(VDBMesher main.cpp common.hpp VDBMesher.hpp VDBMesher.cpp)
target_compile_options(VDBMesher PUBLIC -O2)
target_link_libraries(VDBMesher PUBLIC OpenVDB::openvdb)
