/****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2007, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	otg_device.c
*
* Programmer:    	Grey
*
* Created: 	 		09/xx/2008
*
* Description: 		OTG device controller application layer
*					
*              
*****************************************************************/

#include <string.h>
#include "../public/type.h"
#include "../public/func.h"
#include "otg_config.h"
#include "otg_hal.h"
#include "otg.h"

BYTE 	XDATA 	gRecvBuffer[64];
BYTE 	XDATA	gRecvLength = 0;
#define	TX_EP_INDEX					2
#define	RX_EP_INDEX					2

#if DEV_HIGH_SPEED_EN
#define	TX_EP_MAX_PACKET_SIZE		512
#define	RX_EP_MAX_PACKET_SIZE		512
#else
#define	TX_EP_MAX_PACKET_SIZE		64
#define	RX_EP_MAX_PACKET_SIZE		64
#endif

#define	DATA_BUFFER_ADDR			SYS_SHARE_BUF_BASE
#define	DATA_BUFFER_LENGTH			1024
BOOL	gIsSetupDone = 0;
BOOL	gIsDataTransport = 0;
BYTE	gDeviceAddress = 0;
BYTE	gDeviceStatus = 0;

#define	STOR_BLOCK_SIZE				512



#if DEV_HIGH_SPEED_EN
static	BYTE	CODE	gDeviceDescriptor[] = {0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40, 0x51, 0x09, 0x0B, 0x16,
												0x00, 0x01, 0x01, 0x02, 0x03, 0x01,};
static	BYTE	CODE	gConfigDescriptor[] = {0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00, 0x80, 
												0x32, 0x09, 0x04, 0x00, 0x00, 0x02, 0x08, 0x06, 
												0x50, 0x00, 0x07, 0x05, 0x82, 0x02, 0x00, 0x02,
												0xFF, 0x07, 0x05, 0x02, 0x02, 0x00, 0x02, 0xFF,};
#else
static	BYTE	CODE	gDeviceDescriptor[] = {0x12, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x40, 0x51, 0x09, 0x0B, 0x16,
												0x00, 0x01, 0x01, 0x02, 0x03, 0x01,};
static	BYTE	CODE	gConfigDescriptor[] = {0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00, 0x80, 
												0x32, 0x09, 0x04, 0x00, 0x00, 0x02, 0x08, 0x06, 
												0x50, 0x00, 0x07, 0x05, 0x82, 0x02, 0x40, 0x00,
												0xFF, 0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0xFF,};
#endif


static	BYTE	CODE	gStrDescriptorLID[] = {0x04, 0x03, 0x09, 0x04,};

static	BYTE	CODE	gStrDescriptorSerialNum[] = {0x1C, 0x03, 0x30, 0x00, 0x37, 0x00, 0x31, 0x00, 
														0x32, 0x00, 0x30, 0x00, 0x34, 0x00, 0x31, 0x00,
														0x32, 0x00, 0x33, 0x00, 0x33, 0x00, 0x31, 0x00, 
														0x35, 0x00, 0x35, 0x00,}; 

static	BYTE	CODE	gStrDescriptorProduct[] = {0x22, 0x03, 0x44, 0x00, 0x61, 0x00, 0x74, 0x00, 
														0x61, 0x00, 0x54, 0x00, 0x72, 0x00, 0x61, 0x00, 
														0x76, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x65, 0x00,
														0x72, 0x00, 0x32, 0x00, 0x2E, 0x00, 0x30, 0x00, 
														0x20, 0x00,};

static	BYTE	CODE	gInquiryData[] = {0x00, 0x80, 0x02, 0x02, 0x1F, 0x00, 0x00, 0x00, 
											0x4B, 0x69, 0x6E, 0x67, 0x73, 0x74, 0x6F, 0x6E, 
											0x44, 0x61, 0x74, 0x61, 0x54, 0x72, 0x61, 0x76,
											0x65, 0x6C, 0x65, 0x72, 0x32, 0x2E, 0x30, 0x20, 
											0x31, 0x2E, 0x30, 0x30,};

static	BYTE	CODE	gFmtCapacityData[] = {0x00, 0x00, 0x00, 0x08, 0x00, 0x1D, 0xC3, 0xFF, 0x03, 0x00, 0x02, 0x00,};

