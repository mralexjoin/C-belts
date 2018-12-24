cmake_minimum_required(VERSION 3.13)

project(CBelts VERSION 1.0 LANGUAGES CXX)

aux_source_directory(src SRC_FILES)
add_executable(main ${SRC_FILES})
target_include_directories(main PUBLIC include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MAIN_SOURCE_DIR}/build)
