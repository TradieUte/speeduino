#ifndef INIT_H
#define INIT_H
#if defined(SPI_METAL)
  #include "SPI_metal.h"
#endif

void initialiseAll();
void initialiseTriggers();
void setPinMapping(byte);

#endif