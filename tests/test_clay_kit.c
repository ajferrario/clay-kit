/*
 * Unit tests for clay_kit.h
 *
 * Compile: gcc -std=c99 -Wall -Wextra -I. -Ivendor tests/test_clay_kit.c -o tests/test_clay_kit -lm
 * Run: ./tests/test_clay_kit
 */

#define CLAY_IMPLEMENTATION
#include "../vendor/clay.h"

#define CLAYKIT_IMPLEMENTATION
#include "../clay_kit.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

/* ============================================================================
 * Minimal Test Framework
 * ============================================================================ */

static int g_tests_run = 0;
static int g_tests_passed = 0;
static int g_tests_failed = 0;
static const char *g_current_test = NULL;

#define TEST(name) \
    static void test_##name(void); \
    static void run_test_##name(void) { \
        g_current_test = #name; \
        g_tests_run++; \
        test_##name(); \
    } \
    static void test_##name(void)

#define RUN_TEST(name) run_test_##name()

#define ASSERT(cond) do { \
    if (!(cond)) { \
        printf("  FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        g_tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_EQ(a, b) do { \
    if ((a) != (b)) { \
        printf("  FAIL: %s:%d: %s == %s (got %d, expected %d)\n", \
               __FILE__, __LINE__, #a, #b, (int)(a), (int)(b)); \
        g_tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_EQ_FLOAT(a, b, eps) do { \
    if (fabs((a) - (b)) > (eps)) { \
        printf("  FAIL: %s:%d: %s == %s (got %f, expected %f)\n", \
               __FILE__, __LINE__, #a, #b, (double)(a), (double)(b)); \
        g_tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_STR_EQ(a, b) do { \
    if (strcmp((a), (b)) != 0) { \
        printf("  FAIL: %s:%d: %s == %s (got \"%s\", expected \"%s\")\n", \
               __FILE__, __LINE__, #a, #b, (a), (b)); \
        g_tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_NOT_NULL(ptr) do { \
    if ((ptr) == NULL) { \
        printf("  FAIL: %s:%d: %s != NULL\n", __FILE__, __LINE__, #ptr); \
        g_tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_NULL(ptr) do { \
    if ((ptr) != NULL) { \
        printf("  FAIL: %s:%d: %s == NULL\n", __FILE__, __LINE__, #ptr); \
        g_tests_failed++; \
        return; \
    } \
} while(0)

#define TEST_PASS() do { \
    g_tests_passed++; \
    printf("  PASS: %s\n", g_current_test); \
} while(0)

/* ============================================================================
 * State Management Tests
 * ============================================================================ */

TEST(init_context) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[16];
    ClayKit_Context ctx;

    ClayKit_Init(&ctx, &theme, state_buf, 16);

    ASSERT_EQ(ctx.state_cap, 16);
    ASSERT_EQ(ctx.state_count, 0);
    ASSERT_EQ(ctx.focused_id, 0);
    ASSERT_EQ(ctx.prev_focused_id, 0);
    ASSERT(ctx.theme_ptr == &theme);
    ASSERT(ctx.state_ptr == state_buf);
    ASSERT_NULL(ctx.icon_callback);

    TEST_PASS();
}

TEST(init_zeroes_state_buffer) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;

    /* Pre-fill with garbage */
    for (int i = 0; i < 4; i++) {
        state_buf[i].id = 0xDEADBEEF;
        state_buf[i].flags = 0xCAFE;
        state_buf[i].value = 123.456f;
    }

    ClayKit_Init(&ctx, &theme, state_buf, 4);

    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(state_buf[i].id, 0);
        ASSERT_EQ(state_buf[i].flags, 0);
        ASSERT_EQ_FLOAT(state_buf[i].value, 0.0f, 0.001f);
    }

    TEST_PASS();
}

TEST(get_state_empty) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[8];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 8);

    ClayKit_State *s = ClayKit_GetState(&ctx, 12345);
    ASSERT_NULL(s);

    TEST_PASS();
}

TEST(get_or_create_state_new) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[8];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 8);

    ClayKit_State *s = ClayKit_GetOrCreateState(&ctx, 12345);
    ASSERT_NOT_NULL(s);
    ASSERT_EQ(s->id, 12345);
    ASSERT_EQ(s->flags, 0);
    ASSERT_EQ_FLOAT(s->value, 0.0f, 0.001f);
    ASSERT_EQ(ctx.state_count, 1);

    TEST_PASS();
}

TEST(get_or_create_state_existing) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[8];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 8);

    ClayKit_State *s1 = ClayKit_GetOrCreateState(&ctx, 100);
    s1->value = 0.5f;
    s1->flags = 42;

    ClayKit_State *s2 = ClayKit_GetOrCreateState(&ctx, 100);
    ASSERT(s1 == s2);
    ASSERT_EQ_FLOAT(s2->value, 0.5f, 0.001f);
    ASSERT_EQ(s2->flags, 42);
    ASSERT_EQ(ctx.state_count, 1); /* Should not create new */

    TEST_PASS();
}

TEST(get_state_after_create) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[8];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 8);

    ClayKit_State *s1 = ClayKit_GetOrCreateState(&ctx, 999);
    s1->value = 3.14f;

    ClayKit_State *s2 = ClayKit_GetState(&ctx, 999);
    ASSERT(s1 == s2);
    ASSERT_EQ_FLOAT(s2->value, 3.14f, 0.001f);

    TEST_PASS();
}

TEST(state_capacity_limit) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[3];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 3);

    ASSERT_NOT_NULL(ClayKit_GetOrCreateState(&ctx, 1));
    ASSERT_NOT_NULL(ClayKit_GetOrCreateState(&ctx, 2));
    ASSERT_NOT_NULL(ClayKit_GetOrCreateState(&ctx, 3));
    ASSERT_EQ(ctx.state_count, 3);

    /* Should fail - at capacity */
    ClayKit_State *overflow = ClayKit_GetOrCreateState(&ctx, 4);
    ASSERT_NULL(overflow);
    ASSERT_EQ(ctx.state_count, 3);

    TEST_PASS();
}

TEST(multiple_states) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[8];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 8);

    ClayKit_State *a = ClayKit_GetOrCreateState(&ctx, 100);
    ClayKit_State *b = ClayKit_GetOrCreateState(&ctx, 200);
    ClayKit_State *c = ClayKit_GetOrCreateState(&ctx, 300);

    a->value = 1.0f;
    b->value = 2.0f;
    c->value = 3.0f;

    ASSERT_EQ_FLOAT(ClayKit_GetState(&ctx, 100)->value, 1.0f, 0.001f);
    ASSERT_EQ_FLOAT(ClayKit_GetState(&ctx, 200)->value, 2.0f, 0.001f);
    ASSERT_EQ_FLOAT(ClayKit_GetState(&ctx, 300)->value, 3.0f, 0.001f);
    ASSERT_NULL(ClayKit_GetState(&ctx, 400));

    TEST_PASS();
}

/* ============================================================================
 * Focus Management Tests
 * ============================================================================ */

TEST(focus_initial_state) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ASSERT_EQ(ctx.focused_id, 0);
    ASSERT_EQ(ctx.prev_focused_id, 0);

    TEST_PASS();
}

TEST(set_and_check_focus) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    Clay_ElementId elem = { .id = 12345 };
    ClayKit_SetFocus(&ctx, elem);

    ASSERT(ClayKit_HasFocus(&ctx, elem));
    ASSERT_EQ(ctx.focused_id, 12345);

    TEST_PASS();
}

TEST(clear_focus) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    Clay_ElementId elem = { .id = 12345 };
    ClayKit_SetFocus(&ctx, elem);
    ASSERT(ClayKit_HasFocus(&ctx, elem));

    ClayKit_ClearFocus(&ctx);
    ASSERT(!ClayKit_HasFocus(&ctx, elem));
    ASSERT_EQ(ctx.focused_id, 0);

    TEST_PASS();
}

TEST(focus_changed_detection) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    Clay_ElementId elem1 = { .id = 100 };
    Clay_ElementId elem2 = { .id = 200 };

    /* Initial frame */
    ClayKit_BeginFrame(&ctx);
    ASSERT(!ClayKit_FocusChanged(&ctx)); /* No change yet */

    ClayKit_SetFocus(&ctx, elem1);
    ASSERT(ClayKit_FocusChanged(&ctx)); /* Changed within frame */

    /* Next frame */
    ClayKit_BeginFrame(&ctx);
    ASSERT(!ClayKit_FocusChanged(&ctx)); /* prev == current */

    ClayKit_SetFocus(&ctx, elem2);
    ASSERT(ClayKit_FocusChanged(&ctx)); /* Changed again */

    TEST_PASS();
}

TEST(has_focus_different_elements) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    Clay_ElementId elem1 = { .id = 100 };
    Clay_ElementId elem2 = { .id = 200 };

    ClayKit_SetFocus(&ctx, elem1);
    ASSERT(ClayKit_HasFocus(&ctx, elem1));
    ASSERT(!ClayKit_HasFocus(&ctx, elem2));

    ClayKit_SetFocus(&ctx, elem2);
    ASSERT(!ClayKit_HasFocus(&ctx, elem1));
    ASSERT(ClayKit_HasFocus(&ctx, elem2));

    TEST_PASS();
}

/* ============================================================================
 * Theme Helper Tests
 * ============================================================================ */

TEST(get_scheme_color_all) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;

    Clay_Color c;

    c = ClayKit_GetSchemeColor(&theme, CLAYKIT_COLOR_PRIMARY);
    ASSERT_EQ(c.r, theme.primary.r);
    ASSERT_EQ(c.g, theme.primary.g);

    c = ClayKit_GetSchemeColor(&theme, CLAYKIT_COLOR_SECONDARY);
    ASSERT_EQ(c.r, theme.secondary.r);

    c = ClayKit_GetSchemeColor(&theme, CLAYKIT_COLOR_SUCCESS);
    ASSERT_EQ(c.r, theme.success.r);

    c = ClayKit_GetSchemeColor(&theme, CLAYKIT_COLOR_WARNING);
    ASSERT_EQ(c.r, theme.warning.r);

    c = ClayKit_GetSchemeColor(&theme, CLAYKIT_COLOR_ERROR);
    ASSERT_EQ(c.r, theme.error.r);

    TEST_PASS();
}

TEST(get_scheme_color_invalid_defaults_to_primary) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;

    Clay_Color c = ClayKit_GetSchemeColor(&theme, (ClayKit_ColorScheme)99);
    ASSERT_EQ(c.r, theme.primary.r);
    ASSERT_EQ(c.g, theme.primary.g);
    ASSERT_EQ(c.b, theme.primary.b);

    TEST_PASS();
}

TEST(get_spacing_all_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;

    ASSERT_EQ(ClayKit_GetSpacing(&theme, CLAYKIT_SIZE_XS), theme.spacing.xs);
    ASSERT_EQ(ClayKit_GetSpacing(&theme, CLAYKIT_SIZE_SM), theme.spacing.sm);
    ASSERT_EQ(ClayKit_GetSpacing(&theme, CLAYKIT_SIZE_MD), theme.spacing.md);
    ASSERT_EQ(ClayKit_GetSpacing(&theme, CLAYKIT_SIZE_LG), theme.spacing.lg);
    ASSERT_EQ(ClayKit_GetSpacing(&theme, CLAYKIT_SIZE_XL), theme.spacing.xl);

    TEST_PASS();
}

