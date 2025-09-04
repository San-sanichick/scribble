#!/bin/bash

# Script to build a CMake project in Debug or Release mode with separate directories

# Default build type
BUILD_TYPE="Debug"
BASE_BUILD_DIR="build"
PROJECT_NAME="scribble"

# Print usage information
usage() {
    echo "Usage: $0 [debug|release] [-c|--clean] [-h|--help]"
    echo "  debug   : Build in Debug mode (default)"
    echo "  release : Build in Release mode"
    echo "  -c, --clean : Clean the build directory before building"
    echo "  -h, --help  : Show this help message"
    exit 1
}

# Parse command-line arguments
CLEAN_BUILD=false
while [[ $# -gt 0 ]]; do
    case "$1" in
        debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        release)
            BUILD_TYPE="Release"
            shift
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -h|--help)
            usage
            ;;
        *)
            echo "Unknown argument: $1"
            usage
            ;;
    esac
done

# Set build directory based on build type
BUILD_DIR="${BASE_BUILD_DIR}/${BUILD_TYPE}"

# Create or clean build directory
if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning ${BUILD_TYPE} build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Configuring CMake for $BUILD_TYPE build in $BUILD_DIR..."
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ../..


echo "Building $BUILD_TYPE configuration..."
cmake --build . --config "$BUILD_TYPE"

cd ../..
cp "$(pwd)/$BUILD_DIR/compile_commands.json" "$(pwd)/build"
