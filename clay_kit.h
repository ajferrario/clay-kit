/**
 * @file clay_kit.h
 * @brief Zero-allocation UI components for Clay
 * @version 1.0.0
 *
 * ClayKit is a single-header C99 library providing ready-to-use UI components
 * built on the Clay layout system. It features:
 *
 * - Zero heap allocation (you provide all memory)
 * - Complete theming with light/dark presets
 * - Text input with cursor positioning and selection
 * - Hand-written Zig bindings available
 *
 * ## Quick Start
 *
 * ```c
 * // In ONE source file:
 * #define CLAY_IMPLEMENTATION
 * #include "clay.h"
 * #define CLAYKIT_IMPLEMENTATION
 * #include "clay_kit.h"
 *
 * // Initialize
 * ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
 * ClayKit_State states[64] = {0};
 * ClayKit_Context ctx = {0};
 * ClayKit_Init(&ctx, &theme, states, 64);
 *
 * // In render loop
 * Clay_BeginLayout();
 * ClayKit_Button(&ctx, "Click", 5, (ClayKit_ButtonConfig){0});
 * Clay_RenderCommandArray cmds = Clay_EndLayout();
 * ```
 *
 * ## Components
 *
 * - Badge: Status labels (solid/subtle/outline variants)
 * - Button: Interactive buttons with hover states
 * - Progress: Progress bars
 * - Slider: Value sliders
 * - Alert: Notification boxes
 * - Tooltip: Text hints
 * - Tabs: Tab navigation (line/enclosed variants)
 * - Modal: Dialog overlays
 * - Checkbox: Checkable boxes
 * - Switch: Toggle switches
 * - TextInput: Full text editing with cursor
 *
 * ## Documentation
 *
 * See docs/API.md for complete API reference.
 * See docs/QUICKSTART.md for getting started guide.
 *
 * ## License
 *
 * MIT License - see repository for details.
 */

#ifndef CLAY_KIT_H
#define CLAY_KIT_H

#include <stdint.h>
#include <stdbool.h>
#include "clay.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Icon System
 * ============================================================================ */

typedef struct ClayKit_Icon {
    uint16_t id;    /* 0 = no icon */
    uint16_t size;  /* pixels */
} ClayKit_Icon;

typedef void (*ClayKit_IconCallback)(uint16_t icon_id, Clay_BoundingBox box, void *user_data);

/* ============================================================================
 * Text Measurement
 * ============================================================================ */

typedef struct ClayKit_TextDimensions {
    float width;
    float height;
} ClayKit_TextDimensions;

typedef ClayKit_TextDimensions (*ClayKit_MeasureTextCallback)(
    const char *text,
    uint32_t length,
    uint16_t font_id,
    uint16_t font_size,
    void *user_data
);

/* ============================================================================
 * Component State
 * ============================================================================ */

typedef struct ClayKit_State ClayKit_State;
struct ClayKit_State {
    uint32_t id;
    uint32_t flags;  /* component-specific bits */
    float value;     /* for sliders, progress, etc. */
};

/* ============================================================================
 * Theme System
 * ============================================================================ */

typedef struct ClayKit_SpacingScale {
    uint16_t xs;
    uint16_t sm;
    uint16_t md;
    uint16_t lg;
    uint16_t xl;
} ClayKit_SpacingScale;

typedef struct ClayKit_RadiusScale {
    uint16_t sm;
    uint16_t md;
    uint16_t lg;
    uint16_t full;
} ClayKit_RadiusScale;

typedef struct ClayKit_FontIds {
    uint16_t body;
    uint16_t heading;
} ClayKit_FontIds;

typedef struct ClayKit_FontSizeScale {
    uint16_t xs;
    uint16_t sm;
    uint16_t md;
    uint16_t lg;
    uint16_t xl;
} ClayKit_FontSizeScale;

typedef struct ClayKit_Theme ClayKit_Theme;
struct ClayKit_Theme {
    /* Color palette */
    Clay_Color primary;
    Clay_Color secondary;
    Clay_Color success;
    Clay_Color warning;
    Clay_Color error;

    /* Semantic colors */
    Clay_Color bg;
    Clay_Color fg;
    Clay_Color border;
    Clay_Color muted;

    /* Scales */
    ClayKit_SpacingScale spacing;
    ClayKit_RadiusScale radius;
    ClayKit_FontIds font_id;
    ClayKit_FontSizeScale font_size;
};

/* ============================================================================
 * Context
 * ============================================================================ */

typedef struct ClayKit_Context ClayKit_Context;
struct ClayKit_Context {
    ClayKit_Theme *theme_ptr;
    ClayKit_State *state_ptr;
    uint32_t state_count;
    uint32_t state_cap;
    uint32_t focused_id;
    uint32_t prev_focused_id;
    ClayKit_IconCallback icon_callback;
    void *icon_user_data;
    ClayKit_MeasureTextCallback measure_text;
    void *measure_text_user_data;
    float cursor_blink_time;  /* Accumulator for cursor blinking */
};

/* ============================================================================
 * Size Variants
 * ============================================================================ */

typedef enum ClayKit_Size {
    CLAYKIT_SIZE_XS = 0,
    CLAYKIT_SIZE_SM = 1,
    CLAYKIT_SIZE_MD = 2,
    CLAYKIT_SIZE_LG = 3,
    CLAYKIT_SIZE_XL = 4
} ClayKit_Size;

/* ============================================================================
 * Color Schemes
 * ============================================================================ */

typedef enum ClayKit_ColorScheme {
    CLAYKIT_COLOR_PRIMARY = 0,
    CLAYKIT_COLOR_SECONDARY = 1,
    CLAYKIT_COLOR_SUCCESS = 2,
    CLAYKIT_COLOR_WARNING = 3,
    CLAYKIT_COLOR_ERROR = 4
} ClayKit_ColorScheme;

/* ============================================================================
 * Text Input System
 * ============================================================================ */

typedef enum ClayKit_InputFlags {
    CLAYKIT_INPUT_FOCUSED  = 1 << 0,
    CLAYKIT_INPUT_PASSWORD = 1 << 1,
    CLAYKIT_INPUT_READONLY = 1 << 2,
    CLAYKIT_INPUT_DISABLED = 1 << 3
} ClayKit_InputFlags;

typedef struct ClayKit_InputState {
    char *buf;
    uint32_t cap;
    uint32_t len;
    uint32_t cursor;
    uint32_t select_start;  /* == cursor when no selection */
    uint8_t flags;
} ClayKit_InputState;

/* Common keys (user maps platform keys to these) */
typedef enum ClayKit_Key {
    CLAYKIT_KEY_NONE = 0,
    CLAYKIT_KEY_BACKSPACE = 1,
    CLAYKIT_KEY_DELETE = 2,
    CLAYKIT_KEY_LEFT = 3,
    CLAYKIT_KEY_RIGHT = 4,
    CLAYKIT_KEY_HOME = 5,
    CLAYKIT_KEY_END = 6,
    CLAYKIT_KEY_ENTER = 7,
    CLAYKIT_KEY_TAB = 8
} ClayKit_Key;

typedef enum ClayKit_Modifier {
    CLAYKIT_MOD_NONE  = 0,
    CLAYKIT_MOD_SHIFT = 1 << 0,
    CLAYKIT_MOD_CTRL  = 1 << 1,
    CLAYKIT_MOD_ALT   = 1 << 2
} ClayKit_Modifier;

/* ============================================================================
 * Typography Configuration
 * ============================================================================ */

typedef struct ClayKit_TextConfig {
    ClayKit_Size size;           /* Font size from theme scale */
    Clay_Color color;            /* Text color (default: theme fg) */
    uint16_t font_id;            /* Font ID (default: theme body font) */
    uint16_t letter_spacing;     /* Letter spacing in pixels */
    uint16_t line_height;        /* Line height (0 = auto) */
} ClayKit_TextConfig;

typedef struct ClayKit_HeadingConfig {
    ClayKit_Size size;           /* XS=h6, SM=h5, MD=h4, LG=h3, XL=h2, default h1 uses XL */
    Clay_Color color;            /* Text color (default: theme fg) */
    uint16_t font_id;            /* Font ID (default: theme heading font) */
} ClayKit_HeadingConfig;

typedef enum ClayKit_BadgeVariant {
    CLAYKIT_BADGE_SOLID = 0,     /* Solid background */
    CLAYKIT_BADGE_SUBTLE = 1,    /* Light background, colored text */
    CLAYKIT_BADGE_OUTLINE = 2    /* Transparent with border */
} ClayKit_BadgeVariant;

typedef struct ClayKit_BadgeConfig {
    ClayKit_ColorScheme color_scheme;  /* Color from theme palette */
    ClayKit_BadgeVariant variant;      /* solid, subtle, or outline */
    ClayKit_Size size;                 /* Size affects padding and font */
} ClayKit_BadgeConfig;

/* Badge computed style - for renderers that need raw values */
typedef struct ClayKit_BadgeStyle {
    Clay_Color bg_color;
    Clay_Color text_color;
    Clay_Color border_color;
    uint16_t border_width;
    uint16_t pad_x;
    uint16_t pad_y;
    uint16_t font_size;
    uint16_t font_id;
    uint16_t corner_radius;
} ClayKit_BadgeStyle;

/* ============================================================================
 * Tag Configuration
 * ============================================================================ */

typedef enum ClayKit_TagVariant {
    CLAYKIT_TAG_SOLID = 0,       /* Solid background */
    CLAYKIT_TAG_SUBTLE = 1,      /* Light background, colored text */
    CLAYKIT_TAG_OUTLINE = 2      /* Transparent with border */
} ClayKit_TagVariant;

typedef struct ClayKit_TagConfig {
    ClayKit_ColorScheme color_scheme;  /* Color from theme palette */
    ClayKit_TagVariant variant;        /* solid, subtle, or outline */
    ClayKit_Size size;                 /* Size affects padding and font */
    bool closeable;                    /* Show close "x" indicator */
} ClayKit_TagConfig;

/* Tag computed style */
typedef struct ClayKit_TagStyle {
    Clay_Color bg_color;
    Clay_Color text_color;
    Clay_Color border_color;
    Clay_Color close_color;      /* Color for the "x" text */
    uint16_t border_width;
    uint16_t pad_x;
    uint16_t pad_y;
    uint16_t font_size;
    uint16_t font_id;
    uint16_t corner_radius;
    uint16_t gap;                /* Gap between text and close indicator */
    uint16_t close_font_size;    /* Font size for close "x" */
} ClayKit_TagStyle;

/* ============================================================================
 * Stat Configuration
 * ============================================================================ */

typedef struct ClayKit_StatConfig {
    ClayKit_Size size;             /* Affects value font size */
    Clay_Color label_color;        /* Label text color (default: theme muted) */
    Clay_Color value_color;        /* Value text color (default: theme fg) */
    Clay_Color help_color;         /* Help text color (default: theme muted) */
} ClayKit_StatConfig;

/* Stat computed style */
typedef struct ClayKit_StatStyle {
    Clay_Color label_color;
    Clay_Color value_color;
    Clay_Color help_color;
    uint16_t label_font_size;      /* Smaller font for label */
    uint16_t value_font_size;      /* Larger font for value */
    uint16_t help_font_size;       /* Smaller font for help text */
    uint16_t label_font_id;        /* Body font for label */
    uint16_t value_font_id;        /* Heading font for value */
    uint16_t help_font_id;         /* Body font for help */
    uint16_t gap;                  /* Gap between elements */
} ClayKit_StatStyle;

/* ============================================================================
 * List Configuration
 * ============================================================================ */

typedef struct ClayKit_ListConfig {
    bool ordered;                  /* true = numbered (1. 2. 3.), false = bullet */
    ClayKit_Size size;             /* Font size and spacing */
    Clay_Color marker_color;       /* Marker color (default: theme muted) */
    Clay_Color text_color;         /* Item text color (default: theme fg) */
} ClayKit_ListConfig;

/* List computed style */
typedef struct ClayKit_ListStyle {
    Clay_Color marker_color;
    Clay_Color text_color;
    uint16_t font_size;
    uint16_t font_id;
    uint16_t gap;                  /* Gap between items (vertical) */
    uint16_t marker_width;         /* Width reserved for marker column */
    uint16_t item_gap;             /* Gap between marker and text (horizontal) */
} ClayKit_ListStyle;

/* ============================================================================
 * Table Configuration
 * ============================================================================ */

typedef struct ClayKit_TableConfig {
    ClayKit_ColorScheme color_scheme;  /* Header color */
    ClayKit_Size size;                 /* Padding and font size */
    bool striped;                      /* Alternate row backgrounds */
    bool bordered;                     /* Borders between cells */
} ClayKit_TableConfig;

/* Table computed style */
typedef struct ClayKit_TableStyle {
    Clay_Color header_bg;          /* Header row background */
    Clay_Color header_text;        /* Header text color */
    Clay_Color row_bg;             /* Normal row background */
    Clay_Color row_alt_bg;         /* Alternate row background (striped) */
    Clay_Color text_color;         /* Cell text color */
    Clay_Color border_color;       /* Border color (if bordered) */
    uint16_t border_width;         /* Border width (0 if not bordered) */
    uint16_t cell_pad_x;           /* Cell horizontal padding */
    uint16_t cell_pad_y;           /* Cell vertical padding */
    uint16_t font_size;            /* Cell text font size */
    uint16_t header_font_size;     /* Header text font size */
    uint16_t font_id;              /* Body font ID */
    uint16_t corner_radius;        /* Table corner radius */
} ClayKit_TableStyle;

/* ============================================================================
 * Button Configuration
 * ============================================================================ */

typedef enum ClayKit_ButtonVariant {
    CLAYKIT_BUTTON_SOLID = 0,    /* Solid background */
    CLAYKIT_BUTTON_OUTLINE = 1,  /* Transparent with border */
    CLAYKIT_BUTTON_GHOST = 2     /* Transparent, no border */
} ClayKit_ButtonVariant;

typedef struct ClayKit_ButtonConfig {
    ClayKit_ColorScheme color_scheme;  /* Color from theme palette */
    ClayKit_ButtonVariant variant;     /* solid, outline, or ghost */
    ClayKit_Size size;                 /* Size affects padding and font */
    bool disabled;                     /* Disabled state */
    ClayKit_Icon icon_left;            /* Icon on left side */
    ClayKit_Icon icon_right;           /* Icon on right side */
} ClayKit_ButtonConfig;

/* Button interaction result */
typedef struct ClayKit_ButtonResult {
    bool clicked;   /* True if button was clicked this frame */
    bool hovered;   /* True if mouse is over button */
    bool pressed;   /* True if mouse is down on button */
} ClayKit_ButtonResult;

/* ============================================================================
 * Layout Primitives - Configuration Structs
 * ============================================================================ */

/* Box: Basic container with theme-aware styling */
typedef struct ClayKit_BoxConfig {
    Clay_Color bg;              /* Background color (default: transparent) */
    Clay_Color border_color;    /* Border color */
    uint16_t border_width;      /* Border width in pixels */
    uint16_t padding;           /* Padding on all sides */
    uint16_t radius;            /* Corner radius */
    Clay_Sizing sizing;         /* Width/height sizing */
} ClayKit_BoxConfig;

/* Flex: Flexible container with direction and gap */
typedef struct ClayKit_FlexConfig {
    Clay_LayoutDirection direction;  /* CLAY_LEFT_TO_RIGHT or CLAY_TOP_TO_BOTTOM */
    uint16_t gap;                    /* Gap between children */
    Clay_ChildAlignment align;       /* Child alignment */
    uint16_t padding;                /* Padding on all sides */
    Clay_Sizing sizing;              /* Width/height sizing */
    Clay_Color bg;                   /* Background color */
} ClayKit_FlexConfig;

/* Stack direction presets */
typedef enum ClayKit_StackDirection {
    CLAYKIT_STACK_VERTICAL = 0,   /* Top to bottom (VStack) */
    CLAYKIT_STACK_HORIZONTAL = 1  /* Left to right (HStack) */
} ClayKit_StackDirection;

/* Stack: Convenience wrapper for Flex with preset directions */
typedef struct ClayKit_StackConfig {
    ClayKit_StackDirection direction;
    uint16_t gap;                    /* Gap between children */
    Clay_ChildAlignment align;       /* Child alignment */
    uint16_t padding;                /* Padding on all sides */
    Clay_Sizing sizing;              /* Width/height sizing */
    Clay_Color bg;                   /* Background color */
} ClayKit_StackConfig;

/* Container: Centered box with max-width */
typedef struct ClayKit_ContainerConfig {
    uint16_t max_width;   /* Maximum width (0 = use theme default 1200) */
    uint16_t padding;     /* Horizontal padding */
    Clay_Color bg;        /* Background color */
} ClayKit_ContainerConfig;

/* ============================================================================
 * Layout Primitives - Helper Functions
 * ============================================================================ */

/* Create Clay_LayoutConfig for Box */
static inline Clay_LayoutConfig ClayKit_BoxLayout(ClayKit_BoxConfig cfg) {
    Clay_LayoutConfig layout;
    layout.sizing = cfg.sizing;
    layout.padding.left = cfg.padding;
    layout.padding.right = cfg.padding;
    layout.padding.top = cfg.padding;
    layout.padding.bottom = cfg.padding;
    layout.childGap = 0;
    layout.childAlignment.x = CLAY_ALIGN_X_LEFT;
    layout.childAlignment.y = CLAY_ALIGN_Y_TOP;
    layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    return layout;
}

/* Create Clay_LayoutConfig for Flex */
static inline Clay_LayoutConfig ClayKit_FlexLayout(ClayKit_FlexConfig cfg) {
    Clay_LayoutConfig layout;
    layout.sizing = cfg.sizing;
    layout.padding.left = cfg.padding;
    layout.padding.right = cfg.padding;
    layout.padding.top = cfg.padding;
    layout.padding.bottom = cfg.padding;
    layout.childGap = cfg.gap;
    layout.childAlignment = cfg.align;
    layout.layoutDirection = cfg.direction;
    return layout;
}

/* Create Clay_LayoutConfig for Stack */
static inline Clay_LayoutConfig ClayKit_StackLayout(ClayKit_StackConfig cfg) {
    Clay_LayoutConfig layout;
    layout.sizing = cfg.sizing;
    layout.padding.left = cfg.padding;
    layout.padding.right = cfg.padding;
    layout.padding.top = cfg.padding;
    layout.padding.bottom = cfg.padding;
    layout.childGap = cfg.gap;
    layout.childAlignment = cfg.align;
    layout.layoutDirection = (cfg.direction == CLAYKIT_STACK_VERTICAL)
        ? CLAY_TOP_TO_BOTTOM : CLAY_LEFT_TO_RIGHT;
    return layout;
}

/* Create Clay_LayoutConfig for Center */
static inline Clay_LayoutConfig ClayKit_CenterLayout(Clay_Sizing sizing) {
    Clay_LayoutConfig layout;
    layout.sizing = sizing;
    layout.padding.left = 0;
    layout.padding.right = 0;
    layout.padding.top = 0;
    layout.padding.bottom = 0;
    layout.childGap = 0;
    layout.childAlignment.x = CLAY_ALIGN_X_CENTER;
    layout.childAlignment.y = CLAY_ALIGN_Y_CENTER;
    layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    return layout;
}

/* Create Clay_LayoutConfig for Container */
static inline Clay_LayoutConfig ClayKit_ContainerLayout(ClayKit_ContainerConfig cfg) {
    uint16_t max_w = cfg.max_width > 0 ? cfg.max_width : 1200;
    Clay_LayoutConfig layout;
    layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    layout.sizing.width.size.minMax.max = (float)max_w;
    layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    layout.padding.left = cfg.padding;
    layout.padding.right = cfg.padding;
    layout.padding.top = 0;
    layout.padding.bottom = 0;
    layout.childGap = 0;
    layout.childAlignment.x = CLAY_ALIGN_X_LEFT;
    layout.childAlignment.y = CLAY_ALIGN_Y_TOP;
    layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    return layout;
}

/* Create Clay_LayoutConfig for Spacer (grows to fill available space) */
static inline Clay_LayoutConfig ClayKit_SpacerLayout(void) {
    Clay_LayoutConfig layout;
    layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    layout.sizing.height.type = CLAY__SIZING_TYPE_GROW;
    layout.padding.left = 0;
    layout.padding.right = 0;
    layout.padding.top = 0;
    layout.padding.bottom = 0;
    layout.childGap = 0;
    layout.childAlignment.x = CLAY_ALIGN_X_LEFT;
    layout.childAlignment.y = CLAY_ALIGN_Y_TOP;
    layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    return layout;
}

/* ============================================================================
 * Layout Primitives - Macros
 * These wrap CLAY() to provide ClayKit-style components
 * ============================================================================ */

/* CLAYKIT_BOX: Basic container with optional background, border, radius */
#define CLAYKIT_BOX(id, cfg) \
    CLAY(id, { \
        .layout = ClayKit_BoxLayout(cfg), \
        .backgroundColor = (cfg).bg, \
        .cornerRadius = CLAY_CORNER_RADIUS((cfg).radius), \
        .border = { .color = (cfg).border_color, .width = CLAY_BORDER_ALL((cfg).border_width) } \
    })

/* CLAYKIT_FLEX: Flexible container with direction and gap */
#define CLAYKIT_FLEX(id, cfg) \
    CLAY(id, { \
        .layout = ClayKit_FlexLayout(cfg), \
        .backgroundColor = (cfg).bg \
    })

/* CLAYKIT_VSTACK: Vertical stack (top to bottom) */
#define CLAYKIT_VSTACK(id, cfg) \
    CLAY(id, { \
        .layout = ClayKit_StackLayout((ClayKit_StackConfig){ \
            .direction = CLAYKIT_STACK_VERTICAL, \
            .gap = (cfg).gap, \
            .align = (cfg).align, \
            .padding = (cfg).padding, \
            .sizing = (cfg).sizing, \
            .bg = (cfg).bg \
        }), \
        .backgroundColor = (cfg).bg \
    })

/* CLAYKIT_HSTACK: Horizontal stack (left to right) */
#define CLAYKIT_HSTACK(id, cfg) \
    CLAY(id, { \
        .layout = ClayKit_StackLayout((ClayKit_StackConfig){ \
            .direction = CLAYKIT_STACK_HORIZONTAL, \
            .gap = (cfg).gap, \
            .align = (cfg).align, \
            .padding = (cfg).padding, \
            .sizing = (cfg).sizing, \
            .bg = (cfg).bg \
        }), \
        .backgroundColor = (cfg).bg \
    })

/* CLAYKIT_CENTER: Centers children both horizontally and vertically */
#define CLAYKIT_CENTER(id, sizing) \
    CLAY(id, { .layout = ClayKit_CenterLayout(sizing) })

