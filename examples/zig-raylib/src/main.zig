const std = @import("std");
const zclay = @import("zclay");
const raylib = @import("raylib");

// Import clay_kit.h via @cImport
const claykit = @cImport({
    @cDefine("CLAYKIT_IMPLEMENTATION", {});
    @cInclude("clay_kit.h");
});

const WINDOW_WIDTH = 1024;
const WINDOW_HEIGHT = 768;

// Raylib font for measuring text
var raylib_fonts: [1]raylib.Font = undefined;

// Clay text measurement callback
fn measureText(text_slice: []const u8, config: *zclay.TextElementConfig, _: usize) zclay.Dimensions {
    const font = raylib_fonts[config.fontId];
    const font_size: f32 = @floatFromInt(config.fontSize);
    const letter_spacing: f32 = @floatFromInt(config.letterSpacing);

    var text_size = raylib.measureTextEx(
        font,
        @ptrCast(text_slice.ptr),
        font_size,
        letter_spacing,
    );

    // Handle empty strings
    if (text_size.x == 0) {
        text_size.x = 1;
    }

    return .{ .width = text_size.x, .height = text_size.y };
}

pub fn main() !void {
    // Initialize raylib
    raylib.initWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ClayKit + Raylib Demo");
    defer raylib.closeWindow();
    raylib.setTargetFPS(60);

    // Load font
    raylib_fonts[0] = raylib.loadFontEx("resources/Roboto-Regular.ttf", 48, null) orelse
        raylib.getFontDefault();

    // Initialize Clay
    const min_memory_size: u32 = zclay.minMemorySize();
    const memory = std.heap.page_allocator.alloc(u8, min_memory_size) catch {
        std.debug.print("Failed to allocate memory for Clay\n", .{});
        return;
    };
    defer std.heap.page_allocator.free(memory);

    const arena: zclay.Arena = zclay.createArenaWithCapacityAndMemory(memory);
    _ = zclay.initialize(arena, .{
        .width = WINDOW_WIDTH,
        .height = WINDOW_HEIGHT,
    }, null);
    zclay.setMeasureTextFunction(measureText, 0);

    // Initialize ClayKit
    var theme = claykit.CLAYKIT_THEME_LIGHT;
    var state_buf: [64]claykit.ClayKit_State = undefined;
    var ctx: claykit.ClayKit_Context = undefined;
    claykit.ClayKit_Init(&ctx, &theme, &state_buf, 64);

    // Main loop
    while (!raylib.windowShouldClose()) {
        // Update Clay layout dimensions if window resized
        const screen_width: f32 = @floatFromInt(raylib.getScreenWidth());
        const screen_height: f32 = @floatFromInt(raylib.getScreenHeight());
        zclay.setLayoutDimensions(.{ .width = screen_width, .height = screen_height });

        // Update pointer state
        const mouse_pos = raylib.getMousePosition();
        zclay.setPointerState(.{ .x = mouse_pos.x, .y = mouse_pos.y }, raylib.isMouseButtonDown(.mouse_button_left));

        // Begin frame
        claykit.ClayKit_BeginFrame(&ctx);

        // Build UI layout
        zclay.beginLayout();

        // Root container - full screen
        if (zclay.openElement()) |root| {
            root.configure(.{
                .id = zclay.id("Root"),
                .layout = .{
                    .sizing = .{
                        .width = zclay.Element.Sizing.grow(.{}),
                        .height = zclay.Element.Sizing.grow(.{}),
                    },
                    .padding = zclay.Element.Padding.all(24),
                    .layoutDirection = .top_to_bottom,
                    .childGap = 16,
                },
                .backgroundColor = toZclayColor(theme.bg),
            });

            // Header
            if (zclay.openElement()) |header| {
                header.configure(.{
                    .id = zclay.id("Header"),
                    .layout = .{
                        .sizing = .{ .width = zclay.Element.Sizing.grow(.{}) },
                        .padding = zclay.Element.Padding.all(16),
                        .childAlignment = .{ .y = .center },
                    },
                    .backgroundColor = toZclayColor(theme.primary),
                    .cornerRadius = zclay.CornerRadius.all(@floatFromInt(theme.radius.md)),
                });

                zclay.text("ClayKit Demo - Layout Primitives", .{
                    .fontSize = theme.font_size.xl,
                    .textColor = .{ .r = 255, .g = 255, .b = 255, .a = 255 },
                });

                header.close();
            }

            // Content area with boxes
            if (zclay.openElement()) |content| {
                content.configure(.{
                    .id = zclay.id("Content"),
                    .layout = .{
                        .sizing = .{
                            .width = zclay.Element.Sizing.grow(.{}),
                            .height = zclay.Element.Sizing.grow(.{}),
                        },
                        .childGap = 16,
                        .layoutDirection = .left_to_right,
                    },
                });

                // Left panel - demonstrates VStack
                if (zclay.openElement()) |left_panel| {
                    left_panel.configure(.{
                        .id = zclay.id("LeftPanel"),
                        .layout = .{
                            .sizing = .{
                                .width = zclay.Element.Sizing.fixed(300),
                                .height = zclay.Element.Sizing.grow(.{}),
                            },
                            .padding = zclay.Element.Padding.all(16),
                            .layoutDirection = .top_to_bottom,
                            .childGap = 12,
                        },
                        .backgroundColor = toZclayColor(theme.secondary),
                        .cornerRadius = zclay.CornerRadius.all(@floatFromInt(theme.radius.md)),
                    });

                    zclay.text("VStack Example", .{
                        .fontSize = theme.font_size.lg,
                        .textColor = toZclayColor(theme.fg),
                    });

                    // Stack items
                    for (0..4) |i| {
                        if (zclay.openElement()) |item| {
                            item.configure(.{
                                .id = zclay.idI("StackItem", @intCast(i)),
                                .layout = .{
                                    .sizing = .{ .width = zclay.Element.Sizing.grow(.{}) },
                                    .padding = zclay.Element.Padding.all(12),
                                    .childAlignment = .{ .x = .center },
                                },
                                .backgroundColor = toZclayColor(theme.bg),
                                .cornerRadius = zclay.CornerRadius.all(@floatFromInt(theme.radius.sm)),
                            });

                            var buf: [32]u8 = undefined;
                            const label = std.fmt.bufPrint(&buf, "Item {d}", .{i + 1}) catch "Item";
                            zclay.text(label, .{
                                .fontSize = theme.font_size.md,
                                .textColor = toZclayColor(theme.fg),
                            });

                            item.close();
                        }
                    }

                    left_panel.close();
                }

                // Center panel - demonstrates Center
                if (zclay.openElement()) |center_panel| {
                    center_panel.configure(.{
                        .id = zclay.id("CenterPanel"),
                        .layout = .{
                            .sizing = .{
                                .width = zclay.Element.Sizing.grow(.{}),
                                .height = zclay.Element.Sizing.grow(.{}),
                            },
                            .childAlignment = .{ .x = .center, .y = .center },
                        },
                        .backgroundColor = .{ .r = 240, .g = 240, .b = 245, .a = 255 },
                        .cornerRadius = zclay.CornerRadius.all(@floatFromInt(theme.radius.md)),
                    });

                    if (zclay.openElement()) |centered_box| {
                        centered_box.configure(.{
                            .id = zclay.id("CenteredBox"),
                            .layout = .{
                                .sizing = .{
                                    .width = zclay.Element.Sizing.fixed(200),
                                    .height = zclay.Element.Sizing.fixed(100),
                                },
                                .childAlignment = .{ .x = .center, .y = .center },
                            },
                            .backgroundColor = toZclayColor(theme.success),
                            .cornerRadius = zclay.CornerRadius.all(@floatFromInt(theme.radius.lg)),
                        });

                        zclay.text("Centered!", .{
                            .fontSize = theme.font_size.lg,
                            .textColor = .{ .r = 255, .g = 255, .b = 255, .a = 255 },
                        });

                        centered_box.close();
                    }

                    center_panel.close();
                }

                // Right panel - demonstrates HStack
                if (zclay.openElement()) |right_panel| {
                    right_panel.configure(.{
                        .id = zclay.id("RightPanel"),
                        .layout = .{
                            .sizing = .{
                                .width = zclay.Element.Sizing.fixed(300),
                                .height = zclay.Element.Sizing.grow(.{}),
                            },
                            .padding = zclay.Element.Padding.all(16),
                            .layoutDirection = .top_to_bottom,
                            .childGap = 12,
                        },
                        .backgroundColor = toZclayColor(theme.secondary),
                        .cornerRadius = zclay.CornerRadius.all(@floatFromInt(theme.radius.md)),
                    });

                    zclay.text("Theme Colors", .{
                        .fontSize = theme.font_size.lg,
                        .textColor = toZclayColor(theme.fg),
                    });

                    // Color swatches using HStack
                    if (zclay.openElement()) |swatches| {
                        swatches.configure(.{
                            .id = zclay.id("Swatches"),
                            .layout = .{
                                .sizing = .{ .width = zclay.Element.Sizing.grow(.{}) },
                                .childGap = 8,
                                .layoutDirection = .left_to_right,
                            },
                        });

                        // Primary
                        colorSwatch("SwatchPrimary", theme.primary);
                        // Success
                        colorSwatch("SwatchSuccess", theme.success);
                        // Warning
                        colorSwatch("SwatchWarning", theme.warning);
                        // Error
                        colorSwatch("SwatchError", theme.error);

                        swatches.close();
                    }

                    // Spacer demonstration
                    zclay.text("Spacer pushes content:", .{
                        .fontSize = theme.font_size.sm,
                        .textColor = toZclayColor(theme.muted),
                    });

                    // Spacer - grows to fill space
                    if (zclay.openElement()) |spacer| {
                        spacer.configure(.{
                            .id = zclay.id("Spacer"),
                            .layout = .{
                                .sizing = .{
                                    .width = zclay.Element.Sizing.grow(.{}),
                                    .height = zclay.Element.Sizing.grow(.{}),
                                },
                            },
                        });
                        spacer.close();
                    }

                    // Footer pushed to bottom by spacer
                    if (zclay.openElement()) |footer_box| {
                        footer_box.configure(.{
                            .id = zclay.id("FooterBox"),
                            .layout = .{
                                .sizing = .{ .width = zclay.Element.Sizing.grow(.{}) },
                                .padding = zclay.Element.Padding.all(12),
                                .childAlignment = .{ .x = .center },
                            },
                            .backgroundColor = toZclayColor(theme.bg),
                            .cornerRadius = zclay.CornerRadius.all(@floatFromInt(theme.radius.sm)),
                        });

                        zclay.text("Pushed to bottom!", .{
                            .fontSize = theme.font_size.md,
                            .textColor = toZclayColor(theme.fg),
                        });

                        footer_box.close();
                    }

                    right_panel.close();
                }

                content.close();
            }

            // Footer
            if (zclay.openElement()) |footer| {
                footer.configure(.{
                    .id = zclay.id("Footer"),
                    .layout = .{
                        .sizing = .{ .width = zclay.Element.Sizing.grow(.{}) },
                        .padding = zclay.Element.Padding.all(12),
                        .childAlignment = .{ .x = .center },
                    },
                    .backgroundColor = toZclayColor(theme.border),
                    .cornerRadius = zclay.CornerRadius.all(@floatFromInt(theme.radius.sm)),
                });

                zclay.text("ClayKit - Zero-allocation UI components for Clay", .{
                    .fontSize = theme.font_size.sm,
                    .textColor = toZclayColor(theme.muted),
                });

                footer.close();
            }

            root.close();
        }

        // End layout and get render commands
        const render_commands = zclay.endLayout();

        // Render with raylib
        raylib.beginDrawing();
        raylib.clearBackground(raylib.Color.white);

        for (render_commands.items) |cmd| {
            switch (cmd.commandType) {
                .rectangle => {
                    const config = cmd.renderData.rectangle;
                    drawRoundedRect(
                        cmd.boundingBox,
                        config.backgroundColor,
                        config.cornerRadius,
                    );
                },
                .text => {
                    const config = cmd.renderData.text;
                    const font = raylib_fonts[config.fontId];
                    raylib.drawTextEx(
                        font,
                        @ptrCast(config.text.chars),
                        .{ .x = cmd.boundingBox.x, .y = cmd.boundingBox.y },
                        @floatFromInt(config.fontSize),
                        @floatFromInt(config.letterSpacing),
                        toRaylibColor(config.textColor),
                    );
                },
                .border => {
                    const config = cmd.renderData.border;
                    // Simple border drawing (just outline for now)
                    raylib.drawRectangleLinesEx(
                        .{
                            .x = cmd.boundingBox.x,
                            .y = cmd.boundingBox.y,
                            .width = cmd.boundingBox.width,
                            .height = cmd.boundingBox.height,
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

fn colorSwatch(id: []const u8, color: claykit.Clay_Color) void {
    if (zclay.openElement()) |swatch| {
        swatch.configure(.{
            .id = zclay.hashString(id, 0),
            .layout = .{
                .sizing = .{
                    .width = zclay.Element.Sizing.fixed(40),
                    .height = zclay.Element.Sizing.fixed(40),
                },
            },
            .backgroundColor = toZclayColor(color),
            .cornerRadius = zclay.CornerRadius.all(4),
        });
        swatch.close();
    }
}

fn toZclayColor(c: claykit.Clay_Color) zclay.Color {
    return .{
        .r = @floatFromInt(c.r),
        .g = @floatFromInt(c.g),
        .b = @floatFromInt(c.b),
        .a = @floatFromInt(c.a),
    };
}

fn toRaylibColor(c: zclay.Color) raylib.Color {
    return .{
        .r = @intFromFloat(c.r),
        .g = @intFromFloat(c.g),
        .b = @intFromFloat(c.b),
        .a = @intFromFloat(c.a),
    };
}

fn drawRoundedRect(bounds: zclay.BoundingBox, color: zclay.Color, radius: zclay.CornerRadius) void {
    const avg_radius = (radius.topLeft + radius.topRight + radius.bottomLeft + radius.bottomRight) / 4.0;
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
