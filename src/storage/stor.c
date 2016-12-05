/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	stor.c
*
* Programmer:    	Grey
*
* Created: 	 		04/xx/2008
*
* Description: 		usb mass storage device driver
*              
*        
* Change History (most recent first):	2008.04.17
*****************************************************************/

#include <string.h>
#include "../public/type.h"
#include "../public/func.h"
#include "../usb/hcd.h"
#include "../usb/usb_ch9.h"
#include "../usb/usb.h"
#include "stor.h"


STORAGE		XDATA	gStor;
SCSI_CMD	XDATA	gScsiCmd;
BYTE		XDATA	gStorStatus = 0;


/*
 * open storage device
 */
extern VOID
StorOpen(VOID)
{
	memset(&gStor, 0, sizeof(STORAGE));
	memset(&gScsiCmd, 0, sizeof(SCSI_CMD));
	gStorStatus = 0;
}


/*
 * get storage max lun
 */
extern BYTE
StorGetMaxLUN(VOID)
{
	BYTE		to, retVal;

	to = 3;
	while(to)
	{
		/* send get max lun request */
		retVal = UsbControlMsg(US_BULK_GET_MAX_LUN,
							(USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE),
							0,
							gUsbDevice.ActiveInterfaceNum,
							1,
							&gStor.MaxLUN,
							USB_DIR_IN
							);
		if (retVal == CC_NoError)
			break;
		else if (retVal == CC_Stall)
			UsbClearHalt(0);

		to--;
	}/* end while */
	if (to == 0)
		gStor.MaxLUN = 0;		//device do not support this command

	return retVal;
}


/*
 * reset storage device
 */
extern BYTE
StorResetRecovery(VOID)
{
	/* send get max lun request */
	return UsbControlMsg(US_BULK_RESET_REQUEST,
						(USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE),
						0,
						gUsbDevice.ActiveInterfaceNum,
						0,
						NULL,
						USB_DIR_OUT
						);
}


/*
 * fill scsi command to memory
 */
extern VOID
StorFillScsiCmd(
	BYTE		*buf
	)
{
	memset(buf, 0, 16);
	switch(gScsiCmd.Cmd)
	{
		case INQUIRY:
			buf[0] = INQUIRY;
//			buf[1] = gScsiCmd.Lun;		/* set scsi command lun */
//			buf[1] <<= 5;
			buf[4] = gScsiCmd.BufLen;			/* set transfer length */
			break;

		case TEST_UNIT_READY:
			buf[0] = TEST_UNIT_READY;
//			buf[1] = gScsiCmd.Lun;		/* set scsi command lun */
//			buf[1] <<= 5;
//			buf[4] = 0;			/* set transfer length */
			break;

		case REQUEST_SENSE:
			buf[0] = REQUEST_SENSE;
//			buf[1] = gScsiCmd.Lun;		/* set scsi command lun */
//			buf[1] <<= 5;
			buf[4] = gScsiCmd.BufLen;			/* set transfer length */
			break;

		case READ_CAPACITY:
			buf[0] = READ_CAPACITY;
//			buf[1] = gScsiCmd.Lun;		/* set scsi command lun */
//			buf[1] <<= 5;
			break;

		case MODE_SENSE:
			buf[0] = MODE_SENSE;
//			buf[1] = gScsiCmd.Lun;		/* set scsi command lun */
//			buf[1] <<= 5;
			break;

		case READ_10:
			buf[0] = READ_10;

//			buf[1] = gScsiCmd.Lun;		/* set scsi command lun */
//			buf[1] <<= 5;

			buf[2] = (gScsiCmd.Lba >> 24);		/* set LBA */
			buf[3] = (gScsiCmd.Lba >> 16);
			buf[4] = (gScsiCmd.Lba >> 8);
			buf[5] = gScsiCmd.Lba;

			buf[7] = ((gScsiCmd.BufLen / gScsiCmd.BlockSize) >> 8);		/* set transfer length */
			buf[8] = gScsiCmd.BufLen / gScsiCmd.BlockSize;
			break;

		case WRITE_10:
			buf[0] = WRITE_10;

//			buf[1] = gScsiCmd.Lun;		/* set scsi command lun */
//			buf[1] <<= 5;

			buf[2] = (gScsiCmd.Lba >> 24);		/* set LBA */
			buf[3] = (gScsiCmd.Lba >> 16);
			buf[4] = (gScsiCmd.Lba >> 8);
			buf[5] = gScsiCmd.Lba;

			buf[7] = ((gScsiCmd.BufLen / gScsiCmd.BlockSize) >> 8);		/* set transfer length */
			buf[8] = gScsiCmd.BufLen / gScsiCmd.BlockSize;
			break;

		default:
			break;
	}/* end switch */
}


