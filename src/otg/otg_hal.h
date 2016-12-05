/****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2007, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	otg_hal.h
*
* Programmer:    	Grey
*
* Created: 	 		09/xx/2008
*
* Description: 		OTG hareware abstract layer head file
*					
*              
*****************************************************************/

#ifndef	__OTG_HAL_H__
#define	__OTG_HAL_H__

#if CODY_FPGA_TEST_EN

/* Define OTG access register */
#define	otg_op_status				(*(volatile BYTE XDATA *)(SYS_OTG_ACCESS_BASS + 0))
#define	otg_rd_read_data			(*(volatile BYTE XDATA *)(SYS_OTG_ACCESS_BASS + 1))
#define	utmi_src_sel_en				(*(volatile BYTE XDATA *)(SYS_OTG_ACCESS_BASS + 2))
#define	utmi_src_sel				(*(volatile BYTE XDATA *)(SYS_OTG_ACCESS_BASS + 3))

#define	OTG_STATUS_READ_OK			(1 << 0)
/**************************************************
*	Read only, when set read OTG register data ok.
**************************************************/

#define	OTG_READ_REQ				(1 << 1)
/**************************************************
*	Read only, when set re-read OTG register required, 
*	after read OTG_RD_READ_DATA reigster, this bit cleared auto.
**************************************************/

#define	OTG_WRITE_BUSY				(1 << 2)
/**************************************************
*	Read only, when write OTG register busy.
*	Note: Do not rd/wr OTG register until this bit is cleared.
**************************************************/

#define	OTG_UTMI_VBUSVALID			(1 << 0)
#define	OTG_UTMI_AVALID				(1 << 1)
#define	OTG_UTMI_SESSEND			(1 << 2)
#define	OTG_UTMI_IDDIG				(1 << 3)
#define	OTG_UTMI_HOST_DISCONN		(1 << 4)

#else

/* Define OTG access register */
#define	otg_op_status				(*(volatile BYTE XDATA *)(SYS_OTG_ACCESS_BASS + 0))
#define	otg_rd_read_data			(*(volatile BYTE XDATA *)(SYS_OTG_ACCESS_BASS + 1))

#define	OTG_OP_STATUS_READY			(1 << 0)		//Read only, when set OTG register access ready.


#define	utmi_src_sel1				(*(volatile BYTE XDATA *)(SYS_OTG_ACCESS_BASS + 2))
#define	utmi_ctrl1					(*(volatile BYTE XDATA *)(SYS_OTG_ACCESS_BASS + 4))

#define	OTG_UTMI_VBUSVALID			(1 << 0)
#define	OTG_UTMI_SESSVALID			(1 << 1)
#define	OTG_UTMI_SESSEND			(1 << 2)
#define	OTG_UTMI_IDDIG				(1 << 3)
#define	OTG_UTMI_HOST_DISCONN		(1 << 4)
#define	OTG_UTMI_SUSPENDM			(1 << 5)
#define	OTG_UTMI_REG_EN				(1 << 6)

#define	utmi_src_sel2				(*(volatile BYTE XDATA *)(SYS_OTG_ACCESS_BASS + 3))
#define	utmi_ctrl2					(*(volatile BYTE XDATA *)(SYS_OTG_ACCESS_BASS + 5))

#define	OTG_UTMI_DISCHRGVBUS		(1 << 0)
#define	OTG_UTMI_CHRGVBUS			(1 << 1)
#define	OTG_UTMI_ID_PULLUP			(1 << 2)
#define	OTG_UTMI_DP_PULLDOWN		(1 << 3)
#define	OTG_UTMI_DM_PULLDOWN		(1 << 4)
//#define	OTG_UTMI_ID_PIN				(1 << 5)
#define	OTG_UTMI_PHY_RST			(3 << 5)
#define	OTG_UTMI_TEST_BIST			(1 << 7)


#define	utmi_ctrl3					(*(volatile BYTE XDATA *)(SYS_OTG_ACCESS_BASS + 6))

#define	OTG_UTMI_DATA_BUS_16_8		(1 << 0)
#define	OTG_UTMI_TXBITSTUFF_EN		(1 << 1)
#define	OTG_UTMI_FSLS_SERIALMODE	(1 << 2)
#define	OTG_UTMI_TEST				(1 << 3)
#define	OTG_UTMI_TX_SE0				(1 << 4)
#define	OTG_UTMI_TX_DAT				(1 << 5)
#define	OTG_UTMI_TX_ENABLE_N		(1 << 6)
#define	OTG_UTMI_USB_PLAY_MODE		(1 << 7)

#endif//#if CODY_FPGA_TEST_EN


extern BYTE
ReadOTGReg8(
	BYTE		*reg
	);

extern VOID
WriteOTGReg8(
	BYTE		*reg,
	BYTE		val
	);

extern WORD
ReadOTGReg16(
	WORD		*reg
	);

extern VOID
WriteOTGReg16(
	WORD		*reg,
	WORD		val
	);

extern DWORD
ReadOTGReg32(
	DWORD		*reg
	);

extern VOID
WriteOTGReg32(
	DWORD		*reg,
	DWORD		val
	);

extern VOID
SetBitOTGReg8(
	BYTE 		*reg,
	BYTE		mask
	);

extern VOID
ClrBitOTGReg8(
	BYTE		*reg,
	BYTE		mask
	);

extern VOID
SetBitOTGReg16(
	WORD 		*reg,
	WORD		mask
	);

extern VOID
ClrBitOTGReg16(
	WORD		*reg,
	WORD		mask
	);

extern VOID
SetBitOTGReg32(
	DWORD 		*reg,
	DWORD		mask
	);

extern VOID
ClrBitOTGReg32(
	DWORD		*reg,
	DWORD		mask
	);




extern VOID
LoadFIFOData(
	BYTE			*fifo,
	BYTE			*dat,
	WORD			length
	);

extern VOID
UnloadFIFOData(
	BYTE			*fifo,
	BYTE			*dat,
	WORD			length
	);
	


#endif//#ifndef	__OTG_HAL_H__

