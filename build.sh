#!/bin/bash
# Build meo compiler and tests
# Automatically re-runs cmake when CMakeLists.txt changes

BUILD_DIR="build"
CMAKE_FILES=$(find . -name CMakeLists.txt -not -path "./$BUILD_DIR/*" | sort)
CMAKE_HASH_FILE="$BUILD_DIR/.cmake_hash"

# Compute current hash of all CMakeLists.txt files
current_hash=$(cat $CMAKE_FILES 2>/dev/null | md5sum | cut -d' ' -f1)

# Check if cmake needs to re-configure
need_configure=false
if [ ! -f "$CMAKE_HASH_FILE" ]; then
    need_configure=true
elif [ "$(cat $CMAKE_HASH_FILE)" != "$current_hash" ]; then
    need_configure=true
fi

if [ "$need_configure" = true ]; then
    echo "CMakeLists.txt changed, re-configuring..."
    cmake -S . -B "$BUILD_DIR" && echo "$current_hash" > "$CMAKE_HASH_FILE"
fi

cmake --build "$BUILD_DIR" -j$(nproc)
