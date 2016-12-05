/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	ohci.c
*
* Programmer:    	Grey
*
* Created: 	 		04/xx/2008
*
* Description: 		usb ohci host controller driver file
*              
*        
* Change History (most recent first):	2008.04.10
*****************************************************************/

#include <string.h>
#include "../public/type.h"
#include "../public/func.h"
#include "../usb/hcd.h"
#include "ohci_config.h"
#include "ohci.h"


/*
 * define ED TD memory source
 */
BYTE	XDATA	gEdTdMap[MAX_ED_TD_SIZE / 8];
volatile HC_OPERATIONAL_REGISTER		XDATA	*gOhciReg = (volatile HC_OPERATIONAL_REGISTER XDATA	*)OHCI_REG_BASE;
volatile OHCI_HCCA_BLOCK				XDATA	*gOhciHcca = (volatile OHCI_HCCA_BLOCK	XDATA	*)OHCI_HCCA_BASE;



/*
 * write ohci register or memory
 */
extern VOID 
OhciWrite(
	DWORD			*mem,
	DWORD			val
	)
{
	ENTER_CRITICAL();
	*mem = LeToBe32(val);
	EXIT_CRITICAL();
}


/*
 * read ohci register or memory
 */
extern DWORD
OhciRead(
	DWORD			*mem
	)
{
	DWORD		retVal;

	ENTER_CRITICAL();
	retVal = LeToBe32(*mem);
	EXIT_CRITICAL();

	return retVal;
}


/*
 * get frame number
 */
extern WORD
OhciGetFrameNumber(VOID)
{
	return (WORD)(OhciRead(&gOhciReg->HcFmNumber) & 0x0000FFFF);
}


/*
 * allocate ED or TD memory space
 */
extern VOID*
OhciAllocEDTD(VOID)
{
	BYTE	DATA	i;
	BYTE	DATA	temp;

	for (i = 0; i < MAX_ED_TD_SIZE; ++i)
	{
		temp = 0x01 << (i & 7);
		if ((gEdTdMap[i >> 3] & temp) == 0)
		{
			gEdTdMap[i >> 3] |= temp;
			APPLY_MEM();
			memset((VOID XDATA *)(OHCI_ED_TD_BASE + (i << 4)), 0, 16);
			RELEASE_MEM();
			return (VOID XDATA *)(OHCI_ED_TD_BASE + (i << 4));
		}
	}/* end for */

	return NULL;
}


/*
 * free ED or TD memory space
 */
extern VOID
OhciFreeEDTD(
	VOID	*ed_td
	)
{
	BYTE	DATA	i;

	for (i = 0; i < MAX_ED_TD_SIZE; ++i)
	{
		if ((OHCI_ED_TD_BASE + (i << 4)) == (WORD)ed_td)
		{
			gEdTdMap[i >> 3] &= ~(0x01 << (i & 7));
			return;
		}
	}/* end for */
}


/*
 * ohci open pipe
 */
extern VOID*			/* return: pipe pointer */
OhciOpenPipe(
	DWORD			pipeInfo			/* input: pipe inforamtion */
	)
{
	volatile OHCI_ENDPOINT_DESCRIPTOR	XDATA	*HcED;
	volatile OHCI_TRANSFER_DESCRIPTOR	XDATA	*HcTD;
	DWORD	XDATA		control;
	volatile DWORD		XDATA	*head;

	/* allocate one ED and TD space */
	HcED = (volatile OHCI_ENDPOINT_DESCRIPTOR XDATA *)OhciAllocEDTD();
	if (HcED == NULL)
		return NULL;

	HcTD = (volatile OHCI_TRANSFER_DESCRIPTOR XDATA *)OhciAllocEDTD();
	if (HcTD == NULL)
	{
		OhciFreeEDTD(HcED);
		return NULL;
	}

	control = PipeMaxPacketSize(pipeInfo);
	control <<= 5;

	if (PipeControl(pipeInfo))
	{
		head = &gOhciReg->HcControlHeadED;
	}
	else if (PipeBulk(pipeInfo))
	{
		((BYTE *)&control)[3] |= PipeIn(pipeInfo) ? DirIN : DirOUT;			//control |= (gUrb.Direction & 0x03);
		head = &gOhciReg->HcBulkHeadED;
	}
	control <<= 4;

	((BYTE *)&control)[3] |= PipeEndpoint(pipeInfo);		//control |= (gUrb.EndpointAddress & 0x0F);
	control <<= 7;

	((BYTE *)&control)[3] |= PipeDevice(pipeInfo);			//control |= (gUrb.FuncAddress & 0x7F);

	/* fill ED information */
	APPLY_MEM();

	OhciWrite(&HcED->Control, control);
	OhciWrite(&HcED->HeadP, (WORD)HcTD);
	OhciWrite(&HcED->TailP, (WORD)HcTD);

	control = OhciRead(head);			//link ED to ED list head
	OhciWrite(&HcED->NextED, control);	
	OhciWrite(head, (WORD)HcED);

	RELEASE_MEM();
	
	return (VOID *)HcED;
}


