cmake_minimum_required(VERSION 3.11)

project(postgres CXX)
set(CMAKE_CXX_STANDARD 20)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS NO_OUTPUT_DIRS)

find_package(Boost 1.78.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build )

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(book_manager main.cpp)
target_link_libraries(book_manager CONAN_PKG::libpqxx CONAN_PKG::boost)