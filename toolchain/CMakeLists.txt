
# Get the list of subdirectories in the current directory
file(GLOB subdirectories RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

# Iterate over each subdirectory
foreach(subdirectory ${subdirectories})
	if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory})
		message(STATUS "adding subdirectory: ${subdirectory}")
		add_subdirectory(${subdirectory})
	endif()
endforeach()
