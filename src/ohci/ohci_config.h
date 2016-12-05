/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	ohci_config.h
*
* Programmer:    	Grey
*
* Created: 	 		09/xx/2008
*
* Description: 		ohci usb host config head file
*              
*        
* Change History (most recent first):	2008.09.12
*****************************************************************/


#ifndef	__OHCI_CONFIG_H__
#define	__OHCI_CONFIG_H__


/*
 * system memory config
 */
#define ORIOLE_I_EN				1
#define	ORIOLE_II_EN			0
#define	ORIOLE_III_EN			0
#define	ORIOLE_V_EN				0
#define	ORIOLE_VI_EN			0

#if ORIOLE_I_EN
#define	SYS_SHARE_BUF_BASE		0xB000		//share buffer start address
#define	SYS_OHCI_REG_BASE		0xAF00		//0xAF00 ~ 0xAFFF		(256 bytes)
#define	SYS_OHCI_MEMORY_BASE	0xF000		//0xF000 ~ 0xF3FF		(1024 bytes)
#endif

#if ORIOLE_II_EN
#define	SYS_SHARE_BUF_BASE		0xB000		//share buffer start address
#define	SYS_OHCI_REG_BASE		0xAF00		//0xAF00 ~ 0xAFFF		(256 bytes)
#define	SYS_OHCI_MEMORY_BASE	0xD400		//0xD400 ~ 0xD7FF		(1024 bytes)
#endif

#if ORIOLE_III_EN
#define	SYS_SHARE_BUF_BASE		0xB000		//share buffer start address
#define	SYS_OHCI_REG_BASE		0xAF00		//0xAF00 ~ 0xAFFF		(256 bytes)
#define	SYS_OHCI_MEMORY_BASE	0xD400		//0xD400 ~ 0xD7FF		(1024 bytes)
#endif

#if ORIOLE_V_EN
#define	SYS_SHARE_BUF_BASE		0x0C00		//share buffer start address
#define	SYS_OHCI_REG_BASE		0x9C00		//0x9C00 ~ 0x9CFF		(256 bytes)
#define	SYS_OHCI_MEMORY_BASE	0x1C00		//0x1C00 ~ 0x1FFF		(1024 bytes)
#endif

#if ORIOLE_VI_EN
#define	SYS_SHARE_BUF_BASE		0x3800		//share buffer start address
#define	SYS_OHCI_REG_BASE		0x9C00		//0x9C00 ~ 0x9CFF		(256 bytes)
#define	SYS_OHCI_MEMORY_BASE	0x6C00		//0xF000 ~ 0xF3FF		(1024 bytes)
#endif

/*
 * host parameter define
 */

/*
 * define OHCI controller reset, if power on failed, add this value 
 */
#define	HC_RESET_TIME_OUT					1000	

#define	HC_SEND_URB_TIME_OUT				20000

#define	ROOT_HUB_PORT_RESET_MS				100

//#define	DEVICE_CONNECTION_MS				50



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



#endif//#ifndef	__OHCI_CONFIG_H__

