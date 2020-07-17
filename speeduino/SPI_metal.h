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

#if defined (USE_SPI2)
  volatile uint8_t SPI2_CS = 0;
#endif

#define TD 10
#define CH1_MASK 0x01
#define CH2_MASK 0x02
#define CH3_MASK 0x04
#define CH4_MASK 0x08
#define CH5_MASK 0x10
#define CH6_MASK 0x20
#define CH7_MASK 0x40
#define CH8_MASK 0x80
#define PIN_CHNG_CMD 0x3000   // MC33298 command

volatile uint8_t txHold0 = 0; // prevent attempts to overlap data transmit
volatile uint8_t txHold1 = 0;
volatile uint8_t txHold2 = 0;
volatile uint16_t pinCmdMC33298 = 0x3000;
volatile uint8_t pinSelectMC33298 = 0;

void initSPI();
void spi0_isr();
void spi1_isr();
void sendFlash(uint16_t);
void sendSPI1(uint16_t);
#if defined (USE_SPI2)
  void spi2_isr();
  void sendSPI2(uint16_t, uint8_t);
#endif

#define INJ_MC33298(CMD) \
  while(txHold0) {for (unsigned long t = micros(); (micros() - t) < TD;){};break;} \
  txHold0=1;\
  SPI0_PUSHR = (0x0 | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(CS0_1) | SPI_PUSHR_EOQ | SPI_PUSHR_CTCNT) | (PIN_CHNG_CMD | CMD); \
  SPI0_MCR &= ~SPI_MCR_HALT;

#define IGN_MC33298(CMD) \
  while(txHold0) {for (unsigned long t = micros(); (micros() - t) < TD;){};break;} \
  txHold0=1;\
  SPI0_PUSHR = (0x0 | SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(CS0_2) | SPI_PUSHR_EOQ | SPI_PUSHR_CTCNT) | (PIN_CHNG_CMD | CMD); \
  SPI0_MCR &= ~SPI_MCR_HALT;

#endif
#endif //SPI_METAL_H
