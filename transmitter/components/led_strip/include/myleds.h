#ifndef MYLEDS_H
#define MYLEDS_H

void myleds_init();

void myleds_display_rg(int r_percent_g);

void myleds_display_A(bool isOn);
void myleds_display_B(bool isOn);

#endif