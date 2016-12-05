/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	otg_device_isr.c
*
* Programmer:    	Grey
*
* Created: 	 		10/xx/2008
*
* Description: 		otg usb device ISR function file
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
#include "otg_device_isr.h"


/* ------------------------------ USB Device Control Parameter Define --------------------------------- */
BYTE			gSetup[8];				//setup packet buffer
BYTE			gResp[2] = {0, 0,};		//response data packet

USB_DEVICE_STATE	gUsbDevStatus = USB_STATE_ATTACHED;		//USB device status
USB_ENDPOINT_STATE	gUsbEp0Status = USB_EP_STATE_IDLE;		//endpoint-0 status
USB_ENDPOINT_STATE	gUsbEpTxStatus = USB_EP_STATE_HALT;		//endpoint-Tx status
USB_ENDPOINT_STATE	gUsbEpRxStatus = USB_EP_STATE_HALT;		//endpoint-Rx status

BYTE	*gUsbSendPtr = 0;				//usb protocol data buffer pointer
WORD	gUsbSendRemainDataLength = 0;	//usb protocol data send length

BYTE	gUsbAddress = 0;				//allocate usb device address
BOOL	gSetAddressFlag = FALSE;		//set address flag
WORD	gUsbBulkMaxPacketSize = 64;		//usb bulk transfer max packet size
/* ------------------------------ USB Device Control Parameter Define End --------------------------------- */


/* ------------------------------ Descriptor Declaration --------------------------------- */
static	BYTE	XDATA	gDeviceDescriptor[] = 
{
	0x12, 					// bLength
	0x01, 					// bDescriptorType
	0x00, 0x02, 			// bcdUSB
	0x00, 					// bDeviceClass
	0x00, 					// bDeviceSubClass
	0x00, 					// bDeviceProtocol
	OTG_EP0_MAX_PACKET_SIZE, 			// bMaxPacketSize0
	0x00, 0x00, 			// idVendor
	0x00, 0x00,				// idProduct
	0x00, 0x01, 			// bcdDevice
	0x01, 					// iManufacturer
	0x02, 					// iProduct
	0x03, 					// iSerialNumber
	0x01,					// bNumConfigurations
};

static	BYTE	XDATA	gConfigDescriptor[] = 
{
	0x09, 			// Length
	0x02, 			// Type
	0x20, 0x00, 	// Totallength
	0x01, 			// NumInterfaces
	0x01, 			// bConfigurationValue
	0x00, 			// iConfiguration
	0x80, 			// bmAttributes
	0x32, 			// MaxPower
	
	0x09, 			// bLength
	0x04, 			// bDescriptorType
	0x00, 			// bInterfaceNumber
	0x00, 			// bAlternateSetting
	0x02, 			// bNumEndpoints
	0x08, 			// bInterfaceClass
	0x06, 			// bInterfaceSubClass
	0x50, 			// bInterfaceProcotol
	0x00, 			// iInterface
	
	0x07, 			// bLength
	0x05, 			// bDescriptorType
	(OTG_BULK_IN_EP | 0x80), 		// bEndpointAddress
	0x02, 			// bmAttributes
	0x00, 0x02,		// MaxPacketSize (LITTLE ENDIAN)
	0xFF, 			// bInterval
	
	0x07, 			// bLength
	0x05, 			// bDescriptorType
	OTG_BULK_OUT_EP, 			// bEndpointAddress
	0x02, 			// bmAttributes
	0x00, 0x02, 	// MaxPacketSize (LITTLE ENDIAN)
	0xFF,			// bInterval
};


#define STR0LEN 4

BYTE CODE String0Desc[STR0LEN] =
{
   STR0LEN, 0x03, 0x09, 0x04
}; //end of String0Desc

#define STR1LEN sizeof("MVSILICON INC.") * 2

BYTE CODE String1Desc[STR1LEN] =
{
   STR1LEN, 0x03,
   'M', 0,
   'V', 0,
   'S', 0,
   'I', 0,
   'L', 0,
   'I', 0,
   'C', 0,
   'O', 0,
   'N', 0,
   ' ', 0,
   'I', 0,
   'N', 0,
   'C', 0,
   '.', 0
  }; //end of String1Desc

#define STR2LEN sizeof("Oriole SOC") * 2

BYTE CODE String2Desc[STR2LEN] =
{
   STR2LEN, 0x03,
   'O', 0,
   'r', 0,
   'i', 0,
   'o', 0,
   'l', 0,
   'e', 0,
   ' ', 0,
   'S', 0,
   'O', 0,
   'C', 0
}; //end of String2Desc

