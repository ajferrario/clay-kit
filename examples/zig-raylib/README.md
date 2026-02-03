# ClayKit Zig + Raylib Example

A sample application demonstrating ClayKit components rendered with Raylib.

## Prerequisites

- [Zig](https://ziglang.org/download/) (0.13.0 or later recommended)
- No external dependencies needed - raylib and clay-zig are fetched automatically

## Building

```bash
cd examples/zig-raylib
zig build
```

## Running

```bash
zig build run
```

## What it demonstrates

- **Theme System**: Uses `CLAYKIT_THEME_LIGHT` colors throughout
- **VStack**: Left panel with vertically stacked items
- **Center**: Middle panel with centered content
- **HStack**: Color swatches arranged horizontally
- **Spacer**: Pushes footer to bottom of right panel
- **Box styling**: Rounded corners, backgrounds, padding

## Project structure

```
examples/zig-raylib/
├── build.zig       # Zig build configuration
├── build.zig.zon   # Dependencies (raylib-zig, clay-zig)
├── src/
│   └── main.zig    # Demo application
└── README.md
```

## Verifying Zig compatibility

To verify that `clay_kit.h` translates cleanly:

```bash
zig translate-c ../../clay_kit.h -I../../vendor
```

This should produce no errors if the header is Zig-compatible.