/*
 * ohci close pipe
 */
extern VOID
OhciClosePipe(
	VOID			*pipe			/* input: pipe pointer */
	)
{
	volatile OHCI_ENDPOINT_DESCRIPTOR	XDATA	*HcED, *tempED;
	volatile OHCI_TRANSFER_DESCRIPTOR	XDATA	*HcTD, *tempTD;
	DWORD	XDATA		temp, nextED;
	volatile DWORD	XDATA	*listPointer;

	HcED = (volatile OHCI_ENDPOINT_DESCRIPTOR XDATA *)pipe;

	APPLY_MEM();
	temp = OhciRead(&HcED->HeadP);		/* halt ED */
	((BYTE *)&temp)[3] |= 0x01;
	OhciWrite(&HcED->HeadP, temp);

	temp = OhciRead(&HcED->Control);
	RELEASE_MEM();
	if ((((BYTE *)&temp)[2] & 0x18) == 0)
		listPointer = &gOhciReg->HcControlHeadED;
	else
		listPointer = &gOhciReg->HcBulkHeadED;
		
	while(1)
	{
		APPLY_MEM();
		nextED = OhciRead(listPointer);
		RELEASE_MEM();
		if (nextED == 0)		/* list empty */
			break;

		if (nextED == (WORD)HcED)
		{
			/* find the ED, remove it */

			/* free TDs of this ED */
			APPLY_MEM();
			temp = OhciRead(&HcED->HeadP);
			RELEASE_MEM();

			HcTD = (OHCI_TRANSFER_DESCRIPTOR XDATA *)((WORD)temp & 0xFFF0);
			while(HcTD != NULL)
			{
				APPLY_MEM();
				tempTD = (OHCI_TRANSFER_DESCRIPTOR XDATA *)(WORD)OhciRead(&HcTD->NextTD);
				RELEASE_MEM();
		
				OhciFreeEDTD(HcTD);		//free TD
				HcTD = tempTD;
			}/* end while */

			APPLY_MEM();
			OhciWrite(listPointer, OhciRead(&HcED->NextED));		//remove ED from list
			RELEASE_MEM();
			OhciFreeEDTD(HcED);

			break;
		}/* end if */
		else
		{
			tempED = (volatile OHCI_ENDPOINT_DESCRIPTOR	XDATA *)(WORD)nextED;
			listPointer = &tempED->NextED;
		}
	}/* end while */
}


/*
 * ohci process general queue request
 */