#define STR3LEN sizeof("007") * 2

BYTE CODE String3Desc[STR3LEN] =
{
   STR3LEN, 0x03,
   '0', 0,
   '0', 0,
   '7', 0            
}; //end of String2Desc


BYTE CODE * CODE StringDescTable[] =
{
   String0Desc,
   String1Desc,
   String2Desc,
   String3Desc
};
/* ------------------------------ Descriptor Declaration End --------------------------------- */


/* ------------------------------ Standard Request --------------------------------- */
//
// protocol stall for not support request
//
extern VOID
OtgDeviceProtocolStall(VOID)
{
	// set endpoint index = 0
	WriteOTGReg8(otg_CommonUSB_Index, 0);

	// flush endpoint FIFO
	SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_ps_FlushFIFO);

	// set CSR0 Send_Stall bit
	SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_ps_SendStall);

	// ep0_status = HALT.
	gUsbEp0Status = USB_EP_STATE_HALT;
}


//
// Get Status request
// DEVICE status are self_power and remote_wake_up, 
// INTERFACE status is reserved,
// ENDPOINT status is HALT
//
extern VOID 
OtgReq_GetStatus(VOID)                  
{                    
	// wValue must = 0, wLength must = 2
	if (gSetup[2] || gSetup[3]	//gSetup.wValue.c[HIGH] || gSetup.wValue.c[LOW] 
		|| gSetup[6] != 2 || gSetup[7]) //gSetup.wLength.c[HIGH] || gSetup.wLength.c[LOW] != 2)		// request invalid
	{
		// ep0 send stall
		OtgDeviceProtocolStall();
	}

	switch(gSetup[0] & USB_RECIP_MASK)
	{
		case USB_RECIP_DEVICE:
			if (gSetup[4] || gSetup[5])		//device GetStatus wIndex must = 0
			{
				// ep0 send stall
				OtgDeviceProtocolStall();
			}
			else
			{
				// ep0 send zero data packet, indicate bus self_power and no remote_wake_up
				gResp[0] = 0x01; gResp[1] = 0x00;		//device self_power and no remote_wake_up
				gUsbSendPtr = gResp;
				gUsbSendRemainDataLength = 2;
			}
			break;

		case USB_RECIP_INTERFACE:
			if (gUsbDevStatus != USB_STATE_CONFIGURED 
				|| gSetup[4] || gSetup[5])		//interface GetStatus wIndex must = 0, indicate interface 0 status.
			{
				// ep0 send stall
				OtgDeviceProtocolStall();
			}
			else
			{
				// ep0 send zero data packet
				gResp[0] = 0x00; gResp[1] = 0x00;	//interface status reserved
				gUsbSendPtr = gResp;
				gUsbSendRemainDataLength = 2;
			}
			break;

		case USB_RECIP_ENDPOINT:
			if (gUsbDevStatus != USB_STATE_CONFIGURED 
				|| gSetup[5])				//endpoint GetStatus wIndex[HIGH] must = 0,wIndex[LOW] must = endpint number.
			{
				// ep0 send stall
				OtgDeviceProtocolStall();
			}
			else
			{
				if (gSetup[4] == (OTG_BULK_IN_EP | 0x80)) //ep2 IN 
				{
					gResp[0] = (gUsbEpTxStatus == USB_EP_STATE_HALT) ? 0x01 : 0x00;	//send endpoint HALT status
					gResp[1] = 0x00;
					gUsbSendPtr = gResp;
					gUsbSendRemainDataLength = 2;
				}
				else if (gSetup[4] == OTG_BULK_OUT_EP)	//ep2 OUT
				{
					gResp[0] = (gUsbEpRxStatus == USB_EP_STATE_HALT) ? 0x01 : 0x00;	//send endpoint HALT status
					gResp[1] = 0x00;
					gUsbSendPtr = gResp;
					gUsbSendRemainDataLength = 2;
				}
				else
				{
					// ep0 send stall
					OtgDeviceProtocolStall();
				}
			}
			break;

		default:
			OtgDeviceProtocolStall();
			break;
	}// end switch

	if (gUsbEp0Status != USB_EP_STATE_HALT)
	{
		// clear CSR0 RxPktRdy bit, indicate setup packet process done.
		SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_ps_ServicedRxPktRdy);

		// ep0_status = TX
		gUsbEp0Status = USB_EP_STATE_TX;
	}
	//*/                  
}


