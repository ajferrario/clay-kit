//! clay_kit.zig - Hand-written Zig bindings for clay_kit.h
//! Zero-allocation UI components for Clay

const std = @import("std");

// Re-export zclay types for convenience
pub const zclay = @import("zclay");

// ============================================================================
// Clay Types (matching vendor/clay.h)
// ============================================================================

pub const Color = extern struct {
    r: f32 = 0,
    g: f32 = 0,
    b: f32 = 0,
    a: f32 = 0,

    pub fn rgba(r: f32, g: f32, b: f32, a: f32) Color {
        return .{ .r = r, .g = g, .b = b, .a = a };
    }

    pub fn rgb(r: f32, g: f32, b: f32) Color {
        return .{ .r = r, .g = g, .b = b, .a = 255 };
    }
};

pub const BoundingBox = extern struct {
    x: f32 = 0,
    y: f32 = 0,
    width: f32 = 0,
    height: f32 = 0,
};

pub const String = extern struct {
    chars: [*c]u8 = null,
    length: c_int = 0,
};

pub const ElementId = extern struct {
    id: u32 = 0,
    offset: u32 = 0,
    base_id: u32 = 0,
    string_id: String = .{},
};

pub const LayoutAlignmentX = enum(u8) {
    left = 0,
    right = 1,
    center = 2,
};

pub const LayoutAlignmentY = enum(u8) {
    top = 0,
    bottom = 1,
    center = 2,
};

pub const ChildAlignment = extern struct {
    x: LayoutAlignmentX = .left,
    y: LayoutAlignmentY = .top,
};

pub const LayoutDirection = enum(u8) {
    left_to_right = 0,
    top_to_bottom = 1,
};

pub const SizingType = enum(u8) {
    fit = 0,
    grow = 1,
    percent = 2,
    fixed = 3,
};

pub const SizingMinMax = extern struct {
    min: f32 = 0,
    max: f32 = 0,
};

pub const SizingAxis = extern struct {
    size: extern union {
        min_max: SizingMinMax,
        percent: f32,
    } = .{ .min_max = .{} },
    type: SizingType = .fit,
};

pub const Sizing = extern struct {
    width: SizingAxis = .{},
    height: SizingAxis = .{},

    pub fn fixed(w: f32, h: f32) Sizing {
        return .{
            .width = .{ .size = .{ .min_max = .{ .min = w, .max = w } }, .type = .fixed },
            .height = .{ .size = .{ .min_max = .{ .min = h, .max = h } }, .type = .fixed },
        };
    }

    pub fn grow() Sizing {
        return .{
            .width = .{ .type = .grow },
            .height = .{ .type = .grow },
        };
    }

    pub fn fit() Sizing {
        return .{
            .width = .{ .type = .fit },
            .height = .{ .type = .fit },
        };
    }
};

pub const Padding = extern struct {
    left: u16 = 0,
    right: u16 = 0,
    top: u16 = 0,
    bottom: u16 = 0,

    pub fn all(p: u16) Padding {
        return .{ .left = p, .right = p, .top = p, .bottom = p };
    }

    pub fn xy(x: u16, y: u16) Padding {
        return .{ .left = x, .right = x, .top = y, .bottom = y };
    }
};

pub const LayoutConfig = extern struct {
    sizing: Sizing = .{},
    padding: Padding = .{},
    child_gap: u16 = 0,
    child_alignment: ChildAlignment = .{},
    layout_direction: LayoutDirection = .left_to_right,
};

// ============================================================================
// ClayKit Icon System
// ============================================================================

pub const Icon = extern struct {
    id: u16 = 0, // 0 = no icon
    size: u16 = 0, // pixels
};

pub const IconCallback = ?*const fn (icon_id: u16, box: BoundingBox, user_data: ?*anyopaque) callconv(.c) void;

// ============================================================================
// ClayKit Component State
// ============================================================================

pub const State = extern struct {
    id: u32 = 0,
    flags: u32 = 0, // component-specific bits
    value: f32 = 0, // for sliders, progress, etc.
};

// ============================================================================
// ClayKit Theme System
// ============================================================================

