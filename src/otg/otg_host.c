/****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2007, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	otg_host.c
*
* Programmer:    	Grey
*
* Created: 	 		09/xx/2008
*
* Description: 		OTG host controller application layer
*					
*              
*****************************************************************/

#include <string.h>
#include "../public/type.h"
#include "../public/func.h"
#include "otg_config.h"
#include "otg_hal.h"
#include "otg.h"
#include "../usb/hcd.h"

/*---------------------------------OTG host interface config------------------------------*/

BYTE	XDATA	gOtgHcEpMap = 0;

OTG_ENDPOINT_DESCRIPTOR	XDATA	gOtgHcED[MAX_HC_ENDPOINT_NUM * 2];
BYTE	XDATA	gOtgHcEDMap = 0;

extern BYTE
OtgAllocHcEp(
	BYTE		epNum,
	WORD		maxPacketSize
	)
{
//	BYTE	DATA	i;

	if (epNum == 0)
	{
		gOtgHcEpMap |= 0x01;		//hardware endpoint 0 TX,RX buffer size = 64 bytes
		return 0;
	}
	else if ((maxPacketSize <= 64)/* && ((gOtgHcEpMap & 0x02) == 0)*/)
	{
		gOtgHcEpMap |= 0x02;		
		return 1;
	}
	else if ((maxPacketSize <= 512)/* && ((gOtgHcEpMap & 0x04) == 0)*/)
	{
		gOtgHcEpMap |= 0x04;		//hardware endpoint 2 TX,RX buffer size = 512 bytes
		return 2;
	}
	else
	{
		return 0xFF;
	}
}


extern VOID
OtgFreeHcEp(
	BYTE		epIndex
	)
{
	gOtgHcEpMap &= ~(0x01 << epIndex);
}


extern VOID*
OtgAllocED(VOID)
{
	BYTE	DATA	i;

	for (i = 0; i < MAX_HC_ENDPOINT_NUM * 2; ++i)
	{
		if ((gOtgHcEDMap & (0x01 << i)) == 0)
		{
			gOtgHcEDMap |= (0x01 << i);
			return (VOID XDATA *)&gOtgHcED[i];
		}
	}/* end for */

	return NULL;
}


extern VOID
OtgFreeED(
	VOID	*ed
	)
{
	BYTE	DATA	i;

	for (i = 0; i < MAX_HC_ENDPOINT_NUM * 2; ++i)
	{
		if ((VOID XDATA *)&gOtgHcED[i] == ed)
		{
			gOtgHcEDMap &= ~(0x01 << i);
			return;
		}
	}
}


extern BOOL
OtgHostOpen(VOID)
{
#if CODY_FPGA_TEST_EN
	utmi_src_sel &= ~OTG_UTMI_IDDIG;
	utmi_src_sel_en |= OTG_UTMI_IDDIG;		//force A device

	utmi_src_sel |= OTG_UTMI_VBUSVALID;
	utmi_src_sel_en |= OTG_UTMI_VBUSVALID;		//force VBusValid
#else
	/* init otg phy into host mode */
	utmi_ctrl1 |= (OTG_UTMI_VBUSVALID | OTG_UTMI_SESSVALID | OTG_UTMI_SESSEND | OTG_UTMI_SUSPENDM);		//VBUSVALID = 1
	utmi_ctrl1 &= ~OTG_UTMI_IDDIG;									//IDDIG = 0, A device
//	utmi_ctrl2 |= (OTG_UTMI_DP_PULLDOWN | OTG_UTMI_DM_PULLDOWN);	//DP,DM pull down
	utmi_src_sel1 |= (OTG_UTMI_VBUSVALID | OTG_UTMI_IDDIG | OTG_UTMI_SESSVALID | OTG_UTMI_SESSEND | OTG_UTMI_SUSPENDM);		//mcu control these signal
//	utmi_src_sel2 |= (OTG_UTMI_DP_PULLDOWN | OTG_UTMI_DM_PULLDOWN);	
	if (HOST_HIGH_SPEED_EN)
		utmi_ctrl3 &= ~OTG_UTMI_USB_PLAY_MODE;			//set USB read/write mode, using buffer in decoder
	else
		utmi_ctrl3 |= OTG_UTMI_USB_PLAY_MODE;			//set USB read mode, not using buffer in decoder
#endif

	if (HOST_HIGH_SPEED_EN)
		SetBitOTGReg8(otg_CommonUSB_Power, Power_hrw_HSEnab);
	else
		ClrBitOTGReg8(otg_CommonUSB_Power, Power_hrw_HSEnab);

	SetBitOTGReg8(otg_CommonUSB_Power, Power_hrw_EnSuspendMode);	//otg core control phy SUSPENDM

	/* enable all usb interrupt */
	WriteOTGReg8(otg_CommonUSB_IntrUSBE, 0xFF);

	/* start a session */
	SetBitOTGReg8(otg_CtrlFIFO_DevCtl, DevCtl_rw_Session);		/* check the ID pin sensing and verify the core and PHY IDDIG input. */

	return TRUE;
}