extern BOOL				/* return: success 1, else 0 */
OhciQueueGeneralRequest(
	OHCI_ENDPOINT_DESCRIPTOR	*HcED			/* input: ED pointer of these TD */
	)
{
	volatile OHCI_TRANSFER_DESCRIPTOR	XDATA	*HcTD, *tempTD, *lastTD;
	DWORD	XDATA		control;
	WORD	XDATA		remainLength, count, currentBufferPointer, maxPacketSize;
	BYTE	XDATA		type, direction;
	
	APPLY_MEM();
	control = OhciRead(&HcED->Control);
	HcTD = (volatile OHCI_TRANSFER_DESCRIPTOR XDATA *)((WORD)OhciRead(&HcED->HeadP) & 0xFFF0);
	RELEASE_MEM();

	maxPacketSize = ((WORD *)&control)[0];		//set ED max packet size
	direction = (((BYTE *)&control)[2] & 0x18);
	if (direction == 0)
	{
		type = PIPE_CONTROL;
		direction = (gUrb.Direction == 0x80) ? DirIN : DirOUT;		//set direction from urb
	}
	else
	{
		type = PIPE_BULK;
		direction >>= 3;		//set direction form ED
	}

	remainLength = gUrb.BufferLength;
	currentBufferPointer = (WORD)gUrb.Buffer;
	if (gUrb.Buffer != NULL && currentBufferPointer < SYS_SHARE_BUF_BASE)
		currentBufferPointer = OHCI_BUF_BASE;

	/* control transaction setup phase */
	if (type == PIPE_CONTROL)		//control transaction
	{
		/* allocate one empty TD */
		tempTD = (volatile OHCI_TRANSFER_DESCRIPTOR XDATA *)OhciAllocEDTD();
		if (tempTD == NULL)
		{
			gUrb.Status = CC_AllocEDTD;
			return FALSE;
		}

		APPLY_MEM();
		/* fill TD information */
		OhciWrite(&HcTD->Control, 0xF2E40000);
		OhciWrite(&HcTD->CBP, OHCI_SETUP_BASE);
		OhciWrite(&HcTD->BE, OHCI_SETUP_BASE + 7);
		OhciWrite(&HcTD->NextTD, (WORD)tempTD);
		RELEASE_MEM();

		lastTD = HcTD;
		HcTD = tempTD;
	}/* end if (gUrb.Type == 2) */

	/* control or bulk transaction data phase */
	while (remainLength)
	{
		/* allocate one empty TD */
		tempTD = (volatile OHCI_TRANSFER_DESCRIPTOR XDATA *)OhciAllocEDTD();
		if (tempTD == NULL)
		{
			gUrb.Status = CC_AllocEDTD;
			return FALSE;
		}

		/* calculate transfer data length count */
		count = 0x2000 - (currentBufferPointer & 0x0FFF);
		if (count < remainLength)
			count -= count % maxPacketSize;
		else
			count = remainLength; 

		/* config TD control information */
		control = ((DWORD)direction) << TD_CONTROL_DirectionPID;
		if (type == PIPE_CONTROL) 
			((BYTE *)&control)[0] |= 0x03;	//control |= 0x03000000;		//set data toggle DATA1 for control TD
		else
			((BYTE *)&control)[0] &= ~0x03;    //control &= ~0x03000000;		//data toggle from ED togglecarry bit for other transfer mode

		remainLength -= count;
		if (remainLength != 0) 
			((BYTE *)&control)[1] |= 0xE0;	//control |= 0xF0E00000;
		else
			((BYTE *)&control)[1] |= 0xE4;	//control |= 0xF0E40000;
		((BYTE *)&control)[0] |= 0xF0;

		/* fill TD information */
		APPLY_MEM();
		OhciWrite(&HcTD->Control, control);
		OhciWrite(&HcTD->CBP, currentBufferPointer);
		OhciWrite(&HcTD->BE, currentBufferPointer + count - 1);
		OhciWrite(&HcTD->NextTD, (WORD)tempTD);
		RELEASE_MEM();

		currentBufferPointer += count;

		lastTD = HcTD;
		HcTD = tempTD;
	}/* end while */

	/* control transaction status phase */
	if (type == PIPE_CONTROL)		//control transaction
	{
		/* allocate one empty TD */
		tempTD = (volatile OHCI_TRANSFER_DESCRIPTOR XDATA *)OhciAllocEDTD();
		if (tempTD == NULL)
		{
			gUrb.Status = CC_AllocEDTD;
			return FALSE;
		}

		/* set TD control information */
		if (direction == DirIN) 
			control = ((DWORD)DirOUT) << TD_CONTROL_DirectionPID;
		else
			control = ((DWORD)DirIN) << TD_CONTROL_DirectionPID;

		((BYTE *)&control)[0] |= 0xF3;	//control |= 0xF3E00000;
		((BYTE *)&control)[1] |= 0xE0;
	
		/* fill TD information */
		APPLY_MEM();
		OhciWrite(&HcTD->Control, control);
		OhciWrite(&HcTD->CBP, 0);
		OhciWrite(&HcTD->BE, 0);
		OhciWrite(&HcTD->NextTD, (WORD)tempTD);
		RELEASE_MEM();

		lastTD = HcTD;
		HcTD = tempTD;
	}/* end if (gUrb.Type == 2) */

	/* process last TD, set interrupt delay */
	APPLY_MEM();
	control = OhciRead(&lastTD->Control);
	((BYTE *)&control)[1] &= ~0xE0;	//control &= ~OHCI_TD_CONTROL_DelayInterrupt;
	OhciWrite(&lastTD->Control, control);

	/* process ED tailp */
	OhciWrite(&HcED->TailP, (WORD)HcTD);
	
	/* clear ED halt flag */
	control = OhciRead(&HcED->HeadP);
	((BYTE *)&control)[3] &= ~0x01;
	OhciWrite(&HcED->HeadP, control);
	RELEASE_MEM();

	/* host controller start list process */
	if (type == PIPE_CONTROL)
	{
		OhciWrite(&gOhciReg->HcCommandStatus, HcCommandStatusCLF);
	}
	else if (type == PIPE_BULK)
	{
		OhciWrite(&gOhciReg->HcCommandStatus, HcCommandStatusBLF);
	}

	return TRUE;
}


