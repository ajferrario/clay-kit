# ClayKit API Reference

ClayKit is a zero-allocation UI component library for [Clay](https://github.com/nicbarker/clay). It provides ready-to-use components with theming support, written in C99 with hand-written Zig bindings.

## Table of Contents

- [Getting Started](#getting-started)
- [Core Concepts](#core-concepts)
- [Context & Initialization](#context--initialization)
- [Theming](#theming)
- [Layout Primitives](#layout-primitives)
- [Components](#components)
  - [Badge](#badge)
  - [Button](#button)
  - [Progress](#progress)
  - [Slider](#slider)
  - [Alert](#alert)
  - [Tooltip](#tooltip)
  - [Tabs](#tabs)
  - [Modal](#modal)
  - [Checkbox](#checkbox)
  - [Switch](#switch)
  - [Text Input](#text-input)
- [Text Input Handling](#text-input-handling)
- [Focus Management](#focus-management)
- [Zig Bindings](#zig-bindings)

---

## Getting Started

### C Usage

```c
#define CLAY_IMPLEMENTATION
#include "clay.h"

#define CLAYKIT_IMPLEMENTATION
#include "clay_kit.h"

int main(void) {
    // Initialize Clay (see Clay documentation)
    // ...

    // Initialize ClayKit
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[64] = {0};
    ClayKit_Context ctx = {0};
    ClayKit_Init(&ctx, &theme, state_buf, 64);

    // In your render loop:
    Clay_BeginLayout();

    // Use ClayKit components
    ClayKit_Button(&ctx, "Click Me", 8, (ClayKit_ButtonConfig){0});

    Clay_RenderCommandArray commands = Clay_EndLayout();
    // Render commands...
}
```

### Zig Usage

```zig
const claykit = @import("claykit");
const zclay = @import("zclay");

var theme = claykit.Theme.light;
var state_buf: [64]claykit.State = undefined;
var ctx: claykit.Context = .{};
claykit.init(&ctx, &theme, &state_buf);

// In your render loop:
zclay.beginLayout();

if (claykit.button(&ctx, "Btn1", "Click Me", .{})) {
    // Button is hovered
}

const commands = zclay.endLayout();
```

---

## Core Concepts

### Zero Allocation
ClayKit never allocates memory internally. You provide:
- A state buffer for components that need persistent state
- A theme struct (can be stack or static)
- The context struct itself

### Immediate Mode
Like Clay itself, ClayKit uses immediate mode rendering. You call component functions every frame during layout, and they generate Clay render commands.

### Return Values
Most interactive components return `bool` indicating whether they're currently hovered. Use this with your input system to detect clicks:

```c
if (ClayKit_Button(&ctx, "Save", 4, config) && IsMouseButtonPressed()) {
    // Button was clicked
}
```

---

## Context & Initialization

### ClayKit_Context

The context holds references to theme, state, and frame timing.

```c
typedef struct {
    void *theme_ptr;              // Pointer to ClayKit_Theme
    void *state_ptr;              // Pointer to state array
    uint32_t state_count;         // Current number of states
    uint32_t state_cap;           // Capacity of state array
    uint32_t focused_id;          // Currently focused element ID
    uint32_t prev_focused_id;     // Previous frame's focused ID
    float cursor_blink_time;      // Timer for cursor blinking
    ClayKit_TextMeasureCallback measure_text;  // Text measurement function
    void *measure_text_user_data; // User data for text measurement
} ClayKit_Context;
```

### ClayKit_Init

Initialize the context with theme and state buffer.

```c
void ClayKit_Init(
    ClayKit_Context *ctx,      // Context to initialize
    ClayKit_Theme *theme,      // Theme configuration
    ClayKit_State *state_buf,  // Array for component state
    uint32_t state_cap         // Capacity of state array
);
```

**Example:**
```c
ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
ClayKit_State states[64] = {0};
ClayKit_Context ctx = {0};
ClayKit_Init(&ctx, &theme, states, 64);
```

### ClayKit_BeginFrame

Call at the start of each frame to reset per-frame state.

```c
void ClayKit_BeginFrame(ClayKit_Context *ctx);
```

---

## Theming

### ClayKit_Theme

Complete theme configuration including colors, spacing, and typography.

```c
typedef struct {
    // Semantic colors
    Clay_Color primary;    // Primary brand color
    Clay_Color secondary;  // Secondary/subtle backgrounds
    Clay_Color success;    // Success state color
    Clay_Color warning;    // Warning state color
    Clay_Color error;      // Error state color

    // UI colors
    Clay_Color bg;         // Background color
    Clay_Color fg;         // Foreground/text color
    Clay_Color border;     // Border color
    Clay_Color muted;      // Muted/disabled text

    // Spacing scale (in pixels)
    struct { uint16_t xs, sm, md, lg, xl; } spacing;

    // Border radius scale
    struct { uint16_t sm, md, lg, full; } radius;

    // Font IDs (passed to your renderer)
    struct { uint16_t body, heading; } font_id;

    // Font sizes
    struct { uint16_t xs, sm, md, lg, xl; } font_size;
} ClayKit_Theme;
```

### Preset Themes

```c
extern const ClayKit_Theme CLAYKIT_THEME_LIGHT;  // Light mode theme
extern const ClayKit_Theme CLAYKIT_THEME_DARK;   // Dark mode theme
```

### Color Schemes

Components support color schemes for semantic coloring:

```c
typedef enum {
    CLAYKIT_COLOR_PRIMARY = 0,  // Primary brand color
    CLAYKIT_COLOR_SECONDARY,    // Secondary color
    CLAYKIT_COLOR_SUCCESS,      // Success/positive
    CLAYKIT_COLOR_WARNING,      // Warning/caution
    CLAYKIT_COLOR_ERROR         // Error/danger
} ClayKit_ColorScheme;
```

### Size Scale

Components support a size scale:

```c
typedef enum {
    CLAYKIT_SIZE_XS = 0,  // Extra small
    CLAYKIT_SIZE_SM,      // Small
    CLAYKIT_SIZE_MD,      // Medium (default)
    CLAYKIT_SIZE_LG,      // Large
    CLAYKIT_SIZE_XL       // Extra large
} ClayKit_Size;
```

---

## Layout Primitives

ClayKit provides helper functions for common Clay layouts.

### ClayKit_BoxLayout

Creates a padded container with optional border.

```c
typedef struct {
    Clay_Color bg;           // Background color
    Clay_Color border_color; // Border color
    uint16_t border_width;   // Border width in pixels
    uint16_t padding;        // Padding on all sides
    uint16_t radius;         // Corner radius
} ClayKit_BoxConfig;

Clay_LayoutConfig ClayKit_BoxLayout(ClayKit_BoxConfig cfg);
```

### ClayKit_FlexLayout

Creates a flexbox-style layout.

```c
typedef struct {
    Clay_LayoutDirection direction;  // CLAY_LEFT_TO_RIGHT or CLAY_TOP_TO_BOTTOM
    uint16_t gap;                    // Gap between children
    uint16_t padding;                // Padding on all sides
    Clay_ChildAlignment align;       // Child alignment
} ClayKit_FlexConfig;

Clay_LayoutConfig ClayKit_FlexLayout(ClayKit_FlexConfig cfg);
```

### ClayKit_StackLayout

Creates a vertical or horizontal stack.

```c
typedef enum {
    CLAYKIT_STACK_VERTICAL = 0,
    CLAYKIT_STACK_HORIZONTAL
} ClayKit_StackDirection;

typedef struct {
    ClayKit_StackDirection direction;
    uint16_t gap;
} ClayKit_StackConfig;

Clay_LayoutConfig ClayKit_StackLayout(ClayKit_StackConfig cfg);
```

### ClayKit_CenterLayout

Creates a layout that centers its children.

```c
Clay_LayoutConfig ClayKit_CenterLayout(Clay_Sizing sizing);
```

### ClayKit_ContainerLayout

Creates a max-width constrained, centered container.

```c
typedef struct {
    uint16_t max_width;  // Maximum width (0 = theme default 1200px)
    uint16_t padding;    // Horizontal padding
} ClayKit_ContainerConfig;

Clay_LayoutConfig ClayKit_ContainerLayout(ClayKit_ContainerConfig cfg);
```

### ClayKit_SpacerLayout

Creates a flexible spacer that grows to fill available space.

```c
Clay_LayoutConfig ClayKit_SpacerLayout(void);
```

---

## Components

### Badge

A small label for status indicators or counts.

```c
typedef struct {
    ClayKit_BadgeVariant variant;    // solid, subtle, or outline
    ClayKit_ColorScheme color_scheme;
    ClayKit_Size size;
} ClayKit_BadgeConfig;

typedef enum {
    CLAYKIT_BADGE_SOLID = 0,   // Filled background
    CLAYKIT_BADGE_SUBTLE,      // Light background tint
    CLAYKIT_BADGE_OUTLINE      // Border only
} ClayKit_BadgeVariant;

// Render a badge with raw string
void ClayKit_BadgeRaw(
    ClayKit_Context *ctx,
    const char *text,
    int32_t text_len,
    ClayKit_BadgeConfig cfg
);

// Render a badge with Clay_String
void ClayKit_Badge(
    ClayKit_Context *ctx,
    Clay_String text,
    ClayKit_BadgeConfig cfg
);
```

**Example:**
```c
ClayKit_BadgeRaw(&ctx, "New", 3, (ClayKit_BadgeConfig){
    .variant = CLAYKIT_BADGE_SOLID,
    .color_scheme = CLAYKIT_COLOR_SUCCESS,
    .size = CLAYKIT_SIZE_SM
});
```

---

### Button

An interactive button with hover states.

```c
typedef struct {
    ClayKit_ButtonVariant variant;   // solid, outline, or ghost
    ClayKit_ColorScheme color_scheme;
    ClayKit_Size size;
    bool disabled;                    // Disable interaction
} ClayKit_ButtonConfig;

typedef enum {
    CLAYKIT_BUTTON_SOLID = 0,  // Filled background
    CLAYKIT_BUTTON_OUTLINE,    // Border only
    CLAYKIT_BUTTON_GHOST       // No background or border
} ClayKit_ButtonVariant;

// Returns true if hovered
bool ClayKit_Button(
    ClayKit_Context *ctx,
    const char *text,
    int32_t text_len,
    ClayKit_ButtonConfig cfg
);
```

**Example:**
```c
if (ClayKit_Button(&ctx, "Save", 4, (ClayKit_ButtonConfig){
    .variant = CLAYKIT_BUTTON_SOLID,
    .color_scheme = CLAYKIT_COLOR_PRIMARY
})) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        // Handle click
    }
}
```

---

### Progress

A progress bar showing completion percentage.

```c
typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_Size size;               // Affects height
    uint16_t width;                  // Fixed width (0 = grow)
} ClayKit_ProgressConfig;

void ClayKit_Progress(
    ClayKit_Context *ctx,
    float value,                     // 0.0 to 1.0
    ClayKit_ProgressConfig cfg
);
```

**Example:**
```c
ClayKit_Progress(&ctx, 0.75f, (ClayKit_ProgressConfig){
    .color_scheme = CLAYKIT_COLOR_SUCCESS
});
```

---

### Slider

A horizontal slider for value selection.

```c
typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_Size size;
    uint16_t width;                  // Fixed width (0 = grow)
    bool disabled;
} ClayKit_SliderConfig;

// Returns true if hovered
bool ClayKit_Slider(
    ClayKit_Context *ctx,
    float value,                     // 0.0 to 1.0
    ClayKit_SliderConfig cfg
);
```

**Note:** ClayKit renders the slider but doesn't handle drag interaction. You need to implement value updates based on mouse position when dragging.

---

### Alert

A message box for notifications or warnings.

```c
typedef struct {
    ClayKit_AlertVariant variant;    // subtle, solid, or outline
    ClayKit_ColorScheme color_scheme;
} ClayKit_AlertConfig;

typedef enum {
    CLAYKIT_ALERT_SUBTLE = 0,  // Light background tint
    CLAYKIT_ALERT_SOLID,       // Filled background
    CLAYKIT_ALERT_OUTLINE      // Border only
} ClayKit_AlertVariant;

// Render alert with text content
void ClayKit_AlertText(
    ClayKit_Context *ctx,
    const char *text,
    int32_t text_len,
    ClayKit_AlertConfig cfg
);
```

**Example:**
```c
ClayKit_AlertText(&ctx, "File saved successfully!", 23, (ClayKit_AlertConfig){
    .variant = CLAYKIT_ALERT_SUBTLE,
    .color_scheme = CLAYKIT_COLOR_SUCCESS
});
```

---

### Tooltip

A small popup text hint.

```c
typedef struct {
    ClayKit_TooltipPosition position;  // Currently unused, for future
} ClayKit_TooltipConfig;

void ClayKit_Tooltip(
    ClayKit_Context *ctx,
    const char *text,
    int32_t text_len,
    ClayKit_TooltipConfig cfg
);
```

**Note:** This renders a static tooltip element. For hover-triggered tooltips, you need to conditionally render based on another element's hover state.

---

### Tabs

Tab navigation with line or enclosed variants.

```c
typedef struct {
    ClayKit_TabsVariant variant;     // line or enclosed
    ClayKit_ColorScheme color_scheme;
    ClayKit_Size size;
} ClayKit_TabsConfig;

typedef enum {
    CLAYKIT_TABS_LINE = 0,     // Underline indicator
    CLAYKIT_TABS_ENCLOSED      // Box-style tabs
} ClayKit_TabsVariant;

// Render a single tab
// Returns true if hovered
bool ClayKit_Tab(
    ClayKit_Context *ctx,
    const char *label,
    int32_t label_len,
    bool is_active,              // Whether this tab is selected
    ClayKit_TabsConfig cfg
);
```

**Example:**
```c
static int active_tab = 0;

// Wrap tabs in a horizontal container
open_container(sizing_grow(), sizing_fit(), ...);

if (ClayKit_Tab(&ctx, "Tab 1", 5, active_tab == 0, cfg)) {
    if (IsMouseButtonPressed()) active_tab = 0;
}
if (ClayKit_Tab(&ctx, "Tab 2", 5, active_tab == 1, cfg)) {
    if (IsMouseButtonPressed()) active_tab = 1;
}
if (ClayKit_Tab(&ctx, "Tab 3", 5, active_tab == 2, cfg)) {
    if (IsMouseButtonPressed()) active_tab = 2;
}

Clay__CloseElement();
```

---

### Modal

A dialog overlay with backdrop.

For modals, you typically use Clay's floating element system directly. ClayKit provides style computation helpers:

```c
typedef struct {
    ClayKit_Size size;           // Affects width
    uint16_t custom_width;       // Override width (0 = use size)
    uint16_t z_index;            // Stack order (0 = default 1000)
} ClayKit_ModalConfig;

typedef struct {
    uint16_t width;
    uint16_t padding;
    uint16_t gap;
    uint16_t corner_radius;
    uint16_t z_index;
    Clay_Color bg_color;
    Clay_Color backdrop_color;
} ClayKit_ModalStyle;

ClayKit_ModalStyle ClayKit_ComputeModalStyle(
    ClayKit_Context *ctx,
    ClayKit_ModalConfig cfg
);
```

**Example:** See `examples/c-raylib/main.c` for full modal implementation using floating elements.

---

### Checkbox

A checkable box control.

```c
typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_Size size;
    bool disabled;
} ClayKit_CheckboxConfig;

// Returns true if hovered
bool ClayKit_Checkbox(
    ClayKit_Context *ctx,
    bool checked,                // Current checked state
    ClayKit_CheckboxConfig cfg
);
```

**Example:**
```c
static bool is_checked = false;

if (ClayKit_Checkbox(&ctx, is_checked, (ClayKit_CheckboxConfig){0})) {
    if (IsMouseButtonPressed()) {
        is_checked = !is_checked;
    }
}
```

---

### Switch

A toggle switch control.

```c
typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_Size size;
    bool disabled;
} ClayKit_SwitchConfig;

// Returns true if hovered
bool ClayKit_Switch(
    ClayKit_Context *ctx,
    bool on,                     // Current on/off state
    ClayKit_SwitchConfig cfg
);
```

---

### Text Input

A text input field with cursor and placeholder support.

```c
typedef struct {
    ClayKit_Size size;
    uint16_t width;              // Fixed width (0 = grow)
    Clay_Color bg;               // Override background
    Clay_Color text_color;       // Override text color
} ClayKit_InputConfig;

// Returns true if hovered (use for click-to-focus)
bool ClayKit_TextInput(
    ClayKit_Context *ctx,
    const char *id,              // Element ID for lookup
    int32_t id_len,
    ClayKit_InputState *state,   // Input state (you manage this)
    ClayKit_InputConfig cfg,
    const char *placeholder,
    int32_t placeholder_len
);
```

See [Text Input Handling](#text-input-handling) for complete usage.

---

## Text Input Handling

ClayKit provides a complete text input system where you own the text buffer and ClayKit handles rendering.

### ClayKit_InputState

State structure for text input.

```c
typedef struct {
    char *buf;              // Your text buffer (you allocate)
    uint32_t cap;           // Buffer capacity
    uint32_t len;           // Current text length
    uint32_t cursor;        // Cursor position (0 = before first char)
    uint32_t select_start;  // Selection start (== cursor when no selection)
    uint8_t flags;          // CLAYKIT_INPUT_FOCUSED, etc.
} ClayKit_InputState;
```

### Input Flags

```c
#define CLAYKIT_INPUT_FOCUSED   (1 << 0)  // Input has focus
#define CLAYKIT_INPUT_PASSWORD  (1 << 1)  // Mask characters (future)
#define CLAYKIT_INPUT_READONLY  (1 << 2)  // Prevent editing (future)
```

### Keyboard Handling

```c
// Key constants (map from your platform)
enum {
    CLAYKIT_KEY_BACKSPACE = 1,
    CLAYKIT_KEY_DELETE,
    CLAYKIT_KEY_LEFT,
    CLAYKIT_KEY_RIGHT,
    CLAYKIT_KEY_HOME,
    CLAYKIT_KEY_END,
    CLAYKIT_KEY_ENTER,
    CLAYKIT_KEY_TAB,
};

// Modifier flags
enum {
    CLAYKIT_MOD_SHIFT = 1 << 0,
    CLAYKIT_MOD_CTRL  = 1 << 1,
    CLAYKIT_MOD_ALT   = 1 << 2,
};

// Handle special keys (arrows, backspace, etc.)
// Returns true if state changed
bool ClayKit_InputHandleKey(
    ClayKit_InputState *state,
    uint32_t key,            // CLAYKIT_KEY_* constant
    uint32_t mods            // CLAYKIT_MOD_* flags
);

// Handle character input
// Returns true if character was inserted
bool ClayKit_InputHandleChar(
    ClayKit_InputState *state,
    uint32_t codepoint       // Unicode codepoint
);
```

### Click-to-Position Cursor

To position the cursor when the user clicks in the text:

```c
// Get cursor position from x coordinate
uint32_t ClayKit_InputGetCursorFromX(
    ClayKit_Context *ctx,
    const char *text,
    uint32_t length,
    uint16_t font_id,
    uint16_t font_size,
    float x_offset           // X offset from text start
);
```

### Complete Example

```c
// Setup
char input_buffer[256] = {0};
ClayKit_InputState input_state = {
    .buf = input_buffer,
    .cap = sizeof(input_buffer),
    .len = 0,
    .cursor = 0,
    .select_start = 0,
    .flags = 0
};

// In your update loop:

// 1. Handle keyboard when focused
if (input_state.flags & CLAYKIT_INPUT_FOCUSED) {
    if (IsKeyPressed(KEY_BACKSPACE)) {
        ClayKit_InputHandleKey(&input_state, CLAYKIT_KEY_BACKSPACE, get_mods());
    }
    if (IsKeyPressed(KEY_LEFT)) {
        ClayKit_InputHandleKey(&input_state, CLAYKIT_KEY_LEFT, get_mods());
    }
    // ... other keys

    // Character input
    int ch = GetCharPressed();
    while (ch != 0) {
        ClayKit_InputHandleChar(&input_state, (uint32_t)ch);
        ch = GetCharPressed();
    }
}

// 2. Render and track hover
bool input_hovered = ClayKit_TextInput(&ctx, "MyInput", 7,
    &input_state, (ClayKit_InputConfig){0}, "Placeholder...", 14);

// 3. Handle click for focus
if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (input_hovered) {
        input_state.flags |= CLAYKIT_INPUT_FOCUSED;
        // Position cursor at click location (see examples)
    } else {
        input_state.flags &= ~CLAYKIT_INPUT_FOCUSED;
    }
}
```

---

## Focus Management

ClayKit tracks which element has focus for keyboard navigation.

```c
// Set focus to an element
void ClayKit_SetFocus(ClayKit_Context *ctx, Clay_ElementId id);

// Clear focus
void ClayKit_ClearFocus(ClayKit_Context *ctx);

// Check if element has focus
bool ClayKit_HasFocus(ClayKit_Context *ctx, Clay_ElementId id);

// Check if focus changed this frame
bool ClayKit_FocusChanged(ClayKit_Context *ctx);
```

---

## Zig Bindings

ClayKit includes hand-written Zig bindings that provide a more ergonomic API.

### Initialization

```zig
const claykit = @import("claykit");

var theme = claykit.Theme.light;  // or .dark
var state_buf: [64]claykit.State = undefined;
var ctx: claykit.Context = .{};
claykit.init(&ctx, &theme, &state_buf);
```

### Components

```zig
// Badge
claykit.badge(&ctx, "New", .{ .color_scheme = .success });

// Button (returns hover state)
if (claykit.button(&ctx, "btn1", "Click", .{})) {
    if (raylib.isMouseButtonPressed(.left)) {
        // Clicked
    }
}

// Progress
claykit.progress(&ctx, "prog1", 0.5, .{});

// Slider
_ = claykit.slider(&ctx, "slider1", value, .{});

// Alert
claykit.alertText(&ctx, "alert1", "Message", .{ .color_scheme = .warning });

// Tabs
const labels = [_][]const u8{ "Tab 1", "Tab 2", "Tab 3" };
if (claykit.tabs(&ctx, "tabs1", &labels, active_tab, .{})) |hovered_idx| {
    if (raylib.isMouseButtonPressed(.left)) {
        active_tab = hovered_idx;
    }
}

// Text Input
const hovered = claykit.textInput(&ctx, "input1", &input_state, .{}, "Placeholder");
```

### Text Input in Zig

```zig
var input_buffer: [256]u8 = undefined;
var input_state = claykit.InputState.init(&input_buffer);

// Handle keys
if (input_state.isFocused()) {
    if (raylib.isKeyPressed(.backspace)) {
        _ = claykit.inputHandleKey(&input_state, .backspace, getMods());
    }
    // ... etc
}

// Handle click positioning
claykit.inputHandleClick(&ctx, &input_state, bounds, click_x, style);
```

---

## Building

### C Example

```bash
cd examples/c-raylib
make run
```

### Zig Example

```bash
cd examples/zig-raylib
zig build run
```

### Running Tests

```bash
cd tests
gcc -std=c99 -Wall -I.. -I../vendor -o test_clay_kit test_clay_kit.c
./test_clay_kit
```