extern VOID*
OtgHostOpenPipe(
	DWORD		pipeInfo
	)
{
	OTG_ENDPOINT_DESCRIPTOR		XDATA	*HcED;
	WORD	XDATA	temp;

	HcED = (OTG_ENDPOINT_DESCRIPTOR	XDATA *)OtgAllocED();
	if (HcED == NULL)
		return NULL;

	HcED->FuncAddr = PipeDevice(pipeInfo);
	HcED->EndPointNum = PipeEndpoint(pipeInfo);
	HcED->MaxPacketSize = PipeMaxPacketSize(pipeInfo);

	if (PipeControl(pipeInfo))
	{
		HcED->TransferType = TxRxType_rw_Protocol;		//control transfer
		HcED->Direction = 0;
		HcED->HcEpIndex = OtgAllocHcEp(0, HcED->MaxPacketSize);
		if (HcED->HcEpIndex == 0)
			HcED->HcEpBufferSize = 64;		//control endpoint buffer size = 64 bytes
		else	
		{
			OtgFreeED(HcED);
			return NULL;
		}

		/* set endpoint number index */
		WriteOTGReg8(otg_CommonUSB_Index, 0);
		SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hs_FlushFIFO);		/* flush FIFO */
	}
	else if (PipeBulk(pipeInfo))
	{
		HcED->TransferType = TxRxType_Protocol_Bulk;
		HcED->Direction = PipeIn(pipeInfo) ? 2 : 1;		//IN - 2, OUT - 1
		HcED->HcEpIndex = OtgAllocHcEp(HcED->EndPointNum, HcED->MaxPacketSize);
		if (HcED->HcEpIndex == 1)
			HcED->HcEpBufferSize = 64;		//hardware endpoint 1 TX,RX buffer size = 64 bytes
		else if (HcED->HcEpIndex == 2)
			HcED->HcEpBufferSize = 512;		//hardware endpoint 2 TX,RX buffer size = 512 bytes
		else
		{
			OtgFreeED(HcED);
			return NULL;
		}
		
		/* set endpoint number index */
		WriteOTGReg8(otg_CommonUSB_Index, HcED->HcEpIndex);
	
		if (HcED->Direction == 2)	//IN
		{
			WriteOTGReg8(otg_IndexedCSR_host_RxType, 0);			/* set transfer type */
			WriteOTGReg8(otg_IndexedCSR_host_RxType, HcED->EndPointNum);		
			ClrBitOTGReg8(otg_IndexedCSR_host_RxType, TxRxType_rw_Protocol);
			SetBitOTGReg8(otg_IndexedCSR_host_RxType, HcED->TransferType);	
			
			WriteOTGReg16(otg_IndexedCSR_RxMaxP, HcED->MaxPacketSize);		/* set max packet size */

			SetBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_hs_ClrDataTog);		/* clear data toggle */

			SetBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_hs_FlushFIFO);		/* flush FIFO */

			temp = ReadOTGReg16(otg_IndexedCSR_RxCSR);				/* clear Rx control flag */
			if ((temp & (RxCSR_hrc_RxStall | RxCSR_hrc_Error | RxCSR_hrc_DataError_NAKTimeout)) != 0)
				ClrBitOTGReg16(otg_IndexedCSR_RxCSR, (RxCSR_hrc_RxStall | RxCSR_hrc_Error | RxCSR_hrc_DataError_NAKTimeout));
		}
		else	//OUT
		{
			WriteOTGReg8(otg_IndexedCSR_host_TxType, 0);			/* set transfer type */
			WriteOTGReg8(otg_IndexedCSR_host_TxType, HcED->EndPointNum);		
			ClrBitOTGReg8(otg_IndexedCSR_host_TxType, TxRxType_rw_Protocol);
			SetBitOTGReg8(otg_IndexedCSR_host_TxType, HcED->TransferType);

			WriteOTGReg16(otg_IndexedCSR_TxMaxP, HcED->MaxPacketSize);		/* set max packet size */

			SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_hs_ClrDataTog);		/* clear data toggle */

			SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_hs_FlushFIFO);		/* flush FIFO */

			temp = ReadOTGReg16(otg_IndexedCSR_TxCSR);				/* clear Tx control flag */
			if ((temp & (TxCSR_hrc_RxStall | TxCSR_hrc_Error | TxCSR_hrc_NAKTimeout_IncompTx)) != 0)
				ClrBitOTGReg16(otg_IndexedCSR_TxCSR, (TxCSR_hrc_RxStall | TxCSR_hrc_Error | TxCSR_hrc_NAKTimeout_IncompTx));
		}//end else
	}
	else
	{
		OtgFreeED(HcED);
		return NULL;
	}

	return (VOID *)HcED;
}


