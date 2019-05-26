cmake_minimum_required(VERSION 3.8)
project(CPP_PROJECT)

#include helper macro
include(./macros.cmake)

# set compiler flag
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

# uncomment to activate boost
# set(THREADS_PREFER_PTHREAD_FLAG ON)
# find_package(Boost COMPONENTS system filesystem REQUIRED)
# find_package(Threads)

# set project configs
set(BUILD_FOLDER ${PROJECT_SOURCE_DIR}/build)
set(MAIN_DIR ${PROJECT_SOURCE_DIR}/main)
set(VENDOR_DIR ${PROJECT_SOURCE_DIR}/vendor)
set(TESTS_DIR ${PROJECT_SOURCE_DIR}/tests)

# collect source files
file(GLOB MAIN_SOURCE_FILES "./main/*.cpp")
file(GLOB TESTS_SOURCE_FILES "./tests/*.cpp")


# uncomment to activate boost
# include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})


add_subdirectory(main)
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/fmt)
add_subdirectory(tests)