#if defined (BARRA)
//#define USE_SPI2  // SPI2 is all on Pogo Pins

#define CS0_1 1  // first instance of MC33810
#define CS0_2 2  // second instance of MC33810
#define CS0_3 4  // Flash
#define CS0_4 8  // available - not used
#define CS0_5 16 // available - not used
#if defined (USE_SPI2)
#define CS2_1 1
#define CS2_2 2
#endif

#define START_INSTANCE ~SPI_MCR_HALT
#define CLEAR_SR 0x0 | SPI_SR_TCF | SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF // w1c

volatile uint8_t SPI0_CS = 0;  // 2 purposes; rcv isr tx-cs-used, is transmit active.
volatile uint8_t SPI1_CS = 0;  // single purpose; is transmit active.
#if defined (USE_SPI2)
volatile uint8_t SPI2_CS = 0;  // 2 purposes; rcv isr tx-cs-used, is transmit active.
#endif

void initSPI();
//void sendSPI0(uint16_t, uint8_t);
void sendSPI1(uint16_t);
void spi0_isr();
void spi1_isr();
inline void sendSPIO0_1(uint16_t);
inline void sendSPIO0_2(uint16_t);
volatile uint16_t control1 = 0;
volatile uint16_t control2 = 0;

#if defined (USE_SPI2)
void spi2_isr();
void sendSPI2(uint16_t, uint8_t);
#endif

#endif
