/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	otg_device_stor.c
*
* Programmer:    	Grey
*
* Created: 	 		10/xx/2008
*
* Description: 		otg mass storage device function file
*              
*        
* Change History (most recent first):	2008.11.06
*****************************************************************/

#include "../public/type.h"
#include "../public/func.h"
#include "../usb/usb_ch9.h"
#include "../storage/stor.h"
#include "otg_config.h"
#include "otg_hal.h"
#include "otg.h"
#include "otg_device_stor.h"
#include <string.h>


/* ------------------------------ Mass Storage Protocol Declaration --------------------------------- */
extern WORD	gUsbBulkMaxPacketSize;		//bulk transfer max packet size
WORD	gPacketCount = 0;		//bulk transfer data packet counter
BYTE	*gDataBufferPtr = 0;	//bulk transfer data buffer pointer
WORD	gDataBufferLength = 0;	//bulk transfer data length

BYTE	gMsStatus = MS_ST_CBW;	//mass storage device status	
DWORD	gMsReqLBA = 0;			//mass storage request first LBA
WORD	gMsReqLBACount = 0;		//mass storage request LBA number
DWORD	gMsRemainLength = 0;	//mass storage request data length in bytes
WORD	gMsBlockLength = SYS_DATA_BLOCK_SIZE;		//mass storage block size i bytes

static	BYTE	CODE	gInquiryData[] = 
{
	0x00, 		//Direct Access Device
	0x80, 		//RMB
	0x02, 		//ISO/ECMA/ANSI
	0x02, 		//Response Data Format
	0x1F, 		//Additional Length
	0x00, 		//Reserved
	0x00, 		//Reserved
	0x00, 		//Reserved
	'M', 'V', 'S', 'i', 0, 0, 0, 0,				//Vendor Information
	'O', 'r', 'i', 'o', 'l', 'e', '_', 'S', 'O', 'C', 0, 0, 0, 0, 0, 0,		//Product Identification
	'0', '0', '7', 0		//Product Revision Level
};

static	BYTE	XDATA	gFmtCapacityData[] = 
{
	0x00, 0x00, 0x00, 0x08, 
	0x00, 0x1D, 0xC3, 0xFF, 
	0x03, 
	0x00, 0x02, 0x00,
};

static	BYTE	XDATA	gCapacityData[] = 
{
	0x00, 0x1D, 0xC3, 0xFF, 
	0x00, 0x00, 0x02, 0x00,
};

static	BYTE	CODE	gModeSenseProtectPage[] = {0x03, 0x00, 0x00, 0x00};
static	BYTE	XDATA	gModeSenseAllPage[] = 
{
	0x0B, 0x00, 0x00, 0x08, 
	0x00, 
	0x02, 0x00, 0x00, 		// block size 131072
	0x00, 
	0x00, 0x02, 0x00,
};

static 	BYTE	CODE	gRequestSense[] = {0xF0, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,};

BYTE	gCBW[64];
static	BYTE	XDATA	gCSW[] = {0x55, 0x53, 0x42, 0x53, 0x08, 0x70, 0xBA, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00,};
/* ------------------------------ Mass Storage Protocol Declaration End --------------------------------- */


/* ------------------------------ Mass Storage Request --------------------------------- */
extern VOID
OtgMs_ConfigCSW(
	BYTE	status
	)
{
	gCSW[12] = status;
}

extern VOID
OtgMs_Inquiry(VOID)
{
	gDataBufferPtr = gInquiryData;
	gDataBufferLength = sizeof(gInquiryData);

	OtgMs_ConfigCSW(0);
	gMsStatus = MS_ST_CSW;
}	


extern VOID
OtgMs_ReadFmtCapacity(VOID)
{
	gDataBufferPtr = gFmtCapacityData;
	gDataBufferLength = sizeof(gFmtCapacityData);

	OtgMs_ConfigCSW(0);
	gMsStatus = MS_ST_CSW;
}


extern VOID
OtgMs_ReadCapacity(VOID)
{
	gDataBufferPtr = gCapacityData;
	gDataBufferLength = sizeof(gCapacityData);

	OtgMs_ConfigCSW(0);
	gMsStatus = MS_ST_CSW;
}


