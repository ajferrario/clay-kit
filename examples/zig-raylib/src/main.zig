const std = @import("std");
const zclay = @import("zclay");
const raylib = @import("raylib");
const claykit = @import("claykit");

const WINDOW_WIDTH = 1280;
const WINDOW_HEIGHT = 800;

// Raylib font for measuring text
var raylib_fonts: [1]raylib.Font = undefined;

// Buffer for null-terminating text strings
var text_buffer: [4096]u8 = undefined;

// Text input buffer
var input_buffer: [256]u8 = undefined;
var input_state: claykit.InputState = undefined;

// Pending click state (processed before next frame's layout)
var pending_input_click: bool = false;
var pending_click_x: f32 = 0;

// Tabs state
var active_tab: usize = 0;

// Modal state
var show_modal: bool = false;

// Drawer state
var show_drawer: bool = false;

// Popover state
var show_popover: bool = false;

// Radio state
var selected_radio: usize = 0;

// Select state
var selected_option: ?usize = null;
var select_open: bool = false;

// Accordion state
var accordion_open = [3]bool{ true, false, false };

// Menu state
var menu_open: bool = false;

// ClayKit text measurement callback (different signature from zclay)
fn measureTextForClayKit(
    text: [*c]const u8,
    length: u32,
    font_id: u16,
    font_size: u16,
    _: ?*anyopaque,
) callconv(.c) claykit.TextDimensions {
    if (length == 0) return .{ .width = 0, .height = @floatFromInt(font_size) };

    const font = raylib_fonts[font_id];
    const font_size_f: f32 = @floatFromInt(font_size);

    // Copy to buffer and null-terminate
    const len: usize = @intCast(length);
    const safe_len = @min(len, text_buffer.len - 1);
    @memcpy(text_buffer[0..safe_len], text[0..safe_len]);
    text_buffer[safe_len] = 0;
    const text_z: [:0]const u8 = text_buffer[0..safe_len :0];

    const size = raylib.measureTextEx(font, text_z, font_size_f, 0);
    return .{ .width = size.x, .height = size.y };
}

// Demo icon IDs
const ICON_INFO: u16 = 1;
const ICON_SUCCESS: u16 = 2;
const ICON_WARNING: u16 = 3;
const ICON_ERROR: u16 = 4;

fn iconCallback(icon_id: u16, box: claykit.BoundingBox, _: ?*anyopaque) callconv(.c) void {
    const cx: i32 = @intFromFloat(box.x + box.width / 2.0);
    const cy: i32 = @intFromFloat(box.y + box.height / 2.0);
    const r: f32 = box.width / 2.0 - 1.0;

    const color: raylib.Color = switch (icon_id) {
        ICON_INFO => .{ .r = 66, .g = 133, .b = 244, .a = 255 },
        ICON_SUCCESS => .{ .r = 34, .g = 197, .b = 94, .a = 255 },
        ICON_WARNING => .{ .r = 251, .g = 191, .b = 36, .a = 255 },
        ICON_ERROR => .{ .r = 239, .g = 68, .b = 68, .a = 255 },
        else => .{ .r = 150, .g = 150, .b = 150, .a = 255 },
    };
    const label: [:0]const u8 = switch (icon_id) {
        ICON_INFO => "i",
        ICON_SUCCESS => "v",
        ICON_WARNING => "!",
        ICON_ERROR => "x",
        else => "?",
    };
    raylib.drawCircle(cx, cy, r, color);
    raylib.drawText(label, cx - 3, cy - 5, 10, raylib.Color.white);
}

// Clay text measurement callback
fn measureText(text_slice: []const u8, config: *zclay.TextElementConfig, _: void) zclay.Dimensions {
    const font = raylib_fonts[config.font_id];
    const font_size: f32 = @floatFromInt(config.font_size);
    const letter_spacing: f32 = @floatFromInt(config.letter_spacing);

    // Clay strings aren't null-terminated, so copy to buffer and add terminator
    const len = @min(text_slice.len, text_buffer.len - 1);
    @memcpy(text_buffer[0..len], text_slice[0..len]);
    text_buffer[len] = 0;
    const text_z: [:0]const u8 = text_buffer[0..len :0];

    var text_size = raylib.measureTextEx(
        font,
        text_z,
        font_size,
        letter_spacing,
    );

    // Handle empty strings
    if (text_size.x == 0) {
        text_size.x = 1;
    }

    return .{ .w = text_size.x, .h = text_size.y };
}

