#include "screen_handler.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_log.h"

static TFT_t dev;

static const char *SH_TAG = "Screen handler";

void init_screen_handler() {
	/* Init SPIFFS */
    ESP_LOGI(SH_TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 6,
        .format_if_mount_failed =true
    };

    // Use settings defined above toinitialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is anall-in-one convenience function.
    esp_err_t ret =esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(SH_TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(SH_TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(SH_TAG, "Failed to initialize SPIFFS (%s)",esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total,&used);
    if (ret != ESP_OK) {
        ESP_LOGE(SH_TAG,"Failed to get SPIFFS partition information (%s)",esp_err_to_name(ret));
    } else {
        ESP_LOGI(SH_TAG,"Partition size: total: %d, used: %d", total, used);
    }

    /* Init screen */
    spi_master_init(&dev, CONFIG_SCLK_GPIO, CONFIG_MOSI_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO, CONFIG_BL_GPIO);
    lcdInit(&dev, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_OFFSETX, CONFIG_OFFSETY);
}

/* Redo all drawing functions */

void mylcd_DrawPixel(uint16_t x, uint16_t y, uint16_t color) { lcdDrawPixel(&dev, x, y, color); }
void mylcd_DrawFillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) { lcdDrawFillRect(&dev, x1, y1, x2,  y2, color); }
void mylcd_DisplayOff() { lcdDisplayOff(&dev); }
void mylcd_DisplayOn() { lcdDisplayOn(&dev); }
void mylcd_FillScreen(uint16_t color) { lcdFillScreen(&dev, color); }
void mylcd_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) { lcdDrawLine(&dev, x1, y1, x2, y2, color); }
void mylcd_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) { lcdDrawRect(&dev, x1, y1, x2, y2, color); }
void mylcd_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) { lcdDrawCircle(&dev, x0, y0, r, color); }
void mylcd_DrawFillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) { lcdDrawFillCircle(&dev, x0, y0, r, color); }
void mylcd_DrawRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r, uint16_t color) { lcdDrawRoundRect(&dev, x1, y1, x2, y2, r, color); }
void mylcd_DrawArrow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t w, uint16_t color) { lcdDrawArrow(&dev, x0, y0, x1, y1, w, color); }
void mylcd_DrawFillArrow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t w, uint16_t color) { lcdDrawFillArrow(&dev, x0, y0, x1, y1, w, color); }
int mylcd_DrawChar(FontxFile *fx, uint16_t x, uint16_t y, uint8_t ascii, uint16_t color) { return lcdDrawChar(&dev, fx, x, y, ascii, color); }
int mylcd_DrawString(FontxFile *fx, uint16_t x, uint16_t y, uint8_t * ascii, uint16_t color) { return lcdDrawString(&dev, fx, x, y, ascii, color); }
void mylcd_SetFontDirection(uint16_t dir) { lcdSetFontDirection(&dev, dir); }
void mylcd_SetFontFill(uint16_t color) {  lcdSetFontFill(&dev, color); }
void mylcd_UnsetFontFill() {  lcdUnsetFontFill(&dev); }
void mylcd_SetFontUnderLine(uint16_t color) {  lcdSetFontUnderLine(&dev, color); }
void mylcd_UnsetFontUnderLine() { lcdUnsetFontUnderLine(&dev); }
void mylcd_BacklightOff() { lcdBacklightOff(&dev); }
void mylcd_BacklightOn() { lcdBacklightOn(&dev); }
void mylcd_InversionOn() { lcdInversionOn(&dev); }