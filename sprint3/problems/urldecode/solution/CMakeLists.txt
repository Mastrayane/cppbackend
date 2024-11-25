cmake_minimum_required(VERSION 3.13)

project(tests CXX)
set(CMAKE_CXX_STANDARD 20)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(SRCDIR src)
set(TESTDIR tests)

set(BOOST_EXEC boost_tests)
set(GOOGLE_EXEC google_tests)

set(SRC 
	${SRCDIR}/url_decode.cpp
  ${SRCDIR}/url_decode.h
)

set(BOOST_TEST_SRC 
	${TESTDIR}/boost_test_main.cpp
)

set(GOOGLE_TEST_SRC 
	${TESTDIR}/google_test_main.cpp
)


find_package(Boost 1.78.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()



add_executable(${BOOST_EXEC} ${SRC} ${BOOST_TEST_SRC})
target_include_directories(${BOOST_EXEC} PRIVATE CONAN_PKG::boost)
target_link_libraries(${BOOST_EXEC} PRIVATE CONAN_PKG::boost)


add_executable(${GOOGLE_EXEC} ${SRC} ${GOOGLE_TEST_SRC})
target_include_directories(${GOOGLE_EXEC} PRIVATE CONAN_PKG::gtest)
target_link_libraries(${GOOGLE_EXEC} PRIVATE CONAN_PKG::gtest)