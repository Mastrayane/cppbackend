cmake_minimum_required(VERSION 3.18)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

get_property(importTargets DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)
message(STATUS "${importTargets}") 

#find_package(Boost 1.78.0 REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(game_server
    src/main.cpp
    src/http_server.cpp
    src/http_server.h
    src/sdk.h
    src/model.h
    src/model.cpp
    src/tagged.h
    src/boost_json.cpp
    src/json_loader.h
    src/json_loader.cpp
    src/request_handler.cpp
    src/request_handler.h
    src/serializer.h
    src/serializer.cpp
    src/stream_adapter.h
    src/utils.h
    src/utils.cpp
    src/logger.cpp
    src/logger.h
    src/logging_request_handler.h
)

target_link_libraries(game_server PRIVATE Threads::Threads CONAN_PKG::boost)