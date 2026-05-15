/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * Modified from led_strip_rmt_ws2812 example
 * - Changed GPIO pin and LED count
 * - Added color control functions
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "esp_log.h"
#include "esp_err.h"
#include <math.h>

// ========== 用户配置区 ==========
// WS2812 数据线连接的 GPIO 引脚号（根据你的接线修改）
#define LED_STRIP_GPIO_PIN      2

// 灯带上的 LED 数量（根据你的灯带修改）
#define LED_STRIP_LED_COUNT     100

// RMT 时钟分辨率 (Hz)，10MHz = 1 tick = 0.1us
#define LED_STRIP_RMT_RES_HZ    (10 * 1000 * 1000)

// LED 亮度 (0-255)，建议不超过 50 以减少电流和 RMT 中断压力
#define LED_BRIGHTNESS          255

// RMT 内存块大小（单位：words，4字节）
// 不使用 DMA 时建议适当增大，避免中断频繁
#define LED_STRIP_MEM_BLOCK_SYMBOLS 128
// ================================


static const char *TAG = "WS2812";

// LED strip 句柄
static led_strip_handle_t led_strip;

/**
 * @brief 初始化 LED strip
 */
static void configure_led(void)
{
    // LED strip 通用配置
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO_PIN,   // 数据线 GPIO
        .max_leds = LED_STRIP_LED_COUNT,        // LED 数量
        .led_model = LED_MODEL_WS2812,          // LED 型号
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, // WS2812 颜色顺序: GRB
        .flags = {
            .invert_out = false,                // 不反转输出信号
        }
    };

    // RMT 后端配置
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,         // 默认时钟源
        .resolution_hz = LED_STRIP_RMT_RES_HZ,  // RMT 计数器时钟频率
        .mem_block_symbols = LED_STRIP_MEM_BLOCK_SYMBOLS, // 增大内存块减少中断频率
        .flags = {
            .with_dma = false,                  // 不使用 DMA
        }
    };

    // 创建 LED strip 对象
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_LOGI(TAG, "LED strip 初始化完成 (GPIO:%d, LED数:%lu)", LED_STRIP_GPIO_PIN, (unsigned long)LED_STRIP_LED_COUNT);
}

/**
 * @brief 设置 10x10 矩阵全部 100 颗 LED 为指定颜色
 * @param red     红色分量 (0-255)
 * @param green   绿色分量 (0-255)
 * @param blue    蓝色分量 (0-255)
 */
static void set_all_leds(uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < LED_STRIP_LED_COUNT; i++) {
        ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, red, green, blue));
    }
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

void app_main(void)
{
    ESP_LOGI(TAG, "WS2812 10x10 矩阵 - 全部 100 颗 LED 亮绿色（亮度:%d）", LED_BRIGHTNESS);

    // 初始化 LED strip
    configure_led();

    // 全部 100 颗 LED 亮绿色，使用 LED_BRIGHTNESS 亮度
    set_all_leds(0, LED_BRIGHTNESS, 0);

    ESP_LOGI(TAG, "已设置全部 100 颗 LED 亮绿色（亮度: %d/255）", LED_BRIGHTNESS);

    // 程序保持运行，不做其他事情
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