/* CLAYKIT_CONTAINER: Centered container with max-width */
#define CLAYKIT_CONTAINER(id, cfg) \
    CLAY(id, { \
        .layout = ClayKit_ContainerLayout(cfg), \
        .backgroundColor = (cfg).bg \
    })

/* CLAYKIT_SPACER: Grows to fill available space */
#define CLAYKIT_SPACER(id) \
    CLAY(id, { .layout = ClayKit_SpacerLayout() })

/* CLAYKIT_INPUT: Text input container
 * Usage: CLAYKIT_INPUT(ctx, CLAY_ID("myInput"), cfg, focused) { CLAY_TEXT(...); }
 * The input renders the container - you render the text content inside */
#define CLAYKIT_INPUT(ctx, id, cfg, focused) \
    CLAY(id, { \
        .layout = { \
            .sizing = { \
                .width = ((cfg).width > 0) \
                    ? ((Clay_SizingAxis){ .type = CLAY__SIZING_TYPE_FIXED, .size = { .minMax = { (cfg).width, (cfg).width } } }) \
                    : ((Clay_SizingAxis){ .type = CLAY__SIZING_TYPE_GROW }), \
                .height = CLAY_SIZING_FIT(0) \
            }, \
            .padding = { \
                ClayKit_InputPaddingX(ctx, (cfg).size), \
                ClayKit_InputPaddingX(ctx, (cfg).size), \
                ClayKit_InputPaddingY(ctx, (cfg).size), \
                ClayKit_InputPaddingY(ctx, (cfg).size) \
            } \
        }, \
        .backgroundColor = ((cfg).bg.a != 0) ? (cfg).bg : (ctx)->theme_ptr->bg, \
        .cornerRadius = CLAY_CORNER_RADIUS((ctx)->theme_ptr->radius.sm), \
        .border = { \
            .color = ClayKit_InputBorderColor(ctx, cfg, focused), \
            .width = { 1, 1, 1, 1, 0 } \
        } \
    })

/* CLAYKIT_BUTTON: Interactive button with hover state
 * Usage: CLAYKIT_BUTTON(ctx, CLAY_ID("myBtn"), cfg) { CLAY_TEXT(...); }
 * Use Clay_Hovered() inside the block to check hover state */
#define CLAYKIT_BUTTON(ctx, id, cfg) \
    CLAY(id, { \
        .layout = { \
            .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) }, \
            .padding = { \
                ClayKit_ButtonPaddingX(ctx, (cfg).size), \
                ClayKit_ButtonPaddingX(ctx, (cfg).size), \
                ClayKit_ButtonPaddingY(ctx, (cfg).size), \
                ClayKit_ButtonPaddingY(ctx, (cfg).size) \
            }, \
            .childGap = 8, \
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }, \
            .layoutDirection = CLAY_LEFT_TO_RIGHT \
        }, \
        .backgroundColor = ClayKit_ButtonBgColor(ctx, cfg, Clay_Hovered()), \
        .cornerRadius = CLAY_CORNER_RADIUS(ClayKit_ButtonRadius(ctx, (cfg).size)), \
        .border = { \
            .color = ClayKit_ButtonBorderColor(ctx, cfg), \
            .width = { \
                ClayKit_ButtonBorderWidth(cfg), \
                ClayKit_ButtonBorderWidth(cfg), \
                ClayKit_ButtonBorderWidth(cfg), \
                ClayKit_ButtonBorderWidth(cfg), \
                0 \
            } \
        } \
    })

/* ============================================================================
 * API Function Declarations
 * ============================================================================ */

/* Context & State Management */
void ClayKit_Init(ClayKit_Context *ctx, ClayKit_Theme *theme,
                  ClayKit_State *state_buf, uint32_t state_cap);
ClayKit_State* ClayKit_GetState(ClayKit_Context *ctx, uint32_t id);
ClayKit_State* ClayKit_GetOrCreateState(ClayKit_Context *ctx, uint32_t id);

/* Focus Management */
void ClayKit_SetFocus(ClayKit_Context *ctx, Clay_ElementId id);
void ClayKit_ClearFocus(ClayKit_Context *ctx);
bool ClayKit_HasFocus(ClayKit_Context *ctx, Clay_ElementId id);
bool ClayKit_FocusChanged(ClayKit_Context *ctx);
void ClayKit_FocusNext(ClayKit_Context *ctx);
void ClayKit_FocusPrev(ClayKit_Context *ctx);
void ClayKit_BeginFrame(ClayKit_Context *ctx);

/* Text Input */
bool ClayKit_InputHandleKey(ClayKit_InputState *s, uint32_t key, uint32_t mods);
bool ClayKit_InputHandleChar(ClayKit_InputState *s, uint32_t codepoint);

/* Theme Helpers */
Clay_Color ClayKit_GetSchemeColor(ClayKit_Theme *theme, ClayKit_ColorScheme scheme);
uint16_t ClayKit_GetSpacing(ClayKit_Theme *theme, ClayKit_Size size);
uint16_t ClayKit_GetFontSize(ClayKit_Theme *theme, ClayKit_Size size);
uint16_t ClayKit_GetRadius(ClayKit_Theme *theme, ClayKit_Size size);

/* Typography - these return Clay_TextElementConfig for use with CLAY_TEXT */
Clay_TextElementConfig ClayKit_TextStyle(ClayKit_Context *ctx, ClayKit_TextConfig cfg);
Clay_TextElementConfig ClayKit_HeadingStyle(ClayKit_Context *ctx, ClayKit_HeadingConfig cfg);

/* Compute badge style - returns styling info for custom renderers */
ClayKit_BadgeStyle ClayKit_ComputeBadgeStyle(ClayKit_Context *ctx, ClayKit_BadgeConfig cfg);

/* Badge - renders a badge element, call within a Clay layout context */
void ClayKit_Badge(ClayKit_Context *ctx, Clay_String text, ClayKit_BadgeConfig cfg);

/* Badge with raw string pointer - for FFI compatibility */
void ClayKit_BadgeRaw(ClayKit_Context *ctx, const char *text, int32_t text_len, ClayKit_BadgeConfig cfg);

/* Tag */
ClayKit_TagStyle ClayKit_ComputeTagStyle(ClayKit_Context *ctx, ClayKit_TagConfig cfg);
void ClayKit_Tag(ClayKit_Context *ctx, Clay_String text, ClayKit_TagConfig cfg);
void ClayKit_TagRaw(ClayKit_Context *ctx, const char *text, int32_t text_len, ClayKit_TagConfig cfg);

/* Stat */
ClayKit_StatStyle ClayKit_ComputeStatStyle(ClayKit_Context *ctx, ClayKit_StatConfig cfg);
void ClayKit_Stat(ClayKit_Context *ctx,
                  const char *label, int32_t label_len,
                  const char *value, int32_t value_len,
                  const char *help_text, int32_t help_len,
                  ClayKit_StatConfig cfg);

/* List */
ClayKit_ListStyle ClayKit_ComputeListStyle(ClayKit_Context *ctx, ClayKit_ListConfig cfg);
void ClayKit_ListBegin(ClayKit_Context *ctx, ClayKit_ListConfig cfg);
void ClayKit_ListItemRaw(ClayKit_Context *ctx, const char *text, int32_t text_len,
                         uint32_t index, ClayKit_ListConfig cfg);
void ClayKit_ListEnd(void);

/* Table */
ClayKit_TableStyle ClayKit_ComputeTableStyle(ClayKit_Context *ctx, ClayKit_TableConfig cfg);
void ClayKit_TableBegin(ClayKit_Context *ctx, ClayKit_TableConfig cfg);
void ClayKit_TableHeaderRow(ClayKit_Context *ctx, ClayKit_TableConfig cfg);
void ClayKit_TableRow(ClayKit_Context *ctx, uint32_t row_index, ClayKit_TableConfig cfg);
void ClayKit_TableHeaderCell(ClayKit_Context *ctx, float width_percent, ClayKit_TableConfig cfg);
void ClayKit_TableCell(ClayKit_Context *ctx, float width_percent, uint32_t row_index, ClayKit_TableConfig cfg);
void ClayKit_TableCellEnd(void);
void ClayKit_TableRowEnd(void);
void ClayKit_TableEnd(void);

/* ============================================================================
 * Input Configuration
 * ============================================================================ */

/* ============================================================================
 * Checkbox Configuration
 * ============================================================================ */

typedef struct ClayKit_CheckboxConfig {
    ClayKit_ColorScheme color_scheme;  /* Color when checked */
    ClayKit_Size size;                 /* Size of checkbox */
    bool disabled;                     /* Disabled state */
} ClayKit_CheckboxConfig;

/* ============================================================================
 * Radio Configuration
 * ============================================================================ */

typedef struct ClayKit_RadioConfig {
    ClayKit_ColorScheme color_scheme;  /* Color when selected */
    ClayKit_Size size;                 /* Size of radio button */
    bool disabled;                     /* Disabled state */
} ClayKit_RadioConfig;

/* ============================================================================
 * Switch Configuration
 * ============================================================================ */

typedef struct ClayKit_SwitchConfig {
    ClayKit_ColorScheme color_scheme;  /* Color when on */
    ClayKit_Size size;                 /* Size of switch */
    bool disabled;                     /* Disabled state */
} ClayKit_SwitchConfig;

/* ============================================================================
 * Progress Configuration
 * ============================================================================ */

typedef struct ClayKit_ProgressConfig {
    ClayKit_ColorScheme color_scheme;  /* Color of filled portion */
    ClayKit_Size size;                 /* Height of progress bar */
    bool striped;                      /* Show striped pattern (visual only) */
} ClayKit_ProgressConfig;

/* Progress computed style */
typedef struct ClayKit_ProgressStyle {
    Clay_Color track_color;      /* Background track color */
    Clay_Color fill_color;       /* Filled portion color */
    uint16_t height;             /* Track height in pixels */
    uint16_t corner_radius;      /* Corner radius */
} ClayKit_ProgressStyle;

/* ============================================================================
 * Slider Configuration
 * ============================================================================ */

typedef struct ClayKit_SliderConfig {
    ClayKit_ColorScheme color_scheme;  /* Color of filled portion and thumb */
    ClayKit_Size size;                 /* Height of track */
    float min;                         /* Minimum value */
    float max;                         /* Maximum value */
    bool disabled;                     /* Disabled state */
} ClayKit_SliderConfig;

/* Slider computed style */
typedef struct ClayKit_SliderStyle {
    Clay_Color track_color;      /* Background track color */
    Clay_Color fill_color;       /* Filled portion color */
    Clay_Color thumb_color;      /* Thumb color */
    uint16_t track_height;       /* Track height in pixels */
    uint16_t thumb_size;         /* Thumb diameter in pixels */
    uint16_t corner_radius;      /* Track corner radius */
} ClayKit_SliderStyle;

/* ============================================================================
 * Alert Configuration
 * ============================================================================ */

typedef enum ClayKit_AlertVariant {
    CLAYKIT_ALERT_SUBTLE = 0,    /* Light background, colored border */
    CLAYKIT_ALERT_SOLID = 1,     /* Solid colored background */
    CLAYKIT_ALERT_OUTLINE = 2    /* Transparent with colored border */
} ClayKit_AlertVariant;

typedef struct ClayKit_AlertConfig {
    ClayKit_ColorScheme color_scheme;  /* Alert color (info=primary, success, warning, error) */
    ClayKit_AlertVariant variant;      /* Visual style variant */
    ClayKit_Icon icon;                 /* Optional icon (0 = no icon) */
} ClayKit_AlertConfig;

/* Alert computed style */
typedef struct ClayKit_AlertStyle {
    Clay_Color bg_color;         /* Background color */
    Clay_Color border_color;     /* Border color */
    Clay_Color text_color;       /* Text color */
    Clay_Color icon_color;       /* Icon color */
    uint16_t border_width;       /* Border width */
    uint16_t padding;            /* Inner padding */
    uint16_t corner_radius;      /* Corner radius */
    uint16_t icon_size;          /* Icon size if present */
} ClayKit_AlertStyle;

/* ============================================================================
 * Tooltip Configuration
 * ============================================================================ */

typedef enum ClayKit_TooltipPosition {
    CLAYKIT_TOOLTIP_TOP = 0,
    CLAYKIT_TOOLTIP_BOTTOM = 1,
    CLAYKIT_TOOLTIP_LEFT = 2,
    CLAYKIT_TOOLTIP_RIGHT = 3
} ClayKit_TooltipPosition;

typedef struct ClayKit_TooltipConfig {
    ClayKit_TooltipPosition position;  /* Where to show relative to anchor */
} ClayKit_TooltipConfig;

/* Tooltip computed style */
typedef struct ClayKit_TooltipStyle {
    Clay_Color bg_color;         /* Background color (dark) */
    Clay_Color text_color;       /* Text color (light) */
    uint16_t padding_x;          /* Horizontal padding */
    uint16_t padding_y;          /* Vertical padding */
    uint16_t corner_radius;      /* Corner radius */
    uint16_t font_size;          /* Font size */
} ClayKit_TooltipStyle;

/* ============================================================================
 * Tabs Configuration
 * ============================================================================ */

typedef enum ClayKit_TabsVariant {
    CLAYKIT_TABS_LINE = 0,       /* Underline indicator */
    CLAYKIT_TABS_ENCLOSED = 1,   /* Enclosed/boxed tabs */
    CLAYKIT_TABS_SOFT = 2        /* Soft rounded background */
} ClayKit_TabsVariant;

typedef struct ClayKit_TabsConfig {
    ClayKit_ColorScheme color_scheme;  /* Active tab color */
    ClayKit_TabsVariant variant;       /* Visual style */
    ClayKit_Size size;                 /* Size affects padding/font */
} ClayKit_TabsConfig;

/* Tabs computed style */
typedef struct ClayKit_TabsStyle {
    Clay_Color active_color;     /* Active tab indicator/bg color */
    Clay_Color inactive_color;   /* Inactive tab text color */
    Clay_Color active_text;      /* Active tab text color */
    Clay_Color bg_color;         /* Tab bar background */
    Clay_Color border_color;     /* Border color (for enclosed) */
    uint16_t padding_x;          /* Tab horizontal padding */
    uint16_t padding_y;          /* Tab vertical padding */
    uint16_t font_size;          /* Font size */
    uint16_t indicator_height;   /* Underline height (for line variant) */
    uint16_t corner_radius;      /* Corner radius (for enclosed/soft) */
    uint16_t gap;                /* Gap between tabs */
} ClayKit_TabsStyle;

/* ============================================================================
 * Modal Configuration
 * ============================================================================ */

typedef enum ClayKit_ModalSize {
    CLAYKIT_MODAL_SM = 0,        /* Small modal (400px) */
    CLAYKIT_MODAL_MD = 1,        /* Medium modal (500px) */
    CLAYKIT_MODAL_LG = 2,        /* Large modal (600px) */
    CLAYKIT_MODAL_XL = 3,        /* Extra large modal (800px) */
    CLAYKIT_MODAL_FULL = 4       /* Full width (with margins) */
} ClayKit_ModalSize;

typedef struct ClayKit_ModalConfig {
    ClayKit_ModalSize size;      /* Modal width */
    bool close_on_backdrop;      /* Close when clicking backdrop (default: true) */
    uint16_t z_index;            /* Z-index for stacking (default: 1000) */
} ClayKit_ModalConfig;

/* Modal computed style */
typedef struct ClayKit_ModalStyle {
    Clay_Color backdrop_color;   /* Semi-transparent backdrop */
    Clay_Color bg_color;         /* Modal background */
    Clay_Color border_color;     /* Modal border */
    uint16_t width;              /* Modal width in pixels */
    uint16_t padding;            /* Inner padding */
    uint16_t corner_radius;      /* Corner radius */
    uint16_t z_index;            /* Z-index for floating */
} ClayKit_ModalStyle;

/* ============================================================================
 * Spinner Configuration
 * ============================================================================ */

typedef struct ClayKit_SpinnerConfig {
    ClayKit_ColorScheme color_scheme;  /* Spinner color */
    ClayKit_Size size;                 /* Spinner diameter */
    float speed;                       /* Rotations per second (0 = default 1.0) */
} ClayKit_SpinnerConfig;

/* Spinner computed style */
typedef struct ClayKit_SpinnerStyle {
    Clay_Color color;            /* Spinner color */
    Clay_Color track_color;      /* Track (background circle) color */
    uint16_t diameter;           /* Outer diameter in pixels */
    uint16_t thickness;          /* Line thickness in pixels */
    float speed;                 /* Rotations per second */
} ClayKit_SpinnerStyle;

/* ============================================================================
 * Drawer Configuration
 * ============================================================================ */

typedef enum ClayKit_DrawerSide {
    CLAYKIT_DRAWER_LEFT = 0,
    CLAYKIT_DRAWER_RIGHT = 1,
    CLAYKIT_DRAWER_TOP = 2,
    CLAYKIT_DRAWER_BOTTOM = 3
} ClayKit_DrawerSide;

typedef struct ClayKit_DrawerConfig {
    ClayKit_DrawerSide side;         /* Which edge to slide from */
    uint16_t size;                   /* Width (left/right) or height (top/bottom), 0 = default 300 */
    bool close_on_backdrop;          /* Close when clicking backdrop (default: true) */
    uint16_t z_index;                /* Z-index for stacking (0 = default 1000) */
} ClayKit_DrawerConfig;

/* Drawer computed style */
typedef struct ClayKit_DrawerStyle {
    Clay_Color backdrop_color;   /* Semi-transparent backdrop */
    Clay_Color bg_color;         /* Drawer background */
    Clay_Color border_color;     /* Drawer border */
    uint16_t size;               /* Width or height in pixels */
    uint16_t padding;            /* Inner padding */
    uint16_t z_index;            /* Z-index for floating */
} ClayKit_DrawerStyle;

/* ============================================================================
 * Popover Configuration
 * ============================================================================ */

typedef enum ClayKit_PopoverPosition {
    CLAYKIT_POPOVER_TOP = 0,
    CLAYKIT_POPOVER_BOTTOM = 1,
    CLAYKIT_POPOVER_LEFT = 2,
    CLAYKIT_POPOVER_RIGHT = 3
} ClayKit_PopoverPosition;

typedef struct ClayKit_PopoverConfig {
    ClayKit_PopoverPosition position;  /* Position relative to anchor */
    uint16_t z_index;                  /* Z-index for stacking (0 = default 50) */
} ClayKit_PopoverConfig;

/* Popover computed style */
typedef struct ClayKit_PopoverStyle {
    Clay_Color bg_color;         /* Background color */
    Clay_Color border_color;     /* Border color */
    uint16_t padding;            /* Inner padding */
    uint16_t corner_radius;      /* Corner radius */
    uint16_t z_index;            /* Z-index for floating */
} ClayKit_PopoverStyle;

/* ============================================================================
 * Link Configuration
 * ============================================================================ */

typedef enum ClayKit_LinkVariant {
    CLAYKIT_LINK_UNDERLINE = 0,       /* Always underlined */
    CLAYKIT_LINK_HOVER_UNDERLINE = 1, /* Underline on hover only */
    CLAYKIT_LINK_NONE = 2             /* No underline */
} ClayKit_LinkVariant;

typedef struct ClayKit_LinkConfig {
    ClayKit_ColorScheme color_scheme;  /* Link color */
    ClayKit_Size size;                 /* Font size */
    ClayKit_LinkVariant variant;       /* Underline style */
    bool disabled;                     /* Disabled state */
} ClayKit_LinkConfig;

/* Link computed style */
typedef struct ClayKit_LinkStyle {
    Clay_Color text_color;       /* Normal text color */
    Clay_Color hover_color;      /* Hovered text color */
    Clay_Color disabled_color;   /* Disabled text color */
    uint16_t font_size;          /* Font size */
    uint16_t font_id;            /* Font ID */
    uint16_t underline_height;   /* Underline thickness */
} ClayKit_LinkStyle;

/* ============================================================================
 * Breadcrumb Configuration
 * ============================================================================ */

typedef struct ClayKit_BreadcrumbConfig {
    ClayKit_ColorScheme color_scheme;  /* Color scheme */
    ClayKit_Size size;                 /* Size affects font and gap */
    const char *separator;             /* Separator text (NULL = "/") */
    int32_t separator_len;             /* Length of separator */
} ClayKit_BreadcrumbConfig;

/* Breadcrumb computed style */
typedef struct ClayKit_BreadcrumbStyle {
    Clay_Color link_color;         /* Normal link color */
    Clay_Color hover_color;        /* Hovered link color */
    Clay_Color current_color;      /* Current (last) item color */
    Clay_Color separator_color;    /* Separator text color */
    uint16_t font_size;            /* Font size */
    uint16_t font_id;              /* Font ID */
    uint16_t gap;                  /* Gap between items */
} ClayKit_BreadcrumbStyle;

/* ============================================================================
 * Accordion Configuration
 * ============================================================================ */

typedef enum ClayKit_AccordionVariant {
    CLAYKIT_ACCORDION_BORDERED = 0,   /* Border between items */
    CLAYKIT_ACCORDION_SEPARATED = 1   /* Cards with gap */
} ClayKit_AccordionVariant;

typedef struct ClayKit_AccordionConfig {
    ClayKit_ColorScheme color_scheme;  /* Accent color */
    ClayKit_Size size;                 /* Size affects padding and font */
    ClayKit_AccordionVariant variant;  /* Visual style */
} ClayKit_AccordionConfig;

/* Accordion computed style */
typedef struct ClayKit_AccordionStyle {
    Clay_Color header_bg;          /* Header background */
    Clay_Color header_hover_bg;    /* Header hover background */
    Clay_Color header_text;        /* Header text color */
    Clay_Color active_accent;      /* Accent for open indicator */
    Clay_Color content_bg;         /* Content area background */
    Clay_Color border_color;       /* Border color */
    uint16_t padding_x;            /* Header horizontal padding */
    uint16_t padding_y;            /* Header vertical padding */
    uint16_t content_padding;      /* Content area padding */
    uint16_t font_size;            /* Font size */
    uint16_t font_id;              /* Font ID */
    uint16_t border_width;         /* Border width */
    uint16_t corner_radius;        /* Corner radius (for separated) */
    uint16_t gap;                  /* Gap between items (for separated) */
} ClayKit_AccordionStyle;

/* ============================================================================
 * Menu Configuration
 * ============================================================================ */

typedef struct ClayKit_MenuConfig {
    ClayKit_ColorScheme color_scheme;  /* Color scheme */
    ClayKit_Size size;                 /* Size affects padding and font */
    bool disabled;                     /* Disabled state */
} ClayKit_MenuConfig;

