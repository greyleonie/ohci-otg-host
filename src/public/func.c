/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	func.c
*
* Programmer:    	Grey
*
* Created: 	 		06/xx/2008
*
* Description: 		public functions file
*              
*        
* Change History (most recent first):	2008.06.03
*****************************************************************/
#include "type.h"


/*
 * 16bit data little endian to big endian 
 */
extern WORD
LeToBe16(
	WORD		val
	)
{
	WORD	XDATA	ret;

	((BYTE XDATA *)&ret)[0] = ((BYTE XDATA *)&val)[1];
	((BYTE XDATA *)&ret)[1] = ((BYTE XDATA *)&val)[0];

	return ret;
}


/*
 * 32bit data little endian to big endian 
 */
extern DWORD
LeToBe32(
	DWORD		val
	)
{
	DWORD	XDATA	ret;

	((BYTE XDATA *)&ret)[0] = ((BYTE XDATA *)&val)[3];
	((BYTE XDATA *)&ret)[1] = ((BYTE XDATA *)&val)[2];
	((BYTE XDATA *)&ret)[2] = ((BYTE XDATA *)&val)[1];
	((BYTE XDATA *)&ret)[3] = ((BYTE XDATA *)&val)[0];

	return ret;
}


/*
 * get data exp 2 value
 */
extern BYTE
GetExp2(
	WORD		val
	)
{
	BYTE	XDATA	exp = 0;

	val >>= 1;
	while(val)
	{
		val >>= 1;
		exp++;
	}
	
	return exp;	
}
