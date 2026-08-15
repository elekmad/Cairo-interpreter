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


project(CairoInterpreter LANGUAGES C)


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
    #COMPILE_FLAGS "-Wcounterexamples"
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


add_executable(CairoInterpreter
	${CPP_SOURCE}
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

#target_link_libraries(CairoInterpreter PRIVATE fl)
target_link_libraries(CairoInterpreter PRIVATE m)
find_library(B64_LIBRARY
    NAMES libb64.a
)

target_link_libraries(CairoInterpreter PRIVATE ${B64_LIBRARY})

include(FetchContent)

FetchContent_Declare(
    LinkedLists
    GIT_REPOSITORY https://github.com/elekmad/LinkedList.git
)

if( UPDISTANT )
	FetchContent_MakeAvailable(LinkedLists)
endif()

target_link_libraries(CairoInterpreter PRIVATE LinkedLists)
target_include_directories(CairoInterpreter PRIVATE ${linkedlists_SOURCE_DIR})

FetchContent_Declare(
	Strings
	GIT_REPOSITORY https://github.com/elekmad/String.git
)

if( UPDISTANT )
	FetchContent_MakeAvailable(Strings)
endif()

target_link_libraries(CairoInterpreter PRIVATE Strings)
target_include_directories(CairoInterpreter PRIVATE ${strings_SOURCE_DIR})

find_package(Python3 REQUIRED)

foreach(source ${CPP_SOURCE})
	if(${source} MATCHES "\\.c$")
		message(STATUS ${source}" must gen proto")
		string(REGEX REPLACE "\\.c$" ".h" header "${source}")
		if(NOT EXISTS "${header}")
		message(STATUS ${header}" do not exist")
	        continue()
    	endif()
    	file(READ "${header}" HEADER_CONTENT)

    	if(HEADER_CONTENT MATCHES "#[ \t]*include[ \t]*<[^<]*\\.proto\\.h>")
    		string(REGEX REPLACE "\\.c$" ".proto.h" pheader "${source}")
    		message(STATUS ${pheader}" must be gen produced")
         	add_custom_command(
			    	OUTPUT
    				${pheader}
					COMMAND ${Python3_EXECUTABLE}
            			${CMAKE_SOURCE_DIR}/proto.py ${CPROTO_INCLUDES} --source ${source} --compile ${CMAKE_SOURCE_DIR}
    				DEPENDS ${source}
					)
			set_source_files_properties(
    				${source}
    				PROPERTIES
        			OBJECT_DEPENDS ${pheader}
			)
		else()
			message(STATUS ${header}" do not contain proto")
    	endif()
	endif()
endforeach()


message(STATUS "top cproto includes : ${CPROTO_INCLUDES}")

find_package(PkgConfig REQUIRED)

#Cairo
find_package(Cairo)
target_link_libraries(CairoInterpreter PRIVATE Cairo::Cairo)
message(STATUS "Cairo include: ${Cairo_INCLUDE_DIR}")
message(STATUS "Cairo lib: ${Cairo_LIBRARY}")

set(WITHSDL 1)
if( NOSDL )
	set(WITHSDL 0)
	add_definitions(-DNOSDL)
endif()

if( CGIMODE )
	add_definitions(-DCGIMODE)
endif()

if( WITHSDL)
find_package(SDL2 REQUIRED CONFIG)
target_link_libraries(CairoInterpreter PRIVATE SDL2::SDL2)
endif()


include_directories(./)


#to add ggdb3 : -DCMAKE_RELEASE_MODE=Debug
