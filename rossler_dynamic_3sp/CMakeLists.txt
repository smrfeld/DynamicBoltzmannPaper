cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(dynamic_3species)

include_directories(/usr/local/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../bin)

find_library(DBLZ_LIB dblz HINTS /usr/local/lib/)
find_library(Q3C1_LIB q3c1 HINTS /usr/local/lib/)

############################################################
# sample
############################################################

add_executable(sample src/sample.cpp src/dbm.hpp)

target_link_libraries(sample PUBLIC ${DBLZ_LIB})
target_link_libraries(sample PUBLIC ${Q3C1_LIB})

############################################################
# learn_parallel
############################################################

add_executable(learn_parallel src/learn_parallel.cpp src/dbm.hpp)

target_link_libraries(learn_parallel PUBLIC ${DBLZ_LIB})
target_link_libraries(learn_parallel PUBLIC ${Q3C1_LIB})

############################################################
# learn_sequential
############################################################

add_executable(learn_sequential src/learn_sequential.cpp src/dbm.hpp)

target_link_libraries(learn_sequential PUBLIC ${DBLZ_LIB})
target_link_libraries(learn_sequential PUBLIC ${Q3C1_LIB})

############################################################
# learn_sequential_f
############################################################

add_executable(learn_sequential_f src/learn_sequential_f.cpp src/dbm_f.hpp)

target_link_libraries(learn_sequential_f PUBLIC ${DBLZ_LIB})
target_link_libraries(learn_sequential_f PUBLIC ${Q3C1_LIB})

############################################################
# learn_sequential_i
############################################################

add_executable(learn_sequential_i src/learn_sequential_i.cpp src/dbm_i.hpp)

target_link_libraries(learn_sequential_i PUBLIC ${DBLZ_LIB})
target_link_libraries(learn_sequential_i PUBLIC ${Q3C1_LIB})

############################################################
# extra
############################################################

# If want to include install target
# install(TARGETS bmla_layer_1 RUNTIME DESTINATION bin)