extern VOID
OtgMs_Read10(VOID)
{
	// get LBA and size from CBW, set gMsReqLBA and gMsReqLBACount.

	((BYTE *)&gMsRemainLength)[0] = gCBW[11];
	((BYTE *)&gMsRemainLength)[1] = gCBW[10];
	((BYTE *)&gMsRemainLength)[2] = gCBW[9];
	((BYTE *)&gMsRemainLength)[3] = gCBW[8];

	((BYTE *)&gMsReqLBA)[0] = gCBW[17];
	((BYTE *)&gMsReqLBA)[1] = gCBW[18];		//get block count
	((BYTE *)&gMsReqLBA)[2] = gCBW[19];		//get block count
	((BYTE *)&gMsReqLBA)[3] = gCBW[20];		//get block count

	((BYTE *)&gMsReqLBACount)[0] = gCBW[22];
	((BYTE *)&gMsReqLBACount)[1] = gCBW[23];		//get block count

	gPacketCount = 0;

	// send first packet data
	gMsStatus = MS_ST_DATA_TX;
	//*/
}


extern VOID
OtgMs_Write10(VOID)
{
	// get LBA and size from CBW, set gMsReqLBA and gMsReqLBACount.

	((BYTE *)&gMsRemainLength)[0] = gCBW[11];
	((BYTE *)&gMsRemainLength)[1] = gCBW[10];
	((BYTE *)&gMsRemainLength)[2] = gCBW[9];
	((BYTE *)&gMsRemainLength)[3] = gCBW[8];

	((BYTE *)&gMsReqLBA)[0] = gCBW[17];
	((BYTE *)&gMsReqLBA)[1] = gCBW[18];		//get block count
	((BYTE *)&gMsReqLBA)[2] = gCBW[19];		//get block count
	((BYTE *)&gMsReqLBA)[3] = gCBW[20];		//get block count

	((BYTE *)&gMsReqLBACount)[0] = gCBW[22];
	((BYTE *)&gMsReqLBACount)[1] = gCBW[23];		//get block count

	gDataBufferPtr = (BYTE XDATA *)SYS_DATA_BUFFER_ADDRESS;
	gPacketCount = 0;

	gMsStatus = MS_ST_DATA_RX;
}


extern VOID
OtgMs_ModeSense(VOID)
{
	switch(gCBW[17])
	{
		case 0x1C:		//mode sense, Timer and Protect Page
			gDataBufferPtr = gModeSenseProtectPage;
			gDataBufferLength = sizeof(gModeSenseProtectPage);
			OtgMs_ConfigCSW(0);
			gMsStatus = MS_ST_CSW;
			break;

		case 0x3F:		//mode sense, Return all pages
			gDataBufferPtr = gModeSenseAllPage;
			gDataBufferLength = sizeof(gModeSenseAllPage);
			OtgMs_ConfigCSW(0);
			gMsStatus = MS_ST_CSW;
			break;

		default:
			break;
	}//end switch
}


extern VOID
OtgMs_TestUnitReady(VOID)
{
	OtgMs_ConfigCSW(0);
	gMsStatus = MS_ST_DONE;
}


extern VOID
OtgMs_MediumRemoval(VOID)
{
	if (gCBW[19] == 0)
		OtgMs_ConfigCSW(0);
	else
		OtgMs_ConfigCSW(1);

	gMsStatus = MS_ST_DONE;
}


extern VOID
OtgMs_RequestSense(VOID)
{
	gDataBufferPtr = gRequestSense;
	gDataBufferLength = sizeof(gRequestSense);

	OtgMs_ConfigCSW(0);
	gMsStatus = MS_ST_CSW;
}
/* ------------------------------ Mass Storage Request End --------------------------------- */


/* ------------------------------ Mass Storage Process --------------------------------- */
extern VOID
OtgStorDMALoadData(VOID)
{
	if ((gPacketCount % (SYS_DATA_BUFFER_LENGTH / gUsbBulkMaxPacketSize)) == 0)
	{
		// read data from flash to buffer
		OtgStorDataFlash2Buffer(SYS_DATA_BUFFER_ADDRESS, 
								gMsReqLBA, 
								(SYS_DATA_BUFFER_LENGTH / SYS_DATA_BLOCK_SIZE));
		gMsReqLBA += (SYS_DATA_BUFFER_LENGTH / SYS_DATA_BLOCK_SIZE);
		gDataBufferPtr = (BYTE XDATA *)SYS_DATA_BUFFER_ADDRESS;
	}

	// config DMA load data packet to FIFO
	WriteOTGReg32(otg_DMA_Addr, (WORD)gDataBufferPtr);
	WriteOTGReg32(otg_DMA_Count, gUsbBulkMaxPacketSize);
	SetBitOTGReg32(otg_DMA_Ctrl, (OTG_BULK_IN_EP << 4));
	SetBitOTGReg32(otg_DMA_Ctrl, (DMA_Ctrl_EnDMA | DMA_Ctrl_Direction | DMA_Ctrl_IntrEn));

	gDataBufferPtr += gUsbBulkMaxPacketSize;
	gPacketCount++;
}


