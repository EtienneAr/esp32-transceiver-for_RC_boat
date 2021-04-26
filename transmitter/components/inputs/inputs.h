#ifndef INPUTS_H
#define INPUTS_H
#include <stdbool.h>

#define INPUTS_RESOLBIT 11
#define INPUTS_VALUEMAX ((2<<(INPUTS_RESOLBIT-1)) -1)

void inputs_init(bool autoTrimJoy); //if autotrimmed, init takes ~500ms during which the sticks should be at rest position

float inputs_readJoyA();
float inputs_readJoyB();

int inputs_readPotA();
int inputs_readPotB();

bool inputs_readButtonA();
bool inputs_readButtonB();

#endif