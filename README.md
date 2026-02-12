# ClayKit

Zero-allocation UI components for [Clay](https://github.com/nicbarker/clay).

## What is ClayKit?

A single-header C99 library providing ready-to-use UI components built on Clay's layout system. Includes hand-written Zig bindings for seamless integration.

## Installation

### C

Copy `clay_kit.h` and `vendor/clay.h` into your project. In **one** source file:

```c
#define CLAY_IMPLEMENTATION
#include "clay.h"
#define CLAYKIT_IMPLEMENTATION
#include "clay_kit.h"
```

### Zig

Add ClayKit as a dependency in your `build.zig.zon`, then in `build.zig`:

```zig
const claykit_dep = b.dependency("clay_kit", .{
    .target = target,
    .optimize = optimize,
});
exe.root_module.addImport("claykit", claykit_dep.module("claykit"));
```

## Features

- **Zero heap allocation** - You provide all memory
- **Single header** - Just include `clay_kit.h`
- **Pure C99** - Maximum portability
- **Zig bindings** - Hand-written ergonomic API (not `@cImport`)
- **Theming** - Light/dark presets or custom themes
- **Complete text input** - Cursor, selection, keyboard handling

## Components

### Typography & Data Display
| Component | Description |
|-----------|-------------|
| **Badge** | Status labels with solid/subtle/outline variants |
| **Tag** | Closeable labels with solid/subtle/outline variants |
| **Stat** | Label/value/help-text statistics display |
| **List** | Ordered (numbered) or unordered (bullet) lists |
| **Table** | Tabular data with optional striping and borders |

### Form Controls
| Component | Description |
|-----------|-------------|
| **Button** | Interactive buttons with solid/outline/ghost variants |
| **Text Input** | Full text editing with cursor positioning and selection |
| **Checkbox** | Checkable boxes with color schemes |
| **Radio** | Radio buttons for single-selection groups |
| **Switch** | Toggle switches with on/off state |
| **Slider** | Horizontal value sliders |
| **Select** | Dropdown select with trigger and options |

### Feedback & Overlay
| Component | Description |
|-----------|-------------|
| **Alert** | Notification boxes with subtle/solid/outline variants |
| **Progress** | Progress bars with color schemes |
| **Spinner** | Animated loading indicator |
| **Tooltip** | Text hints |
| **Modal** | Dialog overlays with backdrop |
| **Drawer** | Slide-in panels from any edge |
| **Popover** | Floating content anchored to parent |

### Navigation & Disclosure
| Component | Description |
|-----------|-------------|
| **Tabs** | Tab navigation (line, enclosed, soft variants) |
| **Link** | Text links with underline/hover-underline/none variants |
| **Breadcrumb** | Horizontal navigation path with separators |
| **Accordion** | Collapsible sections (bordered/separated variants) |
| **Menu** | Floating dropdown menu with items and separators |

## Quick Start

### C

```c
#define CLAY_IMPLEMENTATION
#include "clay.h"

#define CLAYKIT_IMPLEMENTATION
#include "clay_kit.h"

// Initialize
ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
ClayKit_State states[64] = {0};
ClayKit_Context ctx = {0};
ClayKit_Init(&ctx, &theme, states, 64);

// In your render loop
Clay_BeginLayout();

ClayKit_BadgeRaw(&ctx, "New", 3, (ClayKit_BadgeConfig){
    .color_scheme = CLAYKIT_COLOR_SUCCESS
});

if (ClayKit_Button(&ctx, "Click Me", 8, (ClayKit_ButtonConfig){0})) {
    // Button is hovered
}

ClayKit_Progress(&ctx, 0.75f, (ClayKit_ProgressConfig){0});

Clay_RenderCommandArray commands = Clay_EndLayout();
// Render commands with your backend...
```

### Zig

```zig
const claykit = @import("claykit");

var theme = claykit.Theme.light;
var states: [64]claykit.State = undefined;
var ctx: claykit.Context = .{};
claykit.init(&ctx, &theme, &states);

// In render loop
zclay.beginLayout();

claykit.badge(&ctx, "New", .{ .color_scheme = .success });

if (claykit.button(&ctx, "btn1", "Click Me", .{})) {
    if (raylib.isMouseButtonPressed(.left)) {
        // Clicked!
    }
}

claykit.progress(&ctx, "prog1", 0.75, .{});

const commands = zclay.endLayout();
```

## Building

### C Raylib Example

```bash
cd examples/c-raylib
make run
```

### Zig Raylib Example

```bash
cd examples/zig-raylib
zig build run
```

### Run Tests

```bash
cd tests
gcc -std=c99 -Wall -I.. -I../vendor -o test_clay_kit test_clay_kit.c
./test_clay_kit
```

## Documentation

- **[Quick Start Guide](docs/QUICKSTART.md)** - Get running in 5 minutes
- **[API Reference](docs/API.md)** - Complete function documentation
- **[Architecture](docs/ARCHITECTURE.md)** - Internal design and patterns

## Theming

```c
// Preset themes
ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
ClayKit_Theme theme = CLAYKIT_THEME_DARK;

// Or customize
ClayKit_Theme my_theme = {
    .primary = { 0, 122, 255, 255 },
    .success = { 52, 199, 89, 255 },
    // ... see docs for full structure
};
```

## Text Input

ClayKit provides complete text input handling:

```c
char buffer[256] = {0};
ClayKit_InputState input = {
    .buf = buffer,
    .cap = sizeof(buffer),
};

// Handle keyboard
if (input.flags & CLAYKIT_INPUT_FOCUSED) {
    if (IsKeyPressed(KEY_BACKSPACE))
        ClayKit_InputHandleKey(&input, CLAYKIT_KEY_BACKSPACE, 0);

    int ch = GetCharPressed();
    while (ch) {
        ClayKit_InputHandleChar(&input, ch);
        ch = GetCharPressed();
    }
}

// Render
bool hovered = ClayKit_TextInput(&ctx, "input1", 6, &input,
    (ClayKit_InputConfig){0}, "Placeholder...", 14);

// Focus on click
if (hovered && IsMouseClicked())
    input.flags |= CLAYKIT_INPUT_FOCUSED;
```

## Project Structure

```
clay-kit/
├── clay_kit.h          # C library (include this)
├── clay_kit.zig        # Zig bindings
├── vendor/
│   └── clay.h          # Clay UI library
├── examples/
│   ├── c-raylib/       # C + Raylib demo
│   └── zig-raylib/     # Zig + Raylib demo
├── tests/
│   └── test_clay_kit.c # Unit tests (153 tests)
└── docs/               # Documentation
```

## Requirements

- C99 compiler (gcc, clang, MSVC)
- Clay UI library (included in `vendor/`)
- For C example: git (to download raylib on first build)
- For Zig example: Zig 0.13+ (raylib downloaded via package manager)

## License

MIT

## Credits

- Built with [Claude](https://claude.ai) by Anthropic
- [Clay](https://github.com/nicbarker/clay) by Nic Barker - The underlying layout library
- [Raylib](https://github.com/raysan5/raylib) by Ramon Santamaria - Used in examples