extern VOID
OtgHostClosePipe(
	VOID		*pipe
	)
{
	OtgFreeHcEp(((OTG_ENDPOINT_DESCRIPTOR	XDATA *)pipe)->HcEpIndex);
	OtgFreeED(pipe);
}


extern VOID
OtgHostSendUrb(VOID)
{
	OTG_ENDPOINT_DESCRIPTOR		XDATA	*HcED;
	WORD	XDATA	temp;
	DWORD	XDATA	temp1;
	WORD	XDATA	to;
	WORD	XDATA	remainLength;
	WORD	XDATA	opLength;
	BYTE	XDATA	*dat;


	HcED = (OTG_ENDPOINT_DESCRIPTOR	XDATA *)gUrb.Pipe;

	/* set function address */
	WriteOTGReg8(otg_CommonUSB_FAddr, HcED->FuncAddr);
	/* set endpoint number index */
	WriteOTGReg8(otg_CommonUSB_Index, HcED->HcEpIndex);

	if (HcED->TransferType == TxRxType_rw_Protocol)		//control transfer
	{
		/* ---------------------- setup transaction start -----------------------*/
		/* load setup packet data into FIFO */
		LoadFIFOData((BYTE *)&otg_EndpointFIFO[0], gUrb.SetupPacket, 8);
		/* send setup packet */
		SetBitOTGReg16(otg_IndexedCSR_CSR0, (CSR0_hrw_SetupPkt | CSR0_hrs_TxPktRdy));		/* note: these two bit must be set together */
		
		/* Wait EndPoint 0 Tx interrupt */
		to = TRANSACTION_TIME_OUT;
		while(to)
		{
			temp = ReadOTGReg16(otg_IndexedCSR_CSR0);
			if ((temp & CSR0_hrs_TxPktRdy) == 0)
				break;
			to--;
		}/* end while */
		if (to == 0)
		{
			gUrb.Status = CC_DeviceNotResponding;
			return;
		}
	
		if ((temp & CSR0_hrc_RxStall) != 0)
		{
			ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_RxStall);	/* get STALL response */
			gUrb.Status = CC_Stall;
			return;
		}
		else if ((temp & CSR0_hrc_Error) != 0)
		{
			ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_Error);		/* get ERROR response */	
			gUrb.Status = CC_DeviceNotResponding;
			return;
		}
		else if ((temp & CSR0_hrc_NAKTimeout) != 0)
		{
			ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_NAKTimeout);		/* get NAK timeoue response */
			gUrb.Status = CC_DeviceNotResponding;
			return;
		}

		/* ---------------------- setup transaction end -----------------------*/

		remainLength = gUrb.BufferLength;
		dat = gUrb.Buffer;
		if (gUrb.Direction == 0x80)		//control data IN
		{
			/* ----------------- setup data IN transaction start ------------------- */
			//receive data
			while(remainLength)	
			{
				SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrw_ReqPkt);		/* set request packet, start IN phase */
				
				/* wait Endpoint 0 Rx interrupt */
				to = TRANSACTION_TIME_OUT;
				while(to)
				{
					temp = ReadOTGReg16(otg_IndexedCSR_CSR0);
					if ((temp & CSR0_hrc_RxPktRdy) != 0)
						break;
					to--;
				}/* end while */
				if (to == 0)
				{
					gUrb.Status = CC_DeviceNotResponding;
					return;
				}
			
				if ((temp & CSR0_hrc_RxStall) != 0)
				{
					ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_RxStall);		/* get STALL response */
					gUrb.Status = CC_Stall;
					return;
				}
				else if ((temp & CSR0_hrc_Error) != 0)
				{
					ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_Error);		/* get ERROR response */
					gUrb.Status = CC_DeviceNotResponding;
					return;
				}
				else if ((temp & CSR0_hrc_NAKTimeout) != 0)
				{
					ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_NAKTimeout);		/* get NAK timeoue response */
					gUrb.Status = CC_DeviceNotResponding;
					return;
				}

				temp = ReadOTGReg16(otg_IndexedCSR_Count0);
				UnloadFIFOData((BYTE *)&otg_EndpointFIFO[0], dat, temp);
				ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_RxPktRdy);
				remainLength -= temp;
				dat += temp;
				if (temp < HcED->MaxPacketSize)
					break;
			}/* end while */	
			/* no STALL, ERROR, NAKTimeout */ 
			/* ----------------- setup data IN transaction end ------------------- */

			/* ----------------- setup data OUT transaction start(empty packet) ------------------- */
			//OUT empty packet
			SetBitOTGReg16(otg_IndexedCSR_CSR0, (CSR0_hrw_StatusPkt | CSR0_hrs_TxPktRdy));		/* set OUT and status bit */

			/* wait Endpoint 0 Tx interrupt */
			to = TRANSACTION_TIME_OUT;
			while(to)
			{
				temp = ReadOTGReg16(otg_IndexedCSR_CSR0);
				if ((temp & CSR0_hrs_TxPktRdy) == 0)
					break;
				to--;
			}/* end while */
			if (to == 0)
			{
				gUrb.Status = CC_DeviceNotResponding;
				return;
			}
	
			if ((temp & CSR0_hrc_RxStall) != 0)
			{
				ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_RxStall);		/* get STALL response */
				gUrb.Status = CC_Stall;
				return;
			}
			else if ((temp & CSR0_hrc_Error) != 0)
			{
				ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_Error);		/* get ERROR response */
				gUrb.Status = CC_DeviceNotResponding;
				return;
			}
			else if ((temp & CSR0_hrc_NAKTimeout) != 0)
			{
				ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_NAKTimeout);		/* get NAK timeoue response */
				gUrb.Status = CC_DeviceNotResponding;
				return;
			}

			/* ----------------- setup data OUT transaction end(empty packet) ------------------- */
		}//end if (gUrb.Direction == 0x80)
		else
		{
			/* ----------------- setup data OUT transaction start ------------------- */
			while(remainLength)
			{
				if (remainLength < HcED->MaxPacketSize)
					opLength = remainLength;
				else
					opLength = HcED->MaxPacketSize;
			
				/* load data into endpoint0 FIFO */
				LoadFIFOData((BYTE *)&otg_EndpointFIFO[0], dat, opLength);
				/* set OUT bit */
				SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrs_TxPktRdy);

				/* wait Endpoint 0 Tx interrupt */
				to = TRANSACTION_TIME_OUT;
				while(to)
				{
					temp = ReadOTGReg16(otg_IndexedCSR_CSR0);
					if ((temp & CSR0_hrs_TxPktRdy) == 0)
						break;
					to--;
				}/* end while */
				if (to == 0)
				{
					gUrb.Status = CC_DeviceNotResponding;
					return;
				}
	
				if ((temp & CSR0_hrc_RxStall) != 0)
				{
					ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_RxStall);		/* get STALL response */
					gUrb.Status = CC_Stall;
					return;
				}
				else if ((temp & CSR0_hrc_Error) != 0)
				{
					ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_Error);		/* get ERROR response */
					gUrb.Status = CC_DeviceNotResponding;
					return;
				}
				else if ((temp & CSR0_hrc_NAKTimeout) != 0)
				{
					ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_NAKTimeout);		/* get NAK timeoue response */
					gUrb.Status = CC_DeviceNotResponding;
					return;
				}

				remainLength -= opLength;
				dat += opLength;	
			}/* end while */
			/* ----------------- setup data OUT transaction end ------------------- */

			/* ----------------- setup data IN transaction start(empty packet) ------------------- */
			SetBitOTGReg16(otg_IndexedCSR_CSR0, (CSR0_hrw_StatusPkt | CSR0_hrw_ReqPkt));		/* set setup and status bit */

			/* wait Endpoint 0 Rx interrupt */
			to = TRANSACTION_TIME_OUT;
			while(to)
			{
				temp = ReadOTGReg16(otg_IndexedCSR_CSR0);
				if ((temp & CSR0_hrc_RxPktRdy) != 0)
					break;
				to--;
			}/* end while */
			if (to == 0)
			{
				gUrb.Status = CC_DeviceNotResponding;
				return;
			}
	
			if ((temp & CSR0_hrc_RxStall) != 0)
			{
				ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_RxStall);		/* get STALL response */
				gUrb.Status = CC_Stall;
				return;
			}
			else if ((temp & CSR0_hrc_Error) != 0)
			{
				ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_Error);		/* get ERROR response */
				gUrb.Status = CC_DeviceNotResponding;
				return;
			}
			else if ((temp & CSR0_hrc_NAKTimeout) != 0)
			{
				ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_hrc_NAKTimeout);		/* get NAK timeoue response */
				gUrb.Status = CC_DeviceNotResponding;
				return;
			}

			ClrBitOTGReg16(otg_IndexedCSR_CSR0, (CSR0_hrw_StatusPkt | CSR0_hrc_RxPktRdy));	/* no error */
			/* ----------------- setup data IN transaction end(empty packet) ------------------- */
		}//end else (gUrb.Direction == 0x80)
	}//end if (HcED->TransferType == TxRxType_rw_Protocol)
	else if (HcED->TransferType == TxRxType_Protocol_Bulk) 	//bulk transfer
	{
		dat = gUrb.Buffer;
		remainLength = gUrb.BufferLength;
		if (HcED->Direction == 2)		//bulk IN
		{
			ClrBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_hrw_Mode);	/* Rx endpoint */
			/* ----------------- bulk IN transaction start ------------------- */
			if ((WORD)dat < SYS_SHARE_BUF_BASE)
			{
				while(remainLength)
				{
					SetBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_hrw_ReqPkt);			/* send first in token */
					/* Wait EndPoint Rx interrupt */
					to = TRANSACTION_TIME_OUT;
					while(to)
					{
						temp = ReadOTGReg16(otg_IndexedCSR_RxCSR);
						if ((temp & RxCSR_hrc_RxPktRdy) != 0)
							break;
						to--;
					}/* end while */	
					if (to == 0)
					{
						gUrb.Status = CC_DeviceNotResponding;
						return;
					}
		
					if ((temp & RxCSR_hrc_RxStall) != 0)
					{
						ClrBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_hrc_RxStall);		/* get STALL response */
						gUrb.Status = CC_Stall;
						return;
					}
					else if ((temp & RxCSR_hrc_Error) != 0)
					{
						ClrBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_hrc_Error);		/* get ERROR response */
						gUrb.Status = CC_DeviceNotResponding;
						return;
					}
					else if ((temp & RxCSR_hrc_DataError_NAKTimeout) != 0)
					{
						ClrBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_hrc_DataError_NAKTimeout);		/* get NAK timeoue response */
						gUrb.Status = CC_DeviceNotResponding;
						return;
					}		
					else
					{
						temp = ReadOTGReg16(otg_IndexedCSR_RxCount);
						UnloadFIFOData((BYTE *)&otg_EndpointFIFO[HcED->HcEpIndex], dat, temp);
						ClrBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_hrc_RxPktRdy);
						remainLength -= temp;
						dat += temp;
						if (temp < HcED->MaxPacketSize)
							break;
					}
				}/* end while */
			}//end if ((WORD)dat < SYS_SHARE_BUF_BASE)
			else
			{
				/* config DMA */
				WriteOTGReg32(otg_DMA_Addr, (WORD)dat);
				WriteOTGReg32(otg_DMA_Count, remainLength);
				WriteOTGReg32(otg_DMA_Ctrl, 0);
				SetBitOTGReg32(otg_DMA_Ctrl, (HcED->HcEpIndex << 4));
				SetBitOTGReg32(otg_DMA_Ctrl, (DMA_Ctrl_EnDMA | DMA_Ctrl_Mode | DMA_Ctrl_IntrEn));
				/* get max packet size value */
				temp = HcED->MaxPacketSize;
				temp |= ((HcED->HcEpBufferSize / HcED->MaxPacketSize - 1) << 11);
				WriteOTGReg16(otg_IndexedCSR_RxMaxP, temp);	
				/* config request packet count of Rx endpoint */
				temp = remainLength / HcED->MaxPacketSize;
				WriteOTGReg32(&otg_RqPktCount[HcED->HcEpIndex], temp);
				/* config CSR */
				SetBitOTGReg16(otg_IndexedCSR_RxCSR, (RxCSR_hrw_AutoClear | RxCSR_hrw_AutoReq | RxCSR_hrw_DMAReqEnab | RxCSR_hrw_DMAReqMode)); //config RXCSR
		
				SetBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_hrw_ReqPkt);			/* send first in token */
				/* wait DMA unload data */
				to = TRANSACTION_TIME_OUT;
				while(to)
				{
					temp1 = ReadOTGReg32(otg_DMA_Intr);
					if ((temp1 & DMA_Intr_Ch1) != 0)	
						break;
					to--;
				}/* end while */
				if (to == 0)
				{
					gUrb.Status = CC_DeviceNotResponding;
					return;
				}
		
				ClrBitOTGReg16(otg_IndexedCSR_RxCSR, (RxCSR_hrw_AutoClear | RxCSR_hrw_AutoReq | RxCSR_hrw_DMAReqEnab | RxCSR_hrw_DMAReqMode)); //config RXCSR
				ClrBitOTGReg32(otg_DMA_Ctrl, (DMA_Ctrl_EnDMA | DMA_Ctrl_Mode | DMA_Ctrl_IntrEn));
				temp = ReadOTGReg16(otg_IndexedCSR_RxCSR);
		
				if ((temp & RxCSR_hrc_RxStall) != 0)
				{
					ClrBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_hrc_RxStall);		/* get STALL response */
					gUrb.Status = CC_Stall;
					return;
				}
				else if ((temp & RxCSR_hrc_Error) != 0)
				{
					ClrBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_hrc_Error);		/* get ERROR response */
					gUrb.Status = CC_DeviceNotResponding;
					return;
				}
				else if ((temp & RxCSR_hrc_DataError_NAKTimeout) != 0)
				{
					ClrBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_hrc_DataError_NAKTimeout);		/* get NAK timeoue response */
					gUrb.Status = CC_DeviceNotResponding;
					return;
				}		
			}//end else ((WORD)dat < SYS_SHARE_BUF_BASE)
			/* ----------------- bulk IN transaction end ------------------- */

		}//end if (HcED->Direction == 2)		//IN
		else if (HcED->Direction == 1)	//bulk OUT
		{
			SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_hrw_Mode);	/* Tx endpoint */
			/* ----------------- bulk OUT transaction start ------------------- */
			if ((WORD)dat < SYS_SHARE_BUF_BASE)
			{
				while(remainLength)
				{
					if (remainLength < HcED->MaxPacketSize)
						opLength = remainLength;
					else
						opLength = HcED->MaxPacketSize;
					/* load data into FIFO */
					LoadFIFOData((BYTE *)&otg_EndpointFIFO[HcED->HcEpIndex], dat, opLength);
					/* send out token */
					SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_hrs_TxPktRdy);
					/* Wait EndPoint Tx interrupt */
					to = TRANSACTION_TIME_OUT;
					while(to)
					{
						temp = ReadOTGReg16(otg_IndexedCSR_TxCSR);
						if ((temp & TxCSR_hrs_TxPktRdy) == 0)
							break;
						to--;
					}/* end while */	
					if (to == 0)
					{
						gUrb.Status = CC_DeviceNotResponding;
						return;
					}
		
					/* check CSR response */
					if ((temp & TxCSR_hrc_RxStall) != 0)
					{
						ClrBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_hrc_RxStall);		/* get STALL response */
						gUrb.Status = CC_Stall;
						return;
					}
					else if ((temp & TxCSR_hrc_Error) != 0)
					{
						ClrBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_hrc_Error);			/* get ERROR response */
						gUrb.Status = CC_DeviceNotResponding;
						return;
					}
					else if ((temp & TxCSR_hrc_NAKTimeout_IncompTx) != 0)
					{
						ClrBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_hrc_NAKTimeout_IncompTx);		/* get NAK timeoue response */
						gUrb.Status = CC_DeviceNotResponding;
						return;
					}
					else
					{
						remainLength -= opLength;
						dat += opLength;
					}	
				}/* end while */
			}//end if ((WORD)dat < SYS_SHARE_BUF_BASE)
			else
			{
				/* config DMA */
				WriteOTGReg32(otg_DMA_Addr, (WORD)dat);
				WriteOTGReg32(otg_DMA_Count, remainLength);
				WriteOTGReg32(otg_DMA_Ctrl, 0);
				SetBitOTGReg32(otg_DMA_Ctrl, (HcED->HcEpIndex << 4));
				SetBitOTGReg32(otg_DMA_Ctrl, (DMA_Ctrl_EnDMA | DMA_Ctrl_Direction | DMA_Ctrl_Mode | DMA_Ctrl_IntrEn));
				/* get max packet size value */
				temp = HcED->MaxPacketSize;
				temp |= ((HcED->HcEpBufferSize / HcED->MaxPacketSize - 1) << 11);
				WriteOTGReg16(otg_IndexedCSR_TxMaxP, temp);
				/* config CSR */
				SetBitOTGReg16(otg_IndexedCSR_TxCSR, (TxCSR_hrw_AutoSet | TxCSR_hrw_DMAReqEnab | TxCSR_hrw_DMAReqMode)); //config TxCSR
		
				/* wait DMA load data */
				to = TRANSACTION_TIME_OUT;
				while(to)
				{
					temp1 = ReadOTGReg32(otg_DMA_Intr);
					if ((temp1 & DMA_Intr_Ch1) != 0)	
						break;
					to--;
				}/* end while */
				if (to == 0)
				{
					gUrb.Status = CC_DeviceNotResponding;
					return;
				}
		
				ClrBitOTGReg16(otg_IndexedCSR_TxCSR, (TxCSR_hrw_AutoSet | TxCSR_hrw_DMAReqEnab | TxCSR_hrw_DMAReqMode));
				ClrBitOTGReg32(otg_DMA_Ctrl, (DMA_Ctrl_EnDMA | DMA_Ctrl_Direction | DMA_Ctrl_Mode | DMA_Ctrl_IntrEn));
				temp = ReadOTGReg16(otg_IndexedCSR_RxCSR);
		
					/* check CSR response */
				if ((temp & TxCSR_hrc_RxStall) != 0)
				{
					ClrBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_hrc_RxStall);		/* get STALL response */
					gUrb.Status = CC_Stall;
					return;
				}
				else if ((temp & TxCSR_hrc_Error) != 0)
				{
					ClrBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_hrc_Error);			/* get ERROR response */
					gUrb.Status = CC_DeviceNotResponding;
					return;
				}
				else if ((temp & TxCSR_hrc_NAKTimeout_IncompTx) != 0)
				{
					ClrBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_hrc_NAKTimeout_IncompTx);		/* get NAK timeoue response */
					gUrb.Status = CC_DeviceNotResponding;
					return;
				}
			}//end else ((WORD)dat < SYS_SHARE_BUF_BASE)
			/* ----------------- bulk OUT transaction end ------------------- */
		}//end else if (HcED->Direction == 1)	//bulk OUT
	}//end else if (HcED->TransferType == TxRxType_Protocol_Bulk) 	//bulk transfer
}


