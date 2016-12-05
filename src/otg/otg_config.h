/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	otg_host_config.h
*
* Programmer:    	Grey
*
* Created: 	 		09/xx/2008
*
* Description: 		otg usb host config head file
*              
*        
* Change History (most recent first):	2008.11.06
*****************************************************************/

#ifndef	__OTG_CONFIG_H__
#define	__OTG_CONFIG_H__

/*---------------- otg controller register allocate -----------------*/
#define	CODY_FPGA_TEST_EN		0		//if using cody's old version FPGA set 1, else set 0.

#if CODY_FPGA_TEST_EN

/* system register define */
#define	SYS_SHARE_BUF_BASE			0xB000		//share buffer start address
#define	SYS_OTG_REG_BASS			0x9C00		//otg register bass address
#define	SYS_OTG_ACCESS_BASS			0x9F20		//otg mcu access register bass address

#else//#if CODY_FPGA_TEST_EN

/* system register define */
#define ORIOLE_VII_EN				1		//if using Oriole VII chip or FPGA set 1, else set 0.

#if ORIOLE_VII_EN
#define	SYS_SHARE_BUF_BASE			0x1400		//share buffer start address
#define	SYS_OTG_REG_BASS			0xA200		//otg register bass address
#define	SYS_OTG_ACCESS_BASS			0xA520		//otg mcu access register bass address
#endif//#if ORIOLE_VII_EN

#endif//#if CODY_FPGA_TEST_EN
/*---------------- otg controller register allocate end -----------------*/


/*---------------- otg host controller hardware config -----------------*/
#define	HOST_HIGH_SPEED_EN			0		/* enable host high-speed function */


#define	MAX_HC_ENDPOINT_NUM			3		/* host hardware endpoint number 0 ~ 2 */

#define	TRANSACTION_TIME_OUT		5000	/* wait TxPktRdy bit clear and RxPktRdy bit set time out */
/*---------------- otg host controller hardware config end -----------------*/


/*---------------- otg device hardware config ----------------*/
#define	OTG_EP0_MAX_PACKET_SIZE			64		
#define	OTG_BULK_HS_MAX_PACKET_SIZE		512		
#define	OTG_BULK_FS_MAX_PACKET_SIZE		64		

#define	OTG_BULK_IN_EP				2
#define	OTG_BULK_OUT_EP				2	

#define	SYS_DATA_BUFFER_ADDRESS			0x1400
#define	SYS_DATA_BUFFER_LENGTH			512
#define	SYS_DATA_BLOCK_SIZE				512
/*---------------- otg device hardware config end ----------------*/


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

#if CODY_FPGA_TEST_EN
#define	APPLY_MEM()			ApplyREQ()
#define	RELEASE_MEM()		ReleaseREQ()
#else
#define	APPLY_MEM()
#define	RELEASE_MEM()
#endif

#endif//#ifndef	__OTG_CONFIG_H__