//
// Clear Feature request
// It is only valid for clear endpoint HALT feature.
//
extern VOID
OtgReq_ClearFeature(VOID)   
{                     
	// Response of this request must be in CONFIGURED status.
	// Request receive object must be ENDPOINT.
	// wValue must = 0, indicate Feature_Selector is ENDPOINT_HALT.
	// wLength must = 0, and wIndex[HIGH] must = 0.
	if (gUsbDevStatus != USB_STATE_CONFIGURED
		|| (gSetup[0] & USB_RECIP_MASK) != USB_RECIP_ENDPOINT
		|| gSetup[2] || gSetup[3]		//wValue
		|| gSetup[5]							//wIndex
		|| gSetup[6] || gSetup[7])  	//wLength		//request invalid
	{
		// ep0 send stall
		OtgDeviceProtocolStall();
	}
	else
    {
		if (gSetup[4] == (OTG_BULK_IN_EP | 0x80)
			&& gUsbEpTxStatus == USB_EP_STATE_HALT)	//ep2 IN
		{
			// set endpoint index = 2
			WriteOTGReg8(otg_CommonUSB_Index, OTG_BULK_IN_EP);

			// clear TxCSR Sent_Stall bit to clear HALT
			ClrBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prc_SentStall);

			// ep2Tx_status = IDLE;
			gUsbEpTxStatus = USB_EP_STATE_IDLE;
		}
		else if (gSetup[4] == OTG_BULK_OUT_EP
			&& gUsbEpRxStatus == USB_EP_STATE_HALT)		//ep2 OUT
		{
			// set endpoint index = 2
			WriteOTGReg8(otg_CommonUSB_Index, OTG_BULK_OUT_EP);

			// clear RxCSR Sent_Stall bit to clear HALT
			ClrBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_prc_SentStall);

			// ep2Rx_status = IDLE;
			gUsbEpRxStatus = USB_EP_STATE_IDLE;
		}
		else
		{
			// ep0 send stall
			OtgDeviceProtocolStall();		//request invalid
		}
    }     

	// set endpoint index = 0
	WriteOTGReg8(otg_CommonUSB_Index, 0);

	if (gUsbEp0Status != USB_EP_STATE_HALT)
	{
		// clear CSR0 RxPktRdy bit, indicate setup packet process done.
		SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_ps_ServicedRxPktRdy);
	}
}


//
// Set Feature request
// It is only valid for set endpoint HALT feature.
//
extern VOID
OtgReq_SetFeature(VOID)                          
{        
	// Response of this request must be in CONFIGURED status.
	// Request receive object must be ENDPOINT.
	// wValue must = 0, indicate Feature_Selector is ENDPOINT_HALT.
	// wLength must = 0, and wIndex[HIGH] must = 0.
	if (gUsbDevStatus != USB_STATE_CONFIGURED
		|| (gSetup[0] & USB_RECIP_MASK) != USB_RECIP_ENDPOINT
		|| gSetup[2] || gSetup[3]		//wValue
		|| gSetup[5]							//wIndex
		|| gSetup[6] || gSetup[7])  	//wLength		//request invalid
	{
		// ep0 send stall
		OtgDeviceProtocolStall();
	}
	else
	{
		if (gSetup[4] == (OTG_BULK_IN_EP | 0x80))	//ep2 IN
		{
			// set endpoint index = 2
			WriteOTGReg8(otg_CommonUSB_Index, OTG_BULK_IN_EP);

			// set TxCSR Send_Stall bit to set HALT
			SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prw_SendStall);

			// ep2Tx_status = HALT;	
			gUsbEpTxStatus = USB_EP_STATE_HALT;
		}
		else if (gSetup[4] == OTG_BULK_OUT_EP)		//ep2 OUT
		{
			// set endpoint index = 2
			WriteOTGReg8(otg_CommonUSB_Index, OTG_BULK_OUT_EP);

			// set RxCSR Send_Stall bit to set HALT
			SetBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_prw_SendStall);

			// ep2Rx_status = HALT;
			gUsbEpRxStatus = USB_EP_STATE_HALT;
		}
		else
		{
			// ep0 send stall
			OtgDeviceProtocolStall();
		}
	}

	// set endpoint index = 0

	if (gUsbEp0Status != USB_EP_STATE_HALT)
	{
		// clear CSR0 RxPktRdy bit, indicate setup packet process done.
		SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_ps_ServicedRxPktRdy);
	}
}


