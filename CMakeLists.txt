cmake_minimum_required(VERSION 3.2.2)
project (morfessor)
project (morfessor-tests)
set(CMAKE_BUILD_TYPE Debug)

# My code
include_directories("include")
file(GLOB TESTS "tests/*.cc")
set(SOURCES "src/corpus.cc" "src/lexicon.cc" "src/morph.cc" "src/morph_node.cc" "src/viterbi.cc")
set(MAINSOURCE "src/morfessor_main.cc")
add_executable(morfessor ${SOURCES} ${MAINSOURCE})
add_executable(morfessor-tests ${SOURCES} ${TESTS})
set_property(TARGET morfessor PROPERTY CXX_STANDARD 14)
set_property(TARGET morfessor-tests PROPERTY CXX_STANDARD 14)

# gflags
find_package(gflags REQUIRED)

# Boost Math headers
find_package(Boost 1.58)
include_directories(${Boost_INCLUDE_DIRS})

# GoogleTest
link_directories(/usr/local/lib)
target_link_libraries(morfessor-tests /usr/local/lib/gtest_main.a)

# Threads for GoogleTest
find_package(Threads)
target_link_libraries(morfessor-tests ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(morfessor gflags)

