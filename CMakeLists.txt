cmake_minimum_required(VERSION 3.20)

project(XFix)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold -fPIC -ggdb -Wno-deprecated -Wall -Werror -Wno-unknown-pragmas -Wno-comment")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow=global")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -fconcepts-diagnostics-depth=2")

include_directories(.
                    external/pugixml/src)

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(test)
