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
// Text Measurement
// ============================================================================

pub const TextDimensions = extern struct {
    width: f32 = 0,
    height: f32 = 0,
};

pub const MeasureTextCallback = ?*const fn (
    text: [*c]const u8,
    length: u32,
    font_id: u16,
    font_size: u16,
    user_data: ?*anyopaque,
) callconv(.c) TextDimensions;

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
    measure_text: MeasureTextCallback = null,
    measure_text_user_data: ?*anyopaque = null,
    cursor_blink_time: f32 = 0,

    pub fn theme(self: *Context) *Theme {
        return self.theme_ptr.?;
    }

    pub fn setMeasureText(self: *Context, callback: MeasureTextCallback, user_data: ?*anyopaque) void {
        self.measure_text = callback;
        self.measure_text_user_data = user_data;
    }

    /// Measure text width using the configured callback
    pub fn measureTextWidth(self: *Context, text: []const u8, font_id: u16, font_size: u16) f32 {
        if (self.measure_text) |callback| {
            const dims = callback(text.ptr, @intCast(text.len), font_id, font_size, self.measure_text_user_data);
            return dims.width;
        }
        return 0;
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

    /// Initialize input state with a buffer
    pub fn init(buf: []u8) InputState {
        return .{
            .buf = buf.ptr,
            .cap = @intCast(buf.len),
            .len = 0,
            .cursor = 0,
            .select_start = 0,
            .flags = 0,
        };
    }

    /// Get current text as a slice
    pub fn text(self: *const InputState) []const u8 {
        if (self.buf == null or self.len == 0) return "";
        return self.buf[0..self.len];
    }

    /// Check if there is a selection
    pub fn hasSelection(self: *const InputState) bool {
        return self.cursor != self.select_start;
    }

    /// Get selection range (start, end)
    pub fn selectionRange(self: *const InputState) struct { start: u32, end: u32 } {
        return if (self.cursor < self.select_start)
            .{ .start = self.cursor, .end = self.select_start }
        else
            .{ .start = self.select_start, .end = self.cursor };
    }

    /// Check if focused
    pub fn isFocused(self: *const InputState) bool {
        return (self.flags & @intFromEnum(InputFlags.focused)) != 0;
    }

    /// Set focused state
    pub fn setFocused(self: *InputState, focused: bool) void {
        if (focused) {
            self.flags |= @intFromEnum(InputFlags.focused);
        } else {
            self.flags &= ~@intFromEnum(InputFlags.focused);
        }
    }
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

/// Badge computed style - for renderers that need raw values
pub const BadgeStyle = extern struct {
    bg_color: Color,
    text_color: Color,
    border_color: Color,
    border_width: u16,
    pad_x: u16,
    pad_y: u16,
    font_size: u16,
    font_id: u16,
    corner_radius: u16,
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

// ============================================================================
// Checkbox Configuration
// ============================================================================

pub const CheckboxConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    size: Size = .md,
    disabled: bool = false,
};

// ============================================================================
// Switch Configuration
// ============================================================================

pub const SwitchConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    size: Size = .md,
    disabled: bool = false,
};

// ============================================================================
// Progress Configuration
// ============================================================================

pub const ProgressConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    size: Size = .md,
    striped: bool = false,
};

/// Progress computed style
pub const ProgressStyle = extern struct {
    track_color: Color,
    fill_color: Color,
    height: u16,
    corner_radius: u16,
};

// ============================================================================
// Slider Configuration
// ============================================================================

pub const SliderConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    size: Size = .md,
    min: f32 = 0,
    max: f32 = 1,
    disabled: bool = false,
};

/// Slider computed style
pub const SliderStyle = extern struct {
    track_color: Color,
    fill_color: Color,
    thumb_color: Color,
    track_height: u16,
    thumb_size: u16,
    corner_radius: u16,
};

// ============================================================================
// Alert Configuration
// ============================================================================

pub const AlertVariant = enum(c_int) {
    subtle = 0, // Light background, colored border
    solid = 1, // Solid colored background
    outline = 2, // Transparent with colored border
};

pub const AlertConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    variant: AlertVariant = .subtle,
    icon: Icon = .{},
};

/// Alert computed style
pub const AlertStyle = extern struct {
    bg_color: Color,
    border_color: Color,
    text_color: Color,
    icon_color: Color,
    border_width: u16,
    padding: u16,
    corner_radius: u16,
    icon_size: u16,
};