//
// Set Address request
// Host allocate device address
//
extern VOID
OtgReq_SetAddress(VOID)                          
{
	// Request receive object must be DEVICE.
	// wValue[HIGH] must = 0, wIndex and wLength must be 0
	if ((gSetup[0] & USB_RECIP_MASK) != USB_RECIP_DEVICE
		|| gSetup[3] 						//wValue
		|| gSetup[4] || gSetup[5]	//wIndex
		|| gSetup[6] || gSetup[7])	//wLength	//request invalid
	{
		// ep0 send stall
		OtgDeviceProtocolStall();
	}
	else
	{	
		// set (gSetup.wValue.c[LOW] & 0x7F) hardware allocated address
		gUsbAddress = (gSetup[2] & 0x7F);
		gSetAddressFlag = TRUE;

		//set device status
		gUsbDevStatus = ((gSetup[2] & 0x7F) != 0) ? USB_STATE_ADDRESS : USB_STATE_DEFAULT;

		gUsbSendRemainDataLength = 0;

		// ep_status = TX
		gUsbEp0Status = USB_EP_STATE_TX;

		// clear CSR0 RxPktRdy bit, indicate setup packet process done.
		SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_ps_ServicedRxPktRdy);
	}
}


//
// Get Descriptor request
// Send all descriptors to host
//
extern VOID
OtgReq_GetDescriptor(VOID)                       
{    
	switch(gSetup[3])	//descriptor type
	{
		case USB_DT_DEVICE:
			// set send data pointer and size of DeviceDesc
			gUsbSendPtr = gDeviceDescriptor;
			gUsbSendRemainDataLength = sizeof(gDeviceDescriptor);
			break;

		case USB_DT_CONFIG:
			// set send data pointer and size of ConfigDesc
			gUsbSendPtr = gConfigDescriptor;
			gUsbSendRemainDataLength = gConfigDescriptor[3];
			gUsbSendRemainDataLength <<= 8;
			gUsbSendRemainDataLength += gConfigDescriptor[2];
			break;

		case USB_DT_STRING:
			// set send data pointer and size of StringDescTable 
			gUsbSendPtr = StringDescTable[gSetup[2]];
			gUsbSendRemainDataLength = gUsbSendPtr[0];
			break;

		case USB_DT_INTERFACE:
			// set send data pointer and size of InterfaceDesc 
			gUsbSendPtr = (BYTE *)&gConfigDescriptor[10];
			gUsbSendRemainDataLength = gConfigDescriptor[10];
			break;

		case USB_DT_ENDPOINT:
			if (gSetup[2] == (OTG_BULK_IN_EP | 0x80))		//ep2 IN
			{
				// set send data pointer and size of EndpointInDesc 
				gUsbSendPtr = (BYTE *)&gConfigDescriptor[18];
				gUsbSendRemainDataLength = gConfigDescriptor[18];
			}
			else if (gSetup[2] == OTG_BULK_OUT_EP)			//ep2 OUT
			{
				// set send data pointer and size of EndpointOutDesc
				gUsbSendPtr = (BYTE *)&gConfigDescriptor[25];
				gUsbSendRemainDataLength = gConfigDescriptor[25];
			}
			else
			{
				// ep0 send stall	
				OtgDeviceProtocolStall();
				return;
			}
//*/
			break;

		default:
			// ep0 send stall
			OtgDeviceProtocolStall();
		 	return;
//			break;
	}//end switch

	if (gUsbSendRemainDataLength > gSetup[6])
		gUsbSendRemainDataLength = gSetup[6];

	if (gUsbEp0Status != USB_EP_STATE_HALT)
	{
		// ep0_status = TX
		gUsbEp0Status = USB_EP_STATE_TX;

		// clear CSR0 RxPktRdy bit, indicate setup packet process done.
		SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_ps_ServicedRxPktRdy);
	}
}


//
// Get Configuration request
// Send configure information to host.
//
extern VOID
OtgReq_GetConfiguration(VOID)                   
{
	// Request receive object must be DEVICE.
	// wValue and wIndex must be 0, wLength must be 1.
	if ((gSetup[0] & USB_RECIP_MASK) != USB_RECIP_DEVICE
		|| gSetup[2] || gSetup[3]			//wValue
		|| gSetup[4] || gSetup[5]			//wIndex
		|| gSetup[6] != 1 || gSetup[7])		//wLength
	{
		// ep0 send stall
		OtgDeviceProtocolStall();
	}
	else
	{
		//configure is active response 1, else response 0.
		gResp[0] = (gUsbDevStatus == USB_STATE_CONFIGURED) ? 0x01 : 0x00;
		gUsbSendRemainDataLength = 1;	
	
		// clear CSR0 RxPktRdy bit, indicate setup packet process done.
		SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_ps_ServicedRxPktRdy);

		// ep0_status = TX
		gUsbEp0Status = USB_EP_STATE_TX;
	}
}


