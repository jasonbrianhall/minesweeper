# Makefile for MS-DOS with DPMI support
VERSION=1.0

# Source files
SRCS = minesweeper.cpp highscores.cpp ncurses.cpp

# Output executable
DOS_TARGET = mnsweep.exe

# Docker image for DJGPP
DJGPP_IMAGE = djfdyuruiry/djgpp

# Correct CSDPMI URL
CSDPMI_URL = http://na.mirror.garr.it/mirrors/djgpp/current/v2misc/csdpmi7b.zip

# Get current user and group IDs for Docker
USER_ID = $(shell id -u)
GROUP_ID = $(shell id -g)

# Default target
all: msdos

# Target to pull the DJGPP Docker image
pull-djgpp:
	docker pull $(DJGPP_IMAGE)

# Target to download CSDPMI
get-csdpmi:
	wget $(CSDPMI_URL)
	unzip -o csdpmi7b.zip -d csdpmi

# Target to build for MS-DOS using DJGPP in Docker
msdos: pull-djgpp get-csdpmi
	docker run --rm -v $(PWD):/src:z -u $(USER_ID):$(GROUP_ID) $(DJGPP_IMAGE) /bin/sh -c "cd /src && g++ $(SRCS) -o $(DOS_TARGET)"
	cp csdpmi/bin/CWSDPMI.EXE .

# Target to run the MS-DOS executable using DOSBox
run: msdos
	dosbox $(DOS_TARGET)

# Clean target to remove generated files
clean:
	rm -f $(DOS_TARGET) csdpmi7b.zip || true
	rm -rf csdpmi || true
	rm *.exe || true
	rm *.EXE || true
	rm *.zip || true
	rm *.zip.* || true

.PHONY: all pull-djgpp get-csdpmi msdos run clean
