PROJECT_NAME = QWEE_Engine
VERSION = 0.3
BUILD_DATE = $(shell date +"%Y-%m-%d %H:%M:%S")

CC = gcc
CXX = g++

ifeq ($(OS),Windows_NT)
    PLATFORM = WINDOWS
    RAYLIB_LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
    EXE_EXT = .exe
    RM = del /Q
    MKDIR = mkdir
    RMDIR = rmdir /S /Q
    COPY = copy
    SEP = \\
else
    PLATFORM = UNIX
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM = LINUX
        RAYLIB_LIBS = -lraylib -lm -lpthread -ldl
    endif
    ifeq ($(UNAME_S),Darwin)
        PLATFORM = MACOS
        RAYLIB_LIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit
    endif
    EXE_EXT =
    RM = rm -f
    MKDIR = mkdir -p
    RMDIR = rm -rf
    COPY = cp
    SEP = /
endif

SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin
LIB_DIR = lib
INCLUDE_DIR = include
EXAMPLES_DIR = examples
SCRIPTS_DIR = scripts

CORE_SOURCES = \
    $(SRC_DIR)$(SEP)engine.c \
    $(SRC_DIR)$(SEP)objects.c \
    $(SRC_DIR)$(SEP)physics.c \
    $(SRC_DIR)$(SEP)camera.c \
    $(SRC_DIR)$(SEP)utils.c \
    $(SRC_DIR)$(SEP)vector_math.c \
    $(SRC_DIR)$(SEP)particles.c \
    $(SRC_DIR)$(SEP)fog.c \
    $(SRC_DIR)$(SEP)shadows.c \
    $(SRC_DIR)$(SEP)audio.c \
    $(SRC_DIR)$(SEP)scene.c \
    $(SRC_DIR)$(SEP)billboard.c

HEADERS = \
    $(SRC_DIR)$(SEP)engine.h \
    $(SRC_DIR)$(SEP)objects.h \
    $(SRC_DIR)$(SEP)physics.h \
    $(SRC_DIR)$(SEP)camera.h \
    $(SRC_DIR)$(SEP)utils.h \
    $(SRC_DIR)$(SEP)vector_math.h \
    $(SRC_DIR)$(SEP)particles.h \
    $(SRC_DIR)$(SEP)fog.h \
    $(SRC_DIR)$(SEP)shadows.h \
    $(SRC_DIR)$(SEP)audio.h \
    $(SRC_DIR)$(SEP)scene.h

EXAMPLES = \
    $(EXAMPLES_DIR)$(SEP)arena_shooter$(SEP)script.c \
    $(EXAMPLES_DIR)$(SEP)empty_template$(SEP)game.c \
    $(EXAMPLES_DIR)$(SEP)platformer$(SEP)platformer.c

CORE_OBJECTS = $(patsubst $(SRC_DIR)$(SEP)%.c,$(OBJ_DIR)$(SEP)%.o,$(CORE_SOURCES))

CFLAGS = -Wall -Wextra -std=c99 -pedantic
OPTIMIZATION = -O2 -s
DEBUG_FLAGS = -g -DDEBUG -O0
RELEASE_FLAGS = $(OPTIMIZATION) -DNDEBUG
WARNING_FLAGS = -Wno-unused-parameter -Wno-unused-variable
INCLUDE_FLAGS = -I$(SRC_DIR) -I$(INCLUDE_DIR)

LDFLAGS = $(RAYLIB_LIBS)

TARGET_ENGINE_LIB = $(BIN_DIR)$(SEP)libqwengine.a
TARGET_ARENA_SHOOTER = $(BIN_DIR)$(SEP)arena_shooter$(EXE_EXT)
TARGET_EMPTY_TEMPLATE = $(BIN_DIR)$(SEP)empty_game$(EXE_EXT)
TARGET_PLATFORMER = $(BIN_DIR)$(SEP)platformer$(EXE_EXT)

SCRIPT_INTERPRETER = $(SCRIPTS_DIR)$(SEP)script_interpreter.py

all: setup engine examples

