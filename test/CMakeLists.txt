cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
# GoogleTest requires at least C++11

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

get_filename_component(PARENT_DIR ../ ABSOLUTE)
include_directories(${PARENT_DIR}/src)
include_directories(${PARENT_DIR}/src/util)

add_executable(
    test_threadsafe_queue
    test_threadsafe_queue.cpp
    test_thread_pool
    test_thread_pool.cpp
    test_actor
    test_actor.cpp
    test_actor_system
    test_actor_system.cpp
)
target_link_libraries(
    test_threadsafe_queue
    gtest_main # link to gtest 
    # cpp_actor # link to the cpp_actor project to allow the tests to import it as a lib
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(test_threadsafe_queue)
