#ifndef MYLEDS_H
#define MYLEDS_H

void myleds_init();

void myleds_display_rg(int r_percent_g);
void myleds_display_A(int mode);
void myleds_display_B(int mode);

void myleds_allRed();
void myleds_allOff();

#endif