pub const SpacingScale = extern struct {
    xs: u16 = 4,
    sm: u16 = 8,
    md: u16 = 16,
    lg: u16 = 24,
    xl: u16 = 32,
};

pub const RadiusScale = extern struct {
    sm: u16 = 4,
    md: u16 = 8,
    lg: u16 = 12,
    full: u16 = 9999,
};

pub const FontIds = extern struct {
    body: u16 = 0,
    heading: u16 = 0,
};

pub const FontSizeScale = extern struct {
    xs: u16 = 12,
    sm: u16 = 14,
    md: u16 = 16,
    lg: u16 = 18,
    xl: u16 = 24,
};

pub const Theme = extern struct {
    // Color palette
    primary: Color = .{ .r = 66, .g = 133, .b = 244, .a = 255 },
    secondary: Color = .{ .r = 156, .g = 163, .b = 175, .a = 255 },
    success: Color = .{ .r = 34, .g = 197, .b = 94, .a = 255 },
    warning: Color = .{ .r = 251, .g = 191, .b = 36, .a = 255 },
    @"error": Color = .{ .r = 239, .g = 68, .b = 68, .a = 255 },

    // Semantic colors
    bg: Color = .{ .r = 255, .g = 255, .b = 255, .a = 255 },
    fg: Color = .{ .r = 17, .g = 24, .b = 39, .a = 255 },
    border: Color = .{ .r = 229, .g = 231, .b = 235, .a = 255 },
    muted: Color = .{ .r = 107, .g = 114, .b = 128, .a = 255 },

    // Scales
    spacing: SpacingScale = .{},
    radius: RadiusScale = .{},
    font_id: FontIds = .{},
    font_size: FontSizeScale = .{},

    pub const light = Theme{};

    pub const dark = Theme{
        .primary = .{ .r = 96, .g = 165, .b = 250, .a = 255 },
        .secondary = .{ .r = 156, .g = 163, .b = 175, .a = 255 },
        .success = .{ .r = 74, .g = 222, .b = 128, .a = 255 },
        .warning = .{ .r = 251, .g = 191, .b = 36, .a = 255 },
        .@"error" = .{ .r = 248, .g = 113, .b = 113, .a = 255 },
        .bg = .{ .r = 17, .g = 24, .b = 39, .a = 255 },
        .fg = .{ .r = 249, .g = 250, .b = 251, .a = 255 },
        .border = .{ .r = 55, .g = 65, .b = 81, .a = 255 },
        .muted = .{ .r = 156, .g = 163, .b = 175, .a = 255 },
    };
};

// ============================================================================
// ClayKit Context
// ============================================================================

pub const Context = extern struct {
    theme_ptr: ?*Theme = null,
    state_ptr: ?[*]State = null,
    state_count: u32 = 0,
    state_cap: u32 = 0,
    focused_id: u32 = 0,
    prev_focused_id: u32 = 0,
    icon_callback: IconCallback = null,
    icon_user_data: ?*anyopaque = null,

    pub fn theme(self: *Context) *Theme {
        return self.theme_ptr.?;
    }
};

// ============================================================================
// ClayKit Enums
// ============================================================================

pub const Size = enum(c_int) {
    xs = 0,
    sm = 1,
    md = 2,
    lg = 3,
    xl = 4,
};

pub const ColorScheme = enum(c_int) {
    primary = 0,
    secondary = 1,
    success = 2,
    warning = 3,
    @"error" = 4,
};

pub const InputFlags = enum(u8) {
    focused = 1 << 0,
    password = 1 << 1,
    readonly = 1 << 2,
    disabled = 1 << 3,
};

pub const Key = enum(c_int) {
    none = 0,
    backspace = 1,
    delete = 2,
    left = 3,
    right = 4,
    home = 5,
    end = 6,
    enter = 7,
    tab = 8,
};

pub const Modifier = enum(c_int) {
    none = 0,
    shift = 1 << 0,
    ctrl = 1 << 1,
    alt = 1 << 2,
};

// ============================================================================
// ClayKit Text Input State
// ============================================================================

pub const InputState = extern struct {
    buf: [*c]u8 = null,
    cap: u32 = 0,
    len: u32 = 0,
    cursor: u32 = 0,
    select_start: u32 = 0, // == cursor when no selection
    flags: u8 = 0,
};

