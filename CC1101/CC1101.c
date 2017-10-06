/******************************************************************************
 *
 * www.payitforward.edu.vn
 *
 *****************************************************************************/

/*
 * CC1101.c
 *      Author: PIF
 *      Editor: minht57
 */

#include <stdint.h>
#include <stdbool.h>
#include "CC1101.h"

#define DelayMs(x) vTaskDelay(x/portTICK_PERIOD_MS)
#define NUM_RXBYTES  0x7F        // Mask "# of bytes" field in _RXBYTES
static void (*CC1101_callback)(void);
void CC1101_GDO_Handler(void);

radioMode_t CC1101_radioMode = RX_MODE;
uint8_t CC1101_GetRandom(void)
{
	static uint8_t randomState = 0;
	randomState = (uint8_t)CC1101_CSMA_MULTIPLER*randomState + (uint8_t)CC1101_CSMA_OFFSET;
	return randomState;
}

void cc1101_init(void)
{
	CC1101_Setup();			//configure SPI
	ROM_SysCtlDelay(ROM_SysCtlClockGet()/300);
	CC1101_PowerupReset();		//Reset CC1101
	ROM_SysCtlDelay(ROM_SysCtlClockGet()/300);
	cc1101_writesettings();	//Send configuration commands
	
	cc1101_setreceive();
	cc1101_gdo2_int_disable();
	ROM_IntMasterEnable();
}

void cc1101_gdo0_int_enable(void)
{
	ROM_IntEnable(CC1101_GDO0_IRQn);
	GPIOIntRegister(CC1101_GDO0_PORT,&CC1101_GDO_Handler);
	ROM_GPIOIntTypeSet(CC1101_GDO0_PORT,CC1101_GDO0_PIN,GPIO_FALLING_EDGE);
	GPIOIntClear(CC1101_GDO0_PORT,CC1101_GDO0_PIN);
	GPIOIntEnable(CC1101_GDO0_PORT,CC1101_GDO0_PIN);
}

void cc1101_gdo0_int_disable(void)
{
	//ROM_IntEnable(CC1101_GDO0_IRQn);
	//GPIOIntRegister(CC1101_GDO0_PORT,&CC1101_GDO_Handler);
	//ROM_GPIOIntTypeSet(CC1101_GDO0_PORT,CC1101_GDO0_PIN,GPIO_RISING_EDGE);
	GPIOIntClear(CC1101_GDO0_PORT,CC1101_GDO0_PIN);
	GPIOIntDisable(CC1101_GDO0_PORT,CC1101_GDO0_PIN);
}

void cc1101_gdo2_int_enable(void)
{
	ROM_IntEnable(CC1101_GDO2_IRQn);
	GPIOIntRegister(CC1101_GDO2_PORT,&CC1101_GDO_Handler);
	ROM_GPIOIntTypeSet(CC1101_GDO2_PORT,CC1101_GDO2_PIN,GPIO_FALLING_EDGE);
	GPIOIntClear(CC1101_GDO2_PORT,CC1101_GDO2_PIN);
	GPIOIntEnable(CC1101_GDO2_PORT,CC1101_GDO2_PIN);
}

void cc1101_gdo2_int_disable(void)
{
	//ROM_IntEnable(CC1101_GDO0_IRQn);
	//GPIOIntRegister(CC1101_GDO0_PORT,&CC1101_GDO_Handler);
	//ROM_GPIOIntTypeSet(CC1101_GDO0_PORT,CC1101_GDO0_PIN,GPIO_RISING_EDGE);
	GPIOIntClear(CC1101_GDO2_PORT,CC1101_GDO2_PIN);
	GPIOIntDisable(CC1101_GDO2_PORT,CC1101_GDO2_PIN);
}

#define cc1101_gdo0_int_clear() GPIOIntClear(CC1101_GDO0_PORT, CC1101_GDO0_PIN);
#define cc1101_gdo2_int_clear() GPIOIntClear(CC1101_GDO2_PORT, CC1101_GDO2_PIN);

int8_t cc1101_sendpacket(uint8_t *txBuffer, char size)
{
	cc1101_gdo2_int_disable();
	CC1101_WriteBurstReg(CC1100_TXFIFO, txBuffer, size); // Write TX data
	CC1101_Strobe(CC1100_STX);           				// Change state to TX, initiating
                                            // data transfer

	while (!(CC1101_GDO0_READ()&CC1101_GDO0_PIN));
                                            // Wait GDO0 to go hi -> sync TX'ed
	while (CC1101_GDO0_READ()&CC1101_GDO0_PIN);
                                            // Wait GDO0 to clear -> end of pkt
//  GDO0_PxIFG &= ~GDO0_PIN;      			// After pkt TX, this flag is set.
                                            // Has to be cleared before existing
	CC1101_Strobe(CC1100_SIDLE);
	cc1101_gdo2_int_enable();
	return 0;
}

