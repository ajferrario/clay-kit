# clay-kit

Zero-allocation UI components for Clay. Single-header C99, Zig-compatible.

## Build & Test
```bash
gcc -std=c99 -Wall -Wextra -pedantic -I. examples/basic.c -o basic
zig translate-c clay_kit.h  # verify clean zig output
```

## Architecture

### Memory Strategy
```c
typedef void (*ClayKit_IconCallback)(uint16_t icon_id, Clay_BoundingBox box, void *user_data);

typedef struct {
    void *theme_ptr;
    void *state_ptr;
    uint32_t state_count;
    uint32_t state_cap;
    uint32_t focused_id;
    uint32_t prev_focused_id;
    ClayKit_IconCallback icon_callback;
    void *icon_user_data;
} ClayKit_Context;

void ClayKit_Init(ClayKit_Context *ctx, ClayKit_Theme *theme, 
                  ClayKit_State *state_buf, uint32_t state_cap);
```

### Zig Compatibility Rules
To ensure `zig translate-c` works cleanly:
1. No bitfields
2. No flexible array members
3. No VLAs
4. No complex macros with statement expressions (use inline functions instead)
5. Use fixed-size integer types (`uint32_t`, `int16_t`, etc.)
6. Avoid `__attribute__` unless guarded
7. Keep struct layouts explicit (no anonymous unions/structs)

### Component State
Components needing state (checkbox, switch, accordion) use an ID-based lookup into user-provided state array:
```c
typedef struct {
    uint32_t id;
    uint32_t flags;  // component-specific bits
    float value;     // for sliders, progress, etc.
} ClayKit_State;

ClayKit_State* ClayKit_GetState(ClayKit_Context *ctx, uint32_t id);
```

## Component Categories & Priority

### Phase 1: Layout Primitives
These map directly to Clay's layout system:
| Component | ChakraUI | Implementation |
|-----------|----------|----------------|
| Box | Box | Thin wrapper, adds theme colors |
| Flex | Flex | Sets `layoutDirection`, `childGap` |
| Stack | Stack/VStack/HStack | Flex with direction preset |
| Center | Center | Flex with centered alignment |
| Container | Container | Box with max-width from theme |
| Grid | SimpleGrid | Uses Clay's sizing + loops |
| Spacer | Spacer | `CLAY_SIZING_GROW` element |

### Phase 2: Typography & Data Display
| Component | ChakraUI | Implementation |
|-----------|----------|----------------|
| Text | Text | CLAY_TEXT with theme fonts |
| Heading | Heading | Text with preset sizes |
| Badge | Badge | Rounded rect + text |
| Tag | Tag | Badge variant |
| Stat | Stat | Composed box with label/value |
| List | List | Vertical stack with bullets |
| Table | Table | Grid-based layout |

### Phase 3: Form Controls
| Component | ChakraUI | Notes |
|-----------|----------|-------|
| Button | Button | Rect + text + hover state |
| Input | Input | Text box with cursor/selection, user owns buffer |
| Textarea | Textarea | Multi-line input, same state model |
| Checkbox | Checkbox | Requires state |
| Switch | Switch | Requires state + animation value |
| Slider | Slider | Requires state |
| Select | Select | Floating + scroll container |
| Radio | Radio | Group state management |

### Phase 4: Feedback & Overlay
| Component | ChakraUI | Notes |
|-----------|----------|-------|
| Alert | Alert | Box with icon slot |
| Progress | Progress | Two rects, value from state |
| Spinner | Spinner | Animated, needs frame delta |
| Tooltip | Tooltip | Floating element |
| Modal | Modal | Floating + backdrop |
| Drawer | Drawer | Floating + slide animation |
| Popover | Popover | Floating anchored to parent |

### Phase 5: Navigation & Disclosure
| Component | ChakraUI | Notes |
|-----------|----------|-------|
| Tabs | Tabs | State for active tab |
| Accordion | Accordion | State for open/closed |
| Breadcrumb | Breadcrumb | Horizontal list with separators |
| Menu | Menu | Floating list |
| Link | Link | Text with underline |

## Theming System
```c
typedef struct {
    Clay_Color primary, secondary, success, warning, error;
    Clay_Color bg, fg, border, muted;
    struct { uint16_t xs, sm, md, lg, xl; } spacing;
    struct { uint16_t sm, md, lg, full; } radius;
    struct { uint16_t body, heading; } font_id;
    struct { uint16_t xs, sm, md, lg, xl; } font_size;
} ClayKit_Theme;

// Preset themes
extern const ClayKit_Theme CLAYKIT_THEME_LIGHT;
extern const ClayKit_Theme CLAYKIT_THEME_DARK;
```

## API Design

### Macro-based (matches Clay style)
```c
// Usage mirrors Clay's pattern
CLAYKIT_BUTTON(ctx, CLAY_ID("submit"), {
    .variant = CLAYKIT_BUTTON_SOLID,
    .color_scheme = CLAYKIT_COLOR_PRIMARY,
    .size = CLAYKIT_SIZE_MD,
}) {
    CLAY_TEXT(CLAY_STRING("Submit"), &ctx->theme->text_config);
}

// Expands to Clay primitives internally
```

