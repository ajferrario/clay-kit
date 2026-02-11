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
  - [Tag](#tag)
  - [Stat](#stat)
  - [List](#list)
  - [Table](#table)
  - [Button](#button)
  - [Checkbox](#checkbox)
  - [Radio](#radio)
  - [Switch](#switch)
  - [Progress](#progress)
  - [Slider](#slider)
  - [Select](#select)
  - [Alert](#alert)
  - [Tooltip](#tooltip)
  - [Spinner](#spinner)
  - [Tabs](#tabs)
  - [Link](#link)
  - [Breadcrumb](#breadcrumb)
  - [Accordion](#accordion)
  - [Menu](#menu)
  - [Modal](#modal)
  - [Drawer](#drawer)
  - [Popover](#popover)
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

### Tag

A closeable label, similar to Badge but with an optional close indicator.

```c
typedef enum {
    CLAYKIT_TAG_SOLID = 0,    // Solid background
    CLAYKIT_TAG_SUBTLE,       // Light background, colored text
    CLAYKIT_TAG_OUTLINE       // Transparent with border
} ClayKit_TagVariant;

typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_TagVariant variant;
    ClayKit_Size size;
    bool closeable;              // Show close "x" indicator
} ClayKit_TagConfig;

void ClayKit_TagRaw(
    ClayKit_Context *ctx,
    const char *text,
    int32_t text_len,
    ClayKit_TagConfig cfg
);

void ClayKit_Tag(
    ClayKit_Context *ctx,
    Clay_String text,
    ClayKit_TagConfig cfg
);
```

**Example:**
```c
ClayKit_TagRaw(&ctx, "JavaScript", 10, (ClayKit_TagConfig){
    .color_scheme = CLAYKIT_COLOR_WARNING,
    .closeable = true
});
```

---

### Stat

A statistics display with label, large value, and optional help text.

```c
typedef struct {
    ClayKit_Size size;             // Affects value font size
    Clay_Color label_color;        // Label text color (default: theme muted)
    Clay_Color value_color;        // Value text color (default: theme fg)
    Clay_Color help_color;         // Help text color (default: theme muted)
} ClayKit_StatConfig;

void ClayKit_Stat(
    ClayKit_Context *ctx,
    const char *label, int32_t label_len,
    const char *value, int32_t value_len,
    const char *help_text, int32_t help_len,  // NULL for no help text
    ClayKit_StatConfig cfg
);
```

**Example:**
```c
ClayKit_Stat(&ctx,
    "Total Users", 11,
    "1,024", 5,
    "+12% from last month", 20,
    (ClayKit_StatConfig){0}
);
```

---

### List

An ordered (numbered) or unordered (bullet) list.

```c
typedef struct {
    bool ordered;              // true = numbered, false = bullet
    ClayKit_Size size;
    Clay_Color marker_color;   // Marker color (default: theme muted)
    Clay_Color text_color;     // Item text color (default: theme fg)
} ClayKit_ListConfig;

void ClayKit_ListBegin(ClayKit_Context *ctx, ClayKit_ListConfig cfg);
void ClayKit_ListItemRaw(
    ClayKit_Context *ctx,
    const char *text, int32_t text_len,
    uint32_t index,            // Item index (used for numbering)
    ClayKit_ListConfig cfg
);
void ClayKit_ListEnd(void);
```

**Example:**
```c
// Unordered list
ClayKit_ListBegin(&ctx, (ClayKit_ListConfig){ .ordered = false });
ClayKit_ListItemRaw(&ctx, "First item", 10, 0, cfg);
ClayKit_ListItemRaw(&ctx, "Second item", 11, 1, cfg);
ClayKit_ListItemRaw(&ctx, "Third item", 10, 2, cfg);
ClayKit_ListEnd();

// Ordered list
ClayKit_ListBegin(&ctx, (ClayKit_ListConfig){ .ordered = true });
ClayKit_ListItemRaw(&ctx, "Step one", 8, 0, cfg);
ClayKit_ListItemRaw(&ctx, "Step two", 8, 1, cfg);
ClayKit_ListEnd();
```

---

### Table

A data table with header row, optional striping, and borders.

```c
typedef struct {
    ClayKit_ColorScheme color_scheme;  // Header color
    ClayKit_Size size;                 // Padding and font size
    bool striped;                      // Alternate row backgrounds
    bool bordered;                     // Borders between cells
} ClayKit_TableConfig;

void ClayKit_TableBegin(ClayKit_Context *ctx, ClayKit_TableConfig cfg);
void ClayKit_TableHeaderRow(ClayKit_Context *ctx, ClayKit_TableConfig cfg);
void ClayKit_TableHeaderCell(ClayKit_Context *ctx, float width_percent, ClayKit_TableConfig cfg);
void ClayKit_TableCellEnd(void);
void ClayKit_TableRowEnd(void);
void ClayKit_TableRow(ClayKit_Context *ctx, uint32_t row_index, ClayKit_TableConfig cfg);
void ClayKit_TableCell(ClayKit_Context *ctx, float width_percent, uint32_t row_index, ClayKit_TableConfig cfg);
void ClayKit_TableEnd(void);
```

**Example:**
```c
ClayKit_TableConfig cfg = { .striped = true, .bordered = true };

ClayKit_TableBegin(&ctx, cfg);
  // Header
  ClayKit_TableHeaderRow(&ctx, cfg);
    ClayKit_TableHeaderCell(&ctx, 0.5f, cfg);
      CLAY_TEXT(CLAY_STRING("Name"), &header_text_cfg);
    ClayKit_TableCellEnd();
    ClayKit_TableHeaderCell(&ctx, 0.5f, cfg);
      CLAY_TEXT(CLAY_STRING("Value"), &header_text_cfg);
    ClayKit_TableCellEnd();
  ClayKit_TableRowEnd();
  // Data rows
  ClayKit_TableRow(&ctx, 0, cfg);
    ClayKit_TableCell(&ctx, 0.5f, 0, cfg);
      CLAY_TEXT(CLAY_STRING("Alpha"), &text_cfg);
    ClayKit_TableCellEnd();
    ClayKit_TableCell(&ctx, 0.5f, 0, cfg);
      CLAY_TEXT(CLAY_STRING("100"), &text_cfg);
    ClayKit_TableCellEnd();
  ClayKit_TableRowEnd();
ClayKit_TableEnd();
```

---

### Button

An interactive button with hover states.

```c
typedef enum {
    CLAYKIT_BUTTON_SOLID = 0,  // Filled background
    CLAYKIT_BUTTON_OUTLINE,    // Border only
    CLAYKIT_BUTTON_GHOST       // No background or border
} ClayKit_ButtonVariant;

typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_ButtonVariant variant;
    ClayKit_Size size;
    bool disabled;
    ClayKit_Icon icon_left;      // Optional left icon
    ClayKit_Icon icon_right;     // Optional right icon
} ClayKit_ButtonConfig;

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
    bool checked,
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

### Radio

A radio button for single-selection groups.

```c
typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_Size size;
    bool disabled;
} ClayKit_RadioConfig;

// Returns true if hovered
bool ClayKit_Radio(
    ClayKit_Context *ctx,
    bool selected,               // Whether this option is selected
    ClayKit_RadioConfig cfg
);
```

**Example:**
```c
static int selected_option = 0;

for (int i = 0; i < 3; i++) {
    if (ClayKit_Radio(&ctx, selected_option == i, (ClayKit_RadioConfig){0})) {
        if (IsMouseButtonPressed()) {
            selected_option = i;
        }
    }
    // Render label next to radio...
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
    bool on,
    ClayKit_SwitchConfig cfg
);
```

**Example:**
```c
static bool enabled = false;

if (ClayKit_Switch(&ctx, enabled, (ClayKit_SwitchConfig){0})) {
    if (IsMouseButtonPressed()) {
        enabled = !enabled;
    }
}
```

---

### Progress

A progress bar showing completion percentage.

```c
typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_Size size;
    bool striped;                // Show striped pattern (visual only)
} ClayKit_ProgressConfig;

void ClayKit_Progress(
    ClayKit_Context *ctx,
    float value,                 // 0.0 to 1.0
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
    float min;                   // Minimum value
    float max;                   // Maximum value
    bool disabled;
} ClayKit_SliderConfig;

// Returns true if hovered
bool ClayKit_Slider(
    ClayKit_Context *ctx,
    float value,                 // 0.0 to 1.0
    ClayKit_SliderConfig cfg
);
```

**Note:** ClayKit renders the slider but doesn't handle drag interaction. You need to implement value updates based on mouse position when dragging.

---

### Select

A dropdown select with trigger button and floating options list.

```c
typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_Size size;
    bool disabled;
} ClayKit_SelectConfig;

// Render the trigger button. Returns true if hovered.
bool ClayKit_SelectTrigger(
    ClayKit_Context *ctx,
    const char *id, int32_t id_len,
    const char *display_text, int32_t display_len,
    ClayKit_SelectConfig cfg
);

// Open the floating dropdown (only call when select is open)
void ClayKit_SelectDropdownBegin(
    ClayKit_Context *ctx,
    const char *id, int32_t id_len,
    ClayKit_SelectConfig cfg
);

// Render an option. Returns true if hovered.
bool ClayKit_SelectOption(
    ClayKit_Context *ctx,
    const char *text, int32_t text_len,
    bool is_selected,
    ClayKit_SelectConfig cfg
);

void ClayKit_SelectDropdownEnd(void);
```

**Example:**
```c
static int selected = -1;
static bool select_open = false;
const char *options[] = { "Apple", "Banana", "Cherry" };

const char *display = selected >= 0 ? options[selected] : "Choose...";
if (ClayKit_SelectTrigger(&ctx, "fruit", 5, display, strlen(display),
    (ClayKit_SelectConfig){0})) {
    if (IsMouseButtonPressed()) select_open = !select_open;
}

if (select_open) {
    ClayKit_SelectDropdownBegin(&ctx, "fruit", 5, (ClayKit_SelectConfig){0});
    for (int i = 0; i < 3; i++) {
        if (ClayKit_SelectOption(&ctx, options[i], strlen(options[i]),
            selected == i, (ClayKit_SelectConfig){0})) {
            if (IsMouseButtonPressed()) {
                selected = i;
                select_open = false;
            }
        }
    }
    ClayKit_SelectDropdownEnd();
}
```

---

### Alert

A message box for notifications or warnings.

```c
typedef enum {
    CLAYKIT_ALERT_SUBTLE = 0,  // Light background tint
    CLAYKIT_ALERT_SOLID,       // Filled background
    CLAYKIT_ALERT_OUTLINE      // Border only
} ClayKit_AlertVariant;

typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_AlertVariant variant;
    ClayKit_Icon icon;           // Optional icon
} ClayKit_AlertConfig;

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
    ClayKit_TooltipPosition position;  // TOP, BOTTOM, LEFT, RIGHT
} ClayKit_TooltipConfig;

void ClayKit_Tooltip(
    ClayKit_Context *ctx,
    const char *text,
    int32_t text_len,
    ClayKit_TooltipConfig cfg
);
```

**Note:** This renders a static tooltip element. For hover-triggered tooltips, conditionally render based on another element's hover state.

---

### Spinner

An animated loading indicator. Renders as a circular element; actual rotation drawing is done by your renderer using the angle value.

```c
typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_Size size;               // Spinner diameter
    float speed;                     // Rotations per second (0 = default 1.0)
} ClayKit_SpinnerConfig;

void ClayKit_Spinner(
    ClayKit_Context *ctx,
    ClayKit_SpinnerConfig cfg
);

// Get current rotation angle (call in render loop)
float ClayKit_SpinnerAngle(
    ClayKit_Context *ctx,
    ClayKit_SpinnerConfig cfg
);
```

**Example:**
```c
ClayKit_Spinner(&ctx, (ClayKit_SpinnerConfig){
    .color_scheme = CLAYKIT_COLOR_PRIMARY,
    .size = CLAYKIT_SIZE_MD
});
```

---

### Tabs

Tab navigation with line, enclosed, or soft variants.

```c
typedef enum {
    CLAYKIT_TABS_LINE = 0,     // Underline indicator
    CLAYKIT_TABS_ENCLOSED,     // Box-style tabs
    CLAYKIT_TABS_SOFT          // Soft rounded background
} ClayKit_TabsVariant;

typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_TabsVariant variant;
    ClayKit_Size size;
} ClayKit_TabsConfig;

// Render a single tab. Returns true if hovered.
bool ClayKit_Tab(
    ClayKit_Context *ctx,
    const char *label,
    int32_t label_len,
    bool is_active,
    ClayKit_TabsConfig cfg
);
```

**Example:**
```c
static int active_tab = 0;
ClayKit_TabsConfig cfg = { .variant = CLAYKIT_TABS_LINE };

// Wrap tabs in a horizontal container
CLAY({ .layout = { .layoutDirection = CLAY_LEFT_TO_RIGHT } }) {
    if (ClayKit_Tab(&ctx, "Tab 1", 5, active_tab == 0, cfg)) {
        if (IsMouseButtonPressed()) active_tab = 0;
    }
    if (ClayKit_Tab(&ctx, "Tab 2", 5, active_tab == 1, cfg)) {
        if (IsMouseButtonPressed()) active_tab = 1;
    }
}
```

---

### Link

A text element with hover color change and optional underline.

```c
typedef enum {
    CLAYKIT_LINK_UNDERLINE = 0,       // Always underlined
    CLAYKIT_LINK_HOVER_UNDERLINE,     // Underline on hover only
    CLAYKIT_LINK_NONE                 // No underline
} ClayKit_LinkVariant;

typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_Size size;
    ClayKit_LinkVariant variant;
    bool disabled;
} ClayKit_LinkConfig;

// Returns true if hovered
bool ClayKit_Link(
    ClayKit_Context *ctx,
    const char *text,
    int32_t text_len,
    ClayKit_LinkConfig cfg
);
```

**Example:**
```c
if (ClayKit_Link(&ctx, "Click here", 10, (ClayKit_LinkConfig){
    .variant = CLAYKIT_LINK_UNDERLINE,
    .color_scheme = CLAYKIT_COLOR_PRIMARY
})) {
    if (IsMouseButtonPressed()) {
        // Handle navigation
    }
}
```

---

### Breadcrumb

A horizontal navigation path with separators. The last item is styled as the current page (non-interactive).

```c
typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_Size size;
    const char *separator;         // Separator text (NULL = "/")
    int32_t separator_len;
} ClayKit_BreadcrumbConfig;

void ClayKit_BreadcrumbBegin(ClayKit_Context *ctx, ClayKit_BreadcrumbConfig cfg);
bool ClayKit_BreadcrumbItem(
    ClayKit_Context *ctx,
    const char *text, int32_t text_len,
    bool is_current,               // Current page (non-interactive)
    ClayKit_BreadcrumbConfig cfg
);
void ClayKit_BreadcrumbSeparator(ClayKit_Context *ctx, ClayKit_BreadcrumbConfig cfg);
void ClayKit_BreadcrumbEnd(void);
```

**Example:**
```c
ClayKit_BreadcrumbConfig cfg = {0};

ClayKit_BreadcrumbBegin(&ctx, cfg);
  if (ClayKit_BreadcrumbItem(&ctx, "Home", 4, false, cfg)) {
      if (IsMouseButtonPressed()) { /* navigate */ }
  }
  ClayKit_BreadcrumbSeparator(&ctx, cfg);
  if (ClayKit_BreadcrumbItem(&ctx, "Products", 8, false, cfg)) {
      if (IsMouseButtonPressed()) { /* navigate */ }
  }
  ClayKit_BreadcrumbSeparator(&ctx, cfg);
  ClayKit_BreadcrumbItem(&ctx, "Widget", 6, true, cfg);  // current page
ClayKit_BreadcrumbEnd();
```

---

### Accordion

Collapsible sections with clickable headers. User controls the open/close state (immediate mode).

```c
typedef enum {
    CLAYKIT_ACCORDION_BORDERED = 0,   // Border between items
    CLAYKIT_ACCORDION_SEPARATED       // Cards with gap
} ClayKit_AccordionVariant;

typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_Size size;
    ClayKit_AccordionVariant variant;
} ClayKit_AccordionConfig;

void ClayKit_AccordionBegin(ClayKit_Context *ctx, ClayKit_AccordionConfig cfg);
void ClayKit_AccordionItemBegin(ClayKit_Context *ctx, bool is_open, ClayKit_AccordionConfig cfg);
bool ClayKit_AccordionHeader(
    ClayKit_Context *ctx,
    const char *text, int32_t text_len,
    bool is_open,                  // Controls chevron direction
    ClayKit_AccordionConfig cfg
);
void ClayKit_AccordionContentBegin(ClayKit_Context *ctx, ClayKit_AccordionConfig cfg);
void ClayKit_AccordionContentEnd(void);
void ClayKit_AccordionItemEnd(void);
void ClayKit_AccordionEnd(void);
```

**Example:**
```c
static bool section_open[3] = { true, false, false };
ClayKit_AccordionConfig cfg = { .variant = CLAYKIT_ACCORDION_BORDERED };

ClayKit_AccordionBegin(&ctx, cfg);
for (int i = 0; i < 3; i++) {
    ClayKit_AccordionItemBegin(&ctx, section_open[i], cfg);
    if (ClayKit_AccordionHeader(&ctx, titles[i], title_lens[i],
        section_open[i], cfg)) {
        if (IsMouseButtonPressed()) section_open[i] = !section_open[i];
    }
    if (section_open[i]) {
        ClayKit_AccordionContentBegin(&ctx, cfg);
        // Render content...
        CLAY_TEXT(CLAY_STRING("Section content here"), &text_cfg);
        ClayKit_AccordionContentEnd();
    }
    ClayKit_AccordionItemEnd();
}
ClayKit_AccordionEnd();
```

---

### Menu

A floating dropdown menu with items and separators. No built-in trigger - use a Button or any element to toggle it.

```c
typedef struct {
    ClayKit_ColorScheme color_scheme;
    ClayKit_Size size;
    bool disabled;
} ClayKit_MenuConfig;

void ClayKit_MenuDropdownBegin(
    ClayKit_Context *ctx,
    const char *id, int32_t id_len,
    ClayKit_MenuConfig cfg
);

// Returns true if hovered and not disabled
bool ClayKit_MenuItem(
    ClayKit_Context *ctx,
    const char *text, int32_t text_len,
    bool disabled,
    ClayKit_MenuConfig cfg
);

void ClayKit_MenuSeparator(ClayKit_Context *ctx, ClayKit_MenuConfig cfg);
void ClayKit_MenuDropdownEnd(void);
```

**Example:**
```c
static bool menu_open = false;

if (ClayKit_Button(&ctx, "Actions", 7, (ClayKit_ButtonConfig){0})) {
    if (IsMouseButtonPressed()) menu_open = !menu_open;
}

if (menu_open) {
    ClayKit_MenuConfig cfg = {0};
    ClayKit_MenuDropdownBegin(&ctx, "actions_menu", 12, cfg);
    if (ClayKit_MenuItem(&ctx, "Edit", 4, false, cfg)) {
        if (IsMouseButtonPressed()) { /* handle edit */ menu_open = false; }
    }
    if (ClayKit_MenuItem(&ctx, "Duplicate", 9, false, cfg)) {
        if (IsMouseButtonPressed()) { /* handle duplicate */ menu_open = false; }
    }
    ClayKit_MenuSeparator(&ctx, cfg);
    ClayKit_MenuItem(&ctx, "Delete", 6, true, cfg);  // disabled
    ClayKit_MenuDropdownEnd();
}
```

---

### Modal

A dialog overlay with backdrop. Uses Clay's floating element system. ClayKit provides style computation.

```c
typedef enum {
    CLAYKIT_MODAL_SM = 0,    // 400px
    CLAYKIT_MODAL_MD,        // 500px
    CLAYKIT_MODAL_LG,        // 600px
    CLAYKIT_MODAL_XL,        // 800px
    CLAYKIT_MODAL_FULL       // Full width with margins
} ClayKit_ModalSize;

typedef struct {
    ClayKit_ModalSize size;
    bool close_on_backdrop;      // Close when clicking backdrop
    uint16_t z_index;            // Z-index (default: 1000)
} ClayKit_ModalConfig;

ClayKit_ModalStyle ClayKit_ComputeModalStyle(
    ClayKit_Context *ctx,
    ClayKit_ModalConfig cfg
);
```

**Note:** See `examples/c-raylib/main.c` for full modal implementation using floating elements and the computed style.

---

### Drawer

A slide-in panel from any edge of the screen. Returns true if the backdrop was hovered (for click-to-close).

```c
typedef enum {
    CLAYKIT_DRAWER_LEFT = 0,
    CLAYKIT_DRAWER_RIGHT,
    CLAYKIT_DRAWER_TOP,
    CLAYKIT_DRAWER_BOTTOM
} ClayKit_DrawerSide;

typedef struct {
    ClayKit_DrawerSide side;         // Which edge to slide from
    uint16_t size;                   // Width or height (0 = default 300)
    bool close_on_backdrop;          // Close on backdrop click
    uint16_t z_index;                // Z-index (default: 1000)
} ClayKit_DrawerConfig;

// Returns true if backdrop is hovered (for close on click)
bool ClayKit_DrawerBegin(
    ClayKit_Context *ctx,
    const char *id, int32_t id_len,
    ClayKit_DrawerConfig cfg
);
void ClayKit_DrawerEnd(void);
```

**Example:**
```c
static bool drawer_open = false;

if (drawer_open) {
    bool backdrop_hovered = ClayKit_DrawerBegin(&ctx, "drawer1", 7,
        (ClayKit_DrawerConfig){ .side = CLAYKIT_DRAWER_LEFT });
    // Render drawer content...
    CLAY_TEXT(CLAY_STRING("Drawer content"), &text_cfg);
    ClayKit_DrawerEnd();

    if (backdrop_hovered && IsMouseButtonPressed()) {
        drawer_open = false;
    }
}
```

---

### Popover

A floating content area anchored to its parent element.

```c
typedef enum {
    CLAYKIT_POPOVER_TOP = 0,
    CLAYKIT_POPOVER_BOTTOM,
    CLAYKIT_POPOVER_LEFT,
    CLAYKIT_POPOVER_RIGHT
} ClayKit_PopoverPosition;

typedef struct {
    ClayKit_PopoverPosition position;  // Position relative to anchor
    uint16_t z_index;                  // Z-index (default: 50)
} ClayKit_PopoverConfig;

void ClayKit_PopoverBegin(
    ClayKit_Context *ctx,
    const char *id, int32_t id_len,
    ClayKit_PopoverConfig cfg
);
void ClayKit_PopoverEnd(void);
```

**Example:**
```c
static bool popover_open = false;

// Trigger element (popover attaches to parent)
if (ClayKit_Button(&ctx, "Info", 4, (ClayKit_ButtonConfig){0})) {
    if (IsMouseButtonPressed()) popover_open = !popover_open;
}

if (popover_open) {
    ClayKit_PopoverBegin(&ctx, "info_pop", 8,
        (ClayKit_PopoverConfig){ .position = CLAYKIT_POPOVER_BOTTOM });
    CLAY_TEXT(CLAY_STRING("Additional information here"), &text_cfg);
    ClayKit_PopoverEnd();
}
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

// Tag
claykit.tag(&ctx, "Label", .{ .closeable = true });

// Stat
claykit.stat(&ctx, "Users", "1,024", "Since launch", .{});

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

// Link
if (claykit.link(&ctx, "Click here", .{ .variant = .underline })) {
    // Hovered
}

// Breadcrumb (convenience wrapper - renders all items with separators)
const crumbs = [_][]const u8{ "Home", "Products", "Widget" };
if (claykit.breadcrumb(&ctx, &crumbs, .{})) |hovered_idx| {
    // An item was hovered (last item = current page, not interactive)
}

// Accordion
claykit.accordionBegin(&ctx, .{});
claykit.accordionItemBegin(&ctx, is_open, .{});
if (claykit.accordionHeader(&ctx, "Section 1", is_open, .{})) {
    if (raylib.isMouseButtonPressed(.left)) is_open = !is_open;
}
if (is_open) {
    claykit.accordionContentBegin(&ctx, .{});
    // Content...
    claykit.accordionContentEnd();
}
claykit.accordionItemEnd();
claykit.accordionEnd();

// Menu
claykit.menuDropdownBegin(&ctx, "menu1", .{});
if (claykit.menuItem(&ctx, "Edit", false, .{})) { /* hovered */ }
claykit.menuSeparator(&ctx, .{});
_ = claykit.menuItem(&ctx, "Delete", true, .{});  // disabled
claykit.menuDropdownEnd();

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