// ============================================================================
// ClayKit Layout Config Structs
// ============================================================================

pub const BoxConfig = extern struct {
    bg: Color = .{},
    border_color: Color = .{},
    border_width: u16 = 0,
    padding: u16 = 0,
    radius: u16 = 0,
    sizing: Sizing = .{},
};

pub const FlexConfig = extern struct {
    direction: LayoutDirection = .left_to_right,
    gap: u16 = 0,
    @"align": ChildAlignment = .{},
    padding: u16 = 0,
    sizing: Sizing = .{},
    bg: Color = .{},
};

pub const StackDirection = enum(c_int) {
    vertical = 0, // Top to bottom (VStack)
    horizontal = 1, // Left to right (HStack)
};

pub const StackConfig = extern struct {
    direction: StackDirection = .vertical,
    gap: u16 = 0,
    @"align": ChildAlignment = .{},
    padding: u16 = 0,
    sizing: Sizing = .{},
    bg: Color = .{},
};

pub const ContainerConfig = extern struct {
    max_width: u16 = 0, // 0 = use theme default 1200
    padding: u16 = 0,
    bg: Color = .{},
};

// ============================================================================
// Typography Configuration
// ============================================================================

pub const TextConfig = extern struct {
    size: Size = .md, // Font size from theme scale
    color: Color = .{}, // Text color (default: theme fg)
    font_id: u16 = 0, // Font ID (default: theme body font)
    letter_spacing: u16 = 0, // Letter spacing in pixels
    line_height: u16 = 0, // Line height (0 = auto)
};

pub const HeadingConfig = extern struct {
    size: Size = .xl, // XS=h6, SM=h5, MD=h4, LG=h3, XL=h2
    color: Color = .{}, // Text color (default: theme fg)
    font_id: u16 = 0, // Font ID (default: theme heading font)
};

pub const BadgeVariant = enum(c_int) {
    solid = 0, // Solid background
    subtle = 1, // Light background, colored text
    outline = 2, // Transparent with border
};

pub const BadgeConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    variant: BadgeVariant = .solid,
    size: Size = .md,
};

// ============================================================================
// Button Configuration
// ============================================================================

pub const ButtonVariant = enum(c_int) {
    solid = 0, // Solid background
    outline = 1, // Transparent with border
    ghost = 2, // Transparent, no border
};

pub const ButtonConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    variant: ButtonVariant = .solid,
    size: Size = .md,
    disabled: bool = false,
    icon_left: Icon = .{},
    icon_right: Icon = .{},
};

pub const ButtonResult = extern struct {
    clicked: bool = false,
    hovered: bool = false,
    pressed: bool = false,
};

// ============================================================================
// Input Configuration
// ============================================================================

pub const InputConfig = extern struct {
    size: Size = .md,
    bg: Color = .{}, // Background color (default: theme bg)
    border_color: Color = .{}, // Border color (default: theme border)
    focus_color: Color = .{}, // Border when focused (default: theme primary)
    text_color: Color = .{}, // Text color (default: theme fg)
    placeholder_color: Color = .{}, // Placeholder color (default: theme muted)
    width: u16 = 0, // Fixed width (0 = grow)
};

// ============================================================================
// External C Functions
// ============================================================================

extern fn ClayKit_Init(ctx: *Context, theme: *Theme, state_buf: [*]State, state_cap: u32) void;
extern fn ClayKit_GetState(ctx: *Context, id: u32) ?*State;
extern fn ClayKit_GetOrCreateState(ctx: *Context, id: u32) ?*State;

extern fn ClayKit_SetFocus(ctx: *Context, id: ElementId) void;
extern fn ClayKit_ClearFocus(ctx: *Context) void;
extern fn ClayKit_HasFocus(ctx: *Context, id: ElementId) bool;
extern fn ClayKit_FocusChanged(ctx: *Context) bool;
extern fn ClayKit_FocusNext(ctx: *Context) void;
extern fn ClayKit_FocusPrev(ctx: *Context) void;
extern fn ClayKit_BeginFrame(ctx: *Context) void;