/*
 * storage bulk transport interface
 */
extern BYTE
StorBulkTransport(VOID)
{
	CBW			cbw;
	CSW			csw;
	BYTE		scsi[16];
	BYTE		transportStatus = 0;
	BYTE		direction;//, endpointNum;
	BYTE		status;

	/* file scsi command string */
	StorFillScsiCmd(scsi);	

	/* config CBW */
	memset(&cbw, 0, sizeof(CBW));
	cbw.Signature = LeToBe32(US_BULK_CB_SIGN);
	cbw.Tag = 0;
	cbw.DataTransferLength = LeToBe32(gScsiCmd.BufLen);
	cbw.Flags = (gScsiCmd.DataDirection << 7);
	cbw.Lun = gScsiCmd.Lun;
	cbw.Length = gScsiCmd.CmdLen;
	memcpy(cbw.CDB, scsi, sizeof(cbw.CDB));	

	direction = (gScsiCmd.DataDirection == US_BULK_FLAG_IN) ? USB_DIR_IN : USB_DIR_OUT;

	/* send CBW */
	status = UsbBulkMsg(&cbw, sizeof(CBW), USB_DIR_OUT);
	if (status != CC_NoError && status != CC_Stall)
		return US_BULK_STAT_PHASE;

	if (gScsiCmd.Buf != NULL)
	{
		/* receive or send data */
		status = UsbBulkMsg(gScsiCmd.Buf, gScsiCmd.BufLen, direction);
		if (status != CC_NoError && status != CC_Stall)
			return US_BULK_STAT_PHASE;
	}/* end if (gScsiCmd.Buf != NULL) */

	/* receive CSW */
	status = UsbBulkMsg(&csw, sizeof(CSW), USB_DIR_IN);
	if (status != CC_NoError)
	{
		if (status != CC_Stall)
			return US_BULK_STAT_PHASE;
		status = UsbBulkMsg(&csw, sizeof(CSW), USB_DIR_IN);
		if (status != CC_NoError)
		{
			if (status == CC_Stall)
				StorResetRecovery();
			return US_BULK_STAT_PHASE;
		}
	}/* end if (status != CC_NoError) */

	/* check CSW status */
	if (LeToBe32(csw.Signature) != US_BULK_CS_SIGN || csw.Tag != 0)
		return US_BULK_STAT_PHASE;

	return csw.Status;
}


/*
 * storage inquiry command
 */
extern BYTE
StorInquiry(
	BYTE			lun,
	BYTE			*buf,
	BYTE			size
	)
{
	gScsiCmd.Cmd = INQUIRY;
	gScsiCmd.CmdLen = 6;
	gScsiCmd.Lun = lun;
	gScsiCmd.DataDirection = US_BULK_FLAG_IN;
	gScsiCmd.Buf = buf;
	gScsiCmd.BufLen = size;

	return StorBulkTransport();
}


/*
 * storage test unit ready command
 */
extern BYTE
StorTestUnitReady(
	BYTE			lun
	)
{
	gScsiCmd.Cmd = TEST_UNIT_READY;
	gScsiCmd.CmdLen = 6;
	gScsiCmd.Lun = lun;
	gScsiCmd.DataDirection = US_BULK_FLAG_OUT;
	gScsiCmd.Buf = NULL;
	gScsiCmd.BufLen = 0;

	return StorBulkTransport();
}


