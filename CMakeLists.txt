project(ucapa)
cmake_minimum_required(VERSION 2.8.9)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

find_package(Asio REQUIRED)
find_package(FFMPEG REQUIRED)

add_definitions("-DASIO_STANDALONE")
include_directories(${ASIO_INCLUDE_DIR})
include_directories(${FFMPEG_INCLUDE_DIR})

if (CMAKE_COMPILER_IS_GNUCXX OR MINGW OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	add_definitions("-std=c++11")
	add_definitions("-Wall")
	add_definitions("-Wextra")
	add_definitions("-Wno-long-long")
	add_definitions("-pedantic")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

file(GLOB_RECURSE ucapa_src src/*.cpp)
file(GLOB_RECURSE ucapa_headers include/*.h)

include_directories(./include)

set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib")

add_library(${PROJECT_NAME} STATIC ${ucapa_headers} ${ucapa_src})
set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -d )

set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib" )
set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/lib" )
set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${PROJECT_SOURCE_DIR}/lib" )
set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/lib" )
set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_SOURCE_DIR}/lib" )

# Build examples
add_subdirectory(examples/derivate)
add_subdirectory(examples/navigator)

