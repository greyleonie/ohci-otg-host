

#include "../host_if/host.h"

#define	GPIOD_OUT		*(volatile unsigned char xdata *)0xAE3F
#define	GPIOD_OE		*(volatile unsigned char xdata *)0xAE40

void
LedOn(void)
{
	GPIOD_OE |= 0x80;
	GPIOD_OUT |= 0x80;
}

void
LedOff(void)
{
	GPIOD_OE |= 0x80;
	GPIOD_OUT &= ~0x80;
}



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



void
main(void)
{
	unsigned char	tick = 0, flag, status;

	HostInit();

	while(1)
	{
		status = HostGetStatus();
		if (status == HOST_STOR_READY)
		{
			flag = HostReadBlock(tick, 0x1400, 2);
			if (flag != 0)
			{
				flag = 1;
			}
			HostWriteBlock(tick, 0x1400, 2);
		}
		else
		{
			HostPoll(1);
		}
	}/* end while */

}