/*
 * storage request sense command
 */
extern BYTE
StorRequestSense(
	BYTE			lun,
	BYTE			*buf,
	BYTE			size
	)
{
	gScsiCmd.Cmd = REQUEST_SENSE;
	gScsiCmd.CmdLen = 12;
	gScsiCmd.Lun = lun;
	gScsiCmd.DataDirection = US_BULK_FLAG_IN;
	gScsiCmd.Buf = buf;
	gScsiCmd.BufLen = size;

	return StorBulkTransport();
}


/*
 * storage read capacity command
 */
extern BYTE
StorReadCapacity(
	BYTE			lun,
	BYTE			*buf,
	BYTE			size
	)
{
	gScsiCmd.Cmd = READ_CAPACITY;
	gScsiCmd.CmdLen = 10;
	gScsiCmd.Lun = lun;
	gScsiCmd.DataDirection = US_BULK_FLAG_IN;
	gScsiCmd.Buf = buf;
	gScsiCmd.BufLen = size;

	return StorBulkTransport();
}


/*
 * storage read block command
 */
extern BYTE
StorReadWriteBlock(
//	BYTE		lun,
	DWORD		lba,
	VOID		*buf,
	WORD		size,
	BOOL		read	
	)
{
	LOGICAL_UNIT	*lu;

	lu = &gStor.CurrLu;

	if (read)
	{
		gScsiCmd.Cmd = READ_10;
		gScsiCmd.DataDirection = US_BULK_FLAG_IN;
	}
	else
	{
		gScsiCmd.Cmd = WRITE_10;
		gScsiCmd.DataDirection = US_BULK_FLAG_OUT;
	}
	gScsiCmd.CmdLen = 10;
	gScsiCmd.Lun = lu->LuNum;
	gScsiCmd.Buf = buf;
	gScsiCmd.BufLen = size * lu->BlockSize;
	gScsiCmd.Lba = lba;
	gScsiCmd.BlockSize = lu->BlockSize;

	return StorBulkTransport();
}


/*
 * storage start device
 */
extern BOOL
StorStartDevice(VOID)
{
	BYTE		to;
	BYTE		storBuf[40];
	BYTE		i;

	if (gStorStatus == 0)			//init storage device
	{
		StorGetMaxLUN();

		for (i = 0; i < gStor.MaxLUN + 1; ++i)	
		{
			if (StorInquiry(i, storBuf, 36) == 0)
			{
				if (storBuf[0] == 0)
				{
					gStor.CurrLu.LuNum = i;
					break;
				}
			}
		}
		if (i == gStor.MaxLUN + 1)
			return FALSE;

		gStorStatus = 1;
	}
	
	if (gStorStatus == 1)			//check storage unit ready
	{
		to = 3;
		while(to)
		{		
			if (StorTestUnitReady(gStor.CurrLu.LuNum) == 0)
				break;
			StorRequestSense(gStor.CurrLu.LuNum, storBuf, 18);
			to--;
		}
		if (to == 0)
			return FALSE;
	
		gStorStatus = 2;
	}

	if (gStorStatus == 2)		
	{
		if (StorReadCapacity(gStor.CurrLu.LuNum, storBuf, 8) != 0)
			return FALSE;

		gStor.CurrLu.BlockSize = *(WORD *)&storBuf[6];
		gStor.CurrLu.LastLBA = *(DWORD *)&storBuf[0];
		gStorStatus = 1;
		return TRUE;
	}

	return FALSE;
}


/*
 * storage start device
 */
extern DWORD
StorGetLastLBA(VOID)
{
	return gStor.CurrLu.LastLBA;
}


/*
 * storage start device
 */
extern WORD
StorGetBlockSize(VOID)
{
	return gStor.CurrLu.BlockSize;
}
