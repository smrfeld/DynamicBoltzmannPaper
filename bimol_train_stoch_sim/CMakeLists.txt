cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(bimol_train_stoch_sim)

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
# learn_ic
############################################################

add_executable(learn_ic src/learn_ic.cpp)

target_link_libraries(learn_ic PUBLIC ${BMLA_LIB})

############################################################
# extra
############################################################

# If want to include install target
# install(TARGETS bmla_layer_1 RUNTIME DESTINATION bin)