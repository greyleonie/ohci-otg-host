/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	host.h
*
* Programmer:    	Grey
*
* Created: 	 		04/xx/2008
*
* Description: 		usb host application interface head file
*              
*        
* Change History (most recent first):	2008.04.21
*****************************************************************/

#ifndef __HOST_H__
#define	__HOST_H__


/*
 * define usb host status
 */
#define	HOST_IDLE					0		/* usb host module idle status, after reset in this status */
#define	HOST_READY					1		/* after HostInit(), if usb host init complete, but no device connection */
#define	HOST_USB_CONN				2		/* have usb device insert to hub port */
#define	HOST_USB_READY				3		/* find valid usb device */
#define	HOST_STOR_READY				4		/* fing valid storage device */
#define	HOST_INIT_ERR				5		/* after HostInit(), if usb host init failed, in this status */
#define	HOST_USB_UNSUPPORT_DEV		6		/* find unsupport usb device */
#define	HOST_TRANSFER_ERR			7		/* data transfer error */


/*
 * init usb host module
 */
extern void
HostInit(void);


/*
 * usb host device status
 */
extern void
HostPoll(
	char		isAutoProcessDevice
	);


/*
 * get usb host status
 */
extern unsigned char
HostGetStatus(void);


/*
 * usb host read storage block
 */
extern unsigned char		/* return: success 0, else failed */
HostReadBlock(
	unsigned long		lba,		/* input: logic block address */
	void				*buf,		/*		  data buffer pointer */
	unsigned int		size		/*		  transfer block size */
	);

extern unsigned char		/* return: success 0, else failed */
HostWriteBlock(
	unsigned long		lba,		/* input: logic block address */
	void				*buf,		/*		  data buffer pointer */
	unsigned int		size		/*		  transfer data length */
	);

/*
 * get storage device block size
 */
extern unsigned int
HostGetBlockSize(void);


/*
 * get storage device last lba number
 */
extern unsigned long
HostGetLastLBA(void);



#endif
