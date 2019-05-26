cmake_minimum_required(VERSION 3.8)
project(tests)

# remove main.cpp to avoid entry point redefinition 
list(FILTER MAIN_SOURCE_FILES EXCLUDE REGEX "main.cpp$")

add_executable(tests ${TESTS_SOURCE_FILES} ${MAIN_SOURCE_FILES})