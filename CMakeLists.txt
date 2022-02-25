option(USE_OPENVDB "Use OpenVDB to build the mesher" ON)
option(USE_JSON "Enable JSON parser" ON)

cmake_minimum_required(VERSION 3.20)
project(FluidVisualization LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include cpm for package managing
include("${PROJECT_SOURCE_DIR}/etc/get_cpm.cmake")

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin")

# find_package(CUDA REQUIRED)
find_package(OpenMP)
if (OPENMP_FOUND OR OpenMP_CXX_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif ()

# Add glad to replace libs/
CPMAddPackage(
    NAME glad
    GITHUB_REPOSITORY Dav1dde/glad
    GIT_TAG v0.1.36
    VERSION 0.1.36
    OPTIONS
        "GLAD_API gl=3.3"
        "GLAD_PROFILE core"
        "GLAD_SPEC gl"
        "GLAD_NO_LOADER OFF"
        "GLAD_INSTALL ON"
)

include_directories(${glad_BINARY_DIR}/include)

# Add glm
CPMAddPackage("gh:g-truc/glm#0.9.9.8")
CPMAddPackage(
    NAME glfw
    GITHUB_REPOSITORY glfw/glfw 
    GIT_TAG 3.3.6
    OPTIONS 
        "GLFW_BUILD_TESTS off" 
        "GLFW_BUILD_EXAMPLES off" 
        "GLFW_BUILD_DOCS off"
)

# Add IMGUI
CPMAddPackage(
    NAME imgui
    GITHUB_REPOSITORY ocornut/imgui
    GIT_TAG v1.87
    VERSION 1.87
    DOWNLOAD_ONLY
)
include_directories("${imgui_SOURCE_DIR}")
include_directories("${imgui_SOURCE_DIR}/backends")
file(GLOB imgui_FILES ${imgui_SOURCE_DIR}/*)
list(APPEND imgui_FILES ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)
add_library(imgui STATIC ${imgui_FILES})

# Add NSearch Utils
CPMAddPackage(
    NAME CompactNSearch
    GITHUB_REPOSITORY InteractiveComputerGraphics/CompactNSearch
    GIT_TAG b8c41fcefd6a8a7896cf3972dcb92aa407969ed7
    OPTIONS "USE_DOUBLE_PRECISION OFF" "BUILD_DEMO OFF"
)
include_directories(${CompactNSearch_BINARY_DIR}/include)

# Add OpenVDB
if (USE_OPENVDB)

    add_subdirectory("${PROJECT_SOURCE_DIR}/src/mesher/")
endif ()

# Add JSON Utils
if (USE_JSON)
    CPMAddPackage(
        NAME json
        GITHUB_REPOSITORY nlohmann/json
        GIT_TAG v3.10.5
        VERSION 3.10.5
        OPTIONS "JSON_BuildTests OFF"
    )
endif()

# Add googletest
CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    GIT_TAG release-1.11.0
    VERSION 1.11.0
    OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt" "BUILD_GMOCK OFF"
)

enable_testing()
add_subdirectory("${PROJECT_SOURCE_DIR}/src")
add_subdirectory("${PROJECT_SOURCE_DIR}/tests")
