cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(stoch_sims)

include_directories(/usr/local/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../bin)

find_library(BMLA_LIB bmla HINTS /usr/local/lib/)
find_library(LATTG_LIB lattgillespie HINTS /usr/local/lib)

############################################################
# main
############################################################

add_executable(main src/main.cpp)

target_link_libraries(main PUBLIC ${LATTG_LIB})

############################################################
# sample_ic
############################################################

add_executable(sample_ic src/sample_ic.cpp src/dbm_v1.hpp src/dbm_v2.hpp)

target_link_libraries(sample_ic PUBLIC ${BMLA_LIB})

############################################################
# extra
############################################################

# If want to include install target
# install(TARGETS bmla_layer_1 RUNTIME DESTINATION bin)