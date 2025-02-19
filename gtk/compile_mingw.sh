#!/bin/bash

# Help function
show_help() {
    echo "Usage: $0 [OPTIONS] [EXE_PATH] [DLL_SOURCE_DIR] [OUTPUT_DIR]"
    echo
    echo "Compiles a GTK3 application and collects required DLLs."
    echo
    echo "When run without parameters, uses these defaults:"
    echo "  EXE_PATH: Compiles minesweeper from current directory"
    echo "  DLL_SOURCE_DIR: /usr/x86_64-w64-mingw32/sys-root/mingw/bin"
    echo "  OUTPUT_DIR: ./build"
    echo
    echo "Options:"
    echo "  -h, --help    Show this help message"
    echo
    echo "Examples:"
    echo "  $0                   # Use defaults"
    echo "  $0 ./myapp.exe /path/to/dlls ./dist    # Custom paths"
    exit 0
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            ;;
        *)
            if [ -z "$EXE_PATH" ]; then
                EXE_PATH="$1"
            elif [ -z "$DLL_SOURCE_DIR" ]; then
                DLL_SOURCE_DIR="$1"
            elif [ -z "$OUTPUT_DIR" ]; then
                OUTPUT_DIR="$1"
            else
                echo "Error: Too many arguments"
                show_help
            fi
            shift
            ;;
    esac
done

# Set default values if not provided
BUILD_DIR=${OUTPUT_DIR:-"build"}
DLL_SOURCE_DIR=${DLL_SOURCE_DIR:-"/usr/x86_64-w64-mingw32/sys-root/mingw/bin"}

# Create build directory
mkdir -p "$BUILD_DIR"

# Compile the application if no exe path provided
if [ -z "$EXE_PATH" ]; then
    echo "Compiling minesweeper..."
    x86_64-w64-mingw32-gcc -o "$BUILD_DIR/minesweeper_gtk.exe" gtk_minesweeper.cpp highscores.cpp `mingw64-pkg-config --cflags gtk+-3.0 --libs gtk+-3.0` -lstdc++
    
    if [ $? -ne 0 ]; then
        echo "Compilation failed!"
        exit 1
    fi
    
    echo "Compilation successful!"
    EXE_PATH="$BUILD_DIR/minesweeper_gtk.exe"
else
    # If exe path provided but file doesn't exist
    if [ ! -f "$EXE_PATH" ]; then
        echo "Error: File not found: $EXE_PATH"
        exit 1
    fi
fi

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

# Verify DLL directory exists
if [ ! -d "$DLL_SOURCE_DIR" ]; then
    echo "Error: DLL directory not found: $DLL_SOURCE_DIR"
    exit 1
fi

echo "Using DLL source directory: $DLL_SOURCE_DIR"
echo "Output directory: $BUILD_DIR"

# Initialize arrays for processing
declare -A processed_dlls
declare -a dlls_to_process
declare -a missing_dlls

# Get initial dependencies
echo "Analyzing dependencies for $(basename "$EXE_PATH")..."
initial_dlls=$(get_dependencies "$EXE_PATH")

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
