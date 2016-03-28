cmake_minimum_required(VERSION 3.2.2)
project (morfessor)
#project (morfessor-tests)

find_package(Threads)
include_directories("include")
link_directories(/usr/local/lib)

file(GLOB TESTS "tests/*.cpp")
#set(SOURCES )
set(MAINSOURCE "src/MorfessorProgram.cpp")

add_executable(morfessor ${SOURCES} ${MAINSOURCE})
#add_executable(morfessor-tests ${SOURCES} ${TESTS})
#target_link_libraries(morfessor-tests /usr/local/lib/gtest_main.a ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET morfessor PROPERTY CXX_STANDARD 14)
#set_property(TARGET morfessor-tests PROPERTY CXX_STANDARD 14)