/* Menu computed style */
typedef struct ClayKit_MenuStyle {
    Clay_Color bg_color;           /* Dropdown background */
    Clay_Color border_color;       /* Dropdown border */
    Clay_Color text_color;         /* Item text color */
    Clay_Color disabled_text;      /* Disabled item text */
    Clay_Color hover_bg;           /* Hovered item background */
    Clay_Color separator_color;    /* Separator line color */
    uint16_t padding_x;            /* Item horizontal padding */
    uint16_t padding_y;            /* Item vertical padding */
    uint16_t font_size;            /* Font size */
    uint16_t font_id;              /* Font ID */
    uint16_t corner_radius;        /* Dropdown corner radius */
    uint16_t item_gap;             /* Gap between items */
    uint16_t separator_height;     /* Separator thickness */
    uint16_t dropdown_padding;     /* Dropdown inner padding */
} ClayKit_MenuStyle;

/* ============================================================================
 * Select Configuration
 * ============================================================================ */

typedef struct ClayKit_SelectConfig {
    ClayKit_ColorScheme color_scheme;  /* Color scheme */
    ClayKit_Size size;                 /* Size affects padding and font */
    bool disabled;                     /* Disabled state */
} ClayKit_SelectConfig;

/* Select computed style */
typedef struct ClayKit_SelectStyle {
    Clay_Color bg_color;             /* Trigger background */
    Clay_Color border_color;         /* Trigger border */
    Clay_Color text_color;           /* Trigger text */
    Clay_Color placeholder_color;    /* When no selection */
    Clay_Color dropdown_bg;          /* Dropdown container background */
    Clay_Color dropdown_border;      /* Dropdown border */
    Clay_Color option_hover_bg;      /* Hovered option background */
    uint16_t padding_x;
    uint16_t padding_y;
    uint16_t font_size;
    uint16_t font_id;
    uint16_t corner_radius;
    uint16_t dropdown_gap;           /* Gap between options */
} ClayKit_SelectStyle;

/* ============================================================================
 * Input Configuration
 * ============================================================================ */

typedef struct ClayKit_InputConfig {
    ClayKit_Size size;           /* Size affects padding and font */
    Clay_Color bg;               /* Background color (default: theme bg) */
    Clay_Color border_color;     /* Border color (default: theme border) */
    Clay_Color focus_color;      /* Border color when focused (default: theme primary) */
    Clay_Color text_color;       /* Text color (default: theme fg) */
    Clay_Color placeholder_color; /* Placeholder text color (default: theme muted) */
    Clay_Color cursor_color;     /* Cursor color (default: theme fg) */
    Clay_Color selection_color;  /* Selection background (default: primary with alpha) */
    uint16_t width;              /* Fixed width (0 = grow to fill) */
} ClayKit_InputConfig;

/* Input computed style */
typedef struct ClayKit_InputStyle {
    Clay_Color bg_color;         /* Background color */
    Clay_Color border_color;     /* Border color */
    Clay_Color text_color;       /* Text color */
    Clay_Color placeholder_color; /* Placeholder text color */
    Clay_Color cursor_color;     /* Cursor color */
    Clay_Color selection_color;  /* Selection background color */
    uint16_t padding_x;          /* Horizontal padding */
    uint16_t padding_y;          /* Vertical padding */
    uint16_t font_size;          /* Font size */
    uint16_t font_id;            /* Font ID */
    uint16_t corner_radius;      /* Corner radius */
    uint16_t cursor_width;       /* Cursor width in pixels */
} ClayKit_InputStyle;

/* Button helper functions for computing styles */
Clay_Color ClayKit_ButtonBgColor(ClayKit_Context *ctx, ClayKit_ButtonConfig cfg, bool hovered);
Clay_Color ClayKit_ButtonTextColor(ClayKit_Context *ctx, ClayKit_ButtonConfig cfg);
Clay_Color ClayKit_ButtonBorderColor(ClayKit_Context *ctx, ClayKit_ButtonConfig cfg);
uint16_t ClayKit_ButtonBorderWidth(ClayKit_ButtonConfig cfg);
uint16_t ClayKit_ButtonPaddingX(ClayKit_Context *ctx, ClayKit_Size size);
uint16_t ClayKit_ButtonPaddingY(ClayKit_Context *ctx, ClayKit_Size size);
uint16_t ClayKit_ButtonRadius(ClayKit_Context *ctx, ClayKit_Size size);
uint16_t ClayKit_ButtonFontSize(ClayKit_Context *ctx, ClayKit_Size size);

/* Input helper functions */
uint16_t ClayKit_InputPaddingX(ClayKit_Context *ctx, ClayKit_Size size);
uint16_t ClayKit_InputPaddingY(ClayKit_Context *ctx, ClayKit_Size size);
uint16_t ClayKit_InputFontSize(ClayKit_Context *ctx, ClayKit_Size size);
Clay_Color ClayKit_InputBorderColor(ClayKit_Context *ctx, ClayKit_InputConfig cfg, bool focused);
ClayKit_InputStyle ClayKit_ComputeInputStyle(ClayKit_Context *ctx, ClayKit_InputConfig cfg, bool focused);
float ClayKit_MeasureTextWidth(ClayKit_Context *ctx, const char *text, uint32_t length, uint16_t font_id, uint16_t font_size);
uint32_t ClayKit_InputGetCursorFromX(ClayKit_Context *ctx, const char *text, uint32_t length, uint16_t font_id, uint16_t font_size, float x_offset);

/* Checkbox helper functions */
uint16_t ClayKit_CheckboxSize(ClayKit_Context *ctx, ClayKit_Size size);
Clay_Color ClayKit_CheckboxBgColor(ClayKit_Context *ctx, ClayKit_CheckboxConfig cfg, bool checked, bool hovered);
Clay_Color ClayKit_CheckboxBorderColor(ClayKit_Context *ctx, ClayKit_CheckboxConfig cfg, bool checked);

/* Radio helper functions */
uint16_t ClayKit_RadioSize(ClayKit_Context *ctx, ClayKit_Size size);
Clay_Color ClayKit_RadioBgColor(ClayKit_Context *ctx, ClayKit_RadioConfig cfg, bool selected, bool hovered);
Clay_Color ClayKit_RadioBorderColor(ClayKit_Context *ctx, ClayKit_RadioConfig cfg, bool selected);

/* Switch helper functions */
uint16_t ClayKit_SwitchWidth(ClayKit_Context *ctx, ClayKit_Size size);
uint16_t ClayKit_SwitchHeight(ClayKit_Context *ctx, ClayKit_Size size);
uint16_t ClayKit_SwitchKnobSize(ClayKit_Context *ctx, ClayKit_Size size);
Clay_Color ClayKit_SwitchBgColor(ClayKit_Context *ctx, ClayKit_SwitchConfig cfg, bool on, bool hovered);

/* Progress helper functions */
ClayKit_ProgressStyle ClayKit_ComputeProgressStyle(ClayKit_Context *ctx, ClayKit_ProgressConfig cfg);
void ClayKit_Progress(ClayKit_Context *ctx, float value, ClayKit_ProgressConfig cfg);

/* Slider helper functions */
ClayKit_SliderStyle ClayKit_ComputeSliderStyle(ClayKit_Context *ctx, ClayKit_SliderConfig cfg, bool hovered);

/* Alert helper functions */
ClayKit_AlertStyle ClayKit_ComputeAlertStyle(ClayKit_Context *ctx, ClayKit_AlertConfig cfg);
void ClayKit_AlertText(ClayKit_Context *ctx, const char *text, int32_t text_len, ClayKit_AlertConfig cfg);

/* Tooltip helper functions */
ClayKit_TooltipStyle ClayKit_ComputeTooltipStyle(ClayKit_Context *ctx, ClayKit_TooltipConfig cfg);
void ClayKit_Tooltip(ClayKit_Context *ctx, const char *text, int32_t text_len, ClayKit_TooltipConfig cfg);

/* Tabs helper functions */
ClayKit_TabsStyle ClayKit_ComputeTabsStyle(ClayKit_Context *ctx, ClayKit_TabsConfig cfg);

/* Modal helper functions */
ClayKit_ModalStyle ClayKit_ComputeModalStyle(ClayKit_Context *ctx, ClayKit_ModalConfig cfg);

/* Spinner helper functions */
ClayKit_SpinnerStyle ClayKit_ComputeSpinnerStyle(ClayKit_Context *ctx, ClayKit_SpinnerConfig cfg);
float ClayKit_SpinnerAngle(ClayKit_Context *ctx, ClayKit_SpinnerConfig cfg);
void ClayKit_Spinner(ClayKit_Context *ctx, ClayKit_SpinnerConfig cfg);

/* Drawer helper functions */
ClayKit_DrawerStyle ClayKit_ComputeDrawerStyle(ClayKit_Context *ctx, ClayKit_DrawerConfig cfg);
bool ClayKit_DrawerBegin(ClayKit_Context *ctx, const char *id, int32_t id_len, ClayKit_DrawerConfig cfg);
void ClayKit_DrawerEnd(void);

/* Popover helper functions */
ClayKit_PopoverStyle ClayKit_ComputePopoverStyle(ClayKit_Context *ctx, ClayKit_PopoverConfig cfg);
void ClayKit_PopoverBegin(ClayKit_Context *ctx, const char *id, int32_t id_len, ClayKit_PopoverConfig cfg);
void ClayKit_PopoverEnd(void);

/* Button rendering - returns true if hovered */
bool ClayKit_Button(ClayKit_Context *ctx, const char *text, int32_t text_len, ClayKit_ButtonConfig cfg);

/* Checkbox rendering - returns true if hovered */
bool ClayKit_Checkbox(ClayKit_Context *ctx, bool checked, ClayKit_CheckboxConfig cfg);

/* Radio rendering - returns true if hovered */
bool ClayKit_Radio(ClayKit_Context *ctx, bool selected, ClayKit_RadioConfig cfg);

/* Switch rendering - returns true if hovered */
bool ClayKit_Switch(ClayKit_Context *ctx, bool on, ClayKit_SwitchConfig cfg);

/* Slider rendering - returns true if hovered */
bool ClayKit_Slider(ClayKit_Context *ctx, float value, ClayKit_SliderConfig cfg);

/* Single tab rendering - returns true if hovered */
bool ClayKit_Tab(ClayKit_Context *ctx, const char *label, int32_t label_len, bool is_active, ClayKit_TabsConfig cfg);

/* Link helper functions */
ClayKit_LinkStyle ClayKit_ComputeLinkStyle(ClayKit_Context *ctx, ClayKit_LinkConfig cfg);
bool ClayKit_Link(ClayKit_Context *ctx, const char *text, int32_t text_len, ClayKit_LinkConfig cfg);

/* Breadcrumb helper functions */
ClayKit_BreadcrumbStyle ClayKit_ComputeBreadcrumbStyle(ClayKit_Context *ctx, ClayKit_BreadcrumbConfig cfg);
void ClayKit_BreadcrumbBegin(ClayKit_Context *ctx, ClayKit_BreadcrumbConfig cfg);
bool ClayKit_BreadcrumbItem(ClayKit_Context *ctx, const char *text, int32_t text_len, bool is_current, ClayKit_BreadcrumbConfig cfg);
void ClayKit_BreadcrumbSeparator(ClayKit_Context *ctx, ClayKit_BreadcrumbConfig cfg);
void ClayKit_BreadcrumbEnd(void);

/* Accordion helper functions */
ClayKit_AccordionStyle ClayKit_ComputeAccordionStyle(ClayKit_Context *ctx, ClayKit_AccordionConfig cfg);
void ClayKit_AccordionBegin(ClayKit_Context *ctx, ClayKit_AccordionConfig cfg);
void ClayKit_AccordionItemBegin(ClayKit_Context *ctx, bool is_open, ClayKit_AccordionConfig cfg);
void ClayKit_AccordionItemEnd(void);
bool ClayKit_AccordionHeader(ClayKit_Context *ctx, const char *text, int32_t text_len, bool is_open, ClayKit_AccordionConfig cfg);
void ClayKit_AccordionContentBegin(ClayKit_Context *ctx, ClayKit_AccordionConfig cfg);
void ClayKit_AccordionContentEnd(void);
void ClayKit_AccordionEnd(void);

/* Menu helper functions */
ClayKit_MenuStyle ClayKit_ComputeMenuStyle(ClayKit_Context *ctx, ClayKit_MenuConfig cfg);
void ClayKit_MenuDropdownBegin(ClayKit_Context *ctx, const char *id, int32_t id_len, ClayKit_MenuConfig cfg);
bool ClayKit_MenuItem(ClayKit_Context *ctx, const char *text, int32_t text_len, bool disabled, ClayKit_MenuConfig cfg);
void ClayKit_MenuSeparator(ClayKit_Context *ctx, ClayKit_MenuConfig cfg);
void ClayKit_MenuDropdownEnd(void);

/* Select helper functions */
ClayKit_SelectStyle ClayKit_ComputeSelectStyle(ClayKit_Context *ctx, ClayKit_SelectConfig cfg);
bool ClayKit_SelectTrigger(ClayKit_Context *ctx, const char *id, int32_t id_len,
                           const char *display_text, int32_t display_len,
                           ClayKit_SelectConfig cfg);
void ClayKit_SelectDropdownBegin(ClayKit_Context *ctx, const char *id, int32_t id_len,
                                 ClayKit_SelectConfig cfg);
bool ClayKit_SelectOption(ClayKit_Context *ctx, const char *text, int32_t text_len,
                          bool is_selected, ClayKit_SelectConfig cfg);
void ClayKit_SelectDropdownEnd(void);

/* Text input rendering - renders input box with text, cursor, and optional placeholder
 * id/id_len: element ID for later lookup via Clay_GetElementData
 * Returns true if hovered (for click detection to set focus) */
bool ClayKit_TextInput(ClayKit_Context *ctx, const char *id, int32_t id_len,
                       ClayKit_InputState *state, ClayKit_InputConfig cfg,
                       const char *placeholder, int32_t placeholder_len);

/* ============================================================================
 * Theme Presets (defined in implementation)
 * ============================================================================ */

extern const ClayKit_Theme CLAYKIT_THEME_LIGHT;
extern const ClayKit_Theme CLAYKIT_THEME_DARK;

/* ============================================================================
 * Implementation
 * ============================================================================ */

#ifdef CLAYKIT_IMPLEMENTATION

/* ----------------------------------------------------------------------------
 * Theme Presets
 * ---------------------------------------------------------------------------- */

const ClayKit_Theme CLAYKIT_THEME_LIGHT = {
    /* Color palette */
    .primary   = { 66, 133, 244, 255 },   /* Blue */
    .secondary = { 156, 163, 175, 255 },  /* Gray */
    .success   = { 34, 197, 94, 255 },    /* Green */
    .warning   = { 251, 191, 36, 255 },   /* Amber */
    .error     = { 239, 68, 68, 255 },    /* Red */

    /* Semantic colors */
    .bg     = { 255, 255, 255, 255 },  /* White */
    .fg     = { 17, 24, 39, 255 },     /* Gray-900 */
    .border = { 229, 231, 235, 255 },  /* Gray-200 */
    .muted  = { 107, 114, 128, 255 },  /* Gray-500 */

    /* Spacing scale */
    .spacing = { .xs = 4, .sm = 8, .md = 16, .lg = 24, .xl = 32 },

    /* Border radius scale */
    .radius = { .sm = 4, .md = 8, .lg = 12, .full = 9999 },

    /* Font IDs (user assigns actual font IDs) */
    .font_id = { .body = 0, .heading = 0 },

    /* Font size scale */
    .font_size = { .xs = 12, .sm = 14, .md = 16, .lg = 18, .xl = 24 }
};

const ClayKit_Theme CLAYKIT_THEME_DARK = {
    /* Color palette */
    .primary   = { 96, 165, 250, 255 },   /* Blue-400 */
    .secondary = { 156, 163, 175, 255 },  /* Gray-400 */
    .success   = { 74, 222, 128, 255 },   /* Green-400 */
    .warning   = { 251, 191, 36, 255 },   /* Amber-400 */
    .error     = { 248, 113, 113, 255 },  /* Red-400 */

    /* Semantic colors */
    .bg     = { 17, 24, 39, 255 },     /* Gray-900 */
    .fg     = { 249, 250, 251, 255 },  /* Gray-50 */
    .border = { 55, 65, 81, 255 },     /* Gray-700 */
    .muted  = { 156, 163, 175, 255 },  /* Gray-400 */

    /* Spacing scale */
    .spacing = { .xs = 4, .sm = 8, .md = 16, .lg = 24, .xl = 32 },

    /* Border radius scale */
    .radius = { .sm = 4, .md = 8, .lg = 12, .full = 9999 },

    /* Font IDs (user assigns actual font IDs) */
    .font_id = { .body = 0, .heading = 0 },

    /* Font size scale */
    .font_size = { .xs = 12, .sm = 14, .md = 16, .lg = 18, .xl = 24 }
};

/* ----------------------------------------------------------------------------
 * Context & State Management
 * ---------------------------------------------------------------------------- */

void ClayKit_Init(ClayKit_Context *ctx, ClayKit_Theme *theme,
                  ClayKit_State *state_buf, uint32_t state_cap) {
    ctx->theme_ptr = theme;
    ctx->state_ptr = state_buf;
    ctx->state_count = 0;
    ctx->state_cap = state_cap;
    ctx->focused_id = 0;
    ctx->prev_focused_id = 0;
    ctx->icon_callback = NULL;
    ctx->icon_user_data = NULL;

    /* Zero out state buffer */
    for (uint32_t i = 0; i < state_cap; i++) {
        state_buf[i].id = 0;
        state_buf[i].flags = 0;
        state_buf[i].value = 0.0f;
    }
}

ClayKit_State* ClayKit_GetState(ClayKit_Context *ctx, uint32_t id) {
    for (uint32_t i = 0; i < ctx->state_count; i++) {
        if (ctx->state_ptr[i].id == id) {
            return &ctx->state_ptr[i];
        }
    }
    return NULL;
}

ClayKit_State* ClayKit_GetOrCreateState(ClayKit_Context *ctx, uint32_t id) {
    /* First, try to find existing */
    ClayKit_State *existing = ClayKit_GetState(ctx, id);
    if (existing) {
        return existing;
    }

    /* Create new if we have capacity */
    if (ctx->state_count < ctx->state_cap) {
        ClayKit_State *s = &ctx->state_ptr[ctx->state_count];
        s->id = id;
        s->flags = 0;
        s->value = 0.0f;
        ctx->state_count++;
        return s;
    }

    /* Out of state slots */
    return NULL;
}

/* ----------------------------------------------------------------------------
 * Focus Management
 * ---------------------------------------------------------------------------- */

void ClayKit_BeginFrame(ClayKit_Context *ctx) {
    ctx->prev_focused_id = ctx->focused_id;
}

void ClayKit_SetFocus(ClayKit_Context *ctx, Clay_ElementId id) {
    ctx->focused_id = id.id;
}

void ClayKit_ClearFocus(ClayKit_Context *ctx) {
    ctx->focused_id = 0;
}

bool ClayKit_HasFocus(ClayKit_Context *ctx, Clay_ElementId id) {
    return ctx->focused_id == id.id;
}

bool ClayKit_FocusChanged(ClayKit_Context *ctx) {
    return ctx->focused_id != ctx->prev_focused_id;
}

void ClayKit_FocusNext(ClayKit_Context *ctx) {
    /* TODO: Implement tab navigation through focusable elements */
    (void)ctx;
}

void ClayKit_FocusPrev(ClayKit_Context *ctx) {
    /* TODO: Implement shift+tab navigation through focusable elements */
    (void)ctx;
}

/* ----------------------------------------------------------------------------
 * Theme Helpers
 * ---------------------------------------------------------------------------- */

Clay_Color ClayKit_GetSchemeColor(ClayKit_Theme *theme, ClayKit_ColorScheme scheme) {
    switch (scheme) {
        case CLAYKIT_COLOR_PRIMARY:   return theme->primary;
        case CLAYKIT_COLOR_SECONDARY: return theme->secondary;
        case CLAYKIT_COLOR_SUCCESS:   return theme->success;
        case CLAYKIT_COLOR_WARNING:   return theme->warning;
        case CLAYKIT_COLOR_ERROR:     return theme->error;
        default:                      return theme->primary;
    }
}

uint16_t ClayKit_GetSpacing(ClayKit_Theme *theme, ClayKit_Size size) {
    switch (size) {
        case CLAYKIT_SIZE_XS: return theme->spacing.xs;
        case CLAYKIT_SIZE_SM: return theme->spacing.sm;
        case CLAYKIT_SIZE_MD: return theme->spacing.md;
        case CLAYKIT_SIZE_LG: return theme->spacing.lg;
        case CLAYKIT_SIZE_XL: return theme->spacing.xl;
        default:              return theme->spacing.md;
    }
}

uint16_t ClayKit_GetFontSize(ClayKit_Theme *theme, ClayKit_Size size) {
    switch (size) {
        case CLAYKIT_SIZE_XS: return theme->font_size.xs;
        case CLAYKIT_SIZE_SM: return theme->font_size.sm;
        case CLAYKIT_SIZE_MD: return theme->font_size.md;
        case CLAYKIT_SIZE_LG: return theme->font_size.lg;
        case CLAYKIT_SIZE_XL: return theme->font_size.xl;
        default:              return theme->font_size.md;
    }
}

uint16_t ClayKit_GetRadius(ClayKit_Theme *theme, ClayKit_Size size) {
    switch (size) {
        case CLAYKIT_SIZE_XS: return theme->radius.sm;
        case CLAYKIT_SIZE_SM: return theme->radius.sm;
        case CLAYKIT_SIZE_MD: return theme->radius.md;
        case CLAYKIT_SIZE_LG: return theme->radius.lg;
        case CLAYKIT_SIZE_XL: return theme->radius.lg;
        default:              return theme->radius.md;
    }
}

/* ----------------------------------------------------------------------------
 * Text Input Handling
 * ---------------------------------------------------------------------------- */

static uint32_t claykit_min_u32(uint32_t a, uint32_t b) {
    return a < b ? a : b;
}

static uint32_t claykit_max_u32(uint32_t a, uint32_t b) {
    return a > b ? a : b;
}

static void claykit_input_delete_selection(ClayKit_InputState *s) {
    if (s->cursor == s->select_start) return;

    uint32_t start = claykit_min_u32(s->cursor, s->select_start);
    uint32_t end = claykit_max_u32(s->cursor, s->select_start);
    uint32_t del_len = end - start;

    /* Shift remaining text left */
    for (uint32_t i = start; i < s->len - del_len; i++) {
        s->buf[i] = s->buf[i + del_len];
    }
    s->len -= del_len;
    s->cursor = start;
    s->select_start = start;
}

