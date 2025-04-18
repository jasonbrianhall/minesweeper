#!/bin/bash

# Variables
DJGPP_IMAGE="djfdyuruiry/djgpp"
PDCURSES_REPO="https://github.com/wmcbrine/PDCurses.git"
PDCURSES_DIR="pdcurses_build"
CSDPMI_URL="http://na.mirror.garr.it/mirrors/djgpp/current/v2misc/csdpmi7b.zip"
USER_ID=$(id -u)
GROUP_ID=$(id -g)
DOS_TARGET="mnsweep.exe"
CWSDSTUB_URL="http://na.mirror.garr.it/mirrors/djgpp/current/v2/djdev205.zip"

# Check and compile Linux version if g++ is available
if command -v g++ &> /dev/null; then
    echo "Compiling Linux version..."
    g++ minesweeper.cpp highscores.cpp -lncurses -o minesweeper
else
    echo "g++ not found - skipping Linux build"
fi

# Check and compile Windows version if cross-compiler is available
if command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    mkdir -p win_minesweeper

    echo "Compiling Windows version (PDCurses) ..."
    x86_64-w64-mingw32-g++ minesweeper.cpp highscores.cpp -lpdcurses -std=c++14 -o win_minesweeper/minesweepr_pdcurses.exe
    ./collect_dlls.sh win_minesweeper/minesweepr_pdcurses.exe /usr/x86_64-w64-mingw32/sys-root/mingw/bin win_minesweeper
else
    echo "Windows cross-compiler not found - skipping Windows build"
fi

# Download CSDPMI if needed
if [ ! -d "csdpmi" ]; then
    echo "Downloading CSDPMI..."
    wget ${CSDPMI_URL}
    mkdir -p csdpmi
    unzip -o csdpmi7b.zip -d csdpmi
fi

# Download CWSDSTUB if needed
# CWSDSTUB.EXE should be in the csdpmi/bin directory along with CWSDPMI.EXE
if [ ! -f "csdpmi/bin/CWSDSTUB.EXE" ]; then
    echo "CWSDSTUB.EXE not found in csdpmi/bin directory"
    echo "Make sure you have the complete CSDPMI package"
    exit 1
fi

# Clone PDCurses if needed
if [ ! -d "${PDCURSES_DIR}" ]; then
    echo "Cloning PDCurses repository..."
    git clone ${PDCURSES_REPO} ${PDCURSES_DIR}
fi

# Build MSDOS version with PDCurses using Docker
echo "Building MSDOS version with PDCurses..."

# Pull the DJGPP Docker image
echo "Pulling DJGPP Docker image..."
docker pull ${DJGPP_IMAGE}

# Create a script to run inside Docker for building PDCurses and the application
cat > build_msdos.sh << 'EOF'
#!/bin/bash
set -e

# Build PDCurses for MSDOS
cd /src/pdcurses_build/dos
echo "Building PDCurses for MSDOS..."
# First clean any existing build artifacts to ensure a fresh build
make -f Makefile clean PDCURSES_SRCDIR=/src/pdcurses_build PLATFORM=djgpp
# Now build PDCurses
make -f Makefile PDCURSES_SRCDIR=/src/pdcurses_build PLATFORM=djgpp

# Move PDCurses library to a location we can reference
mkdir -p /src/lib /src/include
cp pdcurses.a /src/lib/libpdcurses.a
cp /src/pdcurses_build/curses.h /src/include/
cp /src/pdcurses_build/panel.h /src/include/

# Create an empty term.h if needed by your code
# (term.h is not part of PDCurses but might be referenced by your code)
touch /src/include/term.h

# Build the minesweeper application
cd /src
echo "Building minesweeper for MSDOS..."
# Add preprocessor define for MSDOS to handle any platform-specific code
g++ minesweeper.cpp highscores.cpp -o mnsweep.exe -I/src/include -L/src/lib -lpdcurses -DMSDOS

# Now embed CWSDPMI directly into the executable
echo "Embedding CWSDPMI into executable..."
# First convert the EXE to COFF format
exe2coff mnsweep.exe
cp mnsweep mnsweep.coff
# Then combine CWSDSTUB with our program
cat /src/CWSDSTUB.EXE mnsweep.coff > embedded.exe
# Rename the embedded executable to our target name
mv embedded.exe mnsweep.exe
# Make sure it's executable
chmod +x mnsweep.exe

