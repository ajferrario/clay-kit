/*
 * ClayKit C + Raylib Demo
 *
 * This example mirrors the Zig demo to verify all ClayKit
 * components work correctly from pure C.
 *
 * Build: See Makefile
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* Raylib from vendor/raylib/src */
#include "raylib/src/raylib.h"

/* Clay from vendor */
#define CLAY_IMPLEMENTATION
#include "clay.h"

/* ClayKit from root */
#define CLAYKIT_IMPLEMENTATION
#include "clay_kit.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

/* Raylib font for text rendering */
static Font raylib_font;

/* Text buffer for null-termination */
static char text_buffer[4096];

/* Text input state */
static char input_buffer[256];
static ClayKit_InputState input_state;

/* UI state */
static int active_tab = 0;
static bool show_modal = false;

/* Pending click state */
static bool pending_input_click = false;
static float pending_click_x = 0;

/* Interaction states - set during UI building, used after layout */
static bool input_hovered = false;
static int tab_hovered = -1;  /* -1 = none, 0-2 = tab index */
static bool modal_btn_hovered = false;
static bool close_modal_btn_hovered = false;
static bool backdrop_hovered = false;

/* Forward declarations */
static void render_demo_ui(ClayKit_Context *ctx, ClayKit_Theme *theme);
static Clay_Dimensions measure_text(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData);

/* Raylib color conversion */
static Color to_raylib_color(Clay_Color c) {
    return (Color){ (unsigned char)c.r, (unsigned char)c.g, (unsigned char)c.b, (unsigned char)c.a };
}

/* Text measurement callback for ClayKit */
static ClayKit_TextDimensions measure_text_for_claykit(
    const char *text, uint32_t length, uint16_t font_id, uint16_t font_size, void *user_data
) {
    (void)font_id;
    (void)user_data;

    if (length == 0) {
        return (ClayKit_TextDimensions){ 0, (float)font_size };
    }

    /* Copy to buffer and null-terminate */
    uint32_t safe_len = length < sizeof(text_buffer) - 1 ? length : sizeof(text_buffer) - 1;
    memcpy(text_buffer, text, safe_len);
    text_buffer[safe_len] = '\0';

    Vector2 size = MeasureTextEx(raylib_font, text_buffer, (float)font_size, 0);
    return (ClayKit_TextDimensions){ size.x, size.y };
}

/* Text measurement callback for Clay */
static Clay_Dimensions measure_text(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData) {
    (void)userData;

    if (text.length == 0) {
        return (Clay_Dimensions){ 1, (float)config->fontSize };
    }

    /* Copy to buffer and null-terminate */
    uint32_t len = (uint32_t)text.length;
    uint32_t safe_len = len < sizeof(text_buffer) - 1 ? len : sizeof(text_buffer) - 1;
    memcpy(text_buffer, text.chars, safe_len);
    text_buffer[safe_len] = '\0';

    Vector2 size = MeasureTextEx(raylib_font, text_buffer, (float)config->fontSize, (float)config->letterSpacing);
    if (size.x == 0) size.x = 1;

    return (Clay_Dimensions){ size.x, size.y };
}

/* Get keyboard modifiers */
static uint32_t get_modifiers(void) {
    uint32_t mods = 0;
    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
        mods |= CLAYKIT_MOD_SHIFT;
    }
    if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
        mods |= CLAYKIT_MOD_CTRL;
    }
    if (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) {
        mods |= CLAYKIT_MOD_ALT;
    }
    return mods;
}

/* Draw a rounded rectangle */
static void draw_rounded_rect(Clay_BoundingBox bounds, Clay_Color color, Clay_CornerRadius radius) {
    float avg_radius = (radius.topLeft + radius.topRight + radius.bottomLeft + radius.bottomRight) / 4.0f;
    float min_dim = bounds.width < bounds.height ? bounds.width : bounds.height;
    float roundness = (min_dim > 0) ? (avg_radius / min_dim * 2.0f) : 0;

    DrawRectangleRounded(
        (Rectangle){ bounds.x, bounds.y, bounds.width, bounds.height },
        roundness, 4, to_raylib_color(color)
    );
}

