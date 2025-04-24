# Makefile for MS-DOS or FreeDOS with DPMI support
VERSION=1.0

# Source files
SRCS = minesweeper.cpp highscores.cpp ncurses.cpp

# Output executable
DOS_TARGET = mnsweep.exe
DOS_COFF = mnsweep
COFF_TARGET = mnsweep.coff
EMBEDDED_TARGET = embedded.exe

# Docker image for DJGPP
DJGPP_IMAGE = djfdyuruiry/djgpp

# Correct CSDPMI URL
CSDPMI_URL = http://na.mirror.garr.it/mirrors/djgpp/current/v2misc/csdpmi7b.zip

# Get current user and group IDs for Docker
USER_ID = $(shell id -u)
GROUP_ID = $(shell id -g)

# Default target
all: msdos_embedded

# Target to pull the DJGPP Docker image
pull-djgpp:
	docker pull $(DJGPP_IMAGE)

# Target to download CSDPMI
get-csdpmi:
	@if [ ! -d "csdpmi" ]; then \
		wget $(CSDPMI_URL); \
		unzip -o csdpmi7b.zip -d csdpmi; \
	fi

# Original MSDOS build target that requires separate CWSDPMI.EXE
msdos_original: pull-djgpp get-csdpmi
	docker run --rm -v $(PWD):/src:z -u $(USER_ID):$(GROUP_ID) $(DJGPP_IMAGE) /bin/sh -c "cd /src && g++ $(SRCS) -o $(DOS_TARGET)"
	cp csdpmi/bin/CWSDPMI.EXE .

# Target to build for MS-DOS with embedded CWSDPMI
msdos_embedded: pull-djgpp get-csdpmi
	@if [ ! -f "csdpmi/bin/CWSDSTUB.EXE" ]; then \
		echo "ERROR: CWSDSTUB.EXE not found in csdpmi/bin directory!"; \
		echo "Make sure you have the complete CSDPMI package."; \
		exit 1; \
	fi
	docker run --rm -v $(PWD):/src:z -u $(USER_ID):$(GROUP_ID) $(DJGPP_IMAGE) /bin/sh -c "\
		cd /src && \
		g++ -Os -s -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--no-keep-memory $(SRCS) -o $(DOS_TARGET) && \
		exe2coff $(DOS_TARGET) && \
		cp $(DOS_COFF) $(COFF_TARGET) && \
		cat csdpmi/bin/CWSDSTUB.EXE $(COFF_TARGET) > $(EMBEDDED_TARGET) && \
		mv $(EMBEDDED_TARGET) $(DOS_TARGET) && \
		chmod +x $(DOS_TARGET) && \
		rm -f $(COFF_TARGET)"
	@echo "Created $(DOS_TARGET) with embedded DPMI extender"

# Alias for backward compatibility
msdos: msdos_embedded

# Target to run the MS-DOS executable using DOSBox
run: msdos_embedded
	dosbox $(DOS_TARGET)

# Clean target to remove generated files
clean:
	rm -f $(DOS_TARGET) $(COFF_TARGET) $(EMBEDDED_TARGET) csdpmi7b.zip || true
	rm -rf csdpmi || true
	rm -f *.exe *.EXE *.zip *.zip.* || true

.PHONY: all pull-djgpp get-csdpmi msdos msdos_original msdos_embedded run clean
