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

    // Main loop
    while (!raylib.windowShouldClose()) {
        // Update Clay layout dimensions if window resized
        const screen_width: f32 = @floatFromInt(raylib.getScreenWidth());
        const screen_height: f32 = @floatFromInt(raylib.getScreenHeight());
        zclay.setLayoutDimensions(.{ .w = screen_width, .h = screen_height });

        // Update pointer state
        const mouse_pos = raylib.getMousePosition();
        zclay.setPointerState(.{ .x = mouse_pos.x, .y = mouse_pos.y }, raylib.isMouseButtonDown(.left));

        // Begin frame
        claykit.beginFrame(&ctx);

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
                        .sizing = .{ .w = zclay.SizingAxis.fixed(300), .h = .grow },
                        .padding = zclay.Padding.all(16),
                        .child_gap = 12,
                        .direction = .top_to_bottom,
                    },
                    .background_color = toZclayColor(theme.secondary),
                    .corner_radius = zclay.CornerRadius.all(@floatFromInt(theme.radius.md)),
                })({
                    // Use ClayKit heading style
                    zclay.text("Typography Demo", claykit.headingStyle(&ctx, .{ .size = .lg }));

                    // Use ClayKit text style
                    zclay.text("Using ClayKit text styles:", claykit.textStyle(&ctx, .{ .size = .sm, .color = theme.muted }));

                    // Show state count from context
                    var buf: [32]u8 = undefined;
                    const count_str = std.fmt.bufPrint(&buf, "{d} states allocated", .{ctx.state_count}) catch "?";
                    zclay.text(count_str, claykit.textStyle(&ctx, .{ .size = .md }));

                    // Badge demos
                    zclay.UI()(.{
                        .id = zclay.ElementId.ID("BadgeRow"),
                        .layout = .{
                            .sizing = .{ .w = .grow },
                            .child_gap = 8,
                            .direction = .left_to_right,
                        },
                    })({
                        claykit.badge(&ctx, "Primary", .{ .color_scheme = .primary, .variant = .solid });
                        claykit.badge(&ctx, "Success", .{ .color_scheme = .success, .variant = .solid });
                        claykit.badge(&ctx, "Warning", .{ .color_scheme = .warning, .variant = .solid });
                    });

                    // Subtle badges
                    zclay.UI()(.{
                        .id = zclay.ElementId.ID("BadgeRow2"),
                        .layout = .{
                            .sizing = .{ .w = .grow },
                            .child_gap = 8,
                            .direction = .left_to_right,
                        },
                    })({
                        claykit.badge(&ctx, "Subtle", .{ .color_scheme = .primary, .variant = .subtle });
                        claykit.badge(&ctx, "Outline", .{ .color_scheme = .primary, .variant = .outline });
                    });
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
                    raylib.drawRectangleLinesEx(
                        .{
                            .x = cmd.bounding_box.x,
                            .y = cmd.bounding_box.y,
                            .width = cmd.bounding_box.width,
                            .height = cmd.bounding_box.height,
                        },
                        @floatFromInt(config.width.top),
                        toRaylibColor(config.color),
                    );
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
