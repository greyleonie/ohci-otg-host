/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	usb.c
*
* Programmer:    	Grey
*
* Created: 	 		04/xx/2008
*
* Description: 		usb driver function
*              
*        
* Change History (most recent first):	2008.04.16
*****************************************************************/

#include <string.h>
#include "../public/type.h"
#include "../public/func.h"
#include "hcd.h"
#include "usb_ch9.h"
#include "usb.h"


USB_DEVICE	XDATA	gUsbDevice;


/*
 *	Open usb and init it
 */
extern VOID
UsbOpen(VOID)
{
	memset(&gUsbDevice, 0, sizeof(USB_DEVICE));
}


/*
 * Close usb device
 */
extern VOID
UsbClose(VOID)
{
	if (gUsbDevice.BulkInPipe != NULL)
	{
		HcdClosePipe(gUsbDevice.BulkInPipe);
		gUsbDevice.BulkInPipe = NULL;
	}

	if (gUsbDevice.BulkOutPipe != NULL)
	{
		HcdClosePipe(gUsbDevice.BulkOutPipe);
		gUsbDevice.BulkOutPipe = NULL;
	}

	if (gUsbDevice.ControlPipe != NULL)
	{
		HcdClosePipe(gUsbDevice.ControlPipe);
		gUsbDevice.ControlPipe = NULL;
	}
}


/*
 *	USB control transfer massage operation
 *	Reference to chapter 9 of USB2.0 spec for the standard USB request
 */
extern BYTE				/* Return: usb operation error code */
UsbControlMsg(
	BYTE			request,			/*        request number */
	BYTE			requestType,		/*        request type */
	WORD			value,				/*        request value */
	WORD			index,				/*        request index */
	WORD			size,				/*        request size */
	VOID			*dat,				/*        request data buffer pointer */
	BYTE			direction
	)
{
	BYTE	XDATA	setup[8];

	/* config setup packet */
	setup[0] = requestType;
	setup[1] = request;
	setup[2] = (BYTE)value;
	setup[3] = (BYTE)(value >> 8);
	setup[4] = (BYTE)index;
	setup[5] = (BYTE)(index >> 8);
	setup[6] = (BYTE)size;
	setup[7] = (BYTE)(size >> 8);


	/* set urb */

	gUrb.Pipe = gUsbDevice.ControlPipe;		
	gUrb.Direction = direction;
	gUrb.SetupPacket = setup;
	gUrb.Buffer = dat;
	gUrb.BufferLength = size;
	gUrb.Status = 0;

	HcdSendUrb();

	return gUrb.Status;
}


/*
 *	USB bulk transfer massage operation
 *	This is the interface for device bulk transfer
 */
extern BYTE				/* Return: usb operation error code */
UsbBulkMsg(
	VOID			*dat,				/*        bulk transfer data buffer pointer */
	WORD			size,				/*        bulk transfer data length */
	BYTE			direction
	)
{
	BYTE		endpointNum;
	BYTE		retVal;

	if (direction == USB_DIR_IN)
	{
		gUrb.Pipe = gUsbDevice.BulkInPipe;
		endpointNum = gUsbDevice.BulkInEpNum;
	}
	else
	{
		gUrb.Pipe = gUsbDevice.BulkOutPipe;
		endpointNum = gUsbDevice.BulkOutEpNum;
	}
	gUrb.Direction = direction;
	gUrb.SetupPacket = NULL;
	gUrb.Buffer = dat;
	gUrb.BufferLength = size;
	gUrb.Status = 0;

	HcdSendUrb();
	retVal = gUrb.Status;
	if (retVal == CC_Stall)
		UsbClearHalt(endpointNum);		/* clear endpoint stall */

	return retVal;
}


/*
 *	Protocol set usb device address 
 */
extern BYTE				/* Return: usb operation error code */
UsbSetAddress(
	BYTE		address
	)
{
	/* send set address request */
	return UsbControlMsg(USB_REQ_SET_ADDRESS,
						(USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE),
						address,
						0,
						0,
						NULL,
						USB_DIR_OUT
						);
}


