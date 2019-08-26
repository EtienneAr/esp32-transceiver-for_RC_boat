#ifndef SCREEN_HANDLER_H
#define SCREEN_HANDLER_H

#include <stdio.h>
#include <stdlib.h>

#include "st7789.h"
#include "fontx.h"

void init_screen_handler();

/* Redo all drawing functions */

void mylcd_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void mylcd_DrawFillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void mylcd_DisplayOff();
void mylcd_DisplayOn();
void mylcd_FillScreen(uint16_t color);
void mylcd_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void mylcd_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void mylcd_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void mylcd_DrawFillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void mylcd_DrawRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r, uint16_t color);
void mylcd_DrawArrow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t w, uint16_t color);
void mylcd_DrawFillArrow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t w, uint16_t color);
int mylcd_DrawChar(FontxFile *fx, uint16_t x, uint16_t y, uint8_t ascii, uint16_t color);
int mylcd_DrawString(FontxFile *fx, uint16_t x, uint16_t y, uint8_t * ascii, uint16_t color);
void mylcd_SetFontDirection(uint16_t dir);
void mylcd_SetFontFill(uint16_t color);
void mylcd_UnsetFontFill();
void mylcd_SetFontUnderLine(uint16_t color);
void mylcd_UnsetFontUnderLine();
void mylcd_BacklightOff();
void mylcd_BacklightOn();
void mylcd_InversionOn();

uint16_t rgb565_conv(uint16_t r, uint16_t g, uint16_t b);

#endif