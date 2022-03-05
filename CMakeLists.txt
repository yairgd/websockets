
cmake_minimum_required(VERSION 3.16)


set (3rd_part  "3rd_part")

set(PRE_CONFIGURE_FILE "${CMAKE_SOURCE_DIR}/cmake/git.h.in")
set(POST_CONFIGURE_FILE "${CMAKE_BINARY_DIR}/git.h")
include (${CMAKE_SOURCE_DIR}/cmake/git_watcher.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/functions.cmake)
include(ExternalProject)



add_library(websockets INTERFACE)
set_target_properties(websockets PROPERTIES
	#INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/${3rd_part}/restbed/${CMAKE_BUILD_TYPE}/include
	INTERFACE_LINK_DIRECTORIES ${CMAKE_SOURCE_DIR}/${3rd_part}/websockets/${CMAKE_BUILD_TYPE}/lib/
	INTERFACE_LINK_LIBRARIES libwebsockets.so
	)
find_package(websockets 
	PATHS
	"${CMAKE_SOURCE_DIR}/${3rd_part}/websockets/${CMAKE_BUILD_TYPE}/usr/share")
if(NOT websockets_FOUND)
	ExternalProject_Add(websockets_external
		GIT_REPOSITORY https://github.com/warmcat/libwebsockets.git
		GIT_TAG v4.3.0
		CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DHAVE_SSL=ON -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/${3rd_part}/websockets/${CMAKE_BUILD_TYPE}/
		DEPENDS websockets
		)
	add_custom_target(websockets_install ALL DEPENDS  websockets  websockets_external)
	add_custom_command(TARGET   websockets_install
		COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/${3rd_part}/websockets/${CMAKE_BUILD_TYPE}/usr/share
		COMMAND cp ${CMAKE_SOURCE_DIR}/cmake/websockets-config.cmake ${CMAKE_SOURCE_DIR}/${3rd_part}/websockets/${CMAKE_BUILD_TYPE}/usr/share
		)
else()
	add_custom_target(websockets_install ALL    )	
	add_custom_command(TARGET   websockets_install
		COMMAND cp ${CMAKE_SOURCE_DIR}/cmake/websockets-config.cmake ${CMAKE_SOURCE_DIR}/${3rd_part}/websockets/${CMAKE_BUILD_TYPE}/usr/share

		)
endif()



project(appdemo)
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "" FORCE)








set (demo_SRC
	WebSockets.cpp
	main.cpp
	)
set (demo_H
	WebSockets.cpp
	)





add_executable(demo 
	${demo_SRC}
	${demo_H}
	)

add_dependencies(demo  
	websockets_install
	)
target_link_libraries(demo PRIVATE
	-Wl,--start-group  	
	websockets
	pthread
	-Wl,--end-group
	)

target_include_directories(demo PUBLIC
	${PROJECT_BINARY_DIR}/	
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/${3rd_part}/websockets/${CMAKE_BUILD_TYPE}/include/
	)