bool ClayKit_InputHandleKey(ClayKit_InputState *s, uint32_t key, uint32_t mods) {
    bool shift = (mods & CLAYKIT_MOD_SHIFT) != 0;
    bool ctrl = (mods & CLAYKIT_MOD_CTRL) != 0;
    bool changed = false;

    switch (key) {
        case CLAYKIT_KEY_BACKSPACE:
            if (s->cursor != s->select_start) {
                claykit_input_delete_selection(s);
                changed = true;
            } else if (s->cursor > 0) {
                /* Delete char before cursor */
                for (uint32_t i = s->cursor - 1; i < s->len - 1; i++) {
                    s->buf[i] = s->buf[i + 1];
                }
                s->len--;
                s->cursor--;
                s->select_start = s->cursor;
                changed = true;
            }
            break;

        case CLAYKIT_KEY_DELETE:
            if (s->cursor != s->select_start) {
                claykit_input_delete_selection(s);
                changed = true;
            } else if (s->cursor < s->len) {
                /* Delete char at cursor */
                for (uint32_t i = s->cursor; i < s->len - 1; i++) {
                    s->buf[i] = s->buf[i + 1];
                }
                s->len--;
                changed = true;
            }
            break;

        case CLAYKIT_KEY_LEFT:
            if (ctrl) {
                /* Move to start of previous word */
                while (s->cursor > 0 && s->buf[s->cursor - 1] == ' ') s->cursor--;
                while (s->cursor > 0 && s->buf[s->cursor - 1] != ' ') s->cursor--;
            } else if (s->cursor > 0) {
                s->cursor--;
            }
            if (!shift) s->select_start = s->cursor;
            changed = true;
            break;

        case CLAYKIT_KEY_RIGHT:
            if (ctrl) {
                /* Move to end of next word */
                while (s->cursor < s->len && s->buf[s->cursor] != ' ') s->cursor++;
                while (s->cursor < s->len && s->buf[s->cursor] == ' ') s->cursor++;
            } else if (s->cursor < s->len) {
                s->cursor++;
            }
            if (!shift) s->select_start = s->cursor;
            changed = true;
            break;

        case CLAYKIT_KEY_HOME:
            s->cursor = 0;
            if (!shift) s->select_start = s->cursor;
            changed = true;
            break;

        case CLAYKIT_KEY_END:
            s->cursor = s->len;
            if (!shift) s->select_start = s->cursor;
            changed = true;
            break;

        default:
            break;
    }

    return changed;
}

bool ClayKit_InputHandleChar(ClayKit_InputState *s, uint32_t codepoint) {
    /* Only handle basic ASCII for now */
    if (codepoint < 32 || codepoint > 126) return false;

    /* Delete selection first if present */
    if (s->cursor != s->select_start) {
        claykit_input_delete_selection(s);
    }

    /* Check capacity */
    if (s->len >= s->cap - 1) return false;

    /* Insert character at cursor */
    for (uint32_t i = s->len; i > s->cursor; i--) {
        s->buf[i] = s->buf[i - 1];
    }
    s->buf[s->cursor] = (char)codepoint;
    s->len++;
    s->cursor++;
    s->select_start = s->cursor;

    return true;
}

/* ----------------------------------------------------------------------------
 * Typography
 * ---------------------------------------------------------------------------- */

Clay_TextElementConfig ClayKit_TextStyle(ClayKit_Context *ctx, ClayKit_TextConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    Clay_TextElementConfig config = {0};

    config.fontSize = cfg.size != 0
        ? ClayKit_GetFontSize(theme, cfg.size)
        : theme->font_size.md;

    config.fontId = cfg.font_id != 0 ? cfg.font_id : theme->font_id.body;

    config.textColor = (cfg.color.a != 0) ? cfg.color : theme->fg;

    config.letterSpacing = cfg.letter_spacing;
    config.lineHeight = cfg.line_height;
    config.wrapMode = CLAY_TEXT_WRAP_WORDS;

    return config;
}

Clay_TextElementConfig ClayKit_HeadingStyle(ClayKit_Context *ctx, ClayKit_HeadingConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    Clay_TextElementConfig config = {0};

    /* Heading sizes: XL=h1 (largest), LG=h2, MD=h3, SM=h4, XS=h5/h6 */
    uint16_t size;
    switch (cfg.size) {
        case CLAYKIT_SIZE_XS: size = theme->font_size.md; break;   /* h5/h6 */
        case CLAYKIT_SIZE_SM: size = theme->font_size.lg; break;   /* h4 */
        case CLAYKIT_SIZE_MD: size = theme->font_size.xl; break;   /* h3 */
        case CLAYKIT_SIZE_LG: size = theme->font_size.xl + 4; break; /* h2 */
        case CLAYKIT_SIZE_XL: size = theme->font_size.xl + 8; break; /* h1 */
        default:              size = theme->font_size.xl; break;
    }
    config.fontSize = size;

    config.fontId = cfg.font_id != 0 ? cfg.font_id : theme->font_id.heading;

    config.textColor = (cfg.color.a != 0) ? cfg.color : theme->fg;

    config.letterSpacing = 0;
    config.lineHeight = 0;
    config.wrapMode = CLAY_TEXT_WRAP_WORDS;

    return config;
}

/* Helper to create lighter variant of colors */
static Clay_Color claykit_color_lighten(Clay_Color c, float amount) {
    float r = c.r + (255.0f - c.r) * amount;
    float g = c.g + (255.0f - c.g) * amount;
    float b = c.b + (255.0f - c.b) * amount;
    return (Clay_Color){
        (r > 255.0f) ? 255.0f : r,
        (g > 255.0f) ? 255.0f : g,
        (b > 255.0f) ? 255.0f : b,
        c.a
    };
}

/* ----------------------------------------------------------------------------
 * Button Helper Functions
 * ---------------------------------------------------------------------------- */

static Clay_Color claykit_color_darken(Clay_Color c, float amount) {
    return (Clay_Color){
        c.r * (1.0f - amount),
        c.g * (1.0f - amount),
        c.b * (1.0f - amount),
        c.a
    };
}

uint16_t ClayKit_ButtonPaddingX(ClayKit_Context *ctx, ClayKit_Size size) {
    (void)ctx;
    switch (size) {
        case CLAYKIT_SIZE_XS: return 8;
        case CLAYKIT_SIZE_SM: return 12;
        case CLAYKIT_SIZE_LG: return 20;
        case CLAYKIT_SIZE_XL: return 24;
        case CLAYKIT_SIZE_MD:
        default: return 16;
    }
}

uint16_t ClayKit_ButtonPaddingY(ClayKit_Context *ctx, ClayKit_Size size) {
    (void)ctx;
    switch (size) {
        case CLAYKIT_SIZE_XS: return 4;
        case CLAYKIT_SIZE_SM: return 6;
        case CLAYKIT_SIZE_LG: return 12;
        case CLAYKIT_SIZE_XL: return 14;
        case CLAYKIT_SIZE_MD:
        default: return 8;
    }
}

uint16_t ClayKit_ButtonRadius(ClayKit_Context *ctx, ClayKit_Size size) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    switch (size) {
        case CLAYKIT_SIZE_XS:
        case CLAYKIT_SIZE_SM: return theme->radius.sm;
        case CLAYKIT_SIZE_LG:
        case CLAYKIT_SIZE_XL: return theme->radius.lg;
        case CLAYKIT_SIZE_MD:
        default: return theme->radius.md;
    }
}

uint16_t ClayKit_ButtonFontSize(ClayKit_Context *ctx, ClayKit_Size size) {
    return ClayKit_GetFontSize(ctx->theme_ptr, size);
}

Clay_Color ClayKit_ButtonBgColor(ClayKit_Context *ctx, ClayKit_ButtonConfig cfg, bool hovered) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);

    if (cfg.disabled) {
        return theme->border;
    }

    switch (cfg.variant) {
        case CLAYKIT_BUTTON_SOLID:
            return hovered ? claykit_color_darken(scheme_color, 0.1f) : scheme_color;
        case CLAYKIT_BUTTON_OUTLINE:
        case CLAYKIT_BUTTON_GHOST:
            return hovered ? claykit_color_lighten(scheme_color, 0.9f) : (Clay_Color){ 0, 0, 0, 0 };
        default:
            return scheme_color;
    }
}

Clay_Color ClayKit_ButtonTextColor(ClayKit_Context *ctx, ClayKit_ButtonConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);

    if (cfg.disabled) {
        return theme->muted;
    }

    switch (cfg.variant) {
        case CLAYKIT_BUTTON_SOLID:
            return (Clay_Color){ 255, 255, 255, 255 };
        case CLAYKIT_BUTTON_OUTLINE:
        case CLAYKIT_BUTTON_GHOST:
            return scheme_color;
        default:
            return (Clay_Color){ 255, 255, 255, 255 };
    }
}

Clay_Color ClayKit_ButtonBorderColor(ClayKit_Context *ctx, ClayKit_ButtonConfig cfg) {
    if (cfg.disabled || cfg.variant != CLAYKIT_BUTTON_OUTLINE) {
        return (Clay_Color){ 0, 0, 0, 0 };
    }
    return ClayKit_GetSchemeColor(ctx->theme_ptr, cfg.color_scheme);
}

uint16_t ClayKit_ButtonBorderWidth(ClayKit_ButtonConfig cfg) {
    return (cfg.variant == CLAYKIT_BUTTON_OUTLINE && !cfg.disabled) ? 1 : 0;
}

/* ----------------------------------------------------------------------------
 * Input Helper Functions
 * ---------------------------------------------------------------------------- */

uint16_t ClayKit_InputPaddingX(ClayKit_Context *ctx, ClayKit_Size size) {
    (void)ctx;
    switch (size) {
        case CLAYKIT_SIZE_XS: return 6;
        case CLAYKIT_SIZE_SM: return 8;
        case CLAYKIT_SIZE_LG: return 14;
        case CLAYKIT_SIZE_XL: return 16;
        case CLAYKIT_SIZE_MD:
        default: return 12;
    }
}

uint16_t ClayKit_InputPaddingY(ClayKit_Context *ctx, ClayKit_Size size) {
    (void)ctx;
    switch (size) {
        case CLAYKIT_SIZE_XS: return 4;
        case CLAYKIT_SIZE_SM: return 6;
        case CLAYKIT_SIZE_LG: return 10;
        case CLAYKIT_SIZE_XL: return 12;
        case CLAYKIT_SIZE_MD:
        default: return 8;
    }
}

uint16_t ClayKit_InputFontSize(ClayKit_Context *ctx, ClayKit_Size size) {
    return ClayKit_GetFontSize(ctx->theme_ptr, size);
}

Clay_Color ClayKit_InputBorderColor(ClayKit_Context *ctx, ClayKit_InputConfig cfg, bool focused) {
    ClayKit_Theme *theme = ctx->theme_ptr;

    if (focused) {
        return (cfg.focus_color.a != 0) ? cfg.focus_color : theme->primary;
    }
    return (cfg.border_color.a != 0) ? cfg.border_color : theme->border;
}

ClayKit_InputStyle ClayKit_ComputeInputStyle(ClayKit_Context *ctx, ClayKit_InputConfig cfg, bool focused) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_InputStyle style;

    /* Background */
    style.bg_color = (cfg.bg.a != 0) ? cfg.bg : theme->bg;

    /* Border */
    style.border_color = ClayKit_InputBorderColor(ctx, cfg, focused);

    /* Text colors */
    style.text_color = (cfg.text_color.a != 0) ? cfg.text_color : theme->fg;
    style.placeholder_color = (cfg.placeholder_color.a != 0) ? cfg.placeholder_color : theme->muted;

    /* Cursor - defaults to text color */
    style.cursor_color = (cfg.cursor_color.a != 0) ? cfg.cursor_color : style.text_color;

    /* Selection - defaults to primary with transparency */
    if (cfg.selection_color.a != 0) {
        style.selection_color = cfg.selection_color;
    } else {
        style.selection_color = theme->primary;
        style.selection_color.a = 77; /* ~30% opacity */
    }

    /* Sizing */
    style.padding_x = ClayKit_InputPaddingX(ctx, cfg.size);
    style.padding_y = ClayKit_InputPaddingY(ctx, cfg.size);
    style.font_size = ClayKit_InputFontSize(ctx, cfg.size);
    style.font_id = theme->font_id.body;
    style.corner_radius = theme->radius.sm;
    style.cursor_width = 2;

    return style;
}

float ClayKit_MeasureTextWidth(ClayKit_Context *ctx, const char *text, uint32_t length, uint16_t font_id, uint16_t font_size) {
    if (ctx->measure_text == NULL || length == 0) {
        return 0.0f;
    }
    ClayKit_TextDimensions dims = ctx->measure_text(text, length, font_id, font_size, ctx->measure_text_user_data);
    return dims.width;
}

uint32_t ClayKit_InputGetCursorFromX(ClayKit_Context *ctx, const char *text, uint32_t length, uint16_t font_id, uint16_t font_size, float x_offset) {
    if (ctx->measure_text == NULL || length == 0 || x_offset <= 0) {
        return 0;
    }

    /* Binary search would be more efficient, but linear is simpler and text inputs are usually short */
    float prev_width = 0.0f;
    for (uint32_t i = 1; i <= length; i++) {
        float width = ClayKit_MeasureTextWidth(ctx, text, i, font_id, font_size);
        /* Check if click is closer to this position or the previous one */
        float mid = (prev_width + width) / 2.0f;
        if (x_offset < mid) {
            return i - 1;
        }
        prev_width = width;
    }
    return length;
}

/* ----------------------------------------------------------------------------
 * Checkbox Helper Functions
 * ---------------------------------------------------------------------------- */

uint16_t ClayKit_CheckboxSize(ClayKit_Context *ctx, ClayKit_Size size) {
    (void)ctx;
    switch (size) {
        case CLAYKIT_SIZE_XS: return 14;
        case CLAYKIT_SIZE_SM: return 16;
        case CLAYKIT_SIZE_LG: return 22;
        case CLAYKIT_SIZE_XL: return 26;
        case CLAYKIT_SIZE_MD:
        default: return 18;
    }
}

Clay_Color ClayKit_CheckboxBgColor(ClayKit_Context *ctx, ClayKit_CheckboxConfig cfg, bool checked, bool hovered) {
    ClayKit_Theme *theme = ctx->theme_ptr;

    if (cfg.disabled) {
        return checked ? theme->muted : theme->border;
    }

    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);

    if (checked) {
        return hovered ? claykit_color_darken(scheme_color, 0.1f) : scheme_color;
    }

    return hovered ? claykit_color_lighten(scheme_color, 0.95f) : theme->bg;
}

Clay_Color ClayKit_CheckboxBorderColor(ClayKit_Context *ctx, ClayKit_CheckboxConfig cfg, bool checked) {
    ClayKit_Theme *theme = ctx->theme_ptr;

    if (cfg.disabled) {
        return theme->muted;
    }

    if (checked) {
        return ClayKit_GetSchemeColor(theme, cfg.color_scheme);
    }

    return theme->border;
}

/* ----------------------------------------------------------------------------
 * Radio Helper Functions
 * ---------------------------------------------------------------------------- */

uint16_t ClayKit_RadioSize(ClayKit_Context *ctx, ClayKit_Size size) {
    /* Same dimensions as Checkbox */
    return ClayKit_CheckboxSize(ctx, size);
}

Clay_Color ClayKit_RadioBgColor(ClayKit_Context *ctx, ClayKit_RadioConfig cfg, bool selected, bool hovered) {
    ClayKit_Theme *theme = ctx->theme_ptr;

    if (cfg.disabled) {
        return selected ? theme->muted : theme->border;
    }

    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);

    if (selected) {
        return hovered ? claykit_color_darken(scheme_color, 0.1f) : scheme_color;
    }

    return hovered ? claykit_color_lighten(scheme_color, 0.95f) : theme->bg;
}

Clay_Color ClayKit_RadioBorderColor(ClayKit_Context *ctx, ClayKit_RadioConfig cfg, bool selected) {
    ClayKit_Theme *theme = ctx->theme_ptr;

    if (cfg.disabled) {
        return theme->muted;
    }

    if (selected) {
        return ClayKit_GetSchemeColor(theme, cfg.color_scheme);
    }

    return theme->border;
}

/* ----------------------------------------------------------------------------
 * Switch Helper Functions
 * ---------------------------------------------------------------------------- */

uint16_t ClayKit_SwitchWidth(ClayKit_Context *ctx, ClayKit_Size size) {
    (void)ctx;
    switch (size) {
        case CLAYKIT_SIZE_XS: return 28;
        case CLAYKIT_SIZE_SM: return 34;
        case CLAYKIT_SIZE_LG: return 50;
        case CLAYKIT_SIZE_XL: return 58;
        case CLAYKIT_SIZE_MD:
        default: return 42;
    }
}

uint16_t ClayKit_SwitchHeight(ClayKit_Context *ctx, ClayKit_Size size) {
    (void)ctx;
    switch (size) {
        case CLAYKIT_SIZE_XS: return 16;
        case CLAYKIT_SIZE_SM: return 20;
        case CLAYKIT_SIZE_LG: return 28;
        case CLAYKIT_SIZE_XL: return 32;
        case CLAYKIT_SIZE_MD:
        default: return 24;
    }
}

uint16_t ClayKit_SwitchKnobSize(ClayKit_Context *ctx, ClayKit_Size size) {
    (void)ctx;
    switch (size) {
        case CLAYKIT_SIZE_XS: return 12;
        case CLAYKIT_SIZE_SM: return 16;
        case CLAYKIT_SIZE_LG: return 24;
        case CLAYKIT_SIZE_XL: return 28;
        case CLAYKIT_SIZE_MD:
        default: return 20;
    }
}

Clay_Color ClayKit_SwitchBgColor(ClayKit_Context *ctx, ClayKit_SwitchConfig cfg, bool on, bool hovered) {
    ClayKit_Theme *theme = ctx->theme_ptr;

    if (cfg.disabled) {
        return on ? theme->muted : theme->border;
    }

    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);

    if (on) {
        return hovered ? claykit_color_darken(scheme_color, 0.1f) : scheme_color;
    }

    return hovered ? claykit_color_darken(theme->border, 0.05f) : theme->border;
}

/* ----------------------------------------------------------------------------
 * Badge
 * ---------------------------------------------------------------------------- */

ClayKit_BadgeStyle ClayKit_ComputeBadgeStyle(ClayKit_Context *ctx, ClayKit_BadgeConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);
    ClayKit_BadgeStyle style;

    /* Determine padding based on size */
    switch (cfg.size) {
        case CLAYKIT_SIZE_XS:
            style.pad_x = 4; style.pad_y = 1; style.font_size = theme->font_size.xs; break;
        case CLAYKIT_SIZE_SM:
            style.pad_x = 6; style.pad_y = 2; style.font_size = theme->font_size.xs; break;
        case CLAYKIT_SIZE_LG:
            style.pad_x = 10; style.pad_y = 4; style.font_size = theme->font_size.md; break;
        case CLAYKIT_SIZE_XL:
            style.pad_x = 12; style.pad_y = 5; style.font_size = theme->font_size.lg; break;
        case CLAYKIT_SIZE_MD:
        default:
            style.pad_x = 8; style.pad_y = 3; style.font_size = theme->font_size.sm; break;
    }

    /* Determine colors based on variant */
    style.border_width = 0;

    switch (cfg.variant) {
        case CLAYKIT_BADGE_SOLID:
            style.bg_color = scheme_color;
            style.text_color = (Clay_Color){ 255, 255, 255, 255 };
            style.border_color = (Clay_Color){ 0, 0, 0, 0 };
            break;
        case CLAYKIT_BADGE_SUBTLE:
            style.bg_color = claykit_color_lighten(scheme_color, 0.85f);
            style.text_color = scheme_color;
            style.border_color = (Clay_Color){ 0, 0, 0, 0 };
            break;
        case CLAYKIT_BADGE_OUTLINE:
            style.bg_color = (Clay_Color){ 0, 0, 0, 0 };
            style.text_color = scheme_color;
            style.border_color = scheme_color;
            style.border_width = 1;
            break;
        default:
            style.bg_color = scheme_color;
            style.text_color = (Clay_Color){ 255, 255, 255, 255 };
            style.border_color = (Clay_Color){ 0, 0, 0, 0 };
            break;
    }

    style.font_id = theme->font_id.body;
    style.corner_radius = theme->radius.full;

    return style;
}

/* ----------------------------------------------------------------------------
 * Badge Rendering
 * ---------------------------------------------------------------------------- */

void ClayKit_BadgeRaw(ClayKit_Context *ctx, const char *text, int32_t text_len, ClayKit_BadgeConfig cfg) {
    ClayKit_BadgeStyle style = ClayKit_ComputeBadgeStyle(ctx, cfg);

    /* Construct Clay_String from raw pointer */
    Clay_String clay_text = { false, text_len, text };

    /* Use low-level Clay API to avoid macro compatibility issues */
    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = style.pad_x;
    decl.layout.padding.right = style.pad_x;
    decl.layout.padding.top = style.pad_y;
    decl.layout.padding.bottom = style.pad_y;
    decl.backgroundColor = style.bg_color;
    decl.cornerRadius.topLeft = (float)style.corner_radius;
    decl.cornerRadius.topRight = (float)style.corner_radius;
    decl.cornerRadius.bottomLeft = (float)style.corner_radius;
    decl.cornerRadius.bottomRight = (float)style.corner_radius;
    decl.border.color = style.border_color;
    decl.border.width.left = style.border_width;
    decl.border.width.right = style.border_width;
    decl.border.width.top = style.border_width;
    decl.border.width.bottom = style.border_width;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);

    /* Add text element */
    Clay_TextElementConfig text_config = {0};
    text_config.fontSize = style.font_size;
    text_config.fontId = style.font_id;
    text_config.textColor = style.text_color;
    text_config.wrapMode = CLAY_TEXT_WRAP_NONE;

    Clay__OpenTextElement(clay_text, Clay__StoreTextElementConfig(text_config));

    Clay__CloseElement();
}

void ClayKit_Badge(ClayKit_Context *ctx, Clay_String text, ClayKit_BadgeConfig cfg) {
    ClayKit_BadgeRaw(ctx, text.chars, text.length, cfg);
}

/* ----------------------------------------------------------------------------
 * Tag
 * ---------------------------------------------------------------------------- */