extern fn ClayKit_InputHandleKey(s: *InputState, key: u32, mods: u32) bool;
extern fn ClayKit_InputHandleChar(s: *InputState, codepoint: u32) bool;

extern fn ClayKit_GetSchemeColor(theme: *Theme, scheme: ColorScheme) Color;
extern fn ClayKit_GetSpacing(theme: *Theme, size: Size) u16;
extern fn ClayKit_GetFontSize(theme: *Theme, size: Size) u16;
extern fn ClayKit_GetRadius(theme: *Theme, size: Size) u16;

// Typography - note: these return Clay_TextElementConfig which is a zclay type
// We expose simpler Zig wrappers below
extern fn ClayKit_TextStyle(ctx: *Context, cfg: TextConfig) zclay.TextElementConfig;
extern fn ClayKit_HeadingStyle(ctx: *Context, cfg: HeadingConfig) zclay.TextElementConfig;
extern fn ClayKit_Badge(ctx: *Context, text: zclay.String, cfg: BadgeConfig) void;

// Button helper functions
extern fn ClayKit_ButtonBgColor(ctx: *Context, cfg: ButtonConfig, hovered: bool) Color;
extern fn ClayKit_ButtonTextColor(ctx: *Context, cfg: ButtonConfig) Color;
extern fn ClayKit_ButtonBorderColor(ctx: *Context, cfg: ButtonConfig) Color;
extern fn ClayKit_ButtonBorderWidth(cfg: ButtonConfig) u16;
extern fn ClayKit_ButtonPaddingX(ctx: *Context, size: Size) u16;
extern fn ClayKit_ButtonPaddingY(ctx: *Context, size: Size) u16;
extern fn ClayKit_ButtonRadius(ctx: *Context, size: Size) u16;
extern fn ClayKit_ButtonFontSize(ctx: *Context, size: Size) u16;

// Input helper functions
extern fn ClayKit_InputPaddingX(ctx: *Context, size: Size) u16;
extern fn ClayKit_InputPaddingY(ctx: *Context, size: Size) u16;
extern fn ClayKit_InputFontSize(ctx: *Context, size: Size) u16;
extern fn ClayKit_InputBorderColor(ctx: *Context, cfg: InputConfig, focused: bool) Color;

// Theme presets (extern const)
extern const CLAYKIT_THEME_LIGHT: Theme;
extern const CLAYKIT_THEME_DARK: Theme;

// ============================================================================
// Public API (wrapping C functions with nicer Zig interface)
// ============================================================================

/// Initialize a ClayKit context with a theme and state buffer
pub fn init(ctx: *Context, theme: *Theme, state_buf: []State) void {
    ClayKit_Init(ctx, theme, state_buf.ptr, @intCast(state_buf.len));
}

/// Get state for a component by ID, returns null if not found
pub fn getState(ctx: *Context, id: u32) ?*State {
    return ClayKit_GetState(ctx, id);
}

/// Get or create state for a component by ID
pub fn getOrCreateState(ctx: *Context, id: u32) ?*State {
    return ClayKit_GetOrCreateState(ctx, id);
}

/// Set focus to an element
pub fn setFocus(ctx: *Context, id: ElementId) void {
    ClayKit_SetFocus(ctx, id);
}

/// Clear focus
pub fn clearFocus(ctx: *Context) void {
    ClayKit_ClearFocus(ctx);
}

/// Check if an element has focus
pub fn hasFocus(ctx: *Context, id: ElementId) bool {
    return ClayKit_HasFocus(ctx, id);
}

/// Check if focus changed this frame
pub fn focusChanged(ctx: *Context) bool {
    return ClayKit_FocusChanged(ctx);
}

/// Move focus to next focusable element
pub fn focusNext(ctx: *Context) void {
    ClayKit_FocusNext(ctx);
}

/// Move focus to previous focusable element
pub fn focusPrev(ctx: *Context) void {
    ClayKit_FocusPrev(ctx);
}

/// Call at the beginning of each frame
pub fn beginFrame(ctx: *Context) void {
    ClayKit_BeginFrame(ctx);
}

/// Handle keyboard input for text input
pub fn inputHandleKey(s: *InputState, key: Key, mods: u32) bool {
    return ClayKit_InputHandleKey(s, @intFromEnum(key), mods);
}