static	BYTE	CODE	gCapacityData[] = {0x00, 0x1D, 0xC3, 0xFF, 0x00, 0x00, 0x02, 0x00,};

static	BYTE	CODE	gModeSenseProtectPage[] = {0x03, 0x00, 0x00, 0x00};
static	BYTE	CODE	gModeSenseAllPage[] = {0x0B, 0x00, 0x00, 0x80, 0x00, 0x1D, 0xDB, 0x7F, 0x00, 0x00, 0x02, 0x00,};
static	BYTE	XDATA	gCSWData[] = {0x55, 0x53, 0x42, 0x53, 0x08, 0x70, 0xBA, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00,};

extern VOID
OtgDevProcessSetup(VOID);

extern VOID
OtgDevMassStorage(VOID);




extern BOOL
OtgDevOpen(VOID)
{
//	BYTE	DATA	i;	
#if CODY_FPGA_TEST_EN
	utmi_src_sel |= OTG_UTMI_IDDIG;
	utmi_src_sel_en |= OTG_UTMI_IDDIG;		//force B device

	utmi_src_sel |= OTG_UTMI_VBUSVALID;
	utmi_src_sel_en |= OTG_UTMI_VBUSVALID;		//force VBusValid
#else
	/* init otg phy into host mode */
	utmi_ctrl1 |= (OTG_UTMI_VBUSVALID/* | OTG_UTMI_SESSVALID | OTG_UTMI_SESSEND*/ | OTG_UTMI_SUSPENDM);		//VBUSVALID = 1
	utmi_ctrl1 |= OTG_UTMI_IDDIG;									//IDDIG = 1, B device
//	utmi_ctrl2 |= (OTG_UTMI_DP_PULLDOWN | OTG_UTMI_DM_PULLDOWN);	//DP,DM pull down
	utmi_src_sel1 |= (OTG_UTMI_VBUSVALID | OTG_UTMI_IDDIG/* | OTG_UTMI_SESSVALID | OTG_UTMI_SESSEND*/ | OTG_UTMI_SUSPENDM);		//mcu control these signal
//	utmi_src_sel2 |= (OTG_UTMI_DP_PULLDOWN | OTG_UTMI_DM_PULLDOWN);	
	utmi_ctrl3 &= ~OTG_UTMI_USB_PLAY_MODE;			//set USB read/write mode, using buffer in decoder
#endif

	if (DEV_HIGH_SPEED_EN)
		SetBitOTGReg8(otg_CommonUSB_Power, Power_hrw_HSEnab);
	else
		ClrBitOTGReg8(otg_CommonUSB_Power, Power_hrw_HSEnab);

//	SetBitOTGReg8(otg_CommonUSB_Power, Power_hrw_EnSuspendMode);	//otg core control phy SUSPENDM

	/* enable all usb interrupt */
	WriteOTGReg8(otg_CommonUSB_IntrUSBE, 0xFF);

	/* clear endpoint data toggle and flush fifo */
//	for (i = 1; i < MAX_HC_ENDPOINT_NUM; ++i)
//	{
//		WriteOTGReg8(otg_CommonUSB_Index, i);
//		SetBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_hs_ClrDataTog);	/* clear data toggle */
//		SetBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_hs_FlushFIFO);		/* flush FIFO */
//		SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_hs_ClrDataTog);	/* clear data toggle */
//		SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_hs_FlushFIFO);		/* flush FIFO */
//	}

	/* start a session */
//	SetBitOTGReg8(otg_CtrlFIFO_DevCtl, DevCtl_rw_Session);		/* check the ID pin sensing and verify the core and PHY IDDIG input. */

	
	WriteOTGReg8(otg_CommonUSB_FAddr, 0);	//set device address 0


	/* set endpoint number */
	WriteOTGReg8(otg_CommonUSB_Index, RX_EP_INDEX);

	/* set endpoint 2 Rx mode */
	ClrBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prw_Mode);

	/* set ClrDataTog for fisrt configuration */
	SetBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_ps_ClrDataTog);
	/* FIFO need flush */
	SetBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_ps_FlushFIFO);
	WriteOTGReg16(otg_IndexedCSR_RxMaxP, RX_EP_MAX_PACKET_SIZE);

//	WriteOTGReg8(otg_CommonUSB_Index, TX_EP_INDEX);
//	SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prw_Mode);
	/* set ClrDataTog for first configuration */