TEST(get_spacing_invalid_defaults_to_md) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;

    uint16_t spacing = ClayKit_GetSpacing(&theme, (ClayKit_Size)99);
    ASSERT_EQ(spacing, theme.spacing.md);

    TEST_PASS();
}

TEST(get_font_size_all_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;

    ASSERT_EQ(ClayKit_GetFontSize(&theme, CLAYKIT_SIZE_XS), theme.font_size.xs);
    ASSERT_EQ(ClayKit_GetFontSize(&theme, CLAYKIT_SIZE_SM), theme.font_size.sm);
    ASSERT_EQ(ClayKit_GetFontSize(&theme, CLAYKIT_SIZE_MD), theme.font_size.md);
    ASSERT_EQ(ClayKit_GetFontSize(&theme, CLAYKIT_SIZE_LG), theme.font_size.lg);
    ASSERT_EQ(ClayKit_GetFontSize(&theme, CLAYKIT_SIZE_XL), theme.font_size.xl);

    TEST_PASS();
}

TEST(get_radius_all_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;

    /* XS and SM both return radius.sm */
    ASSERT_EQ(ClayKit_GetRadius(&theme, CLAYKIT_SIZE_XS), theme.radius.sm);
    ASSERT_EQ(ClayKit_GetRadius(&theme, CLAYKIT_SIZE_SM), theme.radius.sm);
    ASSERT_EQ(ClayKit_GetRadius(&theme, CLAYKIT_SIZE_MD), theme.radius.md);
    /* LG and XL both return radius.lg */
    ASSERT_EQ(ClayKit_GetRadius(&theme, CLAYKIT_SIZE_LG), theme.radius.lg);
    ASSERT_EQ(ClayKit_GetRadius(&theme, CLAYKIT_SIZE_XL), theme.radius.lg);

    TEST_PASS();
}

TEST(theme_light_values) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;

    /* Verify light theme has expected values */
    ASSERT_EQ(theme.bg.r, 255);
    ASSERT_EQ(theme.bg.g, 255);
    ASSERT_EQ(theme.bg.b, 255);
    ASSERT_EQ(theme.spacing.md, 16);
    ASSERT_EQ(theme.radius.md, 8);
    ASSERT_EQ(theme.font_size.md, 16);

    TEST_PASS();
}

TEST(theme_dark_values) {
    ClayKit_Theme theme = CLAYKIT_THEME_DARK;

    /* Verify dark theme has different bg */
    ASSERT_EQ(theme.bg.r, 17);
    ASSERT_EQ(theme.bg.g, 24);
    ASSERT_EQ(theme.bg.b, 39);
    /* But same spacing */
    ASSERT_EQ(theme.spacing.md, 16);

    TEST_PASS();
}

/* ============================================================================
 * Text Input Tests
 * ============================================================================ */

TEST(input_handle_char_basic) {
    char buf[32] = "";
    ClayKit_InputState input = {
        .buf = buf, .cap = 32, .len = 0,
        .cursor = 0, .select_start = 0, .flags = 0
    };

    ASSERT(ClayKit_InputHandleChar(&input, 'H'));
    ASSERT(ClayKit_InputHandleChar(&input, 'i'));
    buf[input.len] = '\0';

    ASSERT_STR_EQ(buf, "Hi");
    ASSERT_EQ(input.len, 2);
    ASSERT_EQ(input.cursor, 2);

    TEST_PASS();
}

TEST(input_handle_char_insert_middle) {
    char buf[32] = "Hllo";
    ClayKit_InputState input = {
        .buf = buf, .cap = 32, .len = 4,
        .cursor = 1, .select_start = 1, .flags = 0
    };

    ASSERT(ClayKit_InputHandleChar(&input, 'e'));
    buf[input.len] = '\0';

    ASSERT_STR_EQ(buf, "Hello");
    ASSERT_EQ(input.len, 5);
    ASSERT_EQ(input.cursor, 2);

    TEST_PASS();
}

TEST(input_handle_char_capacity_limit) {
    char buf[5] = "1234";
    ClayKit_InputState input = {
        .buf = buf, .cap = 5, .len = 4,
        .cursor = 4, .select_start = 4, .flags = 0
    };

    /* Buffer is full (need 1 byte for potential null) */
    ASSERT(!ClayKit_InputHandleChar(&input, '5'));
    ASSERT_EQ(input.len, 4);

    TEST_PASS();
}

TEST(input_handle_char_rejects_control) {
    char buf[32] = "";
    ClayKit_InputState input = {
        .buf = buf, .cap = 32, .len = 0,
        .cursor = 0, .select_start = 0, .flags = 0
    };

    ASSERT(!ClayKit_InputHandleChar(&input, '\n'));
    ASSERT(!ClayKit_InputHandleChar(&input, '\t'));
    ASSERT(!ClayKit_InputHandleChar(&input, 0x7F)); /* DEL */
    ASSERT_EQ(input.len, 0);

    TEST_PASS();
}

TEST(input_backspace_delete_char) {
    char buf[32] = "Hello";
    ClayKit_InputState input = {
        .buf = buf, .cap = 32, .len = 5,
        .cursor = 5, .select_start = 5, .flags = 0
    };

    ASSERT(ClayKit_InputHandleKey(&input, CLAYKIT_KEY_BACKSPACE, 0));
    buf[input.len] = '\0';

    ASSERT_STR_EQ(buf, "Hell");
    ASSERT_EQ(input.cursor, 4);

    TEST_PASS();
}

TEST(input_backspace_at_start) {
    char buf[32] = "Hello";
    ClayKit_InputState input = {
        .buf = buf, .cap = 32, .len = 5,
        .cursor = 0, .select_start = 0, .flags = 0
    };

    ASSERT(!ClayKit_InputHandleKey(&input, CLAYKIT_KEY_BACKSPACE, 0));
    buf[input.len] = '\0';

    ASSERT_STR_EQ(buf, "Hello");
    ASSERT_EQ(input.len, 5);

    TEST_PASS();
}

TEST(input_delete_char) {
    char buf[32] = "Hello";
    ClayKit_InputState input = {
        .buf = buf, .cap = 32, .len = 5,
        .cursor = 0, .select_start = 0, .flags = 0
    };

    ASSERT(ClayKit_InputHandleKey(&input, CLAYKIT_KEY_DELETE, 0));
    buf[input.len] = '\0';

    ASSERT_STR_EQ(buf, "ello");
    ASSERT_EQ(input.cursor, 0);

    TEST_PASS();
}

TEST(input_delete_at_end) {
    char buf[32] = "Hello";
    ClayKit_InputState input = {
        .buf = buf, .cap = 32, .len = 5,
        .cursor = 5, .select_start = 5, .flags = 0
    };

    ASSERT(!ClayKit_InputHandleKey(&input, CLAYKIT_KEY_DELETE, 0));
    ASSERT_EQ(input.len, 5);

    TEST_PASS();
}

TEST(input_arrow_left) {
    char buf[32] = "Hello";
    ClayKit_InputState input = {
        .buf = buf, .cap = 32, .len = 5,
        .cursor = 3, .select_start = 3, .flags = 0
    };

    ASSERT(ClayKit_InputHandleKey(&input, CLAYKIT_KEY_LEFT, 0));
    ASSERT_EQ(input.cursor, 2);
    ASSERT_EQ(input.select_start, 2); /* No selection */

    TEST_PASS();
}

TEST(input_arrow_right) {
    char buf[32] = "Hello";
    ClayKit_InputState input = {
        .buf = buf, .cap = 32, .len = 5,
        .cursor = 2, .select_start = 2, .flags = 0
    };

    ASSERT(ClayKit_InputHandleKey(&input, CLAYKIT_KEY_RIGHT, 0));
    ASSERT_EQ(input.cursor, 3);
    ASSERT_EQ(input.select_start, 3);

    TEST_PASS();
}

TEST(input_home_end) {
    char buf[32] = "Hello";
    ClayKit_InputState input = {
        .buf = buf, .cap = 32, .len = 5,
        .cursor = 2, .select_start = 2, .flags = 0
    };

    ASSERT(ClayKit_InputHandleKey(&input, CLAYKIT_KEY_HOME, 0));
    ASSERT_EQ(input.cursor, 0);

    ASSERT(ClayKit_InputHandleKey(&input, CLAYKIT_KEY_END, 0));
    ASSERT_EQ(input.cursor, 5);

    TEST_PASS();
}

TEST(input_shift_select) {
    char buf[32] = "Hello";
    ClayKit_InputState input = {
        .buf = buf, .cap = 32, .len = 5,
        .cursor = 2, .select_start = 2, .flags = 0
    };

    /* Shift+Right should extend selection */
    ASSERT(ClayKit_InputHandleKey(&input, CLAYKIT_KEY_RIGHT, CLAYKIT_MOD_SHIFT));
    ASSERT_EQ(input.cursor, 3);
    ASSERT_EQ(input.select_start, 2); /* Selection start unchanged */

    ASSERT(ClayKit_InputHandleKey(&input, CLAYKIT_KEY_RIGHT, CLAYKIT_MOD_SHIFT));
    ASSERT_EQ(input.cursor, 4);
    ASSERT_EQ(input.select_start, 2);

    TEST_PASS();
}

TEST(input_delete_selection) {
    char buf[32] = "Hello World";
    ClayKit_InputState input = {
        .buf = buf, .cap = 32, .len = 11,
        .cursor = 6, .select_start = 0, .flags = 0 /* "Hello " selected */
    };

    ASSERT(ClayKit_InputHandleKey(&input, CLAYKIT_KEY_BACKSPACE, 0));
    buf[input.len] = '\0';

    ASSERT_STR_EQ(buf, "World");
    ASSERT_EQ(input.cursor, 0);
    ASSERT_EQ(input.select_start, 0);

    TEST_PASS();
}

TEST(input_type_replaces_selection) {
    char buf[32] = "Hello World";
    ClayKit_InputState input = {
        .buf = buf, .cap = 32, .len = 11,
        .cursor = 11, .select_start = 6, .flags = 0 /* "World" selected */
    };

    ASSERT(ClayKit_InputHandleChar(&input, 'X'));
    buf[input.len] = '\0';

    ASSERT_STR_EQ(buf, "Hello X");
    ASSERT_EQ(input.len, 7);

    TEST_PASS();
}

TEST(input_ctrl_left_word) {
    char buf[32] = "Hello World Test";
    ClayKit_InputState input = {
        .buf = buf, .cap = 32, .len = 16,
        .cursor = 16, .select_start = 16, .flags = 0
    };

    ASSERT(ClayKit_InputHandleKey(&input, CLAYKIT_KEY_LEFT, CLAYKIT_MOD_CTRL));
    ASSERT_EQ(input.cursor, 12); /* Before "Test" */

    ASSERT(ClayKit_InputHandleKey(&input, CLAYKIT_KEY_LEFT, CLAYKIT_MOD_CTRL));
    ASSERT_EQ(input.cursor, 6); /* Before "World" */

    TEST_PASS();
}

