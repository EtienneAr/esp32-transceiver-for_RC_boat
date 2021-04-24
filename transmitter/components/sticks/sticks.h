#ifndef STICKS_H
#define STICKS_H
#include <stdbool.h>

#define STICKS_RESOLBIT 11
#define STICKS_VALUEMAX ((2<<(STICKS_RESOLBIT-1)) -1)

void sticks_init(bool autoTrimJoy); //if autotrimmed, init takes ~500ms during which the sticks should be at rest position

int sticks_readJoyA();
int sticks_readJoyB();

int sticks_readPotA();
int sticks_readPotB();

#endif