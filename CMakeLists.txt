option(USE_OPENVDB "Use OpenVDB to build the mesher" ON)
option(USE_JSON "Enable JSON parser" ON)

cmake_minimum_required(VERSION 3.20)
project(FluidVisualization LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TODO: Install to ${CMAKE_PROJECT_SOURCE}/bin
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
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

enable_testing()

# Add GLM
add_subdirectory(libs/glm)
include_directories(libs/glm)

# Add GLFW
option(GLFW_BUILD_TESTS off)
option(GLFW_BUILD_EXAMPLES off)
option(GLFW_BUILD_DOCS off)
add_subdirectory(libs/glfw)
include_directories(libs/glfw/include)
list(APPEND LIBRARIES glfw ${glfw_LIBRARIES})

# Add IMGUI
include_directories("${PROJECT_SOURCE_DIR}/libs/imgui")
include_directories("${PROJECT_SOURCE_DIR}/libs/imgui/backends")
set(imgui_DIR ${PROJECT_SOURCE_DIR}/libs/imgui)
file(GLOB imgui_FILES ${imgui_DIR}/*)
list(APPEND imgui_FILES ${imgui_DIR}/backends/imgui_impl_opengl3.cpp ${imgui_DIR}/backends/imgui_impl_glfw.cpp)
add_library(imgui STATIC ${imgui_FILES})

# Add NSearch Utils
include("${PROJECT_SOURCE_DIR}/etc/Ext_NSearch.cmake")
include_directories("${NEIGHBORHOOD_SEARCH_INCLUDE_DIR}")
link_directories("${NEIGHBORHOOD_SEARCH_LIBRARIES}")

# Add OpenVDB
if (USE_OPENVDB)
    message("-- Enable OpenVDB, building mesher")
    add_subdirectory("${PROJECT_SOURCE_DIR}/src/mesher/")
endif ()

# Add JSON Utils
if (USE_JSON)
    message("-- Enable JSON parser")
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    add_subdirectory("${PROJECT_SOURCE_DIR}/libs/json")
endif ()

add_subdirectory("${PROJECT_SOURCE_DIR}/src")
add_subdirectory("${PROJECT_SOURCE_DIR}/tests")
