cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(bimol_test)

include_directories(/usr/local/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../bin)

find_library(DBLZ_LIB dblz HINTS /usr/local/lib/)
find_library(Q3C1_LIB q3c1 HINTS /usr/local/lib/)

############################################################
# sample_hidden
############################################################

add_executable(sample_hidden src/sample_hidden.cpp)

target_link_libraries(sample_hidden PUBLIC ${DBLZ_LIB})
target_link_libraries(sample_hidden PUBLIC ${Q3C1_LIB})

############################################################
# sample_visible
############################################################

add_executable(sample_visible src/sample_visible.cpp)

target_link_libraries(sample_visible PUBLIC ${DBLZ_LIB})
target_link_libraries(sample_visible PUBLIC ${Q3C1_LIB})

############################################################
# learned_trajs_hidden
############################################################

add_executable(learned_trajs_hidden src/learned_trajs_hidden.cpp)

target_link_libraries(learned_trajs_hidden PUBLIC ${DBLZ_LIB})
target_link_libraries(learned_trajs_hidden PUBLIC ${Q3C1_LIB})

############################################################
# learned_trajs_visible
############################################################

add_executable(learned_trajs_visible src/learned_trajs_visible)

target_link_libraries(learned_trajs_visible PUBLIC ${DBLZ_LIB})
target_link_libraries(learned_trajs_visible PUBLIC ${Q3C1_LIB})

############################################################
# extra
############################################################

# If want to include install target
# install(TARGETS bmla_layer_1 RUNTIME DESTINATION bin)