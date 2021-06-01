cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(dynamic_3species)

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
# sample
############################################################

add_executable(sample src/sample.cpp src/dbm.hpp)

# Include dirs for dblz, q3c1
target_include_directories(sample PRIVATE ${dblz_SOURCE_DIR}/include/)
target_include_directories(sample PRIVATE ${q3c1_SOURCE_DIR}/include/)

target_link_libraries(sample PUBLIC dblz)
target_link_libraries(sample PUBLIC q3c1)

############################################################
# learn_parallel
############################################################

add_executable(learn_parallel src/learn_parallel.cpp src/dbm.hpp)

# Include dirs for dblz, q3c1
target_include_directories(learn_parallel PRIVATE ${dblz_SOURCE_DIR}/include/)
target_include_directories(learn_parallel PRIVATE ${q3c1_SOURCE_DIR}/include/)

target_link_libraries(learn_parallel PUBLIC dblz)
target_link_libraries(learn_parallel PUBLIC q3c1)

############################################################
# learn_sequential
############################################################

add_executable(learn_sequential src/learn_sequential.cpp src/dbm.hpp)

# Include dirs for dblz, q3c1
target_include_directories(learn_sequential PRIVATE ${dblz_SOURCE_DIR}/include/)
target_include_directories(learn_sequential PRIVATE ${q3c1_SOURCE_DIR}/include/)

target_link_libraries(learn_sequential PUBLIC dblz)
target_link_libraries(learn_sequential PUBLIC q3c1)

############################################################
# learn_sequential_f
############################################################

add_executable(learn_sequential_f src/learn_sequential_f.cpp src/dbm_f.hpp)

# Include dirs for dblz, q3c1
target_include_directories(learn_sequential_f PRIVATE ${dblz_SOURCE_DIR}/include/)
target_include_directories(learn_sequential_f PRIVATE ${q3c1_SOURCE_DIR}/include/)

target_link_libraries(learn_sequential_f PUBLIC dblz)
target_link_libraries(learn_sequential_f PUBLIC q3c1)

############################################################
# learn_sequential_i
############################################################

add_executable(learn_sequential_i src/learn_sequential_i.cpp src/dbm_i.hpp)

# Include dirs for dblz, q3c1
target_include_directories(learn_sequential_i PRIVATE ${dblz_SOURCE_DIR}/include/)
target_include_directories(learn_sequential_i PRIVATE ${q3c1_SOURCE_DIR}/include/)

target_link_libraries(learn_sequential_i PUBLIC dblz)
target_link_libraries(learn_sequential_i PUBLIC q3c1)

############################################################
# extra
############################################################

# If want to include install target
# install(TARGETS bmla_layer_1 RUNTIME DESTINATION bin)
