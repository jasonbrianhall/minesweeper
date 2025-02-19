#!/bin/bash

# Directory setup
BUILD_DIR="build"
mkdir -p "$BUILD_DIR"

# Compile the application
echo "Compiling minesweeper..."
x86_64-w64-mingw32-gcc -o "$BUILD_DIR/minesweeper_gtk.exe" gtk_minesweeper.cpp highscores.cpp `mingw64-pkg-config --cflags gtk+-3.0 --libs gtk+-3.0` -lstdc++

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

echo "Compilation successful!"

# Function to get direct dependencies of a file
get_dependencies() {
    local file="$1"
    objdump -p "$file" | grep "DLL Name:" | sed 's/\s*DLL Name: //'
}

# Function to check if a DLL exists in source directory
dll_exists() {
    local dll="$1"
    [ -f "$DLL_SOURCE_DIR/$dll" ]
}

# Set MinGW DLL directory
DLL_SOURCE_DIR="/usr/x86_64-w64-mingw32/sys-root/mingw/bin"
if [ ! -d "$DLL_SOURCE_DIR" ]; then
    echo "Could not find MinGW DLL directory at: $DLL_SOURCE_DIR"
    echo "Please ensure MinGW is properly installed."
    exit 1
fi

echo "Using DLL source directory: $DLL_SOURCE_DIR"

# Initialize arrays for processing
declare -A processed_dlls
declare -a dlls_to_process
declare -a missing_dlls

# Get initial dependencies
echo "Analyzing dependencies for minesweeper_gtk.exe..."
initial_dlls=$(get_dependencies "$BUILD_DIR/minesweeper_gtk.exe")

# Add initial DLLs to processing queue
for dll in $initial_dlls; do
    if dll_exists "$dll"; then
        dlls_to_process+=("$dll")
    else
        missing_dlls+=("$dll")
    fi
done

# Process DLLs recursively
while [ ${#dlls_to_process[@]} -gt 0 ]; do
    current_dll="${dlls_to_process[0]}"
    dlls_to_process=("${dlls_to_process[@]:1}") # Remove first element
    
    # Skip if already processed
    [ "${processed_dlls[$current_dll]}" = "1" ] && continue
    
    echo "Processing: $current_dll"
    
    # Mark as processed
    processed_dlls[$current_dll]=1
    
    # Copy DLL to output directory
    cp "$DLL_SOURCE_DIR/$current_dll" "$BUILD_DIR/"
    
    # Get dependencies of current DLL
    subdeps=$(get_dependencies "$DLL_SOURCE_DIR/$current_dll")
    
    # Add new dependencies to processing queue
    for dll in $subdeps; do
        if [ "${processed_dlls[$dll]}" != "1" ]; then
            if dll_exists "$dll"; then
                dlls_to_process+=("$dll")
            else
                # Only add to missing if not already listed
                if [[ ! " ${missing_dlls[@]} " =~ " ${dll} " ]]; then
                    missing_dlls+=("$dll")
                fi
            fi
        fi
    done
done

# Print summary
echo -e "\nBuild Complete!"
echo "Found and copied ${#processed_dlls[@]} DLLs"

if [ ${#missing_dlls[@]} -gt 0 ]; then
    echo -e "\nNote: The following DLLs were not found in the source directory:"
    printf '%s\n' "${missing_dlls[@]}"
    echo "These are likely system DLLs that will be present on the target Windows system."
fi

echo -e "\nYour application and all required DLLs are in: $BUILD_DIR"
