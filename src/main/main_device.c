

#include "../public/type.h"

extern BOOL
OtgDevOpen(VOID);

extern VOID
OtgDevRecvPacket(VOID);



#define	MEM_ACCESS		*(volatile unsigned char xdata *)0xAE4D

#define MEM_REQ		0x02
#define MEM_ACK		0x01

extern void 
ApplyREQ(void)
{
	do
	{
		MEM_ACCESS &= (~MEM_REQ);
	}
	while((MEM_ACCESS & MEM_ACK) != 0);			//bug
}


extern void 
ReleaseREQ(void)
{
	MEM_ACCESS = MEM_REQ;
}


static BYTE	CODE	gLBA0[] = 
{
0xEB, 0x58, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53, 0x35, 0x2E, 0x30, 0x00, 0x02, 0x08, 0x24, 0x00,
0x02, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x3F, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xF4, 0x01, 0x00, /*FAT32 size*/0x7D, 0x00, 0x00, 0x00,/*FAT32 size*/0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,		
0x01, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x29, 0x59, 0x17, 0xAB, 0xEC, 0x4E, 0x4F, 0x20, 0x4E, 0x41, 0x4D, 0x45, 0x20, 0x20,
0x20, 0x20, 0x46, 0x41, 0x54, 0x33, 0x32, 0x20, 0x20, 0x20, 0x33, 0xC9, 0x8E, 0xD1, 0xBC, 0xF4,
0x7B, 0x8E, 0xC1, 0x8E, 0xD9, 0xBD, 0x00, 0x7C, 0x88, 0x4E, 0x02, 0x8A, 0x56, 0x40, 0xB4, 0x08,
0xCD, 0x13, 0x73, 0x05, 0xB9, 0xFF, 0xFF, 0x8A, 0xF1, 0x66, 0x0F, 0xB6, 0xC6, 0x40, 0x66, 0x0F,
0xB6, 0xD1, 0x80, 0xE2, 0x3F, 0xF7, 0xE2, 0x86, 0xCD, 0xC0, 0xED, 0x06, 0x41, 0x66, 0x0F, 0xB7,
0xC9, 0x66, 0xF7, 0xE1, 0x66, 0x89, 0x46, 0xF8, 0x83, 0x7E, 0x16, 0x00, 0x75, 0x38, 0x83, 0x7E,
0x2A, 0x00, 0x77, 0x32, 0x66, 0x8B, 0x46, 0x1C, 0x66, 0x83, 0xC0, 0x0C, 0xBB, 0x00, 0x80, 0xB9,
0x01, 0x00, 0xE8, 0x2B, 0x00, 0xE9, 0x48, 0x03, 0xA0, 0xFA, 0x7D, 0xB4, 0x7D, 0x8B, 0xF0, 0xAC,
0x84, 0xC0, 0x74, 0x17, 0x3C, 0xFF, 0x74, 0x09, 0xB4, 0x0E, 0xBB, 0x07, 0x00, 0xCD, 0x10, 0xEB,
0xEE, 0xA0, 0xFB, 0x7D, 0xEB, 0xE5, 0xA0, 0xF9, 0x7D, 0xEB, 0xE0, 0x98, 0xCD, 0x16, 0xCD, 0x19,
0x66, 0x60, 0x66, 0x3B, 0x46, 0xF8, 0x0F, 0x82, 0x4A, 0x00, 0x66, 0x6A, 0x00, 0x66, 0x50, 0x06,
0x53, 0x66, 0x68, 0x10, 0x00, 0x01, 0x00, 0x80, 0x7E, 0x02, 0x00, 0x0F, 0x85, 0x20, 0x00, 0xB4,
0x41, 0xBB, 0xAA, 0x55, 0x8A, 0x56, 0x40, 0xCD, 0x13, 0x0F, 0x82, 0x1C, 0x00, 0x81, 0xFB, 0x55,
0xAA, 0x0F, 0x85, 0x14, 0x00, 0xF6, 0xC1, 0x01, 0x0F, 0x84, 0x0D, 0x00, 0xFE, 0x46, 0x02, 0xB4,
0x42, 0x8A, 0x56, 0x40, 0x8B, 0xF4, 0xCD, 0x13, 0xB0, 0xF9, 0x66, 0x58, 0x66, 0x58, 0x66, 0x58,
0x66, 0x58, 0xEB, 0x2A, 0x66, 0x33, 0xD2, 0x66, 0x0F, 0xB7, 0x4E, 0x18, 0x66, 0xF7, 0xF1, 0xFE,
0xC2, 0x8A, 0xCA, 0x66, 0x8B, 0xD0, 0x66, 0xC1, 0xEA, 0x10, 0xF7, 0x76, 0x1A, 0x86, 0xD6, 0x8A,
0x56, 0x40, 0x8A, 0xE8, 0xC0, 0xE4, 0x06, 0x0A, 0xCC, 0xB8, 0x01, 0x02, 0xCD, 0x13, 0x66, 0x61,
0x0F, 0x82, 0x54, 0xFF, 0x81, 0xC3, 0x00, 0x02, 0x66, 0x40, 0x49, 0x0F, 0x85, 0x71, 0xFF, 0xC3,
0x4E, 0x54, 0x4C, 0x44, 0x52, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x0A, 0x52, 0x65,
0x6D, 0x6F, 0x76, 0x65, 0x20, 0x64, 0x69, 0x73, 0x6B, 0x73, 0x20, 0x6F, 0x72, 0x20, 0x6F, 0x74,
0x68, 0x65, 0x72, 0x20, 0x6D, 0x65, 0x64, 0x69, 0x61, 0x2E, 0xFF, 0x0D, 0x0A, 0x44, 0x69, 0x73,
0x6B, 0x20, 0x65, 0x72, 0x72, 0x6F, 0x72, 0xFF, 0x0D, 0x0A, 0x50, 0x72, 0x65, 0x73, 0x73, 0x20,
0x61, 0x6E, 0x79, 0x20, 0x6B, 0x65, 0x79, 0x20, 0x74, 0x6F, 0x20, 0x72, 0x65, 0x73, 0x74, 0x61,
0x72, 0x74, 0x0D, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAC, 0xCB, 0xD8, 0x00, 0x00, 0x55, 0xAA,

0x52, 0x52, 0x61, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x72, 0x72, 0x41, 0x61, 0xFF, 0xFF, 0xFF, 0xFF, 0x02, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA,
};


