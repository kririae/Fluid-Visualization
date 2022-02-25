set(CMAKE_CUDA_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CUDA_FLAGS}")
set(CMAKE_CUDA_FLAGS " -Xcompiler ${CMAKE_CUDA_FLAGS} -w -g --expt-relaxed-constexpr --expt-extended-lambda")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wpedantic -Wextra -Wshadow -Wpointer-arith -Wformat=2 -Wall")

file(GLOB SOURCE
        "${PROJECT_SOURCE_DIR}/src/*.c*")
list(REMOVE_ITEM SOURCE "${PROJECT_SOURCE_DIR}/src/main.cpp")

add_library(SPHLib ${SOURCE})
set_target_properties(SPHLib PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
add_dependencies(SPHLib CompactNSearch glfw glad)

target_compile_definitions(SPHLib PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(SPHLib PUBLIC imgui glfw glad CompactNSearch nlohmann_json::nlohmann_json ${CMAKE_DL_LIBS})

add_executable(SPHInterface "${PROJECT_SOURCE_DIR}/src/main.cpp")
target_link_libraries(SPHInterface PRIVATE SPHLib)

# install(TARGETS SPHLib DESTINATION ${PROJECT_BINARY_DIR}/bin)
# install(TARGETS SPHInterface DESTINATION ${PROJECT_BINARY_DIR}/bin)