TEST(input_ctrl_right_word) {
    char buf[32] = "Hello World Test";
    ClayKit_InputState input = {
        .buf = buf, .cap = 32, .len = 16,
        .cursor = 0, .select_start = 0, .flags = 0
    };

    ASSERT(ClayKit_InputHandleKey(&input, CLAYKIT_KEY_RIGHT, CLAYKIT_MOD_CTRL));
    ASSERT_EQ(input.cursor, 6); /* After "Hello " */

    ASSERT(ClayKit_InputHandleKey(&input, CLAYKIT_KEY_RIGHT, CLAYKIT_MOD_CTRL));
    ASSERT_EQ(input.cursor, 12); /* After "World " */

    TEST_PASS();
}

/* ============================================================================
 * Layout Primitive Tests
 * ============================================================================ */

TEST(box_layout_padding) {
    ClayKit_BoxConfig cfg = { .padding = 16 };
    Clay_LayoutConfig layout = ClayKit_BoxLayout(cfg);

    ASSERT_EQ(layout.padding.left, 16);
    ASSERT_EQ(layout.padding.right, 16);
    ASSERT_EQ(layout.padding.top, 16);
    ASSERT_EQ(layout.padding.bottom, 16);

    TEST_PASS();
}

TEST(box_layout_defaults) {
    ClayKit_BoxConfig cfg = { 0 };
    Clay_LayoutConfig layout = ClayKit_BoxLayout(cfg);

    ASSERT_EQ(layout.childGap, 0);
    ASSERT_EQ(layout.childAlignment.x, CLAY_ALIGN_X_LEFT);
    ASSERT_EQ(layout.childAlignment.y, CLAY_ALIGN_Y_TOP);
    ASSERT_EQ(layout.layoutDirection, CLAY_LEFT_TO_RIGHT);

    TEST_PASS();
}

TEST(flex_layout_direction_and_gap) {
    ClayKit_FlexConfig cfg = {
        .direction = CLAY_TOP_TO_BOTTOM,
        .gap = 12
    };
    Clay_LayoutConfig layout = ClayKit_FlexLayout(cfg);

    ASSERT_EQ(layout.layoutDirection, CLAY_TOP_TO_BOTTOM);
    ASSERT_EQ(layout.childGap, 12);

    TEST_PASS();
}

TEST(flex_layout_alignment) {
    ClayKit_FlexConfig cfg = {
        .align = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_BOTTOM }
    };
    Clay_LayoutConfig layout = ClayKit_FlexLayout(cfg);

    ASSERT_EQ(layout.childAlignment.x, CLAY_ALIGN_X_CENTER);
    ASSERT_EQ(layout.childAlignment.y, CLAY_ALIGN_Y_BOTTOM);

    TEST_PASS();
}

TEST(stack_layout_vertical) {
    ClayKit_StackConfig cfg = {
        .direction = CLAYKIT_STACK_VERTICAL,
        .gap = 8
    };
    Clay_LayoutConfig layout = ClayKit_StackLayout(cfg);

    ASSERT_EQ(layout.layoutDirection, CLAY_TOP_TO_BOTTOM);
    ASSERT_EQ(layout.childGap, 8);

    TEST_PASS();
}

TEST(stack_layout_horizontal) {
    ClayKit_StackConfig cfg = {
        .direction = CLAYKIT_STACK_HORIZONTAL,
        .gap = 16
    };
    Clay_LayoutConfig layout = ClayKit_StackLayout(cfg);

    ASSERT_EQ(layout.layoutDirection, CLAY_LEFT_TO_RIGHT);
    ASSERT_EQ(layout.childGap, 16);

    TEST_PASS();
}

TEST(center_layout_alignment) {
    Clay_Sizing sizing = { 0 };
    Clay_LayoutConfig layout = ClayKit_CenterLayout(sizing);

    ASSERT_EQ(layout.childAlignment.x, CLAY_ALIGN_X_CENTER);
    ASSERT_EQ(layout.childAlignment.y, CLAY_ALIGN_Y_CENTER);
    ASSERT_EQ(layout.childGap, 0);
    ASSERT_EQ(layout.padding.left, 0);

    TEST_PASS();
}

TEST(container_layout_max_width) {
    ClayKit_ContainerConfig cfg = { .max_width = 800, .padding = 24 };
    Clay_LayoutConfig layout = ClayKit_ContainerLayout(cfg);

    ASSERT_EQ(layout.sizing.width.type, CLAY__SIZING_TYPE_GROW);
    ASSERT_EQ_FLOAT(layout.sizing.width.size.minMax.max, 800.0f, 0.001f);
    ASSERT_EQ(layout.padding.left, 24);
    ASSERT_EQ(layout.padding.right, 24);
    ASSERT_EQ(layout.layoutDirection, CLAY_TOP_TO_BOTTOM);

    TEST_PASS();
}

TEST(container_layout_default_max_width) {
    ClayKit_ContainerConfig cfg = { .max_width = 0 }; /* Should default to 1200 */
    Clay_LayoutConfig layout = ClayKit_ContainerLayout(cfg);

    ASSERT_EQ_FLOAT(layout.sizing.width.size.minMax.max, 1200.0f, 0.001f);

    TEST_PASS();
}

TEST(spacer_layout_grow) {
    Clay_LayoutConfig layout = ClayKit_SpacerLayout();

    ASSERT_EQ(layout.sizing.width.type, CLAY__SIZING_TYPE_GROW);
    ASSERT_EQ(layout.sizing.height.type, CLAY__SIZING_TYPE_GROW);
    ASSERT_EQ(layout.padding.left, 0);
    ASSERT_EQ(layout.childGap, 0);

    TEST_PASS();
}

TEST(box_layout_sizing) {
    ClayKit_BoxConfig cfg = {
        .sizing = {
            .width = CLAY_SIZING_FIXED(100),
            .height = CLAY_SIZING_FIXED(50)
        }
    };
    Clay_LayoutConfig layout = ClayKit_BoxLayout(cfg);

    ASSERT_EQ(layout.sizing.width.type, CLAY__SIZING_TYPE_FIXED);
    ASSERT_EQ_FLOAT(layout.sizing.width.size.minMax.min, 100.0f, 0.001f);
    ASSERT_EQ(layout.sizing.height.type, CLAY__SIZING_TYPE_FIXED);
    ASSERT_EQ_FLOAT(layout.sizing.height.size.minMax.min, 50.0f, 0.001f);

    TEST_PASS();
}

/* ============================================================================
 * Badge Style Tests
 * ============================================================================ */

TEST(badge_style_solid_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_BadgeConfig cfg = { .variant = CLAYKIT_BADGE_SOLID, .color_scheme = CLAYKIT_COLOR_PRIMARY, .size = CLAYKIT_SIZE_MD };
    ClayKit_BadgeStyle style = ClayKit_ComputeBadgeStyle(&ctx, cfg);

    /* Solid badge has scheme color background, white text */
    ASSERT_EQ(style.bg_color.r, theme.primary.r);
    ASSERT_EQ(style.text_color.r, 255);
    ASSERT_EQ(style.text_color.g, 255);
    ASSERT_EQ(style.text_color.b, 255);
    ASSERT_EQ(style.border_width, 0);
    ASSERT_EQ(style.font_size, theme.font_size.sm);
    ASSERT_EQ(style.corner_radius, theme.radius.full);

    TEST_PASS();
}

TEST(badge_style_subtle) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_BadgeConfig cfg = { .variant = CLAYKIT_BADGE_SUBTLE, .color_scheme = CLAYKIT_COLOR_SUCCESS, .size = CLAYKIT_SIZE_MD };
    ClayKit_BadgeStyle style = ClayKit_ComputeBadgeStyle(&ctx, cfg);

    /* Subtle badge has lightened background, scheme color text */
    ASSERT(style.bg_color.r > theme.success.r); /* Lightened */
    ASSERT_EQ(style.text_color.r, theme.success.r);
    ASSERT_EQ(style.text_color.g, theme.success.g);
    ASSERT_EQ(style.text_color.b, theme.success.b);
    ASSERT_EQ(style.border_width, 0);

    TEST_PASS();
}

TEST(badge_style_outline) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_BadgeConfig cfg = { .variant = CLAYKIT_BADGE_OUTLINE, .color_scheme = CLAYKIT_COLOR_ERROR, .size = CLAYKIT_SIZE_MD };
    ClayKit_BadgeStyle style = ClayKit_ComputeBadgeStyle(&ctx, cfg);

    /* Outline badge has transparent bg, scheme color border and text */
    ASSERT_EQ(style.bg_color.a, 0);
    ASSERT_EQ(style.text_color.r, theme.error.r);
    ASSERT_EQ(style.border_color.r, theme.error.r);
    ASSERT_EQ(style.border_width, 1);

    TEST_PASS();
}

TEST(badge_style_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_BadgeConfig cfg = { .variant = CLAYKIT_BADGE_SOLID, .color_scheme = CLAYKIT_COLOR_PRIMARY, .size = CLAYKIT_SIZE_XS };
    ClayKit_BadgeStyle style_xs = ClayKit_ComputeBadgeStyle(&ctx, cfg);

    cfg.size = CLAYKIT_SIZE_XL;
    ClayKit_BadgeStyle style_xl = ClayKit_ComputeBadgeStyle(&ctx, cfg);

    /* XL should have larger padding than XS */
    ASSERT(style_xl.pad_x > style_xs.pad_x);
    ASSERT(style_xl.pad_y > style_xs.pad_y);
    ASSERT(style_xl.font_size > style_xs.font_size);

    TEST_PASS();
}

/* ============================================================================
 * Tag Style Tests
 * ============================================================================ */

TEST(tag_style_solid_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_TagConfig cfg = { .variant = CLAYKIT_TAG_SOLID, .color_scheme = CLAYKIT_COLOR_PRIMARY, .size = CLAYKIT_SIZE_MD };
    ClayKit_TagStyle style = ClayKit_ComputeTagStyle(&ctx, cfg);

    ASSERT_EQ(style.bg_color.r, theme.primary.r);
    ASSERT_EQ(style.text_color.r, 255);
    ASSERT_EQ(style.text_color.g, 255);
    ASSERT_EQ(style.border_width, 0);
    ASSERT_EQ(style.corner_radius, theme.radius.md);

    TEST_PASS();
}

TEST(tag_style_subtle) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_TagConfig cfg = { .variant = CLAYKIT_TAG_SUBTLE, .color_scheme = CLAYKIT_COLOR_SUCCESS };
    ClayKit_TagStyle style = ClayKit_ComputeTagStyle(&ctx, cfg);

    ASSERT(style.bg_color.r > theme.success.r);  /* Lightened */
    ASSERT_EQ(style.text_color.r, theme.success.r);
    ASSERT_EQ(style.border_width, 0);

    TEST_PASS();
}

