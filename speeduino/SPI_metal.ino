#if defined (CORE_TEENSY35)
#include <kinetis.h>
#include <Arduino.h>
#include <HardwareSerial.h>

//#define USE_SPI2

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
#define CLEAR_SR 0x0 | SPI_SR_TCF | SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF

volatile uint8_t SPI0_CS = 0;  // 2 purposes, rcv isr tx-cs-used, is transmit active.
volatile uint8_t SPI1_CS = 0;  // single purpose, is transmit active.
#if defined (USE_SPI2)
volatile uint8_t SPI2_CS = 0;  // 2 purposes, rcv isr tx-cs-used, is transmit active.
#endif

void initSPI();
void sendSPI0(uint16_t, uint8_t);
void sendSPI1(uint16_t);
void spi0_isr();
void spi1_isr();

#if defined (USE_SPI2)
void spi2_isr();
void sendSPI2(uint16_t, uint8_t);
#endif

void initSPI()  // setup for 3 SPI instances and all available Teensy 3.5 pins
{
  // SPI0
  PORTC_PCR4 |= PORT_PCR_MUX(2);            // PORTC4 - SPI0_PCS0 (Teensy - 10) (CS0_1)
  PORTC_PCR3 |= PORT_PCR_MUX(2);            // PORTC3 - SPI0_PCS1 (Teensy - 9)  (CS0_2)
  PORTD_PCR5 |= PORT_PCR_MUX(2);            // PORTD5 - SPI0_PCS2 (Teensy - 20) (CS0_3)
//  PORTD_PCR6 |= PORT_PCR_MUX(2);            // PORTD6 - SPI0_PCS3 (Teensy - 21) (CS0_4)
//  PORTC_PCR0 |= PORT_PCR_MUX(2);            // PORTC0 - SPI0_PCS4 (Teensy - 15) (CS0_5)
  PORTD_PCR1 |= PORT_PCR_MUX(2);            // PORTD1 - SPI0_SCK  (Teensy - 14)
  PORTD_PCR2 |= PORT_PCR_MUX(2);            // PORTD2 - SPI0_SOUT (Teensy - 7)
  PORTD_PCR3 |= PORT_PCR_MUX(2);            // PORTD3 - SPI0_SIN  (Teensy - 8)
  // SPI0 ALT PINS
  //  PORTA_PCR14 |= PORT_PCR_MUX(2);            // PORTA14 - SPI0_PCS0 (Teensy - 26)
  //  PORTD_PCR0  |= PORT_PCR_MUX(2);            // PORTD0  - SPI0_PCS0 (Teensy - 2)
  //  PORTD_PCR4  |= PORT_PCR_MUX(2);            // PORTD4  - SPI0_PCS1 (Teensy - 6)
  //  PORTC_PCR2  |= PORT_PCR_MUX(2);            // PORTC2  - SPI0_PCS2 (Teensy - 23)
  //  PORTC_PCR1  |= PORT_PCR_MUX(2);            // PORTC1  - SPI0_PCS3 (Teensy - 22)
  //  PORTC_PCR5  |= PORT_PCR_MUX(2);            // PORTC5  - SPI0_SCK  (Teensy - 13)
  //  PORTA_PCR15 |= PORT_PCR_MUX(2);            // PORTA15 - SPI0_SCK  (Teensy - 27)
  //  PORTC_PCR6  |= PORT_PCR_MUX(2);            // PORTC6  - SPI0_SOUT (Teensy - 11
  //  PORTA_PCR16 |= PORT_PCR_MUX(2);            // PORTA16 - SPI0_SOUT (Teensy - 28)
  //  PORTC_PCR7  |= PORT_PCR_MUX(2);            // PORTC7 -  SPI0_SIN  (Teensy - 12)
  //  PORTA_PCR17 |= PORT_PCR_MUX(2);            // PORTA17 - SPI0_SIN  (Teensy - 39)

  // SPI1
  PORTB_PCR10 |= PORT_PCR_MUX(2);           // PORTB10 - SPI1_PCS0 (Teensy - 31)  (CS2_1)
                                            // NO SPI1_PCS1 or SPI1_PCS2 pins on Teensy 3.5 (SPI module supports the features)
  PORTB_PCR11 |= PORT_PCR_MUX(2);           // PORTB11 - SPI1_SCK  (Teensy - 32)
  PORTB_PCR16 |= PORT_PCR_MUX(2);           // PORTB16 - SPI1_SOUT (Teensy - 0)
  PORTB_PCR17 |= PORT_PCR_MUX(2);           // PORTB17 - SPI1_SIN  (Teensy - 1)
  // SPI1 ALT PINS
  // Be aware of Teensy pin overlaps on other SPI instances.
  //  PORTD_PCR4  |= PORT_PCR_MUX(7);            // PORTD4 - SPI1_PCS0 (Teensy - 6)
  //  PORTD_PCR6  |= PORT_PCR_MUX(7);            // PORTD6 - SPI1_SOUT (Teensy - 21)
  //  PORTD_PCR5  |= PORT_PCR_MUX(7);            // PORTD1 - SPI1_SCK  (Teensy - 20)
  //  PORTD_PCR7  |= PORT_PCR_MUX(7);            // PORTD3 - SPI1_SIN  (Teensy - 5)

  // SPI2 *** ALL ON POGO PINS ****
//  PORTB_PCR20 |= PORT_PCR_MUX(2);           // PORTB20 - SPI2_PCS0 (Teensy - 43) (CS2_1)
//  PORTD_PCR15 |= PORT_PCR_MUX(2);           // PORTD15 - SPI2_PCS1 (Teensy - 54) (CS2_2)
//  PORTB_PCR21 |= PORT_PCR_MUX(2);           // PORTB21 - SPI2_SCK  (Teensy - 46)
//  PORTB_PCR22 |= PORT_PCR_MUX(2);           // PORTB22 - SPI2_SOUT (Teensy - 44)
//  PORTB_PCR23 |= PORT_PCR_MUX(2);           // PORTB23 - SPI2_SIN  (Teensy - 45)
  // ALT PINS
  //  PORTD_PCR11  |= PORT_PCR_MUX(2);           // PORTD11 - SPI2_PCS0 (Teensy - 55)
                                                 // NO SPI2_PCS1
  //  PORTD_PCR12  |= PORT_PCR_MUX(2);           // PORTD12 - SPI2_SCK  (Teensy - 53)
  //  PORTD_PCR13  |= PORT_PCR_MUX(2);           // PORTD13 - SPI2_SOUT (Teensy - 52)
  //  PORTD_PCR14  |= PORT_PCR_MUX(2);           // PORTD14 - SPI2_SIN  (Teensy - 51)

  // CLOCK enable - allows SPI(x) to operate
  SIM_SCGC6 |= SIM_SCGC6_SPI0;              // Enable SPI0 clock 
  SIM_SCGC6 |= SIM_SCGC6_SPI1;              // Enable SPI1 clock 

  // Enable Instances - automatically disabled at reset
  SPI0_MCR &= ~SPI_MCR_MDIS;
  SPI1_MCR &= ~SPI_MCR_MDIS;

  // clear status register - void all pending interrupts
  SPI0_SR = CLEAR_SR;
  SPI1_SR = CLEAR_SR;

  // Setup Registers - Master mode only - don't use FIFO on SPI0 (single value transfer)
  SPI0_MCR  = SPI_MCR_MSTR | SPI_MCR_PCSIS(0x7) | SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF	| SPI_MCR_HALT; // PCIS 1-2-3 active low
  SPI1_MCR  = SPI_MCR_MSTR | SPI_MCR_PCSIS(0x1)	| SPI_MCR_HALT;  // PCIS1 active low

    // MC33810 parameters
  SPI0_CTAR0 = SPI_CTAR_FMSZ(15) | SPI_CTAR_PCSSCK(0) | SPI_CTAR_PASC(0) | SPI_CTAR_PDT(0) | SPI_CTAR_PBR(2) | SPI_CTAR_CSSCK(2) | SPI_CTAR_ASC(0) | SPI_CTAR_DT(5) | SPI_CTAR_BR(1);  // clock = F_BUS, BR = 3MHz
  // CTAR1 needs timing parameters [P]CSSK, [P]ASC and [P]DT set to suit Flash chip
  SPI0_CTAR1 = SPI_CTAR_FMSZ(7) | SPI_CTAR_PCSSCK(0) | SPI_CTAR_PASC(0) | SPI_CTAR_PDT(0) | SPI_CTAR_PBR(2) | SPI_CTAR_CSSCK(0) | SPI_CTAR_ASC(0) | SPI_CTAR_DT(0) | SPI_CTAR_BR(1);  // clock = F_BUS, BR = 3MHz
    // TPIC8101 parameters
  SPI1_CTAR0 = SPI_CTAR_FMSZ(7)  | SPI_CTAR_PCSSCK(0) | SPI_CTAR_PASC(1) | SPI_CTAR_PDT(1) | SPI_CTAR_PBR(2) | SPI_CTAR_CSSCK(0) | SPI_CTAR_ASC(0) | SPI_CTAR_DT(1) | SPI_CTAR_BR(1); // clock = F_BUS, BR = 3MHz

  // setup interrupts
  SPI0_RSER = 0x0 | SPI_RSER_EOQF_RE;   // 'Finished' Int Req Enable
  SPI1_RSER = 0x0 | SPI_RSER_EOQF_RE;   // 'Finished' Int Req Enable
  NVIC_ENABLE_IRQ(IRQ_SPI0);
  NVIC_ENABLE_IRQ(IRQ_SPI1);

#if defined (USE_SPI2)
  // SPI2 *** ALL ON POGO PINS ****
  PORTB_PCR20 |= PORT_PCR_MUX(2);           // PORTB20 - SPI2_PCS0 (Teensy - 43) (CS2_1)
  PORTD_PCR15 |= PORT_PCR_MUX(2);           // PORTD15 - SPI2_PCS1 (Teensy - 54) (CS2_2)
  PORTB_PCR21 |= PORT_PCR_MUX(2);           // PORTB21 - SPI2_SCK  (Teensy - 46)
  PORTB_PCR22 |= PORT_PCR_MUX(2);           // PORTB22 - SPI2_SOUT (Teensy - 44)
  PORTB_PCR23 |= PORT_PCR_MUX(2);           // PORTB23 - SPI2_SIN  (Teensy - 45)
  // ALT PINS
  //  PORTD_PCR11  |= PORT_PCR_MUX(2);           // PORTD11 - SPI2_PCS0 (Teensy - 55)
                                                 // NO SPI2_PCS1
  //  PORTD_PCR12  |= PORT_PCR_MUX(2);           // PORTD12 - SPI2_SCK  (Teensy - 53)
  //  PORTD_PCR13  |= PORT_PCR_MUX(2);           // PORTD13 - SPI2_SOUT (Teensy - 52)
  //  PORTD_PCR14  |= PORT_PCR_MUX(2);           // PORTD14 - SPI2_SIN  (Teensy - 51)
  SIM_SCGC3 |= SIM_SCGC3_SPI2;              // Enable SPI2 clock 
  SPI2_MCR &= ~SPI_MCR_MDIS;                // enable instance
  SPI2_SR = CLEAR_SR;                       // clear any pending interrupts
  SPI2_MCR  = SPI_MCR_MSTR | SPI_MCR_PCSIS(0x3)	| SPI_MCR_HALT; // PCIS 1-2 active low
  SPI2_RSER = 0x0 | SPI_RSER_EOQF_RE;       // 'Finished' Int Req Enable
  NVIC_ENABLE_IRQ(IRQ_SPI2);
#endif
}

