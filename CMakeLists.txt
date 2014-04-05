project(ucapa)
cmake_minimum_required(VERSION 2.8.9)

# Option to switch between shared and static build
if (NOT DEFINED BUILD_SHARED_LIBS)
	set(BUILD_SHARED_LIBS TRUE)
endif()
set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS} CACHE BOOL "TRUE to build UCAPA as a shared library, FALSE to build it as a static library" FORCE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

# Find dependencies
find_package(Asio REQUIRED)
find_package(FFMPEG REQUIRED)

# Set up dependencies
add_definitions("-DASIO_STANDALONE")
include_directories(${ASIO_INCLUDE_DIR})
include_directories(${FFMPEG_INCLUDE_DIR})

# Customize build options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__STDC_CONSTANT_MACROS")
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

# Prepare the target configugarion
file(GLOB_RECURSE ucapa_src src/*.cpp)
file(GLOB_RECURSE ucapa_headers include/*.h)

include_directories(./include)

set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib")

# Set up the target configuration, depending of the build type
if (BUILD_SHARED_LIBS)
	add_definitions("-DSHARED_BUILD")
	add_library(${PROJECT_NAME} SHARED ${ucapa_headers} ${ucapa_src})
	if(WIN32)
		target_link_libraries(${PROJECT_NAME} ws2_32 wsock32)
		target_link_libraries(${PROJECT_NAME} ${FFMPEG_LIB_DIR})
	elseif(UNIX)
		target_link_libraries(${PROJECT_NAME} pthread ${FFMPEG_LIB_DIR} va z bz2)
	endif(WIN32)
else ()
	add_library(${PROJECT_NAME} STATIC ${ucapa_headers} ${ucapa_src})
endif()

# Define various target properties
set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -d )

set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib" )
set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/lib" )
set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${PROJECT_SOURCE_DIR}/lib" )
set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/lib" )
set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_SOURCE_DIR}/lib" )


# Define targets for examples
add_subdirectory(examples/derivate)
add_subdirectory(examples/navigator)

