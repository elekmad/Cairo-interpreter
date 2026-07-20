# Copyright (c) <2014-2017> <Newton Game Dynamics>
#
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely.

cmake_minimum_required(VERSION 3.4.0)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_definitions(-D_POSIX_VER_64)
add_definitions(-ggdb3)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


project(CairoTest LANGUAGES C)


file(GLOB CPP_SOURCE
        ./*.h
        ./*.c
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" FILES ${CPP_SOURCE})


find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(
    Parser
    parser.y
    ${CMAKE_CURRENT_SOURCE_DIR}/parser.tab.c
    COMPILE_FLAGS "-Wcounterexamples"
)

FLEX_TARGET(
    Lexer
    lexer.l
    ${CMAKE_CURRENT_SOURCE_DIR}/lex.yy.c
)

ADD_FLEX_BISON_DEPENDENCY(
    Lexer
    Parser
)


add_executable(CairoTest
	${CPP_SOURCE}
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

target_link_libraries(CairoTest PRIVATE fl)
target_link_libraries(CairoTest PRIVATE m)

find_package(Python3 REQUIRED)

add_custom_target(proto
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/proto.py
#    DEPENDS CairoTest
)
add_dependencies(CairoTest proto)

include(FetchContent)

FetchContent_Declare(
    LinkedLists
    GIT_REPOSITORY https://github.com/elekmad/LinkedList.git
)

if( UPDISTANT )
	FetchContent_MakeAvailable(LinkedLists)
endif()

target_link_libraries(CairoTest PRIVATE LinkedLists)
target_include_directories(CairoTest PRIVATE ${LinkedLists_SOURCE_DIR})

FetchContent_Declare(
	Strings
	GIT_REPOSITORY https://github.com/elekmad/String.git
)

if( UPDISTANT )
	FetchContent_MakeAvailable(Strings)
endif()

target_link_libraries(CairoTest PRIVATE Strings)
target_include_directories(CairoTest PRIVATE ${Strings_SOURCE_DIR})

find_package(PkgConfig REQUIRED)

#Cairo
find_package(Cairo)
target_link_libraries(CairoTest PRIVATE Cairo::Cairo)
message(STATUS "Cairo include: ${Cairo_INCLUDE_DIR}")
message(STATUS "Cairo lib: ${Cairo_LIBRARY}")


find_package(SDL2 REQUIRED CONFIG)
target_link_libraries(CairoTest PRIVATE SDL2::SDL2)



include_directories(./)


#to add ggdb3 : -DCMAKE_RELEASE_MODE=Debug
