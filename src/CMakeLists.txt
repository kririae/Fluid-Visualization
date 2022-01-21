set(CMAKE_CUDA_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CUDA_FLAGS}")
set(CMAKE_CUDA_FLAGS " -Xcompiler ${CMAKE_CUDA_FLAGS} -w -g --expt-relaxed-constexpr --expt-extended-lambda")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wpedantic -Wextra -Wshadow -Wpointer-arith -Wformat=2 -Wall")

# find_package(imgui REQUIRED)
include_directories("${PROJECT_SOURCE_DIR}/libs/include")

file(GLOB SOURCE
        "${PROJECT_SOURCE_DIR}/src/*.c*")
file(GLOB LIB_SOURCE
        "${PROJECT_SOURCE_DIR}/libs/src/*.c*")
list(REMOVE_ITEM SOURCE "${PROJECT_SOURCE_DIR}/src/main.cpp")

add_library(SPHLib ${SOURCE} ${LIB_SOURCE})
target_compile_options(SPHLib PUBLIC -O2)
set_target_properties(SPHLib PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
add_dependencies(SPHLib Ext_CompactNSearch glfw)
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_target_properties(SPHLib PROPERTIES COMPILE_ON_WINDOWS ON)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set_target_properties(SPHLib PROPERTIES COMPILE_ON_LINUX ON)
endif()

target_compile_definitions(SPHLib PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(SPHLib PUBLIC imgui glfw ${CMAKE_DL_LIBS} ${NEIGHBORHOOD_ASSEMBLY_NAME} nlohmann_json::nlohmann_json)

add_executable(SPHInterface "${PROJECT_SOURCE_DIR}/src/main.cpp")
target_compile_options(SPHInterface PUBLIC -O2)
target_link_libraries(SPHInterface PRIVATE SPHLib)

# install(TARGETS SPHLib DESTINATION ${PROJECT_BINARY_DIR}/bin)
# install(TARGETS SPHInterface DESTINATION ${PROJECT_BINARY_DIR}/bin)
