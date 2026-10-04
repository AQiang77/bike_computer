#include "screen_navigation.h"

#include "lvgl.h"

typedef struct
{
    lv_obj_t *connection_label;
    lv_obj_t *maneuver_label;
    lv_obj_t *distance_label;
    lv_obj_t *road_label;
    lv_obj_t *next_road_label;
    lv_obj_t *remaining_label;
} navigation_screen_t;

static navigation_screen_t g_screen;
static bool g_created;

static lv_obj_t *create_panel(lv_obj_t *parent, lv_coord_t x, lv_coord_t y,
                              lv_coord_t width, lv_coord_t height,
                              lv_color_t color)
{
    lv_obj_t *panel = lv_obj_create(parent);

    lv_obj_set_pos(panel, x, y);
    lv_obj_set_size(panel, width, height);
    lv_obj_set_style_radius(panel, 24, 0);
    lv_obj_set_style_bg_color(panel, color, 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    return panel;
}

static lv_obj_t *create_label(lv_obj_t *parent, const char *text,
                              const lv_font_t *font, lv_color_t color)
{
    lv_obj_t *label = lv_label_create(parent);

    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, color, 0);
    return label;
}

static const char *maneuver_text(navigation_maneuver_t maneuver)
{
    switch (maneuver)
    {
    case NAV_MANEUVER_LEFT:
        return "<";
    case NAV_MANEUVER_RIGHT:
        return ">";
    case NAV_MANEUVER_UTURN:
        return "U";
    case NAV_MANEUVER_ARRIVE:
        return "OK";
    case NAV_MANEUVER_STRAIGHT:
    default:
        return "^";
    }
}

rt_err_t screen_navigation_create(void)
{
    lv_obj_t *screen = lv_screen_active();
    lv_obj_t *header;
    lv_obj_t *turn_card;
    lv_obj_t *next_card;
    lv_obj_t *title;
    lv_obj_t *hint;

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x0B1018), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    header = create_panel(screen, 16, 14, 448, 54, lv_color_hex(0x151D29));
    title = create_label(header, "BIKE NAV", &lv_font_montserrat_20,
                         lv_color_hex(0xF5F7FA));
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 18, 0);

    g_screen.connection_label = create_label(header, "WAITING FOR PHONE",
                                              &lv_font_montserrat_16,
                                              lv_color_hex(0xFFB44A));
    lv_obj_align(g_screen.connection_label, LV_ALIGN_RIGHT_MID, -18, 0);

    turn_card = create_panel(screen, 16, 82, 448, 268, lv_color_hex(0x1769E0));
    g_screen.maneuver_label = create_label(turn_card, ">",
                                           &lv_font_montserrat_48,
                                           lv_color_hex(0xFFFFFF));
    lv_obj_set_size(g_screen.maneuver_label, 100, 100);
    lv_obj_set_style_text_align(g_screen.maneuver_label,
                                LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(g_screen.maneuver_label, LV_ALIGN_TOP_LEFT, 24, 35);

    hint = create_label(turn_card, "TURN IN", &lv_font_montserrat_16,
                        lv_color_hex(0xBFD8FF));
    lv_obj_align(hint, LV_ALIGN_TOP_LEFT, 146, 45);

    g_screen.distance_label = create_label(turn_card, "120 m",
                                           &lv_font_montserrat_36,
                                           lv_color_hex(0xFFFFFF));
    lv_obj_align(g_screen.distance_label, LV_ALIGN_TOP_LEFT, 142, 75);

    g_screen.road_label = create_label(turn_card, "Huancheng East Road",
                                       &lv_font_montserrat_24,
                                       lv_color_hex(0xFFFFFF));
    lv_obj_set_width(g_screen.road_label, 400);
    lv_label_set_long_mode(g_screen.road_label, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_align(g_screen.road_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(g_screen.road_label, LV_ALIGN_BOTTOM_MID, 0, -40);

    next_card = create_panel(screen, 16, 364, 448, 100, lv_color_hex(0x151D29));
    g_screen.next_road_label = create_label(next_card, "NEXT  Riverside Road",
                                            &lv_font_montserrat_16,
                                            lv_color_hex(0xAEBACC));
    lv_obj_set_width(g_screen.next_road_label, 410);
    lv_label_set_long_mode(g_screen.next_road_label, LV_LABEL_LONG_DOT);
    lv_obj_align(g_screen.next_road_label, LV_ALIGN_TOP_LEFT, 18, 17);

    g_screen.remaining_label = create_label(next_card, "8.6 km    24 min",
                                            &lv_font_montserrat_24,
                                            lv_color_hex(0xF5F7FA));
    lv_obj_align(g_screen.remaining_label, LV_ALIGN_BOTTOM_LEFT, 18, -15);
    g_created = true;
    return RT_EOK;
}

rt_err_t screen_navigation_update(const navigation_instruction_t *instruction)
{
    char text[48];

    if (instruction == RT_NULL)
    {
        return -RT_EINVAL;
    }
    if (!g_created)
    {
        return -RT_ERROR;
    }

    lv_label_set_text(g_screen.connection_label,
                      instruction->link_connected ? "PHONE CONNECTED"
                                                  : "WAITING FOR PHONE");
    lv_obj_set_style_text_color(g_screen.connection_label,
                                instruction->link_connected
                                    ? lv_color_hex(0x55E68A)
                                    : lv_color_hex(0xFFB44A),
                                0);
    lv_label_set_text(g_screen.maneuver_label,
                      maneuver_text(instruction->maneuver));

    if (instruction->next_distance_m >= 1000)
    {
        lv_snprintf(text, sizeof(text), "%u.%u km",
                    instruction->next_distance_m / 1000,
                    (instruction->next_distance_m % 1000) / 100);
    }
    else
    {
        lv_snprintf(text, sizeof(text), "%u m",
                    instruction->next_distance_m);
    }
    lv_label_set_text(g_screen.distance_label, text);
    lv_label_set_text(g_screen.road_label, instruction->current_road);
    lv_label_set_text_fmt(g_screen.next_road_label, "NEXT  %s",
                          instruction->next_road);
    lv_label_set_text_fmt(g_screen.remaining_label, "%u.%u km    %u min",
                          instruction->remaining_distance_m / 1000,
                          (instruction->remaining_distance_m % 1000) / 100,
                          instruction->remaining_time_s / 60);
    return RT_EOK;
}