//	SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_ps_ClrDataTog);
	/* check FIFO data empty */
//	SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_ps_FlushFIFO);
//	WriteOTGReg16(otg_IndexedCSR_TxMaxP, TX_EP_MAX_PACKET_SIZE);

//	SetBitOTGReg16(otg_IndexedCSR_RxCSR, (RxCSR_prw_AutoClear | RxCSR_prw_DMAReqEnab | RxCSR_prw_DMAReqMode)); //config RXCSR
//	WriteOTGReg32(&otg_RqPktCount[RX_EP_INDEX], DATA_BUFFER_LENGTH / RX_EP_MAX_PACKET_SIZE);		

//	WriteOTGReg32(otg_DMA_Addr, DATA_BUFFER_ADDR);
//	WriteOTGReg32(otg_DMA_Count, DATA_BUFFER_LENGTH);
//	WriteOTGReg32(otg_DMA_Ctrl, 0);
//	SetBitOTGReg32(otg_DMA_Ctrl, (RX_EP_INDEX << 4));
//	SetBitOTGReg32(otg_DMA_Ctrl, (DMA_Ctrl_EnDMA | DMA_Ctrl_Mode | DMA_Ctrl_IntrEn));

	/* enable endpoint 0, 2 Rx interrupt */
//	SetBitOTGReg16(otg_CommonUSB_IntrTxEn, 0x0001);	//enable ep0 Rx/Tx interrupt
//	SetBitOTGReg16(otg_CommonUSB_IntrRxEn, (0x0001 << RX_EP_INDEX));	//enable ep2 Rx interrupt

	/* set softconn */
	SetBitOTGReg8(otg_CommonUSB_Power, Power_prw_SoftConn);

	return TRUE;
}


extern VOID
OtgDevRecvPacket(VOID)
{
	WORD	temp0, temp2;
	DWORD	tempL;

//	temp0 = ReadOTGReg16(otg_CommonUSB_IntrTx);		//read ep0 Rx/Tx interrupt flag
//	temp2 = ReadOTGReg16(otg_CommonUSB_IntrRx);		//read ep2 Rx interrupt flag
//	WriteOTGReg8(otg_CommonUSB_Index, RX_EP_INDEX);
//	temp3 = ReadOTGReg16(otg_IndexedCSR_RxCSR);
//	if (temp0 & 0x0001)		//ep0 interrupt
//	{
		WriteOTGReg8(otg_CommonUSB_Index, 0);
		temp0 = ReadOTGReg16(otg_IndexedCSR_CSR0);
		if (temp0 & CSR0_pr_RxPktRdy)	//ep0 receive one packet
		{
			temp0 = ReadOTGReg16(otg_IndexedCSR_Count0);	//get receive data length
			UnloadFIFOData((BYTE *)&otg_EndpointFIFO[0], gRecvBuffer, temp0);	//read data from fifo
			SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_ps_ServicedRxPktRdy);		//clear receive flag
			OtgDevProcessSetup();
			gRecvLength = temp0;
		}

//	}//end if (temp0 & 0x0001)		//ep0 interrupt
//	else if ((temp2 & (0x0001 << RX_EP_INDEX))/* || (temp3 & RxCSR_prc_RxPktRdy)*/)	//ep2 Rx interrupt
//	{
		WriteOTGReg8(otg_CommonUSB_Index, RX_EP_INDEX);
		if (gIsDataTransport)
		{
			tempL = ReadOTGReg32(otg_DMA_Intr);
			if (tempL & DMA_Intr_Ch1)		//DMA receive data
			{
//				ClrBitOTGReg32(otg_DMA_Ctrl, (DMA_Ctrl_EnDMA | DMA_Ctrl_Mode | DMA_Ctrl_IntrEn));
//				ClrBitOTGReg16(otg_IndexedCSR_RxCSR, (RxCSR_prw_AutoClear | RxCSR_prw_DMAReqEnab | RxCSR_prw_DMAReqMode)); //config RXCSR
				temp2 = DATA_BUFFER_LENGTH;
			}
		}
		else
		{
			temp2 = ReadOTGReg16(otg_IndexedCSR_RxCSR);
			if (temp2 & RxCSR_prc_RxPktRdy)
			{
				temp2 = ReadOTGReg16(otg_IndexedCSR_RxCount);	//get receive data length
//				memset(gRecvBuffer, 0, sizeof(gRecvBuffer));
				UnloadFIFOData((BYTE *)&otg_EndpointFIFO[RX_EP_INDEX], gRecvBuffer, temp2);	
				if (gRecvBuffer[15] != 0x2a)
					ClrBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_prc_RxPktRdy);		//clear receive flag
				OtgDevMassStorage();
				gRecvLength = temp2;
			}
		}
