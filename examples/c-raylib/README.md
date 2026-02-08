# ClayKit C + Raylib Demo

This example demonstrates all ClayKit components using pure C with Raylib for rendering.

**No external dependencies required!** Raylib is compiled from source (included in `vendor/src/`).

## Build

```bash
make
```

## Run

```bash
make run
```

## Clean

```bash
make clean
```

## Components Demonstrated

- Badge
- Button (with hover state)
- Progress bar
- Slider
- Text Input (with cursor and keyboard handling)
- Checkbox
- Switch
- Alert
- Tooltip
- Tabs

## Files

- `main.c` - Demo application
- `Makefile` - Build configuration (compiles raylib from source)
- `../../vendor/raylib/` - Raylib (latest from main branch)
- `../../vendor/clay.h` - Clay UI header
- `../../clay_kit.h` - ClayKit components

## Platform Support

The Makefile automatically detects your platform:
- **macOS** - Uses Cocoa, OpenGL frameworks
- **Linux** - Uses X11, OpenGL
- **Windows** - Uses MinGW (GDI32, OpenGL32)