/// Handle character input for text input
pub fn inputHandleChar(s: *InputState, codepoint: u32) bool {
    return ClayKit_InputHandleChar(s, codepoint);
}

/// Get color from color scheme
pub fn getSchemeColor(theme: *Theme, scheme: ColorScheme) Color {
    return ClayKit_GetSchemeColor(theme, scheme);
}

/// Get spacing value from size enum
pub fn getSpacing(theme: *Theme, size: Size) u16 {
    return ClayKit_GetSpacing(theme, size);
}

/// Get font size from size enum
pub fn getFontSize(theme: *Theme, size: Size) u16 {
    return ClayKit_GetFontSize(theme, size);
}

/// Get border radius from size enum
pub fn getRadius(theme: *Theme, size: Size) u16 {
    return ClayKit_GetRadius(theme, size);
}

/// Get a text style config for use with zclay.text()
pub fn textStyle(ctx: *Context, cfg: TextConfig) zclay.TextElementConfig {
    return ClayKit_TextStyle(ctx, cfg);
}

/// Get a heading style config for use with zclay.text()
pub fn headingStyle(ctx: *Context, cfg: HeadingConfig) zclay.TextElementConfig {
    return ClayKit_HeadingStyle(ctx, cfg);
}

/// Render a badge element (must be called within a zclay layout context)
pub fn badge(ctx: *Context, text: []const u8, cfg: BadgeConfig) void {
    ClayKit_Badge(ctx, zclay.String.fromRuntimeSlice(text), cfg);
}

/// Get button background color (use with Clay_Hovered() for hover state)
pub fn buttonBgColor(ctx: *Context, cfg: ButtonConfig, hovered: bool) Color {
    return ClayKit_ButtonBgColor(ctx, cfg, hovered);
}

/// Get button text color
pub fn buttonTextColor(ctx: *Context, cfg: ButtonConfig) Color {
    return ClayKit_ButtonTextColor(ctx, cfg);
}

/// Get button border color
pub fn buttonBorderColor(ctx: *Context, cfg: ButtonConfig) Color {
    return ClayKit_ButtonBorderColor(ctx, cfg);
}

/// Get button border width
pub fn buttonBorderWidth(cfg: ButtonConfig) u16 {
    return ClayKit_ButtonBorderWidth(cfg);
}

/// Get button horizontal padding
pub fn buttonPaddingX(ctx: *Context, size: Size) u16 {
    return ClayKit_ButtonPaddingX(ctx, size);
}

/// Get button vertical padding
pub fn buttonPaddingY(ctx: *Context, size: Size) u16 {
    return ClayKit_ButtonPaddingY(ctx, size);
}

/// Get button corner radius
pub fn buttonRadius(ctx: *Context, size: Size) u16 {
    return ClayKit_ButtonRadius(ctx, size);
}

/// Get button font size
pub fn buttonFontSize(ctx: *Context, size: Size) u16 {
    return ClayKit_ButtonFontSize(ctx, size);
}

/// Render a button using zclay - returns whether the button was hovered
/// Example:
///   const hovered = claykit.button(&ctx, "myBtn", "Click Me", .{ .color_scheme = .primary });
pub fn button(ctx: *Context, id: []const u8, text: []const u8, cfg: ButtonConfig) bool {
    const pad_x = ClayKit_ButtonPaddingX(ctx, cfg.size);
    const pad_y = ClayKit_ButtonPaddingY(ctx, cfg.size);
    const radius = ClayKit_ButtonRadius(ctx, cfg.size);
    const font_size = ClayKit_ButtonFontSize(ctx, cfg.size);
    const border_width = ClayKit_ButtonBorderWidth(cfg);

    var hovered: bool = false;

    zclay.UI()(.{
        .id = zclay.ElementId.ID(id),
        .layout = .{
            .sizing = .{ .w = .fit, .h = .fit },
            .padding = .{ .left = pad_x, .right = pad_x, .top = pad_y, .bottom = pad_y },
            .child_gap = 8,
            .child_alignment = .{ .x = .center, .y = .center },
            .direction = .left_to_right,
        },
        .background_color = blk: {
            hovered = zclay.hovered();
            const bg = ClayKit_ButtonBgColor(ctx, cfg, hovered);
            break :blk .{ bg.r, bg.g, bg.b, bg.a };
        },
        .corner_radius = zclay.CornerRadius.all(@floatFromInt(radius)),
        .border = .{
            .color = blk: {
                const c = ClayKit_ButtonBorderColor(ctx, cfg);
                break :blk .{ c.r, c.g, c.b, c.a };
            },
            .width = .{ .left = border_width, .right = border_width, .top = border_width, .bottom = border_width },
        },
    })({
        const text_color = ClayKit_ButtonTextColor(ctx, cfg);
        zclay.text(text, .{
            .font_size = font_size,
            .font_id = ctx.theme().font_id.body,
            .color = .{ text_color.r, text_color.g, text_color.b, text_color.a },
        });
    });

    return hovered;
}

