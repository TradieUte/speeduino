#ifndef SPI_METAL_H
#define SPI_METAL_H

#if defined(SPI_METAL)

//#define USE_SPI2  // SPI2 is all on Pogo Pins

#define CS0_1 1  // first instance of MC33298
#define CS0_2 2  // second instance of MC33298
#define CS0_3 4  // Flash
#define CS0_4 8  // available - not used
#define CS0_5 16 // available - not used
#define CS1_1 1  // only a single cs available on Teensy
#if defined (USE_SPI2)
  #define CS2_1 1
  #define CS2_2 2
#endif

#define CLEAR_SR 0x0 | SPI_SR_TCF | SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF // w1c

#define TD 10

// ignition - hi nibble for MCMC33810 (GPGD Mode)
  // chip 1
#define IGN1_MASK 0x10
#define IGN2_MASK 0x20
#define IGN3_MASK 0x40
#define IGN4_MASK 0x80
  // chip 2
#define IGN5_MASK 0x10
#define IGN6_MASK 0x20
#define IGN7_MASK 0x40
#define IGN8_MASK 0x80

// injection - low nibble for MCMC33810
  // chip 1
#define INJ1_MASK 0x01
#define INJ2_MASK 0x02
#define INJ3_MASK 0x04
#define INJ4_MASK 0x08
  // chip 2
#define INJ5_MASK 0x01
#define INJ6_MASK 0x02
#define INJ7_MASK 0x04
#define INJ8_MASK 0x08

#define PIN_CHNG_CMD 0x3000   // MC33810 command

volatile uint8_t txHold0 = 0; // prevent attempts to overlap data transmit
volatile uint8_t txHold1 = 0;
volatile uint8_t txHold2 = 0;
volatile uint16_t pinCmdMC33298 = 0x3000;
volatile uint8_t pinSelectMC33298 = 0;
volatile uint8_t pinSelectMC33810_1;
volatile uint8_t pinSelectMC33810_2;
volatile uint8_t spi1Val;

void initSPI(void);
void spi0_isr(void);
void spi1_isr(void);
void sendSPI1(uint8_t);
void sendFlash(uint16_t);
#if defined (USE_SPI2)
  void spi2_isr(void);
  void sendSPI2(uint16_t, uint8_t);
#endif

// the spi0_isr sets txHold to 0 at end of interrupt processing
// the while() loop waits until the isr set txHold to 0, or TD microseconds (insurance against hanging)

// first chip, ign1-4, inj1-4
#define MC33810_1(CMD) \
  while(txHold0) {for (unsigned long t = micros(); (micros() - t) < TD;){};break;} \
  txHold0=1;\
  SPI0_PUSHR = (0x0 | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(CS0_1) | SPI_PUSHR_EOQ | SPI_PUSHR_CTCNT) | (PIN_CHNG_CMD | CMD); \
  SPI0_MCR &= ~SPI_MCR_HALT;

// second chip ign5-8, inj5-8
#define MC33810_2(CMD) \
  while(txHold0) {for (unsigned long t = micros(); (micros() - t) < TD;){};break;} \
  txHold0=1;\
  SPI0_PUSHR = (0x0 | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(CS0_2) | SPI_PUSHR_EOQ | SPI_PUSHR_CTCNT) | (PIN_CHNG_CMD | CMD); \
  SPI0_MCR &= ~SPI_MCR_HALT;

// case where two inj or ign are called at once, across two chips.
// There is a transmission delay between the two calls.
// would be better if one chip for ign and a second for inj, then this woud not be needed 
#define MC33810_3(CMD1,CMD2) \
  while(txHold0) {for (unsigned long t = micros(); (micros() - t) < TD;){};break;} \
  txHold0=1;\
  SPI0_PUSHR = (0x0 | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(CS0_1) | SPI_PUSHR_EOQ | SPI_PUSHR_CTCNT) | (PIN_CHNG_CMD | CMD1); \
  SPI0_MCR &= ~SPI_MCR_HALT;\
  while(txHold0) {for (unsigned long t = micros(); (micros() - t) < TD;){};break;} \
  txHold0=1;\
  SPI0_PUSHR = (0x0 | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(CS0_2) | SPI_PUSHR_EOQ | SPI_PUSHR_CTCNT) | (PIN_CHNG_CMD | CMD2); \
  SPI0_MCR &= ~SPI_MCR_HALT;

#endif
#endif //SPI_METAL_H