TEST(tag_style_outline) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_TagConfig cfg = { .variant = CLAYKIT_TAG_OUTLINE, .color_scheme = CLAYKIT_COLOR_ERROR };
    ClayKit_TagStyle style = ClayKit_ComputeTagStyle(&ctx, cfg);

    ASSERT_EQ(style.bg_color.a, 0);
    ASSERT_EQ(style.text_color.r, theme.error.r);
    ASSERT_EQ(style.border_color.r, theme.error.r);
    ASSERT_EQ(style.border_width, 1);

    TEST_PASS();
}

TEST(tag_style_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_TagConfig cfg = { .size = CLAYKIT_SIZE_XS };
    ClayKit_TagStyle style_xs = ClayKit_ComputeTagStyle(&ctx, cfg);
    cfg.size = CLAYKIT_SIZE_XL;
    ClayKit_TagStyle style_xl = ClayKit_ComputeTagStyle(&ctx, cfg);

    ASSERT(style_xl.pad_x > style_xs.pad_x);
    ASSERT(style_xl.font_size > style_xs.font_size);
    ASSERT(style_xl.gap > style_xs.gap);

    TEST_PASS();
}

TEST(tag_style_closeable) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_TagConfig cfg = { .closeable = true, .size = CLAYKIT_SIZE_MD };
    ClayKit_TagStyle style = ClayKit_ComputeTagStyle(&ctx, cfg);

    ASSERT(style.gap > 0);
    ASSERT(style.close_font_size > 0);
    ASSERT(style.close_color.a > 0);

    TEST_PASS();
}

/* ============================================================================
 * Stat Style Tests
 * ============================================================================ */

TEST(stat_style_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_StatConfig cfg = {0};
    ClayKit_StatStyle style = ClayKit_ComputeStatStyle(&ctx, cfg);

    /* Defaults to theme colors */
    ASSERT_EQ(style.label_color.r, theme.muted.r);
    ASSERT_EQ(style.value_color.r, theme.fg.r);
    ASSERT_EQ(style.help_color.r, theme.muted.r);
    /* Value font larger than label */
    ASSERT(style.value_font_size > style.label_font_size);
    /* Value uses heading font, label uses body */
    ASSERT_EQ(style.label_font_id, theme.font_id.body);
    ASSERT_EQ(style.value_font_id, theme.font_id.heading);

    TEST_PASS();
}

TEST(stat_style_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_StatConfig cfg_xs = { .size = CLAYKIT_SIZE_XS };
    ClayKit_StatStyle style_xs = ClayKit_ComputeStatStyle(&ctx, cfg_xs);
    ClayKit_StatConfig cfg_xl = { .size = CLAYKIT_SIZE_XL };
    ClayKit_StatStyle style_xl = ClayKit_ComputeStatStyle(&ctx, cfg_xl);

    ASSERT(style_xl.value_font_size > style_xs.value_font_size);
    ASSERT(style_xl.label_font_size > style_xs.label_font_size);

    TEST_PASS();
}

TEST(stat_style_custom_colors) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    Clay_Color custom = { 100, 50, 200, 255 };
    ClayKit_StatConfig cfg = { .label_color = custom, .value_color = custom, .help_color = custom };
    ClayKit_StatStyle style = ClayKit_ComputeStatStyle(&ctx, cfg);

    ASSERT_EQ(style.label_color.r, 100);
    ASSERT_EQ(style.value_color.g, 50);
    ASSERT_EQ(style.help_color.b, 200);

    TEST_PASS();
}

/* ============================================================================
 * List Style Tests
 * ============================================================================ */

TEST(list_style_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_ListConfig cfg = { .size = CLAYKIT_SIZE_MD };
    ClayKit_ListStyle style = ClayKit_ComputeListStyle(&ctx, cfg);

    ASSERT_EQ(style.marker_color.r, theme.muted.r);
    ASSERT_EQ(style.text_color.r, theme.fg.r);
    ASSERT_EQ(style.font_size, theme.font_size.md);
    ASSERT(style.gap > 0);
    ASSERT(style.marker_width > 0);
    ASSERT(style.item_gap > 0);

    TEST_PASS();
}

TEST(list_style_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_ListConfig cfg_xs = { .size = CLAYKIT_SIZE_XS };
    ClayKit_ListStyle style_xs = ClayKit_ComputeListStyle(&ctx, cfg_xs);
    ClayKit_ListConfig cfg_xl = { .size = CLAYKIT_SIZE_XL };
    ClayKit_ListStyle style_xl = ClayKit_ComputeListStyle(&ctx, cfg_xl);

    ASSERT(style_xl.font_size > style_xs.font_size);
    ASSERT(style_xl.gap > style_xs.gap);
    ASSERT(style_xl.marker_width > style_xs.marker_width);

    TEST_PASS();
}

TEST(list_style_custom_colors) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    Clay_Color red = { 255, 0, 0, 255 };
    Clay_Color blue = { 0, 0, 255, 255 };
    ClayKit_ListConfig cfg = { .marker_color = red, .text_color = blue };
    ClayKit_ListStyle style = ClayKit_ComputeListStyle(&ctx, cfg);

    ASSERT_EQ(style.marker_color.r, 255);
    ASSERT_EQ(style.marker_color.g, 0);
    ASSERT_EQ(style.text_color.b, 255);
    ASSERT_EQ(style.text_color.r, 0);

    TEST_PASS();
}

/* ============================================================================
 * Table Style Tests
 * ============================================================================ */

TEST(table_style_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_TableConfig cfg = {0};
    ClayKit_TableStyle style = ClayKit_ComputeTableStyle(&ctx, cfg);

    /* Header bg = primary by default */
    ASSERT_EQ(style.header_bg.r, theme.primary.r);
    ASSERT_EQ(style.header_text.r, 255); /* White text */
    ASSERT_EQ(style.row_bg.r, theme.bg.r);
    ASSERT_EQ(style.text_color.r, theme.fg.r);
    ASSERT_EQ(style.border_width, 0); /* Not bordered by default */

    TEST_PASS();
}

TEST(table_style_striped) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_TableConfig cfg = { .striped = true };
    ClayKit_TableStyle style = ClayKit_ComputeTableStyle(&ctx, cfg);

    /* Alt row bg should differ from row bg */
    ASSERT(style.row_alt_bg.r != style.row_bg.r ||
           style.row_alt_bg.g != style.row_bg.g ||
           style.row_alt_bg.b != style.row_bg.b);

    TEST_PASS();
}

TEST(table_style_bordered) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_TableConfig cfg = { .bordered = true };
    ClayKit_TableStyle style = ClayKit_ComputeTableStyle(&ctx, cfg);

    ASSERT_EQ(style.border_width, 1);
    ASSERT_EQ(style.border_color.r, theme.border.r);

    TEST_PASS();
}

TEST(table_style_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_TableConfig cfg_xs = { .size = CLAYKIT_SIZE_XS };
    ClayKit_TableStyle style_xs = ClayKit_ComputeTableStyle(&ctx, cfg_xs);
    ClayKit_TableConfig cfg_xl = { .size = CLAYKIT_SIZE_XL };
    ClayKit_TableStyle style_xl = ClayKit_ComputeTableStyle(&ctx, cfg_xl);

    ASSERT(style_xl.cell_pad_x > style_xs.cell_pad_x);
    ASSERT(style_xl.cell_pad_y > style_xs.cell_pad_y);
    ASSERT(style_xl.font_size > style_xs.font_size);

    TEST_PASS();
}

TEST(table_style_color_scheme) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_TableConfig cfg = { .color_scheme = CLAYKIT_COLOR_ERROR };
    ClayKit_TableStyle style = ClayKit_ComputeTableStyle(&ctx, cfg);

    ASSERT_EQ(style.header_bg.r, theme.error.r);
    ASSERT_EQ(style.header_bg.g, theme.error.g);

    TEST_PASS();
}

/* ============================================================================
 * Button Style Tests
 * ============================================================================ */

TEST(button_bg_color_solid_normal) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_ButtonConfig cfg = { .variant = CLAYKIT_BUTTON_SOLID, .color_scheme = CLAYKIT_COLOR_PRIMARY };
    Clay_Color color = ClayKit_ButtonBgColor(&ctx, cfg, false);

    ASSERT_EQ(color.r, theme.primary.r);
    ASSERT_EQ(color.g, theme.primary.g);
    ASSERT_EQ(color.b, theme.primary.b);

    TEST_PASS();
}

TEST(button_bg_color_solid_hovered) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_ButtonConfig cfg = { .variant = CLAYKIT_BUTTON_SOLID, .color_scheme = CLAYKIT_COLOR_PRIMARY };
    Clay_Color normal = ClayKit_ButtonBgColor(&ctx, cfg, false);
    Clay_Color hovered = ClayKit_ButtonBgColor(&ctx, cfg, true);

    /* Hovered should be darker */
    ASSERT(hovered.r < normal.r || hovered.g < normal.g || hovered.b < normal.b);

    TEST_PASS();
}

TEST(button_bg_color_outline) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_ButtonConfig cfg = { .variant = CLAYKIT_BUTTON_OUTLINE, .color_scheme = CLAYKIT_COLOR_PRIMARY };
    Clay_Color color = ClayKit_ButtonBgColor(&ctx, cfg, false);

    /* Outline button should be transparent when not hovered */
    ASSERT_EQ(color.a, 0);

    TEST_PASS();
}

TEST(button_bg_color_disabled) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_ButtonConfig cfg = { .variant = CLAYKIT_BUTTON_SOLID, .color_scheme = CLAYKIT_COLOR_PRIMARY, .disabled = true };
    Clay_Color color = ClayKit_ButtonBgColor(&ctx, cfg, false);

    /* Disabled button uses border color */
    ASSERT_EQ(color.r, theme.border.r);
    ASSERT_EQ(color.g, theme.border.g);
    ASSERT_EQ(color.b, theme.border.b);

    TEST_PASS();
}

TEST(button_text_color_solid) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_ButtonConfig cfg = { .variant = CLAYKIT_BUTTON_SOLID };
    Clay_Color color = ClayKit_ButtonTextColor(&ctx, cfg);

    /* Solid button text is white */
    ASSERT_EQ(color.r, 255);
    ASSERT_EQ(color.g, 255);
    ASSERT_EQ(color.b, 255);

    TEST_PASS();
}

TEST(button_text_color_outline) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_ButtonConfig cfg = { .variant = CLAYKIT_BUTTON_OUTLINE, .color_scheme = CLAYKIT_COLOR_SUCCESS };
    Clay_Color color = ClayKit_ButtonTextColor(&ctx, cfg);

    /* Outline button text matches scheme color */
    ASSERT_EQ(color.r, theme.success.r);
    ASSERT_EQ(color.g, theme.success.g);
    ASSERT_EQ(color.b, theme.success.b);

    TEST_PASS();
}

