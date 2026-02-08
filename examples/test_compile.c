/*
 * Minimal compilation test for clay_kit.h
 */

#define CLAY_IMPLEMENTATION
#include "../vendor/clay.h"

#define CLAYKIT_IMPLEMENTATION
#include "../clay_kit.h"

#include <stdio.h>

int main(void) {
    /* Test theme access */
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    printf("Theme primary color: R=%d G=%d B=%d\n",
           (int)theme.primary.r, (int)theme.primary.g, (int)theme.primary.b);

    /* Test context initialization */
    ClayKit_State state_buf[64];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 64);
    printf("Context initialized with capacity: %u\n", ctx.state_cap);

    /* Test state management */
    ClayKit_State *s = ClayKit_GetOrCreateState(&ctx, 12345);
    if (s) {
        s->value = 0.75f;
        printf("Created state with id=%u, value=%.2f\n", s->id, s->value);
    }

    /* Test theme helpers */
    uint16_t spacing = ClayKit_GetSpacing(&theme, CLAYKIT_SIZE_MD);
    uint16_t font_size = ClayKit_GetFontSize(&theme, CLAYKIT_SIZE_LG);
    printf("MD spacing: %u, LG font size: %u\n", spacing, font_size);

    /* Test input handling */
    char input_buf[128] = "Hello";
    ClayKit_InputState input = {
        .buf = input_buf,
        .cap = 128,
        .len = 5,
        .cursor = 5,
        .select_start = 5,
        .flags = 0
    };

    ClayKit_InputHandleChar(&input, ' ');
    ClayKit_InputHandleChar(&input, 'W');
    ClayKit_InputHandleChar(&input, 'o');
    ClayKit_InputHandleChar(&input, 'r');
    ClayKit_InputHandleChar(&input, 'l');
    ClayKit_InputHandleChar(&input, 'd');
    input_buf[input.len] = '\0';
    printf("Input text: \"%s\"\n", input_buf);

    /* Test layout primitive configs */
    ClayKit_BoxConfig box_cfg = {
        .bg = theme.bg,
        .border_color = theme.border,
        .border_width = 1,
        .padding = 16,
        .radius = 8
    };
    Clay_LayoutConfig box_layout = ClayKit_BoxLayout(box_cfg);
    printf("Box layout padding: %u\n", box_layout.padding.left);

    ClayKit_FlexConfig flex_cfg = {
        .direction = CLAY_LEFT_TO_RIGHT,
        .gap = 8,
        .padding = 12
    };
    Clay_LayoutConfig flex_layout = ClayKit_FlexLayout(flex_cfg);
    printf("Flex layout gap: %u, direction: %d\n",
           flex_layout.childGap, flex_layout.layoutDirection);

    ClayKit_StackConfig stack_cfg = {
        .direction = CLAYKIT_STACK_VERTICAL,
        .gap = 16
    };
    Clay_LayoutConfig stack_layout = ClayKit_StackLayout(stack_cfg);
    printf("Stack layout direction: %d (expected TOP_TO_BOTTOM=1)\n",
           stack_layout.layoutDirection);

    Clay_Sizing center_sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) };
    Clay_LayoutConfig center_layout = ClayKit_CenterLayout(center_sizing);
    printf("Center alignment: x=%d, y=%d (expected CENTER=2)\n",
           center_layout.childAlignment.x, center_layout.childAlignment.y);

    ClayKit_ContainerConfig container_cfg = { .max_width = 800, .padding = 24 };
    Clay_LayoutConfig container_layout = ClayKit_ContainerLayout(container_cfg);
    printf("Container max-width: %.0f, padding: %u\n",
           container_layout.sizing.width.size.minMax.max,
           container_layout.padding.left);

    Clay_LayoutConfig spacer_layout = ClayKit_SpacerLayout();
    printf("Spacer sizing type: %d (expected GROW=1)\n",
           spacer_layout.sizing.width.type);

    /* Test component style computation functions */
    printf("\n--- Component Style Tests ---\n");

    ClayKit_BadgeStyle badge_style = ClayKit_ComputeBadgeStyle(&ctx, (ClayKit_BadgeConfig){0});
    printf("Badge font size: %u, corner radius: %u\n",
           badge_style.font_size, badge_style.corner_radius);

    ClayKit_ProgressStyle progress_style = ClayKit_ComputeProgressStyle(&ctx, (ClayKit_ProgressConfig){0});
    printf("Progress height: %u, corner radius: %u\n",
           progress_style.height, progress_style.corner_radius);

    ClayKit_SliderStyle slider_style = ClayKit_ComputeSliderStyle(&ctx, (ClayKit_SliderConfig){0}, false);
    printf("Slider track height: %u, thumb size: %u\n",
           slider_style.track_height, slider_style.thumb_size);

    ClayKit_AlertStyle alert_style = ClayKit_ComputeAlertStyle(&ctx, (ClayKit_AlertConfig){0});
    printf("Alert padding: %u, corner radius: %u\n",
           alert_style.padding, alert_style.corner_radius);

    ClayKit_TooltipStyle tooltip_style = ClayKit_ComputeTooltipStyle(&ctx, (ClayKit_TooltipConfig){0});
    printf("Tooltip padding: %u x %u, font size: %u\n",
           tooltip_style.padding_x, tooltip_style.padding_y, tooltip_style.font_size);

    ClayKit_TabsStyle tabs_style = ClayKit_ComputeTabsStyle(&ctx, (ClayKit_TabsConfig){0});
    printf("Tabs padding: %u x %u, indicator height: %u\n",
           tabs_style.padding_x, tabs_style.padding_y, tabs_style.indicator_height);

    ClayKit_ModalStyle modal_style = ClayKit_ComputeModalStyle(&ctx, (ClayKit_ModalConfig){0});
    printf("Modal width: %u, z-index: %u\n",
           modal_style.width, modal_style.z_index);

    ClayKit_InputStyle input_style = ClayKit_ComputeInputStyle(&ctx, (ClayKit_InputConfig){0}, false);
    printf("Input padding: %u x %u, cursor width: %u\n",
           input_style.padding_x, input_style.padding_y, input_style.cursor_width);

    /* Test helper functions */
    uint16_t checkbox_sz = ClayKit_CheckboxSize(&ctx, CLAYKIT_SIZE_MD);
    printf("Checkbox size (MD): %u\n", checkbox_sz);

    uint16_t switch_w = ClayKit_SwitchWidth(&ctx, CLAYKIT_SIZE_MD);
    uint16_t switch_h = ClayKit_SwitchHeight(&ctx, CLAYKIT_SIZE_MD);
    printf("Switch size (MD): %u x %u\n", switch_w, switch_h);

    printf("\nAll tests passed!\n");
    return 0;
}