/*
 * ohci process done queue
 */
extern VOID
OhciProcessDoneQueue(
	DWORD			hccaDoneHead			/* input: done queue head address */
	)
{
	volatile OHCI_TRANSFER_DESCRIPTOR	XDATA	*HcTD, *tempTD;
	BYTE	XDATA	status;
	
	HcTD = (volatile OHCI_TRANSFER_DESCRIPTOR XDATA *)hccaDoneHead;

	while(HcTD != NULL)
	{	
		/* get TD condition code */
		APPLY_MEM();
		tempTD = (volatile OHCI_TRANSFER_DESCRIPTOR XDATA *)(WORD)OhciRead(&HcTD->NextTD);	
		status = ((OhciRead(&HcTD->Control) & OHCI_TD_CONTROL_ConditionCode) >> TD_CONTROL_ConditionCode);
		RELEASE_MEM();
		if (status != 0)
			gUrb.Status = status;

		/* cut TD from done queue */
		OhciFreeEDTD(HcTD);		//free this TD
		HcTD = tempTD;
	}/* end while */
}


/*
 * ohci interrupt service
 */
extern BOOL
OhciInterruptService(VOID)
{
	DWORD	XDATA	hccaDoneHead;

	APPLY_MEM();
	hccaDoneHead = OhciRead(&gOhciHcca->HccaDoneHead);
	RELEASE_MEM();

	if (hccaDoneHead == 0)
		return FALSE;

	/* Write back Done Head interrupt */
	OhciProcessDoneQueue(hccaDoneHead);
	APPLY_MEM();
	OhciWrite(&gOhciHcca->HccaDoneHead, 0);
	RELEASE_MEM();
	OhciWrite(&gOhciReg->HcInterruptStatus, HcInterruptWDH);		//clear interrupt flag

	return TRUE;
}


/*
 * open ohci, init ohci host controller to send SOF
 */
extern BOOL
OhciOpen(VOID)
{
	DWORD	XDATA	temp;
	WORD	XDATA	to;

	/* mapping register */
	gOhciReg = (volatile HC_OPERATIONAL_REGISTER XDATA *)OHCI_REG_BASE;
	if ((OhciRead(&gOhciReg->HcRevision) & 0xFF) != HcRevision10)
		return FALSE;

	/* init HCCA */
	gOhciHcca = (volatile OHCI_HCCA_BLOCK XDATA *)OHCI_HCCA_BASE;
	APPLY_MEM();
	memset(gOhciHcca, 0, sizeof(OHCI_HCCA_BLOCK));
	RELEASE_MEM();
	
	/* reset ohci */
	temp = OhciRead(&gOhciReg->HcFmInterval);		//save frame interval value

	OhciWrite(&gOhciReg->HcInterruptDisable, HcInterruptMIE);	//disable all interrupt
	OhciWrite(&gOhciReg->HcControl, HcControlHCFS_RESET);		//reset host
	OhciWrite(&gOhciReg->HcCommandStatus, HcCommandStatusHCR);

	to = HC_RESET_TIME_OUT;
	while(to)
	{
		if ((OhciRead(&gOhciReg->HcCommandStatus) & HcCommandStatusHCR) == 0)		//wait reset done
			break;
		to--;	
	}
	if (to == 0)
		return FALSE;

	OhciWrite(&gOhciReg->HcFmInterval, temp);	//write back frame interval value

	/* init ohci operation register */
	OhciWrite(&gOhciReg->HcControlHeadED, 0);
	OhciWrite(&gOhciReg->HcBulkHeadED, 0);
	OhciWrite(&gOhciReg->HcHCCA, OHCI_HCCA_BASE);

	/* enable host controller, set it into operational status */
	OhciWrite(&gOhciReg->HcControl, (HcControlCLE | HcControlBLE | HcControlHCFS_OPER));	//enable control and bulk list

	/* set periodic transfer ratio per frame */
	temp = OhciRead(&gOhciReg->HcFmInterval);
	temp = temp * 9 / 10; 			
	OhciWrite(&gOhciReg->HcPeriodicStart, temp);		//periodic transfer 10% per frame

	/* update frame interval and threshold */
	OhciWrite(&gOhciReg->HcFmInterval, 0xB6173FED);	
	OhciWrite(&gOhciReg->HcLSThreshold, 0x628);

	/* init root hub */
	OhciWrite(&gOhciReg->HcRhStatus, HcRhStatusLPSC);		//turn on power to all ports
	OhciWrite(&gOhciReg->HcRhPortStatus[0], HcRhPortStatusPPS);		//port power on

	return TRUE;
}