TEST(button_border_width) {
    ClayKit_ButtonConfig cfg_outline = { .variant = CLAYKIT_BUTTON_OUTLINE };
    ClayKit_ButtonConfig cfg_solid = { .variant = CLAYKIT_BUTTON_SOLID };
    ClayKit_ButtonConfig cfg_ghost = { .variant = CLAYKIT_BUTTON_GHOST };

    ASSERT_EQ(ClayKit_ButtonBorderWidth(cfg_outline), 1);
    ASSERT_EQ(ClayKit_ButtonBorderWidth(cfg_solid), 0);
    ASSERT_EQ(ClayKit_ButtonBorderWidth(cfg_ghost), 0);

    TEST_PASS();
}

TEST(button_padding_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    uint16_t pad_xs = ClayKit_ButtonPaddingX(&ctx, CLAYKIT_SIZE_XS);
    uint16_t pad_xl = ClayKit_ButtonPaddingX(&ctx, CLAYKIT_SIZE_XL);

    /* XL should have larger padding than XS */
    ASSERT(pad_xl > pad_xs);

    TEST_PASS();
}

/* ============================================================================
 * Progress Style Tests
 * ============================================================================ */

TEST(progress_style_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_ProgressConfig cfg = { .color_scheme = CLAYKIT_COLOR_PRIMARY, .size = CLAYKIT_SIZE_MD };
    ClayKit_ProgressStyle style = ClayKit_ComputeProgressStyle(&ctx, cfg);

    /* Fill color should match scheme color */
    ASSERT_EQ(style.fill_color.r, theme.primary.r);
    ASSERT_EQ(style.fill_color.g, theme.primary.g);
    ASSERT_EQ(style.fill_color.b, theme.primary.b);
    ASSERT_EQ(style.height, 8);
    ASSERT_EQ(style.corner_radius, 4); /* height / 2 */

    TEST_PASS();
}

TEST(progress_style_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_ProgressConfig cfg = { .size = CLAYKIT_SIZE_XS };
    ClayKit_ProgressStyle style_xs = ClayKit_ComputeProgressStyle(&ctx, cfg);

    cfg.size = CLAYKIT_SIZE_XL;
    ClayKit_ProgressStyle style_xl = ClayKit_ComputeProgressStyle(&ctx, cfg);

    ASSERT(style_xl.height > style_xs.height);

    TEST_PASS();
}

/* ============================================================================
 * Slider Style Tests
 * ============================================================================ */

TEST(slider_style_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_SliderConfig cfg = { .color_scheme = CLAYKIT_COLOR_PRIMARY, .size = CLAYKIT_SIZE_MD };
    ClayKit_SliderStyle style = ClayKit_ComputeSliderStyle(&ctx, cfg, false);

    /* Fill and thumb color should match scheme */
    ASSERT_EQ(style.fill_color.r, theme.primary.r);
    ASSERT_EQ(style.thumb_color.r, theme.primary.r);
    ASSERT_EQ(style.track_height, 8);
    ASSERT_EQ(style.thumb_size, 20);

    TEST_PASS();
}

TEST(slider_style_hovered) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_SliderConfig cfg = { .color_scheme = CLAYKIT_COLOR_PRIMARY, .size = CLAYKIT_SIZE_MD };
    ClayKit_SliderStyle normal = ClayKit_ComputeSliderStyle(&ctx, cfg, false);
    ClayKit_SliderStyle hovered = ClayKit_ComputeSliderStyle(&ctx, cfg, true);

    /* Thumb should be darker when hovered */
    ASSERT(hovered.thumb_color.r < normal.thumb_color.r ||
           hovered.thumb_color.g < normal.thumb_color.g ||
           hovered.thumb_color.b < normal.thumb_color.b);

    TEST_PASS();
}

TEST(slider_style_disabled) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_SliderConfig cfg = { .color_scheme = CLAYKIT_COLOR_PRIMARY, .disabled = true };
    ClayKit_SliderStyle style = ClayKit_ComputeSliderStyle(&ctx, cfg, false);

    /* Disabled should use muted color */
    ASSERT_EQ(style.fill_color.r, theme.muted.r);
    ASSERT_EQ(style.thumb_color.r, theme.muted.r);

    TEST_PASS();
}

/* ============================================================================
 * Alert Style Tests
 * ============================================================================ */

TEST(alert_style_subtle) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_AlertConfig cfg = { .variant = CLAYKIT_ALERT_SUBTLE, .color_scheme = CLAYKIT_COLOR_SUCCESS };
    ClayKit_AlertStyle style = ClayKit_ComputeAlertStyle(&ctx, cfg);

    /* Subtle has lightened bg, scheme border */
    ASSERT(style.bg_color.r > theme.success.r); /* Lightened */
    ASSERT_EQ(style.border_width, 1);
    ASSERT_EQ(style.padding, theme.spacing.md);

    TEST_PASS();
}

TEST(alert_style_solid) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_AlertConfig cfg = { .variant = CLAYKIT_ALERT_SOLID, .color_scheme = CLAYKIT_COLOR_ERROR };
    ClayKit_AlertStyle style = ClayKit_ComputeAlertStyle(&ctx, cfg);

    /* Solid has scheme color bg, white text */
    ASSERT_EQ(style.bg_color.r, theme.error.r);
    ASSERT_EQ(style.text_color.r, 255);
    ASSERT_EQ(style.text_color.g, 255);
    ASSERT_EQ(style.text_color.b, 255);
    ASSERT_EQ(style.border_width, 0);

    TEST_PASS();
}

TEST(alert_style_outline) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_AlertConfig cfg = { .variant = CLAYKIT_ALERT_OUTLINE, .color_scheme = CLAYKIT_COLOR_WARNING };
    ClayKit_AlertStyle style = ClayKit_ComputeAlertStyle(&ctx, cfg);

    /* Outline has transparent bg, scheme border and text */
    ASSERT_EQ(style.bg_color.a, 0);
    ASSERT_EQ(style.border_color.r, theme.warning.r);
    ASSERT_EQ(style.text_color.r, theme.warning.r);
    ASSERT_EQ(style.border_width, 1);

    TEST_PASS();
}

/* ============================================================================
 * Tooltip Style Tests
 * ============================================================================ */

TEST(tooltip_style_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_TooltipConfig cfg = { .position = CLAYKIT_TOOLTIP_TOP };
    ClayKit_TooltipStyle style = ClayKit_ComputeTooltipStyle(&ctx, cfg);

    /* Dark background, light text */
    ASSERT(style.bg_color.r < 50); /* Dark */
    ASSERT(style.text_color.r > 200); /* Light */
    ASSERT_EQ(style.padding_x, theme.spacing.sm);
    ASSERT_EQ(style.padding_y, theme.spacing.xs);
    ASSERT_EQ(style.font_size, theme.font_size.sm);

    TEST_PASS();
}

/* ============================================================================
 * Tabs Style Tests
 * ============================================================================ */

TEST(tabs_style_line) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_TabsConfig cfg = { .variant = CLAYKIT_TABS_LINE, .color_scheme = CLAYKIT_COLOR_PRIMARY, .size = CLAYKIT_SIZE_MD };
    ClayKit_TabsStyle style = ClayKit_ComputeTabsStyle(&ctx, cfg);

    /* Line variant: active text is scheme color */
    ASSERT_EQ(style.active_color.r, theme.primary.r);
    ASSERT_EQ(style.active_text.r, theme.primary.r);
    ASSERT_EQ(style.inactive_color.r, theme.muted.r);
    ASSERT(style.indicator_height > 0);

    TEST_PASS();
}

TEST(tabs_style_enclosed) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_TabsConfig cfg = { .variant = CLAYKIT_TABS_ENCLOSED, .color_scheme = CLAYKIT_COLOR_PRIMARY, .size = CLAYKIT_SIZE_MD };
    ClayKit_TabsStyle style = ClayKit_ComputeTabsStyle(&ctx, cfg);

    /* Enclosed: active text is white */
    ASSERT_EQ(style.active_text.r, 255);
    ASSERT_EQ(style.active_text.g, 255);
    ASSERT_EQ(style.active_text.b, 255);

    TEST_PASS();
}

TEST(tabs_style_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_TabsConfig cfg = { .variant = CLAYKIT_TABS_LINE, .size = CLAYKIT_SIZE_XS };
    ClayKit_TabsStyle style_xs = ClayKit_ComputeTabsStyle(&ctx, cfg);

    cfg.size = CLAYKIT_SIZE_XL;
    ClayKit_TabsStyle style_xl = ClayKit_ComputeTabsStyle(&ctx, cfg);

    ASSERT(style_xl.padding_x > style_xs.padding_x);
    ASSERT(style_xl.font_size > style_xs.font_size);

    TEST_PASS();
}

/* ============================================================================
 * Modal Style Tests
 * ============================================================================ */

TEST(modal_style_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_ModalConfig cfg = { .size = CLAYKIT_MODAL_MD };
    ClayKit_ModalStyle style = ClayKit_ComputeModalStyle(&ctx, cfg);

    /* Default backdrop is semi-transparent black */
    ASSERT_EQ(style.backdrop_color.r, 0);
    ASSERT_EQ(style.backdrop_color.g, 0);
    ASSERT_EQ(style.backdrop_color.b, 0);
    ASSERT_EQ(style.backdrop_color.a, 128);

    /* Background matches theme */
    ASSERT_EQ(style.bg_color.r, theme.bg.r);
    ASSERT_EQ(style.width, 500);
    ASSERT_EQ(style.z_index, 1000);

    TEST_PASS();
}

TEST(modal_style_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_ModalConfig cfg = { .size = CLAYKIT_MODAL_SM };
    ClayKit_ModalStyle style_sm = ClayKit_ComputeModalStyle(&ctx, cfg);
    ASSERT_EQ(style_sm.width, 400);

    cfg.size = CLAYKIT_MODAL_LG;
    ClayKit_ModalStyle style_lg = ClayKit_ComputeModalStyle(&ctx, cfg);
    ASSERT_EQ(style_lg.width, 600);

    cfg.size = CLAYKIT_MODAL_XL;
    ClayKit_ModalStyle style_xl = ClayKit_ComputeModalStyle(&ctx, cfg);
    ASSERT_EQ(style_xl.width, 800);

    cfg.size = CLAYKIT_MODAL_FULL;
    ClayKit_ModalStyle style_full = ClayKit_ComputeModalStyle(&ctx, cfg);
    ASSERT_EQ(style_full.width, 0); /* Full uses grow */

    TEST_PASS();
}

TEST(modal_style_custom_z_index) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_ModalConfig cfg = { .size = CLAYKIT_MODAL_MD, .z_index = 2000 };
    ClayKit_ModalStyle style = ClayKit_ComputeModalStyle(&ctx, cfg);

    ASSERT_EQ(style.z_index, 2000);

    TEST_PASS();
}

/* ============================================================================
 * Input Style Tests
 * ============================================================================ */

TEST(input_style_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_InputConfig cfg = { .size = CLAYKIT_SIZE_MD };
    ClayKit_InputStyle style = ClayKit_ComputeInputStyle(&ctx, cfg, false);

    /* Default colors from theme */
    ASSERT_EQ(style.bg_color.r, theme.bg.r);
    ASSERT_EQ(style.text_color.r, theme.fg.r);
    ASSERT_EQ(style.placeholder_color.r, theme.muted.r);
    ASSERT_EQ(style.border_color.r, theme.border.r);
    ASSERT_EQ(style.cursor_width, 2);

    TEST_PASS();
}

