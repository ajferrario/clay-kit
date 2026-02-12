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

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 800

/* Demo icon IDs */
#define ICON_INFO    1
#define ICON_SUCCESS 2
#define ICON_WARNING 3
#define ICON_ERROR   4

/* Icon callback - draws simple shapes for each icon */
static void icon_callback(uint16_t icon_id, Clay_BoundingBox box, void *user_data) {
    (void)user_data;
    float cx = box.x + box.width / 2.0f;
    float cy = box.y + box.height / 2.0f;
    float r = box.width / 2.0f - 1.0f;

    switch (icon_id) {
        case ICON_INFO:
            DrawCircle((int)cx, (int)cy, r, (Color){ 66, 133, 244, 255 });
            DrawText("i", (int)(cx - 3), (int)(cy - 6), 12, WHITE);
            break;
        case ICON_SUCCESS:
            DrawCircle((int)cx, (int)cy, r, (Color){ 34, 197, 94, 255 });
            DrawText("v", (int)(cx - 4), (int)(cy - 6), 12, WHITE);
            break;
        case ICON_WARNING:
            DrawCircle((int)cx, (int)cy, r, (Color){ 251, 191, 36, 255 });
            DrawText("!", (int)(cx - 3), (int)(cy - 6), 12, WHITE);
            break;
        case ICON_ERROR:
            DrawCircle((int)cx, (int)cy, r, (Color){ 239, 68, 68, 255 });
            DrawText("x", (int)(cx - 4), (int)(cy - 6), 12, WHITE);
            break;
        default:
            DrawCircle((int)cx, (int)cy, r, (Color){ 150, 150, 150, 255 });
            break;
    }
}

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
static bool show_drawer = false;
static bool show_popover = false;
static int selected_radio = 0;
static int selected_option = -1;  /* -1 = no selection */
static bool select_open = false;
static bool accordion_open[3] = { true, false, false };
static bool menu_open = false;

/* Pending click state */
static bool pending_input_click = false;
static float pending_click_x = 0;