//	}//end else if (temp2 & 0x0004)	//ep2 Rx interrupt
}


extern VOID
OtgDevSendPacket(
	BYTE		epNum,
	BYTE		*buf,
	DWORD		length
	)
{
	WORD		temp;
	DWORD		tempL;

	if (epNum == 0)
	{
		WriteOTGReg8(otg_CommonUSB_Index, 0);		//set ep0 index
		if (length)
		{	
			LoadFIFOData((BYTE *)&otg_EndpointFIFO[0], buf, length);	//load packet data into fifo	
			SetBitOTGReg16(otg_IndexedCSR_CSR0, (CSR0_prs_TxPktRdy | CSR0_ps_DataEnd));		//send packet to host
		}
		else
		{
			SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_ps_DataEnd);		//send zero data packet to host
		}
		while(1)
		{
			temp = ReadOTGReg16(otg_IndexedCSR_CSR0);
			if ((temp & CSR0_prs_TxPktRdy) == 0)
				break;
		}
	}//end if (epNum == 0)
	else
	{
		WriteOTGReg8(otg_CommonUSB_Index, TX_EP_INDEX);		//set ep2 index
		SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prw_Mode);	//set ep2 Tx mode	
		WriteOTGReg16(otg_IndexedCSR_TxMaxP, TX_EP_MAX_PACKET_SIZE);		//set max packet size, fix it!!!
		if ((WORD)buf < SYS_SHARE_BUF_BASE)
		{
			LoadFIFOData((BYTE *)&otg_EndpointFIFO[TX_EP_INDEX], buf, length);
			SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prs_TxPktRdy);
			while(1)
			{
				temp = ReadOTGReg16(otg_IndexedCSR_TxCSR);
				if ((temp & TxCSR_prs_TxPktRdy) == 0)
					break;
			}
		}
		else
		{
			WriteOTGReg32(otg_DMA_Addr, (WORD)buf);
			WriteOTGReg32(otg_DMA_Count, length);
			WriteOTGReg32(otg_DMA_Ctrl, 0);
			SetBitOTGReg32(otg_DMA_Ctrl, (TX_EP_INDEX << 4));
			SetBitOTGReg32(otg_DMA_Ctrl, (DMA_Ctrl_EnDMA | DMA_Ctrl_Direction | DMA_Ctrl_Mode | DMA_Ctrl_IntrEn));

			SetBitOTGReg16(otg_IndexedCSR_TxCSR, (TxCSR_prw_AutoSet | TxCSR_prw_DMAReqEnab | TxCSR_prw_DMAReqMode)); //config TxCSR
			while(1)
			{
				tempL = ReadOTGReg32(otg_DMA_Intr);
				if ((tempL & DMA_Intr_Ch1) != 0)	
					break;
			}/* end while */
			ClrBitOTGReg16(otg_IndexedCSR_TxCSR, (TxCSR_prw_AutoSet | TxCSR_prw_DMAReqEnab | TxCSR_prw_DMAReqMode));
			ClrBitOTGReg32(otg_DMA_Ctrl, (DMA_Ctrl_EnDMA | DMA_Ctrl_Direction | DMA_Ctrl_Mode | DMA_Ctrl_IntrEn));
		}		
		ClrBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prw_Mode);	//set ep2 Rx mode
		WriteOTGReg16(otg_IndexedCSR_RxMaxP, RX_EP_MAX_PACKET_SIZE);
	}//end else
}