TEST(input_style_focused) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_InputConfig cfg = { .size = CLAYKIT_SIZE_MD };
    ClayKit_InputStyle style = ClayKit_ComputeInputStyle(&ctx, cfg, true);

    /* Focused uses primary color for border */
    ASSERT_EQ(style.border_color.r, theme.primary.r);
    ASSERT_EQ(style.border_color.g, theme.primary.g);
    ASSERT_EQ(style.border_color.b, theme.primary.b);

    TEST_PASS();
}

TEST(input_style_custom_colors) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_InputConfig cfg = {
        .size = CLAYKIT_SIZE_MD,
        .bg = { 100, 100, 100, 255 },
        .text_color = { 200, 200, 200, 255 }
    };
    ClayKit_InputStyle style = ClayKit_ComputeInputStyle(&ctx, cfg, false);

    /* Custom colors should be used */
    ASSERT_EQ(style.bg_color.r, 100);
    ASSERT_EQ(style.text_color.r, 200);

    TEST_PASS();
}

/* ============================================================================
 * Checkbox and Switch Style Tests
 * ============================================================================ */

TEST(checkbox_size_values) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    uint16_t size_xs = ClayKit_CheckboxSize(&ctx, CLAYKIT_SIZE_XS);
    uint16_t size_md = ClayKit_CheckboxSize(&ctx, CLAYKIT_SIZE_MD);
    uint16_t size_xl = ClayKit_CheckboxSize(&ctx, CLAYKIT_SIZE_XL);

    ASSERT_EQ(size_xs, 14);
    ASSERT_EQ(size_md, 18);
    ASSERT_EQ(size_xl, 26);

    TEST_PASS();
}

TEST(checkbox_bg_color_states) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_CheckboxConfig cfg = { .color_scheme = CLAYKIT_COLOR_PRIMARY };

    /* Unchecked, not hovered: theme bg */
    Clay_Color unchecked = ClayKit_CheckboxBgColor(&ctx, cfg, false, false);
    ASSERT_EQ(unchecked.r, theme.bg.r);

    /* Checked, not hovered: scheme color */
    Clay_Color checked = ClayKit_CheckboxBgColor(&ctx, cfg, true, false);
    ASSERT_EQ(checked.r, theme.primary.r);

    TEST_PASS();
}

TEST(switch_size_values) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    uint16_t w_md = ClayKit_SwitchWidth(&ctx, CLAYKIT_SIZE_MD);
    uint16_t h_md = ClayKit_SwitchHeight(&ctx, CLAYKIT_SIZE_MD);
    uint16_t knob_md = ClayKit_SwitchKnobSize(&ctx, CLAYKIT_SIZE_MD);

    ASSERT_EQ(w_md, 42);
    ASSERT_EQ(h_md, 24);
    ASSERT_EQ(knob_md, 20);

    TEST_PASS();
}

TEST(switch_bg_color_states) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_SwitchConfig cfg = { .color_scheme = CLAYKIT_COLOR_SUCCESS };

    /* Off: border color */
    Clay_Color off = ClayKit_SwitchBgColor(&ctx, cfg, false, false);
    ASSERT_EQ(off.r, theme.border.r);

    /* On: scheme color */
    Clay_Color on = ClayKit_SwitchBgColor(&ctx, cfg, true, false);
    ASSERT_EQ(on.r, theme.success.r);

    TEST_PASS();
}

/* ============================================================================
 * Radio Style Tests
 * ============================================================================ */

TEST(radio_size_values) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    /* Radio uses same sizes as Checkbox */
    uint16_t size_xs = ClayKit_RadioSize(&ctx, CLAYKIT_SIZE_XS);
    uint16_t size_md = ClayKit_RadioSize(&ctx, CLAYKIT_SIZE_MD);
    uint16_t size_xl = ClayKit_RadioSize(&ctx, CLAYKIT_SIZE_XL);

    ASSERT_EQ(size_xs, 14);
    ASSERT_EQ(size_md, 18);
    ASSERT_EQ(size_xl, 26);

    TEST_PASS();
}

TEST(radio_bg_color_states) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_RadioConfig cfg = { .color_scheme = CLAYKIT_COLOR_PRIMARY };

    /* Unselected, not hovered: theme bg */
    Clay_Color unselected = ClayKit_RadioBgColor(&ctx, cfg, false, false);
    ASSERT_EQ(unselected.r, theme.bg.r);

    /* Selected, not hovered: scheme color */
    Clay_Color selected = ClayKit_RadioBgColor(&ctx, cfg, true, false);
    ASSERT_EQ(selected.r, theme.primary.r);

    /* Selected, hovered: darker scheme color */
    Clay_Color selected_hovered = ClayKit_RadioBgColor(&ctx, cfg, true, true);
    ASSERT(selected_hovered.r < selected.r || selected_hovered.g < selected.g || selected_hovered.b < selected.b);

    /* Disabled selected: muted */
    cfg.disabled = true;
    Clay_Color disabled_selected = ClayKit_RadioBgColor(&ctx, cfg, true, false);
    ASSERT_EQ(disabled_selected.r, theme.muted.r);

    /* Disabled unselected: border */
    Clay_Color disabled_unselected = ClayKit_RadioBgColor(&ctx, cfg, false, false);
    ASSERT_EQ(disabled_unselected.r, theme.border.r);

    TEST_PASS();
}

TEST(radio_border_color) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_RadioConfig cfg = { .color_scheme = CLAYKIT_COLOR_SUCCESS };

    /* Selected: scheme color */
    Clay_Color selected_border = ClayKit_RadioBorderColor(&ctx, cfg, true);
    ASSERT_EQ(selected_border.r, theme.success.r);

    /* Unselected: theme border */
    Clay_Color unselected_border = ClayKit_RadioBorderColor(&ctx, cfg, false);
    ASSERT_EQ(unselected_border.r, theme.border.r);

    /* Disabled: muted */
    cfg.disabled = true;
    Clay_Color disabled_border = ClayKit_RadioBorderColor(&ctx, cfg, true);
    ASSERT_EQ(disabled_border.r, theme.muted.r);

    TEST_PASS();
}

/* ============================================================================
 * Select Style Tests
 * ============================================================================ */

TEST(select_style_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_SelectConfig cfg = { .size = CLAYKIT_SIZE_MD };
    ClayKit_SelectStyle style = ClayKit_ComputeSelectStyle(&ctx, cfg);

    /* Default colors from theme */
    ASSERT_EQ(style.bg_color.r, theme.bg.r);
    ASSERT_EQ(style.border_color.r, theme.border.r);
    ASSERT_EQ(style.text_color.r, theme.fg.r);
    ASSERT_EQ(style.placeholder_color.r, theme.muted.r);
    ASSERT_EQ(style.dropdown_bg.r, theme.bg.r);
    ASSERT_EQ(style.font_size, theme.font_size.md);
    ASSERT(style.padding_x > 0);
    ASSERT(style.padding_y > 0);

    TEST_PASS();
}

TEST(select_style_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_SelectConfig cfg_xs = { .size = CLAYKIT_SIZE_XS };
    ClayKit_SelectStyle style_xs = ClayKit_ComputeSelectStyle(&ctx, cfg_xs);

    ClayKit_SelectConfig cfg_xl = { .size = CLAYKIT_SIZE_XL };
    ClayKit_SelectStyle style_xl = ClayKit_ComputeSelectStyle(&ctx, cfg_xl);

    /* XL should have larger padding and font than XS */
    ASSERT(style_xl.padding_x > style_xs.padding_x);
    ASSERT(style_xl.padding_y > style_xs.padding_y);
    ASSERT(style_xl.font_size > style_xs.font_size);

    TEST_PASS();
}

TEST(select_style_color_scheme) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_SelectConfig cfg_primary = { .color_scheme = CLAYKIT_COLOR_PRIMARY };
    ClayKit_SelectStyle style_primary = ClayKit_ComputeSelectStyle(&ctx, cfg_primary);

    ClayKit_SelectConfig cfg_error = { .color_scheme = CLAYKIT_COLOR_ERROR };
    ClayKit_SelectStyle style_error = ClayKit_ComputeSelectStyle(&ctx, cfg_error);

    /* option_hover_bg should differ between schemes */
    ASSERT(style_primary.option_hover_bg.r != style_error.option_hover_bg.r ||
           style_primary.option_hover_bg.g != style_error.option_hover_bg.g);

    TEST_PASS();
}

TEST(select_style_disabled) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_SelectConfig cfg = { .disabled = true };
    ClayKit_SelectStyle style = ClayKit_ComputeSelectStyle(&ctx, cfg);

    /* Disabled text color should be muted */
    ASSERT_EQ(style.text_color.r, theme.muted.r);

    TEST_PASS();
}

/* ============================================================================
 * Spinner Style Tests
 * ============================================================================ */

TEST(spinner_style_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_SpinnerConfig cfg = { .size = CLAYKIT_SIZE_MD };
    ClayKit_SpinnerStyle style = ClayKit_ComputeSpinnerStyle(&ctx, cfg);

    ASSERT_EQ(style.color.r, theme.primary.r);
    ASSERT_EQ(style.diameter, 32);
    ASSERT_EQ(style.thickness, 4);
    ASSERT_EQ_FLOAT(style.speed, 1.0f, 0.001f);

    TEST_PASS();
}

TEST(spinner_style_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_SpinnerConfig cfg_xs = { .size = CLAYKIT_SIZE_XS };
    ClayKit_SpinnerStyle style_xs = ClayKit_ComputeSpinnerStyle(&ctx, cfg_xs);

    ClayKit_SpinnerConfig cfg_xl = { .size = CLAYKIT_SIZE_XL };
    ClayKit_SpinnerStyle style_xl = ClayKit_ComputeSpinnerStyle(&ctx, cfg_xl);

    ASSERT(style_xl.diameter > style_xs.diameter);
    ASSERT(style_xl.thickness > style_xs.thickness);

    TEST_PASS();
}

TEST(spinner_style_color_scheme) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_SpinnerConfig cfg = { .color_scheme = CLAYKIT_COLOR_SUCCESS };
    ClayKit_SpinnerStyle style = ClayKit_ComputeSpinnerStyle(&ctx, cfg);

    ASSERT_EQ(style.color.r, theme.success.r);
    ASSERT_EQ(style.color.g, theme.success.g);

    TEST_PASS();
}

TEST(spinner_style_custom_speed) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_SpinnerConfig cfg = { .speed = 2.5f };
    ClayKit_SpinnerStyle style = ClayKit_ComputeSpinnerStyle(&ctx, cfg);

    ASSERT_EQ_FLOAT(style.speed, 2.5f, 0.001f);

    TEST_PASS();
}

