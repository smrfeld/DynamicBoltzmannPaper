cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(bimol_test)

include_directories(/usr/local/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../bin)

# Dependencies
include(cmake/CPM.cmake)
CPMFindPackage(
    GITHUB_REPOSITORY "smrfeld/Q3-C1-Finite-Elements"
    NAME q3c1
    GIT_TAG 2.0
    )
CPMFindPackage(
    GITHUB_REPOSITORY "smrfeld/DynamicBoltzmann"
    NAME dblz
    GIT_TAG 3.1
    )

############################################################
# sample_hidden
############################################################

add_executable(sample_hidden src/sample_hidden.cpp)

# Include dirs for dblz, q3c1
target_include_directories(sample_hidden PRIVATE ${dblz_SOURCE_DIR}/include/)
target_include_directories(sample_hidden PRIVATE ${q3c1_SOURCE_DIR}/include/)

target_link_libraries(sample_hidden PUBLIC dblz)
target_link_libraries(sample_hidden PUBLIC q3c1)

############################################################
# sample_visible
############################################################

add_executable(sample_visible src/sample_visible.cpp)

# Include dirs for dblz, q3c1
target_include_directories(sample_visible PRIVATE ${dblz_SOURCE_DIR}/include/)
target_include_directories(sample_visible PRIVATE ${q3c1_SOURCE_DIR}/include/)

target_link_libraries(sample_visible PUBLIC dblz)
target_link_libraries(sample_visible PUBLIC q3c1)

############################################################
# learned_trajs_hidden
############################################################

add_executable(learned_trajs_hidden src/learned_trajs_hidden.cpp)

# Include dirs for dblz, q3c1
target_include_directories(learned_trajs_hidden PRIVATE ${dblz_SOURCE_DIR}/include/)
target_include_directories(learned_trajs_hidden PRIVATE ${q3c1_SOURCE_DIR}/include/)

target_link_libraries(learned_trajs_hidden PUBLIC dblz)
target_link_libraries(learned_trajs_hidden PUBLIC q3c1)

############################################################
# learned_trajs_visible
############################################################

add_executable(learned_trajs_visible src/learned_trajs_visible)

# Include dirs for dblz, q3c1
target_include_directories(learned_trajs_visible PRIVATE ${dblz_SOURCE_DIR}/include/)
target_include_directories(learned_trajs_visible PRIVATE ${q3c1_SOURCE_DIR}/include/)

target_link_libraries(learned_trajs_visible PUBLIC dblz)
target_link_libraries(learned_trajs_visible PUBLIC q3c1)

############################################################
# extra
############################################################

# If want to include install target
# install(TARGETS bmla_layer_1 RUNTIME DESTINATION bin)