//*/

extern VOID
OtgDevReadTxData(
	WORD		buf,
	DWORD		lba,
	BYTE		size
	)
{
	WORD	i, j;

	if (lba == 0)
	{
		for (i = 0; i < size; ++i)
		{
			for (j = 0; j < 512; ++j)
			{
				((BYTE XDATA *)buf)[i * 512 + j] = gLBA0[i * 512 + j];
			}
		}//end while//*/
	}
	else if (lba == 2)
	{
		for (i = 0; i < size; ++i)
		{
			for (j = 0; j < 512; ++j)
			{
				((BYTE XDATA *)buf)[i * 512 + j] = 0;
			}
		}//end while//*/
		((BYTE XDATA *)buf)[510] = 0x55;
		((BYTE XDATA *)buf)[511] = 0xaa;
	}
	else if (lba == 4)
	{
		for (i = 0; i < size; ++i)
		{
			for (j = 0; j < 512; ++j)
			{
				((BYTE XDATA *)buf)[i * 512 + j] = 0;
			}
		}//end while//*/
	}
	else if (lba == 6)
	{
		for (i = 0; i < size; ++i)
		{
			for (j = 0; j < 512; ++j)
			{
				((BYTE XDATA *)buf)[i * 512 + j] = gLBA0[i * 512 + j];
			}
		}//end while//*/
	}
	else if (lba == 32)
	{
		for (i = 0; i < size; ++i)
		{
			for (j = 0; j < 512; ++j)
			{
				((BYTE XDATA *)buf)[i * 512 + j] = 0;
			}
		}//end while//*/
	}
	else if (lba == 36)
	{
		for (i = 0; i < size; ++i)
		{
			for (j = 0; j < 512; ++j)
			{
				((BYTE XDATA *)buf)[i * 512 + j] = 0;
			}
		}//end while//*/
		((BYTE XDATA *)buf)[0] = 0xf8;
		((BYTE XDATA *)buf)[1] = 0xff;
		((BYTE XDATA *)buf)[2] = 0xff;
		((BYTE XDATA *)buf)[3] = 0x0f;
		((BYTE XDATA *)buf)[4] = 0xff;
		((BYTE XDATA *)buf)[5] = 0xff;
		((BYTE XDATA *)buf)[6] = 0xff;
		((BYTE XDATA *)buf)[7] = 0x0f;
		((BYTE XDATA *)buf)[8] = 0xff;
		((BYTE XDATA *)buf)[9] = 0xff;
		((BYTE XDATA *)buf)[10] = 0xff;
		((BYTE XDATA *)buf)[11] = 0x0f;
	}//*/
	else if (lba == 38)
	{
		for (i = 0; i < size; ++i)
		{
			for (j = 0; j < 512; ++j)
			{
				((BYTE XDATA *)buf)[i * 512 + j] = 0;
			}
		}//end while//*/
	}
	else
	{
		return;
	}

	

	//*/
}