setup:
	@echo "Setting up build directories..."
	@$(MKDIR) $(OBJ_DIR) $(BIN_DIR) $(EXAMPLES_DIR)$(SEP)arena_shooter
	@$(MKDIR) $(EXAMPLES_DIR)$(SEP)empty_template $(EXAMPLES_DIR)$(SEP)platformer
	@echo "Platform: $(PLATFORM)"
	@echo "Build Date: $(BUILD_DATE)"

engine: $(CORE_OBJECTS)
	@echo "Building $(PROJECT_NAME) v$(VERSION) static library..."
	@ar rcs $(TARGET_ENGINE_LIB) $(CORE_OBJECTS)
	@echo "✓ Engine library built: $(TARGET_ENGINE_LIB)"

examples: arena_shooter empty_template platformer

arena_shooter: engine
	@echo "Building Arena Shooter example..."
	@$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(WARNING_FLAGS) $(INCLUDE_FLAGS) \
		$(EXAMPLES_DIR)$(SEP)arena_shooter$(SEP)script.c \
		-L$(BIN_DIR) -lqwengine \
		$(LDFLAGS) -o $(TARGET_ARENA_SHOOTER)
	@echo "✓ Arena Shooter built: $(TARGET_ARENA_SHOOTER)"

empty_template: engine
	@echo "Building empty game template..."
	@$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(WARNING_FLAGS) $(INCLUDE_FLAGS) \
		$(EXAMPLES_DIR)$(SEP)empty_template$(SEP)game.c \
		-L$(BIN_DIR) -lqwengine \
		$(LDFLAGS) -o $(TARGET_EMPTY_TEMPLATE)
	@echo "✓ Empty template built: $(TARGET_EMPTY_TEMPLATE)"

platformer: engine
	@echo "Building Platformer example..."
	@$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(WARNING_FLAGS) $(INCLUDE_FLAGS) \
		$(EXAMPLES_DIR)$(SEP)platformer$(SEP)platformer.c \
		-L$(BIN_DIR) -lqwengine \
		$(LDFLAGS) -o $(TARGET_PLATFORMER)
	@echo "✓ Platformer built: $(TARGET_PLATFORMER)"

debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean setup
	@echo "Building debug version..."
	@$(MAKE) engine
	@echo "Debug build complete. Use 'gdb $(BIN_DIR)$(SEP)arena_shooter' to debug."

release: CFLAGS += $(RELEASE_FLAGS)
release: clean all
	@echo "Release build complete."

$(OBJ_DIR)$(SEP)%.o: $(SRC_DIR)$(SEP)%.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $(WARNING_FLAGS) $(INCLUDE_FLAGS) -c $< -o $@

script: $(SCRIPT_INTERPRETER)
	@echo "Running script interpreter..."
	@python $(SCRIPT_INTERPRETER) $(EXAMPLES_DIR)$(SEP)game.qwee $(EXAMPLES_DIR)$(SEP)generated_game.c
	@echo "✓ Script compiled to $(EXAMPLES_DIR)$(SEP)generated_game.c"

build_script: script
	@$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(WARNING_FLAGS) $(INCLUDE_FLAGS) \
		$(EXAMPLES_DIR)$(SEP)generated_game.c \
		-L$(BIN_DIR) -lqwengine \
		$(LDFLAGS) -o $(BIN_DIR)$(SEP)script_game$(EXE_EXT)
	@echo "✓ Script game built: $(BIN_DIR)$(SEP)script_game$(EXE_EXT)"

install_raylib_windows:
ifeq ($(PLATFORM),WINDOWS)
	@echo "Installing Raylib for Windows..."
	@powershell -Command "Invoke-WebRequest -Uri 'https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_win64_mingw-w64.zip' -OutFile 'raylib.zip'"
	@powershell -Command "Expand-Archive -Path 'raylib.zip' -DestinationPath '$(LIB_DIR)' -Force"
	@$(MKDIR) $(INCLUDE_DIR)
	@$(COPY) $(LIB_DIR)$(SEP)raylib-5.0_win64_mingw-w64$(SEP)include$(SEP)*.h $(INCLUDE_DIR)$(SEP)
	@$(COPY) $(LIB_DIR)$(SEP)raylib-5.0_win64_mingw-w64$(SEP)lib$(SEP)*.a $(LIB_DIR)$(SEP)
	@$(RM) raylib.zip
	@echo "✓ Raylib installed"
