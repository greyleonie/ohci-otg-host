/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	stor.h
*
* Programmer:    	Grey
*
* Created: 	 		04/xx/2008
*
* Description: 		usb mass storage device driver head file
*              
*        
* Change History (most recent first):	2008.04.17
*****************************************************************/

#ifndef __STOR_H__
#define	__STOR_H__

//#include "usb.h"


//#define	MAX_STOR_LOGICAL_UNIT_NUM		4


/*
 *	Logical Unit structure
 */
typedef struct _LOGICAL_UNIT
{
	BYTE			LuNum;
	WORD			BlockSize;
	DWORD			LastLBA;
}LOGICAL_UNIT;


/*
 *	Mass Storage device structure
 */
typedef struct _STORAGE
{
	BYTE			MaxLUN;
	LOGICAL_UNIT	CurrLu;//[MAX_STOR_LOGICAL_UNIT_NUM];	
} STORAGE;



/*
 *	command block wrapper
 */
typedef struct _CBW
{
	DWORD		Signature;
	DWORD		Tag;
	DWORD		DataTransferLength;
	BYTE		Flags;
	BYTE		Lun;
	BYTE		Length;
	BYTE		CDB[16];
}CBW;

#define US_BULK_CB_SIGN			0x43425355	/*spells out USBC */
#define US_BULK_FLAG_IN			1
#define US_BULK_FLAG_OUT		0

/*
 *	command status wrapper
 */
typedef struct _CSW
{
	DWORD		Signature;
	DWORD		Tag;
	DWORD		Residue;
	BYTE		Status;
}CSW;

#define US_BULK_CS_SIGN			0x53425355	/* spells out 'USBS' */
#define US_BULK_STAT_OK			0
#define US_BULK_STAT_FAIL		1
#define US_BULK_STAT_PHASE		2

/* bulk-only class specific requests */
#define US_BULK_RESET_REQUEST	0xff
#define US_BULK_GET_MAX_LUN		0xfe




/* Sub Classes */

#define US_SC_RBC			0x01		/* Typically, flash devices */
#define US_SC_8020			0x02		/* CD-ROM */
#define US_SC_QIC			0x03		/* QIC-157 Tapes */
#define US_SC_UFI			0x04		/* Floppy */
#define US_SC_8070			0x05		/* Removable media */
#define US_SC_SCSI			0x06		/* Transparent */
//#define US_SC_ISD200    	0x07		/* ISD200 ATA */
//#define US_SC_MIN			US_SC_RBC
//#define US_SC_MAX			US_SC_ISD200

/* Protocols */

#define US_PR_CBI			0x00		/* Control/Bulk/Interrupt */
#define US_PR_CB			0x01		/* Control/Bulk w/o interrupt */
#define US_PR_BULK			0x50		/* bulk only */
//#define US_PR_SCM_ATAPI		0x80		/* SCM-ATAPI bridge */
//#define US_PR_EUSB_SDDR09	0x81		/* SCM-SCSI bridge for SDDR-09 */
//#define US_PR_SDDR55		0x82		/* SDDR-55 (made up) */
//#define US_PR_DPCM_USB  	0xf0		/* Combination CB/SDDR09 */
//#define US_PR_FREECOM   	0xf1		/* Freecom */
//#define US_PR_DATAFAB   	0xf2		/* Datafab chipsets */
//#define US_PR_JUMPSHOT  	0xf3		/* Lexar Jumpshot */

#define US_PR_DEVICE		0xff		/* Use device's value */


/* SCSI opcodes */

#define TEST_UNIT_READY       0x00
#define REQUEST_SENSE  		  0x03
#define READ_6                0x08
#define WRITE_6               0x0a
#define SEEK_6                0x0b
#define INQUIRY               0x12
#define MODE_SELECT           0x15
#define MODE_SENSE            0x1a
#define ALLOW_MEDIUM_REMOVAL  0x1e
#define	READ_FORMAT_CAPACITY  0x23
#define READ_CAPACITY         0x25
#define READ_10               0x28
#define WRITE_10              0x2a
#define SEEK_10               0x2b
#define MODE_SELECT_10        0x55
#define MODE_SENSE_10         0x5a
#define READ_12               0xa8
#define WRITE_12              0xaa
#define READ_DISC_STRUCTURE   0xad
#define DVD_MACHANISM_STATUS  0xbd
#define REPORT_KEY			  0xa4
#define SEND_KEY			  0xa3
#define READ_TOC			  0x43
#define READ_MSF			  0xb9


/*
 *	scsi command
 */
typedef struct _SCSI_CMD
{
	BYTE		Cmd;
	BYTE		CmdLen;
	BYTE		Lun;
	BYTE		DataDirection;
	VOID		*Buf;
	WORD		BufLen;
	DWORD		Lba;
	WORD		BlockSize;
}SCSI_CMD;


extern VOID
StorOpen(VOID);

extern BYTE
StorReadWriteBlock(
//	BYTE		lun,
	DWORD		lba,
	VOID		*buf,
	WORD		size,
	BOOL		read	
	);

extern BOOL
StorStartDevice(VOID);

extern DWORD
StorGetLastLBA(VOID);

extern WORD
StorGetBlockSize(VOID);



#endif
