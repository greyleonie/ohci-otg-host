/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	host_config.h
*
* Programmer:    	Grey
*
* Created: 	 		04/xx/2008
*
* Description: 		usb host config head file
*              
*        
* Change History (most recent first):	2008.04.10
*****************************************************************/

#ifndef __HOST_CONFIG_H__
#define	__HOST_CONFIG_H__

#define	USB_HOST_OHCI_EN	0
#define	USB_HOST_OTG_EN		1


#if	USB_HOST_OHCI_EN
/*
 * system memory config
 */
#define	SYS_SHARE_BUF_BASE		0xB000		//share buffer start address
#define	SYS_OHCI_REG_BASE		0xAF00		//0xAF00 ~ 0xAFFF		(256 bytes)
#define	SYS_OHCI_MEMORY_BASE	0xF000		//0xF000 ~ 0xF3FF		(1024 bytes)


/*
 * host parameter define
 */

/*
 * define OHCI controller reset, if power on failed, add this value 
 */
#define	HC_RESET_TIME_OUT					1000	

#define	HC_SEND_URB_TIME_OUT				20000

#define	ROOT_HUB_PORT_RESET_MS				100

#define	DEVICE_CONNECTION_MS				50

#endif//#if	USB_HOST_OHCI_EN




#if USB_HOST_OTG_EN

/* system register define */
#define	SYS_OTG_REG_BASS		0x9C00
#define	SYS_OTG_ACCESS_BASS		0x9F20

/* otg host controller hardware config */
#define	HIGH_SPEED_EN				0		/* enable host high-speed function */

#define	MAX_HC_ENDPOINT_NUM			5		/* host hardware endpoint number 0 ~ 4 */

#define	CTRL_EP_FIFO_SIZE			64		/* control endpoint fifo size 64 bytes */
#define	BULK_EP_FIFO_SIZE			512		/* Tx and Rx endpoint fifo size 512 bytes */

#define	TRANSACTION_TIME_OUT		5000	/* wait TxPktRdy bit clear and RxPktRdy bit set time out */

#define	DEVICE_CONNECTION_MS		50


#endif//#if USB_HOST_OTG_EN



/*
 * define critical area operation
 */

#define	ENTER_CRITICAL()
#define	EXIT_CRITICAL()


/*
 * define share buffer access
 */
extern void 
ApplyREQ(void);

extern void
ReleaseREQ(void);

#define	APPLY_MEM()			ApplyREQ()
#define	RELEASE_MEM()		ReleaseREQ()



#endif