### Function-based (for simpler components)
```c
void ClayKit_Badge(ClayKit_Context *ctx, Clay_String text, ClayKit_BadgeConfig cfg);
void ClayKit_Progress(ClayKit_Context *ctx, float value, ClayKit_ProgressConfig cfg);
```

## File Structure
```
clay_kit.h      # Single header, define CLAYKIT_IMPLEMENTATION once
├── Types & Config structs
├── Theme definitions
├── Context/State management
├── Layout components (Box, Flex, Stack, etc.)
├── Typography (Text, Heading, Badge)
├── Form controls (Button, Input, Checkbox, etc.)
├── Feedback (Alert, Progress, Tooltip)
└── Navigation (Tabs, Accordion, Menu)
```

## Zig Integration
```zig
const clay = @cImport({
    @cDefine("CLAY_IMPLEMENTATION", {});
    @cInclude("clay.h");
});

const claycmp = @cImport({
    @cDefine("CLAYKIT_IMPLEMENTATION", {});
    @cInclude("clay_kit.h");
});

// Usage
var theme = claycmp.CLAYKIT_THEME_LIGHT;
var state_buf: [256]claycmp.ClayKit_State = undefined;
var ctx: claycmp.ClayKit_Context = undefined;
claycmp.ClayKit_Init(&ctx, &theme, &state_buf, 256);
```

## Implementation Order
1. Core types, theme, context init
2. Box, Flex, Stack, Center, Spacer
3. Text, Heading, Badge
4. Button (with hover detection via `Clay_Hovered()`)
5. Input (display only, user handles text)
6. Checkbox, Switch (with state)
7. Progress, Slider
8. Alert, Tooltip
9. Tabs, Accordion
10. Modal, Drawer, Popover
11. Table, Menu, Select

## Testing Strategy
- Compile with `gcc -std=c99 -Wall -Wextra -pedantic`
- Compile with `zig cc -std=c99`
- Run `zig translate-c` and verify clean output
- Visual tests with raylib renderer

## Decisions
1. **Animation**: Include easing functions (impl later)
2. **Icons**: Hybrid - user-defined IDs, callback-based rendering
3. **Responsive**: Skip for now
4. **Focus**: Yes, track focused element ID in context
5. **Text input**: Minimal state - we track cursor/selection, user owns text buffer

## Icon System
```c
// User-defined icon IDs, we just pass them through
typedef struct {
    uint16_t id;    // 0 = no icon
    uint16_t size;  // pixels
} ClayKit_Icon;

// User provides callback at init
typedef void (*ClayKit_IconCallback)(uint16_t icon_id, Clay_BoundingBox box, void *user_data);

// In context
ClayKit_IconCallback icon_callback;
void *icon_user_data;

// Usage
CLAYKIT_BUTTON(ctx, CLAY_ID("save"), {
    .icon_left = { .id = MY_ICON_SAVE, .size = 16 },
}) { ... }
```

## Text Input System
```c
// User provides and owns the text buffer
typedef struct {
    char *buf;
    uint32_t cap;
    uint32_t len;
    uint32_t cursor;
    uint32_t select_start;  // == cursor when no selection
    uint8_t flags;          // CLAYKIT_INPUT_FOCUSED, CLAYKIT_INPUT_PASSWORD, etc.
} ClayKit_InputState;

// Keyboard handling - returns true if text/cursor changed
bool ClayKit_InputHandleKey(ClayKit_InputState *s, uint32_t key, uint32_t mods);
bool ClayKit_InputHandleChar(ClayKit_InputState *s, uint32_t codepoint);

// Rendering - draws box, text, cursor, selection
void ClayKit_Input(ClayKit_Context *ctx, ClayKit_InputState *s, ClayKit_InputConfig cfg);

// Common keys (user maps platform keys to these)
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

enum {
    CLAYKIT_MOD_SHIFT = 1 << 0,
    CLAYKIT_MOD_CTRL  = 1 << 1,
    CLAYKIT_MOD_ALT   = 1 << 2,
};
```

## Focus Management
```c
// In ClayKit_Context
uint32_t focused_id;      // CLAY_ID hash of focused element
uint32_t prev_focused_id; // for detecting focus changes

// Helpers
void ClayKit_SetFocus(ClayKit_Context *ctx, Clay_ElementId id);
void ClayKit_ClearFocus(ClayKit_Context *ctx);
bool ClayKit_HasFocus(ClayKit_Context *ctx, Clay_ElementId id);
bool ClayKit_FocusChanged(ClayKit_Context *ctx); // true if focus changed this frame

// Tab navigation (call with TAB key)
void ClayKit_FocusNext(ClayKit_Context *ctx);
void ClayKit_FocusPrev(ClayKit_Context *ctx);
```

## Next Steps
1. Review and finalize component list
2. Define exact struct layouts for Phase 1
3. Implement Box, Flex, Stack
4. Test Zig translation
5. Iterate
