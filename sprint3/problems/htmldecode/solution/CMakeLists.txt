cmake_minimum_required(VERSION 3.13)

project(tests CXX)
set(CMAKE_CXX_STANDARD 20)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(SRCDIR src)
set(TESTDIR tests)

set(CATCH2_EXEC catch2_tests)


set(SRC 
	${SRCDIR}/html_decode.cpp
  ${SRCDIR}/html_decode.h
)

set(CATCH2_TEST_SRC 
	${TESTDIR}/catch_test_main.cpp
)



# find_package(Boost 1.78.0 REQUIRED)
# if(Boost_FOUND)
#   include_directories(${Boost_INCLUDE_DIRS})
# endif()



add_executable(${CATCH2_EXEC} ${SRC} ${CATCH2_TEST_SRC})
target_include_directories(${CATCH2_EXEC} PRIVATE CONAN_PKG::catch2)
target_link_libraries(${CATCH2_EXEC} PRIVATE  CONAN_PKG::catch2)
