
# Minimum cmake version required
cmake_minimum_required(VERSION 3.5)

# Project name and programming language definition
project(prime LANGUAGES C)

# Libraries subdirectory
add_subdirectory(./libraries)

# Toolchain subdirectory
add_subdirectory(./toolchain)
