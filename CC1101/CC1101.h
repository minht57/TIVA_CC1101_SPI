/******************************************************************************
 *
 * www.payitforward.edu.vn
 *
 *****************************************************************************/

/*
 * CC1101.h
 *      Author: PIF
 *      Editor: minht57
 */

#ifndef __CC1101_H__
#define __CC1101_H__

#include <stdint.h>
#include "CC1101_spi.h"
#include "cc1101_define.h"
/*
 * If define FREQ_433MHz, carrier frequency is 433MHz else carrier frequency is 868MHz.
 */
#define FREQ_433MHz

typedef enum
{
	RX_MODE = 0,
	TX_MODE = 1,
}radioMode_t;

extern radioMode_t CC1101_radioMode;

#define CC1101_CSMA_OFFSET 64
#define CC1101_CSMA_MULTIPLER 107
#define CC1101_CSMA_DEFAULT_SEED 0
#define CC1101_CSMA_RETRIES 4

#define TXPower 0xC0
#define ChannelRF 0x01
#define RSSI_VALID_DELAY_US 1000
#define RF_RATE 1200
#define RF_LEN_TO_MS (8000/RF_RATE)

void cc1101_init(void);
int8_t cc1101_sendpacket_force(uint8_t *txBuffer, char size);
int8_t cc1101_sendpacket(uint8_t *txBuffer, char size);
char cc1101_receivepacket(uint8_t *rxBuffer, char *length);
char cc1101_pollreceiveflag(void);
void cc1101_setreceive(void);
void cc1101_setidle(void);
void cc1101_setsleep(void);
short cc1101_getRSSI(void);
void cc1101_RxOff(void);
uint8_t cc1101_calcCRC(uint8_t *buffer, uint8_t len);
void cc1101_writesettings(void);
uint8_t cc1101_CheckAndSend(uint8_t *txBuffer, char size);
uint8_t cc1101_forceSend(uint8_t *txBuffer, char size);

uint8_t CC1101_GetRandom(void);
void cc1101_gdo0_int_enable(void);
void cc1101_gdo0_int_disable(void);
void cc1101_gdo2_int_enable(void);
void cc1101_gdo2_int_disable(void);
void CC1101_set_handler(void (*Callback)());
#define CALC_TX_TIME(len) ((10+(len))*RF_LEN_TO_MS)

//-----------------------------------------------------------------------------
// CC110 Settings
//-----------------------------------------------------------------------------
#define CC1101_GDO0_SYNC() CC1101_WriteReg(CC1100_IOCFG0, CC1100_GDO_SYNC_PIN)
#define CC1101_GDO0_PA_PD() CC1101_WriteReg(CC1100_IOCFG0, CC1100_GDO_PA_PD)

#endif

/******************************************************************************
 * END OF CC1101.h
 *****************************************************************************/