extern VOID
OtgDevProcessSetup(VOID)
{
	BYTE		temp;

	switch(gRecvBuffer[1])
	{
		case 0x00:		//GET_STATUS
			break;
		case 0x01:		//CLEAR_FEATURE
			break;
		case 0x03:		//SET_FEATURE
			break;

		case 0x05:		//SET_ADDRESS
			gDeviceAddress = gRecvBuffer[2];
			WriteOTGReg8(otg_CommonUSB_FAddr, gDeviceAddress);
			OtgDevSendPacket(0, NULL, 0);
			gDeviceStatus = 1;
			break;

		case 0x06:		//GET_DESCRIPTOR
			switch(gRecvBuffer[3])	
			{
				case 0x01:		//device descriptor
					temp = (gRecvBuffer[6] < sizeof(gDeviceDescriptor)) ? gRecvBuffer[6] : sizeof(gDeviceDescriptor);
					OtgDevSendPacket(0, gDeviceDescriptor, temp);
					break;

				case 0x02:		//config descriptor
					temp = (gRecvBuffer[6] < sizeof(gConfigDescriptor)) ? gRecvBuffer[6] : sizeof(gConfigDescriptor);
					OtgDevSendPacket(0, gConfigDescriptor, temp);
					break;

				case 0x03:		//string descriptor
					switch(gRecvBuffer[2])
					{
						case 0x00:		//language ID
							temp = (gRecvBuffer[6] < sizeof(gStrDescriptorLID)) ? gRecvBuffer[6] : sizeof(gStrDescriptorLID);
							OtgDevSendPacket(0, gStrDescriptorLID, temp);
							break;

						case 0x02:		//product ID
							temp = (gRecvBuffer[6] < sizeof(gStrDescriptorProduct)) ? gRecvBuffer[6] : sizeof(gStrDescriptorProduct);
							OtgDevSendPacket(0, gStrDescriptorProduct, temp);
							break;

						case 0x03:		//serial number
							temp = (gRecvBuffer[6] < sizeof(gStrDescriptorSerialNum)) ? gRecvBuffer[6] : sizeof(gStrDescriptorSerialNum);
							OtgDevSendPacket(0, gStrDescriptorSerialNum, temp);
							break;

						default:
							break;
					}//end switch(gRecvBuffer[2])
					break;

				default:
					break;
			}//end switch(gRecvBuffer[3])
			break;

		case 0x07:		//SET_DESCRIPTOR
 	    	break;
		case 0x08:		//GET_CONFIGURATION
			break;
		case 0x09:		//SET_CONFIGURATION
			if (gRecvBuffer[2] == 1)
			{
				gIsSetupDone = 1;
				OtgDevSendPacket(0, NULL, 0);
				gDeviceStatus = 2;
			}
			break;

		case 0x0a:		//GET_INTERFACE
			break;
		case 0x0b:		//SET_INTERFACE
			break;
		case 0x0c:		//SYNCH_FRAME
			break;

		case 0xFE:		//GET_MAX_LUN
			if (gRecvBuffer[0] == 0xA1)
			{
				OtgDevSendPacket(0, "\0", 1);
				break;
			}
			break;

		default:
			break;
	}//end switch(gRecvBuffer[1])
}


extern VOID
OtgDevReadTxData(
	WORD		buf,
	DWORD		lba,
	BYTE		size
	);

extern VOID
OtgDevWriteRxData(
	WORD		buf,
	DWORD		lba,
	BYTE		size
	);


