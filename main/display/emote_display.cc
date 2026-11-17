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

    // ===== 待会在这里插入眼睛动画代码 =====

    return true;
}

void EmoteDisplay::SetExpression(const std::string &name)
{
    if (emote_handle_)
    {
        expression_emote_set(emote_handle_, name.c_str());
    }
}

void EmoteDisplay::Clear()
{
    if (emote_handle_)
    {
        expression_emote_clear(emote_handle_);
    }
}

}
