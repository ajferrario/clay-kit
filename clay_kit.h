/*
 * clay_kit.h - Zero-allocation UI components for Clay
 * Single-header C99, Zig-compatible
 *
 * Usage:
 *   #define CLAYKIT_IMPLEMENTATION
 *   #include "clay_kit.h"
 *
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

/* Badge - renders a badge element, call within a CLAY() block */
void ClayKit_Badge(ClayKit_Context *ctx, Clay_String text, ClayKit_BadgeConfig cfg);

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
 * Switch Configuration
 * ============================================================================ */

typedef struct ClayKit_SwitchConfig {
    ClayKit_ColorScheme color_scheme;  /* Color when on */
    ClayKit_Size size;                 /* Size of switch */
    bool disabled;                     /* Disabled state */
} ClayKit_SwitchConfig;

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
    uint16_t width;              /* Fixed width (0 = grow to fill) */
} ClayKit_InputConfig;

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

/* Checkbox helper functions */
uint16_t ClayKit_CheckboxSize(ClayKit_Context *ctx, ClayKit_Size size);
Clay_Color ClayKit_CheckboxBgColor(ClayKit_Context *ctx, ClayKit_CheckboxConfig cfg, bool checked, bool hovered);
Clay_Color ClayKit_CheckboxBorderColor(ClayKit_Context *ctx, ClayKit_CheckboxConfig cfg, bool checked);

/* Switch helper functions */
uint16_t ClayKit_SwitchWidth(ClayKit_Context *ctx, ClayKit_Size size);
uint16_t ClayKit_SwitchHeight(ClayKit_Context *ctx, ClayKit_Size size);
uint16_t ClayKit_SwitchKnobSize(ClayKit_Context *ctx, ClayKit_Size size);
Clay_Color ClayKit_SwitchBgColor(ClayKit_Context *ctx, ClayKit_SwitchConfig cfg, bool on, bool hovered);

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

void ClayKit_Badge(ClayKit_Context *ctx, Clay_String text, ClayKit_BadgeConfig cfg) {
    ClayKit_Theme *theme = ctx->theme_ptr;
    Clay_Color scheme_color = ClayKit_GetSchemeColor(theme, cfg.color_scheme);

    /* Determine padding based on size */
    uint16_t pad_x, pad_y, font_size;
    switch (cfg.size) {
        case CLAYKIT_SIZE_XS:
            pad_x = 4; pad_y = 1; font_size = theme->font_size.xs; break;
        case CLAYKIT_SIZE_SM:
            pad_x = 6; pad_y = 2; font_size = theme->font_size.xs; break;
        case CLAYKIT_SIZE_LG:
            pad_x = 10; pad_y = 4; font_size = theme->font_size.md; break;
        case CLAYKIT_SIZE_XL:
            pad_x = 12; pad_y = 5; font_size = theme->font_size.lg; break;
        case CLAYKIT_SIZE_MD:
        default:
            pad_x = 8; pad_y = 3; font_size = theme->font_size.sm; break;
    }

    /* Determine colors based on variant */
    Clay_Color bg_color, text_color, border_color;
    uint16_t border_width = 0;

    switch (cfg.variant) {
        case CLAYKIT_BADGE_SOLID:
            bg_color = scheme_color;
            text_color = (Clay_Color){ 255, 255, 255, 255 };
            border_color = (Clay_Color){ 0, 0, 0, 0 };
            break;
        case CLAYKIT_BADGE_SUBTLE:
            bg_color = claykit_color_lighten(scheme_color, 0.85f);
            text_color = scheme_color;
            border_color = (Clay_Color){ 0, 0, 0, 0 };
            break;
        case CLAYKIT_BADGE_OUTLINE:
            bg_color = (Clay_Color){ 0, 0, 0, 0 };
            text_color = scheme_color;
            border_color = scheme_color;
            border_width = 1;
            break;
        default:
            bg_color = scheme_color;
            text_color = (Clay_Color){ 255, 255, 255, 255 };
            border_color = (Clay_Color){ 0, 0, 0, 0 };
            break;
    }

    /* Render badge container with text */
    CLAY_AUTO_ID({
        .layout = {
            .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) },
            .padding = { pad_x, pad_x, pad_y, pad_y }
        },
        .backgroundColor = bg_color,
        .cornerRadius = CLAY_CORNER_RADIUS(theme->radius.full),
        .border = { .color = border_color, .width = { border_width, border_width, border_width, border_width, 0 } }
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontSize = font_size,
            .fontId = theme->font_id.body,
            .textColor = text_color,
            .wrapMode = CLAY_TEXT_WRAP_NONE
        }));
    }
}

#endif /* CLAYKIT_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* CLAY_KIT_H */