/* Helper to open a container element using low-level API */
static void open_container(Clay_SizingAxis width, Clay_SizingAxis height,
                          Clay_Padding padding, uint16_t gap,
                          Clay_LayoutDirection direction, Clay_ChildAlignment align,
                          Clay_Color bg, float corner_radius) {
    Clay_ElementDeclaration decl = {0};
    decl.layout.sizing.width = width;
    decl.layout.sizing.height = height;
    decl.layout.padding = padding;
    decl.layout.childGap = gap;
    decl.layout.layoutDirection = direction;
    decl.layout.childAlignment = align;
    decl.backgroundColor = bg;
    decl.cornerRadius.topLeft = corner_radius;
    decl.cornerRadius.topRight = corner_radius;
    decl.cornerRadius.bottomLeft = corner_radius;
    decl.cornerRadius.bottomRight = corner_radius;

    Clay__OpenElement();
    Clay__ConfigureOpenElement(decl);
}

/* Shorthand for grow sizing */
static Clay_SizingAxis sizing_grow(void) {
    return (Clay_SizingAxis){ .type = CLAY__SIZING_TYPE_GROW };
}

/* Shorthand for fit sizing */
static Clay_SizingAxis sizing_fit(void) {
    return (Clay_SizingAxis){ .type = CLAY__SIZING_TYPE_FIT };
}

/* Shorthand for fixed sizing */
static Clay_SizingAxis sizing_fixed(float size) {
    return (Clay_SizingAxis){
        .type = CLAY__SIZING_TYPE_FIXED,
        .size.minMax = { size, size }
    };
}

/* Shorthand for padding */
static Clay_Padding padding_all(uint16_t p) {
    return (Clay_Padding){ p, p, p, p };
}

/* Add text element */
static void add_text(const char *str, uint16_t font_size, Clay_Color color) {
    Clay_String clay_str = { false, (int32_t)strlen(str), str };
    Clay_TextElementConfig config = {0};
    config.fontSize = font_size;
    config.textColor = color;
    config.wrapMode = CLAY_TEXT_WRAP_WORDS;

    /* Must store the config via Clay's internal storage */
    Clay_TextElementConfig *stored_config = Clay__StoreTextElementConfig(config);
    Clay__OpenTextElement(clay_str, stored_config);
}

