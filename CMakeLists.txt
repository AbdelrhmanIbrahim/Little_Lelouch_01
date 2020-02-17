cmake_minimum_required(VERSION 3.17)

#project name
project(Little_Lelouch)

#c++17
set(CMAKE_CXX_STANDARD 17)

#headers
set(HEADERS
	Move.h
	Knowledge.h
	Helpers.h
	Board.h
	BestMove.h
	)

#src
set(SOURCES
	Move.cpp
	Knowledge.cpp
	Helpers.cpp
	Board.cpp
	main.cpp
	)

#make it reflect the same structure as the one on disk
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

#include the source directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME} 
				${HEADERS}
				${SOURCES})