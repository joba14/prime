
# Minimum cmake version required
cmake_minimum_required(VERSION 3.5)

# Project name and programming language definition
project(prime_driver LANGUAGES C)

# C standard for the project
set(CMAKE_C_STANDARD 11)

# Source files of the project
set(SOURCES
	./source/main.c
)

# Project executable definition
add_executable(${PROJECT_NAME} ${SOURCES})

# Compile options of the project
target_compile_options(${PROJECT_NAME}
	PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
		-Wshadow
		-Wimplicit
		-Wreturn-type
		-Wunknown-pragmas
		-Wunused-variable
		-Wunused-function
		-Wmissing-prototypes
		-Wstrict-prototypes
		-Wconversion
		-Wsign-conversion
		-Wunreachable-code
)

# Include directories of the project
target_include_directories(${PROJECT_NAME}
	PUBLIC
		./include
	PRIVATE
		prime_common
)

# Link libraries for the target
target_link_libraries(${PROJECT_NAME}
	PUBLIC
	PRIVATE
		prime_common
)

# Compile definitions of the project
target_compile_definitions(${PROJECT_NAME}
	PUBLIC
	PRIVATE
)
