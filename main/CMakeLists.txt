cmake_minimum_required(VERSION 3.8)
project(app)


add_executable(app ${MAIN_SOURCE_FILES})
# uncomment to activate boost
#target_link_libraries(app ${Boost_LIBRARIES} Threads::Threads)