/*
 * ohci send urb
 */
extern VOID				/* return: success 1, else 0 */
OhciSendUrb(VOID)
{
	volatile OHCI_ENDPOINT_DESCRIPTOR	XDATA	*HcED;
	DWORD	XDATA	to;

	/* load buffer data */
	if (gUrb.SetupPacket != NULL)
	{
		APPLY_MEM();
		memcpy((BYTE XDATA *)OHCI_SETUP_BASE, gUrb.SetupPacket, 8);
		RELEASE_MEM();
	}
	if (gUrb.Buffer != NULL && gUrb.Direction == 0 && (WORD)gUrb.Buffer < SYS_SHARE_BUF_BASE)
	{
		APPLY_MEM();
		memcpy((BYTE XDATA *)OHCI_BUF_BASE, gUrb.Buffer, ((gUrb.BufferLength + 3) & ~0x0003));		//4 bytes align
		RELEASE_MEM();
	}

	/* allocate one ED and TD space */
	HcED = (volatile OHCI_ENDPOINT_DESCRIPTOR XDATA *)gUrb.Pipe;

	/* insert TD to the ED, and send request */
	if (!OhciQueueGeneralRequest(HcED))
		return;

	/* process done queue, free TDs */
	to = HC_SEND_URB_TIME_OUT;
	while(to)
	{
		if (OhciInterruptService())
			break;
		to--;
	}
	if (to == 0)
	{
		gUrb.Status = CC_TimeOut;
		return;
	}

	/* free ED from list head */
	if (gUrb.Status != 0)
		return;

	/* unload buffer data */
	if (gUrb.Buffer != NULL && gUrb.Direction == 0x80 && (WORD)gUrb.Buffer < SYS_SHARE_BUF_BASE)
	{
		APPLY_MEM();
		memcpy(gUrb.Buffer, (BYTE XDATA *)OHCI_BUF_BASE, gUrb.BufferLength);
		RELEASE_MEM();
	}
}


/*
 * get ohci root hub port status
 */
extern VOID
OhciRootHubPortStatus(
	BOOL		*isConnect,			/* output: have device connection set 1, else set 0 */
	BOOL		*isChange			/* 			device connection change set 1, else set 0 */
	)
{
	DWORD	XDATA	portStatus;

	portStatus = OhciRead(&gOhciReg->HcRhPortStatus[0]);
	
	*isConnect = ((portStatus & HcRhPortStatusCCS) != 0) ? TRUE : FALSE;
	*isChange = ((portStatus & HcRhPortStatusCSC) != 0) ? TRUE : FALSE;

	if (*isConnect && *isChange)		//new device insert
	{
		memset(gEdTdMap, 0, sizeof(gEdTdMap));		//clear ED and TD map
		memset(&gUrb, 0, sizeof(URB));			//clear URB struct
	}
}


/*
 * ohci root hub reset
 */
extern VOID
OhciRootHubPortReset(VOID)
{
	DWORD	XDATA	portStatus;
	
	OhciWrite(&gOhciReg->HcRhPortStatus[0], HcRhPortStatusPRS);		//set port reset
//	OhciDelayMs(OHCI_ROOT_HUB_RESET_MS);
	while(1)
	{
		portStatus = OhciRead(&gOhciReg->HcRhPortStatus[0]);
		if ((portStatus & HcRhPortStatusPRSC) != 0)
		{
			OhciWrite(&gOhciReg->HcRhPortStatus[0], HcRhPortStatusPRSC);	//clear port reset signal
			break;
		}
		if ((portStatus & HcRhPortStatusCSC) != 0)
		{
			OhciWrite(&gOhciReg->HcRhPortStatus[0], HcRhPortStatusCSC);		//clear port change flag
			break;
		}
	}/* end while */
}


/*
 * ohci root hub clear port feature
 */
extern VOID
OhciRootHubClearPortFeature(VOID)
{
	OhciWrite(&gOhciReg->HcRhPortStatus[0], HcRhPortStatusCSC);		//clear port change flag
}



