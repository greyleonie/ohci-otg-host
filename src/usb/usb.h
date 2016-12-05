/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	usb.h
*
* Programmer:    	Grey
*
* Created: 	 		04/xx/2008
*
* Description: 		usb driver head file
*              
*        
* Change History (most recent first):	2008.04.16
*****************************************************************/

#ifndef __USB_H__
#define	__USB_H__



typedef struct _USB_DEVICE
{
	BYTE		DeviceAddress;
	BYTE		ActiveConfigNum;
	BYTE		ActiveInterfaceNum;
	BYTE		BulkInEpNum;
	BYTE		BulkOutEpNum;
	VOID		*ControlPipe;
	VOID		*BulkInPipe;
	VOID		*BulkOutPipe;	
} USB_DEVICE;

extern USB_DEVICE	XDATA	gUsbDevice;


extern VOID
UsbOpen(VOID);

extern VOID
UsbClose(VOID);

extern BOOL
UsbStartDevice(VOID);

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
	);

/*
 *	USB bulk transfer massage operation
 *	This is the interface for device bulk transfer
 */
extern BYTE				/* Return: usb operation error code */
UsbBulkMsg(
	VOID			*dat,				/*        bulk transfer data buffer pointer */
	WORD			size,				/*        bulk transfer data length */
	BYTE			direction
	);

/*
 *	Protocol clear endpoint stall
 * 	This function use clear feature request, only clear endpoint stall.
 */
extern BYTE				/* Return: usb operation error code */
UsbClearHalt(
	BYTE			endpointNum				/*        endpoint number */
	);

#endif