else
	@echo "This target is for Windows only."
	@echo "Linux: sudo apt-get install libraylib-dev"
	@echo "macOS: brew install raylib"
endif

install_deps:
ifeq ($(PLATFORM),LINUX)
	@echo "Installing dependencies for Linux..."
	@sudo apt-get update
	@sudo apt-get install -y gcc make libraylib-dev libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev
else ifeq ($(PLATFORM),MACOS)
	@echo "Installing dependencies for macOS..."
	@brew update
	@brew install raylib
else ifeq ($(PLATFORM),WINDOWS)
	@echo "Please run: make install_raylib_windows"
endif

run_arena:
	@echo "Running Arena Shooter..."
	@$(BIN_DIR)$(SEP)arena_shooter$(EXE_EXT)

run_empty:
	@echo "Running empty template..."
	@$(BIN_DIR)$(SEP)empty_game$(EXE_EXT)

run_platformer:
	@echo "Running platformer..."
	@$(BIN_DIR)$(SEP)platformer$(EXE_EXT)

run_script_game:
	@echo "Running script game..."
	@$(BIN_DIR)$(SEP)script_game$(EXE_EXT)

test: arena_shooter empty_template platformer
	@echo "Testing all examples..."
	@echo "1. Arena Shooter - FPS shooter with waves"
	@echo "2. Empty Template - Minimal game template"
	@echo "3. Platformer - 3D platformer example"
	@echo "Run individual examples with: make run_arena, make run_empty, make run_platformer"

dist: release
	@echo "Creating distribution package..."
	@$(MKDIR) dist$(SEP)qwengine-$(VERSION)
	@$(COPY) $(SRC_DIR)$(SEP)*.c dist$(SEP)qwengine-$(VERSION)$(SEP)
	@$(COPY) $(SRC_DIR)$(SEP)*.h dist$(SEP)qwengine-$(VERSION)$(SEP)
	@$(COPY) Makefile dist$(SEP)qwengine-$(VERSION)$(SEP)
	@$(COPY) README.md dist$(SEP)qwengine-$(VERSION)$(SEP)
	@$(COPY) LICENSE dist$(SEP)qwengine-$(VERSION)$(SEP)
	@$(MKDIR) dist$(SEP)qwengine-$(VERSION)$(SEP)examples
	@$(COPY) $(EXAMPLES_DIR)$(SEP)* dist$(SEP)qwengine-$(VERSION)$(SEP)examples$(SEP)
	@$(MKDIR) dist$(SEP)qwengine-$(VERSION)$(SEP)scripts
	@$(COPY) $(SCRIPTS_DIR)$(SEP)* dist$(SEP)qwengine-$(VERSION)$(SEP)scripts$(SEP)
	@cd dist && tar -czf qwengine-$(VERSION).tar.gz qwengine-$(VERSION)
	@echo "✓ Distribution package created: dist$(SEP)qwengine-$(VERSION).tar.gz"

clean:
	@echo "Cleaning build artifacts..."
	@$(RMDIR) $(OBJ_DIR) $(BIN_DIR) 2>nul || true
	@$(RM) $(EXAMPLES_DIR)$(SEP)generated_game.c 2>nul || true
	@echo "✓ Clean complete"

distclean: clean
	@echo "Cleaning everything..."
	@$(RMDIR) dist 2>nul || true
	@$(RM) *.exe 2>nul || true
	@$(RM) *.o 2>nul || true
	@$(RM) *.a 2>nul || true
	@echo "✓ Full clean complete"