void spi0_isr(void) // receive ISR
{
  SPI0_MCR |= SPI_MCR_HALT;

  // spi0 bus has three chips - 2 x MC333810 and Flash
  uint16_t val;
  uint8_t val1;
  switch (SPI0_CS)
  {
    case CS0_1:
      val = (uint16_t)SPI0_POPR; // 32 bit register
      if (val)
      {
        // do something relating to MC33810-1 return data
      }
      break;

    case CS0_2:
      val = (uint16_t)SPI0_POPR; // 32 bit register
      if (val)
      {
        // do something relating to MC33810-2 return data
      }
      break;

    case CS0_3:
      val1 = (uint8_t)SPI0_POPR;
      if (val1)
      {
        // do something relating to Flash return data
      }
      break;

    default:
      break;
  }
  SPI0_SR = CLEAR_SR;
  SPI0_CS = 0;  // frame complete
}

void spi1_isr(void) // receive ISR - only a single hardware cs available on Teensy 3.5 for SPI1
{
    SPI1_MCR |= SPI_MCR_HALT;
    uint8_t val = (uint8_t)SPI1_POPR; // 32 bit register
    if (val)
    {
      // do something relating to TPIC8101 return data
    }
    SPI1_SR |= SPI_SR_EOQF; // clear the interrupt flag (w1c)
    SPI1_CS = 0;
}

