# ClayKit Architecture

This document explains the internal architecture and design decisions of ClayKit.

## Design Principles

### 1. Zero Allocation

ClayKit never allocates memory internally. All memory is provided by the user:

- **State Buffer**: A fixed-size array of `ClayKit_State` structs for component state
- **Theme**: User-owned struct, can be stack or static
- **Context**: User-owned struct containing pointers to the above

This makes ClayKit suitable for embedded systems, games, and other environments where dynamic allocation is undesirable.

### 2. Pure C Implementation

All rendering logic is implemented in C99 using Clay's low-level API:

```c
Clay__OpenElement();           // Begin element
Clay__ConfigureOpenElement(decl);  // Set properties
// ... child elements ...
Clay__CloseElement();          // End element
```

This ensures:
- Maximum portability (C99 is supported everywhere)
- No Zig-specific features required for core functionality
- Clean FFI boundary for bindings

### 3. Zig Bindings as Thin Wrappers

The Zig bindings (`clay_kit.zig`) are hand-written, not generated with `@cImport`. They:

- Call through to C functions via `extern fn` declarations
- Provide ergonomic Zig API (slices, optionals, enums)
- Add minimal logic (mostly just type conversions)

```zig
pub fn button(ctx: *Context, id: []const u8, text: []const u8, cfg: ButtonConfig) bool {
    return ClayKit_Button(ctx, text.ptr, @intCast(text.len), cfg);
}
```

## File Structure

```
clay-kit/
├── clay_kit.h          # Single-header C library
├── clay_kit.zig        # Hand-written Zig bindings
├── vendor/
│   ├── clay.h          # Clay UI library
│   └── raylib/         # Raylib for examples
├── examples/
│   ├── c-raylib/       # Pure C demo
│   └── zig-raylib/     # Zig demo
├── tests/
│   └── test_clay_kit.c # Unit tests
└── docs/
    ├── API.md          # API reference
    └── ARCHITECTURE.md # This file
```

## Component Architecture

### Rendering Pipeline

1. **Style Computation**: Each component has a `ClayKit_Compute*Style()` function that calculates visual properties based on config and theme:

```c
ClayKit_ButtonStyle style = ClayKit_ComputeButtonStyle(&ctx, cfg, hovered);
// style contains: bg_color, text_color, border_color, padding, etc.
```

2. **Element Creation**: Components use Clay's low-level API to create elements:

```c
Clay__OpenElement();
bool hovered = Clay_Hovered();  // Check hover before configuring

Clay_ElementDeclaration decl = {0};
decl.layout.sizing = ...;
decl.backgroundColor = style.bg_color;
// ... configure all properties

Clay__ConfigureOpenElement(decl);

// Add children (text, etc.)
Clay__OpenTextElement(text, Clay__StoreTextElementConfig(text_cfg));

Clay__CloseElement();
return hovered;
```

3. **Hover Detection**: `Clay_Hovered()` must be called immediately after `Clay__OpenElement()`, before `Clay__ConfigureOpenElement()`. This is a Clay requirement.

### Text Element Config Storage

**Critical Pattern**: When calling `Clay__OpenTextElement()`, the config must be stored via `Clay__StoreTextElementConfig()`:

```c
// WRONG - config is on stack, goes out of scope
Clay_TextElementConfig cfg = {...};
Clay__OpenTextElement(text, &cfg);

// CORRECT - config is stored in Clay's arena
Clay_TextElementConfig cfg = {...};
Clay__OpenTextElement(text, Clay__StoreTextElementConfig(cfg));
```

Clay stores render commands internally and needs stable pointers to configs.

## State Management

### Component State

Some components need persistent state (e.g., focused element ID). ClayKit uses a simple ID-based lookup:

```c
typedef struct {
    uint32_t id;      // Element ID (hash)
    uint32_t flags;   // Component-specific bits
    float value;      // For sliders, etc.
} ClayKit_State;

ClayKit_State* ClayKit_GetOrCreateState(ClayKit_Context *ctx, uint32_t id);
```

The state array is provided by the user with fixed capacity. Linear search is used (fast enough for typical UI sizes).

### Text Input State

Text input uses a separate state struct because it's more complex:

```c
typedef struct {
    char *buf;              // User's buffer
    uint32_t cap;           // Buffer capacity
    uint32_t len;           // Current length
    uint32_t cursor;        // Cursor position
    uint32_t select_start;  // Selection start
    uint8_t flags;          // FOCUSED, etc.
} ClayKit_InputState;
```

The user owns the text buffer and passes it to ClayKit. This allows:
- Custom buffer sizes
- Buffer reuse
- No hidden allocations

## Theming

### Theme Structure

Themes use a flat struct with all values pre-computed:

```c
typedef struct {
    Clay_Color primary, secondary, success, warning, error;
    Clay_Color bg, fg, border, muted;
    struct { uint16_t xs, sm, md, lg, xl; } spacing;
    struct { uint16_t sm, md, lg, full; } radius;
    struct { uint16_t body, heading; } font_id;
    struct { uint16_t xs, sm, md, lg, xl; } font_size;
} ClayKit_Theme;
```

No theme inheritance or runtime computation - everything is explicit.

### Color Schemes

Components accept a `ClayKit_ColorScheme` enum:

```c
Clay_Color ClayKit_GetSchemeColor(ClayKit_Theme *theme, ClayKit_ColorScheme scheme);
```

This maps to theme colors:
- `CLAYKIT_COLOR_PRIMARY` → `theme->primary`
- `CLAYKIT_COLOR_SUCCESS` → `theme->success`
- etc.

## Zig Integration

### Build System

The Zig example uses `build.zig` to:
1. Compile raylib from C source
2. Import clay_kit.zig as a module
3. Link everything together

```zig
const claykit_mod = b.addModule("claykit", .{
    .root_source_file = b.path("../../clay_kit.zig"),
});
exe.root_module.addImport("claykit", claykit_mod);
```

### Type Mappings

| C Type | Zig Type |
|--------|----------|
| `ClayKit_Context*` | `*Context` |
| `const char*` + `int32_t len` | `[]const u8` |
| `ClayKit_*Config` | `*Config` (same layout) |
| `bool` | `bool` |
| `float` | `f32` |
| `uint32_t` | `u32` |

### zclay Integration

The Zig bindings work alongside `zclay` (Zig bindings for Clay):

```zig
const zclay = @import("zclay");
const claykit = @import("claykit");

zclay.beginLayout();

// Use zclay for layout containers
zclay.UI()(.{ .layout = ... })({
    // Use claykit for components
    claykit.button(&ctx, "btn", "Click", .{});
});

const commands = zclay.endLayout();
```

## Testing

### Unit Tests

`tests/test_clay_kit.c` contains 82+ tests covering:

- Context initialization
- State management
- Focus management
- Theme helpers
- Input handling (keys, chars, selection)
- Layout primitives
- Component style computation

Tests don't require Clay initialization - they test ClayKit functions in isolation.

### Visual Testing

The example apps (`c-raylib`, `zig-raylib`) serve as visual tests. They exercise all components and should be run manually to verify appearance.

## Future Considerations

### Potential Additions

1. **Scroll containers** - Clay supports scrolling but ClayKit doesn't wrap it yet
2. **Select/Dropdown** - Floating menus are complex
3. **Animation** - Easing functions for transitions
4. **Icons** - Callback-based icon rendering

### Non-Goals

1. **Runtime theme switching** - Use multiple theme structs instead
2. **CSS-like styling** - Explicit struct configuration preferred
3. **Automatic state management** - User controls all state
