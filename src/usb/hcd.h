/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	hcd.c
*
* Programmer:    	Grey
*
* Created: 	 		06/xx/2008
*
* Description: 		usb host controller driver interface head file
*              
*        
* Change History (most recent first):	2008.06.03
*****************************************************************/
#ifndef __HCD_H__
#define	__HCD_H__

/*
 * usb host select
 */
#define	USB_HOST_OHCI_EN		1
#define	USB_HOST_OTG_EN			0


typedef struct _URB
{
	VOID			*Pipe;
	BYTE			Direction;		// For contol and bulk: 0x80 = USB_DIR_IN, 0x00 = USB_DIR_OUT
	BYTE			*SetupPacket;
	BYTE			*Buffer;
	WORD			BufferLength;
	BYTE			Status;
} URB;

extern URB		XDATA	gUrb;


/* -------------------------------------------------------------------------- */

/*
 * For various legacy reasons, Linux has a small cookie that's paired with
 * a struct usb_device to identify an endpoint queue.  Queue characteristics
 * are defined by the endpoint's descriptor.  This cookie is called a "pipe",
 * an unsigned int encoded as:
 *
 *	- maxPacketSize	bit 0-6 exp of maxPacketSize
 *  - direction:	bit 7		(0 = Host-to-Device [Out],
 *					 1 = Device-to-Host [In] ...
 *					like endpoint bEndpointAddress)
 *  - device address:	bits 8-14       ... bit positions known to uhci-hcd
 *  - endpoint:		bits 15-18      ... bit positions known to uhci-hcd
 *  - pipe type:	bits 30-31	(00 = isochronous, 01 = interrupt,
 *					 10 = control, 11 = bulk)
 *
 * Given the device address and endpoint descriptor, pipes are redundant.
 */

/* NOTE:  these are not the standard USB_ENDPOINT_XFER_* values!! */
/* (yet ... they're the values used by usbfs) */
#define PIPE_ISOCHRONOUS		0
#define PIPE_INTERRUPT			1
#define PIPE_CONTROL			2
#define PIPE_BULK				3

#define PipeIn(pipe)		((pipe) & 0x80)
#define PipeOut(pipe)		(!PipeIn(pipe))

#define PipeDevice(pipe)	(((pipe) >> 8) & 0x7F)
#define PipeEndpoint(pipe)	(((pipe) >> 15) & 0xF)

#define PipeType(pipe)		(((pipe) >> 30) & 3)
#define PipeIsoc(pipe)		(PipeType((pipe)) == PIPE_ISOCHRONOUS)
#define PipeInt(pipe)		(PipeType((pipe)) == PIPE_INTERRUPT)
#define PipeControl(pipe)	(PipeType((pipe)) == PIPE_CONTROL)
#define PipeBulk(pipe)		(PipeType((pipe)) == PIPE_BULK)

#define	PipeMaxPacketSize(pipe)		(0x0001 << ((pipe & 0x7) + 3))	

/*-------------------------------------------------------------------------*/



//
// ConditionCode Value
//
#define CC_NoError				0x0
#define CC_CRC					0x1
#define CC_BitStuffing			0x2
#define CC_DataToggleMisMatch	0x3
#define CC_Stall				0x4
#define CC_DeviceNotResponding	0x5
#define CC_PIDCheckFailure		0x6
#define CC_UnexpectedPID		0x7
#define CC_DataOverrun			0x8
#define CC_DataUnderRun			0x9
#define CC_BufferOverrun		0xC
#define CC_BufferUnderrun		0xD
#define CC_NotAccessed			0xF

#define	CC_AllocEDTD			0xA
#define	CC_TimeOut				0xB


extern BOOL
HcdOpen(VOID);

extern VOID*
HcdOpenPipe(
	DWORD		pipeInfo
	);

extern VOID
HcdClosePipe(
	VOID		*pipe
	);

extern VOID
HcdSendUrb(VOID);

extern VOID
HcdGetRHPortStatus(
	BOOL		*isConnect,			/* output: have device connection set 1, else set 0 */
	BOOL		*isChange			/* 			device connection change set 1, else set 0 */
	);

extern VOID
HcdRHPortReset(VOID);

extern VOID
HcdRHClearFeature(VOID);

extern WORD
HcdGetFrameNumber(VOID);

extern VOID
HcdDelayMs(
	WORD		ms
	);

#endif