#if defined (USE_SPI2)
void spi2_isr(void) // receive ISR
{
  SPI2_MCR |= SPI_MCR_HALT;

  switch(SPI2_CS)
  {
    uint16_t val;
    case CS2_1:
      val = (uint16_t)SPI2_POPR; // 32 bit register
      if (val)
      {
       // do something relating to return data
      }
      break;
    case CS2_2:
      val = (uint16_t)SPI2_POPR;
      if (val)
      {
        // do something relating to return data
      }
      break;
    default:
      break;
  }

  SPI2_SR = CLEAR_SR;
  SPI2_CS = 0;
}
#endif

// assumes Flash only used during startup, leaving MC33810 operations unimpeded, except for any attempt to overlap transmits,
// in which case new transmit is paused until previous transmit completes.
void sendSPI0(uint16_t data, uint8_t chipSel)
{
  uint32_t pushVal = 0;
  // prevent new transmit before receive complete
  while (SPI0_CS != 0){} // cleared at end of receive isr

  SPI0_CS = chipSel; // tell receive isr what chip responsible for the interrupt

  switch(chipSel) // up to 5 peripherals
  {
    case CS0_1: // MC33810-1
    case CS0_2: // MC33810-2
      pushVal |= SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(chipSel) | SPI_PUSHR_EOQ | SPI_PUSHR_CTCNT | data; // MC33810
      break;
    case CS0_3: // Flash
      pushVal |= SPI_PUSHR_CTAS(1) | SPI_PUSHR_PCS(chipSel) | SPI_PUSHR_EOQ | SPI_PUSHR_CTCNT | data; // Flash
      break;
    case CS0_4: // spare
    case CS0_5: // spare
    default:
      break;
  }
  SPI0_PUSHR = pushVal; // load the value into the double buffered register for transmission
  SPI0_MCR &= START_INSTANCE;   // transmit
}

void sendSPI1(uint8_t val)  // support a single peripheral (Teensy 3.5 limitation)
{
  // prevent new transmit before receive complete
  while (SPI1_CS != 0){} // EOQF cleared at end of receive processing
  SPI1_CS = 1;
  SPI1_PUSHR |= SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ | SPI_PUSHR_CTCNT | SPI_PUSHR_PCS(1) | val;
  SPI1_MCR &= START_INSTANCE;
}

#if defined(USE_SPI2)
void sendSPI2(uint16_t data, uint8_t chipSel) // supports 2 peripherals with a single parameter set
{
  uint32_t pushVal = 0;
  // prevent new transmit before receive complete
  while (SPI2_CS != 0){} // cleared at end of receive isr
  SPI2_CS = chipSel; // tell receive isr what chip caused the interrupt
  pushVal |= SPI_PUSHR_CTAS(0) | SPI_PUSHR_PCS(chipSel) | SPI_PUSHR_EOQ | SPI_PUSHR_CTCNT | data;

  SPI2_PUSHR = pushVal; // load the value into the double buffered register for transmission
  SPI2_MCR &= START_INSTANCE;   // transmit
}
#endif
#endif
