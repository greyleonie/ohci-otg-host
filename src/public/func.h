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

#ifndef __FUNC_H__
#define	__FUNC_H__

/*
 * 16bit data little endian to big endian 
 */
extern WORD
LeToBe16(
	WORD		val
	);


/*
 * 32bit data little endian to big endian 
 */
extern DWORD
LeToBe32(
	DWORD		val
	);


/*
 * get data exp 2 value
 */
extern BYTE
GetExp2(
	WORD		val
	);



#endif
