#include "CC1101_spi.h"

//#define ENTER_CRITICAL_SECTION() taskENTER_CRITICAL()
//#define EXIT_CRITICAL_SECTION() taskEXIT_CRITICAL()
#define ENTER_CRITICAL_SECTION() 
#define EXIT_CRITICAL_SECTION() 


uint8_t g_ucSPIReadVal = 0;

void CC1101_wait(uint32_t cycles)
{
  while(cycles>15)                          // 15 cycles consumed by overhead
    cycles = cycles - 6;                    // 6 cycles consumed each iteration
}

void CC1101_Setup(void)
{
	CC1101_GDO0_CLK_ENABLE();
	CC1101_GDO2_CLK_ENABLE();
	CC1101_MISO_CLK_ENABLE();
	CC1101_MOSI_CLK_ENABLE();
	CC1101_SCLK_CLK_ENABLE();
	CC1101_CS_CLK_ENABLE();
	
	CC1101_GDO0_INPUT();
	CC1101_GDO2_INPUT();
	CC1101_CS_OUTPUT();
	
	CC1101_MISO_FUNCTION();
	CC1101_MOSI_FUNCTION();
	CC1101_SCLK_FUNCTION();
	
	CC1101_SPI_CLK_ENABLE();
	
	ROM_SSIConfigSetExpClk(CC1101_SPI, ROM_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 500000, 8);
	CC1101_CS_ASSERT();
	ROM_SSIEnable(CC1101_SPI);
	{                                                                           
      uint32_t ulTemp;                                                   
      while(ROM_SSIDataGetNonBlocking(CC1101_SPI, &ulTemp))                    
      {                                                                       
      }                                                                       
	}    
	CC1101_CS_DEASSERT();
}

void CC1101_WriteReg(uint8_t addr, uint8_t value)
{
	ENTER_CRITICAL_SECTION();
	CC1101_CS_DEASSERT();
	CC1101_CS_ASSERT();
	CC1101_WAIT();
	CC1101_SEND(addr);
	CC1101_WAIT();
	CC1101_SEND(value);
	CC1101_WAIT();
	CC1101_CS_DEASSERT();
	EXIT_CRITICAL_SECTION();
	// spiRegAccess(addr,value);
}

void CC1101_WriteBurstReg(char addr, uint8_t *buffer, char count)
{
	spiBurstFifoAccess(addr| BURST_BIT, buffer, count);
}

uint8_t CC1101_ReadReg(char addr)
{
	return( spiRegAccess(addr | BURST_BIT | READ_BIT, DUMMY_BYTE) );
}

void CC1101_ReadBurstReg(char addr, uint8_t *buffer, char count)
{
	spiBurstFifoAccess(addr | BURST_BIT | READ_BIT, buffer, count);
}

char CC1101_ReadStatus(char addr)
{
	return( spiRegAccess(addr | BURST_BIT | READ_BIT, DUMMY_BYTE) );
}

uint8_t CC1101_Strobe(char strobe)
{
	uint8_t statusByte;
	CC1101_CS_DEASSERT();
	CC1101_CS_ASSERT();
	CC1101_SEND(strobe);
	CC1101_WAIT();
	CC1101_CS_DEASSERT();
	statusByte = CC1101_GET();
	return statusByte;
}

void CC1101_PowerupReset(void)
{
	CC1101_CS_DEASSERT();
	// vTaskDelay(1/portTICK_PERIOD_MS);
	ROM_SysCtlDelay(ROM_SysCtlClockGet()/3*5/1000000); // 5us
	CC1101_CS_ASSERT();
	ROM_SysCtlDelay(ROM_SysCtlClockGet()/3*10/1000000); // 10us
	// vTaskDelay(1/portTICK_PERIOD_MS);
	CC1101_CS_DEASSERT();
	ROM_SysCtlDelay(ROM_SysCtlClockGet()/3*40/1000000); // 10us
	// vTaskDelay(40/portTICK_PERIOD_MS);

	CC1101_CS_ASSERT();
	CC1101_WAIT();
	CC1101_SEND(CC1100_SRES);               // Send strobe
  // Strobe addr is now being TX'ed
	CC1101_WAIT();
	CC1101_CS_DEASSERT();
//	do
//	{
//		SPIWriteReg( CC1100_PKTLEN, 0xA5 );
//	}
//	while(!(SPIReadReg( CC1100_PKTLEN ) == 0xA5 )); /* SPI is not responding */
}

void spiBurstFifoAccess(uint8_t addrByte, uint8_t *pData, uint8_t len)
{
	ENTER_CRITICAL_SECTION();
	CC1101_CS_DEASSERT();
	CC1101_CS_ASSERT();
	do {
		CC1101_SEND(addrByte);
		CC1101_WAIT();
		do {
			CC1101_SEND(*pData);
			len --;
			CC1101_WAIT();
			if (addrByte & CC1100_READ_SINGLE)
			{
				*pData = CC1101_GET();
			}
			pData++;
			CC1101_CS_ASSERT();
		} while (len);
	} while (len);
	CC1101_CS_DEASSERT();
	EXIT_CRITICAL_SECTION();
}

uint8_t spiRegAccess(uint8_t addrByte, uint8_t writeValue)
{
	ENTER_CRITICAL_SECTION();
	uint8_t readValue;
	CC1101_CS_DEASSERT();
	CC1101_CS_ASSERT();
	CC1101_SEND(addrByte);
	CC1101_WAIT();
	CC1101_SEND(writeValue);
	CC1101_WAIT();
	readValue = CC1101_GET();
	EXIT_CRITICAL_SECTION();
	return(readValue);
}
