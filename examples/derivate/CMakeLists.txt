project(derivate)
cmake_minimum_required(VERSION 2.8.9)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
#set(RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE derivate_src *.cpp)
file(GLOB_RECURSE derivate_headers *.h)

add_executable(${PROJECT_NAME} ${derivate_headers} ${derivate_src})

set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -d )

set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR} )
set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR} )
set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_SOURCE_DIR} )
set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR} )
set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR} )

target_link_libraries(${PROJECT_NAME} ucapa)
if(WIN32)
	target_link_libraries(${PROJECT_NAME} ws2_32 wsock32)
	target_link_libraries(${PROJECT_NAME} ${FFMPEG_LIB_DIR})
elseif(UNIX)
	target_link_libraries(${PROJECT_NAME} pthread ${FFMPEG_LIB_DIR} va z bz2)
endif(WIN32)