echo "Build complete with embedded DPMI!"
EOF

# Make the script executable
chmod +x build_msdos.sh

# Create a temporary directory to avoid symlink issues
TEMP_BUILD_DIR=$(mktemp -d)
echo "Created temporary build directory: ${TEMP_BUILD_DIR}"

# Resolve symbolic links for source files
echo "Resolving symbolic links for source files..."

# For highscores.cpp - follow the symlink to get the actual file
HIGHSCORES_CPP_REAL=$(readlink -f highscores.cpp)
if [ -f "$HIGHSCORES_CPP_REAL" ]; then
    echo "Found real highscores.cpp at: $HIGHSCORES_CPP_REAL"
    cp "$HIGHSCORES_CPP_REAL" "${TEMP_BUILD_DIR}/highscores.cpp"
else
    echo "Error: Could not resolve symlink for highscores.cpp"
    exit 1
fi

# For highscores.h - if it exists and is a symlink
if [ -f "highscores.h" ]; then
    HIGHSCORES_H_REAL=$(readlink -f highscores.h)
    if [ -f "$HIGHSCORES_H_REAL" ]; then
        echo "Found real highscores.h at: $HIGHSCORES_H_REAL"
        cp "$HIGHSCORES_H_REAL" "${TEMP_BUILD_DIR}/highscores.h"
    else
        echo "Warning: Could not resolve symlink for highscores.h"
    fi
fi

# Copy thread.h - your custom file
if [ -f "thread.h" ]; then
    THREAD_H_REAL=$(readlink -f thread.h)
    if [ -f "$THREAD_H_REAL" ]; then
        echo "Found real thread.h at: $THREAD_H_REAL"
        cp "$THREAD_H_REAL" "${TEMP_BUILD_DIR}/thread.h"
    else
        # If it's not a symlink, just copy it directly
        cp -f thread.h "${TEMP_BUILD_DIR}/thread.h"
        echo "Copied thread.h to build directory"
    fi
else
    echo "Warning: thread.h not found"
fi

# Copy minesweeper.cpp directly
cp -L minesweeper.cpp "${TEMP_BUILD_DIR}/"

# Copy PDCurses and build script
cp -r "${PDCURSES_DIR}" "${TEMP_BUILD_DIR}/"
cp build_msdos.sh "${TEMP_BUILD_DIR}/"

# Copy CWSDSTUB.EXE to the build directory
if [ -f "csdpmi/bin/CWSDSTUB.EXE" ]; then
    cp csdpmi/bin/CWSDSTUB.EXE "${TEMP_BUILD_DIR}/"
    echo "Copied CWSDSTUB.EXE to build directory"
else
    echo "Error: CWSDSTUB.EXE not found in csdpmi/bin directory"
    exit 1
fi

# List files in the temporary directory for verification
echo "Files in temporary build directory:"
ls -la "${TEMP_BUILD_DIR}"

# Run the Docker container with the temporary directory
echo "Starting Docker build process..."
docker run --rm -v "${TEMP_BUILD_DIR}:/src:z" -u ${USER_ID}:${GROUP_ID} ${DJGPP_IMAGE} /src/build_msdos.sh

# Copy back the built files
echo "Copying built files from temporary directory..."
cp "${TEMP_BUILD_DIR}/${DOS_TARGET}" ./ 2>/dev/null || echo "Failed to copy executable"
cp "${TEMP_BUILD_DIR}/lib/libpdcurses.a" ./ 2>/dev/null || echo "Failed to copy PDCurses library"

# Clean up
echo "Cleaning up temporary directory..."
rm -rf "${TEMP_BUILD_DIR}"

# Check if build was successful
if [ -f "${DOS_TARGET}" ]; then
    echo "MSDOS build successful! Files created:"
    echo "- ${DOS_TARGET} (with embedded DPMI extender)"
    echo "The executable should now run without requiring CWSDPMI.EXE"
    echo "To run in DOSBox, execute: dosbox ${DOS_TARGET}"
else
    echo "MSDOS build failed."
fi
