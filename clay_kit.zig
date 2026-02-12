//! clay_kit.zig - Hand-written Zig bindings for clay_kit.h
//! Zero-allocation UI components for Clay
//!
//! This project was built with Claude (Anthropic).

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

pub const CUSTOM_ICON: u16 = 0xCE01;

pub const IconRenderData = extern struct {
    type: u16 = 0, // CUSTOM_ICON discriminator
    icon_id: u16 = 0,
    color: Color = .{},
};

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
// Tag Configuration
// ============================================================================

pub const TagVariant = enum(c_int) {
    solid = 0,
    subtle = 1,
    outline = 2,
};

pub const TagConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    variant: TagVariant = .solid,
    size: Size = .md,
    closeable: bool = false,
};

pub const TagStyle = extern struct {
    bg_color: Color,
    text_color: Color,
    border_color: Color,
    close_color: Color,
    border_width: u16,
    pad_x: u16,
    pad_y: u16,
    font_size: u16,
    font_id: u16,
    corner_radius: u16,
    gap: u16,
    close_font_size: u16,
};

// ============================================================================
// Stat Configuration
// ============================================================================

pub const StatConfig = extern struct {
    size: Size = .md,
    label_color: Color = .{ .r = 0, .g = 0, .b = 0, .a = 0 },
    value_color: Color = .{ .r = 0, .g = 0, .b = 0, .a = 0 },
    help_color: Color = .{ .r = 0, .g = 0, .b = 0, .a = 0 },
};

pub const StatStyle = extern struct {
    label_color: Color,
    value_color: Color,
    help_color: Color,
    label_font_size: u16,
    value_font_size: u16,
    help_font_size: u16,
    label_font_id: u16,
    value_font_id: u16,
    help_font_id: u16,
    gap: u16,
};

// ============================================================================
// List Configuration
// ============================================================================

pub const ListConfig = extern struct {
    ordered: bool = false,
    size: Size = .md,
    marker_color: Color = .{ .r = 0, .g = 0, .b = 0, .a = 0 },
    text_color: Color = .{ .r = 0, .g = 0, .b = 0, .a = 0 },
};

pub const ListStyle = extern struct {
    marker_color: Color,
    text_color: Color,
    font_size: u16,
    font_id: u16,
    gap: u16,
    marker_width: u16,
    item_gap: u16,
};

// ============================================================================
// Table Configuration
// ============================================================================

pub const TableConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    size: Size = .md,
    striped: bool = false,
    bordered: bool = false,
};

pub const TableStyle = extern struct {
    header_bg: Color,
    header_text: Color,
    row_bg: Color,
    row_alt_bg: Color,
    text_color: Color,
    border_color: Color,
    border_width: u16,
    cell_pad_x: u16,
    cell_pad_y: u16,
    font_size: u16,
    header_font_size: u16,
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
// Radio Configuration
// ============================================================================

pub const RadioConfig = extern struct {
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
// Tabs Configuration
// ============================================================================

pub const TabsVariant = enum(c_int) {
    line = 0, // Underline indicator
    enclosed = 1, // Enclosed/boxed tabs
    soft = 2, // Soft rounded background
};

pub const TabsConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    variant: TabsVariant = .line,
    size: Size = .md,
};

/// Tabs computed style
pub const TabsStyle = extern struct {
    active_color: Color,
    inactive_color: Color,
    active_text: Color,
    bg_color: Color,
    border_color: Color,
    padding_x: u16,
    padding_y: u16,
    font_size: u16,
    indicator_height: u16,
    corner_radius: u16,
    gap: u16,
};

// ============================================================================
// Modal Configuration
// ============================================================================

pub const ModalSize = enum(c_int) {
    sm = 0, // Small modal (400px)
    md = 1, // Medium modal (500px)
    lg = 2, // Large modal (600px)
    xl = 3, // Extra large modal (800px)
    full = 4, // Full width (with margins)
};

pub const ModalConfig = extern struct {
    size: ModalSize = .md,
    close_on_backdrop: bool = true,
    z_index: u16 = 0, // 0 = default (1000)
};

/// Modal computed style
pub const ModalStyle = extern struct {
    backdrop_color: Color,
    bg_color: Color,
    border_color: Color,
    width: u16,
    padding: u16,
    corner_radius: u16,
    z_index: u16,
};

// ============================================================================
// Spinner Configuration
// ============================================================================

pub const SpinnerConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    size: Size = .md,
    speed: f32 = 0, // 0 = default 1.0 rotations/sec
};

pub const SpinnerStyle = extern struct {
    color: Color,
    track_color: Color,
    diameter: u16,
    thickness: u16,
    speed: f32,
};

// ============================================================================
// Drawer Configuration
// ============================================================================

pub const DrawerSide = enum(c_int) {
    left = 0,
    right = 1,
    top = 2,
    bottom = 3,
};

pub const DrawerConfig = extern struct {
    side: DrawerSide = .left,
    size: u16 = 0, // 0 = default 300
    close_on_backdrop: bool = true,
    z_index: u16 = 0, // 0 = default 1000
};

pub const DrawerStyle = extern struct {
    backdrop_color: Color,
    bg_color: Color,
    border_color: Color,
    size: u16,
    padding: u16,
    z_index: u16,
};

// ============================================================================
// Popover Configuration
// ============================================================================