/*
 *	Protocol get usb device standard configuration descriptor
 *	USB get configuration descriptor
 */
extern BYTE				/* Return: usb operation error code */
UsbGetDescriptor(
	BYTE			type,				/*        descriptor type */
	BYTE			index,				/*        descriptor index */
	VOID			*buf,				/*        descriptor data buffer */
	WORD			size				/*        descriptor size */
	)
{
	WORD	XDATA	value;

	/* set wValue of request */
	value = type;
	value <<= 8;
	value += index;

	/* set get descriptor request */
	return UsbControlMsg(USB_REQ_GET_DESCRIPTOR,
						(USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE),
						value,
						0,
						size,
						buf,
						USB_DIR_IN
						);
}


/*
 *	Protocol set usb device configuration
 */
extern BYTE				/* Return: usb operation error code */
UsbSetConfiguration(
	BYTE			configuration		/*        active configuration number for this device */
	)
{
	/* send set configuration request */
	return UsbControlMsg(USB_REQ_SET_CONFIGURATION,
						(USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE),
						configuration,
						0,
						0,
						NULL,
						USB_DIR_OUT
						);
}


/*
 *	Protocol clear endpoint stall
 * 	This function use clear feature request, only clear endpoint stall.
 */
extern BYTE				/* Return: usb operation error code */
UsbClearHalt(
	BYTE			endpointNum				/*        endpoint number */
	)
{
	/* send clear feature request */
	return UsbControlMsg(USB_REQ_CLEAR_FEATURE,
						(USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_ENDPOINT),
						USB_ENDPOINT_HALT,
						endpointNum,
						0,
						NULL,
						USB_DIR_OUT
						);
}


/*
 *	Start a usb device
 */
