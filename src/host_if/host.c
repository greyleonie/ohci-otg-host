/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	host.c
*
* Programmer:    	Grey
*
* Created: 	 		04/xx/2008
*
* Description: 		usb host application interface
*              
*        
* Change History (most recent first):	2008.04.21
*****************************************************************/

#include <string.h>
#include "../public/type.h"
#include "../public/func.h"
#include "../usb/hcd.h"
#include "../usb/usb_ch9.h"
#include "../usb/usb.h"
#include "../storage/stor.h"
#include "host.h"


unsigned char	gHostStatus = HOST_IDLE;


/*
 * init usb host module
 */
extern void
HostInit(void)
{
	if (!HcdOpen())		//init ohci host controller
		gHostStatus = HOST_INIT_ERR;
	gHostStatus = HOST_READY;
}


/*
 * usb host device status
 */
extern void
HostPoll(
	char		isAutoProcessDevice
	)
{
	char				isConn = 0;
	char				isChange = 0;
	static	char		checkCnt = 0;

	HcdGetRHPortStatus(&isConn, &isChange);		//get current usb connection status

	if (isConn)		
	{
		if (gHostStatus == HOST_READY)			//new usb device insert
		{
			if (isAutoProcessDevice)
			{
				HcdDelayMs(50);
				HcdGetRHPortStatus(&isConn, &isChange);
				if (!isConn)
					return;
				checkCnt = 0;
			}
			gHostStatus = HOST_USB_CONN;
		}/* end if (gHostStatus == HOST_READY) */

		if (gHostStatus == HOST_USB_CONN && isAutoProcessDevice)		//enum usb device
		{
			/* reset port */
			HcdRHClearFeature();
			HcdRHPortReset();
	
			/* start usb device */
			UsbClose();			//FIX it, free usb endpoint source
			UsbOpen();
			if (!UsbStartDevice())
			{
				checkCnt++;
				if (checkCnt == 50)
				{
					checkCnt = 0;
					gHostStatus = HOST_USB_UNSUPPORT_DEV;
				}
				return;
			}
			checkCnt = 0;
			gHostStatus = HOST_USB_READY;
			
			/* start storage device */
			StorOpen();
		}/* end if (gHostStatus == HOST_USB_CONN && isAutoProcessDevice) */

		if (gHostStatus == HOST_USB_READY && isAutoProcessDevice)		//polling storage device
		{
			if (!StorStartDevice())
				return;
			gHostStatus = HOST_STOR_READY;
		}
	}/* end if (isConn) */
	else
	{
		if (isChange && isAutoProcessDevice)			//usb device disconnection
		{
			HcdRHClearFeature();
			UsbClose();			//free usb endpoint source
		}
		gHostStatus = HOST_READY;
	}/* end else */
}


/*
 * get usb host status
 */
extern unsigned char
HostGetStatus(void)
{
	return gHostStatus;
}


/*
 * usb host read storage block
 */
extern unsigned char		/* return: success 0, else failed */
HostReadBlock(
	unsigned long		lba,		/* input: logic block address */
	void				*buf,		/*		  data buffer pointer */
	unsigned int		size		/*		  transfer data length */
	)
{
	char		retVal;
	char		isConn = 0;
	char		isChange = 0;

	retVal = StorReadWriteBlock(lba, buf, size, 1);

	if (retVal != 0)	//read block falied
	{	
		HcdGetRHPortStatus(&isConn, &isChange);		//get current usb connection status
		if (!isConn)
		{
			gHostStatus = HOST_READY;
			HcdRHClearFeature();
			UsbClose();			//free usb endpoint source
		}
		else
		{
			if (isChange)
			{
				gHostStatus = HOST_READY;
				HcdRHClearFeature();
				UsbClose();			//free usb endpoint source
			}
			else
			{
				gHostStatus = HOST_USB_READY;
			}
		}
	}/* end if (retVal != 0) */

	return retVal;
}


extern unsigned char		/* return: success 0, else failed */
HostWriteBlock(
	unsigned long		lba,		/* input: logic block address */
	void				*buf,		/*		  data buffer pointer */
	unsigned int		size		/*		  transfer data length */
	)
{
	char		retVal;
	char		isConn = 0;
	char		isChange = 0;

	retVal = StorReadWriteBlock(lba, buf, size, 0);

	if (retVal != 0)	//read block falied
	{	
		HcdGetRHPortStatus(&isConn, &isChange);		//get current usb connection status
		if (!isConn)
		{
			gHostStatus = HOST_READY;
			HcdRHClearFeature();
			UsbClose();			//free usb endpoint source
		}
		else
		{
			if (isChange)
			{
				gHostStatus = HOST_READY;
				HcdRHClearFeature();
				UsbClose();			//free usb endpoint source
			}
			else
			{
				gHostStatus = HOST_USB_READY;
			}
		}
	}/* end if (retVal != 0) */

	return retVal;
}

/*
 * get storage device block size
 */
extern unsigned int
HostGetBlockSize(void)
{
	return StorGetBlockSize();
}


/*
 * get storage device last lba number
 */
extern unsigned long
HostGetLastLBA(void)
{
	return StorGetLastLBA();
}