ClayKit_TagStyle ClayKit_ComputeTagStyle(ClayKit_Context *ctx, ClayKit_TagConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);
    ClayKit_TagStyle style;

    /* Determine padding and font size based on size */
    switch (cfg.size) {
        case CLAYKIT_SIZE_XS:
            style.pad_x = 6; style.pad_y = 2; style.font_size = theme->font_size.xs;
            style.gap = 4; style.close_font_size = theme->font_size.xs; break;
        case CLAYKIT_SIZE_SM:
            style.pad_x = 8; style.pad_y = 2; style.font_size = theme->font_size.xs;
            style.gap = 4; style.close_font_size = theme->font_size.xs; break;
        case CLAYKIT_SIZE_LG:
            style.pad_x = 12; style.pad_y = 4; style.font_size = theme->font_size.md;
            style.gap = 6; style.close_font_size = theme->font_size.sm; break;
        case CLAYKIT_SIZE_XL:
            style.pad_x = 14; style.pad_y = 5; style.font_size = theme->font_size.lg;
            style.gap = 8; style.close_font_size = theme->font_size.md; break;
        case CLAYKIT_SIZE_MD:
        default:
            style.pad_x = 10; style.pad_y = 3; style.font_size = theme->font_size.sm;
            style.gap = 5; style.close_font_size = theme->font_size.xs; break;
    }

    /* Colors based on variant (same logic as badge) */
    style.border_width = 0;
    style.font_id = theme->font_id.body;
    style.corner_radius = theme->radius.md;

    switch (cfg.variant) {
        case CLAYKIT_TAG_SOLID:
            style.bg_color = scheme_color;
            style.text_color = (Clay_Color){ 255, 255, 255, 255 };
            style.close_color = (Clay_Color){ 255, 255, 255, 180 };
            style.border_color = (Clay_Color){ 0, 0, 0, 0 };
            break;
        case CLAYKIT_TAG_SUBTLE:
            style.bg_color = claykit_color_lighten(scheme_color, 0.85f);
            style.text_color = scheme_color;
            style.close_color = claykit_color_lighten(scheme_color, 0.3f);
            style.border_color = (Clay_Color){ 0, 0, 0, 0 };
            break;
        case CLAYKIT_TAG_OUTLINE:
            style.bg_color = (Clay_Color){ 0, 0, 0, 0 };
            style.text_color = scheme_color;
            style.close_color = claykit_color_lighten(scheme_color, 0.3f);
            style.border_color = scheme_color;
            style.border_width = 1;
            break;
        default:
            style.bg_color = scheme_color;
            style.text_color = (Clay_Color){ 255, 255, 255, 255 };
            style.close_color = (Clay_Color){ 255, 255, 255, 180 };
            style.border_color = (Clay_Color){ 0, 0, 0, 0 };
            break;
    }

    return style;
}

void ClayKit_TagRaw(ClayKit_Context *ctx, const char *text, int32_t text_len, ClayKit_TagConfig cfg) {
    ClayKit_TagStyle style = ClayKit_ComputeTagStyle(ctx, cfg);
    Clay_String clay_text = { false, text_len, text };

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = style.pad_x;
    decl.layout.padding.right = style.pad_x;
    decl.layout.padding.top = style.pad_y;
    decl.layout.padding.bottom = style.pad_y;
    decl.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    decl.layout.childAlignment.y = CLAY_ALIGN_Y_CENTER;
    decl.layout.childGap = cfg.closeable ? style.gap : 0;
    decl.backgroundColor = style.bg_color;
    decl.cornerRadius.topLeft = (float)style.corner_radius;
    decl.cornerRadius.topRight = (float)style.corner_radius;
    decl.cornerRadius.bottomLeft = (float)style.corner_radius;
    decl.cornerRadius.bottomRight = (float)style.corner_radius;
    decl.border.color = style.border_color;
    decl.border.width.left = style.border_width;
    decl.border.width.right = style.border_width;
    decl.border.width.top = style.border_width;
    decl.border.width.bottom = style.border_width;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);

    /* Main text */
    Clay_TextElementConfig text_config = {0};
    text_config.fontSize = style.font_size;
    text_config.fontId = style.font_id;
    text_config.textColor = style.text_color;
    text_config.wrapMode = CLAY_TEXT_WRAP_NONE;
    Clay__OpenTextElement(clay_text, Clay__StoreTextElementConfig(text_config));

    /* Close indicator */
    if (cfg.closeable) {
        Clay_String close_str = { false, 1, "x" };
        Clay_TextElementConfig close_config = {0};
        close_config.fontSize = style.close_font_size;
        close_config.fontId = style.font_id;
        close_config.textColor = style.close_color;
        close_config.wrapMode = CLAY_TEXT_WRAP_NONE;
        Clay__OpenTextElement(close_str, Clay__StoreTextElementConfig(close_config));
    }

    Clay__CloseElement();
}

void ClayKit_Tag(ClayKit_Context *ctx, Clay_String text, ClayKit_TagConfig cfg) {
    ClayKit_TagRaw(ctx, text.chars, text.length, cfg);
}

/* ----------------------------------------------------------------------------
 * Stat
 * ---------------------------------------------------------------------------- */

ClayKit_StatStyle ClayKit_ComputeStatStyle(ClayKit_Context *ctx, ClayKit_StatConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_StatStyle style;

    /* Default colors from theme when alpha == 0 */
    style.label_color = (cfg.label_color.a > 0) ? cfg.label_color : theme->muted;
    style.value_color = (cfg.value_color.a > 0) ? cfg.value_color : theme->fg;
    style.help_color = (cfg.help_color.a > 0) ? cfg.help_color : theme->muted;

    /* Font IDs */
    style.label_font_id = theme->font_id.body;
    style.value_font_id = theme->font_id.heading;
    style.help_font_id = theme->font_id.body;

    /* Font sizes: label one step below, value one step above */
    switch (cfg.size) {
        case CLAYKIT_SIZE_XS:
            style.label_font_size = theme->font_size.xs;
            style.value_font_size = theme->font_size.sm;
            style.help_font_size = theme->font_size.xs;
            style.gap = theme->spacing.xs;
            break;
        case CLAYKIT_SIZE_SM:
            style.label_font_size = theme->font_size.xs;
            style.value_font_size = theme->font_size.md;
            style.help_font_size = theme->font_size.xs;
            style.gap = theme->spacing.xs;
            break;
        case CLAYKIT_SIZE_LG:
            style.label_font_size = theme->font_size.md;
            style.value_font_size = theme->font_size.xl;
            style.help_font_size = theme->font_size.sm;
            style.gap = theme->spacing.sm;
            break;
        case CLAYKIT_SIZE_XL:
            style.label_font_size = theme->font_size.lg;
            style.value_font_size = theme->font_size.xl;
            style.help_font_size = theme->font_size.md;
            style.gap = theme->spacing.sm;
            break;
        case CLAYKIT_SIZE_MD:
        default:
            style.label_font_size = theme->font_size.sm;
            style.value_font_size = theme->font_size.lg;
            style.help_font_size = theme->font_size.xs;
            style.gap = theme->spacing.xs;
            break;
    }

    return style;
}

void ClayKit_Stat(ClayKit_Context *ctx,
                  const char *label, int32_t label_len,
                  const char *value, int32_t value_len,
                  const char *help_text, int32_t help_len,
                  ClayKit_StatConfig cfg) {
    ClayKit_StatStyle style = ClayKit_ComputeStatStyle(ctx, cfg);

    /* Vertical container */
    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    decl.layout.childGap = style.gap;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);

    /* Label */
    {
        Clay_String clay_label = { false, label_len, label };
        Clay_TextElementConfig cfg_label = {0};
        cfg_label.fontSize = style.label_font_size;
        cfg_label.fontId = style.label_font_id;
        cfg_label.textColor = style.label_color;
        cfg_label.wrapMode = CLAY_TEXT_WRAP_NONE;
        Clay__OpenTextElement(clay_label, Clay__StoreTextElementConfig(cfg_label));
    }

    /* Value */
    {
        Clay_String clay_value = { false, value_len, value };
        Clay_TextElementConfig cfg_value = {0};
        cfg_value.fontSize = style.value_font_size;
        cfg_value.fontId = style.value_font_id;
        cfg_value.textColor = style.value_color;
        cfg_value.wrapMode = CLAY_TEXT_WRAP_NONE;
        Clay__OpenTextElement(clay_value, Clay__StoreTextElementConfig(cfg_value));
    }

    /* Help text (optional) */
    if (help_text != NULL && help_len > 0) {
        Clay_String clay_help = { false, help_len, help_text };
        Clay_TextElementConfig cfg_help = {0};
        cfg_help.fontSize = style.help_font_size;
        cfg_help.fontId = style.help_font_id;
        cfg_help.textColor = style.help_color;
        cfg_help.wrapMode = CLAY_TEXT_WRAP_WORDS;
        Clay__OpenTextElement(clay_help, Clay__StoreTextElementConfig(cfg_help));
    }

    Clay__CloseElement();
}

/* ----------------------------------------------------------------------------
 * List
 * ---------------------------------------------------------------------------- */

static void claykit_uint_to_str(uint32_t n, char *buf, int32_t *out_len) {
    char tmp[12];
    int pos = 0;
    if (n == 0) { tmp[pos++] = '0'; }
    else {
        while (n > 0) { tmp[pos++] = '0' + (char)(n % 10); n /= 10; }
    }
    /* Reverse */
    { int i; for (i = 0; i < pos; i++) { buf[i] = tmp[pos - 1 - i]; } }
    buf[pos] = '.';
    *out_len = pos + 1;
}

ClayKit_ListStyle ClayKit_ComputeListStyle(ClayKit_Context *ctx, ClayKit_ListConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_ListStyle style;

    style.marker_color = (cfg.marker_color.a > 0) ? cfg.marker_color : theme->muted;
    style.text_color = (cfg.text_color.a > 0) ? cfg.text_color : theme->fg;
    style.font_id = theme->font_id.body;

    switch (cfg.size) {
        case CLAYKIT_SIZE_XS:
            style.font_size = theme->font_size.xs;
            style.gap = 2; style.marker_width = 18; style.item_gap = 4; break;
        case CLAYKIT_SIZE_SM:
            style.font_size = theme->font_size.sm;
            style.gap = 3; style.marker_width = 20; style.item_gap = 6; break;
        case CLAYKIT_SIZE_LG:
            style.font_size = theme->font_size.lg;
            style.gap = 6; style.marker_width = 28; style.item_gap = 10; break;
        case CLAYKIT_SIZE_XL:
            style.font_size = theme->font_size.xl;
            style.gap = 8; style.marker_width = 32; style.item_gap = 12; break;
        case CLAYKIT_SIZE_MD:
        default:
            style.font_size = theme->font_size.md;
            style.gap = 4; style.marker_width = 24; style.item_gap = 8; break;
    }

    return style;
}

void ClayKit_ListBegin(ClayKit_Context *ctx, ClayKit_ListConfig cfg) {
    ClayKit_ListStyle style = ClayKit_ComputeListStyle(ctx, cfg);

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    decl.layout.childGap = style.gap;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);
}

void ClayKit_ListItemRaw(ClayKit_Context *ctx, const char *text, int32_t text_len,
                         uint32_t index, ClayKit_ListConfig cfg) {
    ClayKit_ListStyle style = ClayKit_ComputeListStyle(ctx, cfg);

    /* Row container: [marker] [text] */
    Clay_ElementDeclaration row_decl = {0};
    row_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    row_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    row_decl.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    row_decl.layout.childGap = style.item_gap;
    row_decl.layout.childAlignment.y = CLAY_ALIGN_Y_TOP;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(row_decl);

    /* Marker container (fixed width, right-aligned text) */
    {
        Clay_ElementDeclaration marker_decl = {0};
        marker_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIXED;
        marker_decl.layout.sizing.width.size.minMax.min = (float)style.marker_width;
        marker_decl.layout.sizing.width.size.minMax.max = (float)style.marker_width;
        marker_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
        marker_decl.layout.childAlignment.x = CLAY_ALIGN_X_RIGHT;

        Clay__OpenElement();
        Clay__ConfigureOpenElement(marker_decl);

        Clay_TextElementConfig marker_text_cfg = {0};
        marker_text_cfg.fontSize = style.font_size;
        marker_text_cfg.fontId = style.font_id;
        marker_text_cfg.textColor = style.marker_color;
        marker_text_cfg.wrapMode = CLAY_TEXT_WRAP_NONE;

        if (cfg.ordered) {
            /* Clay stores string pointers and reads them at render time,
               so the buffer must survive until after Clay_EndLayout().
               Use a static ring of buffers — one per list item index. */
            #define CLAYKIT_NUM_BUF_SLOTS 64
            static char num_bufs[CLAYKIT_NUM_BUF_SLOTS][12];
            char *num_buf = num_bufs[index % CLAYKIT_NUM_BUF_SLOTS];
            int32_t num_len;
            claykit_uint_to_str(index + 1, num_buf, &num_len);
            Clay_String marker_str = { false, num_len, num_buf };
            Clay__OpenTextElement(marker_str, Clay__StoreTextElementConfig(marker_text_cfg));
        } else {
            /* Bullet character: Unicode U+2022 */
            Clay_String bullet_str = { false, 3, "\xe2\x80\xa2" };
            Clay__OpenTextElement(bullet_str, Clay__StoreTextElementConfig(marker_text_cfg));
        }

        Clay__CloseElement(); /* marker container */
    }

    /* Item text */
    {
        Clay_String clay_text = { false, text_len, text };
        Clay_TextElementConfig text_cfg = {0};
        text_cfg.fontSize = style.font_size;
        text_cfg.fontId = style.font_id;
        text_cfg.textColor = style.text_color;
        text_cfg.wrapMode = CLAY_TEXT_WRAP_WORDS;
        Clay__OpenTextElement(clay_text, Clay__StoreTextElementConfig(text_cfg));
    }

    Clay__CloseElement(); /* row */
}

void ClayKit_ListEnd(void) {
    Clay__CloseElement();
}

/* ----------------------------------------------------------------------------
 * Table
 * ---------------------------------------------------------------------------- */

ClayKit_TableStyle ClayKit_ComputeTableStyle(ClayKit_Context *ctx, ClayKit_TableConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);
    ClayKit_TableStyle style;

    style.header_bg = scheme_color;
    style.header_text = (Clay_Color){ 255, 255, 255, 255 };
    style.row_bg = theme->bg;
    style.row_alt_bg = cfg.striped ? claykit_color_lighten(theme->border, 0.7f) : theme->bg;
    style.text_color = theme->fg;
    style.border_color = theme->border;
    style.border_width = cfg.bordered ? 1 : 0;
    style.font_id = theme->font_id.body;
    style.corner_radius = theme->radius.md;

    switch (cfg.size) {
        case CLAYKIT_SIZE_XS:
            style.cell_pad_x = 4; style.cell_pad_y = 2;
            style.font_size = theme->font_size.xs;
            style.header_font_size = theme->font_size.xs; break;
        case CLAYKIT_SIZE_SM:
            style.cell_pad_x = 6; style.cell_pad_y = 4;
            style.font_size = theme->font_size.sm;
            style.header_font_size = theme->font_size.sm; break;
        case CLAYKIT_SIZE_LG:
            style.cell_pad_x = 16; style.cell_pad_y = 10;
            style.font_size = theme->font_size.lg;
            style.header_font_size = theme->font_size.lg; break;
        case CLAYKIT_SIZE_XL:
            style.cell_pad_x = 20; style.cell_pad_y = 12;
            style.font_size = theme->font_size.xl;
            style.header_font_size = theme->font_size.xl; break;
        case CLAYKIT_SIZE_MD:
        default:
            style.cell_pad_x = 12; style.cell_pad_y = 8;
            style.font_size = theme->font_size.md;
            style.header_font_size = theme->font_size.md; break;
    }

    return style;
}

void ClayKit_TableBegin(ClayKit_Context *ctx, ClayKit_TableConfig cfg) {
    ClayKit_TableStyle style = ClayKit_ComputeTableStyle(ctx, cfg);

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    /* Use childGap as horizontal border between rows */
    decl.layout.childGap = style.border_width;
    decl.backgroundColor = style.border_color;
    decl.cornerRadius.topLeft = (float)style.corner_radius;
    decl.cornerRadius.topRight = (float)style.corner_radius;
    decl.cornerRadius.bottomLeft = (float)style.corner_radius;
    decl.cornerRadius.bottomRight = (float)style.corner_radius;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);
}

void ClayKit_TableHeaderRow(ClayKit_Context *ctx, ClayKit_TableConfig cfg) {
    ClayKit_TableStyle style = ClayKit_ComputeTableStyle(ctx, cfg);
    (void)style;

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    decl.layout.childGap = style.border_width;
    decl.backgroundColor = style.border_color;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);
}

void ClayKit_TableRow(ClayKit_Context *ctx, uint32_t row_index, ClayKit_TableConfig cfg) {
    ClayKit_TableStyle style = ClayKit_ComputeTableStyle(ctx, cfg);
    (void)row_index;

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    decl.layout.childGap = style.border_width;
    /* Row bg = border_color when bordered, so gaps between cells show as borders */
    decl.backgroundColor = cfg.bordered ? style.border_color : (Clay_Color){0, 0, 0, 0};

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);
}

void ClayKit_TableHeaderCell(ClayKit_Context *ctx, float width_percent, ClayKit_TableConfig cfg) {
    ClayKit_TableStyle style = ClayKit_ComputeTableStyle(ctx, cfg);

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_PERCENT;
    decl.layout.sizing.width.size.percent = width_percent;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = style.cell_pad_x;
    decl.layout.padding.right = style.cell_pad_x;
    decl.layout.padding.top = style.cell_pad_y;
    decl.layout.padding.bottom = style.cell_pad_y;
    decl.backgroundColor = style.header_bg;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);
}

void ClayKit_TableCell(ClayKit_Context *ctx, float width_percent, uint32_t row_index, ClayKit_TableConfig cfg) {
    ClayKit_TableStyle style = ClayKit_ComputeTableStyle(ctx, cfg);

    Clay_Color bg = (cfg.striped && (row_index % 2 == 1)) ? style.row_alt_bg : style.row_bg;

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_PERCENT;
    decl.layout.sizing.width.size.percent = width_percent;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = style.cell_pad_x;
    decl.layout.padding.right = style.cell_pad_x;
    decl.layout.padding.top = style.cell_pad_y;
    decl.layout.padding.bottom = style.cell_pad_y;
    decl.backgroundColor = bg;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);
}

void ClayKit_TableCellEnd(void) { Clay__CloseElement(); }
void ClayKit_TableRowEnd(void) { Clay__CloseElement(); }
void ClayKit_TableEnd(void) { Clay__CloseElement(); }

/* ----------------------------------------------------------------------------
 * Progress
 * ---------------------------------------------------------------------------- */

ClayKit_ProgressStyle ClayKit_ComputeProgressStyle(ClayKit_Context *ctx, ClayKit_ProgressConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_ProgressStyle style;

    /* Track is a lightened version of border color */
    style.track_color = claykit_color_lighten(theme->border, 0.5f);
    style.fill_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);

    /* Height based on size */
    switch (cfg.size) {
        case CLAYKIT_SIZE_XS: style.height = 4; break;
        case CLAYKIT_SIZE_SM: style.height = 6; break;
        case CLAYKIT_SIZE_LG: style.height = 12; break;
        case CLAYKIT_SIZE_XL: style.height = 16; break;
        case CLAYKIT_SIZE_MD:
        default: style.height = 8; break;
    }

    style.corner_radius = style.height / 2;

    return style;
}

void ClayKit_Progress(ClayKit_Context *ctx, float value, ClayKit_ProgressConfig cfg) {
    ClayKit_ProgressStyle style = ClayKit_ComputeProgressStyle(ctx, cfg);
    float clamped = value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);

    /* Outer track container */
    Clay_ElementDeclaration track_decl = {0};
    track_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    track_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
    track_decl.layout.sizing.height.size.minMax.min = (float)style.height;
    track_decl.layout.sizing.height.size.minMax.max = (float)style.height;
    track_decl.backgroundColor = style.track_color;
    track_decl.cornerRadius.topLeft = (float)style.corner_radius;
    track_decl.cornerRadius.topRight = (float)style.corner_radius;
    track_decl.cornerRadius.bottomLeft = (float)style.corner_radius;
    track_decl.cornerRadius.bottomRight = (float)style.corner_radius;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(track_decl);

    /* Filled portion */
    if (clamped > 0.0f) {
        Clay_ElementDeclaration fill_decl = {0};
        fill_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_PERCENT;
        fill_decl.layout.sizing.width.size.percent = clamped;
        fill_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_GROW;
        fill_decl.backgroundColor = style.fill_color;
        fill_decl.cornerRadius.topLeft = (float)style.corner_radius;
        fill_decl.cornerRadius.topRight = (float)style.corner_radius;
        fill_decl.cornerRadius.bottomLeft = (float)style.corner_radius;
        fill_decl.cornerRadius.bottomRight = (float)style.corner_radius;

        Clay__OpenElement();
        Clay__ConfigureOpenElement(fill_decl);
        Clay__CloseElement();
    }

    Clay__CloseElement();
}

/* ----------------------------------------------------------------------------
 * Slider
 * ---------------------------------------------------------------------------- */

ClayKit_SliderStyle ClayKit_ComputeSliderStyle(ClayKit_Context *ctx, ClayKit_SliderConfig cfg, bool hovered) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_SliderStyle style;

    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);

    /* Track is a lightened version of border color */
    style.track_color = claykit_color_lighten(theme->border, 0.5f);
    style.fill_color = cfg.disabled ? theme->muted : scheme_color;

    /* Thumb color with hover state */
    if (cfg.disabled) {
        style.thumb_color = theme->muted;
    } else if (hovered) {
        style.thumb_color = claykit_color_darken(scheme_color, 0.1f);
    } else {
        style.thumb_color = scheme_color;
    }

    /* Sizes based on size enum */
    switch (cfg.size) {
        case CLAYKIT_SIZE_XS:
            style.track_height = 4;
            style.thumb_size = 12;
            break;
        case CLAYKIT_SIZE_SM:
            style.track_height = 6;
            style.thumb_size = 16;
            break;
        case CLAYKIT_SIZE_LG:
            style.track_height = 10;
            style.thumb_size = 24;
            break;
        case CLAYKIT_SIZE_XL:
            style.track_height = 12;
            style.thumb_size = 28;
            break;
        case CLAYKIT_SIZE_MD:
        default:
            style.track_height = 8;
            style.thumb_size = 20;
            break;
    }

    style.corner_radius = style.track_height / 2;

    return style;
}

/* ----------------------------------------------------------------------------
 * Select
 * ---------------------------------------------------------------------------- */

ClayKit_SelectStyle ClayKit_ComputeSelectStyle(ClayKit_Context *ctx, ClayKit_SelectConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_SelectStyle style;

    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);

    style.bg_color = theme->bg;
    style.border_color = theme->border;
    style.text_color = cfg.disabled ? theme->muted : theme->fg;
    style.placeholder_color = theme->muted;
    style.dropdown_bg = theme->bg;
    style.dropdown_border = theme->border;
    style.option_hover_bg = claykit_color_lighten(scheme_color, 0.9f);
    style.font_id = theme->font_id.body;
    style.corner_radius = ClayKit_GetRadius(theme, cfg.size);

    switch (cfg.size) {
        case CLAYKIT_SIZE_XS:
            style.padding_x = 8;
            style.padding_y = 4;
            style.font_size = theme->font_size.xs;
            style.dropdown_gap = 0;
            break;
        case CLAYKIT_SIZE_SM:
            style.padding_x = 10;
            style.padding_y = 5;
            style.font_size = theme->font_size.sm;
            style.dropdown_gap = 0;
            break;
        case CLAYKIT_SIZE_LG:
            style.padding_x = 16;
            style.padding_y = 10;
            style.font_size = theme->font_size.lg;
            style.dropdown_gap = 2;
            break;
        case CLAYKIT_SIZE_XL:
            style.padding_x = 20;
            style.padding_y = 12;
            style.font_size = theme->font_size.xl;
            style.dropdown_gap = 4;
            break;
        case CLAYKIT_SIZE_MD:
        default:
            style.padding_x = 12;
            style.padding_y = 8;
            style.font_size = theme->font_size.md;
            style.dropdown_gap = 2;
            break;
    }

    return style;
}