extern BOOL
UsbStartDevice(VOID)
{
	BYTE		XDATA	descriptorBuf[64];
	DWORD		XDATA	pipeInfo;
	WORD		XDATA	maxPacketSize;
	VOID		XDATA	*controlPipe, *bulkPipe;
	BYTE		XDATA	descriptorLength = 8;
	BYTE		XDATA	maxPacketSize0 = 8;
	BYTE		XDATA	address = 1;
	BYTE		XDATA	i, j, k;
	BYTE		XDATA	*buf;
	USB_DEVICE_DESCRIPTOR		XDATA	*device;
	USB_CONFIG_DESCRIPTOR		XDATA	*config;
	USB_INTERFACE_DESCRIPTOR	XDATA	*interface;
	USB_ENDPOINT_DESCRIPTOR		XDATA	*endpoint;

	/* open control pipe */
	pipeInfo = 0;
	pipeInfo |= PIPE_CONTROL;
	pipeInfo <<= 30;
	controlPipe = HcdOpenPipe(pipeInfo);

	if (controlPipe == NULL)
		goto USB_START_DEVICE_ERR;
	gUsbDevice.ControlPipe = controlPipe;

	/* first get device descriptor */
	if (UsbGetDescriptor(USB_DT_DEVICE, 0, descriptorBuf, descriptorLength) != 0)
		goto USB_START_DEVICE_ERR;
	descriptorLength = descriptorBuf[0];
	maxPacketSize0 = descriptorBuf[7];

	/* reset root hub port */	
	HcdRHPortReset();

	/* set address */
	if (UsbSetAddress(address) != 0)
		goto USB_START_DEVICE_ERR;
	gUsbDevice.DeviceAddress = address;

	/* close control pipe */
	HcdClosePipe(controlPipe);

	/* update control pipe information and open control pipe */
	pipeInfo = 0;
	pipeInfo |= PIPE_CONTROL;
	pipeInfo <<= 22;

	pipeInfo |= address;		//set device address
	pipeInfo <<= 8;

	pipeInfo |= (GetExp2(maxPacketSize0) - 3);		//set maxpacketsize exp
	controlPipe = HcdOpenPipe(pipeInfo);

	if (controlPipe == NULL)
		goto USB_START_DEVICE_ERR;
	gUsbDevice.ControlPipe = controlPipe;

	/* second get device descriptor */
	if (UsbGetDescriptor(USB_DT_DEVICE, 0, descriptorBuf, descriptorLength) != 0)
		goto USB_START_DEVICE_ERR;
	device = (USB_DEVICE_DESCRIPTOR	XDATA *)descriptorBuf;

	for (i = 0; i < device->bNumConfigurations; ++i)
	{
		/* first get config descriptor */
		descriptorLength = 9;
		if (UsbGetDescriptor(USB_DT_CONFIG, 0, descriptorBuf, descriptorLength) != 0)
			goto USB_START_DEVICE_ERR;
		config = (USB_CONFIG_DESCRIPTOR	XDATA *)descriptorBuf;
		descriptorLength = (descriptorBuf[2] < 64) ? descriptorBuf[2] : 64;
	
		/* get all config descriptor */
		if (UsbGetDescriptor(USB_DT_CONFIG, 0, descriptorBuf, descriptorLength) != 0)
			goto USB_START_DEVICE_ERR;
		buf = descriptorBuf;
		buf += USB_DT_CONFIG_SIZE;
	
		/* anaylze all config descriptor, open bulk pipe */
		for (j = 0; j < config->bNumInterfaces; ++j)
		{
			interface = (USB_INTERFACE_DESCRIPTOR	XDATA *)buf;
			buf += USB_DT_INTERFACE_SIZE;
			if (interface->bInterfaceClass != USB_CLASS_MASS_STORAGE 
				|| (interface->bInterfaceSubClass != 5 && interface->bInterfaceSubClass != 6)
				|| interface->bInterfaceProtocol != 0x50)
				continue;
			
			for (k = 0; k < interface->bNumEndpoints; ++k)
			{
				endpoint = (USB_ENDPOINT_DESCRIPTOR	XDATA *)buf;
				buf += USB_DT_ENDPOINT_SIZE;

				if ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK)
				{
					/* open a bulk pipe */
					pipeInfo = 0;
					pipeInfo |= PIPE_BULK;		//set pipe type
					pipeInfo <<= 15;

					pipeInfo |= (endpoint->bEndpointAddress & 0x0F);	//set endpoint address
					pipeInfo <<= 7;

					pipeInfo |= address;		//set device address
					pipeInfo <<= 8;
		
					pipeInfo |= (endpoint->bEndpointAddress & 0x80);	//set direction

					maxPacketSize = LeToBe16(endpoint->wMaxPacketSize);
					pipeInfo |= (GetExp2(maxPacketSize) - 3);		//set maxpacketsize exp

					bulkPipe = HcdOpenPipe(pipeInfo);

					if (bulkPipe == NULL)
						goto USB_START_DEVICE_ERR;

					if ((endpoint->bEndpointAddress & 0x80) != 0)
					{
						gUsbDevice.BulkInEpNum = endpoint->bEndpointAddress;
						gUsbDevice.BulkInPipe = bulkPipe;
					}
					else
					{
						gUsbDevice.BulkOutEpNum = endpoint->bEndpointAddress;
						gUsbDevice.BulkOutPipe = bulkPipe;
					}
				}/* end if */
			}/* end for k */

			if (gUsbDevice.BulkInPipe != NULL && gUsbDevice.BulkOutPipe != NULL)		
			{
				if (UsbSetConfiguration(config->bConfigurationValue) != 0)			//config storage device
					goto USB_START_DEVICE_ERR;
				gUsbDevice.ActiveConfigNum = config->bConfigurationValue;
				gUsbDevice.ActiveInterfaceNum = interface->bInterfaceNumber;
				return TRUE;
			}/* end if */
		}/* end for j */
	}/* end for i */

USB_START_DEVICE_ERR:

	UsbClose();
	
	return FALSE;
}



