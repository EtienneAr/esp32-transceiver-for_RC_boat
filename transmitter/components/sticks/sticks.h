#ifndef STICKS_H
#define STICKS_H
#include <stdbool.h>

#define STICKS_RESOLBIT 11
#define STICKS_VALUEMAX ((2<<(STICKS_RESOLBIT-1)) -1)

void sticks_init(bool autoTrimOnCenter); //if autotrimmed, init takes ~500ms during which the sticks should be at rest position
int sticks_readA();
int sticks_readB();

#endif