bool ClayKit_SelectTrigger(ClayKit_Context *ctx, const char *id, int32_t id_len,
                           const char *display_text, int32_t display_len,
                           ClayKit_SelectConfig cfg) {
    ClayKit_SelectStyle style = ClayKit_ComputeSelectStyle(ctx, cfg);

    Clay__OpenElement();
    bool hovered = Clay_Hovered();

    Clay_String id_str = { false, id_len, id };
    Clay_ElementDeclaration decl = {0};
    decl.id = Clay__HashString(id_str, 0, 0);
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = style.padding_x;
    decl.layout.padding.right = style.padding_x;
    decl.layout.padding.top = style.padding_y;
    decl.layout.padding.bottom = style.padding_y;
    decl.layout.childGap = 8;
    decl.layout.childAlignment.x = CLAY_ALIGN_X_LEFT;
    decl.layout.childAlignment.y = CLAY_ALIGN_Y_CENTER;
    decl.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    decl.backgroundColor = hovered && !cfg.disabled
        ? claykit_color_darken(style.bg_color, 0.03f)
        : style.bg_color;
    decl.cornerRadius.topLeft = (float)style.corner_radius;
    decl.cornerRadius.topRight = (float)style.corner_radius;
    decl.cornerRadius.bottomLeft = (float)style.corner_radius;
    decl.cornerRadius.bottomRight = (float)style.corner_radius;
    decl.border.color = style.border_color;
    decl.border.width.left = 1;
    decl.border.width.right = 1;
    decl.border.width.top = 1;
    decl.border.width.bottom = 1;

    Clay__ConfigureOpenElement(decl);

    /* Display text (or placeholder) - grows to push arrow right */
    {
        Clay_ElementDeclaration text_wrapper = {0};
        text_wrapper.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
        text_wrapper.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;

        Clay__OpenElement();
        Clay__ConfigureOpenElement(text_wrapper);

        if (display_text != NULL && display_len > 0) {
            Clay_String text_str = { false, display_len, display_text };
            Clay_TextElementConfig text_cfg = {0};
            text_cfg.fontSize = style.font_size;
            text_cfg.textColor = style.text_color;
            text_cfg.fontId = style.font_id;
            text_cfg.wrapMode = CLAY_TEXT_WRAP_NONE;
            Clay__OpenTextElement(text_str, Clay__StoreTextElementConfig(text_cfg));
        } else {
            Clay_String placeholder = { false, 9, "Select..." };
            Clay_TextElementConfig text_cfg = {0};
            text_cfg.fontSize = style.font_size;
            text_cfg.textColor = style.placeholder_color;
            text_cfg.fontId = style.font_id;
            text_cfg.wrapMode = CLAY_TEXT_WRAP_NONE;
            Clay__OpenTextElement(placeholder, Clay__StoreTextElementConfig(text_cfg));
        }

        Clay__CloseElement();
    }

    /* Down arrow indicator */
    {
        Clay_String arrow = { false, 3, "\xe2\x96\xbc" }; /* Unicode U+25BC BLACK DOWN-POINTING TRIANGLE */
        Clay_TextElementConfig arrow_cfg = {0};
        arrow_cfg.fontSize = style.font_size > 4 ? style.font_size - 4 : style.font_size;
        arrow_cfg.textColor = style.text_color;
        arrow_cfg.fontId = style.font_id;
        Clay__OpenTextElement(arrow, Clay__StoreTextElementConfig(arrow_cfg));
    }

    Clay__CloseElement();
    return hovered && !cfg.disabled;
}

void ClayKit_SelectDropdownBegin(ClayKit_Context *ctx, const char *id, int32_t id_len,
                                 ClayKit_SelectConfig cfg) {
    ClayKit_SelectStyle style = ClayKit_ComputeSelectStyle(ctx, cfg);

    Clay_String id_str = { false, id_len, id };
    Clay_ElementDeclaration decl = {0};
    decl.id = Clay__HashString(id_str, 0, 0);
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = 4;
    decl.layout.padding.right = 4;
    decl.layout.padding.top = 4;
    decl.layout.padding.bottom = 4;
    decl.layout.childGap = style.dropdown_gap;
    decl.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    decl.backgroundColor = style.dropdown_bg;
    decl.cornerRadius.topLeft = (float)style.corner_radius;
    decl.cornerRadius.topRight = (float)style.corner_radius;
    decl.cornerRadius.bottomLeft = (float)style.corner_radius;
    decl.cornerRadius.bottomRight = (float)style.corner_radius;
    decl.border.color = style.dropdown_border;
    decl.border.width.left = 1;
    decl.border.width.right = 1;
    decl.border.width.top = 1;
    decl.border.width.bottom = 1;
    decl.floating.attachTo = CLAY_ATTACH_TO_PARENT;
    decl.floating.attachPoints.element = CLAY_ATTACH_POINT_LEFT_TOP;
    decl.floating.attachPoints.parent = CLAY_ATTACH_POINT_LEFT_BOTTOM;
    decl.floating.zIndex = 100;
    decl.floating.offset.y = 4;
    decl.floating.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);
}

bool ClayKit_SelectOption(ClayKit_Context *ctx, const char *text, int32_t text_len,
                          bool is_selected, ClayKit_SelectConfig cfg) {
    ClayKit_SelectStyle style = ClayKit_ComputeSelectStyle(ctx, cfg);

    Clay__OpenElement();
    bool hovered = Clay_Hovered();

    Clay_Color bg = (Clay_Color){ 0, 0, 0, 0 };
    if (is_selected) {
        bg = claykit_color_lighten(ClayKit_GetSchemeColor(ctx->theme_ptr, cfg.color_scheme), 0.85f);
    } else if (hovered) {
        bg = style.option_hover_bg;
    }

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = style.padding_x;
    decl.layout.padding.right = style.padding_x;
    decl.layout.padding.top = style.padding_y > 2 ? style.padding_y - 2 : style.padding_y;
    decl.layout.padding.bottom = style.padding_y > 2 ? style.padding_y - 2 : style.padding_y;
    decl.backgroundColor = bg;
    decl.cornerRadius.topLeft = (float)(style.corner_radius > 2 ? style.corner_radius - 2 : style.corner_radius);
    decl.cornerRadius.topRight = decl.cornerRadius.topLeft;
    decl.cornerRadius.bottomLeft = decl.cornerRadius.topLeft;
    decl.cornerRadius.bottomRight = decl.cornerRadius.topLeft;

    Clay__ConfigureOpenElement(decl);

    Clay_String text_str = { false, text_len, text };
    Clay_TextElementConfig text_cfg = {0};
    text_cfg.fontSize = style.font_size;
    text_cfg.textColor = style.text_color;
    text_cfg.fontId = style.font_id;
    text_cfg.wrapMode = CLAY_TEXT_WRAP_NONE;
    Clay__OpenTextElement(text_str, Clay__StoreTextElementConfig(text_cfg));

    Clay__CloseElement();
    return hovered;
}

void ClayKit_SelectDropdownEnd(void) {
    Clay__CloseElement();
}

/* ----------------------------------------------------------------------------
 * Alert
 * ---------------------------------------------------------------------------- */

ClayKit_AlertStyle ClayKit_ComputeAlertStyle(ClayKit_Context *ctx, ClayKit_AlertConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_AlertStyle style;

    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);

    switch (cfg.variant) {
        case CLAYKIT_ALERT_SOLID:
            style.bg_color = scheme_color;
            style.border_color = scheme_color;
            style.text_color = (Clay_Color){ 255, 255, 255, 255 };
            style.icon_color = (Clay_Color){ 255, 255, 255, 255 };
            style.border_width = 0;
            break;
        case CLAYKIT_ALERT_OUTLINE:
            style.bg_color = (Clay_Color){ 0, 0, 0, 0 };
            style.border_color = scheme_color;
            style.text_color = scheme_color;
            style.icon_color = scheme_color;
            style.border_width = 1;
            break;
        case CLAYKIT_ALERT_SUBTLE:
        default:
            style.bg_color = claykit_color_lighten(scheme_color, 0.9f);
            style.border_color = claykit_color_lighten(scheme_color, 0.5f);
            style.text_color = claykit_color_darken(scheme_color, 0.3f);
            style.icon_color = scheme_color;
            style.border_width = 1;
            break;
    }

    style.padding = theme->spacing.md;
    style.corner_radius = theme->radius.md;
    style.icon_size = (cfg.icon.size > 0) ? cfg.icon.size : 20;

    return style;
}

void ClayKit_AlertText(ClayKit_Context *ctx, const char *text, int32_t text_len, ClayKit_AlertConfig cfg) {
    ClayKit_AlertStyle style = ClayKit_ComputeAlertStyle(ctx, cfg);

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = style.padding;
    decl.layout.padding.right = style.padding;
    decl.layout.padding.top = style.padding;
    decl.layout.padding.bottom = style.padding;
    decl.layout.childGap = 12;
    decl.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    decl.layout.childAlignment.y = CLAY_ALIGN_Y_CENTER;
    decl.backgroundColor = style.bg_color;
    decl.cornerRadius.topLeft = (float)style.corner_radius;
    decl.cornerRadius.topRight = (float)style.corner_radius;
    decl.cornerRadius.bottomLeft = (float)style.corner_radius;
    decl.cornerRadius.bottomRight = (float)style.corner_radius;
    decl.border.color = style.border_color;
    decl.border.width.left = style.border_width;
    decl.border.width.right = style.border_width;
    decl.border.width.top = style.border_width;
    decl.border.width.bottom = style.border_width;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);

    /* Text */
    Clay_String clay_text = { false, text_len, text };
    Clay_TextElementConfig text_config = {0};
    text_config.fontSize = ctx->theme_ptr->font_size.md;
    text_config.textColor = style.text_color;
    text_config.wrapMode = CLAY_TEXT_WRAP_WORDS;

    Clay__OpenTextElement(clay_text, Clay__StoreTextElementConfig(text_config));

    Clay__CloseElement();
}

/* ----------------------------------------------------------------------------
 * Tooltip
 * ---------------------------------------------------------------------------- */

ClayKit_TooltipStyle ClayKit_ComputeTooltipStyle(ClayKit_Context *ctx, ClayKit_TooltipConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_TooltipStyle style;
    (void)cfg; /* Position doesn't affect style, only layout */

    /* Dark background, light text */
    style.bg_color = (Clay_Color){ 31, 41, 55, 240 }; /* Gray-800 with slight transparency */
    style.text_color = (Clay_Color){ 249, 250, 251, 255 }; /* Gray-50 */

    style.padding_x = theme->spacing.sm;
    style.padding_y = theme->spacing.xs;
    style.corner_radius = theme->radius.sm;
    style.font_size = theme->font_size.sm;

    return style;
}

void ClayKit_Tooltip(ClayKit_Context *ctx, const char *text, int32_t text_len, ClayKit_TooltipConfig cfg) {
    ClayKit_TooltipStyle style = ClayKit_ComputeTooltipStyle(ctx, cfg);

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = style.padding_x;
    decl.layout.padding.right = style.padding_x;
    decl.layout.padding.top = style.padding_y;
    decl.layout.padding.bottom = style.padding_y;
    decl.backgroundColor = style.bg_color;
    decl.cornerRadius.topLeft = (float)style.corner_radius;
    decl.cornerRadius.topRight = (float)style.corner_radius;
    decl.cornerRadius.bottomLeft = (float)style.corner_radius;
    decl.cornerRadius.bottomRight = (float)style.corner_radius;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);

    Clay_String clay_text = { false, text_len, text };
    Clay_TextElementConfig text_config = {0};
    text_config.fontSize = style.font_size;
    text_config.textColor = style.text_color;
    text_config.wrapMode = CLAY_TEXT_WRAP_NONE;

    Clay__OpenTextElement(clay_text, Clay__StoreTextElementConfig(text_config));

    Clay__CloseElement();
}

/* ----------------------------------------------------------------------------
 * Tabs
 * ---------------------------------------------------------------------------- */

ClayKit_TabsStyle ClayKit_ComputeTabsStyle(ClayKit_Context *ctx, ClayKit_TabsConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_TabsStyle style;

    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);

    style.active_color = scheme_color;
    style.inactive_color = theme->muted;
    style.bg_color = (Clay_Color){ 0, 0, 0, 0 }; /* Transparent by default */
    style.border_color = theme->border;

    /* Active text color depends on variant */
    switch (cfg.variant) {
        case CLAYKIT_TABS_ENCLOSED:
        case CLAYKIT_TABS_SOFT:
            style.active_text = (Clay_Color){ 255, 255, 255, 255 };
            break;
        case CLAYKIT_TABS_LINE:
        default:
            style.active_text = scheme_color;
            break;
    }

    /* Sizes based on size enum */
    switch (cfg.size) {
        case CLAYKIT_SIZE_XS:
            style.padding_x = 8;
            style.padding_y = 4;
            style.font_size = theme->font_size.xs;
            style.indicator_height = 2;
            style.gap = 4;
            break;
        case CLAYKIT_SIZE_SM:
            style.padding_x = 12;
            style.padding_y = 6;
            style.font_size = theme->font_size.sm;
            style.indicator_height = 2;
            style.gap = 8;
            break;
        case CLAYKIT_SIZE_LG:
            style.padding_x = 20;
            style.padding_y = 12;
            style.font_size = theme->font_size.lg;
            style.indicator_height = 3;
            style.gap = 16;
            break;
        case CLAYKIT_SIZE_XL:
            style.padding_x = 24;
            style.padding_y = 14;
            style.font_size = theme->font_size.xl;
            style.indicator_height = 4;
            style.gap = 20;
            break;
        case CLAYKIT_SIZE_MD:
        default:
            style.padding_x = 16;
            style.padding_y = 8;
            style.font_size = theme->font_size.md;
            style.indicator_height = 2;
            style.gap = 12;
            break;
    }

    style.corner_radius = theme->radius.sm;

    return style;
}

/* ----------------------------------------------------------------------------
 * Modal
 * ---------------------------------------------------------------------------- */

ClayKit_ModalStyle ClayKit_ComputeModalStyle(ClayKit_Context *ctx, ClayKit_ModalConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_ModalStyle style;

    /* Semi-transparent black backdrop */
    style.backdrop_color = (Clay_Color){ 0, 0, 0, 128 };
    style.bg_color = theme->bg;
    style.border_color = theme->border;
    style.corner_radius = theme->radius.lg;
    style.padding = theme->spacing.lg;

    /* Z-index with default */
    style.z_index = cfg.z_index > 0 ? cfg.z_index : 1000;

    /* Width based on size */
    switch (cfg.size) {
        case CLAYKIT_MODAL_SM:
            style.width = 400;
            break;
        case CLAYKIT_MODAL_LG:
            style.width = 600;
            break;
        case CLAYKIT_MODAL_XL:
            style.width = 800;
            break;
        case CLAYKIT_MODAL_FULL:
            style.width = 0; /* Will use grow with margins */
            break;
        case CLAYKIT_MODAL_MD:
        default:
            style.width = 500;
            break;
    }

    return style;
}

/* ----------------------------------------------------------------------------
 * Spinner
 * ---------------------------------------------------------------------------- */

ClayKit_SpinnerStyle ClayKit_ComputeSpinnerStyle(ClayKit_Context *ctx, ClayKit_SpinnerConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_SpinnerStyle style;

    style.color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);
    style.track_color = claykit_color_lighten(theme->border, 0.5f);
    style.speed = cfg.speed > 0.0f ? cfg.speed : 1.0f;

    switch (cfg.size) {
        case CLAYKIT_SIZE_XS:
            style.diameter = 16;
            style.thickness = 2;
            break;
        case CLAYKIT_SIZE_SM:
            style.diameter = 24;
            style.thickness = 3;
            break;
        case CLAYKIT_SIZE_LG:
            style.diameter = 48;
            style.thickness = 5;
            break;
        case CLAYKIT_SIZE_XL:
            style.diameter = 64;
            style.thickness = 6;
            break;
        case CLAYKIT_SIZE_MD:
        default:
            style.diameter = 32;
            style.thickness = 4;
            break;
    }

    return style;
}

float ClayKit_SpinnerAngle(ClayKit_Context *ctx, ClayKit_SpinnerConfig cfg) {
    float speed = cfg.speed > 0.0f ? cfg.speed : 1.0f;
    float angle = ctx->cursor_blink_time * speed * 360.0f;
    /* Normalize to 0-360 */
    angle = angle - (float)((int)(angle / 360.0f)) * 360.0f;
    if (angle < 0.0f) angle += 360.0f;
    return angle;
}

void ClayKit_Spinner(ClayKit_Context *ctx, ClayKit_SpinnerConfig cfg) {
    ClayKit_SpinnerStyle style = ClayKit_ComputeSpinnerStyle(ctx, cfg);

    /* Render as a fixed-size circular element.
     * The track circle is the background, the spinner arc must be drawn by the renderer.
     * We render two concentric circles to approximate: outer = track, inner = transparent hole. */

    /* Outer circle (track) */
    float radius = (float)(style.diameter / 2);

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIXED;
    decl.layout.sizing.width.size.minMax.min = (float)style.diameter;
    decl.layout.sizing.width.size.minMax.max = (float)style.diameter;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
    decl.layout.sizing.height.size.minMax.min = (float)style.diameter;
    decl.layout.sizing.height.size.minMax.max = (float)style.diameter;
    decl.layout.childAlignment.x = CLAY_ALIGN_X_CENTER;
    decl.layout.childAlignment.y = CLAY_ALIGN_Y_CENTER;
    decl.backgroundColor = style.track_color;
    decl.cornerRadius.topLeft = radius;
    decl.cornerRadius.topRight = radius;
    decl.cornerRadius.bottomLeft = radius;
    decl.cornerRadius.bottomRight = radius;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);

    /* Inner hole (transparent center to create ring appearance) */
    {
        uint16_t inner_diameter = style.diameter > (style.thickness * 2) ? style.diameter - (style.thickness * 2) : 2;
        float inner_radius = (float)(inner_diameter / 2);

        Clay_ElementDeclaration inner_decl = {0};
        inner_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIXED;
        inner_decl.layout.sizing.width.size.minMax.min = (float)inner_diameter;
        inner_decl.layout.sizing.width.size.minMax.max = (float)inner_diameter;
        inner_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
        inner_decl.layout.sizing.height.size.minMax.min = (float)inner_diameter;
        inner_decl.layout.sizing.height.size.minMax.max = (float)inner_diameter;
        inner_decl.backgroundColor = ctx->theme_ptr->bg;
        inner_decl.cornerRadius.topLeft = inner_radius;
        inner_decl.cornerRadius.topRight = inner_radius;
        inner_decl.cornerRadius.bottomLeft = inner_radius;
        inner_decl.cornerRadius.bottomRight = inner_radius;

        Clay__OpenElement();
        Clay__ConfigureOpenElement(inner_decl);
        Clay__CloseElement();
    }

    Clay__CloseElement();
}

/* ----------------------------------------------------------------------------
 * Drawer
 * ---------------------------------------------------------------------------- */

ClayKit_DrawerStyle ClayKit_ComputeDrawerStyle(ClayKit_Context *ctx, ClayKit_DrawerConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_DrawerStyle style;

    style.backdrop_color = (Clay_Color){ 0, 0, 0, 128 };
    style.bg_color = theme->bg;
    style.border_color = theme->border;
    style.padding = theme->spacing.lg;
    style.size = cfg.size > 0 ? cfg.size : 300;
    style.z_index = cfg.z_index > 0 ? cfg.z_index : 1000;

    return style;
}

bool ClayKit_DrawerBegin(ClayKit_Context *ctx, const char *id, int32_t id_len, ClayKit_DrawerConfig cfg) {
    ClayKit_DrawerStyle style = ClayKit_ComputeDrawerStyle(ctx, cfg);

    char backdrop_id_buf[128];
    int32_t backdrop_id_len = 0;
    {
        /* Build backdrop ID: id + "Backdrop" */
        int32_t copy_len = id_len < 110 ? id_len : 110;
        int32_t i;
        for (i = 0; i < copy_len; i++) backdrop_id_buf[i] = id[i];
        backdrop_id_buf[copy_len] = 'B'; backdrop_id_buf[copy_len+1] = 'k';
        backdrop_id_buf[copy_len+2] = 'd'; backdrop_id_buf[copy_len+3] = 'p';
        backdrop_id_len = copy_len + 4;
    }

    /* Backdrop - full screen overlay */
    Clay_String backdrop_str = { false, backdrop_id_len, backdrop_id_buf };
    Clay_ElementDeclaration backdrop_decl = {0};
    backdrop_decl.id = Clay__HashString(backdrop_str, 0, 0);
    backdrop_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    backdrop_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_GROW;
    backdrop_decl.backgroundColor = style.backdrop_color;
    backdrop_decl.floating.attachTo = CLAY_ATTACH_TO_ROOT;
    backdrop_decl.floating.attachPoints.element = CLAY_ATTACH_POINT_LEFT_TOP;
    backdrop_decl.floating.attachPoints.parent = CLAY_ATTACH_POINT_LEFT_TOP;
    backdrop_decl.floating.zIndex = (int16_t)style.z_index;
    backdrop_decl.floating.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE;

    /* Set child alignment based on drawer side */
    switch (cfg.side) {
        case CLAYKIT_DRAWER_LEFT:
            backdrop_decl.layout.childAlignment.x = CLAY_ALIGN_X_LEFT;
            backdrop_decl.layout.childAlignment.y = CLAY_ALIGN_Y_TOP;
            break;
        case CLAYKIT_DRAWER_RIGHT:
            backdrop_decl.layout.childAlignment.x = CLAY_ALIGN_X_RIGHT;
            backdrop_decl.layout.childAlignment.y = CLAY_ALIGN_Y_TOP;
            break;
        case CLAYKIT_DRAWER_TOP:
            backdrop_decl.layout.childAlignment.x = CLAY_ALIGN_X_LEFT;
            backdrop_decl.layout.childAlignment.y = CLAY_ALIGN_Y_TOP;
            break;
        case CLAYKIT_DRAWER_BOTTOM:
            backdrop_decl.layout.childAlignment.x = CLAY_ALIGN_X_LEFT;
            backdrop_decl.layout.childAlignment.y = CLAY_ALIGN_Y_BOTTOM;
            break;
    }

    Clay__OpenElement();
    bool backdrop_hovered = Clay_Hovered();
    Clay__ConfigureOpenElement(backdrop_decl);

    /* Drawer panel */
    Clay_String panel_str = { false, id_len, id };
    Clay_ElementDeclaration panel_decl = {0};
    panel_decl.id = Clay__HashString(panel_str, 0, 0);
    panel_decl.layout.padding.left = style.padding;
    panel_decl.layout.padding.right = style.padding;
    panel_decl.layout.padding.top = style.padding;
    panel_decl.layout.padding.bottom = style.padding;
    panel_decl.layout.childGap = 16;
    panel_decl.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    panel_decl.backgroundColor = style.bg_color;

    /* Size depends on side */
    if (cfg.side == CLAYKIT_DRAWER_LEFT || cfg.side == CLAYKIT_DRAWER_RIGHT) {
        panel_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIXED;
        panel_decl.layout.sizing.width.size.minMax.min = (float)style.size;
        panel_decl.layout.sizing.width.size.minMax.max = (float)style.size;
        panel_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_GROW;
    } else {
        panel_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
        panel_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
        panel_decl.layout.sizing.height.size.minMax.min = (float)style.size;
        panel_decl.layout.sizing.height.size.minMax.max = (float)style.size;
    }

    /* Border on the inner edge */
    switch (cfg.side) {
        case CLAYKIT_DRAWER_LEFT:
            panel_decl.border.color = style.border_color;
            panel_decl.border.width.right = 1;
            break;
        case CLAYKIT_DRAWER_RIGHT:
            panel_decl.border.color = style.border_color;
            panel_decl.border.width.left = 1;
            break;
        case CLAYKIT_DRAWER_TOP:
            panel_decl.border.color = style.border_color;
            panel_decl.border.width.bottom = 1;
            break;
        case CLAYKIT_DRAWER_BOTTOM:
            panel_decl.border.color = style.border_color;
            panel_decl.border.width.top = 1;
            break;
    }

    Clay__OpenElement();
    bool panel_hovered = Clay_Hovered();
    Clay__ConfigureOpenElement(panel_decl);

    /* Return true if backdrop (not panel) is hovered - for close-on-backdrop logic */
    return backdrop_hovered && !panel_hovered;
}