//
// Set Configuration request
// Host activate the configuration.
//
extern VOID
OtgReq_SetConfiguration(VOID)                   
{                     
	// Device must be in ADDRESS status.
	// Request receive object must be DEVICE.
	// wIndex and wLength must be 0.
	if (gUsbDevStatus != USB_STATE_ADDRESS
		|| (gSetup[0] & USB_RECIP_MASK) != USB_RECIP_DEVICE
		|| gSetup[4] || gSetup[5]		//wIndex
		|| gSetup[6] || gSetup[7])      //wLength
    {
		// ep0 send stall
		OtgDeviceProtocolStall();
	}
	else
	{
		if (gSetup[2] == gConfigDescriptor[5])		//check configuration value
		{
			// usb_status = CONFIGURED
			gUsbDevStatus = USB_STATE_CONFIGURED;
			// ep2Tx_status = ep2Rx_status = IDLE
			gUsbEpTxStatus = gUsbEpRxStatus = USB_EP_STATE_IDLE;

			// Init Tx and Rx hardware endpoint
			// Rx endpoint prepare to receive data

			WriteOTGReg8(otg_CommonUSB_Index, OTG_BULK_OUT_EP);
			// set endpoint 2 Rx mode 
			ClrBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prw_Mode);
			WriteOTGReg16(otg_IndexedCSR_RxMaxP, gUsbBulkMaxPacketSize);
			// set ClrDataTog for fisrt configuration 
			SetBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_ps_ClrDataTog);
			// FIFO need flush
			SetBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_ps_FlushFIFO);
//			ReadOTGReg16(otg_IndexedCSR_RxCSR);
//			SetBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_prc_RxPktRdy);
			// config ep2Rx to receive first packet ???

			WriteOTGReg8(otg_CommonUSB_Index, 0);
		}
		else
		{
			// usb_status = ADDRESS
//			gUsbDevStatus = USB_STATE_ADDRESS;
			// ep2Tx_status = ep2Rx_status = HALT
			gUsbEpTxStatus = gUsbEpRxStatus = USB_EP_STATE_HALT;
		}

		// clear CSR0 RxPktRdy bit, indicate setup packet process done.
		SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_ps_ServicedRxPktRdy);
	}
}


//
// Get Interface request
// Send interface information to host
//
extern VOID
OtgReq_GetInterface(VOID)                        
{   
	// Device must be in CONFIGURED status.
	// Request receive object must be INTERFACE.
	// wValue must be 0, wLength must be 1, wIndex must be 0 to indicate interface-0.
	if (gUsbDevStatus != USB_STATE_CONFIGURED
		|| (gSetup[0] & USB_RECIP_MASK) != USB_RECIP_INTERFACE
		|| gSetup[2] || gSetup[3]				//wValue
		|| gSetup[4] != gConfigDescriptor[11] || gSetup[5]		//wIndex
		|| gSetup[6] != 1 || gSetup[7])			//wLength
    {
		// ep0 send stall
		OtgDeviceProtocolStall();
	}
	else
	{
		gResp[0] = 0x00;
		gUsbSendPtr = gResp;
		gUsbSendRemainDataLength = 1;

		// clear CSR0 RxPktRdy bit, indicate setup packet process done.
		SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_ps_ServicedRxPktRdy);
		
		// ep0_status = TX
		gUsbEp0Status = USB_EP_STATE_TX;
	}
}


//
// Set Interface request
// Host set interface to device.
//
extern VOID
OtgReq_SetInterface(VOID)		//not support this function
{
	// Device must be in CONFIGURED status.
	// Request receive object must be INTERFACE.
	// wValue and wLength must be 0, wIndex must be 0 to indicate interface-0.
	if (gUsbDevStatus != USB_STATE_CONFIGURED
		|| (gSetup[0] & USB_RECIP_MASK) != USB_RECIP_INTERFACE
		|| gSetup[2] || gSetup[3]			//wValue
		|| gSetup[4] != gConfigDescriptor[11] || gSetup[5]		//wIndex
		|| gSetup[6] || gSetup[7])			//wLength
	{
		// ep0 send stall
		OtgDeviceProtocolStall();
	}
	else
	{
		// clear CSR0 RxPktRdy bit, indicate setup packet process done.
		SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_ps_ServicedRxPktRdy);
	}
}
/* ------------------------------ Standard Request End --------------------------------- */