extern VOID
OtgStorDMAUnloadData(VOID)
{
	// config DMA to unload data from FIFO to buffer, and send first packet, in this part gDataBufferLength is byte count
	WriteOTGReg32(otg_DMA_Addr, (WORD)gDataBufferPtr);
	WriteOTGReg32(otg_DMA_Count, gUsbBulkMaxPacketSize);
	SetBitOTGReg32(otg_DMA_Ctrl, (OTG_BULK_IN_EP << 4));
	SetBitOTGReg32(otg_DMA_Ctrl, (DMA_Ctrl_EnDMA | DMA_Ctrl_IntrEn));

	gDataBufferPtr += gUsbBulkMaxPacketSize;
	gPacketCount++;
}


extern VOID
OtgStorDMAProcess(VOID)
{
	WriteOTGReg32(otg_DMA_Ctrl, 0);		//clear DMA control bit

	if (gMsStatus == MS_ST_DATA_TX)		//Tx
	{
		SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prs_TxPktRdy);		//send packet

		gMsRemainLength -= gUsbBulkMaxPacketSize;
		if (gMsRemainLength == 0)
		{
			OtgMs_ConfigCSW(0);
			gMsStatus = MS_ST_CSW;		//send last packet, set status to CSW
		}
	}
	else if (gMsStatus == MS_ST_DATA_RX)		//Rx
	{
		ClrBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_prc_RxPktRdy);		//clear receive flag

		gMsRemainLength -= gUsbBulkMaxPacketSize;
		if (gMsRemainLength == 0)
		{
			// write last data from buffer to flash
			OtgStorDataBuffer2Flash(SYS_DATA_BUFFER_ADDRESS, 
									gMsReqLBA, 
									((gPacketCount * gUsbBulkMaxPacketSize + SYS_DATA_BLOCK_SIZE - 1) / SYS_DATA_BLOCK_SIZE));

			//send CSW
			OtgMs_ConfigCSW(0);
			SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prw_Mode);	//set Tx mode
			WriteOTGReg16(otg_IndexedCSR_TxMaxP, gUsbBulkMaxPacketSize);		//set max packet size, fix it!!!

			LoadFIFOData((BYTE *)&otg_EndpointFIFO[OTG_BULK_IN_EP], gCSW, sizeof(gCSW));
			SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prs_TxPktRdy);
			gMsStatus = MS_ST_DONE;		//get last packet, send CSW
		}
		else
		{
			if ((gPacketCount % (SYS_DATA_BUFFER_LENGTH / gUsbBulkMaxPacketSize)) == 0)
			{
				// data buffer full, write data from buffer to flash
				OtgStorDataBuffer2Flash(SYS_DATA_BUFFER_ADDRESS, 
										gMsReqLBA, 
										(SYS_DATA_BUFFER_LENGTH / SYS_DATA_BLOCK_SIZE));
				gMsReqLBA += (SYS_DATA_BUFFER_LENGTH / SYS_DATA_BLOCK_SIZE);
				gDataBufferPtr = (BYTE XDATA *)SYS_DATA_BUFFER_ADDRESS;
			}
		}
	}//end else if (gMsStatus == MS_ST_DATA_RX)
}


