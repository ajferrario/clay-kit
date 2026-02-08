# ClayKit C + Raylib Demo Makefile
#
# This example compiles raylib from source (included in vendor/src/)
# No external dependencies needed!
#
# Build:   make
# Run:     make run
# Clean:   make clean

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers

# ClayKit and Clay includes
CLAYKIT_INCLUDES = -I../.. -I../../vendor

# Raylib source directory
RAYLIB_SRC = ../../vendor/raylib/src
RAYLIB_INCLUDES = -I$(RAYLIB_SRC) -I$(RAYLIB_SRC)/external/glfw/include

# Raylib source files (latest raylib merged utils.c into rcore.c)
RAYLIB_SRCS = $(RAYLIB_SRC)/rcore.c \
              $(RAYLIB_SRC)/rshapes.c \
              $(RAYLIB_SRC)/rtextures.c \
              $(RAYLIB_SRC)/rtext.c \
              $(RAYLIB_SRC)/rmodels.c \
              $(RAYLIB_SRC)/raudio.c

# Platform-specific configuration
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    # macOS
    RAYLIB_SRCS += $(RAYLIB_SRC)/rglfw.c
    PLATFORM_CFLAGS = -DPLATFORM_DESKTOP -ObjC
    PLATFORM_LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework CoreFoundation
else ifeq ($(UNAME_S),Linux)
    # Linux
    RAYLIB_SRCS += $(RAYLIB_SRC)/rglfw.c
    PLATFORM_CFLAGS = -DPLATFORM_DESKTOP
    PLATFORM_LIBS = -lGL -lm -lpthread -ldl -lrt -lX11
else
    # Windows (MinGW)
    RAYLIB_SRCS += $(RAYLIB_SRC)/rglfw.c
    PLATFORM_CFLAGS = -DPLATFORM_DESKTOP
    PLATFORM_LIBS = -lopengl32 -lgdi32 -lwinmm
endif

ALL_INCLUDES = $(CLAYKIT_INCLUDES) $(RAYLIB_INCLUDES)
ALL_CFLAGS = $(CFLAGS) $(PLATFORM_CFLAGS)

TARGET = claykit-demo-c
SRC = main.c

.PHONY: all clean run resources

all: $(TARGET)

# Build raylib as a static library first
libraylib.a: $(RAYLIB_SRCS)
	@echo "Building raylib..."
	$(CC) $(ALL_CFLAGS) $(RAYLIB_INCLUDES) -c $(RAYLIB_SRCS)
	ar rcs $@ *.o
	rm -f *.o

# Build the demo
$(TARGET): $(SRC) libraylib.a
	@echo "Building demo..."
	$(CC) $(ALL_CFLAGS) $(ALL_INCLUDES) -o $@ $< -L. -lraylib $(PLATFORM_LIBS)

# Copy resources from zig-raylib example
resources:
	mkdir -p resources
	cp ../zig-raylib/src/resources/*.ttf resources/ 2>/dev/null || echo "No font files to copy"

run: $(TARGET) resources
	./$(TARGET)

clean:
	rm -f $(TARGET) libraylib.a *.o
	rm -rf resources