/// Get input horizontal padding
pub fn inputPaddingX(ctx: *Context, size: Size) u16 {
    return ClayKit_InputPaddingX(ctx, size);
}

/// Get input vertical padding
pub fn inputPaddingY(ctx: *Context, size: Size) u16 {
    return ClayKit_InputPaddingY(ctx, size);
}

/// Get input font size
pub fn inputFontSize(ctx: *Context, size: Size) u16 {
    return ClayKit_InputFontSize(ctx, size);
}

/// Get input border color based on focus state
pub fn inputBorderColor(ctx: *Context, cfg: InputConfig, focused: bool) Color {
    return ClayKit_InputBorderColor(ctx, cfg, focused);
}

/// Render an input field
/// Returns whether the input was clicked (useful for setting focus)
pub fn input(ctx: *Context, id: []const u8, text: []const u8, cfg: InputConfig, focused: bool) bool {
    const pad_x = ClayKit_InputPaddingX(ctx, cfg.size);
    const pad_y = ClayKit_InputPaddingY(ctx, cfg.size);
    const font_size = ClayKit_InputFontSize(ctx, cfg.size);
    const border_color = ClayKit_InputBorderColor(ctx, cfg, focused);
    const theme = ctx.theme();

    var clicked: bool = false;

    zclay.UI()(.{
        .id = zclay.ElementId.ID(id),
        .layout = .{
            .sizing = .{
                .w = if (cfg.width > 0) zclay.SizingAxis.fixed(@floatFromInt(cfg.width)) else .grow,
                .h = .fit,
            },
            .padding = .{ .left = pad_x, .right = pad_x, .top = pad_y, .bottom = pad_y },
        },
        .background_color = if (cfg.bg.a != 0) .{ cfg.bg.r, cfg.bg.g, cfg.bg.b, cfg.bg.a } else .{ theme.bg.r, theme.bg.g, theme.bg.b, theme.bg.a },
        .corner_radius = zclay.CornerRadius.all(@floatFromInt(theme.radius.sm)),
        .border = .{
            .color = .{ border_color.r, border_color.g, border_color.b, border_color.a },
            .width = .{ .left = 1, .right = 1, .top = 1, .bottom = 1 },
        },
    })({
        clicked = zclay.hovered();
        const text_color = if (cfg.text_color.a != 0) cfg.text_color else theme.fg;

        if (text.len > 0) {
            zclay.text(text, .{
                .font_size = font_size,
                .font_id = theme.font_id.body,
                .color = .{ text_color.r, text_color.g, text_color.b, text_color.a },
            });
        }
    });

    return clicked;
}

// ============================================================================
// Theme Presets
// ============================================================================

pub const theme_light = &CLAYKIT_THEME_LIGHT;
pub const theme_dark = &CLAYKIT_THEME_DARK;

// ============================================================================
// Utility Functions
// ============================================================================

/// Convert ClayKit Color to zclay Color
pub fn toZclayColor(c: Color) zclay.Color {
    return .{ .r = c.r, .g = c.g, .b = c.b, .a = c.a };
}

/// Convert zclay Color to ClayKit Color
pub fn fromZclayColor(c: zclay.Color) Color {
    return .{ .r = c.r, .g = c.g, .b = c.b, .a = c.a };
}