help:
	@echo "============================================"
	@echo "QWEE Engine v$(VERSION) - Makefile Help"
	@echo "============================================"
	@echo ""
	@echo "Platform: $(PLATFORM)"
	@echo ""
	@echo "Build Targets:"
	@echo "  all               - Build everything (release mode)"
	@echo "  engine            - Build core engine library"
	@echo "  examples          - Build all example games"
	@echo "  arena_shooter     - Build Arena Shooter example"
	@echo "  empty_template    - Build empty game template"
	@echo "  platformer        - Build platformer example"
	@echo ""
	@echo "Development Targets:"
	@echo "  debug             - Build with debug symbols"
	@echo "  release           - Build optimized release"
	@echo "  script            - Convert .qwee script to C code"
	@echo "  build_script      - Build from script file"
	@echo ""
	@echo "Installation:"
	@echo "  install_deps      - Install dependencies for current platform"
	@echo "  install_raylib_windows - Install Raylib on Windows"
	@echo ""
	@echo "Running:"
	@echo "  run_arena         - Run Arena Shooter"
	@echo "  run_empty         - Run empty template"
	@echo "  run_platformer    - Run platformer"
	@echo "  run_script_game   - Run script-generated game"
	@echo "  test              - Build and test all examples"
	@echo ""
	@echo "Distribution:"
	@echo "  dist              - Create distribution package"
	@echo "  clean             - Clean build artifacts"
	@echo "  distclean         - Clean everything"
	@echo "  help              - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  $$ make all                # Build everything"
	@echo "  $$ make debug              # Debug build"
	@echo "  $$ make run_arena          # Run Arena Shooter"
	@echo "  $$ make clean              # Clean build"
	@echo ""

ifeq ($(PLATFORM),WINDOWS)
LDFLAGS += -static -static-libgcc -static-libstdc++
endif

ifeq ($(PLATFORM),LINUX)
LDFLAGS += -no-pie
endif

ifeq ($(PLATFORM),MACOS)
CFLAGS += -ObjC
endif

.PHONY: all setup engine examples arena_shooter empty_template platformer \
        debug release script build_script install_raylib_windows install_deps \
        run_arena run_empty run_platformer run_script_game test dist clean \
        distclean help

$(OBJ_DIR)$(SEP)engine.o: $(SRC_DIR)$(SEP)engine.c $(SRC_DIR)$(SEP)engine.h \
                         $(SRC_DIR)$(SEP)objects.h $(SRC_DIR)$(SEP)physics.h \
                         $(SRC_DIR)$(SEP)camera.h $(SRC_DIR)$(SEP)utils.h \
                         $(SRC_DIR)$(SEP)particles.h $(SRC_DIR)$(SEP)fog.h \
                         $(SRC_DIR)$(SEP)shadows.h $(SRC_DIR)$(SEP)audio.h \
                         $(SRC_DIR)$(SEP)scene.h

$(OBJ_DIR)$(SEP)objects.o: $(SRC_DIR)$(SEP)objects.c $(SRC_DIR)$(SEP)objects.h \
                          $(SRC_DIR)$(SEP)engine.h $(SRC_DIR)$(SEP)physics.h

$(OBJ_DIR)$(SEP)physics.o: $(SRC_DIR)$(SEP)physics.c $(SRC_DIR)$(SEP)physics.h \
                          $(SRC_DIR)$(SEP)engine.h $(SRC_DIR)$(SEP)objects.h

$(OBJ_DIR)$(SEP)camera.o: $(SRC_DIR)$(SEP)camera.c $(SRC_DIR)$(SEP)camera.h \
                         $(SRC_DIR)$(SEP)engine.h $(SRC_DIR)$(SEP)vector_math.h

DEPFILES = $(CORE_OBJECTS:.o=.d)

%.d: %.c
	@$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -MM -MT '$(patsubst $(SRC_DIR)$(SEP)%.c,$(OBJ_DIR)$(SEP)%.o,$<)' $< > $@

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $(DEPFILES)
endif
endif

print_info:
	@echo "========================================"
	@echo "QWEE Engine Build Information"
	@echo "========================================"
	@echo "Version: $(VERSION)"
	@echo "Platform: $(PLATFORM)"
	@echo "Compiler: $(CC)"
	@echo "Build Date: $(BUILD_DATE)"
	@echo "Source Files: $(words $(CORE_SOURCES))"
	@echo "Example Games: $(words $(EXAMPLES))"
	@echo "========================================"