void ClayKit_DrawerEnd(void) {
    Clay__CloseElement(); /* panel */
    Clay__CloseElement(); /* backdrop */
}

/* ----------------------------------------------------------------------------
 * Popover
 * ---------------------------------------------------------------------------- */

ClayKit_PopoverStyle ClayKit_ComputePopoverStyle(ClayKit_Context *ctx, ClayKit_PopoverConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_PopoverStyle style;

    style.bg_color = theme->bg;
    style.border_color = theme->border;
    style.padding = theme->spacing.md;
    style.corner_radius = theme->radius.md;
    style.z_index = cfg.z_index > 0 ? cfg.z_index : 50;

    return style;
}

void ClayKit_PopoverBegin(ClayKit_Context *ctx, const char *id, int32_t id_len, ClayKit_PopoverConfig cfg) {
    ClayKit_PopoverStyle style = ClayKit_ComputePopoverStyle(ctx, cfg);

    Clay_String id_str = { false, id_len, id };
    Clay_ElementDeclaration decl = {0};
    decl.id = Clay__HashString(id_str, 0, 0);
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = style.padding;
    decl.layout.padding.right = style.padding;
    decl.layout.padding.top = style.padding;
    decl.layout.padding.bottom = style.padding;
    decl.layout.childGap = 8;
    decl.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    decl.backgroundColor = style.bg_color;
    decl.cornerRadius.topLeft = (float)style.corner_radius;
    decl.cornerRadius.topRight = (float)style.corner_radius;
    decl.cornerRadius.bottomLeft = (float)style.corner_radius;
    decl.cornerRadius.bottomRight = (float)style.corner_radius;
    decl.border.color = style.border_color;
    decl.border.width.left = 1;
    decl.border.width.right = 1;
    decl.border.width.top = 1;
    decl.border.width.bottom = 1;

    /* Floating attached to parent */
    decl.floating.attachTo = CLAY_ATTACH_TO_PARENT;
    decl.floating.zIndex = (int16_t)style.z_index;
    decl.floating.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE;

    /* Position depends on config */
    switch (cfg.position) {
        case CLAYKIT_POPOVER_TOP:
            decl.floating.attachPoints.element = CLAY_ATTACH_POINT_LEFT_BOTTOM;
            decl.floating.attachPoints.parent = CLAY_ATTACH_POINT_LEFT_TOP;
            decl.floating.offset.y = -4;
            break;
        case CLAYKIT_POPOVER_BOTTOM:
            decl.floating.attachPoints.element = CLAY_ATTACH_POINT_LEFT_TOP;
            decl.floating.attachPoints.parent = CLAY_ATTACH_POINT_LEFT_BOTTOM;
            decl.floating.offset.y = 4;
            break;
        case CLAYKIT_POPOVER_LEFT:
            decl.floating.attachPoints.element = CLAY_ATTACH_POINT_RIGHT_TOP;
            decl.floating.attachPoints.parent = CLAY_ATTACH_POINT_LEFT_TOP;
            decl.floating.offset.x = -4;
            break;
        case CLAYKIT_POPOVER_RIGHT:
            decl.floating.attachPoints.element = CLAY_ATTACH_POINT_LEFT_TOP;
            decl.floating.attachPoints.parent = CLAY_ATTACH_POINT_RIGHT_TOP;
            decl.floating.offset.x = 4;
            break;
    }

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);
}

void ClayKit_PopoverEnd(void) {
    Clay__CloseElement();
}

/* ----------------------------------------------------------------------------
 * Button Rendering
 * ---------------------------------------------------------------------------- */

bool ClayKit_Button(ClayKit_Context *ctx, const char *text, int32_t text_len, ClayKit_ButtonConfig cfg) {
    uint16_t pad_x = ClayKit_ButtonPaddingX(ctx, cfg.size);
    uint16_t pad_y = ClayKit_ButtonPaddingY(ctx, cfg.size);
    uint16_t radius = ClayKit_ButtonRadius(ctx, cfg.size);
    uint16_t font_size = ClayKit_ButtonFontSize(ctx, cfg.size);
    uint16_t border_width = ClayKit_ButtonBorderWidth(cfg);

    /* Open element first, then check hover state */
    Clay__OpenElement();
    bool hovered = Clay_Hovered();

    Clay_Color bg_color = ClayKit_ButtonBgColor(ctx, cfg, hovered);
    Clay_Color text_color = ClayKit_ButtonTextColor(ctx, cfg);
    Clay_Color border_color = ClayKit_ButtonBorderColor(ctx, cfg);

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = pad_x;
    decl.layout.padding.right = pad_x;
    decl.layout.padding.top = pad_y;
    decl.layout.padding.bottom = pad_y;
    decl.layout.childGap = 8;
    decl.layout.childAlignment.x = CLAY_ALIGN_X_CENTER;
    decl.layout.childAlignment.y = CLAY_ALIGN_Y_CENTER;
    decl.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    decl.backgroundColor = bg_color;
    decl.cornerRadius.topLeft = (float)radius;
    decl.cornerRadius.topRight = (float)radius;
    decl.cornerRadius.bottomLeft = (float)radius;
    decl.cornerRadius.bottomRight = (float)radius;
    decl.border.color = border_color;
    decl.border.width.left = border_width;
    decl.border.width.right = border_width;
    decl.border.width.top = border_width;
    decl.border.width.bottom = border_width;

    Clay__ConfigureOpenElement(decl);

    Clay_String clay_text = { false, text_len, text };
    Clay_TextElementConfig text_config = {0};
    text_config.fontSize = font_size;
    text_config.fontId = ctx->theme_ptr->font_id.body;
    text_config.textColor = text_color;
    text_config.wrapMode = CLAY_TEXT_WRAP_NONE;

    Clay__OpenTextElement(clay_text, Clay__StoreTextElementConfig(text_config));

    Clay__CloseElement();
    return hovered;
}

/* ----------------------------------------------------------------------------
 * Checkbox Rendering
 * ---------------------------------------------------------------------------- */

bool ClayKit_Checkbox(ClayKit_Context *ctx, bool checked, ClayKit_CheckboxConfig cfg) {
    uint16_t size = ClayKit_CheckboxSize(ctx, cfg.size);
    ClayKit_Theme *theme = ctx->theme_ptr;

    Clay__OpenElement();
    bool hovered = Clay_Hovered();

    Clay_Color bg_color = ClayKit_CheckboxBgColor(ctx, cfg, checked, hovered);
    Clay_Color border_color = ClayKit_CheckboxBorderColor(ctx, cfg, checked);

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIXED;
    decl.layout.sizing.width.size.minMax.min = (float)size;
    decl.layout.sizing.width.size.minMax.max = (float)size;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
    decl.layout.sizing.height.size.minMax.min = (float)size;
    decl.layout.sizing.height.size.minMax.max = (float)size;
    decl.layout.childAlignment.x = CLAY_ALIGN_X_CENTER;
    decl.layout.childAlignment.y = CLAY_ALIGN_Y_CENTER;
    decl.backgroundColor = bg_color;
    decl.cornerRadius.topLeft = (float)theme->radius.sm;
    decl.cornerRadius.topRight = (float)theme->radius.sm;
    decl.cornerRadius.bottomLeft = (float)theme->radius.sm;
    decl.cornerRadius.bottomRight = (float)theme->radius.sm;
    decl.border.color = border_color;
    decl.border.width.left = 2;
    decl.border.width.right = 2;
    decl.border.width.top = 2;
    decl.border.width.bottom = 2;

    Clay__ConfigureOpenElement(decl);

    /* Draw checkmark when checked (inner white square) */
    if (checked) {
        uint16_t inner_size = size > 8 ? size - 8 : 2;
        Clay_ElementDeclaration inner_decl = {0};
        inner_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIXED;
        inner_decl.layout.sizing.width.size.minMax.min = (float)inner_size;
        inner_decl.layout.sizing.width.size.minMax.max = (float)inner_size;
        inner_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
        inner_decl.layout.sizing.height.size.minMax.min = (float)inner_size;
        inner_decl.layout.sizing.height.size.minMax.max = (float)inner_size;
        inner_decl.backgroundColor = (Clay_Color){ 255, 255, 255, 255 };
        inner_decl.cornerRadius.topLeft = 2;
        inner_decl.cornerRadius.topRight = 2;
        inner_decl.cornerRadius.bottomLeft = 2;
        inner_decl.cornerRadius.bottomRight = 2;

        Clay__OpenElement();
        Clay__ConfigureOpenElement(inner_decl);
        Clay__CloseElement();
    }

    Clay__CloseElement();
    return hovered;
}

/* ----------------------------------------------------------------------------
 * Radio Rendering
 * ---------------------------------------------------------------------------- */

bool ClayKit_Radio(ClayKit_Context *ctx, bool selected, ClayKit_RadioConfig cfg) {
    uint16_t size = ClayKit_RadioSize(ctx, cfg.size);
    float radius = (float)(size / 2);

    Clay__OpenElement();
    bool hovered = Clay_Hovered();

    Clay_Color bg_color = ClayKit_RadioBgColor(ctx, cfg, selected, hovered);
    Clay_Color border_color = ClayKit_RadioBorderColor(ctx, cfg, selected);

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIXED;
    decl.layout.sizing.width.size.minMax.min = (float)size;
    decl.layout.sizing.width.size.minMax.max = (float)size;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
    decl.layout.sizing.height.size.minMax.min = (float)size;
    decl.layout.sizing.height.size.minMax.max = (float)size;
    decl.layout.childAlignment.x = CLAY_ALIGN_X_CENTER;
    decl.layout.childAlignment.y = CLAY_ALIGN_Y_CENTER;
    decl.backgroundColor = bg_color;
    /* Circular: corner radius = size/2 */
    decl.cornerRadius.topLeft = radius;
    decl.cornerRadius.topRight = radius;
    decl.cornerRadius.bottomLeft = radius;
    decl.cornerRadius.bottomRight = radius;
    decl.border.color = border_color;
    decl.border.width.left = 2;
    decl.border.width.right = 2;
    decl.border.width.top = 2;
    decl.border.width.bottom = 2;

    Clay__ConfigureOpenElement(decl);

    /* Inner indicator circle when selected */
    if (selected) {
        uint16_t inner_size = size > 10 ? size - 10 : 4;
        float inner_radius = (float)(inner_size / 2);

        Clay_ElementDeclaration inner_decl = {0};
        inner_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIXED;
        inner_decl.layout.sizing.width.size.minMax.min = (float)inner_size;
        inner_decl.layout.sizing.width.size.minMax.max = (float)inner_size;
        inner_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
        inner_decl.layout.sizing.height.size.minMax.min = (float)inner_size;
        inner_decl.layout.sizing.height.size.minMax.max = (float)inner_size;
        inner_decl.backgroundColor = (Clay_Color){ 255, 255, 255, 255 };
        inner_decl.cornerRadius.topLeft = inner_radius;
        inner_decl.cornerRadius.topRight = inner_radius;
        inner_decl.cornerRadius.bottomLeft = inner_radius;
        inner_decl.cornerRadius.bottomRight = inner_radius;

        Clay__OpenElement();
        Clay__ConfigureOpenElement(inner_decl);
        Clay__CloseElement();
    }

    Clay__CloseElement();
    return hovered;
}

/* ----------------------------------------------------------------------------
 * Switch Rendering
 * ---------------------------------------------------------------------------- */

bool ClayKit_Switch(ClayKit_Context *ctx, bool on, ClayKit_SwitchConfig cfg) {
    uint16_t width = ClayKit_SwitchWidth(ctx, cfg.size);
    uint16_t height = ClayKit_SwitchHeight(ctx, cfg.size);
    uint16_t knob_size = ClayKit_SwitchKnobSize(ctx, cfg.size);
    uint16_t padding = (height - knob_size) / 2;

    Clay__OpenElement();
    bool hovered = Clay_Hovered();

    Clay_Color bg_color = ClayKit_SwitchBgColor(ctx, cfg, on, hovered);

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIXED;
    decl.layout.sizing.width.size.minMax.min = (float)width;
    decl.layout.sizing.width.size.minMax.max = (float)width;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
    decl.layout.sizing.height.size.minMax.min = (float)height;
    decl.layout.sizing.height.size.minMax.max = (float)height;
    decl.layout.padding.left = padding;
    decl.layout.padding.right = padding;
    decl.layout.padding.top = padding;
    decl.layout.padding.bottom = padding;
    decl.layout.childAlignment.x = on ? CLAY_ALIGN_X_RIGHT : CLAY_ALIGN_X_LEFT;
    decl.layout.childAlignment.y = CLAY_ALIGN_Y_CENTER;
    decl.backgroundColor = bg_color;
    decl.cornerRadius.topLeft = (float)(height / 2);
    decl.cornerRadius.topRight = (float)(height / 2);
    decl.cornerRadius.bottomLeft = (float)(height / 2);
    decl.cornerRadius.bottomRight = (float)(height / 2);

    Clay__ConfigureOpenElement(decl);

    /* Knob */
    Clay_ElementDeclaration knob_decl = {0};
    knob_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIXED;
    knob_decl.layout.sizing.width.size.minMax.min = (float)knob_size;
    knob_decl.layout.sizing.width.size.minMax.max = (float)knob_size;
    knob_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
    knob_decl.layout.sizing.height.size.minMax.min = (float)knob_size;
    knob_decl.layout.sizing.height.size.minMax.max = (float)knob_size;
    knob_decl.backgroundColor = (Clay_Color){ 255, 255, 255, 255 };
    knob_decl.cornerRadius.topLeft = (float)(knob_size / 2);
    knob_decl.cornerRadius.topRight = (float)(knob_size / 2);
    knob_decl.cornerRadius.bottomLeft = (float)(knob_size / 2);
    knob_decl.cornerRadius.bottomRight = (float)(knob_size / 2);

    Clay__OpenElement();
    Clay__ConfigureOpenElement(knob_decl);
    Clay__CloseElement();

    Clay__CloseElement();
    return hovered;
}

/* ----------------------------------------------------------------------------
 * Slider Rendering
 * ---------------------------------------------------------------------------- */

bool ClayKit_Slider(ClayKit_Context *ctx, float value, ClayKit_SliderConfig cfg) {
    float min_val = (cfg.min == 0.0f && cfg.max == 0.0f) ? 0.0f : cfg.min;
    float max_val = (cfg.min == 0.0f && cfg.max == 0.0f) ? 1.0f : cfg.max;
    float range = max_val - min_val;
    float normalized = (range > 0) ? (value - min_val) / range : 0.0f;
    float clamped = normalized < 0.0f ? 0.0f : (normalized > 1.0f ? 1.0f : normalized);

    Clay__OpenElement();
    bool hovered = Clay_Hovered();

    ClayKit_SliderStyle style = ClayKit_ComputeSliderStyle(ctx, cfg, hovered);

    /* Outer wrapper to allow child alignment */
    Clay_ElementDeclaration outer_decl = {0};
    outer_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    outer_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    outer_decl.layout.childAlignment.y = CLAY_ALIGN_Y_CENTER;

    Clay__ConfigureOpenElement(outer_decl);

    /* Track background */
    Clay_ElementDeclaration track_decl = {0};
    track_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    track_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
    track_decl.layout.sizing.height.size.minMax.min = (float)style.track_height;
    track_decl.layout.sizing.height.size.minMax.max = (float)style.track_height;
    track_decl.backgroundColor = style.track_color;
    track_decl.cornerRadius.topLeft = (float)style.corner_radius;
    track_decl.cornerRadius.topRight = (float)style.corner_radius;
    track_decl.cornerRadius.bottomLeft = (float)style.corner_radius;
    track_decl.cornerRadius.bottomRight = (float)style.corner_radius;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(track_decl);

    /* Filled portion */
    if (clamped > 0.0f) {
        Clay_ElementDeclaration fill_decl = {0};
        fill_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_PERCENT;
        fill_decl.layout.sizing.width.size.percent = clamped;
        fill_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_GROW;
        fill_decl.backgroundColor = style.fill_color;
        fill_decl.cornerRadius.topLeft = (float)style.corner_radius;
        fill_decl.cornerRadius.topRight = (float)style.corner_radius;
        fill_decl.cornerRadius.bottomLeft = (float)style.corner_radius;
        fill_decl.cornerRadius.bottomRight = (float)style.corner_radius;

        Clay__OpenElement();
        Clay__ConfigureOpenElement(fill_decl);
        Clay__CloseElement();
    }

    Clay__CloseElement(); /* track */
    Clay__CloseElement(); /* outer */
    return hovered;
}

/* ----------------------------------------------------------------------------
 * Single Tab Rendering
 * ---------------------------------------------------------------------------- */

bool ClayKit_Tab(ClayKit_Context *ctx, const char *label, int32_t label_len, bool is_active, ClayKit_TabsConfig cfg) {
    ClayKit_TabsStyle style = ClayKit_ComputeTabsStyle(ctx, cfg);

    Clay__OpenElement();
    bool hovered = Clay_Hovered();

    /* Determine colors */
    Clay_Color text_color = is_active ? style.active_text : style.inactive_color;
    Clay_Color bg_color = {0};

    if (cfg.variant == CLAYKIT_TABS_LINE) {
        bg_color = (Clay_Color){ 0, 0, 0, 0 };
    } else {
        bg_color = is_active ? style.active_color : (Clay_Color){ 0, 0, 0, 0 };
    }

    float corner_radius = (cfg.variant != CLAYKIT_TABS_LINE) ? (float)style.corner_radius : 0.0f;

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = style.padding_x;
    decl.layout.padding.right = style.padding_x;
    decl.layout.padding.top = style.padding_y;
    decl.layout.padding.bottom = style.padding_y;
    decl.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    decl.backgroundColor = bg_color;
    decl.cornerRadius.topLeft = corner_radius;
    decl.cornerRadius.topRight = corner_radius;
    decl.cornerRadius.bottomLeft = corner_radius;
    decl.cornerRadius.bottomRight = corner_radius;

    Clay__ConfigureOpenElement(decl);

    /* Tab label text */
    Clay_String clay_label = { false, label_len, label };
    Clay_TextElementConfig text_config = {0};
    text_config.fontSize = style.font_size;
    text_config.fontId = ctx->theme_ptr->font_id.body;
    text_config.textColor = text_color;
    text_config.wrapMode = CLAY_TEXT_WRAP_NONE;

    Clay__OpenTextElement(clay_label, Clay__StoreTextElementConfig(text_config));

    /* Underline indicator for line variant */
    if (cfg.variant == CLAYKIT_TABS_LINE && is_active) {
        Clay_ElementDeclaration indicator_decl = {0};
        indicator_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
        indicator_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
        indicator_decl.layout.sizing.height.size.minMax.min = (float)style.indicator_height;
        indicator_decl.layout.sizing.height.size.minMax.max = (float)style.indicator_height;
        indicator_decl.backgroundColor = style.active_color;

        Clay__OpenElement();
        Clay__ConfigureOpenElement(indicator_decl);
        Clay__CloseElement();
    }

    Clay__CloseElement();
    return hovered;
}

/* ----------------------------------------------------------------------------
 * Text Input Rendering
 * ---------------------------------------------------------------------------- */