extern VOID
OtgHostRootHubPortStatus(
	BOOL		*isConnect,			/* output: have device connection set 1, else set 0 */
	BOOL		*isChange			/* 			device connection change set 1, else set 0 */
	)
{
	BYTE	DATA	temp;

	temp = ReadOTGReg8(otg_CommonUSB_IntrUSB);

	if ((temp & IntUSB_Conn) != 0)		/* new device connection */
	{
		*isConnect = TRUE;
		*isChange = TRUE;
	}
	else if ((temp & IntUSB_DisCon) != 0)		/* device disconnection */
	{
		*isConnect = FALSE;
		*isChange = TRUE;
	}
	else
	{
		*isChange = FALSE;		/* device not change */
	}
}


extern VOID
OtgHostRootHubPortReset(VOID)
{
	WORD	DATA	i;

	SetBitOTGReg8(otg_CommonUSB_Power, Power_hrw_Reset);
	for (i = 0; i < 4000; ++i);
	ClrBitOTGReg8(otg_CommonUSB_Power, Power_hrw_Reset);
}


extern VOID
OtgHostRootHubClearPortFeature(VOID)
{

}


extern WORD
OtgHostGetFrameNumber(VOID)
{
	return ReadOTGReg16(otg_CommonUSB_Frame);
}
