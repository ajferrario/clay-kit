const std = @import("std");
const zclay = @import("zclay");
const raylib = @import("raylib");
const claykit = @import("claykit");

const WINDOW_WIDTH = 1024;
const WINDOW_HEIGHT = 768;

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
                .padding = zclay.Padding.all(24),
                .child_gap = 16,
                .direction = .top_to_bottom,
            },
            .background_color = toZclayColor(theme.bg),
        })({
            // Header
            zclay.UI()(.{
                .id = zclay.ElementId.ID("Header"),
                .layout = .{
                    .sizing = .{ .w = .grow },
                    .padding = zclay.Padding.all(16),
                },
                .background_color = toZclayColor(theme.primary),
                .corner_radius = zclay.CornerRadius.all(@floatFromInt(theme.radius.md)),
            })({
                zclay.text("ClayKit Demo - Zig Bindings", .{
                    .font_size = theme.font_size.xl,
                    .color = .{ 255, 255, 255, 255 },
                });
            });

            // Content area
            zclay.UI()(.{
                .id = zclay.ElementId.ID("Content"),
                .layout = .{
                    .sizing = .{ .w = .grow, .h = .grow },
                    .child_gap = 16,
                    .direction = .left_to_right,
                },
            })({
                // Left panel
                zclay.UI()(.{
                    .id = zclay.ElementId.ID("LeftPanel"),
                    .layout = .{
                        .sizing = .{ .w = zclay.SizingAxis.fixed(300), .h = .fit },
                        .padding = zclay.Padding.all(16),
                        .child_gap = 10,
                        .direction = .top_to_bottom,
                    },
                    .background_color = toZclayColor(theme.secondary),
                    .corner_radius = zclay.CornerRadius.all(@floatFromInt(theme.radius.md)),
                })({
                    zclay.text("ClayKit Demo", claykit.headingStyle(&ctx, .{ .size = .lg }));

                    // Badge
                    claykit.badge(&ctx, "Badge", .{});

                    // Button
                    _ = claykit.button(&ctx, "Btn1", "Button", .{});

                    // Progress bar
                    zclay.text("Progress", claykit.textStyle(&ctx, .{ .size = .sm }));
                    claykit.progress(&ctx, "Progress1", 0.7, .{});

                    // Slider
                    zclay.text("Slider", claykit.textStyle(&ctx, .{ .size = .sm }));
                    _ = claykit.slider(&ctx, "Slider1", 0.5, .{});

                    // Text Input
                    zclay.text("Text Input", claykit.textStyle(&ctx, .{ .size = .sm }));
                    input_clicked = claykit.textInput(&ctx, "TextInput1", &input_state, .{}, "Type here...");

                    // Alerts
                    zclay.text("Alerts", claykit.textStyle(&ctx, .{ .size = .sm }));
                    claykit.alertText(&ctx, "Alert1", "Info alert", .{});
                    claykit.alertText(&ctx, "Alert2", "Success!", .{ .color_scheme = .success });

                    // Tooltip (static display for demo)
                    zclay.text("Tooltip", claykit.textStyle(&ctx, .{ .size = .sm }));
                    claykit.tooltip(&ctx, "Tooltip1", "This is a tooltip", .{});

                    // Tabs (line variant)
                    zclay.text("Tabs", claykit.textStyle(&ctx, .{ .size = .sm }));
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
                });

                // Center panel
                zclay.UI()(.{
                    .id = zclay.ElementId.ID("CenterPanel"),
                    .layout = .{
                        .sizing = .{ .w = .grow, .h = .grow },
                        .child_alignment = .{ .x = .center, .y = .center },
                    },
                    .background_color = .{ 240, 240, 245, 255 },
                    .corner_radius = zclay.CornerRadius.all(@floatFromInt(theme.radius.md)),
                })({
                    zclay.UI()(.{
                        .id = zclay.ElementId.ID("CenteredBox"),
                        .layout = .{
                            .sizing = .{
                                .w = zclay.SizingAxis.fixed(250),
                                .h = zclay.SizingAxis.fixed(120),
                            },
                            .child_alignment = .{ .x = .center, .y = .center },
                            .direction = .top_to_bottom,
                            .child_gap = 8,
                        },
                        .background_color = toZclayColor(theme.success),
                        .corner_radius = zclay.CornerRadius.all(@floatFromInt(theme.radius.lg)),
                    })({
                        zclay.text("Zig Bindings Work!", .{
                            .font_size = theme.font_size.lg,
                            .color = .{ 255, 255, 255, 255 },
                        });
                        zclay.text("No @cImport needed", .{
                            .font_size = theme.font_size.sm,
                            .color = .{ 220, 255, 220, 255 },
                        });
                    });
                });

                // Right panel - color swatches
                zclay.UI()(.{
                    .id = zclay.ElementId.ID("RightPanel"),
                    .layout = .{
                        .sizing = .{ .w = zclay.SizingAxis.fixed(200), .h = .grow },
                        .padding = zclay.Padding.all(16),
                        .child_gap = 12,
                        .direction = .top_to_bottom,
                    },
                    .background_color = toZclayColor(theme.secondary),
                    .corner_radius = zclay.CornerRadius.all(@floatFromInt(theme.radius.md)),
                })({
                    zclay.text("Theme Colors", .{
                        .font_size = theme.font_size.lg,
                        .color = toZclayColor(theme.fg),
                    });

                    // Color swatches row
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
                    .padding = zclay.Padding.all(12),
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