/* ------------------------------ Mass Storage Request --------------------------------- */
//
// Get Max LUN request
// Send Max LUN to host
//
extern VOID
OtgReq_MaxLUN(VOID)
{
	// Device must be in CONFIGURED status.
	// Request receive object must be INTERFACE.
	if (gUsbDevStatus != USB_STATE_CONFIGURED
		|| (gSetup[0] & USB_RECIP_MASK) != USB_RECIP_INTERFACE
		|| gSetup[2] || gSetup[3]			//wValue
		|| gSetup[4] || gSetup[5]			//wIndex
		|| gSetup[6] != 1 || gSetup[7])		//wLength
	{
		// ep0 send stall
		OtgDeviceProtocolStall();
	}
	else
	{
		gResp[0] = 0x00;	//send Max LUN 
		gUsbSendPtr = gResp;
		gUsbSendRemainDataLength = 1;

		// clear CSR0 RxPktRdy bit, indicate setup packet process done.
		SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_ps_ServicedRxPktRdy);

		// ep0_status = TX
		gUsbEp0Status = USB_EP_STATE_TX;
	}
}
/* ------------------------------ Mass Storage Request End --------------------------------- */


/* ------------------------------ USB Device Interrupt Service --------------------------------- */
extern VOID
OtgDeviceSuspend(VOID)
{
	// add code you need
}


extern VOID
OtgDeviceResume(VOID)
{
	// add code you need
}


extern VOID
OtgDeviceSessionEnd(VOID)
{
	// add code you need
}


extern VOID
OtgDeviceReset(VOID)
{
	// usb_status = DEFAULT
	gUsbDevStatus = USB_STATE_DEFAULT;

	// set SuspendM of POWER register to enable suspend signal detection
 
	// update all endpoint status
	// ep0_status = IDLE
	gUsbEp0Status = USB_EP_STATE_IDLE;
	// ep2RX_status = HALT
	// ep2TX_status = HALT
	gUsbEpTxStatus = gUsbEpRxStatus = USB_EP_STATE_HALT;

	// set bulk endpoint max packet size
	if (ReadOTGReg8(otg_CommonUSB_Power) & Power_pr_HSMode)
	{
		gDeviceDescriptor[2] = 0x00;
		gDeviceDescriptor[3] = 0x02;		// USB2.0
		gUsbBulkMaxPacketSize = OTG_BULK_HS_MAX_PACKET_SIZE;
	}
	else
	{
		gDeviceDescriptor[2] = 0x10;
		gDeviceDescriptor[3] = 0x01;		// USB1.1
		gUsbBulkMaxPacketSize = OTG_BULK_FS_MAX_PACKET_SIZE;
	}
		
	// set configuration descriptor max packet size
	gConfigDescriptor[22] = gConfigDescriptor[29] = ((BYTE *)&gUsbBulkMaxPacketSize)[1];
	gConfigDescriptor[23] = gConfigDescriptor[30] = ((BYTE *)&gUsbBulkMaxPacketSize)[0];
}


extern VOID
OtgDeviceEp0Process(VOID)
{
	WORD	csr0;

	// set endpoint index = 0
	WriteOTGReg8(otg_CommonUSB_Index, 0);
	// read CSR0 register
	csr0 = ReadOTGReg16(otg_IndexedCSR_CSR0);

	if (gSetAddressFlag)
	{
		gSetAddressFlag = FALSE;

		// config hardware allocated address
		WriteOTGReg8(otg_CommonUSB_FAddr, gUsbAddress);
	}

//	if (CSR0 Sent_Stall set?)
	if (csr0 & CSR0_prc_SentStall)
	{
		// clear CSR0 Sent_Stall bit
		ClrBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_prc_SentStall);

		// ep_status = IDLE
		gUsbEp0Status = USB_EP_STATE_IDLE;

		return;
	}

//	if (CSR0 Setup_End set?)
	if (csr0 & CSR0_pr_SetupEnd)
	{
		// set CSR0 Service_Setup_End to clear Setup_End
		SetBitOTGReg16(otg_IndexedCSR_CSR0, CSR0_ps_ServicedSetupEnd);

		// ep_status = IDLE
		gUsbEp0Status = USB_EP_STATE_IDLE;
	}