TEST(spinner_angle_advances) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_SpinnerConfig cfg = {0};

    ctx.cursor_blink_time = 0.0f;
    float angle0 = ClayKit_SpinnerAngle(&ctx, cfg);
    ASSERT_EQ_FLOAT(angle0, 0.0f, 0.001f);

    ctx.cursor_blink_time = 0.5f;
    float angle1 = ClayKit_SpinnerAngle(&ctx, cfg);
    ASSERT_EQ_FLOAT(angle1, 180.0f, 0.001f);

    ctx.cursor_blink_time = 1.0f;
    float angle2 = ClayKit_SpinnerAngle(&ctx, cfg);
    ASSERT_EQ_FLOAT(angle2, 0.0f, 1.0f); /* Full rotation wraps */

    TEST_PASS();
}

/* ============================================================================
 * Drawer Style Tests
 * ============================================================================ */

TEST(drawer_style_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_DrawerConfig cfg = {0};
    ClayKit_DrawerStyle style = ClayKit_ComputeDrawerStyle(&ctx, cfg);

    ASSERT_EQ(style.backdrop_color.r, 0);
    ASSERT_EQ(style.backdrop_color.a, 128);
    ASSERT_EQ(style.bg_color.r, theme.bg.r);
    ASSERT_EQ(style.border_color.r, theme.border.r);
    ASSERT_EQ(style.size, 300);
    ASSERT_EQ(style.z_index, 1000);
    ASSERT_EQ(style.padding, theme.spacing.lg);

    TEST_PASS();
}

TEST(drawer_style_custom_size) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_DrawerConfig cfg = { .size = 400 };
    ClayKit_DrawerStyle style = ClayKit_ComputeDrawerStyle(&ctx, cfg);

    ASSERT_EQ(style.size, 400);

    TEST_PASS();
}

TEST(drawer_style_custom_z_index) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_DrawerConfig cfg = { .z_index = 2000 };
    ClayKit_DrawerStyle style = ClayKit_ComputeDrawerStyle(&ctx, cfg);

    ASSERT_EQ(style.z_index, 2000);

    TEST_PASS();
}

/* ============================================================================
 * Popover Style Tests
 * ============================================================================ */

TEST(popover_style_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_PopoverConfig cfg = {0};
    ClayKit_PopoverStyle style = ClayKit_ComputePopoverStyle(&ctx, cfg);

    ASSERT_EQ(style.bg_color.r, theme.bg.r);
    ASSERT_EQ(style.border_color.r, theme.border.r);
    ASSERT_EQ(style.padding, theme.spacing.md);
    ASSERT_EQ(style.corner_radius, theme.radius.md);
    ASSERT_EQ(style.z_index, 50);

    TEST_PASS();
}

TEST(popover_style_custom_z_index) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_PopoverConfig cfg = { .z_index = 200 };
    ClayKit_PopoverStyle style = ClayKit_ComputePopoverStyle(&ctx, cfg);

    ASSERT_EQ(style.z_index, 200);

    TEST_PASS();
}

/* ============================================================================
 * Link Style Tests
 * ============================================================================ */

TEST(link_style_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_LinkConfig cfg = { .size = CLAYKIT_SIZE_MD };
    ClayKit_LinkStyle style = ClayKit_ComputeLinkStyle(&ctx, cfg);

    /* Default color is primary scheme color */
    ASSERT_EQ(style.text_color.r, theme.primary.r);
    ASSERT_EQ(style.text_color.g, theme.primary.g);
    ASSERT_EQ(style.font_size, theme.font_size.md);
    ASSERT_EQ(style.font_id, theme.font_id.body);
    ASSERT_EQ(style.underline_height, 1);

    TEST_PASS();
}

TEST(link_style_hover_darker) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_LinkConfig cfg = {0};
    ClayKit_LinkStyle style = ClayKit_ComputeLinkStyle(&ctx, cfg);

    /* Hover color should be darker than text color */
    ASSERT(style.hover_color.r < style.text_color.r ||
           style.hover_color.g < style.text_color.g ||
           style.hover_color.b < style.text_color.b);

    TEST_PASS();
}

TEST(link_style_disabled) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_LinkConfig cfg = { .disabled = true };
    ClayKit_LinkStyle style = ClayKit_ComputeLinkStyle(&ctx, cfg);

    /* Disabled color should be muted */
    ASSERT_EQ(style.disabled_color.r, theme.muted.r);
    ASSERT_EQ(style.disabled_color.g, theme.muted.g);

    TEST_PASS();
}

TEST(link_style_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_LinkConfig cfg_xs = { .size = CLAYKIT_SIZE_XS };
    ClayKit_LinkStyle style_xs = ClayKit_ComputeLinkStyle(&ctx, cfg_xs);

    ClayKit_LinkConfig cfg_xl = { .size = CLAYKIT_SIZE_XL };
    ClayKit_LinkStyle style_xl = ClayKit_ComputeLinkStyle(&ctx, cfg_xl);

    ASSERT(style_xl.font_size > style_xs.font_size);

    TEST_PASS();
}

TEST(link_style_color_scheme) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_LinkConfig cfg_primary = { .color_scheme = CLAYKIT_COLOR_PRIMARY };
    ClayKit_LinkStyle style_primary = ClayKit_ComputeLinkStyle(&ctx, cfg_primary);

    ClayKit_LinkConfig cfg_error = { .color_scheme = CLAYKIT_COLOR_ERROR };
    ClayKit_LinkStyle style_error = ClayKit_ComputeLinkStyle(&ctx, cfg_error);

    /* Different color schemes should produce different text colors */
    ASSERT(style_primary.text_color.r != style_error.text_color.r ||
           style_primary.text_color.g != style_error.text_color.g);

    TEST_PASS();
}

/* ============================================================================
 * Breadcrumb Style Tests
 * ============================================================================ */

TEST(breadcrumb_style_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_BreadcrumbConfig cfg = { .size = CLAYKIT_SIZE_MD };
    ClayKit_BreadcrumbStyle style = ClayKit_ComputeBreadcrumbStyle(&ctx, cfg);

    /* Default link color is primary */
    ASSERT_EQ(style.link_color.r, theme.primary.r);
    /* Current color is fg */
    ASSERT_EQ(style.current_color.r, theme.fg.r);
    /* Separator is muted */
    ASSERT_EQ(style.separator_color.r, theme.muted.r);
    ASSERT_EQ(style.font_size, theme.font_size.md);
    ASSERT(style.gap > 0);

    TEST_PASS();
}

TEST(breadcrumb_style_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_BreadcrumbConfig cfg_xs = { .size = CLAYKIT_SIZE_XS };
    ClayKit_BreadcrumbStyle style_xs = ClayKit_ComputeBreadcrumbStyle(&ctx, cfg_xs);

    ClayKit_BreadcrumbConfig cfg_xl = { .size = CLAYKIT_SIZE_XL };
    ClayKit_BreadcrumbStyle style_xl = ClayKit_ComputeBreadcrumbStyle(&ctx, cfg_xl);

    ASSERT(style_xl.font_size > style_xs.font_size);
    ASSERT(style_xl.gap > style_xs.gap);

    TEST_PASS();
}

TEST(breadcrumb_style_color_scheme) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_BreadcrumbConfig cfg_primary = { .color_scheme = CLAYKIT_COLOR_PRIMARY };
    ClayKit_BreadcrumbStyle style_primary = ClayKit_ComputeBreadcrumbStyle(&ctx, cfg_primary);

    ClayKit_BreadcrumbConfig cfg_success = { .color_scheme = CLAYKIT_COLOR_SUCCESS };
    ClayKit_BreadcrumbStyle style_success = ClayKit_ComputeBreadcrumbStyle(&ctx, cfg_success);

    ASSERT(style_primary.link_color.r != style_success.link_color.r ||
           style_primary.link_color.g != style_success.link_color.g);

    TEST_PASS();
}

TEST(breadcrumb_style_hover_darker) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_BreadcrumbConfig cfg = {0};
    ClayKit_BreadcrumbStyle style = ClayKit_ComputeBreadcrumbStyle(&ctx, cfg);

    ASSERT(style.hover_color.r < style.link_color.r ||
           style.hover_color.g < style.link_color.g ||
           style.hover_color.b < style.link_color.b);

    TEST_PASS();
}

/* ============================================================================
 * Accordion Style Tests
 * ============================================================================ */

TEST(accordion_style_bordered_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_AccordionConfig cfg = { .size = CLAYKIT_SIZE_MD };
    ClayKit_AccordionStyle style = ClayKit_ComputeAccordionStyle(&ctx, cfg);

    ASSERT_EQ(style.header_bg.r, theme.bg.r);
    ASSERT_EQ(style.header_text.r, theme.fg.r);
    ASSERT_EQ(style.active_accent.r, theme.primary.r);
    ASSERT_EQ(style.border_color.r, theme.border.r);
    ASSERT_EQ(style.font_size, theme.font_size.md);
    ASSERT(style.padding_x > 0);
    ASSERT(style.padding_y > 0);
    ASSERT_EQ(style.corner_radius, 0); /* Bordered has no corner radius */
    ASSERT_EQ(style.gap, 0); /* Bordered has no gap */

    TEST_PASS();
}

TEST(accordion_style_separated) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_AccordionConfig cfg = { .variant = CLAYKIT_ACCORDION_SEPARATED };
    ClayKit_AccordionStyle style = ClayKit_ComputeAccordionStyle(&ctx, cfg);

    ASSERT(style.corner_radius > 0); /* Separated has corner radius */
    ASSERT(style.gap > 0); /* Separated has gap between items */

    TEST_PASS();
}

TEST(accordion_style_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_AccordionConfig cfg_xs = { .size = CLAYKIT_SIZE_XS };
    ClayKit_AccordionStyle style_xs = ClayKit_ComputeAccordionStyle(&ctx, cfg_xs);

    ClayKit_AccordionConfig cfg_xl = { .size = CLAYKIT_SIZE_XL };
    ClayKit_AccordionStyle style_xl = ClayKit_ComputeAccordionStyle(&ctx, cfg_xl);

    ASSERT(style_xl.padding_x > style_xs.padding_x);
    ASSERT(style_xl.padding_y > style_xs.padding_y);
    ASSERT(style_xl.font_size > style_xs.font_size);

    TEST_PASS();
}

TEST(accordion_style_color_scheme) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_AccordionConfig cfg_primary = { .color_scheme = CLAYKIT_COLOR_PRIMARY };
    ClayKit_AccordionStyle style_primary = ClayKit_ComputeAccordionStyle(&ctx, cfg_primary);

    ClayKit_AccordionConfig cfg_success = { .color_scheme = CLAYKIT_COLOR_SUCCESS };
    ClayKit_AccordionStyle style_success = ClayKit_ComputeAccordionStyle(&ctx, cfg_success);

    ASSERT(style_primary.active_accent.r != style_success.active_accent.r ||
           style_primary.active_accent.g != style_success.active_accent.g);

    TEST_PASS();
}

TEST(accordion_style_hover_bg) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_AccordionConfig cfg = {0};
    ClayKit_AccordionStyle style = ClayKit_ComputeAccordionStyle(&ctx, cfg);

    /* Hover bg should differ from normal bg */
    ASSERT(style.header_hover_bg.r != style.header_bg.r ||
           style.header_hover_bg.g != style.header_bg.g ||
           style.header_hover_bg.b != style.header_bg.b);

    TEST_PASS();
}