int main(void) {
    /* Initialize raylib */
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ClayKit + Raylib Demo (C)");
    SetTargetFPS(60);

    /* Load font */
    raylib_font = LoadFontEx("resources/Roboto-Regular.ttf", 48, NULL, 0);
    if (raylib_font.glyphCount == 0) {
        printf("Warning: Could not load font from resources/Roboto-Regular.ttf, using default\n");
        raylib_font = GetFontDefault();
    } else {
        printf("Loaded font with %d glyphs, base size %d\n", raylib_font.glyphCount, raylib_font.baseSize);
        SetTextureFilter(raylib_font.texture, TEXTURE_FILTER_BILINEAR);
    }

    /* Initialize Clay */
    uint32_t min_memory = Clay_MinMemorySize();
    void *clay_memory = malloc(min_memory);
    if (!clay_memory) {
        printf("Failed to allocate Clay memory\n");
        return 1;
    }

    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(min_memory, clay_memory);
    Clay_Initialize(arena, (Clay_Dimensions){ WINDOW_WIDTH, WINDOW_HEIGHT }, (Clay_ErrorHandler){0});
    Clay_SetMeasureTextFunction(measure_text, NULL);

    /* Initialize ClayKit */
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[64] = {0};
    ClayKit_Context ctx = {0};
    ClayKit_Init(&ctx, &theme, state_buf, 64);
    ctx.measure_text = measure_text_for_claykit;

    /* Initialize text input state */
    memset(input_buffer, 0, sizeof(input_buffer));
    input_state.buf = input_buffer;
    input_state.cap = sizeof(input_buffer);
    input_state.len = 0;
    input_state.cursor = 0;
    input_state.select_start = 0;
    input_state.flags = 0;

    /* Main loop */
    while (!WindowShouldClose()) {
        /* Update cursor blink timer */
        ctx.cursor_blink_time += GetFrameTime();

        /* Handle keyboard input for text field */
        if (input_state.flags & CLAYKIT_INPUT_FOCUSED) {
            if (IsKeyPressed(KEY_BACKSPACE)) {
                ClayKit_InputHandleKey(&input_state, CLAYKIT_KEY_BACKSPACE, get_modifiers());
            }
            if (IsKeyPressed(KEY_DELETE)) {
                ClayKit_InputHandleKey(&input_state, CLAYKIT_KEY_DELETE, get_modifiers());
            }
            if (IsKeyPressed(KEY_LEFT)) {
                ClayKit_InputHandleKey(&input_state, CLAYKIT_KEY_LEFT, get_modifiers());
            }
            if (IsKeyPressed(KEY_RIGHT)) {
                ClayKit_InputHandleKey(&input_state, CLAYKIT_KEY_RIGHT, get_modifiers());
            }
            if (IsKeyPressed(KEY_HOME)) {
                ClayKit_InputHandleKey(&input_state, CLAYKIT_KEY_HOME, get_modifiers());
            }
            if (IsKeyPressed(KEY_END)) {
                ClayKit_InputHandleKey(&input_state, CLAYKIT_KEY_END, get_modifiers());
            }

            /* Handle character input */
            int ch = GetCharPressed();
            while (ch != 0) {
                ClayKit_InputHandleChar(&input_state, (uint32_t)ch);
                ch = GetCharPressed();
            }
        }

        /* Update Clay layout dimensions */
        Clay_SetLayoutDimensions((Clay_Dimensions){
            (float)GetScreenWidth(),
            (float)GetScreenHeight()
        });

        /* Update pointer state */
        Vector2 mouse = GetMousePosition();
        Clay_SetPointerState((Clay_Vector2){ mouse.x, mouse.y }, IsMouseButtonDown(MOUSE_LEFT_BUTTON));

        /* Begin frame */
        ClayKit_BeginFrame(&ctx);

        /* Process pending click from last frame (for cursor positioning) */
        if (pending_input_click) {
            pending_input_click = false;
            ClayKit_InputStyle style = ClayKit_ComputeInputStyle(&ctx, (ClayKit_InputConfig){0}, true);

            /* Get input element bounding box using same ID as ClayKit_TextInput */
            Clay_String id_str = { false, 9, "TextInput" };
            Clay_ElementId input_id = Clay__HashString(id_str, 0, 0);
            Clay_ElementData elem = Clay_GetElementData(input_id);
            if (elem.found) {
                float local_x = pending_click_x - elem.boundingBox.x - style.padding_x;
                uint32_t new_cursor = ClayKit_InputGetCursorFromX(
                    &ctx, input_state.buf, input_state.len,
                    style.font_id, style.font_size, local_x
                );
                input_state.cursor = new_cursor;
                input_state.select_start = new_cursor;
            }
        }

        /* Reset hover states before building UI */
        input_hovered = false;
        tab_hovered = -1;
        modal_btn_hovered = false;
        close_modal_btn_hovered = false;
        backdrop_hovered = false;

        /* Build UI */
        Clay_BeginLayout();

        render_demo_ui(&ctx, &theme);

        /* End layout and get render commands */
        Clay_RenderCommandArray commands = Clay_EndLayout();

        /* Handle interactions after layout */
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            /* Text input focus */
            if (input_hovered) {
                input_state.flags |= CLAYKIT_INPUT_FOCUSED;
                ctx.cursor_blink_time = 0;
                pending_input_click = true;
                pending_click_x = mouse.x;
            } else if (!show_modal) {
                /* Only unfocus if not clicking on modal */
                input_state.flags &= ~CLAYKIT_INPUT_FOCUSED;
            }

            /* Tab switching */
            if (tab_hovered >= 0) {
                active_tab = tab_hovered;
            }

            /* Modal open */
            if (modal_btn_hovered) {
                show_modal = true;
            }

            /* Modal close (backdrop or close button) */
            if (show_modal && (backdrop_hovered || close_modal_btn_hovered)) {
                show_modal = false;
            }
        }

        /* Render */
        BeginDrawing();
        ClearBackground(WHITE);

        for (int32_t i = 0; i < commands.length; i++) {
            Clay_RenderCommand *cmd = Clay_RenderCommandArray_Get(&commands, i);

            switch (cmd->commandType) {
                case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
                    Clay_RectangleRenderData *rect = &cmd->renderData.rectangle;
                    draw_rounded_rect(cmd->boundingBox, rect->backgroundColor, rect->cornerRadius);
                    break;
                }
                case CLAY_RENDER_COMMAND_TYPE_TEXT: {
                    Clay_TextRenderData *text = &cmd->renderData.text;
                    /* Null-terminate the string */
                    uint32_t len = (uint32_t)text->stringContents.length;
                    uint32_t safe_len = len < sizeof(text_buffer) - 1 ? len : sizeof(text_buffer) - 1;
                    memcpy(text_buffer, text->stringContents.chars, safe_len);
                    text_buffer[safe_len] = '\0';

                    Color textColor = to_raylib_color(text->textColor);

                    DrawTextEx(
                        raylib_font, text_buffer,
                        (Vector2){ cmd->boundingBox.x, cmd->boundingBox.y },
                        (float)text->fontSize, (float)text->letterSpacing,
                        textColor
                    );
                    break;
                }
                case CLAY_RENDER_COMMAND_TYPE_BORDER: {
                    Clay_BorderRenderData *border = &cmd->renderData.border;
                    Rectangle rect = {
                        cmd->boundingBox.x, cmd->boundingBox.y,
                        cmd->boundingBox.width, cmd->boundingBox.height
                    };
                    float avg_radius = (border->cornerRadius.topLeft + border->cornerRadius.topRight +
                                       border->cornerRadius.bottomLeft + border->cornerRadius.bottomRight) / 4.0f;
                    float min_dim = rect.width < rect.height ? rect.width : rect.height;
                    float roundness = (min_dim > 0) ? (avg_radius / min_dim * 2.0f) : 0;

                    /* Note: Latest raylib removed line thickness from DrawRectangleRoundedLines */
                    if (avg_radius > 0) {
                        DrawRectangleRoundedLines(rect, roundness, 4, to_raylib_color(border->color));
                    } else {
                        DrawRectangleLinesEx(rect, (float)border->width.top, to_raylib_color(border->color));
                    }
                    break;
                }
                default:
                    break;
            }
        }

        EndDrawing();
    }

    /* Cleanup */
    UnloadFont(raylib_font);
    free(clay_memory);
    CloseWindow();

    return 0;
}

