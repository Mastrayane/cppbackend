cmake_minimum_required(VERSION 3.13)

project(game_server_tests CXX)
set(CMAKE_CXX_STANDARD 20)

# обратите внимание на аргумент TARGETS у команды conan_basic_setup
#include(${CMAKE_SOURCE_DIR}/conanbuildinfo.cmake)
#conan_basic_setup(TARGETS)


set(TEST_EXEC game_server_tests)

set(TEST_DIR ${CMAKE_SOURCE_DIR}/tests)

set(DEP_DIR ${CMAKE_SOURCE_DIR}/src)

set(SRC_TEST 
  ${TEST_DIR}/loot_generator_tests.cpp
  ${DEP_DIR}/model/loot_generator.cpp
  ${DEP_DIR}/model/loot_generator.h
)

add_executable(${TEST_EXEC} ${SRC_TEST})
target_include_directories(${TEST_EXEC} PRIVATE CONAN_PKG::catch2)
target_link_libraries(${TEST_EXEC} PRIVATE Threads::Threads CONAN_PKG::catch2)