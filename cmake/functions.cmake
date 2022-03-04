# cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j8 ..
cmake_minimum_required (VERSION 3.16)

# create version file (work only in the linux version)
# add_custom_command does not create a new target. You have to define targets explicitly
# by add_executable, add_library or add_custom_target in order to make them visible to make
#  cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j8 -DCMAKE_SYSTEM_NAME="Generic" -DGCC_PATH="C:/Users/gadely2/ModusToolbox/tools_2.1/gcc-7.2.1/bin/" ..


#project (top NONE)







#set( UNIX TRUE)

#set (CMAKE_SYSTEM_NAME Generic)

SET(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "" FORCE)
if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE "Debug" CACHE STRING
		"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
		FORCE
		)
endif (NOT CMAKE_BUILD_TYPE)


#if (NOT CMAKE_BUILD_TYPE)
#    message(STATUS "No build type selected, default to Release")
#    set(CMAKE_BUILD_TYPE "Release")
#endif()




set (CMAKE_USE_RELATIVE_PATHS True)



####### chnage build flags here #####################
# https://stackoverflow.com/questions/43890117/enforce-32-bit-enums-with-gcc-linker/4670134  - regading to shor enums
set (CMAKE_C_FLAGS_DEBUG "-O0 -g -fno-short-enums")
set (CMAKE_CXX_FLAGS_DEBUG "-O0 -g -std=c++17")
set (CMAKE_C_FLAGS_RELEASE "-O2 -g -fno-short-enums")
set (CMAKE_CXX_FLAGS_RELEASE "-O2 -g -std=c++17")


message("CMAKE_C_FLAGS_DEBUG is ${CMAKE_C_FLAGS_DEBUG}")
message("CMAKE_C_FLAGS_RELEASE is ${CMAKE_C_FLAGS_RELEASE}")
message("CMAKE_C_FLAGS_RELWITHDEBINFO is ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message("CMAKE_C_FLAGS_MINSIZEREL is ${CMAKE_C_FLAGS_MINSIZEREL}")



macro(update_git_details)
	
unset(GIT_HASH1  CACHE )
	unset(GIT_BRANCH1  CACHE )

	
	execute_process(
		COMMAND git rev-parse  HEAD
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE GIT_HASH1
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)

	execute_process(
		COMMAND git branch --show-current
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE GIT_BRANCH1
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
       	
	set(GIT_HASH  ${GIT_HASH1}  )
	set(GIT_BRANCH ${GIT_BRANCH1}  )

endmacro()



macro(target_flags)
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		message("debug mode")
		set (CMAKE_C_FLAGS 
			" ${CPU_FLAGS}  ${CMAKE_C_FLAGS_DEBUG}" )

		set (CMAKE_CXX_FLAGS 
			" ${CPU_FLAGS}  ${CMAKE_CXX_FLAGS_DEBUG}  -fpermissive" )

	endif (CMAKE_BUILD_TYPE STREQUAL "Debug") 
	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		message("Release mode")
		set (CMAKE_C_FLAGS 
			" ${CPU_FLAGS}  ${CMAKE_C_FLAGS_RELEASE}" )

		set (CMAKE_CXX_FLAGS
			" ${CPU_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}  -fpermissive" )
	endif (CMAKE_BUILD_TYPE STREQUAL "Release") 

	enable_language(ASM)
	SET (ASM_OPTIONS "-x assembler-with-cpp")
	SET(CMAKE_ASM_FLAGS " ${CPU_FLAGS}  ${ASM_OPTIONS}" )
endmacro()






#SET(CMAKE_GENERATOR "Unix Makefiles")
function(message)
	list(GET ARGV 0 MessageType)
	if(MessageType STREQUAL FATAL_ERROR OR
			MessageType STREQUAL SEND_ERROR OR
			MessageType STREQUAL WARNING OR
			MessageType STREQUAL AUTHOR_WARNING)
		list(REMOVE_AT ARGV 0)
		_message(${MessageType} "${ARGV}")
	endif()
endfunction()




# https://stackoverflow.com/questions/9625503/make-dist-equivalent-in-cmake
# make dist like
# function(make_dist_creator _variable _access _value _current_list_file _stack)
#	if (_access STREQUAL "MODIFIED_ACCESS")
#		# Check if we are finished (end of main CMakeLists.txt)
#		if (NOT _current_list_file)
#			get_property(_subdirs GLOBAL PROPERTY MAKE_DIST_DIRECTORIES)
#			list(REMOVE_DUPLICATES _subdirs)
#			foreach(_subdir IN LISTS _subdirs)
#				list(APPEND _make_dist_sources "${_subdir}/CMakeLists.txt")
#				get_property(_targets DIRECTORY "${_subdir}" PROPERTY BUILDSYSTEM_TARGETS)
#				foreach(_target IN LISTS _targets)
#					get_property(_sources TARGET "${_target}" PROPERTY SOURCES)
#					foreach(_source IN LISTS _sources)
#						list(APPEND _make_dist_sources "${_subdir}/${_source}")
#					endforeach()
#				endforeach()
#			endforeach()
#
#			add_custom_target(
#				dist
#				COMMAND "${CMAKE_COMMAND}" -E tar zcvf "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.tar.gz" -- ${_make_dist_sources}
#				COMMENT "Make distribution ${PROJECT_NAME}.tar.gz"
#				WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
#				)
#			message("_make_dist_sources = ${_make_dist_sources}")
#		else()
#			# else collect subdirectories in my source dir
#			file(RELATIVE_PATH _dir_rel "${CMAKE_SOURCE_DIR}" "${_value}")
#			if (NOT _dir_rel MATCHES "\.\.")
#				set_property(GLOBAL APPEND PROPERTY MAKE_DIST_DIRECTORIES "${_value}")
#			endif()
#		endif()
#	endif()
#endfunction()

#variable_watch("CMAKE_CURRENT_LIST_DIR" make_dist_creator)







