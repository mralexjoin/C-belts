cmake_minimum_required(VERSION 3.12)

project(CBelts VERSION 1.0 LANGUAGES CXX)

aux_source_directory(src SRC_FILES)
add_executable(main ${SRC_FILES})
target_include_directories(main PUBLIC include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MAIN_SOURCE_DIR}/build)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
  set(CMAKE_CXX_FLAGS "/Wall /WX /std:c++17")
  set(CMAKE_CXX_FLAGS_DEBUG "/Zi")
else()
  set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -std=c++17 -pthread")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=address -fsanitize=undefined")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