pub fn main() !void {
    // Get the executable's directory for resource paths
    var exe_dir_buf: [std.fs.max_path_bytes]u8 = undefined;
    const exe_dir = std.fs.selfExeDirPath(&exe_dir_buf) catch {
        std.debug.print("Failed to get executable directory\n", .{});
        return;
    };

    // Build path to font file relative to executable
    var font_path_buf: [std.fs.max_path_bytes]u8 = undefined;
    const font_path = std.fmt.bufPrintZ(&font_path_buf, "{s}/resources/Roboto-Regular.ttf", .{exe_dir}) catch {
        std.debug.print("Failed to build font path\n", .{});
        return;
    };

    // Initialize raylib
    raylib.initWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ClayKit + Raylib Demo");
    defer raylib.closeWindow();
    raylib.setTargetFPS(60);

    // Load font using absolute path
    raylib_fonts[0] = raylib.loadFontEx(font_path, 48, null) catch
        (raylib.getFontDefault() catch unreachable);

    // Initialize Clay
    const min_memory_size: u32 = zclay.minMemorySize();
    const memory = std.heap.page_allocator.alloc(u8, min_memory_size) catch {
        std.debug.print("Failed to allocate memory for Clay\n", .{});
        return;
    };
    defer std.heap.page_allocator.free(memory);

    const arena: zclay.Arena = zclay.createArenaWithCapacityAndMemory(memory);
    _ = zclay.initialize(arena, .{
        .w = WINDOW_WIDTH,
        .h = WINDOW_HEIGHT,
    }, .{});
    zclay.setMeasureTextFunction(void, {}, measureText);

    // Initialize ClayKit using the Zig bindings
    var theme = claykit.Theme.light;
    var state_buf: [64]claykit.State = undefined;
    var ctx: claykit.Context = .{};
    claykit.init(&ctx, &theme, &state_buf);

    // Set up text measurement for ClayKit (needed for cursor positioning)
    ctx.setMeasureText(measureTextForClayKit, null);
    ctx.icon_callback = iconCallback;

    // Initialize text input state
    input_state = claykit.InputState.init(&input_buffer);

    // Main loop
    while (!raylib.windowShouldClose()) {
        // Update cursor blink timer
        ctx.cursor_blink_time += raylib.getFrameTime();

        // Handle keyboard input for text field
        if (input_state.isFocused()) {
            // Handle special keys
            if (raylib.isKeyPressed(.backspace)) {
                _ = claykit.inputHandleKey(&input_state, .backspace, getModifiers());
            }
            if (raylib.isKeyPressed(.delete)) {
                _ = claykit.inputHandleKey(&input_state, .delete, getModifiers());
            }
            if (raylib.isKeyPressed(.left)) {
                _ = claykit.inputHandleKey(&input_state, .left, getModifiers());
            }
            if (raylib.isKeyPressed(.right)) {
                _ = claykit.inputHandleKey(&input_state, .right, getModifiers());
            }
            if (raylib.isKeyPressed(.home)) {
                _ = claykit.inputHandleKey(&input_state, .home, getModifiers());
            }
            if (raylib.isKeyPressed(.end)) {
                _ = claykit.inputHandleKey(&input_state, .end, getModifiers());
            }

            // Handle character input
            var char = raylib.getCharPressed();
            while (char != 0) {
                _ = claykit.inputHandleChar(&input_state, @intCast(char));
                char = raylib.getCharPressed();
            }
        }
        // Update Clay layout dimensions if window resized
        const screen_width: f32 = @floatFromInt(raylib.getScreenWidth());
        const screen_height: f32 = @floatFromInt(raylib.getScreenHeight());
        zclay.setLayoutDimensions(.{ .w = screen_width, .h = screen_height });

        // Update pointer state
        const mouse_pos = raylib.getMousePosition();
        zclay.setPointerState(.{ .x = mouse_pos.x, .y = mouse_pos.y }, raylib.isMouseButtonDown(.left));

        // Begin frame
        claykit.beginFrame(&ctx);

        // Process pending click from last frame (before layout so cursor renders correctly)
        if (pending_input_click) {
            pending_input_click = false;
            const input_elem = zclay.getElementData(zclay.ElementId.ID("TextInput1"));
            if (input_elem.found) {
                const style = claykit.computeInputStyle(&ctx, .{}, true);
                claykit.inputHandleClick(&ctx, &input_state, input_elem.bounding_box, pending_click_x, style);
            }
        }

        // Track input click state (set during UI building, used after layout)
        var input_clicked: bool = false;

        // Build UI layout using new zclay API
        zclay.beginLayout();

        // Root container
        zclay.UI()(.{
            .id = zclay.ElementId.ID("Root"),
            .layout = .{
                .sizing = .{ .w = .grow, .h = .grow },
                .padding = zclay.Padding.all(16),
                .child_gap = 12,
                .direction = .top_to_bottom,
            },
            .background_color = toZclayColor(theme.bg),
        })({
            // Header
            zclay.UI()(.{
                .id = zclay.ElementId.ID("Header"),
                .layout = .{
                    .sizing = .{ .w = .grow },
                    .padding = zclay.Padding.all(12),
                },
                .background_color = toZclayColor(theme.primary),
                .corner_radius = zclay.CornerRadius.all(@floatFromInt(theme.radius.md)),
            })({
                zclay.text("ClayKit Demo - Zig Bindings", .{
                    .font_size = theme.font_size.xl,
                    .color = .{ 255, 255, 255, 255 },
                });
            });

            // Content area - 4 columns
            zclay.UI()(.{
                .id = zclay.ElementId.ID("Content"),
                .layout = .{
                    .sizing = .{ .w = .grow, .h = .grow },
                    .child_gap = 12,
                    .direction = .left_to_right,
                },
            })({
                // ===== Column 1: Form Controls =====
                zclay.UI()(.{
                    .id = zclay.ElementId.ID("Col1"),
                    .layout = .{
                        .sizing = .{ .w = .grow, .h = .fit },
                        .padding = zclay.Padding.all(12),
                        .child_gap = 8,
                        .direction = .top_to_bottom,
                    },
                    .background_color = toZclayColor(theme.secondary),
                    .corner_radius = zclay.CornerRadius.all(@floatFromInt(theme.radius.md)),
                })({
                    zclay.text("Form Controls", claykit.headingStyle(&ctx, .{ .size = .md }));

                    // Button
                    zclay.text("Button:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    _ = claykit.button(&ctx, "Btn1", "Button", .{ .icon_left = .{ .id = ICON_SUCCESS, .size = 16 } });

                    // Text Input
                    zclay.text("Text Input:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    input_clicked = claykit.textInput(&ctx, "TextInput1", &input_state, .{}, "Type here...");

                    // Slider
                    zclay.text("Slider:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    _ = claykit.slider(&ctx, "Slider1", 0.5, .{});

                    // Radio group
                    zclay.text("Radio:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    {
                        const radio_labels = [_][]const u8{ "Option A", "Option B", "Option C" };
                        for (radio_labels, 0..) |label, i| {
                            zclay.UI()(.{
                                .layout = .{
                                    .sizing = .{ .w = .grow },
                                    .child_gap = 8,
                                    .direction = .left_to_right,
                                    .child_alignment = .{ .y = .center },
                                },
                            })({
                                const hovered = claykit.radio(&ctx, "", selected_radio == i, .{});
                                zclay.text(label, claykit.textStyle(&ctx, .{ .size = .sm }));
                                if (hovered and raylib.isMouseButtonPressed(.left)) {
                                    selected_radio = i;
                                }
                            });
                        }
                    }

                    // Select dropdown
                    zclay.text("Select:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    {
                        const options = [_][]const u8{ "Apple", "Banana", "Cherry" };
                        const display_text: ?[]const u8 = if (selected_option) |idx| options[idx] else null;

                        const trigger_hovered = claykit.selectTrigger(&ctx, "Select1", display_text, .{});

                        if (select_open) {
                            claykit.selectDropdownBegin(&ctx, "SelectDrop1", .{});
                            for (options, 0..) |opt, i| {
                                const opt_hovered = claykit.selectOption(&ctx, opt, if (selected_option) |idx| idx == i else false, .{});
                                if (opt_hovered and raylib.isMouseButtonPressed(.left)) {
                                    selected_option = i;
                                    select_open = false;
                                }
                            }
                            claykit.selectDropdownEnd();
                        }

                        if (trigger_hovered and raylib.isMouseButtonPressed(.left)) {
                            select_open = !select_open;
                        } else if (!trigger_hovered and raylib.isMouseButtonPressed(.left) and select_open) {
                            select_open = false;
                        }
                    }
                });

                // ===== Column 2: Data Display =====
                zclay.UI()(.{
                    .id = zclay.ElementId.ID("Col2"),
                    .layout = .{
                        .sizing = .{ .w = .grow, .h = .fit },
                        .padding = zclay.Padding.all(12),
                        .child_gap = 8,
                        .direction = .top_to_bottom,
                    },
                    .background_color = toZclayColor(theme.secondary),
                    .corner_radius = zclay.CornerRadius.all(@floatFromInt(theme.radius.md)),
                })({
                    zclay.text("Data Display", claykit.headingStyle(&ctx, .{ .size = .md }));

                    // Badge
                    zclay.text("Badge:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    claykit.badge(&ctx, "Badge", .{});

                    // Tags
                    zclay.text("Tags:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    zclay.UI()(.{
                        .id = zclay.ElementId.ID("TagRow"),
                        .layout = .{
                            .sizing = .{ .w = .grow },
                            .child_gap = 6,
                            .direction = .left_to_right,
                        },
                    })({
                        claykit.tag(&ctx, "Default", .{});
                        claykit.tag(&ctx, "Subtle", .{ .variant = .subtle, .color_scheme = .success });
                        claykit.tag(&ctx, "Close", .{ .closeable = true, .color_scheme = .@"error" });
                    });

                    // Progress bar
                    zclay.text("Progress:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    claykit.progress(&ctx, "Progress1", 0.7, .{});

                    // Spinner
                    zclay.text("Spinner:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    zclay.UI()(.{
                        .layout = .{
                            .sizing = .{ .w = .grow },
                            .child_gap = 12,
                            .direction = .left_to_right,
                            .child_alignment = .{ .y = .center },
                        },
                    })({
                        claykit.spinner(&ctx, .{});
                        claykit.spinner(&ctx, .{ .size = .lg, .color_scheme = .success });
                        claykit.spinner(&ctx, .{ .size = .xs, .color_scheme = .@"error" });
                    });

                    // Alerts
                    zclay.text("Alerts:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    claykit.alertText(&ctx, "Alert1", "Info alert", .{ .icon = .{ .id = ICON_INFO } });
                    claykit.alertText(&ctx, "Alert2", "Success!", .{ .color_scheme = .success, .icon = .{ .id = ICON_SUCCESS } });

                    // Tooltip
                    zclay.text("Tooltip:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    claykit.tooltip(&ctx, "Tooltip1", "This is a tooltip", .{});

                    // Stats
                    zclay.text("Stats:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    claykit.stat(&ctx, "Revenue", "$45,231", "+20.1%", .{ .size = .sm });
                    claykit.stat(&ctx, "Users", "2,350", "+180", .{ .size = .sm });
                });

                // ===== Column 3: Lists & Table =====
                zclay.UI()(.{
                    .id = zclay.ElementId.ID("Col3"),
                    .layout = .{
                        .sizing = .{ .w = .grow, .h = .fit },
                        .padding = zclay.Padding.all(12),
                        .child_gap = 8,
                        .direction = .top_to_bottom,
                    },
                    .background_color = .{ 240, 240, 245, 255 },
                    .corner_radius = zclay.CornerRadius.all(@floatFromInt(theme.radius.md)),
                })({
                    zclay.text("Lists & Table", claykit.headingStyle(&ctx, .{ .size = .md }));

                    // Unordered list
                    zclay.text("Unordered:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    {
                        const ul_cfg = claykit.ListConfig{};
                        claykit.listBegin(&ctx, ul_cfg);
                        claykit.listItem(&ctx, "First item", 0, ul_cfg);
                        claykit.listItem(&ctx, "Second item", 1, ul_cfg);
                        claykit.listItem(&ctx, "Third item", 2, ul_cfg);
                        claykit.listEnd();
                    }

                    // Ordered list
                    zclay.text("Ordered:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    {
                        const ol_cfg = claykit.ListConfig{ .ordered = true };
                        claykit.listBegin(&ctx, ol_cfg);
                        claykit.listItem(&ctx, "Step one", 0, ol_cfg);
                        claykit.listItem(&ctx, "Step two", 1, ol_cfg);
                        claykit.listItem(&ctx, "Step three", 2, ol_cfg);
                        claykit.listEnd();
                    }

                    // Table
                    zclay.text("Table:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    {
                        const t_cfg = claykit.TableConfig{ .striped = true, .bordered = true };
                        const text_style = claykit.textStyle(&ctx, .{ .size = .sm });
                        const header_color: zclay.Color = .{ 255, 255, 255, 255 };

                        claykit.tableBegin(&ctx, t_cfg);

                        claykit.tableHeaderRow(&ctx, t_cfg);
                        claykit.tableHeaderCell(&ctx, 0.33, t_cfg);
                        zclay.text("Name", .{ .font_size = text_style.font_size, .color = header_color });
                        claykit.tableCellEnd();
                        claykit.tableHeaderCell(&ctx, 0.33, t_cfg);
                        zclay.text("Role", .{ .font_size = text_style.font_size, .color = header_color });
                        claykit.tableCellEnd();
                        claykit.tableHeaderCell(&ctx, 0.34, t_cfg);
                        zclay.text("Status", .{ .font_size = text_style.font_size, .color = header_color });
                        claykit.tableCellEnd();
                        claykit.tableRowEnd();

                        claykit.tableRow(&ctx, 0, t_cfg);
                        claykit.tableCell(&ctx, 0.33, 0, t_cfg);
                        zclay.text("Alice", text_style);
                        claykit.tableCellEnd();
                        claykit.tableCell(&ctx, 0.33, 0, t_cfg);
                        zclay.text("Engineer", text_style);
                        claykit.tableCellEnd();
                        claykit.tableCell(&ctx, 0.34, 0, t_cfg);
                        zclay.text("Active", text_style);
                        claykit.tableCellEnd();
                        claykit.tableRowEnd();

                        claykit.tableRow(&ctx, 1, t_cfg);
                        claykit.tableCell(&ctx, 0.33, 1, t_cfg);
                        zclay.text("Bob", text_style);
                        claykit.tableCellEnd();
                        claykit.tableCell(&ctx, 0.33, 1, t_cfg);
                        zclay.text("Designer", text_style);
                        claykit.tableCellEnd();
                        claykit.tableCell(&ctx, 0.34, 1, t_cfg);
                        zclay.text("Away", text_style);
                        claykit.tableCellEnd();
                        claykit.tableRowEnd();

                        claykit.tableEnd();
                    }
                });

                // ===== Column 4: Navigation & Overlays =====
                zclay.UI()(.{
                    .id = zclay.ElementId.ID("Col4"),
                    .layout = .{
                        .sizing = .{ .w = .grow, .h = .fit },
                        .padding = zclay.Padding.all(12),
                        .child_gap = 8,
                        .direction = .top_to_bottom,
                    },
                    .background_color = toZclayColor(theme.secondary),
                    .corner_radius = zclay.CornerRadius.all(@floatFromInt(theme.radius.md)),
                })({
                    zclay.text("Navigation", claykit.headingStyle(&ctx, .{ .size = .md }));

                    // Tabs (line variant)
                    zclay.text("Tabs:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    const tab_labels = [_][]const u8{ "Tab 1", "Tab 2", "Tab 3" };
                    if (claykit.tabs(&ctx, "Tabs1", &tab_labels, active_tab, .{})) |hovered| {
                        if (raylib.isMouseButtonPressed(.left)) {
                            active_tab = hovered;
                        }
                    }

                    // Tabs with enclosed variant
                    if (claykit.tabs(&ctx, "Tabs2", &tab_labels, active_tab, .{ .variant = .enclosed })) |hovered| {
                        if (raylib.isMouseButtonPressed(.left)) {
                            active_tab = hovered;
                        }
                    }

                    // Links
                    zclay.text("Links:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    zclay.UI()(.{
                        .layout = .{
                            .sizing = .{ .w = .grow },
                            .child_gap = 8,
                            .direction = .left_to_right,
                            .child_alignment = .{ .y = .center },
                        },
                    })({
                        _ = claykit.link(&ctx, "Default", .{});
                        _ = claykit.link(&ctx, "Hover", .{ .variant = .hover_underline });
                        _ = claykit.link(&ctx, "Disabled", .{ .disabled = true });
                    });

                    // Breadcrumb
                    zclay.text("Breadcrumb:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    {
                        const bc_items = [_][]const u8{ "Home", "Products", "Widget" };
                        _ = claykit.breadcrumb(&ctx, &bc_items, .{});
                    }

                    // Accordion
                    zclay.text("Accordion:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    {
                        const acc_cfg = claykit.AccordionConfig{};
                        claykit.accordionBegin(&ctx, acc_cfg);

                        const headers = [_][]const u8{ "Section 1", "Section 2", "Section 3" };
                        const contents = [_][]const u8{
                            "Content for section 1. This is expanded by default.",
                            "Content for section 2.",
                            "Content for section 3.",
                        };

                        for (headers, contents, 0..) |header, content, i| {
                            claykit.accordionItemBegin(&ctx, accordion_open[i], acc_cfg);
                            const hovered = claykit.accordionHeader(&ctx, header, accordion_open[i], acc_cfg);
                            if (accordion_open[i]) {
                                claykit.accordionContentBegin(&ctx, acc_cfg);
                                zclay.text(content, claykit.textStyle(&ctx, .{ .size = .sm }));
                                claykit.accordionContentEnd();
                            }
                            claykit.accordionItemEnd();
                            if (hovered and raylib.isMouseButtonPressed(.left)) {
                                accordion_open[i] = !accordion_open[i];
                            }
                        }

                        claykit.accordionEnd();
                    }

                    // Menu
                    zclay.text("Menu:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    zclay.UI()(.{
                        .id = zclay.ElementId.ID("MenuWrap"),
                        .layout = .{ .sizing = .{ .w = .fit, .h = .fit } },
                    })({
                        const menu_btn_hovered = claykit.button(&ctx, "MenuBtn", "Actions", .{});
                        if (menu_btn_hovered and raylib.isMouseButtonPressed(.left)) {
                            menu_open = !menu_open;
                        }
                        if (menu_open) {
                            const menu_cfg = claykit.MenuConfig{};
                            claykit.menuDropdownBegin(&ctx, "Menu1", menu_cfg);
                            if (claykit.menuItem(&ctx, "Edit", false, menu_cfg) and raylib.isMouseButtonPressed(.left)) {
                                menu_open = false;
                            }
                            if (claykit.menuItem(&ctx, "Duplicate", false, menu_cfg) and raylib.isMouseButtonPressed(.left)) {
                                menu_open = false;
                            }
                            claykit.menuSeparator(&ctx, menu_cfg);
                            _ = claykit.menuItem(&ctx, "Delete", true, menu_cfg); // disabled
                            claykit.menuDropdownEnd();
                        }
                    });

                    // Popover - wrapped so popover attaches to button's parent, not column
                    zclay.text("Popover:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    zclay.UI()(.{
                        .id = zclay.ElementId.ID("PopoverWrap"),
                        .layout = .{ .sizing = .{ .w = .fit, .h = .fit } },
                    })({
                        const anchor_hovered = claykit.button(&ctx, "PopoverAnchor", "Hover me", .{});
                        if (anchor_hovered) {
                            show_popover = true;
                        } else {
                            show_popover = false;
                        }
                        if (show_popover) {
                            claykit.popoverBegin(&ctx, "Popover1", .{});
                            zclay.text("Popover content!", claykit.textStyle(&ctx, .{ .size = .sm }));
                            claykit.popoverEnd();
                        }
                    });

                    // Drawer trigger button
                    zclay.text("Drawer:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    if (claykit.button(&ctx, "OpenDrawer", "Open Drawer", .{})) {
                        if (raylib.isMouseButtonPressed(.left)) {
                            show_drawer = true;
                        }
                    }

                    // Modal trigger button
                    zclay.text("Modal:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    if (claykit.button(&ctx, "OpenModal", "Open Modal", .{})) {
                        if (raylib.isMouseButtonPressed(.left)) {
                            show_modal = true;
                        }
                    }

                    // Theme Colors
                    zclay.text("Theme:", claykit.textStyle(&ctx, .{ .size = .sm }));
                    zclay.UI()(.{
                        .id = zclay.ElementId.ID("Swatches"),
                        .layout = .{
                            .sizing = .{ .w = .grow },
                            .child_gap = 8,
                            .direction = .left_to_right,
                        },
                    })({
                        colorSwatch("Primary", theme.primary);
                        colorSwatch("Success", theme.success);
                        colorSwatch("Warning", theme.warning);
                        colorSwatch("Error", theme.@"error");
                    });
                });
            });

            // Footer
            zclay.UI()(.{
                .id = zclay.ElementId.ID("Footer"),
                .layout = .{
                    .sizing = .{ .w = .grow },
                    .padding = zclay.Padding.all(8),
                    .child_alignment = .{ .x = .center },
                },
                .background_color = toZclayColor(theme.border),
                .corner_radius = zclay.CornerRadius.all(@floatFromInt(theme.radius.sm)),
            })({
                zclay.text("ClayKit - Hand-written Zig bindings", .{
                    .font_size = theme.font_size.sm,
                    .color = toZclayColor(theme.muted),
                });
            });
        });

        // Drawer (rendered as floating overlay)
        if (show_drawer) {
            const drawer_backdrop_hovered = claykit.drawerBegin(&ctx, "Drawer1", .{ .side = .right });

            zclay.text("Drawer Content", .{
                .font_size = 20,
                .color = .{ 50, 50, 50, 255 },
            });
            zclay.text("This is a drawer panel that slides in from the right side.", .{
                .font_size = 14,
                .color = .{ 100, 100, 100, 255 },
            });

            const close_drawer_hovered = claykit.button(&ctx, "CloseDrawer", "Close Drawer", .{});

            claykit.drawerEnd();

            if (raylib.isMouseButtonPressed(.left) and (drawer_backdrop_hovered or close_drawer_hovered)) {
                show_drawer = false;
            }
        }

        // Modal (rendered as floating overlay)
        // Returns true when backdrop (not modal content) is hovered
        const backdrop_hovered = claykit.modal(&ctx, "DemoModal", show_modal, .{}, struct {
            fn content() void {
                zclay.text("Modal Title", .{
                    .font_size = 24,
                    .color = .{ 50, 50, 50, 255 },
                });
                zclay.text("This is a modal dialog. Click the backdrop or the close button to dismiss.", .{
                    .font_size = 16,
                    .color = .{ 100, 100, 100, 255 },
                });

                // Close button row
                zclay.UI()(.{
                    .layout = .{
                        .sizing = .{ .w = .grow },
                        .child_alignment = .{ .x = .right },
                    },
                })({
                    zclay.UI()(.{
                        .id = zclay.ElementId.ID("CloseModalBtn"),
                        .layout = .{
                            .padding = .{ .left = 16, .right = 16, .top = 8, .bottom = 8 },
                        },
                        .background_color = .{ 59, 130, 246, 255 },
                        .corner_radius = zclay.CornerRadius.all(6),
                    })({
                        zclay.text("Close", .{
                            .font_size = 14,
                            .color = .{ 255, 255, 255, 255 },
                        });
                    });
                });
            }
        }.content);

        // Close modal on backdrop click or close button click
        if (show_modal and raylib.isMouseButtonPressed(.left)) {
            if (backdrop_hovered) {
                show_modal = false;
            } else {
                const close_btn_id = zclay.ElementId.ID("CloseModalBtn");
                if (zclay.pointerOver(close_btn_id)) {
                    show_modal = false;
                }
            }
        }

        // End layout and get render commands
        const render_commands = zclay.endLayout();

        // Handle text input click (store for processing before next frame's layout)
        if (raylib.isMouseButtonPressed(.left)) {
            if (input_clicked) {
                // Focus immediately, store click for cursor positioning next frame
                input_state.setFocused(true);
                ctx.cursor_blink_time = 0;
                pending_input_click = true;
                pending_click_x = mouse_pos.x;
            } else {
                // Clicked elsewhere - unfocus
                input_state.setFocused(false);
            }
        }

        // Render with raylib
        raylib.beginDrawing();
        raylib.clearBackground(raylib.Color.white);

        for (render_commands) |cmd| {
            switch (cmd.command_type) {
                .rectangle => {
                    const config = cmd.render_data.rectangle;
                    drawRoundedRect(
                        cmd.bounding_box,
                        config.background_color,
                        config.corner_radius,
                    );
                },
                .text => {
                    const config = cmd.render_data.text;
                    const font = raylib_fonts[config.font_id];
                    // Copy to buffer and null-terminate (Clay strings aren't null-terminated)
                    const len: usize = @intCast(config.string_contents.length);
                    const safe_len = @min(len, text_buffer.len - 1);
                    @memcpy(text_buffer[0..safe_len], config.string_contents.chars[0..safe_len]);
                    text_buffer[safe_len] = 0;
                    const text_z: [:0]const u8 = text_buffer[0..safe_len :0];
                    raylib.drawTextEx(
                        font,
                        text_z,
                        .{ .x = cmd.bounding_box.x, .y = cmd.bounding_box.y },
                        @floatFromInt(config.font_size),
                        @floatFromInt(config.letter_spacing),
                        toRaylibColor(config.text_color),
                    );
                },
                .border => {
                    const config = cmd.render_data.border;
                    const rect = raylib.Rectangle{
                        .x = cmd.bounding_box.x,
                        .y = cmd.bounding_box.y,
                        .width = cmd.bounding_box.width,
                        .height = cmd.bounding_box.height,
                    };
                    const avg_radius = (config.corner_radius.top_left + config.corner_radius.top_right + config.corner_radius.bottom_left + config.corner_radius.bottom_right) / 4.0;
                    if (avg_radius > 0) {
                        const roundness = if (cmd.bounding_box.width > 0 and cmd.bounding_box.height > 0)
                            avg_radius / @min(cmd.bounding_box.width, cmd.bounding_box.height) * 2.0
                        else
                            0;
                        raylib.drawRectangleRoundedLinesEx(
                            rect,
                            roundness,
                            4,
                            @floatFromInt(config.width.top),
                            toRaylibColor(config.color),
                        );
                    } else {
                        raylib.drawRectangleLinesEx(
                            rect,
                            @floatFromInt(config.width.top),
                            toRaylibColor(config.color),
                        );
                    }
                },
                .custom => {
                    const custom = cmd.render_data.custom;
                    if (custom.custom_data) |ptr| {
                        const icon_data: *const claykit.IconRenderData = @ptrCast(@alignCast(ptr));
                        if (icon_data.type == claykit.CUSTOM_ICON) {
                            if (ctx.icon_callback) |cb| {
                                const bbox: claykit.BoundingBox = @bitCast(cmd.bounding_box);
                                cb(icon_data.icon_id, bbox, ctx.icon_user_data);
                            }
                        }
                    }
                },
                else => {},
            }
        }

        raylib.endDrawing();
    }
}

fn colorSwatch(id: []const u8, color: claykit.Color) void {
    zclay.UI()(.{
        .id = zclay.ElementId.ID(id),
        .layout = .{
            .sizing = .{
                .w = zclay.SizingAxis.fixed(32),
                .h = zclay.SizingAxis.fixed(32),
            },
        },
        .background_color = toZclayColor(color),
        .corner_radius = zclay.CornerRadius.all(4),
    })({});
}

fn toZclayColor(c: claykit.Color) zclay.Color {
    return .{ c.r, c.g, c.b, c.a };
}

fn toRaylibColor(c: zclay.Color) raylib.Color {
    return .{
        .r = @intFromFloat(c[0]),
        .g = @intFromFloat(c[1]),
        .b = @intFromFloat(c[2]),
        .a = @intFromFloat(c[3]),
    };
}

fn drawRoundedRect(bounds: zclay.BoundingBox, color: zclay.Color, radius: zclay.CornerRadius) void {
    const avg_radius = (radius.top_left + radius.top_right + radius.bottom_left + radius.bottom_right) / 4.0;
    const roundness = if (bounds.width > 0 and bounds.height > 0)
        avg_radius / @min(bounds.width, bounds.height) * 2.0
    else
        0;

    raylib.drawRectangleRounded(
        .{
            .x = bounds.x,
            .y = bounds.y,
            .width = bounds.width,
            .height = bounds.height,
        },
        roundness,
        4,
        toRaylibColor(color),
    );
}

fn getModifiers() u32 {
    var mods: u32 = 0;
    if (raylib.isKeyDown(.left_shift) or raylib.isKeyDown(.right_shift)) {
        mods |= @intFromEnum(claykit.Modifier.shift);
    }
    if (raylib.isKeyDown(.left_control) or raylib.isKeyDown(.right_control)) {
        mods |= @intFromEnum(claykit.Modifier.ctrl);
    }
    if (raylib.isKeyDown(.left_alt) or raylib.isKeyDown(.right_alt)) {
        mods |= @intFromEnum(claykit.Modifier.alt);
    }
    return mods;
}
