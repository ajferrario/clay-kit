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

    printf("\nAll tests passed!\n");
    return 0;
}