pub const PopoverPosition = enum(c_int) {
    top = 0,
    bottom = 1,
    left = 2,
    right = 3,
};

pub const PopoverConfig = extern struct {
    position: PopoverPosition = .bottom,
    z_index: u16 = 0, // 0 = default 50
};

pub const PopoverStyle = extern struct {
    bg_color: Color,
    border_color: Color,
    padding: u16,
    corner_radius: u16,
    z_index: u16,
};

// ============================================================================
// Link Configuration
// ============================================================================

pub const LinkVariant = enum(c_int) {
    underline = 0, // Always underlined
    hover_underline = 1, // Underline on hover only
    none = 2, // No underline
};

pub const LinkConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    size: Size = .md,
    variant: LinkVariant = .underline,
    disabled: bool = false,
};

pub const LinkStyle = extern struct {
    text_color: Color,
    hover_color: Color,
    disabled_color: Color,
    font_size: u16,
    font_id: u16,
    underline_height: u16,
};

// ============================================================================
// Breadcrumb Configuration
// ============================================================================

pub const BreadcrumbConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    size: Size = .md,
    separator: [*c]const u8 = null,
    separator_len: i32 = 0,
};

pub const BreadcrumbStyle = extern struct {
    link_color: Color,
    hover_color: Color,
    current_color: Color,
    separator_color: Color,
    font_size: u16,
    font_id: u16,
    gap: u16,
};

// ============================================================================
// Accordion Configuration
// ============================================================================

pub const AccordionVariant = enum(c_int) {
    bordered = 0, // Border between items
    separated = 1, // Cards with gap
};

pub const AccordionConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    size: Size = .md,
    variant: AccordionVariant = .bordered,
};

pub const AccordionStyle = extern struct {
    header_bg: Color,
    header_hover_bg: Color,
    header_text: Color,
    active_accent: Color,
    content_bg: Color,
    border_color: Color,
    padding_x: u16,
    padding_y: u16,
    content_padding: u16,
    font_size: u16,
    font_id: u16,
    border_width: u16,
    corner_radius: u16,
    gap: u16,
};

// ============================================================================
// Menu Configuration
// ============================================================================

pub const MenuConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    size: Size = .md,
    disabled: bool = false,
};

pub const MenuStyle = extern struct {
    bg_color: Color,
    border_color: Color,
    text_color: Color,
    disabled_text: Color,
    hover_bg: Color,
    separator_color: Color,
    padding_x: u16,
    padding_y: u16,
    font_size: u16,
    font_id: u16,
    corner_radius: u16,
    item_gap: u16,
    separator_height: u16,
    dropdown_padding: u16,
};

// ============================================================================
// Select Configuration
// ============================================================================

pub const SelectConfig = extern struct {
    color_scheme: ColorScheme = .primary,
    size: Size = .md,
    disabled: bool = false,
};

