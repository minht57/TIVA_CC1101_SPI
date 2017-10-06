/******************************************************************************
 *
 * www.payitforward.edu.vn
 *
 *****************************************************************************/

/*
 * cc1101_spi.h
 *      Author: PIF
 *      Editor: minht57
 */

#ifndef __CC1101_SPI_H__
#define __CC1101_SPI_H__
#include <stdint.h>
#include "CC1101_define.h"

#define NULL    0

//            |   TIVA C   |        |  MODULE CC1101 |
//            |         3v3|------->|VCC             |
//            |         GND|------->|GND             |
//            |         PB4|------->|CLK             |
//            |         PB6|------->|MISO            |
//            |         PB7|------->|MOSI            |
//            |         PE0|------->|CSN             |
//            |         PB2|------->|DO              |
//            |         PB5|------->|D2              |

#define CC1101_GDO0_PIN             GPIO_PIN_2
#define CC1101_GDO2_PIN             GPIO_PIN_5
#define CC1101_MOSI_PIN             GPIO_PIN_7
#define CC1101_MISO_PIN             GPIO_PIN_6
#define CC1101_SCLK_PIN             GPIO_PIN_4
#define CC1101_CS_PIN               GPIO_PIN_0
#define CC1101_GDO0_PORT            GPIO_PORTB_BASE
#define CC1101_GDO2_PORT            GPIO_PORTB_BASE
#define CC1101_MISO_PORT            GPIO_PORTB_BASE
#define CC1101_MOSI_PORT            GPIO_PORTB_BASE
#define CC1101_SCLK_PORT            GPIO_PORTB_BASE
#define CC1101_CS_PORT              GPIO_PORTE_BASE
#define CC1101_SPI                  SSI2_BASE

#define CC1101_MISO_AF              GPIO_PB6_SSI2RX
#define CC1101_MOSI_AF              GPIO_PB7_SSI2TX
#define CC1101_SCLK_AF              GPIO_PB4_SSI2CLK

#define CC1101_GDO0_CLK_ENABLE()    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB)
#define CC1101_GDO2_CLK_ENABLE()    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB)
#define CC1101_CS_CLK_ENABLE()      ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE)
#define CC1101_SCLK_CLK_ENABLE()    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB)
#define CC1101_MOSI_CLK_ENABLE()    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB)
#define CC1101_MISO_CLK_ENABLE()    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB)
#define CC1101_SPI_CLK_ENABLE()     ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2)

#define CC1101_SPI_IRQn             INT_SSI2
#define CC1101_GDO0_IRQn            INT_GPIOB
#define CC1101_GDO2_IRQn            INT_GPIOB

#define CC1101_CS_ASSERT()          ROM_GPIOPinWrite(CC1101_CS_PORT, CC1101_CS_PIN, 0)
#define CC1101_CS_DEASSERT()        ROM_GPIOPinWrite(CC1101_CS_PORT, CC1101_CS_PIN, CC1101_CS_PIN)

#define CC1101_SPI_BEGIN()          halSpiBegin()
#define CC1101_SPI_END()            CC1101_CS_DEASSERT()
#define CC1101_SPI_SOMI_VAL()       ROM_GPIOPinRead(CC1101_MISO_PORT, CC1101_MISO_PIN)

#define CC1101_MOSI_DOWN()          ROM_GPIOPinWrite(CC1101_MOSI_PORT, CC1101_MOSI_PIN, 0)
#define CC1101_MOSI_SET()           ROM_GPIOPinWrite(CC1101_MOSI_PORT, CC1101_MOSI_PIN, CC1101_MOSI_PIN)
#define CC1101_CLK_SET()            ROM_GPIOPinWrite(CC1101_SCLK_PORT, CC1101_SCLK_PIN, CC1101_SCLK_PIN)
#define CC1101_CLK_DOWN()           ROM_GPIOPinWrite(CC1101_SCLK_PORT, CC1101_SCLK_PIN, 0)
#define CC1101_MISO_GET()           ROM_GPIOPinRead(CC1101_MISO_PORT, CC1101_MISO_PIN)
#define MAP_UtilsDelay(a)           SysCtlDelay(SysCtlClockGet()/3000000*a);
#define SPI_BITBANG_DELAY_TIME      1