// ============================================================================
// Tooltip Configuration
// ============================================================================

pub const TooltipPosition = enum(c_int) {
    top = 0,
    bottom = 1,
    left = 2,
    right = 3,
};

pub const TooltipConfig = extern struct {
    position: TooltipPosition = .top,
};

/// Tooltip computed style
pub const TooltipStyle = extern struct {
    bg_color: Color,
    text_color: Color,
    padding_x: u16,
    padding_y: u16,
    corner_radius: u16,
    font_size: u16,
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
    cursor_color: Color = .{}, // Cursor color (default: theme fg)
    selection_color: Color = .{}, // Selection bg (default: primary with alpha)
    width: u16 = 0, // Fixed width (0 = grow)
};

/// Input computed style
pub const InputStyle = extern struct {
    bg_color: Color,
    border_color: Color,
    text_color: Color,
    placeholder_color: Color,
    cursor_color: Color,
    selection_color: Color,
    padding_x: u16,
    padding_y: u16,
    font_size: u16,
    font_id: u16,
    corner_radius: u16,
    cursor_width: u16,
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
// Badge - compute style for Zig to render with zclay
extern fn ClayKit_ComputeBadgeStyle(ctx: *Context, cfg: BadgeConfig) BadgeStyle;

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
extern fn ClayKit_ComputeInputStyle(ctx: *Context, cfg: InputConfig, focused: bool) InputStyle;
extern fn ClayKit_MeasureTextWidth(ctx: *Context, text: [*c]const u8, length: u32, font_id: u16, font_size: u16) f32;

// Checkbox helper functions
extern fn ClayKit_CheckboxSize(ctx: *Context, size: Size) u16;
extern fn ClayKit_CheckboxBgColor(ctx: *Context, cfg: CheckboxConfig, checked: bool, hovered: bool) Color;
extern fn ClayKit_CheckboxBorderColor(ctx: *Context, cfg: CheckboxConfig, checked: bool) Color;

// Switch helper functions
extern fn ClayKit_SwitchWidth(ctx: *Context, size: Size) u16;
extern fn ClayKit_SwitchHeight(ctx: *Context, size: Size) u16;
extern fn ClayKit_SwitchKnobSize(ctx: *Context, size: Size) u16;
extern fn ClayKit_SwitchBgColor(ctx: *Context, cfg: SwitchConfig, on: bool, hovered: bool) Color;

// Progress helper functions
extern fn ClayKit_ComputeProgressStyle(ctx: *Context, cfg: ProgressConfig) ProgressStyle;

// Slider helper functions
extern fn ClayKit_ComputeSliderStyle(ctx: *Context, cfg: SliderConfig, hovered: bool) SliderStyle;

// Alert helper functions
extern fn ClayKit_ComputeAlertStyle(ctx: *Context, cfg: AlertConfig) AlertStyle;

// Tooltip helper functions
extern fn ClayKit_ComputeTooltipStyle(ctx: *Context, cfg: TooltipConfig) TooltipStyle;

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
    return ClayKit_InputHandleKey(s, @intCast(@intFromEnum(key)), mods);
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

/// Compute badge style (for custom rendering)
pub fn computeBadgeStyle(ctx: *Context, cfg: BadgeConfig) BadgeStyle {
    return ClayKit_ComputeBadgeStyle(ctx, cfg);
}

/// Render a badge element using zclay (must be called within a zclay layout context)
pub fn badge(ctx: *Context, text: []const u8, cfg: BadgeConfig) void {
    const style = ClayKit_ComputeBadgeStyle(ctx, cfg);

    zclay.UI()(.{
        .layout = .{
            .sizing = .{ .w = .fit, .h = .fit },
            .padding = .{ .left = style.pad_x, .right = style.pad_x, .top = style.pad_y, .bottom = style.pad_y },
        },
        .background_color = .{ style.bg_color.r, style.bg_color.g, style.bg_color.b, style.bg_color.a },
        .corner_radius = zclay.CornerRadius.all(@floatFromInt(style.corner_radius)),
        .border = .{
            .color = .{ style.border_color.r, style.border_color.g, style.border_color.b, style.border_color.a },
            .width = .{ .left = style.border_width, .right = style.border_width, .top = style.border_width, .bottom = style.border_width },
        },
    })({
        zclay.text(text, .{
            .font_size = style.font_size,
            .font_id = style.font_id,
            .color = .{ style.text_color.r, style.text_color.g, style.text_color.b, style.text_color.a },
        });
    });
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

/// Compute input style (for custom rendering)
pub fn computeInputStyle(ctx: *Context, cfg: InputConfig, focused: bool) InputStyle {
    return ClayKit_ComputeInputStyle(ctx, cfg, focused);
}

/// Render a full text input with cursor and selection
/// Returns whether the input was clicked (useful for setting focus)
pub fn textInput(ctx: *Context, id: []const u8, state: *InputState, cfg: InputConfig, placeholder: []const u8) bool {
    const focused = state.isFocused();
    const style = ClayKit_ComputeInputStyle(ctx, cfg, focused);

    // Determine if cursor should be visible (blink every 0.5s)
    const show_cursor = focused and (@mod(ctx.cursor_blink_time, 1.0) < 0.5);

    // Get text to display
    const display_text = state.text();
    const has_text = display_text.len > 0;

    var clicked: bool = false;

    // Outer container
    zclay.UI()(.{
        .id = zclay.ElementId.ID(id),
        .layout = .{
            .sizing = .{
                .w = if (cfg.width > 0) zclay.SizingAxis.fixed(@floatFromInt(cfg.width)) else .grow,
                .h = .fit,
            },
            .padding = .{
                .left = style.padding_x,
                .right = style.padding_x,
                .top = style.padding_y,
                .bottom = style.padding_y,
            },
        },
        .background_color = .{ style.bg_color.r, style.bg_color.g, style.bg_color.b, style.bg_color.a },
        .corner_radius = zclay.CornerRadius.all(@floatFromInt(style.corner_radius)),
        .border = .{
            .color = .{ style.border_color.r, style.border_color.g, style.border_color.b, style.border_color.a },
            .width = .{ .left = 1, .right = 1, .top = 1, .bottom = 1 },
        },
    })({
        clicked = zclay.hovered();

        // Inner content wrapper for text + cursor positioning
        zclay.UI()(.{
            .layout = .{
                .sizing = .{ .w = .grow, .h = .fit },
            },
        })({
            // Selection highlight (rendered behind text)
            if (focused and state.hasSelection()) {
                const range = state.selectionRange();
                const text_before_sel = if (range.start > 0) display_text[0..range.start] else "";
                const sel_text = display_text[range.start..range.end];

                const sel_start_x = ctx.measureTextWidth(text_before_sel, style.font_id, style.font_size);
                const sel_width = ctx.measureTextWidth(sel_text, style.font_id, style.font_size);

                if (sel_width > 0) {
                    // Selection background
                    zclay.UI()(.{
                        .floating = .{
                            .offset = .{ .x = sel_start_x, .y = 0 },
                            .attach_points = .{ .element = .left_top, .parent = .left_top },
                        },
                        .layout = .{
                            .sizing = .{
                                .w = zclay.SizingAxis.fixed(sel_width),
                                .h = zclay.SizingAxis.fixed(@floatFromInt(style.font_size)),
                            },
                        },
                        .background_color = .{
                            style.selection_color.r,
                            style.selection_color.g,
                            style.selection_color.b,
                            style.selection_color.a,
                        },
                    })({});
                }
            }

            // Text or placeholder
            if (has_text) {
                zclay.text(display_text, .{
                    .font_size = style.font_size,
                    .font_id = style.font_id,
                    .color = .{
                        style.text_color.r,
                        style.text_color.g,
                        style.text_color.b,
                        style.text_color.a,
                    },
                });
            } else if (placeholder.len > 0) {
                zclay.text(placeholder, .{
                    .font_size = style.font_size,
                    .font_id = style.font_id,
                    .color = .{
                        style.placeholder_color.r,
                        style.placeholder_color.g,
                        style.placeholder_color.b,
                        style.placeholder_color.a,
                    },
                });
            }

            // Cursor (rendered as floating element)
            if (show_cursor) {
                const text_before_cursor = if (state.cursor > 0 and has_text)
                    display_text[0..@min(state.cursor, display_text.len)]
                else
                    "";
                const cursor_x = ctx.measureTextWidth(text_before_cursor, style.font_id, style.font_size);

                zclay.UI()(.{
                    .floating = .{
                        .offset = .{ .x = cursor_x, .y = 0 },
                        .attach_points = .{ .element = .left_top, .parent = .left_top },
                    },
                    .layout = .{
                        .sizing = .{
                            .w = zclay.SizingAxis.fixed(@floatFromInt(style.cursor_width)),
                            .h = zclay.SizingAxis.fixed(@floatFromInt(style.font_size)),
                        },
                    },
                    .background_color = .{
                        style.cursor_color.r,
                        style.cursor_color.g,
                        style.cursor_color.b,
                        style.cursor_color.a,
                    },
                })({});
            }
        });
    });

    return clicked;
}

/// Render a checkbox
/// Returns whether the checkbox was clicked (to toggle state)
pub fn checkbox(ctx: *Context, id: []const u8, checked: bool, cfg: CheckboxConfig) bool {
    const size = ClayKit_CheckboxSize(ctx, cfg.size);
    const theme = ctx.theme();
    var clicked: bool = false;

    zclay.UI()(.{
        .id = zclay.ElementId.ID(id),
        .layout = .{
            .sizing = .{
                .w = zclay.SizingAxis.fixed(@floatFromInt(size)),
                .h = zclay.SizingAxis.fixed(@floatFromInt(size)),
            },
            .child_alignment = .{ .x = .center, .y = .center },
        },
        .background_color = blk: {
            const hovered = zclay.hovered();
            clicked = hovered;
            const bg = ClayKit_CheckboxBgColor(ctx, cfg, checked, hovered);
            break :blk .{ bg.r, bg.g, bg.b, bg.a };
        },
        .corner_radius = zclay.CornerRadius.all(@floatFromInt(theme.radius.sm)),
        .border = blk: {
            const bc = ClayKit_CheckboxBorderColor(ctx, cfg, checked);
            break :blk .{
                .color = .{ bc.r, bc.g, bc.b, bc.a },
                .width = .{ .left = 2, .right = 2, .top = 2, .bottom = 2 },
            };
        },
    })({
        // Draw checkmark when checked
        if (checked) {
            // Simple inner square as checkmark indicator
            const inner_size = size - 8;
            zclay.UI()(.{
                .layout = .{
                    .sizing = .{
                        .w = zclay.SizingAxis.fixed(@floatFromInt(inner_size)),
                        .h = zclay.SizingAxis.fixed(@floatFromInt(inner_size)),
                    },
                },
                .background_color = .{ 255, 255, 255, 255 },
                .corner_radius = zclay.CornerRadius.all(2),
            })({});
        }
    });

    return clicked;
}

/// Render a switch (toggle)
/// Returns whether the switch was clicked (to toggle state)
pub fn switch_(ctx: *Context, id: []const u8, on: bool, cfg: SwitchConfig) bool {
    const width = ClayKit_SwitchWidth(ctx, cfg.size);
    const height = ClayKit_SwitchHeight(ctx, cfg.size);
    const knob_size = ClayKit_SwitchKnobSize(ctx, cfg.size);
    const padding: u16 = (height - knob_size) / 2;
    var clicked: bool = false;

    zclay.UI()(.{
        .id = zclay.ElementId.ID(id),
        .layout = .{
            .sizing = .{
                .w = zclay.SizingAxis.fixed(@floatFromInt(width)),
                .h = zclay.SizingAxis.fixed(@floatFromInt(height)),
            },
            .padding = .{
                .left = padding,
                .right = padding,
                .top = padding,
                .bottom = padding,
            },
            .child_alignment = .{
                .x = if (on) .right else .left,
                .y = .center,
            },
        },
        .background_color = blk: {
            const hovered = zclay.hovered();
            clicked = hovered;
            const bg = ClayKit_SwitchBgColor(ctx, cfg, on, hovered);
            break :blk .{ bg.r, bg.g, bg.b, bg.a };
        },
        .corner_radius = zclay.CornerRadius.all(@floatFromInt(height / 2)),
    })({
        // Knob
        zclay.UI()(.{
            .layout = .{
                .sizing = .{
                    .w = zclay.SizingAxis.fixed(@floatFromInt(knob_size)),
                    .h = zclay.SizingAxis.fixed(@floatFromInt(knob_size)),
                },
            },
            .background_color = .{ 255, 255, 255, 255 },
            .corner_radius = zclay.CornerRadius.all(@floatFromInt(knob_size / 2)),
        })({});
    });

    return clicked;
}

/// Compute progress style (for custom rendering)
pub fn computeProgressStyle(ctx: *Context, cfg: ProgressConfig) ProgressStyle {
    return ClayKit_ComputeProgressStyle(ctx, cfg);
}

/// Render a progress bar
/// value should be between 0.0 and 1.0
pub fn progress(ctx: *Context, id: []const u8, value: f32, cfg: ProgressConfig) void {
    const style = ClayKit_ComputeProgressStyle(ctx, cfg);
    const clamped_value = @max(0.0, @min(1.0, value));

    // Outer track container
    zclay.UI()(.{
        .id = zclay.ElementId.ID(id),
        .layout = .{
            .sizing = .{ .w = .grow, .h = zclay.SizingAxis.fixed(@floatFromInt(style.height)) },
        },
        .background_color = .{ style.track_color.r, style.track_color.g, style.track_color.b, style.track_color.a },
        .corner_radius = zclay.CornerRadius.all(@floatFromInt(style.corner_radius)),
    })({
        // Filled portion
        if (clamped_value > 0) {
            zclay.UI()(.{
                .layout = .{
                    .sizing = .{
                        .w = zclay.SizingAxis.percent(clamped_value),
                        .h = .grow,
                    },
                },
                .background_color = .{ style.fill_color.r, style.fill_color.g, style.fill_color.b, style.fill_color.a },
                .corner_radius = zclay.CornerRadius.all(@floatFromInt(style.corner_radius)),
            })({});
        }
    });
}

/// Compute slider style (for custom rendering)
pub fn computeSliderStyle(ctx: *Context, cfg: SliderConfig, hovered: bool) SliderStyle {
    return ClayKit_ComputeSliderStyle(ctx, cfg, hovered);
}

/// Render a slider
/// Returns whether the slider was clicked (for drag handling)
/// value should be between cfg.min and cfg.max
pub fn slider(ctx: *Context, id: []const u8, value: f32, cfg: SliderConfig) bool {
    const min_val = if (cfg.min == 0 and cfg.max == 0) 0.0 else cfg.min;
    const max_val = if (cfg.min == 0 and cfg.max == 0) 1.0 else cfg.max;
    const range = max_val - min_val;
    const normalized = if (range > 0) (value - min_val) / range else 0;
    const clamped = @max(0.0, @min(1.0, normalized));

    var clicked: bool = false;

    // Get style with hover state computed inside
    zclay.UI()(.{
        .id = zclay.ElementId.ID(id),
        .layout = .{
            .sizing = .{ .w = .grow, .h = .fit },
            .child_alignment = .{ .y = .center },
        },
    })({
        const hovered = zclay.hovered();
        clicked = hovered;
        const style = ClayKit_ComputeSliderStyle(ctx, cfg, hovered);

        // Track background
        zclay.UI()(.{
            .layout = .{
                .sizing = .{ .w = .grow, .h = zclay.SizingAxis.fixed(@floatFromInt(style.track_height)) },
            },
            .background_color = .{ style.track_color.r, style.track_color.g, style.track_color.b, style.track_color.a },
            .corner_radius = zclay.CornerRadius.all(@floatFromInt(style.corner_radius)),
        })({
            // Filled portion
            if (clamped > 0) {
                zclay.UI()(.{
                    .layout = .{
                        .sizing = .{
                            .w = zclay.SizingAxis.percent(clamped),
                            .h = .grow,
                        },
                    },
                    .background_color = .{ style.fill_color.r, style.fill_color.g, style.fill_color.b, style.fill_color.a },
                    .corner_radius = zclay.CornerRadius.all(@floatFromInt(style.corner_radius)),
                })({});
            }
        });

        // Thumb (positioned absolutely would be ideal, but we'll use a simple approach)
        // Note: For a proper slider, you'd want to position the thumb based on value
        // This is a simplified version that shows the concept
    });

    return clicked;
}

/// Compute alert style (for custom rendering)
pub fn computeAlertStyle(ctx: *Context, cfg: AlertConfig) AlertStyle {
    return ClayKit_ComputeAlertStyle(ctx, cfg);
}

/// Render an alert box
/// The content callback should render the alert content (text, etc.)
pub fn alert(ctx: *Context, id: []const u8, cfg: AlertConfig, content: fn () void) void {
    const style = ClayKit_ComputeAlertStyle(ctx, cfg);

    zclay.UI()(.{
        .id = zclay.ElementId.ID(id),
        .layout = .{
            .sizing = .{ .w = .grow, .h = .fit },
            .padding = zclay.Padding.all(style.padding),
            .child_gap = 12,
            .direction = .left_to_right,
            .child_alignment = .{ .y = .center },
        },
        .background_color = .{ style.bg_color.r, style.bg_color.g, style.bg_color.b, style.bg_color.a },
        .corner_radius = zclay.CornerRadius.all(@floatFromInt(style.corner_radius)),
        .border = .{
            .color = .{ style.border_color.r, style.border_color.g, style.border_color.b, style.border_color.a },
            .width = .{ .left = style.border_width, .right = style.border_width, .top = style.border_width, .bottom = style.border_width },
        },
    })({
        // Icon slot (if icon callback is set and icon id > 0, user renders it)
        if (cfg.icon.id > 0) {
            // Placeholder for icon - user should use icon callback
            zclay.UI()(.{
                .layout = .{
                    .sizing = .{
                        .w = zclay.SizingAxis.fixed(@floatFromInt(style.icon_size)),
                        .h = zclay.SizingAxis.fixed(@floatFromInt(style.icon_size)),
                    },
                },
            })({});
        }

        // Content area
        zclay.UI()(.{
            .layout = .{
                .sizing = .{ .w = .grow, .h = .fit },
                .direction = .top_to_bottom,
                .child_gap = 4,
            },
        })({
            content();
        });
    });
}

/// Simpler alert that just takes text
pub fn alertText(ctx: *Context, id: []const u8, text: []const u8, cfg: AlertConfig) void {
    const style = ClayKit_ComputeAlertStyle(ctx, cfg);

    zclay.UI()(.{
        .id = zclay.ElementId.ID(id),
        .layout = .{
            .sizing = .{ .w = .grow, .h = .fit },
            .padding = zclay.Padding.all(style.padding),
            .child_gap = 12,
            .direction = .left_to_right,
            .child_alignment = .{ .y = .center },
        },
        .background_color = .{ style.bg_color.r, style.bg_color.g, style.bg_color.b, style.bg_color.a },
        .corner_radius = zclay.CornerRadius.all(@floatFromInt(style.corner_radius)),
        .border = .{
            .color = .{ style.border_color.r, style.border_color.g, style.border_color.b, style.border_color.a },
            .width = .{ .left = style.border_width, .right = style.border_width, .top = style.border_width, .bottom = style.border_width },
        },
    })({
        zclay.text(text, .{
            .font_size = ctx.theme().font_size.md,
            .color = .{ style.text_color.r, style.text_color.g, style.text_color.b, style.text_color.a },
        });
    });
}

/// Compute tooltip style (for custom rendering)
pub fn computeTooltipStyle(ctx: *Context, cfg: TooltipConfig) TooltipStyle {
    return ClayKit_ComputeTooltipStyle(ctx, cfg);
}

/// Render a tooltip
/// Note: Positioning relative to anchor requires floating elements which Clay supports
/// This is a simplified version that just renders the tooltip content
pub fn tooltip(ctx: *Context, id: []const u8, text: []const u8, cfg: TooltipConfig) void {
    const style = ClayKit_ComputeTooltipStyle(ctx, cfg);
    // Note: cfg.position would be used for floating placement in a full implementation

    zclay.UI()(.{
        .id = zclay.ElementId.ID(id),
        .layout = .{
            .sizing = .{ .w = .fit, .h = .fit },
            .padding = .{ .left = style.padding_x, .right = style.padding_x, .top = style.padding_y, .bottom = style.padding_y },
        },
        .background_color = .{ style.bg_color.r, style.bg_color.g, style.bg_color.b, style.bg_color.a },
        .corner_radius = zclay.CornerRadius.all(@floatFromInt(style.corner_radius)),
    })({
        zclay.text(text, .{
            .font_size = style.font_size,
            .color = .{ style.text_color.r, style.text_color.g, style.text_color.b, style.text_color.a },
        });
    });
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