uint8_t cc1101_forceSend(uint8_t *txBuffer, char size)
{
	cc1101_gdo2_int_disable();
	cc1101_RxOff();
	CC1101_WriteReg(CC1100_TXFIFO, size);
	CC1101_WriteBurstReg(CC1100_TXFIFO, txBuffer, size); // Write TX data
	CC1101_Strobe(CC1100_STX);           				// Change state to TX, initiating
                                            // data transfer
	cc1101_gdo2_int_enable();
	return 1;
}

//uint8_t cc1101_CheckAndSend(uint8_t *txBuffer, char size)
//{
//	cc1101_gdo2_int_disable();

//	/* Turn off reciever. We can ignore/drop incoming packets during transmit. */
//	cc1101_RxOff();

//	CC1101_WriteBurstReg(CC1100_TXFIFO, txBuffer, size); // Write TX data
//	CC1101_GDO0_PA_PD();
//	CC1101_Strobe(CC1100_SRX);
//	DelayMs(1); // Wait RSSI valid
//	CC1101_WriteReg(CC1100_TXFIFO, size);
//	CC1101_WriteBurstReg(CC1100_TXFIFO, txBuffer, size); // Write TX data
//	cc1101_gdo2_int_clear();
//	CC1101_Strobe(CC1100_STX);           				// Change state to TX, initiating
//	DelayUsec(50);
//	if ((CC1101_CHECK_GDO0_INT()))
//	{
//		// CCA done
//		cc1101_gdo0_int_clear();
//		CC1101_GDO0_SYNC();
//		CC1101_CLEAR_GDO0_INT();      			// After pkt TX, this flag is set.
//		cc1101_gdo0_int_enable();
//	}
//	else
//	{
//		CC1101_GDO0_SYNC();
//		cc1101_gdo2_int_enable();
//		return 0;
//	}
//	return 1;
//}

char cc1101_receivepacket(uint8_t *rxBuffer, char *length)
{
  uint8_t status[2];
  char pktLen;
  char returnval = 0;
	char timeout = 5;
  uint8_t rxBytes;
  {
    uint8_t rxBytesVerify;

    rxBytesVerify = CC1101_ReadReg( CC1100_RXBYTES );
    do
    {
      rxBytes = rxBytesVerify;
      rxBytesVerify = CC1101_ReadReg( CC1100_RXBYTES );
			timeout--;
    }
    while ((rxBytes != rxBytesVerify) && timeout);
  }
	rxBytes &= NUM_RXBYTES;
  if ((rxBytes))
  {
    pktLen = CC1101_ReadReg(CC1100_RXFIFO); 			// Read length byte

    if (pktLen<64)                  // If pktLen size <= rxBuffer
    {
      CC1101_ReadBurstReg(CC1100_RXFIFO, rxBuffer, pktLen); // Pull data
      *length = pktLen;                     // Return the actual size

      CC1101_ReadBurstReg(CC1100_RXFIFO, status, 2);
                                            // Read appended status bytes
      if ((pktLen+1+2)!=rxBytes)
			{
		    // CC1101_Strobe(CC1100_SFRX);           // Flush RXFIFO
				returnval = 0;
      }
			else
			{
				 // Return CRC_OK bit
				returnval = (char)(status[LQI_RX] & CRC_OK);
			}
    }                                      
    else
    {
        *length = pktLen;                     // Return read size
        CC1101_Strobe(CC1100_SFRX);           // Flush RXFIFO
        ROM_SysCtlDelay(ROM_SysCtlClockGet()/3000);
    }

  }
  cc1101_setreceive();
  return returnval;							//1 = good, 0 = bad
}

char cc1101_pollreceiveflag(void)
{
	char returnval = 0;
	if(CC1101_GDO2_READ()&CC1101_GDO2_PIN)
	{
		
		while(CC1101_GDO2_READ() & CC1101_GDO2_PIN){}
		returnval = 1;
	}
	return returnval;
}

void cc1101_setreceive(void)
{
	CC1101_Strobe(CC1100_SRX);			//put into RX mode
}

void cc1101_setidle(void)
{
	CC1101_Strobe(CC1100_SIDLE);		//put int IDLE mode
}

void cc1101_setsleep(void)
{
	CC1101_Strobe(CC1100_SXOFF);		//turn off
}

short cc1101_getRSSI(void)
{
	unsigned char RSSI_dec;			//base ten reading
	short RSSI_dBm;			//dbm value
	const char RSSI_offset = 74;
	const unsigned char NEGATIVE = 128;

	//get RSSI
	RSSI_dec = CC1101_ReadStatus(CC1100_RSSI);
	//if negative, convert to dBm
	if (RSSI_dec >= NEGATIVE)
	{
		RSSI_dBm = (( RSSI_dec - 256) / 2) - RSSI_offset;
	}
	//If positive, convert to dBm
	else
	{
		RSSI_dBm = (RSSI_dec / 2) - RSSI_offset;
	}
	return RSSI_dBm;
}

