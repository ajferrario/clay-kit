# clay-kit

Zero-allocation UI components for [Clay](https://github.com/nicbarker/clay).

## What

Single-header C99 library providing ChakraUI-style components (buttons, inputs, checkboxes, modals, etc.) built on Clay's layout primitives. Exports clean Zig bindings via `@cImport`.

## Features

- Zero heap allocation - you provide all memory
- Single header `clay_kit.h`
- C99 compatible
- Zig-friendly (no bitfields, no VLAs, no complex macros)
- Theming system with light/dark presets
- Focus management for keyboard navigation
- Text input with cursor/selection handling

## Usage

```c
#define CLAY_IMPLEMENTATION
#include "clay.h"

#define CLAYKIT_IMPLEMENTATION
#include "clay_kit.h"

// Setup
ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
ClayKit_State state_buf[256];
ClayKit_Context ctx;
ClayKit_Init(&ctx, &theme, state_buf, 256);

// In your layout
CLAYKIT_BUTTON(&ctx, CLAY_ID("submit"), {
    .variant = CLAYKIT_BUTTON_SOLID,
    .size = CLAYKIT_SIZE_MD,
}) {
    CLAY_TEXT(CLAY_STRING("Submit"), &ctx.theme->button_text);
}
```

## Zig

```zig
const clay = @cImport(@cInclude("clay.h"));
const kit = @cImport(@cInclude("clay_kit.h"));
```

## Components

**Layout**: Box, Flex, Stack, Center, Container, Grid, Spacer

**Typography**: Text, Heading, Badge, Tag, List

**Form**: Button, Input, Textarea, Checkbox, Switch, Slider, Select, Radio

**Feedback**: Alert, Progress, Spinner, Tooltip, Modal, Drawer

**Navigation**: Tabs, Accordion, Breadcrumb, Menu, Link

## License

MIT
