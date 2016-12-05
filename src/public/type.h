/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	type.h
*
* Programmer:    	Grey
*
* Created: 	 		06/xx/2008
*
* Description: 		usb host software data type define
*              
*        
* Change History (most recent first):	2008.06.03
*****************************************************************/

#ifndef __TYPE_H__
#define	__TYPE_H__

/*
 * data type
 */
#define	XDATA	xdata
#define	DATA	data
#define	CODE	code

#define FALSE			0
#define TRUE			1

typedef void			VOID, *PVOID;
typedef char			BOOL;
typedef char			BOOLEAN;
typedef char			*PCHAR;
typedef unsigned char	BYTE, UCHAR, *PUCHAR;
typedef unsigned short	USHORT, *PUSHORT;
typedef unsigned short	WORD, *PWORD;
typedef unsigned long	DWORD, ULONG, *PULONG;
typedef volatile UCHAR	*PVUCHAR;
typedef volatile USHORT	*PVUSHORT;
typedef volatile ULONG 	*PVULONG;


#endif
