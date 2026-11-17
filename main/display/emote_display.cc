#include "emote_display.h"

// Standard C++ headers
#include <algorithm>
#include <cinttypes>
#include <cstring>
#include <memory>
#include <tuple>
#include <unordered_map>

// Standard C headers
#include <sys/time.h>
#include <time.h>

// ESP-IDF headers
#include <esp_lcd_panel_io.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <lvgl.h>

// FreeRTOS headers
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Project headers
#include "assets.h"
#include "assets/lang_config.h"
#include "board.h"
#include "expression_emote.h"
#include "gfx.h"

static const char* TAG = "emote_display";
static lv_timer_t* g_eye_timer = nullptr;
static lv_obj_t* g_eye_container = nullptr;
static lv_obj_t* g_left_eye = nullptr;
static lv_obj_t* g_right_eye = nullptr;
static int g_anim_frame = 0;

static void eye_anim_timer_cb(lv_timer_t *timer)
{
    if (!g_left_eye || !g_right_eye) return;

    g_anim_frame++;
    int h;
    if (g_anim_frame % 35 < 30)
    {
        h = 12;
    }
    else
    {
        h = 2;
    }
    lv_obj_set_height(g_left_eye, h);
    lv_obj_set_height(g_right_eye, h);
}

namespace emote {

EmoteDisplay::EmoteDisplay()
{
    emote_handle_ = nullptr;
}

EmoteDisplay::~EmoteDisplay()
{
}

bool EmoteDisplay::init(lv_obj_t *parent)
{
    emote_handle_ = expression_emote_create(parent);
    if (!emote_handle_)
    {
        ESP_LOGE(TAG, "expression_emote create fail");
        return false;
    }

    // 只初始化一次，双重判断，防止重复创建
    if (g_eye_container == nullptr)
    {
        g_eye_container = lv_obj_create(parent);
        lv_obj_set_size(g_eye_container, 128, 64);
        lv_obj_set_pos(g_eye_container, 0, 0);
        lv_obj_set_style_bg_opa(g_eye_container, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(g_eye_container, 0, 0);

        g_left_eye = lv_obj_create(g_eye_container);
        lv_obj_set_size(g_left_eye, 14, 12);
        lv_obj_set_pos(g_left_eye, 30, 22);
        lv_obj_set_style_bg_color(g_left_eye, lv_color_white(), 0);
        lv_obj_set_style_radius(g_left_eye, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_border_width(g_left_eye, 0, 0);

        g_right_eye = lv_obj_create(g_eye_container);
        lv_obj_set_size(g_right_eye, 14, 12);
        lv_obj_set_pos(g_right_eye, 84, 22);
        lv_obj_set_style_bg_color(g_right_eye, lv_color_white(), 0);
        lv_obj_set_style_radius(g_right_eye, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_border_width(g_right_eye, 0, 0);
    }
    // 只创建一次定时器
    if (g_eye_timer == nullptr)
    {
        g_eye_timer = lv_timer_create(eye_anim_timer_cb, 50, nullptr);
    }
    return true;
}

void EmoteDisplay::SetStatus(const char * const status)
{
    if (!emote_handle_) return;

    expression_emote_set(emote_handle_, status);
    if(g_eye_container) lv_obj_add_flag(g_eye_container, LV_OBJ_FLAG_HIDDEN);
}

void EmoteDisplay::ShowNotification(const char *notification, int duration_ms)
{
    if (!emote_handle_) return;
    expression_emote_notify(emote_handle_, notification, duration_ms);
}

void EmoteDisplay::UpdateStatusBar(bool update_all)
{
    expression_emote_update_status_bar(emote_handle_, update_all);
}

void EmoteDisplay::clear()
{
    expression_emote_clear(emote_handle_);
    if(g_eye_container) lv_obj_clear_flag(g_eye_container, LV_OBJ_FLAG_HIDDEN);
}

}
