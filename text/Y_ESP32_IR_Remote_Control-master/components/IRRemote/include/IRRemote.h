#ifndef IRREMOTE_H_
#define IRREMOTE_H_

#include <stdint.h>

void IRrecvInit(uint8_t pin, uint8_t port);

uint8_t IRrecvReadIR(void);

#endif
