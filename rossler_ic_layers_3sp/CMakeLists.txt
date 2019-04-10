cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(ic_layers_3species)

include_directories(/usr/local/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../bin)

find_library(BMLA_LIB bmla HINTS /usr/local/lib/)

############################################################
# sample_layers
############################################################

add_executable(sample_layers src/sample_layers.cpp src/dbm_3species.hpp)

target_link_libraries(sample_layers PUBLIC ${BMLA_LIB})

############################################################
# solve_pcd
############################################################

add_executable(solve_pcd src/solve_pcd.cpp src/dbm_3species.hpp)

target_link_libraries(solve_pcd PUBLIC ${BMLA_LIB})

############################################################
# solve_cd
############################################################

add_executable(solve_cd src/solve_cd.cpp src/dbm_3species.hpp)

target_link_libraries(solve_cd PUBLIC ${BMLA_LIB})

############################################################
# extra
############################################################

# If want to include install target
# install(TARGETS bmla_layer_1 RUNTIME DESTINATION bin)