/* ============================================================================
 * Menu Style Tests
 * ============================================================================ */

TEST(menu_style_default) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_MenuConfig cfg = { .size = CLAYKIT_SIZE_MD };
    ClayKit_MenuStyle style = ClayKit_ComputeMenuStyle(&ctx, cfg);

    ASSERT_EQ(style.bg_color.r, theme.bg.r);
    ASSERT_EQ(style.border_color.r, theme.border.r);
    ASSERT_EQ(style.text_color.r, theme.fg.r);
    ASSERT_EQ(style.disabled_text.r, theme.muted.r);
    ASSERT_EQ(style.separator_color.r, theme.border.r);
    ASSERT_EQ(style.font_size, theme.font_size.md);
    ASSERT(style.padding_x > 0);
    ASSERT(style.padding_y > 0);
    ASSERT_EQ(style.separator_height, 1);

    TEST_PASS();
}

TEST(menu_style_sizes) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_MenuConfig cfg_xs = { .size = CLAYKIT_SIZE_XS };
    ClayKit_MenuStyle style_xs = ClayKit_ComputeMenuStyle(&ctx, cfg_xs);

    ClayKit_MenuConfig cfg_xl = { .size = CLAYKIT_SIZE_XL };
    ClayKit_MenuStyle style_xl = ClayKit_ComputeMenuStyle(&ctx, cfg_xl);

    ASSERT(style_xl.padding_x > style_xs.padding_x);
    ASSERT(style_xl.padding_y > style_xs.padding_y);
    ASSERT(style_xl.font_size > style_xs.font_size);

    TEST_PASS();
}

TEST(menu_style_color_scheme) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_MenuConfig cfg_primary = { .color_scheme = CLAYKIT_COLOR_PRIMARY };
    ClayKit_MenuStyle style_primary = ClayKit_ComputeMenuStyle(&ctx, cfg_primary);

    ClayKit_MenuConfig cfg_error = { .color_scheme = CLAYKIT_COLOR_ERROR };
    ClayKit_MenuStyle style_error = ClayKit_ComputeMenuStyle(&ctx, cfg_error);

    /* hover_bg should differ between schemes */
    ASSERT(style_primary.hover_bg.r != style_error.hover_bg.r ||
           style_primary.hover_bg.g != style_error.hover_bg.g);

    TEST_PASS();
}

TEST(menu_style_separator_color) {
    ClayKit_Theme theme = CLAYKIT_THEME_LIGHT;
    ClayKit_State state_buf[4];
    ClayKit_Context ctx;
    ClayKit_Init(&ctx, &theme, state_buf, 4);

    ClayKit_MenuConfig cfg = {0};
    ClayKit_MenuStyle style = ClayKit_ComputeMenuStyle(&ctx, cfg);

    /* Separator color matches border */
    ASSERT_EQ(style.separator_color.r, theme.border.r);
    ASSERT_EQ(style.separator_color.g, theme.border.g);

    TEST_PASS();
}

/* ============================================================================
 * Main Test Runner
 * ============================================================================ */

int main(void) {
    printf("\n=== ClayKit Unit Tests ===\n\n");

    printf("State Management:\n");
    RUN_TEST(init_context);
    RUN_TEST(init_zeroes_state_buffer);
    RUN_TEST(get_state_empty);
    RUN_TEST(get_or_create_state_new);
    RUN_TEST(get_or_create_state_existing);
    RUN_TEST(get_state_after_create);
    RUN_TEST(state_capacity_limit);
    RUN_TEST(multiple_states);

    printf("\nFocus Management:\n");
    RUN_TEST(focus_initial_state);
    RUN_TEST(set_and_check_focus);
    RUN_TEST(clear_focus);
    RUN_TEST(focus_changed_detection);
    RUN_TEST(has_focus_different_elements);

    printf("\nTheme Helpers:\n");
    RUN_TEST(get_scheme_color_all);
    RUN_TEST(get_scheme_color_invalid_defaults_to_primary);
    RUN_TEST(get_spacing_all_sizes);
    RUN_TEST(get_spacing_invalid_defaults_to_md);
    RUN_TEST(get_font_size_all_sizes);
    RUN_TEST(get_radius_all_sizes);
    RUN_TEST(theme_light_values);
    RUN_TEST(theme_dark_values);

    printf("\nText Input:\n");
    RUN_TEST(input_handle_char_basic);
    RUN_TEST(input_handle_char_insert_middle);
    RUN_TEST(input_handle_char_capacity_limit);
    RUN_TEST(input_handle_char_rejects_control);
    RUN_TEST(input_backspace_delete_char);
    RUN_TEST(input_backspace_at_start);
    RUN_TEST(input_delete_char);
    RUN_TEST(input_delete_at_end);
    RUN_TEST(input_arrow_left);
    RUN_TEST(input_arrow_right);
    RUN_TEST(input_home_end);
    RUN_TEST(input_shift_select);
    RUN_TEST(input_delete_selection);
    RUN_TEST(input_type_replaces_selection);
    RUN_TEST(input_ctrl_left_word);
    RUN_TEST(input_ctrl_right_word);

    printf("\nLayout Primitives:\n");
    RUN_TEST(box_layout_padding);
    RUN_TEST(box_layout_defaults);
    RUN_TEST(box_layout_sizing);
    RUN_TEST(flex_layout_direction_and_gap);
    RUN_TEST(flex_layout_alignment);
    RUN_TEST(stack_layout_vertical);
    RUN_TEST(stack_layout_horizontal);
    RUN_TEST(center_layout_alignment);
    RUN_TEST(container_layout_max_width);
    RUN_TEST(container_layout_default_max_width);
    RUN_TEST(spacer_layout_grow);

    printf("\nBadge Styles:\n");
    RUN_TEST(badge_style_solid_default);
    RUN_TEST(badge_style_subtle);
    RUN_TEST(badge_style_outline);
    RUN_TEST(badge_style_sizes);

    printf("\nTag Styles:\n");
    RUN_TEST(tag_style_solid_default);
    RUN_TEST(tag_style_subtle);
    RUN_TEST(tag_style_outline);
    RUN_TEST(tag_style_sizes);
    RUN_TEST(tag_style_closeable);

    printf("\nStat Styles:\n");
    RUN_TEST(stat_style_default);
    RUN_TEST(stat_style_sizes);
    RUN_TEST(stat_style_custom_colors);

    printf("\nList Styles:\n");
    RUN_TEST(list_style_default);
    RUN_TEST(list_style_sizes);
    RUN_TEST(list_style_custom_colors);

    printf("\nTable Styles:\n");
    RUN_TEST(table_style_default);
    RUN_TEST(table_style_striped);
    RUN_TEST(table_style_bordered);
    RUN_TEST(table_style_sizes);
    RUN_TEST(table_style_color_scheme);

    printf("\nButton Styles:\n");
    RUN_TEST(button_bg_color_solid_normal);
    RUN_TEST(button_bg_color_solid_hovered);
    RUN_TEST(button_bg_color_outline);
    RUN_TEST(button_bg_color_disabled);
    RUN_TEST(button_text_color_solid);
    RUN_TEST(button_text_color_outline);
    RUN_TEST(button_border_width);
    RUN_TEST(button_padding_sizes);

    printf("\nProgress Styles:\n");
    RUN_TEST(progress_style_default);
    RUN_TEST(progress_style_sizes);

    printf("\nSlider Styles:\n");
    RUN_TEST(slider_style_default);
    RUN_TEST(slider_style_hovered);
    RUN_TEST(slider_style_disabled);

    printf("\nAlert Styles:\n");
    RUN_TEST(alert_style_subtle);
    RUN_TEST(alert_style_solid);
    RUN_TEST(alert_style_outline);

    printf("\nTooltip Styles:\n");
    RUN_TEST(tooltip_style_default);

    printf("\nTabs Styles:\n");
    RUN_TEST(tabs_style_line);
    RUN_TEST(tabs_style_enclosed);
    RUN_TEST(tabs_style_sizes);

    printf("\nModal Styles:\n");
    RUN_TEST(modal_style_default);
    RUN_TEST(modal_style_sizes);
    RUN_TEST(modal_style_custom_z_index);

    printf("\nInput Styles:\n");
    RUN_TEST(input_style_default);
    RUN_TEST(input_style_focused);
    RUN_TEST(input_style_custom_colors);

    printf("\nCheckbox/Switch Styles:\n");
    RUN_TEST(checkbox_size_values);
    RUN_TEST(checkbox_bg_color_states);
    RUN_TEST(switch_size_values);
    RUN_TEST(switch_bg_color_states);

    printf("\nRadio Styles:\n");
    RUN_TEST(radio_size_values);
    RUN_TEST(radio_bg_color_states);
    RUN_TEST(radio_border_color);

    printf("\nSelect Styles:\n");
    RUN_TEST(select_style_default);
    RUN_TEST(select_style_sizes);
    RUN_TEST(select_style_color_scheme);
    RUN_TEST(select_style_disabled);

    printf("\nSpinner Styles:\n");
    RUN_TEST(spinner_style_default);
    RUN_TEST(spinner_style_sizes);
    RUN_TEST(spinner_style_color_scheme);
    RUN_TEST(spinner_style_custom_speed);
    RUN_TEST(spinner_angle_advances);

    printf("\nDrawer Styles:\n");
    RUN_TEST(drawer_style_default);
    RUN_TEST(drawer_style_custom_size);
    RUN_TEST(drawer_style_custom_z_index);

    printf("\nPopover Styles:\n");
    RUN_TEST(popover_style_default);
    RUN_TEST(popover_style_custom_z_index);

    printf("\nLink Styles:\n");
    RUN_TEST(link_style_default);
    RUN_TEST(link_style_hover_darker);
    RUN_TEST(link_style_disabled);
    RUN_TEST(link_style_sizes);
    RUN_TEST(link_style_color_scheme);

    printf("\nBreadcrumb Styles:\n");
    RUN_TEST(breadcrumb_style_default);
    RUN_TEST(breadcrumb_style_sizes);
    RUN_TEST(breadcrumb_style_color_scheme);
    RUN_TEST(breadcrumb_style_hover_darker);

    printf("\nAccordion Styles:\n");
    RUN_TEST(accordion_style_bordered_default);
    RUN_TEST(accordion_style_separated);
    RUN_TEST(accordion_style_sizes);
    RUN_TEST(accordion_style_color_scheme);
    RUN_TEST(accordion_style_hover_bg);

    printf("\nMenu Styles:\n");
    RUN_TEST(menu_style_default);
    RUN_TEST(menu_style_sizes);
    RUN_TEST(menu_style_color_scheme);
    RUN_TEST(menu_style_separator_color);

    printf("\n=== Results ===\n");
    printf("Tests run:    %d\n", g_tests_run);
    printf("Tests passed: %d\n", g_tests_passed);
    printf("Tests failed: %d\n", g_tests_failed);

    if (g_tests_failed > 0) {
        printf("\nSOME TESTS FAILED!\n");
        return 1;
    }

    printf("\nALL TESTS PASSED!\n");
    return 0;
}
