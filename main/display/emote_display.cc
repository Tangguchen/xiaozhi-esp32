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

static void eye_anim_timer_cb(lv_timer_t *timer)
{
    EmoteDisplay *disp = static_cast<EmoteDisplay *>(timer->user_data);
    if (!disp) return;

    static int dir = 1;
    static int x_off = 0;
    const int max_move = 4;
    lv_obj_t *left_eye = disp->left_eye;
    lv_obj_t *right_eye = disp->right_eye;

    x_off += dir;
    if (x_off >= max_move) dir = -1;
    if (x_off <= -max_move) dir = 1;

    lv_obj_set_x(left_eye, -2 + x_off);
    lv_obj_set_x(right_eye, 2 + x_off);
}

namespace emote {

// ============================================================================
// Constants and Type Definitions
// ============================================================================

static constexpr const char *TAG = "emote";

EmoteDisplay::EmoteDisplay(lv_obj_t *parent)
{
    parent_ = parent;
    eye_container_ = nullptr;
    left_eye = nullptr;
    right_eye = nullptr;
    anim_timer = nullptr;
}

EmoteDisplay::~EmoteDisplay()
{
    if (anim_timer)
    {
        lv_timer_del(anim_timer);
        anim_timer = nullptr;
    }
}

bool EmoteDisplay::init()
{
    eye_container_ = lv_obj_create(parent_);
    lv_obj_remove_style_all(eye_container_);
    lv_obj_set_size(eye_container_, 128, 32);
    lv_obj_set_pos(eye_container_, 0, 0);
    lv_obj_set_flex_align(eye_container_, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_flex_flow(eye_container_, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(eye_container_, 12, 0);

    // 左眼
    left_eye = lv_obj_create(eye_container_);
    lv_obj_remove_style_all(left_eye);
    lv_obj_set_size(left_eye, 18, 18);
    lv_obj_set_style_radius(left_eye, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(left_eye, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(left_eye, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(left_eye, 0, 0);

    // 右眼
    right_eye = lv_obj_create(eye_container_);
    lv_obj_remove_style_all(right_eye);
    lv_obj_set_size(right_eye, 18, 18);
    lv_obj_set_style_radius(right_eye, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(right_eye, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(right_eye, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(right_eye, 0, 0);

    // 启动眼球左右晃动动画
    anim_timer = lv_timer_create(eye_anim_timer_cb, 50, this);
    return true;
}

void EmoteDisplay::set_state(const char *state)
{
    if (!eye_container_) return;

    expression_emote_set_emote_handle_status(state);
    if (strcmp(state, "idle") == 0)
    {
        // 待机：显示眼睛动画
        lv_obj_clear_flag(eye_container_, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        // 说话、聆听、思考：隐藏眼睛，使用官方表情
        lv_obj_add_flag(eye_container_, LV_OBJ_FLAG_HIDDEN);
    }
}

void EmoteDisplay::show_notification(const char *notification, int duration_ms)
{
    // 这里保留空实现，不改动原有逻辑
}

} // namespace emote