#define CC1101_MISO_INPUT()         ROM_GPIOPinTypeGPIOInput(CC1101_MISO_PORT,CC1101_MISO_PIN)
#define CC1101_MOSI_OUTPUT()        ROM_GPIOPinTypeGPIOOutput(CC1101_MOSI_PORT,CC1101_MOSI_PIN)
#define CC1101_SCLK_OUTPUT()        ROM_GPIOPinTypeGPIOOutput(CC1101_SCLK_PORT,CC1101_SCLK_PIN)
#define CC1101_GDO0_INPUT()         ROM_GPIOPinTypeGPIOInput(CC1101_GDO0_PORT,CC1101_GDO0_PIN);\
														GPIOPadConfigSet(CC1101_GDO0_PORT, CC1101_GDO0_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU)
#define CC1101_GDO2_INPUT()         ROM_GPIOPinTypeGPIOInput(CC1101_GDO2_PORT,CC1101_GDO2_PIN);\
														GPIOPadConfigSet(CC1101_GDO2_PORT, CC1101_GDO2_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU)
#define CC1101_CS_OUTPUT()          ROM_GPIOPinTypeGPIOOutput(CC1101_CS_PORT,CC1101_CS_PIN)
#define CC1101_GDO0_READ()          ROM_GPIOPinRead(CC1101_GDO0_PORT,CC1101_GDO0_PIN)
#define CC1101_GDO2_READ()          ROM_GPIOPinRead(CC1101_GDO2_PORT,CC1101_GDO2_PIN)

#define CC1101_MISO_FUNCTION()      {ROM_GPIOPinTypeSSI(CC1101_MISO_PORT,CC1101_MISO_PIN);\
								     ROM_GPIOPinConfigure(CC1101_MISO_AF);}
#define CC1101_MOSI_FUNCTION()      {ROM_GPIOPinTypeSSI(CC1101_MOSI_PORT,CC1101_MOSI_PIN);\
								     ROM_GPIOPinConfigure(CC1101_MOSI_AF);}
#define CC1101_SCLK_FUNCTION()      {ROM_GPIOPinTypeSSI(CC1101_SCLK_PORT,CC1101_SCLK_PIN);\
								     ROM_GPIOPinConfigure(CC1101_SCLK_AF);}

#define CC1101_SEND(x)              {ROM_SSIDataPut(CC1101_SPI, (x));             \
					                {                                             \
						              uint32_t ulTemp;                            \
						              ROM_SSIDataGet(CC1101_SPI, &ulTemp);        \
						              g_ucSPIReadVal = (uint8_t)(ulTemp & 0xFF);  \
					                }}
#define CC1101_WAIT()               while(ROM_SSIBusy(CC1101_SPI)){};
#define CC1101_GET()                g_ucSPIReadVal
	
void CC1101_wait(unsigned int cycles);
void CC1101_Setup(void);
void CC1101_WriteReg(uint8_t addr, uint8_t value);
void CC1101_WriteBurstReg(char addr, uint8_t *buffer, char count);
uint8_t CC1101_ReadReg(char addr);
void CC1101_ReadBurstReg(char addr, uint8_t *buffer, char count);
char CC1101_ReadStatus(char addr);
uint8_t CC1101_Strobe(char strobe);
void CC1101_PowerupReset(void);
uint8_t spiRegAccess(uint8_t addrByte, uint8_t writeValue);
void spiBurstFifoAccess(uint8_t addrByte, uint8_t *pData, uint8_t len);

#endif

/******************************************************************************
 * END OF CC1101_spi.h
 *****************************************************************************/
