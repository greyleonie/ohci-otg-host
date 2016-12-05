/****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2007, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	otg_hal.c
*
* Programmer:    	Grey
*
* Created: 	 		09/xx/2008
*
* Description: 		OTG hareware abstract layer
*					
*              
*****************************************************************/

#include <string.h>
#include "../public/type.h"
#include "../public/func.h"
#include "otg_config.h"
#include "otg_hal.h"

/*---------------------------------OTG register access--------------------------*/
extern BYTE
ReadOTGReg8(
	BYTE		*reg
	)
{
	BYTE	DATA	retVal;
	
	ENTER_CRITICAL();
	/* read register */
	retVal = *reg;				
#if CODY_FPGA_TEST_EN
	/* check OTG_STATUS_READ_OK signal, wait OTG_STATUS_READ_OK == 1 */
	while((otg_op_status & OTG_STATUS_READ_OK) == 0);	

	/* check OTG_READ_REQ signal */
	if ((otg_op_status & OTG_READ_REQ) != 0)		/* OTG_READ_REQ == 1 */
		retVal = otg_rd_read_data;	
#else
	/* check OTG_STATUS_READ_OK signal, wait OTG_OP_STATUS_READY == 1 */
	while((otg_op_status & OTG_OP_STATUS_READY) == 0);	

	/* check OTG_READ_REQ signal */
//	if ((otg_op_status & OTG_READ_REQ) != 0)		/* OTG_READ_REQ == 1 */
		retVal = otg_rd_read_data;	
#endif
	EXIT_CRITICAL();

	return retVal;
}


extern VOID
WriteOTGReg8(
	BYTE		*reg,
	BYTE		val
	)
{	
	ENTER_CRITICAL();
#if CODY_FPGA_TEST_EN
	/* write register */
	*reg = val;						
		
	/* check OTG_WRITE_BUSY signal, wait OTG_WRITE_BUSY == 0 */
	while((otg_op_status & OTG_WRITE_BUSY) != 0);
#else
	/* check OTG_WRITE_BUSY signal, wait OTG_OP_STATUS_READY == 1 */
	while((otg_op_status & OTG_OP_STATUS_READY) == 0);	

	/* write register */
	*reg = val;						
#endif		
	EXIT_CRITICAL();
}



extern WORD
ReadOTGReg16(
	WORD		*reg
	)
{
	BYTE	XDATA	*p = (BYTE XDATA *)reg;
	WORD	DATA	retVal;
	
	((BYTE *)&retVal)[0] = ReadOTGReg8(&p[1]);
	((BYTE *)&retVal)[1] = ReadOTGReg8(&p[0]);
	
	return retVal;
}

extern VOID
WriteOTGReg16(
	WORD		*reg,
	WORD		val
	)
{
	BYTE	XDATA 	*p = (BYTE XDATA *)reg;
	
	WriteOTGReg8(&p[1], ((BYTE *)&val)[0]);
	WriteOTGReg8(&p[0], ((BYTE *)&val)[1]);
}

extern DWORD
ReadOTGReg32(
	DWORD		*reg
	)
{
	BYTE	XDATA	*p = (BYTE XDATA *)reg;
	DWORD	DATA	retVal;
	
	((BYTE *)&retVal)[0] = ReadOTGReg8(&p[3]);
	((BYTE *)&retVal)[1] = ReadOTGReg8(&p[2]);
	((BYTE *)&retVal)[2] = ReadOTGReg8(&p[1]);
	((BYTE *)&retVal)[3] = ReadOTGReg8(&p[0]);
	
	return retVal;
}

extern VOID
WriteOTGReg32(
	DWORD		*reg,
	DWORD		val
	)
{
	BYTE	XDATA 	*p = (BYTE XDATA *)reg;

	WriteOTGReg8(&p[3], ((BYTE *)&val)[0]);
	WriteOTGReg8(&p[2], ((BYTE *)&val)[1]);
	WriteOTGReg8(&p[1], ((BYTE *)&val)[2]);
	WriteOTGReg8(&p[0], ((BYTE *)&val)[3]);
}


extern VOID
SetBitOTGReg8(
	BYTE 		*reg,
	BYTE		mask
	)
{
	BYTE	DATA	temp;
	
	temp = ReadOTGReg8(reg);	
	temp |= mask;	
	WriteOTGReg8(reg, temp);		
}

extern VOID
SetBitOTGReg16(
	WORD 		*reg,
	WORD		mask
	)
{
	WORD	DATA	temp;
	
	temp = ReadOTGReg16(reg);	
	temp |= mask;	
	WriteOTGReg16(reg, temp);	
}

extern VOID
SetBitOTGReg32(
	DWORD 		*reg,
	DWORD		mask
	)
{
	DWORD	DATA	temp;
	
	temp = ReadOTGReg32(reg);	
	temp |= mask;	
	WriteOTGReg32(reg, temp);
}


extern VOID
ClrBitOTGReg8(
	BYTE		*reg,
	BYTE		mask
	)
{
	BYTE	DATA	temp;
	
	temp = ReadOTGReg8(reg);	
	temp &= ~mask;	
	WriteOTGReg8(reg, temp);	
}

extern VOID
ClrBitOTGReg16(
	WORD		*reg,
	WORD		mask
	)
{
	WORD	DATA	temp;
	
	temp = ReadOTGReg16(reg);	
	temp &= ~mask;	
	WriteOTGReg16(reg, temp);
}

extern VOID
ClrBitOTGReg32(
	DWORD		*reg,
	DWORD		mask
	)
{
	DWORD	DATA	temp;
	
	temp = ReadOTGReg32(reg);	
	temp &= ~mask;	
	WriteOTGReg32(reg, temp);
}


extern VOID
ToggleBitOTGReg8(
	BYTE		*reg,
	BYTE		mask
	)
{
	BYTE	DATA	temp;
	
	temp = ReadOTGReg8(reg);	
	temp ^= mask;	
	WriteOTGReg8(reg, temp);
}

extern VOID
ToggleBitOTGReg16(
	WORD		*reg,
	WORD		mask
	)
{
	WORD	DATA	temp;
	
	temp = ReadOTGReg16(reg);	
	temp ^= mask;	
	WriteOTGReg16(reg, temp);
}
/*--------------------------------------------------------------------------*/

/*------------------------------Data FIFO access----------------------------*/
extern VOID
LoadFIFOData(
	BYTE			*fifo,
	BYTE			*dat,
	WORD			length
	)
{
	/* load data from data buffer into FIFO */
	while(length)
	{
		WriteOTGReg8(fifo, *dat);		/* write fifo data */
		dat++;
		length--;
	}/* end while */
}

extern VOID
UnloadFIFOData(
	BYTE			*fifo,
	BYTE			*dat,
	WORD			length
	)
{
	/* unload data from FIFO into data buffer */
	while(length)
	{
		*dat = ReadOTGReg8(fifo);					/* read fifo data from data address 0 */
		dat++;
		length--;
	}/* end while */
}
/*----------------------------------------------------------------------------*/