extern VOID
OtgDevWriteRxData(
	WORD		buf,
	DWORD		lba,
	BYTE		size
	)
{
	buf = buf;
	lba = lba;
	size = size;
}


extern VOID 
OtgDeviceISR(VOID);

extern VOID
OtgDeviceInit(VOID);

extern VOID
OtgStorInit(
	DWORD		blockNum
	);


//
// write this function yourself, process flash data.
//
extern VOID
OtgStorDataBuffer2Flash(	// write blocks from buffer to flash
	WORD		addr,		// data buffer address
	DWORD		lba,		// logic block address of storage device
	BYTE		size		// prepare block count 
	)
{
	
}


//
// write this function yourself, process flash data.
//
extern VOID
OtgStorDataFlash2Buffer(	// read blocks from flash to buffer.
	WORD		addr,		// data buffer address
	DWORD		lba,		// logic block address of storage device
	BYTE		size		// prepare block count 
	)
{
	WORD	i, j;

	if (lba == 0)
	{
		for (j = 0; j < 512; ++j)
		{
			((BYTE XDATA *)addr)[j] = gLBA0[j];
		}
	}
	else if (lba == 1)
	{
		for (j = 0; j < 512; ++j)
		{
			((BYTE XDATA *)addr)[j] = gLBA0[512 + j];
		}
	}
	else if (lba == 2 || lba == 3)
	{
		for (j = 0; j < 512; ++j)
		{
			((BYTE XDATA *)addr)[j] = 0;
		}
		((BYTE XDATA *)addr)[510] = 0x55;
		((BYTE XDATA *)addr)[511] = 0xaa;
	}
	else if (lba == 4 || lba == 5)
	{
		for (j = 0; j < 512; ++j)
		{
			((BYTE XDATA *)addr)[j] = 0;
		}
	}
	else if (lba == 6)
	{
		for (j = 0; j < 512; ++j)
		{
			((BYTE XDATA *)addr)[j] = gLBA0[j];
		}
	}
	else if (lba == 7)
	{
		for (j = 0; j < 512; ++j)
		{
			((BYTE XDATA *)addr)[j] = gLBA0[512 + j];
		}
	}
	else if (lba == 36 || lba == 161)
	{
		for (j = 0; j < 512; ++j)
		{
			((BYTE XDATA *)addr)[j] = 0;
		}
		((BYTE XDATA *)addr)[0] = 0xf8;
		((BYTE XDATA *)addr)[1] = 0xff;
		((BYTE XDATA *)addr)[2] = 0xff;
		((BYTE XDATA *)addr)[3] = 0x0f;
		((BYTE XDATA *)addr)[4] = 0xff;
		((BYTE XDATA *)addr)[5] = 0xff;
		((BYTE XDATA *)addr)[6] = 0xff;
		((BYTE XDATA *)addr)[7] = 0xff;
		((BYTE XDATA *)addr)[8] = 0xff;
		((BYTE XDATA *)addr)[9] = 0xff;
		((BYTE XDATA *)addr)[10] = 0xff;
		((BYTE XDATA *)addr)[11] = 0x0f;
	}//*/
	else// if (lba == 37 || lba == 165)
	{
		for (j = 0; j < 512; ++j)
		{
			((BYTE XDATA *)addr)[j] = 0;
		}
	}
}

void main(void)
{
/*	OtgDevOpen();

	while(1)
	{
		OtgDevRecvPacket();

	}
//*/

	OtgStorInit(131072);		//64M, block size default is 512
	OtgDeviceInit();

	while(1)
	{
		OtgDeviceISR();
	}
	//*/
}