/* Interaction states - set during UI building, used after layout */
static bool input_hovered = false;
static int tab_hovered = -1;  /* -1 = none, 0-2 = tab index */
static bool modal_btn_hovered = false;
static bool close_modal_btn_hovered = false;
static bool backdrop_hovered = false;
static bool drawer_btn_hovered = false;
static bool drawer_backdrop_hovered = false;
static bool close_drawer_btn_hovered = false;
static bool popover_anchor_hovered = false;
static int radio_hovered = -1;   /* -1 = none, 0-2 = radio index */
static bool select_trigger_hovered = false;
static int select_option_hovered = -1;  /* -1 = none */
static int link_hovered = -1;  /* -1 = none, 0-2 = link index */
static int breadcrumb_hovered = -1;
static bool accordion_header_hovered[3] = {false, false, false};
static bool menu_btn_hovered = false;
static int menu_item_hovered = -1;

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
    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
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
    ctx.icon_callback = icon_callback;

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
        drawer_btn_hovered = false;
        drawer_backdrop_hovered = false;
        close_drawer_btn_hovered = false;
        popover_anchor_hovered = false;
        radio_hovered = -1;
        select_trigger_hovered = false;
        select_option_hovered = -1;
        link_hovered = -1;
        breadcrumb_hovered = -1;
        for (int i = 0; i < 3; i++) accordion_header_hovered[i] = false;
        menu_btn_hovered = false;
        menu_item_hovered = -1;

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

            /* Radio selection */
            if (radio_hovered >= 0) {
                selected_radio = radio_hovered;
            }

            /* Select trigger toggle */
            if (select_trigger_hovered) {
                select_open = !select_open;
            } else if (select_option_hovered >= 0) {
                selected_option = select_option_hovered;
                select_open = false;
            } else if (select_open) {
                select_open = false;
            }

            /* Popover toggle on hover */
            /* (handled below outside click block) */

            /* Drawer open */
            if (drawer_btn_hovered) {
                show_drawer = true;
            }

            /* Drawer close (backdrop or close button) */
            if (show_drawer && (drawer_backdrop_hovered || close_drawer_btn_hovered)) {
                show_drawer = false;
            }

            /* Modal open */
            if (modal_btn_hovered) {
                show_modal = true;
            }

            /* Modal close (backdrop or close button) */
            if (show_modal && (backdrop_hovered || close_modal_btn_hovered)) {
                show_modal = false;
            }

            /* Accordion toggle */
            for (int i = 0; i < 3; i++) {
                if (accordion_header_hovered[i]) {
                    accordion_open[i] = !accordion_open[i];
                }
            }

            /* Menu toggle */
            if (menu_btn_hovered) {
                menu_open = !menu_open;
            } else if (menu_item_hovered >= 0) {
                menu_open = false;
            } else if (menu_open) {
                menu_open = false;
            }
        }

        /* Popover shows on hover */
        show_popover = popover_anchor_hovered;

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
                case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {
                    Clay_CustomRenderData *custom = &cmd->renderData.custom;
                    ClayKit_IconRenderData *icon_data = (ClayKit_IconRenderData *)custom->customData;
                    if (icon_data && icon_data->type == CLAYKIT_CUSTOM_ICON && ctx.icon_callback) {
                        ctx.icon_callback(icon_data->icon_id, cmd->boundingBox, ctx.icon_user_data);
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
        padding_all(16), 12,
        CLAY_TOP_TO_BOTTOM,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_TOP },
        theme->bg, 0
    );

    /* Header */
    open_container(
        sizing_grow(), sizing_fit(),
        padding_all(12), 0,
        CLAY_LEFT_TO_RIGHT,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER },
        theme->primary, (float)theme->radius.md
    );
    add_text("ClayKit Demo - Pure C", theme->font_size.xl, (Clay_Color){ 255, 255, 255, 255 });
    Clay__CloseElement(); /* Header */

    /* Content area - 4 columns */
    open_container(
        sizing_grow(), sizing_grow(),
        (Clay_Padding){0}, 12,
        CLAY_LEFT_TO_RIGHT,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_TOP },
        (Clay_Color){0}, 0
    );

    /* ===== Column 1: Form Controls ===== */
    open_container(
        sizing_grow(), sizing_fit(),
        padding_all(12), 8,
        CLAY_TOP_TO_BOTTOM,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_TOP },
        theme->secondary, (float)theme->radius.md
    );

    add_text("Form Controls", theme->font_size.md, theme->fg);

    /* Button */
    add_text("Button:", theme->font_size.sm, theme->muted);
    ClayKit_Button(ctx, "Click Me", 8, (ClayKit_ButtonConfig){ .icon_left = { .id = ICON_SUCCESS, .size = 16 } });

    /* Text Input */
    add_text("Text Input:", theme->font_size.sm, theme->muted);
    input_hovered = ClayKit_TextInput(ctx, "TextInput", 9, &input_state, (ClayKit_InputConfig){0}, "Type here...", 12);

    /* Slider */
    add_text("Slider:", theme->font_size.sm, theme->muted);
    ClayKit_Slider(ctx, 0.5f, (ClayKit_SliderConfig){0});

    /* Radio group */
    add_text("Radio:", theme->font_size.sm, theme->muted);
    {
        static const char *radio_labels[] = { "Option A", "Option B", "Option C" };
        int i;
        for (i = 0; i < 3; i++) {
            open_container(sizing_grow(), sizing_fit(), (Clay_Padding){0}, 8,
                CLAY_LEFT_TO_RIGHT,
                (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER },
                (Clay_Color){0}, 0);
            if (ClayKit_Radio(ctx, selected_radio == i, (ClayKit_RadioConfig){0})) {
                radio_hovered = i;
            }
            add_text(radio_labels[i], theme->font_size.sm, theme->fg);
            Clay__CloseElement();
        }
    }

    /* Select */
    add_text("Select:", theme->font_size.sm, theme->muted);
    {
        static const char *options[] = { "Apple", "Banana", "Cherry" };
        static const int option_lens[] = { 5, 6, 6 };
        const char *display = (selected_option >= 0) ? options[selected_option] : NULL;
        int display_len = (selected_option >= 0) ? option_lens[selected_option] : 0;

        select_trigger_hovered = ClayKit_SelectTrigger(ctx, "Select1", 7,
            display, display_len, (ClayKit_SelectConfig){0});

        if (select_open) {
            ClayKit_SelectDropdownBegin(ctx, "SelectDrop1", 11, (ClayKit_SelectConfig){0});
            int i;
            for (i = 0; i < 3; i++) {
                if (ClayKit_SelectOption(ctx, options[i], option_lens[i],
                        selected_option == i, (ClayKit_SelectConfig){0})) {
                    select_option_hovered = i;
                }
            }
            ClayKit_SelectDropdownEnd();
        }
    }

    Clay__CloseElement(); /* Column 1 */

    /* ===== Column 2: Data Display ===== */
    open_container(
        sizing_grow(), sizing_fit(),
        padding_all(12), 8,
        CLAY_TOP_TO_BOTTOM,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_TOP },
        theme->secondary, (float)theme->radius.md
    );

    add_text("Data Display", theme->font_size.md, theme->fg);

    /* Badge */
    add_text("Badge:", theme->font_size.sm, theme->muted);
    ClayKit_BadgeRaw(ctx, "Badge", 5, (ClayKit_BadgeConfig){0});

    /* Tags */
    add_text("Tags:", theme->font_size.sm, theme->muted);
    open_container(sizing_grow(), sizing_fit(), (Clay_Padding){0}, 6,
        CLAY_LEFT_TO_RIGHT, (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER },
        (Clay_Color){0}, 0);
    ClayKit_TagRaw(ctx, "Default", 7, (ClayKit_TagConfig){0});
    ClayKit_TagRaw(ctx, "Subtle", 6, (ClayKit_TagConfig){ .variant = CLAYKIT_TAG_SUBTLE, .color_scheme = CLAYKIT_COLOR_SUCCESS });
    ClayKit_TagRaw(ctx, "Close", 5, (ClayKit_TagConfig){ .closeable = true, .color_scheme = CLAYKIT_COLOR_ERROR });
    Clay__CloseElement();

    /* Progress */
    add_text("Progress:", theme->font_size.sm, theme->muted);
    ClayKit_Progress(ctx, 0.7f, (ClayKit_ProgressConfig){0});

    /* Spinner */
    add_text("Spinner:", theme->font_size.sm, theme->muted);
    open_container(sizing_grow(), sizing_fit(), (Clay_Padding){0}, 12,
        CLAY_LEFT_TO_RIGHT,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER },
        (Clay_Color){0}, 0);
    ClayKit_Spinner(ctx, (ClayKit_SpinnerConfig){0});
    ClayKit_Spinner(ctx, (ClayKit_SpinnerConfig){ .size = CLAYKIT_SIZE_LG, .color_scheme = CLAYKIT_COLOR_SUCCESS });
    ClayKit_Spinner(ctx, (ClayKit_SpinnerConfig){ .size = CLAYKIT_SIZE_XS, .color_scheme = CLAYKIT_COLOR_ERROR });
    Clay__CloseElement();

    /* Alert */
    add_text("Alerts:", theme->font_size.sm, theme->muted);
    ClayKit_AlertText(ctx, "Info alert message", 18, (ClayKit_AlertConfig){ .icon = { .id = ICON_INFO } });
    ClayKit_AlertText(ctx, "Success!", 8, (ClayKit_AlertConfig){ .color_scheme = CLAYKIT_COLOR_SUCCESS, .icon = { .id = ICON_SUCCESS } });

    /* Tooltip */
    add_text("Tooltip:", theme->font_size.sm, theme->muted);
    ClayKit_Tooltip(ctx, "This is a tooltip", 17, (ClayKit_TooltipConfig){0});

    /* Stats */
    add_text("Stats:", theme->font_size.sm, theme->muted);
    ClayKit_Stat(ctx, "Revenue", 7, "$45,231", 7, "+20.1%", 6, (ClayKit_StatConfig){ .size = CLAYKIT_SIZE_SM });
    ClayKit_Stat(ctx, "Users", 5, "2,350", 5, "+180", 4, (ClayKit_StatConfig){ .size = CLAYKIT_SIZE_SM });

    Clay__CloseElement(); /* Column 2 */

    /* ===== Column 3: Lists & Table ===== */
    open_container(
        sizing_grow(), sizing_fit(),
        padding_all(12), 8,
        CLAY_TOP_TO_BOTTOM,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_TOP },
        (Clay_Color){ 240, 240, 245, 255 }, (float)theme->radius.md
    );

    add_text("Lists & Table", theme->font_size.md, theme->fg);

    /* Unordered list */
    add_text("Unordered:", theme->font_size.sm, theme->muted);
    {
        ClayKit_ListConfig list_cfg = {0};
        ClayKit_ListBegin(ctx, list_cfg);
        ClayKit_ListItemRaw(ctx, "First item", 10, 0, list_cfg);
        ClayKit_ListItemRaw(ctx, "Second item", 11, 1, list_cfg);
        ClayKit_ListItemRaw(ctx, "Third item", 10, 2, list_cfg);
        ClayKit_ListEnd();
    }

    /* Ordered list */
    add_text("Ordered:", theme->font_size.sm, theme->muted);
    {
        ClayKit_ListConfig list_cfg = { .ordered = true };
        ClayKit_ListBegin(ctx, list_cfg);
        ClayKit_ListItemRaw(ctx, "Step one", 8, 0, list_cfg);
        ClayKit_ListItemRaw(ctx, "Step two", 8, 1, list_cfg);
        ClayKit_ListItemRaw(ctx, "Step three", 10, 2, list_cfg);
        ClayKit_ListEnd();
    }

    /* Table */
    add_text("Table:", theme->font_size.sm, theme->muted);
    {
        ClayKit_TableConfig table_cfg = { .striped = true, .bordered = true };

        ClayKit_TableBegin(ctx, table_cfg);

        ClayKit_TableHeaderRow(ctx, table_cfg);
        ClayKit_TableHeaderCell(ctx, 0.33f, table_cfg);
        add_text("Name", theme->font_size.sm, (Clay_Color){ 255, 255, 255, 255 });
        ClayKit_TableCellEnd();
        ClayKit_TableHeaderCell(ctx, 0.33f, table_cfg);
        add_text("Role", theme->font_size.sm, (Clay_Color){ 255, 255, 255, 255 });
        ClayKit_TableCellEnd();
        ClayKit_TableHeaderCell(ctx, 0.34f, table_cfg);
        add_text("Status", theme->font_size.sm, (Clay_Color){ 255, 255, 255, 255 });
        ClayKit_TableCellEnd();
        ClayKit_TableRowEnd();

        ClayKit_TableRow(ctx, 0, table_cfg);
        ClayKit_TableCell(ctx, 0.33f, 0, table_cfg);
        add_text("Alice", theme->font_size.sm, theme->fg);
        ClayKit_TableCellEnd();
        ClayKit_TableCell(ctx, 0.33f, 0, table_cfg);
        add_text("Engineer", theme->font_size.sm, theme->fg);
        ClayKit_TableCellEnd();
        ClayKit_TableCell(ctx, 0.34f, 0, table_cfg);
        add_text("Active", theme->font_size.sm, theme->fg);
        ClayKit_TableCellEnd();
        ClayKit_TableRowEnd();

        ClayKit_TableRow(ctx, 1, table_cfg);
        ClayKit_TableCell(ctx, 0.33f, 1, table_cfg);
        add_text("Bob", theme->font_size.sm, theme->fg);
        ClayKit_TableCellEnd();
        ClayKit_TableCell(ctx, 0.33f, 1, table_cfg);
        add_text("Designer", theme->font_size.sm, theme->fg);
        ClayKit_TableCellEnd();
        ClayKit_TableCell(ctx, 0.34f, 1, table_cfg);
        add_text("Away", theme->font_size.sm, theme->fg);
        ClayKit_TableCellEnd();
        ClayKit_TableRowEnd();

        ClayKit_TableEnd();
    }

    Clay__CloseElement(); /* Column 3 */

    /* ===== Column 4: Navigation & Overlays ===== */
    open_container(
        sizing_grow(), sizing_fit(),
        padding_all(12), 8,
        CLAY_TOP_TO_BOTTOM,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_TOP },
        theme->secondary, (float)theme->radius.md
    );

    add_text("Navigation", theme->font_size.md, theme->fg);

    /* Tabs - line variant */
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

    /* Tabs - enclosed variant */
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

    /* Links */
    add_text("Links:", theme->font_size.sm, theme->muted);
    open_container(
        sizing_grow(), sizing_fit(),
        (Clay_Padding){0}, 8,
        CLAY_LEFT_TO_RIGHT,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER },
        (Clay_Color){0}, 0
    );
    link_hovered = -1;
    if (ClayKit_Link(ctx, "Default", 7, (ClayKit_LinkConfig){0})) link_hovered = 0;
    if (ClayKit_Link(ctx, "Hover", 5, (ClayKit_LinkConfig){ .variant = CLAYKIT_LINK_HOVER_UNDERLINE })) link_hovered = 1;
    if (ClayKit_Link(ctx, "Disabled", 8, (ClayKit_LinkConfig){ .disabled = true })) link_hovered = 2;
    Clay__CloseElement();

    /* Breadcrumb */
    add_text("Breadcrumb:", theme->font_size.sm, theme->muted);
    {
        ClayKit_BreadcrumbConfig bc_cfg = {0};
        breadcrumb_hovered = -1;
        ClayKit_BreadcrumbBegin(ctx, bc_cfg);
        if (ClayKit_BreadcrumbItem(ctx, "Home", 4, false, bc_cfg)) breadcrumb_hovered = 0;
        ClayKit_BreadcrumbSeparator(ctx, bc_cfg);
        if (ClayKit_BreadcrumbItem(ctx, "Products", 8, false, bc_cfg)) breadcrumb_hovered = 1;
        ClayKit_BreadcrumbSeparator(ctx, bc_cfg);
        ClayKit_BreadcrumbItem(ctx, "Widget", 6, true, bc_cfg);
        ClayKit_BreadcrumbEnd();
    }

    /* Accordion */
    add_text("Accordion:", theme->font_size.sm, theme->muted);
    {
        ClayKit_AccordionConfig acc_cfg = {0};
        ClayKit_AccordionBegin(ctx, acc_cfg);

        ClayKit_AccordionItemBegin(ctx, accordion_open[0], acc_cfg);
        accordion_header_hovered[0] = ClayKit_AccordionHeader(ctx, "Section 1", 9, accordion_open[0], acc_cfg);
        if (accordion_open[0]) {
            ClayKit_AccordionContentBegin(ctx, acc_cfg);
            add_text("Content for section 1. This is expanded by default.", theme->font_size.sm, theme->fg);
            ClayKit_AccordionContentEnd();
        }
        ClayKit_AccordionItemEnd();

        ClayKit_AccordionItemBegin(ctx, accordion_open[1], acc_cfg);
        accordion_header_hovered[1] = ClayKit_AccordionHeader(ctx, "Section 2", 9, accordion_open[1], acc_cfg);
        if (accordion_open[1]) {
            ClayKit_AccordionContentBegin(ctx, acc_cfg);
            add_text("Content for section 2.", theme->font_size.sm, theme->fg);
            ClayKit_AccordionContentEnd();
        }
        ClayKit_AccordionItemEnd();

        ClayKit_AccordionItemBegin(ctx, accordion_open[2], acc_cfg);
        accordion_header_hovered[2] = ClayKit_AccordionHeader(ctx, "Section 3", 9, accordion_open[2], acc_cfg);
        if (accordion_open[2]) {
            ClayKit_AccordionContentBegin(ctx, acc_cfg);
            add_text("Content for section 3.", theme->font_size.sm, theme->fg);
            ClayKit_AccordionContentEnd();
        }
        ClayKit_AccordionItemEnd();

        ClayKit_AccordionEnd();
    }

    /* Menu */
    add_text("Menu:", theme->font_size.sm, theme->muted);
    open_container(sizing_fit(), sizing_fit(), (Clay_Padding){0}, 0,
        CLAY_TOP_TO_BOTTOM, (Clay_ChildAlignment){0}, (Clay_Color){0}, 0);
    menu_btn_hovered = ClayKit_Button(ctx, "Actions", 7, (ClayKit_ButtonConfig){0});
    if (menu_open) {
        ClayKit_MenuConfig menu_cfg = {0};
        menu_item_hovered = -1;
        ClayKit_MenuDropdownBegin(ctx, "Menu1", 5, menu_cfg);
        if (ClayKit_MenuItem(ctx, "Edit", 4, false, menu_cfg)) menu_item_hovered = 0;
        if (ClayKit_MenuItem(ctx, "Duplicate", 9, false, menu_cfg)) menu_item_hovered = 1;
        ClayKit_MenuSeparator(ctx, menu_cfg);
        if (ClayKit_MenuItem(ctx, "Delete", 6, true, menu_cfg)) menu_item_hovered = 2;
        ClayKit_MenuDropdownEnd();
    }
    Clay__CloseElement(); /* Menu wrapper */

    /* Popover - wrapped so popover attaches to button's parent, not column */
    add_text("Popover:", theme->font_size.sm, theme->muted);
    open_container(sizing_fit(), sizing_fit(), (Clay_Padding){0}, 0,
        CLAY_TOP_TO_BOTTOM, (Clay_ChildAlignment){0}, (Clay_Color){0}, 0);
    popover_anchor_hovered = ClayKit_Button(ctx, "Hover me", 8, (ClayKit_ButtonConfig){0});
    if (show_popover) {
        ClayKit_PopoverBegin(ctx, "Popover1", 8, (ClayKit_PopoverConfig){0});
        add_text("Popover content!", theme->font_size.sm, theme->fg);
        ClayKit_PopoverEnd();
    }
    Clay__CloseElement(); /* Popover wrapper */

    /* Drawer button */
    add_text("Drawer:", theme->font_size.sm, theme->muted);
    drawer_btn_hovered = ClayKit_Button(ctx, "Open Drawer", 11, (ClayKit_ButtonConfig){0});

    /* Modal button */
    add_text("Modal:", theme->font_size.sm, theme->muted);
    modal_btn_hovered = ClayKit_Button(ctx, "Open Modal", 10, (ClayKit_ButtonConfig){0});

    /* Theme Colors */
    add_text("Theme:", theme->font_size.sm, theme->muted);
    open_container(
        sizing_grow(), sizing_fit(),
        (Clay_Padding){0}, 8,
        CLAY_LEFT_TO_RIGHT,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER },
        (Clay_Color){0}, 0
    );
    open_container(sizing_fixed(24), sizing_fixed(24), (Clay_Padding){0}, 0,
        CLAY_LEFT_TO_RIGHT, (Clay_ChildAlignment){0}, theme->primary, 4);
    Clay__CloseElement();
    open_container(sizing_fixed(24), sizing_fixed(24), (Clay_Padding){0}, 0,
        CLAY_LEFT_TO_RIGHT, (Clay_ChildAlignment){0}, theme->success, 4);
    Clay__CloseElement();
    open_container(sizing_fixed(24), sizing_fixed(24), (Clay_Padding){0}, 0,
        CLAY_LEFT_TO_RIGHT, (Clay_ChildAlignment){0}, theme->warning, 4);
    Clay__CloseElement();
    open_container(sizing_fixed(24), sizing_fixed(24), (Clay_Padding){0}, 0,
        CLAY_LEFT_TO_RIGHT, (Clay_ChildAlignment){0}, theme->error, 4);
    Clay__CloseElement();
    Clay__CloseElement(); /* Swatches row */

    Clay__CloseElement(); /* Column 4 */

    Clay__CloseElement(); /* Content area */

    /* Footer */
    open_container(
        sizing_grow(), sizing_fit(),
        padding_all(8), 0,
        CLAY_LEFT_TO_RIGHT,
        (Clay_ChildAlignment){ CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
        theme->border, (float)theme->radius.sm
    );
    add_text("ClayKit - Zero-allocation UI Components for Clay", theme->font_size.sm, theme->muted);
    Clay__CloseElement(); /* Footer */

    Clay__CloseElement(); /* Root */

    /* Drawer overlay */
    if (show_drawer) {
        drawer_backdrop_hovered = ClayKit_DrawerBegin(ctx, "Drawer1", 7,
            (ClayKit_DrawerConfig){ .side = CLAYKIT_DRAWER_RIGHT });

        add_text("Drawer Content", 20, (Clay_Color){ 50, 50, 50, 255 });
        add_text("This is a drawer panel that slides in from the right side.", 14, (Clay_Color){ 100, 100, 100, 255 });

        close_drawer_btn_hovered = ClayKit_Button(ctx, "Close Drawer", 12, (ClayKit_ButtonConfig){0});

        ClayKit_DrawerEnd();
    }

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