void cc1101_RxOff(void)
{
  /*disable receive interrupts */
  cc1101_gdo2_int_disable();

  /* turn off radio */
  CC1101_Strobe( CC1100_SIDLE );
  while (CC1101_Strobe( CC1100_SNOP ) & 0xF0) ;

  /* flush the receive FIFO of any residual data */
  CC1101_Strobe( CC1100_SFRX );

  /* clear receive interrupt */
  cc1101_gdo2_int_disable();
}

uint8_t cc1101_calcCRC(uint8_t *buffer, uint8_t len)
{
    uint8_t sum = *buffer++;
	static uint8_t i;
	for (i = 1; i<len ;i++)
	{
		sum += *buffer++;
	}
	return ~sum;
}

uint8_t id,ver,chan;

void cc1101_writesettings(void)
{

	uint8_t PA[] =  {TXPower,TXPower,TXPower,TXPower,TXPower,TXPower,TXPower,TXPower};
	const unsigned char PA_LEN = 8;
	CC1101_WriteReg(CC1100_IOCFG2     ,0x06); 
	CC1101_WriteReg(CC1100_IOCFG0     ,0x09); 
	CC1101_WriteReg(CC1100_FSCTRL1    ,0x0C);
	CC1101_WriteReg(CC1100_FSCTRL0    ,0x00);
 /* 433 */

 /* 868Mhz */
 CC1101_WriteReg(CC1100_FREQ2      ,0x21);
 CC1101_WriteReg(CC1100_FREQ1      ,0x62);
 CC1101_WriteReg(CC1100_FREQ0    	 ,0x76);
 CC1101_WriteReg(CC1100_MDMCFG4    ,0xF5);
 CC1101_WriteReg(CC1100_DEVIATN    ,0x15);
 CC1101_WriteReg(CC1100_AGCCTRL2   ,0x03);
 CC1101_WriteReg(CC1100_AGCCTRL1   ,0x40);
 CC1101_WriteReg(CC1100_AGCCTRL0   ,0x91);

 CC1101_WriteReg(CC1100_MDMCFG3    ,0x83);
 CC1101_WriteReg(CC1100_MDMCFG2    ,0x13);
 CC1101_WriteReg(CC1100_MDMCFG1    ,0x22);
 CC1101_WriteReg(CC1100_MDMCFG0    ,0xF8);
 CC1101_WriteReg(CC1100_CHANNR     ,ChannelRF);
 CC1101_WriteReg(CC1100_FREND1     ,0xB6);
 CC1101_WriteReg(CC1100_FREND0     ,0x10);
 CC1101_WriteReg(CC1100_MCSM0      ,0x18);
 CC1101_WriteReg(CC1100_MCSM1      ,0x3F); // Only set in Master node
 CC1101_WriteReg(CC1100_FOCCFG     ,0x1D);
 CC1101_WriteReg(CC1100_BSCFG      ,0x1C);
 CC1101_WriteReg(CC1100_FSCAL3     ,0xEA);
 CC1101_WriteReg(CC1100_FSCAL2     ,0x2A);
 CC1101_WriteReg(CC1100_FSCAL1     ,0x00);
 CC1101_WriteReg(CC1100_FSCAL0     ,0x1F);
 CC1101_WriteReg(CC1100_FSTEST     ,0x59);
 CC1101_WriteReg(CC1100_TEST2      ,0x88);
 CC1101_WriteReg(CC1100_TEST1      ,0x31);
 CC1101_WriteReg(CC1100_TEST0      ,0x09);
 CC1101_WriteReg(CC1100_FIFOTHR    ,0x0D);
 CC1101_WriteReg(CC1100_PKTCTRL1   ,0x04);
 CC1101_WriteReg(CC1100_PKTCTRL0   ,0x45);
 CC1101_WriteReg(CC1100_ADDR       ,0x00);
 CC1101_WriteReg(CC1100_PKTLEN     ,0xFF);

	CC1101_WriteBurstReg(CC1100_PATABLE, PA, PA_LEN);
	
	// Test ID & Ver
	ver = CC1101_ReadReg(CC1100_VERSION);
	id = CC1101_ReadReg(CC1100_PARTNUM);
	chan = CC1101_ReadReg(CC1100_CHANNR);
}

void CC1101_GDO_Handler(void)
{
	if (GPIOIntStatus(CC1101_GDO2_PORT,false)&CC1101_GDO2_PIN)
	{
		if (CC1101_callback!=NULL)
		{
			CC1101_callback();
		}
		GPIOIntClear(CC1101_GDO2_PORT,CC1101_GDO2_PIN);
	}
}

void CC1101_set_handler(void (*Callback)())
{
	CC1101_callback = Callback;
}

/******************************************************************************
 * END OF CC1101.c
 *****************************************************************************/