//*/
	if (gUsbEp0Status == USB_EP_STATE_IDLE)
	{
//		if (CSR0 RxPktRdy set?)
		if (csr0 & CSR0_pr_RxPktRdy)
		{
			// load data from FIFO to gSetup
			UnloadFIFOData((BYTE *)&otg_EndpointFIFO[0], gSetup, 8);

			// process host request
			switch (gSetup[1])
			{
				case USB_REQ_GET_STATUS:
					OtgReq_GetStatus();
					break;
				case USB_REQ_CLEAR_FEATURE:
					OtgReq_ClearFeature();
					break;
				case USB_REQ_SET_FEATURE:
					OtgReq_SetFeature();
					break;
				case USB_REQ_SET_ADDRESS:
					OtgReq_SetAddress();
					break;
				case USB_REQ_GET_DESCRIPTOR:
					OtgReq_GetDescriptor();
					break;
				case USB_REQ_GET_CONFIGURATION:
					OtgReq_GetConfiguration();
					break;
				case USB_REQ_SET_CONFIGURATION:
					OtgReq_SetConfiguration();
					break;
				case USB_REQ_GET_INTERFACE:
					OtgReq_GetInterface();
					break;
				case USB_REQ_SET_INTERFACE:
					OtgReq_SetInterface();
					break;
				case 0xFE:		//Get Max LUN
					OtgReq_MaxLUN();
					break;
				default:
					OtgDeviceProtocolStall();
					break;
			}//end switch
		}// end if (csr0 & CSR0_pr_RxPktRdy)
	}// end if (gUsbEp0Status == USB_EP_STATE_IDLE)

	if (gUsbEp0Status == USB_EP_STATE_TX)
	{
//		if (CSR0 TxPktRdy not set?)		//make sure no packet is sending
		if ((csr0 & CSR0_prs_TxPktRdy) == 0)
		{
			// read CSRO register	temp = CSR0
			csr0 = ReadOTGReg16(otg_IndexedCSR_CSR0);

//			if (CSR0 Setup_End and RxPktRdy not set?)
			if ((csr0 & (CSR0_pr_SetupEnd | CSR0_pr_RxPktRdy)) == 0)
			{
				if (gUsbSendRemainDataLength > OTG_EP0_MAX_PACKET_SIZE)
				{	
					// load data OTG_EP0_MAX_PACKET_SIZE to fifo
					LoadFIFOData((BYTE *)&otg_EndpointFIFO[0], gUsbSendPtr, OTG_EP0_MAX_PACKET_SIZE);

					// add data send count
					gUsbSendRemainDataLength -= OTG_EP0_MAX_PACKET_SIZE;
					gUsbSendPtr += OTG_EP0_MAX_PACKET_SIZE;
					csr0 |= CSR0_prs_TxPktRdy;
				}
				else if (gUsbSendRemainDataLength == 0)
				{
					// set CSR0 Data_End bit
					csr0 |= CSR0_ps_DataEnd;

					gUsbEp0Status = USB_EP_STATE_IDLE;
				}
				else
				{
					// load data gUsbSendRemainDataLength to fifo
					LoadFIFOData((BYTE *)&otg_EndpointFIFO[0], gUsbSendPtr, gUsbSendRemainDataLength);
					
					// set CSR0 Data_End bit
					csr0 |= (CSR0_ps_DataEnd | CSR0_prs_TxPktRdy);

					// data send done, ep0 to IDLE status
					gUsbSendRemainDataLength = 0;
					gUsbEp0Status = USB_EP_STATE_IDLE;
				}

				// set CSR0 TxPktRdy to send data
				SetBitOTGReg16(otg_IndexedCSR_CSR0, csr0);
			}//end if ((csr0 & (CSR0_pr_SetupEnd | CSR0_pr_RxPktRdy)) == 0)
		}//end if ((csr0 & CSR0_prs_TxPktRdy) == 0)
	}//end if (gUsbEp0Status == USB_EP_STATE_TX)
}


