
# !/bin/sh

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_DIR="$SCRIPT_DIR/.."

# --------------------------------------------------------------------------- #

PROJECT_NAME="primec"

INCLUDES="
	-I$PROJECT_DIR/include
"

SOURCES="
	$PROJECT_DIR/source/primec/debug.c
	$PROJECT_DIR/source/primec/logger.c
	$PROJECT_DIR/source/primec/utils.c
	$PROJECT_DIR/source/primec/utf8.c
	$PROJECT_DIR/source/primec/token.c
	$PROJECT_DIR/source/primec/lexer.c
	$PROJECT_DIR/source/main.c
"

LIBRARIES="
"

# --------------------------------------------------------------------------- #

# Set the build configuration (default to debug if not provided)
BUILD_CONFIGURATION=${1:-"debug"}

# Check if the "build" directory exists, and clean it if it does
if [ -d "$PROJECT_DIR/build" ]; then
	rm -fr "$PROJECT_DIR/build"
fi

mkdir "$PROJECT_DIR/build"

# Choose compiler flags and defines based on build configuration
case $BUILD_CONFIGURATION in
	"debug")
		FLAGS="-g -O0"
		DEFINES="-Dprimec_lexer_only"
		PROJECT_NAME_SUFFIX="_debug"
		;;
	"hybrid")
		FLAGS="-g -O1"
		PROJECT_NAME_SUFFIX="_hybrid"
		;;
	"release")
		FLAGS="-O3"
		DEFINES="$DEFINES -DNDEBUG"
		PROJECT_NAME_SUFFIX="_release"
		;;
	*)
		echo "[error]: invalid build configuration. Please use 'debug', 'hybrid', or 'release'."
		exit 1
		;;
esac

# Compilation command
gcc -Wall \
	-Wextra \
	-Wpedantic \
	-Werror \
	-Wshadow \
	-Wimplicit \
	-Wreturn-type \
	-Wunknown-pragmas \
	-Wunused-variable \
	-Wunused-function \
	-Wmissing-prototypes \
	-Wstrict-prototypes \
	-Wconversion \
	-Wsign-conversion \
	-Wunreachable-code \
	$INCLUDES \
	$FLAGS \
	$DEFINES \
	$SOURCES \
	-o "$PROJECT_DIR/build/$PROJECT_NAME$PROJECT_NAME_SUFFIX" \
	$LIBRARIES

# Check if compilation was successful
if [ $? -eq 0 ]; then
	echo "[info]: compilation successful - executable: ./build/$PROJECT_NAME$PROJECT_NAME_SUFFIX"
	cp -f "$PROJECT_DIR/build/$PROJECT_NAME$PROJECT_NAME_SUFFIX" "$PROJECT_DIR/build/$PROJECT_NAME"
else
	echo "[error]: compilation failed."
fi
