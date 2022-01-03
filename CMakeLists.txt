cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(cpp_actor VERSION 0.1.0)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(BUILD_SHARED_LIBS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MACOSX_RPATH 1)

include(CPack)
include(CTest)

set(SOURCES 
    src/scheduler.cpp)

set(HEADERS
    src/abstract_actor.hpp
    src/actor_system.hpp
    src/scheduler.hpp)

add_library(cpp_actor ${SOURCES} ${HEADERS})

# PUBLIC to make imports available elsewhere in project
# target_include_directories(${PROJECT_NAME} PUBLIC
#     ${PROJECT_BINARY_DIR}
#     ${PROJECT_SOURCE_DIR}/src
#     ${PROJECT_SOURCE_DIR}/src/util)

# # INTERFACE for library consumers
# target_include_directories(${PROJECT_NAME}
#     INTERFACE ${PROJECT_SOURCE_DIR}/src)

#include_directories(${PROJECT_NAME})

#add_subdirectory(src/util)
add_subdirectory(test)
# enable_testing()

target_compile_features(cpp_actor PUBLIC cxx_std_20)