/* Render the demo UI using ClayKit components */
static void render_demo_ui(ClayKit_Context *ctx, ClayKit_Theme *theme) {
    /* Root container */
    open_container(
        sizing_grow(), sizing_grow(),
        padding_all(24), 16,
        CLAY_TOP_TO_BOTTOM,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_TOP },
        theme->bg, 0
    );

    /* Header */
    open_container(
        sizing_grow(), sizing_fit(),
        padding_all(16), 0,
        CLAY_LEFT_TO_RIGHT,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER },
        theme->primary, (float)theme->radius.md
    );
    add_text("ClayKit Demo - Pure C", theme->font_size.xl, (Clay_Color){ 255, 255, 255, 255 });
    Clay__CloseElement(); /* Header */

    /* Content area */
    open_container(
        sizing_grow(), sizing_grow(),
        (Clay_Padding){0}, 16,
        CLAY_LEFT_TO_RIGHT,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_TOP },
        (Clay_Color){0}, 0
    );

    /* Left panel */
    open_container(
        sizing_fixed(300), sizing_fit(),
        padding_all(16), 10,
        CLAY_TOP_TO_BOTTOM,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_TOP },
        theme->secondary, (float)theme->radius.md
    );

    add_text("Components", theme->font_size.lg, theme->fg);

    /* Badge */
    add_text("Badge:", theme->font_size.sm, theme->muted);
    ClayKit_BadgeRaw(ctx, "Badge", 5, (ClayKit_BadgeConfig){0});

    /* Button */
    add_text("Button:", theme->font_size.sm, theme->muted);
    ClayKit_Button(ctx, "Click Me", 8, (ClayKit_ButtonConfig){0});

    /* Progress */
    add_text("Progress:", theme->font_size.sm, theme->muted);
    ClayKit_Progress(ctx, 0.7f, (ClayKit_ProgressConfig){0});

    /* Slider */
    add_text("Slider:", theme->font_size.sm, theme->muted);
    ClayKit_Slider(ctx, 0.5f, (ClayKit_SliderConfig){0});

    /* Text Input - capture hover state */
    add_text("Text Input:", theme->font_size.sm, theme->muted);
    input_hovered = ClayKit_TextInput(ctx, "TextInput", 9, &input_state, (ClayKit_InputConfig){0}, "Type here...", 12);

    /* Alert */
    add_text("Alert:", theme->font_size.sm, theme->muted);
    ClayKit_AlertText(ctx, "Info alert message", 18, (ClayKit_AlertConfig){0});
    ClayKit_AlertText(ctx, "Success!", 8, (ClayKit_AlertConfig){ .color_scheme = CLAYKIT_COLOR_SUCCESS });

    /* Tooltip */
    add_text("Tooltip:", theme->font_size.sm, theme->muted);
    ClayKit_Tooltip(ctx, "This is a tooltip", 17, (ClayKit_TooltipConfig){0});

    /* Tabs - capture hover states */
    add_text("Tabs:", theme->font_size.sm, theme->muted);
    open_container(
        sizing_grow(), sizing_fit(),
        (Clay_Padding){0}, 0,
        CLAY_LEFT_TO_RIGHT,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER },
        (Clay_Color){0}, 0
    );
    if (ClayKit_Tab(ctx, "Tab 1", 5, active_tab == 0, (ClayKit_TabsConfig){0})) tab_hovered = 0;
    if (ClayKit_Tab(ctx, "Tab 2", 5, active_tab == 1, (ClayKit_TabsConfig){0})) tab_hovered = 1;
    if (ClayKit_Tab(ctx, "Tab 3", 5, active_tab == 2, (ClayKit_TabsConfig){0})) tab_hovered = 2;
    Clay__CloseElement();

    /* Second tabs row with enclosed variant */
    open_container(
        sizing_grow(), sizing_fit(),
        (Clay_Padding){0}, 0,
        CLAY_LEFT_TO_RIGHT,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER },
        (Clay_Color){0}, 0
    );
    ClayKit_TabsConfig enclosed_cfg = { .variant = CLAYKIT_TABS_ENCLOSED };
    if (ClayKit_Tab(ctx, "Tab 1", 5, active_tab == 0, enclosed_cfg)) tab_hovered = 0;
    if (ClayKit_Tab(ctx, "Tab 2", 5, active_tab == 1, enclosed_cfg)) tab_hovered = 1;
    if (ClayKit_Tab(ctx, "Tab 3", 5, active_tab == 2, enclosed_cfg)) tab_hovered = 2;
    Clay__CloseElement();

    /* Modal button */
    add_text("Modal:", theme->font_size.sm, theme->muted);
    modal_btn_hovered = ClayKit_Button(ctx, "Open Modal", 10, (ClayKit_ButtonConfig){0});

    Clay__CloseElement(); /* Left panel */

    /* Center panel */
    open_container(
        sizing_grow(), sizing_grow(),
        (Clay_Padding){0}, 0,
        CLAY_LEFT_TO_RIGHT,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
        (Clay_Color){ 240, 240, 245, 255 }, (float)theme->radius.md
    );

    open_container(
        sizing_fixed(250), sizing_fixed(120),
        padding_all(16), 8,
        CLAY_TOP_TO_BOTTOM,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
        theme->success, (float)theme->radius.lg
    );
    add_text("Pure C Works!", theme->font_size.lg, (Clay_Color){ 255, 255, 255, 255 });
    add_text("All components in C", theme->font_size.sm, (Clay_Color){ 220, 255, 220, 255 });
    Clay__CloseElement();

    Clay__CloseElement(); /* Center panel */

    /* Right panel - color swatches */
    open_container(
        sizing_fixed(200), sizing_grow(),
        padding_all(16), 12,
        CLAY_TOP_TO_BOTTOM,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_TOP },
        theme->secondary, (float)theme->radius.md
    );

    add_text("Theme Colors", theme->font_size.lg, theme->fg);

    /* Color swatches row */
    open_container(
        sizing_grow(), sizing_fit(),
        (Clay_Padding){0}, 8,
        CLAY_LEFT_TO_RIGHT,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER },
        (Clay_Color){0}, 0
    );

    /* Primary swatch */
    open_container(sizing_fixed(32), sizing_fixed(32), (Clay_Padding){0}, 0,
        CLAY_LEFT_TO_RIGHT, (Clay_ChildAlignment){0}, theme->primary, 4);
    Clay__CloseElement();

    /* Success swatch */
    open_container(sizing_fixed(32), sizing_fixed(32), (Clay_Padding){0}, 0,
        CLAY_LEFT_TO_RIGHT, (Clay_ChildAlignment){0}, theme->success, 4);
    Clay__CloseElement();

    /* Warning swatch */
    open_container(sizing_fixed(32), sizing_fixed(32), (Clay_Padding){0}, 0,
        CLAY_LEFT_TO_RIGHT, (Clay_ChildAlignment){0}, theme->warning, 4);
    Clay__CloseElement();

    /* Error swatch */
    open_container(sizing_fixed(32), sizing_fixed(32), (Clay_Padding){0}, 0,
        CLAY_LEFT_TO_RIGHT, (Clay_ChildAlignment){0}, theme->error, 4);
    Clay__CloseElement();

    Clay__CloseElement(); /* Swatches row */

    Clay__CloseElement(); /* Right panel */

    Clay__CloseElement(); /* Content area */

    /* Footer */
    open_container(
        sizing_grow(), sizing_fit(),
        padding_all(12), 0,
        CLAY_LEFT_TO_RIGHT,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
        theme->border, (float)theme->radius.sm
    );
    add_text("ClayKit - Zero-allocation UI Components for Clay", theme->font_size.sm, theme->muted);
    Clay__CloseElement(); /* Footer */

    Clay__CloseElement(); /* Root */

    /* Modal overlay (rendered on top) */
    if (show_modal) {
        /* Backdrop */
        Clay_ElementDeclaration backdrop_decl = {0};
        backdrop_decl.id = Clay__HashString(CLAY_STRING("ModalBackdrop"), 0, 0);
        backdrop_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_GROW;
        backdrop_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_GROW;
        backdrop_decl.layout.childAlignment.x = CLAY_ALIGN_X_CENTER;
        backdrop_decl.layout.childAlignment.y = CLAY_ALIGN_Y_CENTER;
        backdrop_decl.backgroundColor = (Clay_Color){ 0, 0, 0, 128 };
        backdrop_decl.floating.attachTo = CLAY_ATTACH_TO_ROOT;
        backdrop_decl.floating.attachPoints.element = CLAY_ATTACH_POINT_LEFT_TOP;
        backdrop_decl.floating.attachPoints.parent = CLAY_ATTACH_POINT_LEFT_TOP;
        backdrop_decl.floating.zIndex = 1000;

        Clay__OpenElement();
        backdrop_hovered = Clay_Hovered();
        Clay__ConfigureOpenElement(backdrop_decl);

        /* Modal content box */
        Clay_ElementDeclaration modal_decl = {0};
        modal_decl.id = Clay__HashString(CLAY_STRING("ModalContent"), 0, 0);
        modal_decl.layout.sizing.width.type = CLAY__SIZING_TYPE_FIXED;
        modal_decl.layout.sizing.width.size.minMax.min = 400;
        modal_decl.layout.sizing.width.size.minMax.max = 400;
        modal_decl.layout.sizing.height.type = CLAY__SIZING_TYPE_FIT;
        modal_decl.layout.padding = padding_all(24);
        modal_decl.layout.childGap = 16;
        modal_decl.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
        modal_decl.backgroundColor = (Clay_Color){ 255, 255, 255, 255 };
        modal_decl.cornerRadius.topLeft = 12;
        modal_decl.cornerRadius.topRight = 12;
        modal_decl.cornerRadius.bottomLeft = 12;
        modal_decl.cornerRadius.bottomRight = 12;

        Clay__OpenElement();
        bool modal_content_hovered = Clay_Hovered();
        Clay__ConfigureOpenElement(modal_decl);

        /* If modal content is hovered, backdrop is not the click target */
        if (modal_content_hovered) {
            backdrop_hovered = false;
        }

        /* Modal title */
        add_text("Modal Title", 24, (Clay_Color){ 50, 50, 50, 255 });

        /* Modal body */
        add_text("This is a modal dialog. Click the backdrop or the close button to dismiss.",
                 16, (Clay_Color){ 100, 100, 100, 255 });

        /* Close button row */
        open_container(
            sizing_grow(), sizing_fit(),
            (Clay_Padding){0}, 0,
            CLAY_LEFT_TO_RIGHT,
            (Clay_ChildAlignment){ CLAY_ALIGN_X_RIGHT, CLAY_ALIGN_Y_CENTER },
            (Clay_Color){0}, 0
        );

        close_modal_btn_hovered = ClayKit_Button(ctx, "Close", 5, (ClayKit_ButtonConfig){0});

        Clay__CloseElement(); /* Close button row */

        Clay__CloseElement(); /* Modal content */
        Clay__CloseElement(); /* Backdrop */
    }
}
