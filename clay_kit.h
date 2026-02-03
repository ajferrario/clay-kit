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
