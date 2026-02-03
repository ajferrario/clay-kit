/*
 * clay_kit.h - Zero-allocation UI components for Clay
 * Single-header C99, Zig-compatible
 *
 * Usage:
 *   #define CLAYKIT_IMPLEMENTATION
 *   #include "clay_kit.h"
 *
 * Requires clay.h to be included before this header.
 */

#ifndef CLAY_KIT_H
#define CLAY_KIT_H

#include <stdint.h>
#include <stdbool.h>

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

#endif /* CLAYKIT_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* CLAY_KIT_H */
