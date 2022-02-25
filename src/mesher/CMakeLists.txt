# Add external library
list(APPEND CMAKE_MODULE_PATH /usr/lib/cmake/OpenVDB)

CPMAddPackage(
    NAME openvdb
    GITHUB_REPOSITORY AcademySoftwareFoundation/openvdb
    GIT_TAG v9.0.0
    VERSION 9.0.0
    OPTIONS
        "OPENVDB_BUILD_BINARIES OFF"
        "OPENVDB_CORE_STATIC OFF"
        "OPENVDB_BUILD_VDB_LOD OFF"
        "OPENVDB_BUILD_VDB_VIEW OFF"
        "OPENVDB_BUILD_VDB_RENDER OFF"
        "OPENVDB_ENABLE_UNINSTALL OFF"
)
    
add_executable(VDBMesher main.cpp common.hpp VDBMesher.hpp VDBMesher.cpp)
add_dependencies(VDBMesher glad)
target_link_libraries(VDBMesher PUBLIC openvdb)
