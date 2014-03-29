project(navigator)
cmake_minimum_required(VERSION 2.8.9)

# The QtWidgets module find its own dependencies (QtCore and QtGui)
find_package(Qt5Widgets)

# Create the target project only if Qt5 was found
if (DEFINED Qt5Widgets_FOUND AND Qt5Widgets_FOUND)
	set(CMAKE_AUTOMOC ON) # Specific to Qt
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
	
	file(GLOB_RECURSE navigator_src *.cpp)
	file(GLOB_RECURSE navigator_headers *.h)
	file(GLOB_RECURSE navigator_uis *.ui)
	
	qt5_wrap_ui(navigator_ui_headers ${navigator_uis}) # Specific to Qt
	
	add_executable(${PROJECT_NAME} ${navigator_headers} ${navigator_ui_headers} ${navigator_src})
	set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -d )

	set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR} )
	set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR} )
	set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_SOURCE_DIR} )
	set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR} )
	set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR} )
	
	qt5_use_modules(${PROJECT_NAME} Widgets) # Specific to Qt
	#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Qt5Core_QTMAIN_LIBRARIES})
	target_link_libraries(${PROJECT_NAME} ucapa)
	if(WIN32)
		target_link_libraries(${PROJECT_NAME} ws2_32 wsock32)
		target_link_libraries(${PROJECT_NAME} ${FFMPEG_LIB_DIR})
	elseif(UNIX)
		target_link_libraries(${PROJECT_NAME} pthread ${FFMPEG_LIB_DIR} va z)
	endif(WIN32)
else()
	set(Qt5Widgets_DIR "" CACHE STRING "Qt5Widgets lib path which is here: <QTDIR>/lib/cmake/Qt5Widgets")
	message(WARNING "Could not find Qt5. Navigator project wont be generated.")
endif ()
