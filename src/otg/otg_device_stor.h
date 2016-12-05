/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	otg_device_stor.h
*
* Programmer:    	Grey
*
* Created: 	 		10/xx/2008
*
* Description: 		otg mass storage device head file
*              
*        
* Change History (most recent first):	2008.11.06
*****************************************************************/

#ifndef	__OTG_DEVICE_STOR_H__
#define	__OTG_DEVICE_STOR_H__


//
// define mass storage device status
//
#define	MS_ST_CBW			0
#define	MS_ST_DATA_TX		1
#define	MS_ST_DATA_RX		2
#define	MS_ST_DATA_OP		3
#define	MS_ST_CSW			4
#define	MS_ST_DONE			5



extern VOID
OtgStorRxProcess(VOID);

extern VOID
OtgStorTxProcess(VOID);

extern VOID
OtgStorDMAProcess(VOID);


extern VOID
OtgStorInit(				
	DWORD		blockNum	// mass storage block total number	
	);

//
// You must write this function yourself to process flash data!
//
extern VOID
OtgStorDataFlash2Buffer(	// read blocks from flash to buffer.
	WORD		addr,		// data buffer address
	DWORD		lba,		// logic block address of storage device
	BYTE		size		// prepare block count 
	);

//
// You must write this function yourself to process flash data!
//
extern VOID
OtgStorDataBuffer2Flash(	// write blocks from buffer to flash
	WORD		addr,		// data buffer address
	DWORD		lba,		// logic block address of storage device
	BYTE		size		// prepare block count 
	);

#endif//__OTG_DEVICE_STOR_H__