extern VOID
OtgStorRxProcess(VOID)
{
	WORD	recvCnt;

	switch (gMsStatus)
	{
		case MS_ST_CBW:
			recvCnt = ReadOTGReg16(otg_IndexedCSR_RxCount);
			UnloadFIFOData((BYTE *)&otg_EndpointFIFO[OTG_BULK_OUT_EP], gCBW, recvCnt);
			ClrBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_prc_RxPktRdy);		//clear receive flag

			if (memcmp(gCBW, "USBC", 4) != 0)
				return;
		
			memcpy(&gCSW[4], &gCBW[4], 4);		// set CSW tag

			switch(gCBW[15])
			{
				case INQUIRY:
					OtgMs_Inquiry();
					break;
				case READ_FORMAT_CAPACITY:
					OtgMs_ReadFmtCapacity();
					break;
				case READ_CAPACITY:
					OtgMs_ReadCapacity();
					break;
				case READ_10:
					OtgMs_Read10();
					break;
				case WRITE_10:
					OtgMs_Write10();
					break;
				case TEST_UNIT_READY:
					OtgMs_TestUnitReady();
					break;
				case MODE_SENSE:
					OtgMs_ModeSense();
					break;
				case ALLOW_MEDIUM_REMOVAL:
					OtgMs_MediumRemoval();
					break;
				case REQUEST_SENSE:
					OtgMs_RequestSense();
					break;
				default:
					OtgMs_ConfigCSW(1);
					gMsStatus = MS_ST_DONE;
					break;
			}// end switch(gCBW[15])
		
			if (gMsStatus != MS_ST_DATA_RX)
			{
				SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prw_Mode);	//set Tx mode
				WriteOTGReg16(otg_IndexedCSR_TxMaxP, gUsbBulkMaxPacketSize);		//set max packet size, fix it!!!
			}

			if (gMsStatus == MS_ST_DATA_TX)
			{
				OtgStorDMALoadData();
			}
			else if (gMsStatus == MS_ST_CSW)
			{
				// mcu load data from buffer to FIFO, and send it
				LoadFIFOData((BYTE *)&otg_EndpointFIFO[OTG_BULK_IN_EP], gDataBufferPtr, gDataBufferLength);
				SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prs_TxPktRdy);
			}
			else if (gMsStatus == MS_ST_DONE)
			{
				LoadFIFOData((BYTE *)&otg_EndpointFIFO[OTG_BULK_IN_EP], gCSW, sizeof(gCSW));
				SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prs_TxPktRdy);
			}
			break;

		case MS_ST_DATA_RX:
			OtgStorDMAUnloadData();
			break;

		default:
			break;
	}//end switch
}


extern VOID
OtgStorTxProcess(VOID)
{
	switch (gMsStatus)
	{
		case MS_ST_DATA_TX:
			OtgStorDMALoadData();
			break;

		case MS_ST_CSW:
			// mcu load data from buffer to FIFO, and send it
			LoadFIFOData((BYTE *)&otg_EndpointFIFO[OTG_BULK_IN_EP], gCSW, sizeof(gCSW));
			SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prs_TxPktRdy);
			gMsStatus = MS_ST_DONE;
			break;

		case MS_ST_DONE:
			// config endpoint to Rx mode
			ClrBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prw_Mode);	//set Rx mode
			WriteOTGReg16(otg_IndexedCSR_RxMaxP, gUsbBulkMaxPacketSize);
			gMsStatus = MS_ST_CBW;
			break;

		default:
			break;
	}//end switch
}


extern VOID
OtgStorInit(
	DWORD		blockNum
	)
{
	// init global patameter
	gMsStatus = MS_ST_CBW;	//mass storage device status	

	// init storage parameter
	gFmtCapacityData[4] = ((BYTE *)&blockNum)[0];
	gFmtCapacityData[5] = ((BYTE *)&blockNum)[1];
	gFmtCapacityData[6] = ((BYTE *)&blockNum)[2];
	gFmtCapacityData[7] = ((BYTE *)&blockNum)[3];

	gModeSenseAllPage[5] = ((BYTE *)&blockNum)[1];
	gModeSenseAllPage[6] = ((BYTE *)&blockNum)[2];
	gModeSenseAllPage[7] = ((BYTE *)&blockNum)[3];

	blockNum -= 1;

	gCapacityData[0] = ((BYTE *)&blockNum)[0];
	gCapacityData[1] = ((BYTE *)&blockNum)[1];
	gCapacityData[2] = ((BYTE *)&blockNum)[2];
	gCapacityData[3] = ((BYTE *)&blockNum)[3];

	gFmtCapacityData[9] = 0;
	gFmtCapacityData[10] = ((BYTE *)&gMsBlockLength)[0];
	gFmtCapacityData[11] = ((BYTE *)&gMsBlockLength)[1];

	gCapacityData[4] = 0;
	gCapacityData[5] = 0;
	gCapacityData[6] = ((BYTE *)&gMsBlockLength)[0];
	gCapacityData[7] = ((BYTE *)&gMsBlockLength)[1];
}
/* ------------------------------ Mass Storage Process End --------------------------------- */