extern VOID
OtgDevMassStorage(VOID)
{
	WORD		/*temp2, */bufAddr = DATA_BUFFER_ADDR;
	DWORD		tempL;
	DWORD		requestDataLength, opLength, startLba;

	if (memcmp(gRecvBuffer, "USBC", 4) == 0)
	{
		memcpy(&gCSWData[4], &gRecvBuffer[4], 4);		//copy CBW toggle data
		switch(gRecvBuffer[15])
		{
			case 0x12:		//inquiry
//				temp = (gRecvBuffer[19] < sizeof(gInquiryData)) ? gRecvBuffer[19] : sizeof(gInquiryData);			
				OtgDevSendPacket(TX_EP_INDEX, gInquiryData, sizeof(gInquiryData));
				OtgDevSendPacket(TX_EP_INDEX, gCSWData, sizeof(gCSWData));	//send CSW
				break;

			case 0x23:		//read format capacity
				OtgDevSendPacket(TX_EP_INDEX, gFmtCapacityData, sizeof(gFmtCapacityData));
				OtgDevSendPacket(TX_EP_INDEX, gCSWData, sizeof(gCSWData));	//send CSW
				break;
	
			case 0x25:		//read capacity
				OtgDevSendPacket(TX_EP_INDEX, gCapacityData, sizeof(gCapacityData));
				OtgDevSendPacket(TX_EP_INDEX, gCSWData, sizeof(gCSWData));	//send CSW
				break;

			case 0x28:		//read lba
				((BYTE *)&requestDataLength)[0] = gRecvBuffer[11];
				((BYTE *)&requestDataLength)[1] = gRecvBuffer[10];		//get block count
				((BYTE *)&requestDataLength)[2] = gRecvBuffer[9];		//get block count
				((BYTE *)&requestDataLength)[3] = gRecvBuffer[8];		//get block count

				((BYTE *)&startLba)[0] = gRecvBuffer[17];
				((BYTE *)&startLba)[1] = gRecvBuffer[18];		//get block count
				((BYTE *)&startLba)[2] = gRecvBuffer[19];		//get block count
				((BYTE *)&startLba)[3] = gRecvBuffer[20];		//get block count

				while(requestDataLength)
				{
					opLength = (requestDataLength < DATA_BUFFER_LENGTH) ? requestDataLength : DATA_BUFFER_LENGTH;
					OtgDevReadTxData(DATA_BUFFER_ADDR, startLba, opLength / STOR_BLOCK_SIZE);
					OtgDevSendPacket(TX_EP_INDEX, (BYTE XDATA *)DATA_BUFFER_ADDR, opLength);
					startLba += opLength / STOR_BLOCK_SIZE;
					requestDataLength -= opLength;
				}//end while
				OtgDevSendPacket(TX_EP_INDEX, gCSWData, sizeof(gCSWData));	//send CSW
				break;

			case 0x2A:		//write lba
				((BYTE *)&requestDataLength)[0] = gRecvBuffer[11];
				((BYTE *)&requestDataLength)[1] = gRecvBuffer[10];		//get block count
				((BYTE *)&requestDataLength)[2] = gRecvBuffer[9];		//get block count
				((BYTE *)&requestDataLength)[3] = gRecvBuffer[8];		//get block count

				((BYTE *)&startLba)[0] = gRecvBuffer[17];
				((BYTE *)&startLba)[1] = gRecvBuffer[18];		//get block count
				((BYTE *)&startLba)[2] = gRecvBuffer[19];		//get block count
				((BYTE *)&startLba)[3] = gRecvBuffer[20];		//get block count

				while(requestDataLength)
				{
#if 1
//					memset((BYTE XDATA *)bufAddr, 0, DATA_BUFFER_LENGTH);
//					temp = ReadOTGReg16(otg_IndexedCSR_RxCSR);
					opLength = (requestDataLength < DATA_BUFFER_LENGTH) ? requestDataLength : DATA_BUFFER_LENGTH;

					WriteOTGReg8(otg_CommonUSB_Index, RX_EP_INDEX);		//set ep2 index
//					ClrBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prw_Mode);
//					WriteOTGReg16(otg_IndexedCSR_RxMaxP, RX_EP_MAX_PACKET_SIZE);

//					ClrBitOTGReg16(otg_IndexedCSR_RxMaxP, 0xF800);
//					if (RX_EP_MAX_PACKET_SIZE == 512)	
//						SetBitOTGReg16(otg_IndexedCSR_RxMaxP, (0x0000 << 11));	
//					else if (RX_EP_MAX_PACKET_SIZE == 64)
//						SetBitOTGReg16(otg_IndexedCSR_RxMaxP, (0x0007 << 11));

					/* config CSR */
//					SetBitOTGReg16(otg_CommonUSB_IntrRxEn, (0x0001 << RX_EP_INDEX));	//disable Rx interrupt
					SetBitOTGReg16(otg_IndexedCSR_RxCSR, (RxCSR_prw_AutoClear | RxCSR_prw_DMAReqEnab | RxCSR_prw_DMAReqMode)); //config RXCSR
//					WriteOTGReg32(&otg_RqPktCount[RX_EP_INDEX], (opLength / RX_EP_MAX_PACKET_SIZE));
					/* config DMA */
					WriteOTGReg32(otg_DMA_Addr, DATA_BUFFER_ADDR);
					WriteOTGReg32(otg_DMA_Count, opLength);
					WriteOTGReg32(otg_DMA_Ctrl, 0);
					SetBitOTGReg32(otg_DMA_Ctrl, (RX_EP_INDEX << 4));
					SetBitOTGReg32(otg_DMA_Ctrl, (DMA_Ctrl_EnDMA | DMA_Ctrl_Mode | DMA_Ctrl_IntrEn));
					ClrBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_prc_RxPktRdy);		//clear receive flag

					
//					SetBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_hrw_ReqPkt);			/* send first in token */
					/* wait DMA unload data */
					while(1)
					{
						tempL = ReadOTGReg32(otg_DMA_Intr);
						if ((tempL & DMA_Intr_Ch1) != 0)	
							break;
					}/* end while */
			
					ClrBitOTGReg32(otg_DMA_Ctrl, (DMA_Ctrl_EnDMA | DMA_Ctrl_Mode | DMA_Ctrl_IntrEn));
					ClrBitOTGReg16(otg_IndexedCSR_RxCSR, (RxCSR_prw_AutoClear | RxCSR_prw_DMAReqEnab | RxCSR_prw_DMAReqMode)); //config RXCSR

					startLba += opLength / STOR_BLOCK_SIZE;
					requestDataLength -= opLength;

//					OtgDevWriteRxData(DATA_BUFFER_ADDR, startLba, opLength / STOR_BLOCK_SIZE);
#else
//					WriteOTGReg8(otg_CommonUSB_Index, RX_EP_INDEX);
					while(1)
					{
						temp2 = ReadOTGReg16(otg_IndexedCSR_RxCSR);
						if (temp2 & RxCSR_prc_RxPktRdy)
							break;
					}
					temp2 = ReadOTGReg16(otg_IndexedCSR_RxCount);	//get receive data length
					APPLY_MEM();
					memset((BYTE XDATA *)bufAddr, 0, temp2);
					RELEASE_MEM();
#if 0
					APPLY_MEM();
					UnloadFIFOData((BYTE *)&otg_EndpointFIFO[RX_EP_INDEX], (BYTE XDATA *)bufAddr, temp2);
					RELEASE_MEM();
#else
					WriteOTGReg32(otg_DMA_Ctrl, 0);
					WriteOTGReg32(otg_DMA_Addr, bufAddr);
					WriteOTGReg32(otg_DMA_Count, temp2);
					SetBitOTGReg32(otg_DMA_Ctrl, (RX_EP_INDEX << 4));
					SetBitOTGReg32(otg_DMA_Ctrl, (DMA_Ctrl_EnDMA | DMA_Ctrl_IntrEn));
					while(1)
					{
						tempL = ReadOTGReg32(otg_DMA_Intr);
						if ((tempL & DMA_Intr_Ch1) != 0)	
							break;
					}
					ClrBitOTGReg32(otg_DMA_Ctrl, (DMA_Ctrl_EnDMA | DMA_Ctrl_IntrEn));
					APPLY_MEM();
					RELEASE_MEM();
#endif
					ClrBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_prc_RxPktRdy);		//clear receive flag
					requestDataLength -= temp2;
					bufAddr += temp2;
//					gRecvLength -= temp2;
					
#endif
				}//end while

				OtgDevSendPacket(TX_EP_INDEX, gCSWData, sizeof(gCSWData));	//send CSW
				break;

			case 0x1A:		//mode sense
				switch(gRecvBuffer[17])
				{
					case 0x1C:		//mode sense, Timer and Protect Page
						OtgDevSendPacket(TX_EP_INDEX, gModeSenseProtectPage, sizeof(gModeSenseProtectPage));
						OtgDevSendPacket(TX_EP_INDEX, gCSWData, sizeof(gCSWData));	//send CSW
						break;

					case 0x3F:		//mode sense, Return all pages
						OtgDevSendPacket(TX_EP_INDEX, gModeSenseAllPage, sizeof(gModeSenseAllPage));
						OtgDevSendPacket(TX_EP_INDEX, gCSWData, sizeof(gCSWData));	//send CSW
						break;

					default:
						break;
				}//end switch(gRecvBuffer[17]) 
				break;

			case 0x00:		//test unit ready
				OtgDevSendPacket(TX_EP_INDEX, gCSWData, sizeof(gCSWData));	//send CSW
				break;

			default:
				break;
		}//end switch(gRecvBuffer[0x15])
	}
}