pub const SelectStyle = extern struct {
    bg_color: Color,
    border_color: Color,
    text_color: Color,
    placeholder_color: Color,
    dropdown_bg: Color,
    dropdown_border: Color,
    option_hover_bg: Color,
    padding_x: u16,
    padding_y: u16,
    font_size: u16,
    font_id: u16,
    corner_radius: u16,
    dropdown_gap: u16,
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
// Badge functions
extern fn ClayKit_ComputeBadgeStyle(ctx: *Context, cfg: BadgeConfig) BadgeStyle;
extern fn ClayKit_BadgeRaw(ctx: *Context, text: [*c]const u8, text_len: i32, cfg: BadgeConfig) void;

// Tag functions
extern fn ClayKit_ComputeTagStyle(ctx: *Context, cfg: TagConfig) TagStyle;
extern fn ClayKit_TagRaw(ctx: *Context, text: [*c]const u8, text_len: i32, cfg: TagConfig) void;

// Stat functions
extern fn ClayKit_ComputeStatStyle(ctx: *Context, cfg: StatConfig) StatStyle;
extern fn ClayKit_Stat(ctx: *Context, label: [*c]const u8, label_len: i32, value: [*c]const u8, value_len: i32, help_text: [*c]const u8, help_len: i32, cfg: StatConfig) void;

// List functions
extern fn ClayKit_ComputeListStyle(ctx: *Context, cfg: ListConfig) ListStyle;
extern fn ClayKit_ListBegin(ctx: *Context, cfg: ListConfig) void;
extern fn ClayKit_ListItemRaw(ctx: *Context, text: [*c]const u8, text_len: i32, index: u32, cfg: ListConfig) void;
extern fn ClayKit_ListEnd() void;

// Table functions
extern fn ClayKit_ComputeTableStyle(ctx: *Context, cfg: TableConfig) TableStyle;
extern fn ClayKit_TableBegin(ctx: *Context, cfg: TableConfig) void;
extern fn ClayKit_TableHeaderRow(ctx: *Context, cfg: TableConfig) void;
extern fn ClayKit_TableRow(ctx: *Context, row_index: u32, cfg: TableConfig) void;
extern fn ClayKit_TableHeaderCell(ctx: *Context, width_percent: f32, cfg: TableConfig) void;
extern fn ClayKit_TableCell(ctx: *Context, width_percent: f32, row_index: u32, cfg: TableConfig) void;
extern fn ClayKit_TableCellEnd() void;
extern fn ClayKit_TableRowEnd() void;
extern fn ClayKit_TableEnd() void;

// Button helper functions
extern fn ClayKit_ButtonBgColor(ctx: *Context, cfg: ButtonConfig, hovered: bool) Color;
extern fn ClayKit_ButtonTextColor(ctx: *Context, cfg: ButtonConfig) Color;
extern fn ClayKit_ButtonBorderColor(ctx: *Context, cfg: ButtonConfig) Color;
extern fn ClayKit_ButtonBorderWidth(cfg: ButtonConfig) u16;
extern fn ClayKit_ButtonPaddingX(ctx: *Context, size: Size) u16;
extern fn ClayKit_ButtonPaddingY(ctx: *Context, size: Size) u16;
extern fn ClayKit_ButtonRadius(ctx: *Context, size: Size) u16;
extern fn ClayKit_ButtonFontSize(ctx: *Context, size: Size) u16;
extern fn ClayKit_Button(ctx: *Context, text: [*c]const u8, text_len: i32, cfg: ButtonConfig) bool;

// Input helper functions
extern fn ClayKit_InputPaddingX(ctx: *Context, size: Size) u16;
extern fn ClayKit_InputPaddingY(ctx: *Context, size: Size) u16;
extern fn ClayKit_InputFontSize(ctx: *Context, size: Size) u16;
extern fn ClayKit_InputBorderColor(ctx: *Context, cfg: InputConfig, focused: bool) Color;
extern fn ClayKit_ComputeInputStyle(ctx: *Context, cfg: InputConfig, focused: bool) InputStyle;
extern fn ClayKit_MeasureTextWidth(ctx: *Context, text: [*c]const u8, length: u32, font_id: u16, font_size: u16) f32;
extern fn ClayKit_InputGetCursorFromX(ctx: *Context, text: [*c]const u8, length: u32, font_id: u16, font_size: u16, x_offset: f32) u32;

// Checkbox helper functions
extern fn ClayKit_CheckboxSize(ctx: *Context, size: Size) u16;
extern fn ClayKit_CheckboxBgColor(ctx: *Context, cfg: CheckboxConfig, checked: bool, hovered: bool) Color;
extern fn ClayKit_CheckboxBorderColor(ctx: *Context, cfg: CheckboxConfig, checked: bool) Color;

// Radio helper functions
extern fn ClayKit_RadioSize(ctx: *Context, size: Size) u16;
extern fn ClayKit_RadioBgColor(ctx: *Context, cfg: RadioConfig, selected: bool, hovered: bool) Color;
extern fn ClayKit_RadioBorderColor(ctx: *Context, cfg: RadioConfig, selected: bool) Color;

// Switch helper functions
extern fn ClayKit_SwitchWidth(ctx: *Context, size: Size) u16;
extern fn ClayKit_SwitchHeight(ctx: *Context, size: Size) u16;
extern fn ClayKit_SwitchKnobSize(ctx: *Context, size: Size) u16;
extern fn ClayKit_SwitchBgColor(ctx: *Context, cfg: SwitchConfig, on: bool, hovered: bool) Color;

// Progress helper functions
extern fn ClayKit_ComputeProgressStyle(ctx: *Context, cfg: ProgressConfig) ProgressStyle;
extern fn ClayKit_Progress(ctx: *Context, value: f32, cfg: ProgressConfig) void;

// Slider helper functions
extern fn ClayKit_ComputeSliderStyle(ctx: *Context, cfg: SliderConfig, hovered: bool) SliderStyle;

// Alert helper functions
extern fn ClayKit_ComputeAlertStyle(ctx: *Context, cfg: AlertConfig) AlertStyle;
extern fn ClayKit_AlertText(ctx: *Context, text: [*c]const u8, text_len: i32, cfg: AlertConfig) void;

// Tooltip helper functions
extern fn ClayKit_ComputeTooltipStyle(ctx: *Context, cfg: TooltipConfig) TooltipStyle;
extern fn ClayKit_Tooltip(ctx: *Context, text: [*c]const u8, text_len: i32, cfg: TooltipConfig) void;

// Tabs helper functions
extern fn ClayKit_ComputeTabsStyle(ctx: *Context, cfg: TabsConfig) TabsStyle;

// Modal helper functions
extern fn ClayKit_ComputeModalStyle(ctx: *Context, cfg: ModalConfig) ModalStyle;

// Spinner helper functions
extern fn ClayKit_ComputeSpinnerStyle(ctx: *Context, cfg: SpinnerConfig) SpinnerStyle;
extern fn ClayKit_SpinnerAngle(ctx: *Context, cfg: SpinnerConfig) f32;
extern fn ClayKit_Spinner(ctx: *Context, cfg: SpinnerConfig) void;

// Drawer helper functions
extern fn ClayKit_ComputeDrawerStyle(ctx: *Context, cfg: DrawerConfig) DrawerStyle;
extern fn ClayKit_DrawerBegin(ctx: *Context, id: [*c]const u8, id_len: i32, cfg: DrawerConfig) bool;
extern fn ClayKit_DrawerEnd() void;

// Popover helper functions
extern fn ClayKit_ComputePopoverStyle(ctx: *Context, cfg: PopoverConfig) PopoverStyle;
extern fn ClayKit_PopoverBegin(ctx: *Context, id: [*c]const u8, id_len: i32, cfg: PopoverConfig) void;
extern fn ClayKit_PopoverEnd() void;

// Component rendering functions
extern fn ClayKit_Checkbox(ctx: *Context, checked: bool, cfg: CheckboxConfig) bool;
extern fn ClayKit_Radio(ctx: *Context, selected: bool, cfg: RadioConfig) bool;
extern fn ClayKit_Switch(ctx: *Context, on: bool, cfg: SwitchConfig) bool;
extern fn ClayKit_Slider(ctx: *Context, value: f32, cfg: SliderConfig) bool;
extern fn ClayKit_Tab(ctx: *Context, label: [*c]const u8, label_len: i32, is_active: bool, cfg: TabsConfig) bool;
extern fn ClayKit_TextInput(ctx: *Context, id: [*c]const u8, id_len: i32, state: *InputState, cfg: InputConfig, placeholder: [*c]const u8, placeholder_len: i32) bool;

// Select helper functions
extern fn ClayKit_ComputeSelectStyle(ctx: *Context, cfg: SelectConfig) SelectStyle;
// Link
extern fn ClayKit_ComputeLinkStyle(ctx: *Context, cfg: LinkConfig) LinkStyle;
extern fn ClayKit_Link(ctx: *Context, text: [*c]const u8, text_len: i32, cfg: LinkConfig) bool;

// Breadcrumb
extern fn ClayKit_ComputeBreadcrumbStyle(ctx: *Context, cfg: BreadcrumbConfig) BreadcrumbStyle;
extern fn ClayKit_BreadcrumbBegin(ctx: *Context, cfg: BreadcrumbConfig) void;
extern fn ClayKit_BreadcrumbItem(ctx: *Context, text: [*c]const u8, text_len: i32, is_current: bool, cfg: BreadcrumbConfig) bool;
extern fn ClayKit_BreadcrumbSeparator(ctx: *Context, cfg: BreadcrumbConfig) void;
extern fn ClayKit_BreadcrumbEnd() void;

// Accordion
extern fn ClayKit_ComputeAccordionStyle(ctx: *Context, cfg: AccordionConfig) AccordionStyle;
extern fn ClayKit_AccordionBegin(ctx: *Context, cfg: AccordionConfig) void;
extern fn ClayKit_AccordionItemBegin(ctx: *Context, is_open: bool, cfg: AccordionConfig) void;
extern fn ClayKit_AccordionItemEnd() void;
extern fn ClayKit_AccordionHeader(ctx: *Context, text: [*c]const u8, text_len: i32, is_open: bool, cfg: AccordionConfig) bool;
extern fn ClayKit_AccordionContentBegin(ctx: *Context, cfg: AccordionConfig) void;
extern fn ClayKit_AccordionContentEnd() void;
extern fn ClayKit_AccordionEnd() void;

// Menu
extern fn ClayKit_ComputeMenuStyle(ctx: *Context, cfg: MenuConfig) MenuStyle;
extern fn ClayKit_MenuDropdownBegin(ctx: *Context, id: [*c]const u8, id_len: i32, cfg: MenuConfig) void;
extern fn ClayKit_MenuItem(ctx: *Context, text: [*c]const u8, text_len: i32, disabled: bool, cfg: MenuConfig) bool;
extern fn ClayKit_MenuSeparator(ctx: *Context, cfg: MenuConfig) void;
extern fn ClayKit_MenuDropdownEnd() void;

extern fn ClayKit_SelectTrigger(ctx: *Context, id: [*c]const u8, id_len: i32, display_text: [*c]const u8, display_len: i32, cfg: SelectConfig) bool;
extern fn ClayKit_SelectDropdownBegin(ctx: *Context, id: [*c]const u8, id_len: i32, cfg: SelectConfig) void;
extern fn ClayKit_SelectOption(ctx: *Context, text: [*c]const u8, text_len: i32, is_selected: bool, cfg: SelectConfig) bool;
extern fn ClayKit_SelectDropdownEnd() void;

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

/// Get cursor position from x offset within text
/// x_offset is the click position relative to the start of the text
pub fn inputGetCursorFromX(ctx: *Context, text: []const u8, font_id: u16, font_size: u16, x_offset: f32) u32 {
    return ClayKit_InputGetCursorFromX(ctx, text.ptr, @intCast(text.len), font_id, font_size, x_offset);
}

/// Handle click on text input - sets cursor position based on click x coordinate
/// bounds: the bounding box of the input element
/// click_x: the x coordinate of the click (screen space)
/// state: the input state to update
/// style: the computed input style (for padding and font info)
pub fn inputHandleClick(ctx: *Context, state: *InputState, bounds: zclay.BoundingBox, click_x: f32, style: InputStyle) void {
    // Calculate x offset relative to text start (after padding)
    const text_start_x = bounds.x + @as(f32, @floatFromInt(style.padding_x));
    const x_offset = click_x - text_start_x;

    // Get cursor position from x offset
    const text = state.text();
    const new_cursor = inputGetCursorFromX(ctx, text, style.font_id, style.font_size, x_offset);

    // Update cursor and clear selection
    state.cursor = new_cursor;
    state.select_start = new_cursor;
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

/// Render a badge element (calls C implementation)
pub fn badge(ctx: *Context, text: []const u8, cfg: BadgeConfig) void {
    ClayKit_BadgeRaw(ctx, text.ptr, @intCast(text.len), cfg);
}

/// Compute tag style (for custom rendering)
pub fn computeTagStyle(ctx: *Context, cfg: TagConfig) TagStyle {
    return ClayKit_ComputeTagStyle(ctx, cfg);
}

/// Render a tag element (calls C implementation)
pub fn tag(ctx: *Context, text: []const u8, cfg: TagConfig) void {
    ClayKit_TagRaw(ctx, text.ptr, @intCast(text.len), cfg);
}

/// Compute stat style (for custom rendering)
pub fn computeStatStyle(ctx: *Context, cfg: StatConfig) StatStyle {
    return ClayKit_ComputeStatStyle(ctx, cfg);
}

/// Render a stat element with label, value, and optional help text
pub fn stat(ctx: *Context, label: []const u8, value: []const u8, help_text: ?[]const u8, cfg: StatConfig) void {
    const help_ptr: [*c]const u8 = if (help_text) |h| h.ptr else null;
    const help_len: i32 = if (help_text) |h| @intCast(h.len) else 0;
    ClayKit_Stat(ctx, label.ptr, @intCast(label.len), value.ptr, @intCast(value.len), help_ptr, help_len, cfg);
}

/// Compute list style (for custom rendering)
pub fn computeListStyle(ctx: *Context, cfg: ListConfig) ListStyle {
    return ClayKit_ComputeListStyle(ctx, cfg);
}

/// Begin a list container
pub fn listBegin(ctx: *Context, cfg: ListConfig) void {
    ClayKit_ListBegin(ctx, cfg);
}

/// Render a list item with marker (bullet or number)
pub fn listItem(ctx: *Context, text: []const u8, index: u32, cfg: ListConfig) void {
    ClayKit_ListItemRaw(ctx, text.ptr, @intCast(text.len), index, cfg);
}

/// End a list container
pub fn listEnd() void {
    ClayKit_ListEnd();
}

/// Compute table style (for custom rendering)
pub fn computeTableStyle(ctx: *Context, cfg: TableConfig) TableStyle {
    return ClayKit_ComputeTableStyle(ctx, cfg);
}

/// Begin a table container
pub fn tableBegin(ctx: *Context, cfg: TableConfig) void {
    ClayKit_TableBegin(ctx, cfg);
}

/// Begin a table header row
pub fn tableHeaderRow(ctx: *Context, cfg: TableConfig) void {
    ClayKit_TableHeaderRow(ctx, cfg);
}

/// Begin a table data row
pub fn tableRow(ctx: *Context, row_index: u32, cfg: TableConfig) void {
    ClayKit_TableRow(ctx, row_index, cfg);
}

/// Begin a header cell with percentage width (0.0-1.0)
pub fn tableHeaderCell(ctx: *Context, width_percent: f32, cfg: TableConfig) void {
    ClayKit_TableHeaderCell(ctx, width_percent, cfg);
}

/// Begin a data cell with percentage width (0.0-1.0)
pub fn tableCell(ctx: *Context, width_percent: f32, row_index: u32, cfg: TableConfig) void {
    ClayKit_TableCell(ctx, width_percent, row_index, cfg);
}

/// End a table cell
pub fn tableCellEnd() void {
    ClayKit_TableCellEnd();
}

/// End a table row
pub fn tableRowEnd() void {
    ClayKit_TableRowEnd();
}

/// End a table
pub fn tableEnd() void {
    ClayKit_TableEnd();
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
/// Render a button (calls C implementation)
/// Example:
///   const hovered = claykit.button(&ctx, "myBtn", "Click Me", .{ .color_scheme = .primary });
pub fn button(ctx: *Context, id: []const u8, text: []const u8, cfg: ButtonConfig) bool {
    _ = id; // ID is currently unused in C implementation
    return ClayKit_Button(ctx, text.ptr, @intCast(text.len), cfg);
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

/// Render a full text input with cursor and selection (calls C implementation)
/// Returns whether the input was hovered (useful for click detection to set focus)
pub fn textInput(ctx: *Context, id: []const u8, state: *InputState, cfg: InputConfig, placeholder: []const u8) bool {
    return ClayKit_TextInput(ctx, id.ptr, @intCast(id.len), state, cfg, placeholder.ptr, @intCast(placeholder.len));
}

/// Compute select style (for custom rendering)
pub fn computeSelectStyle(ctx: *Context, cfg: SelectConfig) SelectStyle {
    return ClayKit_ComputeSelectStyle(ctx, cfg);
}

/// Render a select trigger button
/// Returns whether it was hovered (for toggling open state on click)
pub fn selectTrigger(ctx: *Context, id: []const u8, display_text: ?[]const u8, cfg: SelectConfig) bool {
    const text_ptr: [*c]const u8 = if (display_text) |t| t.ptr else null;
    const text_len: i32 = if (display_text) |t| @intCast(t.len) else 0;
    return ClayKit_SelectTrigger(ctx, id.ptr, @intCast(id.len), text_ptr, text_len, cfg);
}

/// Begin a select dropdown (floating element). Only call when open.
pub fn selectDropdownBegin(ctx: *Context, id: []const u8, cfg: SelectConfig) void {
    ClayKit_SelectDropdownBegin(ctx, id.ptr, @intCast(id.len), cfg);
}

/// Render one select option. Returns true if hovered.
pub fn selectOption(ctx: *Context, text: []const u8, is_selected: bool, cfg: SelectConfig) bool {
    return ClayKit_SelectOption(ctx, text.ptr, @intCast(text.len), is_selected, cfg);
}

/// End the select dropdown.
pub fn selectDropdownEnd() void {
    ClayKit_SelectDropdownEnd();
}

/// Render a checkbox (calls C implementation)
/// Returns whether the checkbox was hovered (to toggle state on click)
pub fn checkbox(ctx: *Context, id: []const u8, checked: bool, cfg: CheckboxConfig) bool {
    _ = id; // ID is currently unused in C implementation
    return ClayKit_Checkbox(ctx, checked, cfg);
}

/// Render a radio button (calls C implementation)
/// Returns whether the radio was hovered (to toggle state on click)
pub fn radio(ctx: *Context, id: []const u8, selected: bool, cfg: RadioConfig) bool {
    _ = id; // ID is currently unused in C implementation
    return ClayKit_Radio(ctx, selected, cfg);
}

/// Get radio button size for a given size enum
pub fn radioSize(ctx: *Context, size: Size) u16 {
    return ClayKit_RadioSize(ctx, size);
}

/// Get radio button background color
pub fn radioBgColor(ctx: *Context, cfg: RadioConfig, selected: bool, hovered: bool) Color {
    return ClayKit_RadioBgColor(ctx, cfg, selected, hovered);
}

/// Get radio button border color
pub fn radioBorderColor(ctx: *Context, cfg: RadioConfig, selected: bool) Color {
    return ClayKit_RadioBorderColor(ctx, cfg, selected);
}

/// Render a switch (toggle) (calls C implementation)
/// Returns whether the switch was hovered (to toggle state on click)
pub fn switch_(ctx: *Context, id: []const u8, on: bool, cfg: SwitchConfig) bool {
    _ = id; // ID is currently unused in C implementation
    return ClayKit_Switch(ctx, on, cfg);
}

/// Compute progress style (for custom rendering)
pub fn computeProgressStyle(ctx: *Context, cfg: ProgressConfig) ProgressStyle {
    return ClayKit_ComputeProgressStyle(ctx, cfg);
}

/// Render a progress bar (calls C implementation)
/// value should be between 0.0 and 1.0
pub fn progress(ctx: *Context, id: []const u8, value: f32, cfg: ProgressConfig) void {
    _ = id; // ID is currently unused in C implementation
    ClayKit_Progress(ctx, value, cfg);
}

/// Compute slider style (for custom rendering)
pub fn computeSliderStyle(ctx: *Context, cfg: SliderConfig, hovered: bool) SliderStyle {
    return ClayKit_ComputeSliderStyle(ctx, cfg, hovered);
}

/// Render a slider (calls C implementation)
/// Returns whether the slider was hovered (for drag handling)
/// value should be between cfg.min and cfg.max
pub fn slider(ctx: *Context, id: []const u8, value: f32, cfg: SliderConfig) bool {
    _ = id; // ID is currently unused in C implementation
    return ClayKit_Slider(ctx, value, cfg);
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

/// Simpler alert that just takes text (calls C implementation)
pub fn alertText(ctx: *Context, id: []const u8, text: []const u8, cfg: AlertConfig) void {
    _ = id; // ID is currently unused in C implementation
    ClayKit_AlertText(ctx, text.ptr, @intCast(text.len), cfg);
}

/// Compute tooltip style (for custom rendering)
pub fn computeTooltipStyle(ctx: *Context, cfg: TooltipConfig) TooltipStyle {
    return ClayKit_ComputeTooltipStyle(ctx, cfg);
}

/// Render a tooltip (calls C implementation)
/// Note: Positioning relative to anchor requires floating elements which Clay supports
/// This is a simplified version that just renders the tooltip content
pub fn tooltip(ctx: *Context, id: []const u8, text: []const u8, cfg: TooltipConfig) void {
    _ = id; // ID is currently unused in C implementation
    ClayKit_Tooltip(ctx, text.ptr, @intCast(text.len), cfg);
}

/// Compute tabs style (for custom rendering)
pub fn computeTabsStyle(ctx: *Context, cfg: TabsConfig) TabsStyle {
    return ClayKit_ComputeTabsStyle(ctx, cfg);
}

/// Render a single tab (calls C implementation)
/// Returns whether the tab was hovered
pub fn tab(ctx: *Context, label: []const u8, is_active: bool, cfg: TabsConfig) bool {
    return ClayKit_Tab(ctx, label.ptr, @intCast(label.len), is_active, cfg);
}

/// Render a tab bar and return which tab was clicked (or null if none)
/// active_index is the currently active tab (0-indexed)
/// Note: This function still uses zclay for the outer container, with C for individual tabs
pub fn tabs(ctx: *Context, id: []const u8, labels: []const []const u8, active_index: usize, cfg: TabsConfig) ?usize {
    const style = ClayKit_ComputeTabsStyle(ctx, cfg);
    var clicked_tab: ?usize = null;

    // Tab bar container (using zclay for the outer container as it requires iteration)
    zclay.UI()(.{
        .id = zclay.ElementId.ID(id),
        .layout = .{
            .sizing = .{ .w = .grow, .h = .fit },
            .direction = .left_to_right,
            .child_gap = style.gap,
        },
        .background_color = .{ style.bg_color.r, style.bg_color.g, style.bg_color.b, style.bg_color.a },
    })({
        for (labels, 0..) |label, i| {
            const is_active = i == active_index;
            const is_hovered = ClayKit_Tab(ctx, label.ptr, @intCast(label.len), is_active, cfg);
            if (is_hovered) {
                clicked_tab = i;
            }
        }
    });

    return clicked_tab;
}

// ============================================================================
// Link
// ============================================================================

/// Compute link style (for custom rendering)
pub fn computeLinkStyle(ctx: *Context, cfg: LinkConfig) LinkStyle {
    return ClayKit_ComputeLinkStyle(ctx, cfg);
}

/// Render a link. Returns true if hovered (and not disabled).
pub fn link(ctx: *Context, text: []const u8, cfg: LinkConfig) bool {
    return ClayKit_Link(ctx, text.ptr, @intCast(text.len), cfg);
}

// ============================================================================
// Breadcrumb
// ============================================================================

/// Compute breadcrumb style (for custom rendering)
pub fn computeBreadcrumbStyle(ctx: *Context, cfg: BreadcrumbConfig) BreadcrumbStyle {
    return ClayKit_ComputeBreadcrumbStyle(ctx, cfg);
}

/// Begin a breadcrumb container
pub fn breadcrumbBegin(ctx: *Context, cfg: BreadcrumbConfig) void {
    ClayKit_BreadcrumbBegin(ctx, cfg);
}

/// Render a breadcrumb item. Returns true if hovered (and not current).
pub fn breadcrumbItem(ctx: *Context, text: []const u8, is_current: bool, cfg: BreadcrumbConfig) bool {
    return ClayKit_BreadcrumbItem(ctx, text.ptr, @intCast(text.len), is_current, cfg);
}

/// Render a breadcrumb separator
pub fn breadcrumbSeparator(ctx: *Context, cfg: BreadcrumbConfig) void {
    ClayKit_BreadcrumbSeparator(ctx, cfg);
}

/// End the breadcrumb container
pub fn breadcrumbEnd() void {
    ClayKit_BreadcrumbEnd();
}

/// Convenience: render a full breadcrumb bar from a slice of labels.
/// The last item is styled as "current". Returns ?usize of hovered item index.
pub fn breadcrumb(ctx: *Context, items: []const []const u8, cfg: BreadcrumbConfig) ?usize {
    var hovered_index: ?usize = null;
    ClayKit_BreadcrumbBegin(ctx, cfg);
    for (items, 0..) |item, i| {
        if (i > 0) {
            ClayKit_BreadcrumbSeparator(ctx, cfg);
        }
        const is_current = (i == items.len - 1);
        if (ClayKit_BreadcrumbItem(ctx, item.ptr, @intCast(item.len), is_current, cfg)) {
            hovered_index = i;
        }
    }
    ClayKit_BreadcrumbEnd();
    return hovered_index;
}

// ============================================================================
// Accordion
// ============================================================================

/// Compute accordion style (for custom rendering)
pub fn computeAccordionStyle(ctx: *Context, cfg: AccordionConfig) AccordionStyle {
    return ClayKit_ComputeAccordionStyle(ctx, cfg);
}

/// Begin an accordion container
pub fn accordionBegin(ctx: *Context, cfg: AccordionConfig) void {
    ClayKit_AccordionBegin(ctx, cfg);
}

/// Begin an accordion item (wraps header + optional content).
/// Must be called before accordionHeader. Call accordionItemEnd after
/// the header (and optional content) to close the item wrapper.
pub fn accordionItemBegin(ctx: *Context, is_open: bool, cfg: AccordionConfig) void {
    ClayKit_AccordionItemBegin(ctx, is_open, cfg);
}

/// End an accordion item wrapper
pub fn accordionItemEnd() void {
    ClayKit_AccordionItemEnd();
}

/// Render an accordion header. Returns true if hovered (for toggle on click).
pub fn accordionHeader(ctx: *Context, text: []const u8, is_open: bool, cfg: AccordionConfig) bool {
    return ClayKit_AccordionHeader(ctx, text.ptr, @intCast(text.len), is_open, cfg);
}

/// Begin accordion content area (only call when section is open)
pub fn accordionContentBegin(ctx: *Context, cfg: AccordionConfig) void {
    ClayKit_AccordionContentBegin(ctx, cfg);
}

/// End accordion content area
pub fn accordionContentEnd() void {
    ClayKit_AccordionContentEnd();
}

/// End the accordion container
pub fn accordionEnd() void {
    ClayKit_AccordionEnd();
}

// ============================================================================
// Menu
// ============================================================================

/// Compute menu style (for custom rendering)
pub fn computeMenuStyle(ctx: *Context, cfg: MenuConfig) MenuStyle {
    return ClayKit_ComputeMenuStyle(ctx, cfg);
}

/// Begin a menu dropdown (floating element). Only call when open.
pub fn menuDropdownBegin(ctx: *Context, id: []const u8, cfg: MenuConfig) void {
    ClayKit_MenuDropdownBegin(ctx, id.ptr, @intCast(id.len), cfg);
}

/// Render a menu item. Returns true if hovered and not disabled.
pub fn menuItem(ctx: *Context, text: []const u8, disabled: bool, cfg: MenuConfig) bool {
    return ClayKit_MenuItem(ctx, text.ptr, @intCast(text.len), disabled, cfg);
}

/// Render a menu separator line
pub fn menuSeparator(ctx: *Context, cfg: MenuConfig) void {
    ClayKit_MenuSeparator(ctx, cfg);
}

/// End the menu dropdown
pub fn menuDropdownEnd() void {
    ClayKit_MenuDropdownEnd();
}

// ============================================================================
// Modal
// ============================================================================

/// Compute modal style (for custom rendering)
pub fn computeModalStyle(ctx: *Context, cfg: ModalConfig) ModalStyle {
    return ClayKit_ComputeModalStyle(ctx, cfg);
}

/// Render a modal with backdrop. Returns true if backdrop was clicked (for closing).
/// The `is_open` parameter controls visibility - when false, nothing is rendered.
/// The `content` callback renders the modal body content.
pub fn modal(ctx: *Context, id: []const u8, is_open: bool, cfg: ModalConfig, content: fn () void) bool {
    if (!is_open) return false;

    const style = ClayKit_ComputeModalStyle(ctx, cfg);

    // Create element IDs for backdrop and modal
    const backdrop_id = zclay.ElementId.IDI(id, 0);
    const modal_id = zclay.ElementId.ID(id);

    // Backdrop - full screen overlay attached to root
    zclay.UI()(.{
        .id = backdrop_id,
        .layout = .{
            .sizing = .{ .w = .grow, .h = .grow },
            .child_alignment = .{ .x = .center, .y = .center },
        },
        .background_color = .{
            style.backdrop_color.r,
            style.backdrop_color.g,
            style.backdrop_color.b,
            style.backdrop_color.a,
        },
        .floating = .{
            .attach_to = .to_root,
            .z_index = @intCast(style.z_index),
            .pointer_capture_mode = .capture,
        },
    })({
        // Modal container
        const modal_width: zclay.SizingAxis = if (style.width > 0)
            zclay.SizingAxis.fixed(@floatFromInt(style.width))
        else
            zclay.SizingAxis.growMinMax(.{ .min = 100, .max = 10000 });

        zclay.UI()(.{
            .id = modal_id,
            .layout = .{
                .sizing = .{ .w = modal_width, .h = .fit },
                .padding = zclay.Padding.all(style.padding),
                .child_gap = 16,
                .direction = .top_to_bottom,
            },
            .background_color = .{
                style.bg_color.r,
                style.bg_color.g,
                style.bg_color.b,
                style.bg_color.a,
            },
            .corner_radius = zclay.CornerRadius.all(@floatFromInt(style.corner_radius)),
            .border = .{
                .color = .{
                    style.border_color.r,
                    style.border_color.g,
                    style.border_color.b,
                    style.border_color.a,
                },
                .width = zclay.BorderWidth.all(1),
            },
        })({
            // Render user content
            content();
        });
    });

    // Check if backdrop was clicked (pointer is over backdrop but NOT over modal)
    // Use pointerOver which checks specific element IDs in the hover chain
    const backdrop_hovered = zclay.pointerOver(backdrop_id);
    const modal_hovered = zclay.pointerOver(modal_id);

    return cfg.close_on_backdrop and backdrop_hovered and !modal_hovered;
}

// ============================================================================
// Spinner
// ============================================================================

/// Compute spinner style (for custom rendering)
pub fn computeSpinnerStyle(ctx: *Context, cfg: SpinnerConfig) SpinnerStyle {
    return ClayKit_ComputeSpinnerStyle(ctx, cfg);
}

/// Get current spinner rotation angle in degrees (0-360)
pub fn spinnerAngle(ctx: *Context, cfg: SpinnerConfig) f32 {
    return ClayKit_SpinnerAngle(ctx, cfg);
}

/// Render a spinner (ring placeholder - renderer should draw actual spinning arc)
pub fn spinner(ctx: *Context, cfg: SpinnerConfig) void {
    ClayKit_Spinner(ctx, cfg);
}

// ============================================================================
// Drawer
// ============================================================================

/// Compute drawer style (for custom rendering)
pub fn computeDrawerStyle(ctx: *Context, cfg: DrawerConfig) DrawerStyle {
    return ClayKit_ComputeDrawerStyle(ctx, cfg);
}

/// Begin a drawer (sliding panel with backdrop). Returns true if backdrop clicked.
/// Only call when drawer is open. Call DrawerEnd after content.
pub fn drawerBegin(ctx: *Context, id: []const u8, cfg: DrawerConfig) bool {
    return ClayKit_DrawerBegin(ctx, id.ptr, @intCast(id.len), cfg);
}

/// End the drawer
pub fn drawerEnd() void {
    ClayKit_DrawerEnd();
}

// ============================================================================
// Popover
// ============================================================================

/// Compute popover style (for custom rendering)
pub fn computePopoverStyle(ctx: *Context, cfg: PopoverConfig) PopoverStyle {
    return ClayKit_ComputePopoverStyle(ctx, cfg);
}

/// Begin a popover (floating element anchored to parent)
/// Only call when popover is open. Call PopoverEnd after content.
pub fn popoverBegin(ctx: *Context, id: []const u8, cfg: PopoverConfig) void {
    ClayKit_PopoverBegin(ctx, id.ptr, @intCast(id.len), cfg);
}

/// End the popover
pub fn popoverEnd() void {
    ClayKit_PopoverEnd();
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
