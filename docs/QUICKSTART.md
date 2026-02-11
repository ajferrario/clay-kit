# ClayKit Quick Start Guide

Get up and running with ClayKit in 5 minutes.

## Prerequisites

- C99 compiler (gcc, clang, MSVC)
- [Clay](https://github.com/nicbarker/clay) UI library (included in `vendor/`)
- A renderer (raylib, SDL, OpenGL, etc.)

## Installation

ClayKit is a single-header library. Copy these files to your project:

```
vendor/clay.h      # Clay UI library
clay_kit.h         # ClayKit components
```

## Minimal Example

```c
#include <stdio.h>

// Include Clay first
#define CLAY_IMPLEMENTATION
#include "clay.h"

// Then include ClayKit
#define CLAYKIT_IMPLEMENTATION
#include "clay_kit.h"

// Your text measurement function (required by Clay)
Clay_Dimensions MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData) {
    // Implement based on your renderer
    return (Clay_Dimensions){ text.length * 8, 16 };  // Simple approximation
}

int main(void) {
    // 1. Initialize Clay
    uint32_t clay_mem_size = Clay_MinMemorySize();
    void *clay_memory = malloc(clay_mem_size);
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(clay_mem_size, clay_memory);
    Clay_Initialize(arena, (Clay_Dimensions){ 800, 600 }, (Clay_ErrorHandler){0});
    Clay_SetMeasureTextFunction(MeasureText, NULL);

    // 2. Initialize ClayKit
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State states[64] = {0};
    ClayKit_Context ctx = {0};
    ClayKit_Init(&ctx, &theme, states, 64);

    // 3. Build UI
    Clay_BeginLayout();

    // Root container
    CLAY({
        .layout = {
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = { 20, 20, 20, 20 },
            .childGap = 10,
            .layoutDirection = CLAY_TOP_TO_BOTTOM
        },
        .backgroundColor = theme.bg
    }) {
        // Badge
        ClayKit_BadgeRaw(&ctx, "Hello", 5, (ClayKit_BadgeConfig){0});

        // Button
        if (ClayKit_Button(&ctx, "Click Me", 8, (ClayKit_ButtonConfig){0})) {
            printf("Button hovered!\n");
        }

        // Progress bar
        ClayKit_Progress(&ctx, 0.7f, (ClayKit_ProgressConfig){0});
    }

    // 4. Get render commands
    Clay_RenderCommandArray commands = Clay_EndLayout();

    // 5. Render (implement based on your renderer)
    for (int i = 0; i < commands.length; i++) {
        Clay_RenderCommand *cmd = Clay_RenderCommandArray_Get(&commands, i);
        // Draw rectangles, text, borders based on cmd->commandType
    }

    free(clay_memory);
    return 0;
}
```

## Compile

```bash
gcc -std=c99 -o demo demo.c
```

## Adding Interactivity

### Button Clicks

```c
// Track hover state from button
bool btn_hovered = ClayKit_Button(&ctx, "Save", 4, (ClayKit_ButtonConfig){0});

// After Clay_EndLayout(), check for clicks
if (btn_hovered && IsMouseClicked()) {
    DoSave();
}
```

### Text Input

```c
// Setup (once)
char buffer[256] = {0};
ClayKit_InputState input = {
    .buf = buffer,
    .cap = sizeof(buffer),
    .len = 0,
    .cursor = 0,
    .select_start = 0,
    .flags = 0
};

// Each frame:
// 1. Handle keyboard if focused
if (input.flags & CLAYKIT_INPUT_FOCUSED) {
    if (IsKeyPressed(KEY_BACKSPACE)) {
        ClayKit_InputHandleKey(&input, CLAYKIT_KEY_BACKSPACE, 0);
    }
    int ch = GetCharPressed();
    while (ch) {
        ClayKit_InputHandleChar(&input, ch);
        ch = GetCharPressed();
    }
}

// 2. Render
bool hovered = ClayKit_TextInput(&ctx, "input1", 6, &input,
    (ClayKit_InputConfig){0}, "Type here...", 12);

// 3. Handle click for focus
if (IsMouseClicked()) {
    if (hovered) {
        input.flags |= CLAYKIT_INPUT_FOCUSED;
    } else {
        input.flags &= ~CLAYKIT_INPUT_FOCUSED;
    }
}
```

### Tabs

```c
static int active_tab = 0;

// Render tabs in a row
CLAY({ .layout = { .layoutDirection = CLAY_LEFT_TO_RIGHT } }) {
    if (ClayKit_Tab(&ctx, "Tab 1", 5, active_tab == 0, cfg) && IsMouseClicked()) {
        active_tab = 0;
    }
    if (ClayKit_Tab(&ctx, "Tab 2", 5, active_tab == 1, cfg) && IsMouseClicked()) {
        active_tab = 1;
    }
}

// Render content based on active_tab
if (active_tab == 0) {
    // Tab 1 content
} else {
    // Tab 2 content
}
```

## Theming

### Using Preset Themes

```c
// Light theme (default)
ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;

// Dark theme
ClayKit_Theme theme = CLAYKIT_THEME_DARK;
```

### Custom Theme

```c
ClayKit_Theme my_theme = {
    .primary = { 0, 122, 255, 255 },    // Blue
    .secondary = { 245, 245, 250, 255 },
    .success = { 52, 199, 89, 255 },    // Green
    .warning = { 255, 149, 0, 255 },    // Orange
    .error = { 255, 59, 48, 255 },      // Red
    .bg = { 255, 255, 255, 255 },
    .fg = { 0, 0, 0, 255 },
    .border = { 209, 209, 214, 255 },
    .muted = { 142, 142, 147, 255 },
    .spacing = { .xs = 4, .sm = 8, .md = 16, .lg = 24, .xl = 32 },
    .radius = { .sm = 4, .md = 8, .lg = 16, .full = 9999 },
    .font_id = { .body = 0, .heading = 0 },
    .font_size = { .xs = 10, .sm = 12, .md = 14, .lg = 18, .xl = 24 }
};
```

## Color Schemes

Components accept color schemes for semantic coloring:

```c
// Primary (default)
ClayKit_Button(&ctx, "Primary", 7, (ClayKit_ButtonConfig){0});

// Success
ClayKit_Button(&ctx, "Success", 7, (ClayKit_ButtonConfig){
    .color_scheme = CLAYKIT_COLOR_SUCCESS
});

// Warning
ClayKit_AlertText(&ctx, "Warning!", 8, (ClayKit_AlertConfig){
    .color_scheme = CLAYKIT_COLOR_WARNING
});

// Error
ClayKit_BadgeRaw(&ctx, "Error", 5, (ClayKit_BadgeConfig){
    .color_scheme = CLAYKIT_COLOR_ERROR
});
```

## Component Variants

Many components have style variants:

```c
// Badge variants
ClayKit_BadgeRaw(&ctx, "Solid", 5, (ClayKit_BadgeConfig){
    .variant = CLAYKIT_BADGE_SOLID      // Filled (default)
});
ClayKit_BadgeRaw(&ctx, "Subtle", 6, (ClayKit_BadgeConfig){
    .variant = CLAYKIT_BADGE_SUBTLE     // Light tint
});
ClayKit_BadgeRaw(&ctx, "Outline", 7, (ClayKit_BadgeConfig){
    .variant = CLAYKIT_BADGE_OUTLINE    // Border only
});

// Button variants
ClayKit_Button(&ctx, "Solid", 5, (ClayKit_ButtonConfig){
    .variant = CLAYKIT_BUTTON_SOLID     // Filled (default)
});
ClayKit_Button(&ctx, "Outline", 7, (ClayKit_ButtonConfig){
    .variant = CLAYKIT_BUTTON_OUTLINE   // Border only
});
ClayKit_Button(&ctx, "Ghost", 5, (ClayKit_ButtonConfig){
    .variant = CLAYKIT_BUTTON_GHOST     // No border
});

// Tab variants
ClayKit_Tab(&ctx, "Line", 4, true, (ClayKit_TabsConfig){
    .variant = CLAYKIT_TABS_LINE        // Underline (default)
});
ClayKit_Tab(&ctx, "Enclosed", 8, true, (ClayKit_TabsConfig){
    .variant = CLAYKIT_TABS_ENCLOSED    // Box style
});
```

## Size Scale

Components support multiple sizes:

```c
typedef enum {
    CLAYKIT_SIZE_XS,  // Extra small
    CLAYKIT_SIZE_SM,  // Small
    CLAYKIT_SIZE_MD,  // Medium (default)
    CLAYKIT_SIZE_LG,  // Large
    CLAYKIT_SIZE_XL   // Extra large
} ClayKit_Size;

// Example
ClayKit_Button(&ctx, "Small", 5, (ClayKit_ButtonConfig){
    .size = CLAYKIT_SIZE_SM
});
ClayKit_Button(&ctx, "Large", 5, (ClayKit_ButtonConfig){
    .size = CLAYKIT_SIZE_LG
});
```

## Navigation Components

### Link

```c
if (ClayKit_Link(&ctx, "Learn more", 10, (ClayKit_LinkConfig){
    .variant = CLAYKIT_LINK_UNDERLINE
})) {
    if (IsMouseClicked()) { /* navigate */ }
}
```

### Breadcrumb

```c
ClayKit_BreadcrumbConfig bcfg = {0};
ClayKit_BreadcrumbBegin(&ctx, bcfg);
ClayKit_BreadcrumbItem(&ctx, "Home", 4, false, bcfg);
ClayKit_BreadcrumbSeparator(&ctx, bcfg);
ClayKit_BreadcrumbItem(&ctx, "Docs", 4, true, bcfg);  // current page
ClayKit_BreadcrumbEnd();
```

### Accordion

```c
static bool open[2] = { true, false };
ClayKit_AccordionConfig acfg = {0};

ClayKit_AccordionBegin(&ctx, acfg);
for (int i = 0; i < 2; i++) {
    ClayKit_AccordionItemBegin(&ctx, open[i], acfg);
    if (ClayKit_AccordionHeader(&ctx, titles[i], lens[i], open[i], acfg)) {
        if (IsMouseClicked()) open[i] = !open[i];
    }
    if (open[i]) {
        ClayKit_AccordionContentBegin(&ctx, acfg);
        // Your content here
        ClayKit_AccordionContentEnd();
    }
    ClayKit_AccordionItemEnd();
}
ClayKit_AccordionEnd();
```

### Menu

```c
static bool menu_open = false;

if (ClayKit_Button(&ctx, "Actions", 7, (ClayKit_ButtonConfig){0})) {
    if (IsMouseClicked()) menu_open = !menu_open;
}
if (menu_open) {
    ClayKit_MenuConfig mcfg = {0};
    ClayKit_MenuDropdownBegin(&ctx, "menu", 4, mcfg);
    if (ClayKit_MenuItem(&ctx, "Edit", 4, false, mcfg)) {
        if (IsMouseClicked()) { /* handle */ menu_open = false; }
    }
    ClayKit_MenuSeparator(&ctx, mcfg);
    ClayKit_MenuItem(&ctx, "Delete", 6, true, mcfg);  // disabled
    ClayKit_MenuDropdownEnd();
}
```

## Data Display Components

### Tag

```c
ClayKit_TagRaw(&ctx, "React", 5, (ClayKit_TagConfig){
    .color_scheme = CLAYKIT_COLOR_PRIMARY,
    .closeable = true
});
```

### Stat

```c
ClayKit_Stat(&ctx, "Revenue", 7, "$12,345", 7, "+8% this month", 14,
    (ClayKit_StatConfig){0});
```

### List

```c
ClayKit_ListConfig lcfg = { .ordered = true };
ClayKit_ListBegin(&ctx, lcfg);
ClayKit_ListItemRaw(&ctx, "First step", 10, 0, lcfg);
ClayKit_ListItemRaw(&ctx, "Second step", 11, 1, lcfg);
ClayKit_ListEnd();
```

### Select

```c
static int selected = -1;
static bool sel_open = false;
const char *display = selected >= 0 ? opts[selected] : "Choose...";

if (ClayKit_SelectTrigger(&ctx, "sel", 3, display, strlen(display),
    (ClayKit_SelectConfig){0})) {
    if (IsMouseClicked()) sel_open = !sel_open;
}
if (sel_open) {
    ClayKit_SelectDropdownBegin(&ctx, "sel", 3, (ClayKit_SelectConfig){0});
    for (int i = 0; i < count; i++) {
        if (ClayKit_SelectOption(&ctx, opts[i], lens[i], selected == i,
            (ClayKit_SelectConfig){0})) {
            if (IsMouseClicked()) { selected = i; sel_open = false; }
        }
    }
    ClayKit_SelectDropdownEnd();
}
```

## Next Steps

- See [API Reference](API.md) for complete documentation
- See [Architecture](ARCHITECTURE.md) for internal design details
- Check `examples/c-raylib/` for a full working example
- Check `examples/zig-raylib/` for Zig usage