bool ClayKit_TextInput(ClayKit_Context *ctx, const char *id, int32_t id_len,
                       ClayKit_InputState *state, ClayKit_InputConfig cfg,
                       const char *placeholder, int32_t placeholder_len) {
    bool focused = (state->flags & CLAYKIT_INPUT_FOCUSED) != 0;
    ClayKit_InputStyle style = ClayKit_ComputeInputStyle(ctx, cfg, focused);

    /* Cursor visibility based on blink time */
    bool show_cursor = focused && (((int)(ctx->cursor_blink_time * 2) % 2) == 0);

    Clay__OpenElement();
    bool hovered = Clay_Hovered();

    /* Outer container */
    Clay_ElementDeclaration outer_decl = {0};

    /* Set element ID if provided */
    if (id != NULL && id_len > 0) {
        Clay_String id_str = { false, id_len, id };
        outer_decl.id = Clay__HashString(id_str, 0, 0);
    }
    if (cfg.width > 0) {
        outer_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIXED;
        outer_decl.layout.sizing.width.size.minMax.min = (float)cfg.width;
        outer_decl.layout.sizing.width.size.minMax.max = (float)cfg.width;
    } else {
        outer_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    }
    outer_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    outer_decl.layout.padding.left = style.padding_x;
    outer_decl.layout.padding.right = style.padding_x;
    outer_decl.layout.padding.top = style.padding_y;
    outer_decl.layout.padding.bottom = style.padding_y;
    outer_decl.backgroundColor = style.bg_color;
    outer_decl.cornerRadius.topLeft = (float)style.corner_radius;
    outer_decl.cornerRadius.topRight = (float)style.corner_radius;
    outer_decl.cornerRadius.bottomLeft = (float)style.corner_radius;
    outer_decl.cornerRadius.bottomRight = (float)style.corner_radius;
    outer_decl.border.color = style.border_color;
    outer_decl.border.width.left = 1;
    outer_decl.border.width.right = 1;
    outer_decl.border.width.top = 1;
    outer_decl.border.width.bottom = 1;

    Clay__ConfigureOpenElement(outer_decl);

    /* Inner content container (horizontal layout) */
    Clay_ElementDeclaration inner_decl = {0};
    inner_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    inner_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    inner_decl.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    inner_decl.layout.childAlignment.y = CLAY_ALIGN_Y_CENTER;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(inner_decl);

    uint32_t cursor_pos = state->cursor;
    if (cursor_pos > state->len) cursor_pos = state->len;

    Clay_TextElementConfig text_config = {0};
    text_config.fontSize = style.font_size;
    text_config.fontId = style.font_id;
    text_config.wrapMode = CLAY_TEXT_WRAP_NONE;

    if (state->len > 0) {
        /* Text before cursor */
        if (cursor_pos > 0) {
            Clay_String text_before = { false, (int32_t)cursor_pos, state->buf };
            text_config.textColor = style.text_color;
            Clay__OpenTextElement(text_before, Clay__StoreTextElementConfig(text_config));
        }

        /* Cursor (render when focused, alpha controls visibility) */
        if (focused) {
            Clay_ElementDeclaration cursor_decl = {0};
            cursor_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIXED;
            cursor_decl.layout.sizing.width.size.minMax.min = (float)style.cursor_width;
            cursor_decl.layout.sizing.width.size.minMax.max = (float)style.cursor_width;
            cursor_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
            cursor_decl.layout.sizing.height.size.minMax.min = (float)style.font_size;
            cursor_decl.layout.sizing.height.size.minMax.max = (float)style.font_size;
            cursor_decl.backgroundColor = style.cursor_color;
            if (!show_cursor) {
                cursor_decl.backgroundColor.a = 0;
            }

            Clay__OpenElement();
            Clay__ConfigureOpenElement(cursor_decl);
            Clay__CloseElement();
        }

        /* Text after cursor */
        if (cursor_pos < state->len) {
            Clay_String text_after = { false, (int32_t)(state->len - cursor_pos), state->buf + cursor_pos };
            text_config.textColor = style.text_color;
            Clay__OpenTextElement(text_after, Clay__StoreTextElementConfig(text_config));
        }
    } else {
        /* No text - show cursor or placeholder */
        if (focused) {
            Clay_ElementDeclaration cursor_decl = {0};
            cursor_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIXED;
            cursor_decl.layout.sizing.width.size.minMax.min = (float)style.cursor_width;
            cursor_decl.layout.sizing.width.size.minMax.max = (float)style.cursor_width;
            cursor_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
            cursor_decl.layout.sizing.height.size.minMax.min = (float)style.font_size;
            cursor_decl.layout.sizing.height.size.minMax.max = (float)style.font_size;
            cursor_decl.backgroundColor = style.cursor_color;
            if (!show_cursor) {
                cursor_decl.backgroundColor.a = 0;
            }

            Clay__OpenElement();
            Clay__ConfigureOpenElement(cursor_decl);
            Clay__CloseElement();
        } else if (placeholder != NULL && placeholder_len > 0) {
            Clay_String placeholder_str = { false, placeholder_len, placeholder };
            text_config.textColor = style.placeholder_color;
            Clay__OpenTextElement(placeholder_str, Clay__StoreTextElementConfig(text_config));
        }
    }

    Clay__CloseElement(); /* inner */
    Clay__CloseElement(); /* outer */
    return hovered;
}

/* ----------------------------------------------------------------------------
 * Link
 * ---------------------------------------------------------------------------- */

ClayKit_LinkStyle ClayKit_ComputeLinkStyle(ClayKit_Context *ctx, ClayKit_LinkConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_LinkStyle style;

    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);

    style.text_color = scheme_color;
    style.hover_color = claykit_color_darken(scheme_color, 0.15f);
    style.disabled_color = theme->muted;
    style.font_size = ClayKit_GetFontSize(theme, cfg.size);
    style.font_id = theme->font_id.body;
    style.underline_height = 1;

    return style;
}

bool ClayKit_Link(ClayKit_Context *ctx, const char *text, int32_t text_len, ClayKit_LinkConfig cfg) {
    ClayKit_LinkStyle style = ClayKit_ComputeLinkStyle(ctx, cfg);

    Clay__OpenElement();
    bool hovered = Clay_Hovered();

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;

    Clay__ConfigureOpenElement(decl);

    /* Text */
    Clay_Color text_color;
    if (cfg.disabled) {
        text_color = style.disabled_color;
    } else if (hovered) {
        text_color = style.hover_color;
    } else {
        text_color = style.text_color;
    }

    Clay_String clay_text = { false, text_len, text };
    Clay_TextElementConfig text_config = {0};
    text_config.fontSize = style.font_size;
    text_config.fontId = style.font_id;
    text_config.textColor = text_color;
    text_config.wrapMode = CLAY_TEXT_WRAP_NONE;
    Clay__OpenTextElement(clay_text, Clay__StoreTextElementConfig(text_config));

    /* Underline */
    bool show_underline = false;
    if (!cfg.disabled) {
        switch (cfg.variant) {
            case CLAYKIT_LINK_UNDERLINE:
                show_underline = true;
                break;
            case CLAYKIT_LINK_HOVER_UNDERLINE:
                show_underline = hovered;
                break;
            case CLAYKIT_LINK_NONE:
            default:
                show_underline = false;
                break;
        }
    }

    if (show_underline) {
        Clay_ElementDeclaration underline_decl = {0};
        underline_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
        underline_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
        underline_decl.layout.sizing.height.size.minMax.min = (float)style.underline_height;
        underline_decl.layout.sizing.height.size.minMax.max = (float)style.underline_height;
        underline_decl.backgroundColor = text_color;

        Clay__OpenElement();
        Clay__ConfigureOpenElement(underline_decl);
        Clay__CloseElement();
    }

    Clay__CloseElement();
    return hovered && !cfg.disabled;
}

/* ----------------------------------------------------------------------------
 * Breadcrumb
 * ---------------------------------------------------------------------------- */

ClayKit_BreadcrumbStyle ClayKit_ComputeBreadcrumbStyle(ClayKit_Context *ctx, ClayKit_BreadcrumbConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_BreadcrumbStyle style;

    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);

    style.link_color = scheme_color;
    style.hover_color = claykit_color_darken(scheme_color, 0.15f);
    style.current_color = theme->fg;
    style.separator_color = theme->muted;
    style.font_size = ClayKit_GetFontSize(theme, cfg.size);
    style.font_id = theme->font_id.body;

    switch (cfg.size) {
        case CLAYKIT_SIZE_XS: style.gap = 4; break;
        case CLAYKIT_SIZE_SM: style.gap = 6; break;
        case CLAYKIT_SIZE_LG: style.gap = 10; break;
        case CLAYKIT_SIZE_XL: style.gap = 12; break;
        case CLAYKIT_SIZE_MD:
        default: style.gap = 8; break;
    }

    return style;
}

void ClayKit_BreadcrumbBegin(ClayKit_Context *ctx, ClayKit_BreadcrumbConfig cfg) {
    ClayKit_BreadcrumbStyle style = ClayKit_ComputeBreadcrumbStyle(ctx, cfg);

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.childGap = style.gap;
    decl.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    decl.layout.childAlignment.y = CLAY_ALIGN_Y_CENTER;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);
}

bool ClayKit_BreadcrumbItem(ClayKit_Context *ctx, const char *text, int32_t text_len, bool is_current, ClayKit_BreadcrumbConfig cfg) {
    ClayKit_BreadcrumbStyle style = ClayKit_ComputeBreadcrumbStyle(ctx, cfg);

    Clay__OpenElement();
    bool hovered = Clay_Hovered();

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;

    Clay__ConfigureOpenElement(decl);

    Clay_Color text_color;
    if (is_current) {
        text_color = style.current_color;
    } else if (hovered) {
        text_color = style.hover_color;
    } else {
        text_color = style.link_color;
    }

    Clay_String clay_text = { false, text_len, text };
    Clay_TextElementConfig text_config = {0};
    text_config.fontSize = style.font_size;
    text_config.fontId = style.font_id;
    text_config.textColor = text_color;
    text_config.wrapMode = CLAY_TEXT_WRAP_NONE;
    Clay__OpenTextElement(clay_text, Clay__StoreTextElementConfig(text_config));

    Clay__CloseElement();
    return hovered && !is_current;
}

void ClayKit_BreadcrumbSeparator(ClayKit_Context *ctx, ClayKit_BreadcrumbConfig cfg) {
    ClayKit_BreadcrumbStyle style = ClayKit_ComputeBreadcrumbStyle(ctx, cfg);

    const char *sep = (cfg.separator != NULL && cfg.separator_len > 0) ? cfg.separator : "/";
    int32_t sep_len = (cfg.separator != NULL && cfg.separator_len > 0) ? cfg.separator_len : 1;

    Clay_String sep_str = { false, sep_len, sep };
    Clay_TextElementConfig text_config = {0};
    text_config.fontSize = style.font_size;
    text_config.fontId = style.font_id;
    text_config.textColor = style.separator_color;
    text_config.wrapMode = CLAY_TEXT_WRAP_NONE;
    Clay__OpenTextElement(sep_str, Clay__StoreTextElementConfig(text_config));
}

void ClayKit_BreadcrumbEnd(void) {
    Clay__CloseElement();
}

/* ----------------------------------------------------------------------------
 * Accordion
 * ---------------------------------------------------------------------------- */

ClayKit_AccordionStyle ClayKit_ComputeAccordionStyle(ClayKit_Context *ctx, ClayKit_AccordionConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_AccordionStyle style;

    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);

    style.header_bg = theme->bg;
    style.header_hover_bg = claykit_color_darken(theme->bg, 0.03f);
    style.header_text = theme->fg;
    style.active_accent = scheme_color;
    style.content_bg = theme->bg;
    style.border_color = theme->border;
    style.font_id = theme->font_id.body;

    switch (cfg.size) {
        case CLAYKIT_SIZE_XS:
            style.padding_x = 8;
            style.padding_y = 6;
            style.content_padding = 8;
            style.font_size = theme->font_size.xs;
            break;
        case CLAYKIT_SIZE_SM:
            style.padding_x = 10;
            style.padding_y = 8;
            style.content_padding = 10;
            style.font_size = theme->font_size.sm;
            break;
        case CLAYKIT_SIZE_LG:
            style.padding_x = 16;
            style.padding_y = 14;
            style.content_padding = 16;
            style.font_size = theme->font_size.lg;
            break;
        case CLAYKIT_SIZE_XL:
            style.padding_x = 20;
            style.padding_y = 16;
            style.content_padding = 20;
            style.font_size = theme->font_size.xl;
            break;
        case CLAYKIT_SIZE_MD:
        default:
            style.padding_x = 12;
            style.padding_y = 10;
            style.content_padding = 12;
            style.font_size = theme->font_size.md;
            break;
    }

    style.border_width = 1;

    if (cfg.variant == CLAYKIT_ACCORDION_SEPARATED) {
        style.corner_radius = ClayKit_GetRadius(theme, cfg.size);
        style.gap = 8;
    } else {
        style.corner_radius = 0;
        style.gap = 0;
    }

    return style;
}

void ClayKit_AccordionBegin(ClayKit_Context *ctx, ClayKit_AccordionConfig cfg) {
    ClayKit_AccordionStyle style = ClayKit_ComputeAccordionStyle(ctx, cfg);

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.childGap = style.gap;
    decl.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;

    /* Bordered: outer border, no corner radius (children have their own bg
       so rounding the container would show corners behind them) */
    if (cfg.variant == CLAYKIT_ACCORDION_BORDERED) {
        decl.border.color = style.border_color;
        decl.border.width.left = style.border_width;
        decl.border.width.right = style.border_width;
        decl.border.width.top = style.border_width;
        decl.border.width.bottom = style.border_width;
    }

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);
}

void ClayKit_AccordionItemBegin(ClayKit_Context *ctx, bool is_open, ClayKit_AccordionConfig cfg) {
    (void)is_open;
    ClayKit_AccordionStyle style = ClayKit_ComputeAccordionStyle(ctx, cfg);

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    /* No gap: header and content must be flush */

    (void)style;
    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);
}

void ClayKit_AccordionItemEnd(void) {
    Clay__CloseElement();
}

bool ClayKit_AccordionHeader(ClayKit_Context *ctx, const char *text, int32_t text_len, bool is_open, ClayKit_AccordionConfig cfg) {
    ClayKit_AccordionStyle style = ClayKit_ComputeAccordionStyle(ctx, cfg);

    Clay__OpenElement();
    bool hovered = Clay_Hovered();

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = style.padding_x;
    decl.layout.padding.right = style.padding_x;
    decl.layout.padding.top = style.padding_y;
    decl.layout.padding.bottom = style.padding_y;
    decl.layout.childGap = 8;
    decl.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    decl.layout.childAlignment.y = CLAY_ALIGN_Y_CENTER;
    decl.backgroundColor = hovered ? style.header_hover_bg : style.header_bg;

    if (cfg.variant == CLAYKIT_ACCORDION_SEPARATED) {
        /* Separated: header carries its own border and corners */
        decl.cornerRadius.topLeft = (float)style.corner_radius;
        decl.cornerRadius.topRight = (float)style.corner_radius;
        if (!is_open) {
            decl.cornerRadius.bottomLeft = (float)style.corner_radius;
            decl.cornerRadius.bottomRight = (float)style.corner_radius;
        }
        decl.border.color = style.border_color;
        decl.border.width.left = style.border_width;
        decl.border.width.right = style.border_width;
        decl.border.width.top = style.border_width;
        if (!is_open) {
            decl.border.width.bottom = style.border_width;
        }
    } else {
        /* Bordered: thin separator between items */
        decl.border.color = style.border_color;
        decl.border.width.bottom = style.border_width;
    }

    Clay__ConfigureOpenElement(decl);

    /* Text label (grows to push chevron right) */
    {
        Clay_ElementDeclaration text_wrapper = {0};
        text_wrapper.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
        text_wrapper.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;

        Clay__OpenElement();
        Clay__ConfigureOpenElement(text_wrapper);

        Clay_String clay_text = { false, text_len, text };
        Clay_TextElementConfig text_config = {0};
        text_config.fontSize = style.font_size;
        text_config.fontId = style.font_id;
        text_config.textColor = style.header_text;
        text_config.wrapMode = CLAY_TEXT_WRAP_NONE;
        Clay__OpenTextElement(clay_text, Clay__StoreTextElementConfig(text_config));

        Clay__CloseElement();
    }

    /* Chevron indicator */
    {
        /* U+25B2 BLACK UP-POINTING TRIANGLE = \xe2\x96\xb2 */
        /* U+25BC BLACK DOWN-POINTING TRIANGLE = \xe2\x96\xbc */
        Clay_String chevron = is_open
            ? (Clay_String){ false, 3, "\xe2\x96\xb2" }
            : (Clay_String){ false, 3, "\xe2\x96\xbc" };
        Clay_TextElementConfig chevron_cfg = {0};
        chevron_cfg.fontSize = style.font_size > 4 ? style.font_size - 4 : style.font_size;
        chevron_cfg.fontId = style.font_id;
        chevron_cfg.textColor = is_open ? style.active_accent : style.header_text;
        Clay__OpenTextElement(chevron, Clay__StoreTextElementConfig(chevron_cfg));
    }

    Clay__CloseElement();
    return hovered;
}

void ClayKit_AccordionContentBegin(ClayKit_Context *ctx, ClayKit_AccordionConfig cfg) {
    ClayKit_AccordionStyle style = ClayKit_ComputeAccordionStyle(ctx, cfg);

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = style.content_padding;
    decl.layout.padding.right = style.content_padding;
    decl.layout.padding.top = style.content_padding;
    decl.layout.padding.bottom = style.content_padding;
    decl.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    decl.backgroundColor = style.content_bg;

    if (cfg.variant == CLAYKIT_ACCORDION_SEPARATED) {
        decl.border.color = style.border_color;
        decl.border.width.left = style.border_width;
        decl.border.width.right = style.border_width;
        decl.border.width.bottom = style.border_width;
        decl.cornerRadius.bottomLeft = (float)style.corner_radius;
        decl.cornerRadius.bottomRight = (float)style.corner_radius;
    }

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);
}

void ClayKit_AccordionContentEnd(void) {
    Clay__CloseElement();
}

void ClayKit_AccordionEnd(void) {
    Clay__CloseElement();
}

/* ----------------------------------------------------------------------------
 * Menu
 * ---------------------------------------------------------------------------- */

ClayKit_MenuStyle ClayKit_ComputeMenuStyle(ClayKit_Context *ctx, ClayKit_MenuConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    ClayKit_MenuStyle style;

    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);

    style.bg_color = theme->bg;
    style.border_color = theme->border;
    style.text_color = theme->fg;
    style.disabled_text = theme->muted;
    style.hover_bg = claykit_color_lighten(scheme_color, 0.9f);
    style.separator_color = theme->border;
    style.font_id = theme->font_id.body;
    style.corner_radius = ClayKit_GetRadius(theme, cfg.size);

    switch (cfg.size) {
        case CLAYKIT_SIZE_XS:
            style.padding_x = 8;
            style.padding_y = 4;
            style.font_size = theme->font_size.xs;
            style.item_gap = 0;
            style.separator_height = 1;
            style.dropdown_padding = 4;
            break;
        case CLAYKIT_SIZE_SM:
            style.padding_x = 10;
            style.padding_y = 5;
            style.font_size = theme->font_size.sm;
            style.item_gap = 0;
            style.separator_height = 1;
            style.dropdown_padding = 4;
            break;
        case CLAYKIT_SIZE_LG:
            style.padding_x = 16;
            style.padding_y = 10;
            style.font_size = theme->font_size.lg;
            style.item_gap = 2;
            style.separator_height = 1;
            style.dropdown_padding = 6;
            break;
        case CLAYKIT_SIZE_XL:
            style.padding_x = 20;
            style.padding_y = 12;
            style.font_size = theme->font_size.xl;
            style.item_gap = 4;
            style.separator_height = 1;
            style.dropdown_padding = 8;
            break;
        case CLAYKIT_SIZE_MD:
        default:
            style.padding_x = 12;
            style.padding_y = 8;
            style.font_size = theme->font_size.md;
            style.item_gap = 2;
            style.separator_height = 1;
            style.dropdown_padding = 4;
            break;
    }

    return style;
}

void ClayKit_MenuDropdownBegin(ClayKit_Context *ctx, const char *id, int32_t id_len, ClayKit_MenuConfig cfg) {
    ClayKit_MenuStyle style = ClayKit_ComputeMenuStyle(ctx, cfg);

    Clay_String id_str = { false, id_len, id };
    Clay_ElementDeclaration decl = {0};
    decl.id = Clay__HashString(id_str, 0, 0);
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.sizing.width.size.minMax.min = 150;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = style.dropdown_padding;
    decl.layout.padding.right = style.dropdown_padding;
    decl.layout.padding.top = style.dropdown_padding;
    decl.layout.padding.bottom = style.dropdown_padding;
    decl.layout.childGap = style.item_gap;
    decl.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    decl.backgroundColor = style.bg_color;
    decl.cornerRadius.topLeft = (float)style.corner_radius;
    decl.cornerRadius.topRight = (float)style.corner_radius;
    decl.cornerRadius.bottomLeft = (float)style.corner_radius;
    decl.cornerRadius.bottomRight = (float)style.corner_radius;
    decl.border.color = style.border_color;
    decl.border.width.left = 1;
    decl.border.width.right = 1;
    decl.border.width.top = 1;
    decl.border.width.bottom = 1;
    decl.floating.attachTo = CLAY_ATTACH_TO_PARENT;
    decl.floating.attachPoints.element = CLAY_ATTACH_POINT_LEFT_TOP;
    decl.floating.attachPoints.parent = CLAY_ATTACH_POINT_LEFT_BOTTOM;
    decl.floating.zIndex = 100;
    decl.floating.offset.y = 4;
    decl.floating.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);
}

bool ClayKit_MenuItem(ClayKit_Context *ctx, const char *text, int32_t text_len, bool disabled, ClayKit_MenuConfig cfg) {
    ClayKit_MenuStyle style = ClayKit_ComputeMenuStyle(ctx, cfg);

    Clay__OpenElement();
    bool hovered = Clay_Hovered();

    Clay_Color bg = (Clay_Color){ 0, 0, 0, 0 };
    if (hovered && !disabled) {
        bg = style.hover_bg;
    }

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
    decl.layout.padding.left = style.padding_x;
    decl.layout.padding.right = style.padding_x;
    decl.layout.padding.top = style.padding_y > 2 ? style.padding_y - 2 : style.padding_y;
    decl.layout.padding.bottom = style.padding_y > 2 ? style.padding_y - 2 : style.padding_y;
    decl.backgroundColor = bg;
    decl.cornerRadius.topLeft = (float)(style.corner_radius > 2 ? style.corner_radius - 2 : style.corner_radius);
    decl.cornerRadius.topRight = decl.cornerRadius.topLeft;
    decl.cornerRadius.bottomLeft = decl.cornerRadius.topLeft;
    decl.cornerRadius.bottomRight = decl.cornerRadius.topLeft;

    Clay__ConfigureOpenElement(decl);

    Clay_String text_str = { false, text_len, text };
    Clay_TextElementConfig text_config = {0};
    text_config.fontSize = style.font_size;
    text_config.textColor = disabled ? style.disabled_text : style.text_color;
    text_config.fontId = style.font_id;
    text_config.wrapMode = CLAY_TEXT_WRAP_NONE;
    Clay__OpenTextElement(text_str, Clay__StoreTextElementConfig(text_config));

    Clay__CloseElement();
    return hovered && !disabled;
}

void ClayKit_MenuSeparator(ClayKit_Context *ctx, ClayKit_MenuConfig cfg) {
    ClayKit_MenuStyle style = ClayKit_ComputeMenuStyle(ctx, cfg);

    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
    decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIXED;
    decl.layout.sizing.height.size.minMax.min = (float)style.separator_height;
    decl.layout.sizing.height.size.minMax.max = (float)style.separator_height;
    decl.backgroundColor = style.separator_color;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);
    Clay__CloseElement();
}

void ClayKit_MenuDropdownEnd(void) {
    Clay__CloseElement();
}

#endif /* CLAYKIT_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* CLAY_KIT_H */