extern VOID
OtgDeviceTxDoneProcess(VOID)
{
	WORD	txCSR;

	// set endpoint index = 2
	WriteOTGReg8(otg_CommonUSB_Index, OTG_BULK_IN_EP);
	// read TXCSR register
	txCSR = ReadOTGReg16(otg_IndexedCSR_TxCSR);


	if (gUsbEpTxStatus == USB_EP_STATE_HALT)
	{
		// set TXCSR Send_Stall bit to send a stall to host
		SetBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prw_SendStall);
	}
	else
	{
//		if (TXCSR Sent_Stall set?)
		if (txCSR & TxCSR_prc_SentStall)
		{
			// clear this bit
			ClrBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prc_SentStall);
		}

//		if (TXCSR Under_Run set?)
		if (txCSR & TxCSR_prc_UnderRun)
		{
			// clear this bit
			ClrBitOTGReg16(otg_IndexedCSR_TxCSR, TxCSR_prc_UnderRun);
		}
		
		OtgStorTxProcess();	
		// data send done
	}
}


extern VOID
OtgDeviceRxDoneProcess(VOID)
{
	WORD	rxCSR;//, recvCnt;

	// set endpoint index = 2
	WriteOTGReg8(otg_CommonUSB_Index, OTG_BULK_OUT_EP);
	// read RXCSR register
	rxCSR = ReadOTGReg16(otg_IndexedCSR_RxCSR);

	if (gUsbEpRxStatus == USB_EP_STATE_HALT)
	{
		// set RXCSR Send_Stall bit to send a stall to host
		SetBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_prw_SendStall);
	}
	else
	{
//		if (RXCSR Sent_Stall set?)
		if (rxCSR & RxCSR_prc_SentStall)
		{
			// clear this bit
			ClrBitOTGReg16(otg_IndexedCSR_RxCSR, RxCSR_prc_SentStall);
		}

		OtgStorRxProcess();
	}
	//*/
}


extern VOID
OtgDeviceDMADoneProcess(VOID)
{
	OtgStorDMAProcess();
}


//
// interrupt service
//
extern VOID 
OtgDeviceISR(VOID)
{
	BYTE	intrUsb;
	WORD	intrTx, intrRx;
	DWORD	intrDMA;

	intrUsb = ReadOTGReg8(otg_CommonUSB_IntrUSB);
	intrTx = ReadOTGReg16(otg_CommonUSB_IntrTx);
	intrRx = ReadOTGReg16(otg_CommonUSB_IntrRx);
	intrDMA = ReadOTGReg32(otg_DMA_Intr);

	if (intrUsb & IntUSB_Resume)
	{
		OtgDeviceResume();
	}
	if (intrUsb & IntUSB_DisCon)	//session end
	{
		OtgDeviceSessionEnd();
	}
	if (intrUsb & IntUSB_Suspend)
	{
		OtgDeviceSuspend();
	}
	if (intrUsb & IntUSB_p_Reset)
	{
		OtgDeviceReset();
	}
	if (intrTx & 0x0001)
	{
		OtgDeviceEp0Process();
	}
	if (intrTx & (0x0001 << OTG_BULK_IN_EP))
	{
		OtgDeviceTxDoneProcess();
	}
	if (intrRx & (0x0001 << OTG_BULK_OUT_EP))
	{
		OtgDeviceRxDoneProcess();
	}
	if (intrDMA & DMA_Intr_Ch1)
	{
		OtgDeviceDMADoneProcess();
	}
	//*/
}


extern VOID
OtgDeviceInit(VOID)
{
	// init device and endpoint status
	gUsbDevStatus = USB_STATE_POWERED;
	gUsbEp0Status = gUsbEpTxStatus = gUsbEpRxStatus = USB_EP_STATE_IDLE;

	// config device speed, enable suspendM detection
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

	//enable device high speed
	SetBitOTGReg8(otg_CommonUSB_Power, Power_hrw_HSEnab);	

	// enable ep0 and ep2 Tx,Rx interrupt
	SetBitOTGReg16(otg_CommonUSB_IntrTxEn, (0x0001 | (0x0001 << OTG_BULK_IN_EP)));	//enable ep0 Rx/Tx interrupt and ep2 Tx interrupt
	SetBitOTGReg16(otg_CommonUSB_IntrRxEn, (0x0001 << OTG_BULK_OUT_EP));	//enable ep2 Rx interrupt

	WriteOTGReg8(otg_CommonUSB_FAddr, 0);
	// enable usb suspend, resume, reset interrupt
	WriteOTGReg8(otg_CommonUSB_IntrUSBE, 0xFF);
	
	// set soft connection
	SetBitOTGReg8(otg_CommonUSB_Power, Power_prw_SoftConn);
}
/* ------------------------------ USB Device Interrupt Service End --------------------------------- */

