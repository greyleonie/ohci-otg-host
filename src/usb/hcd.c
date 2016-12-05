/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	hcd.c
*
* Programmer:    	Grey
*
* Created: 	 		06/xx/2008
*
* Description: 		usb host controller driver interface file
*              
*        
* Change History (most recent first):	2008.06.03
*****************************************************************/

#include <string.h>
#include "../public/type.h"
#include "hcd.h"

#if USB_HOST_OHCI_EN
#include "../ohci/ohci_config.h"
#include "../ohci/ohci.h"
#endif

#if USB_HOST_OTG_EN
#include "../otg/otg.h"
#endif


URB		XDATA	gUrb;

extern BOOL
HcdOpen(VOID)
{
#if USB_HOST_OHCI_EN
	return OhciOpen();
#endif

#if USB_HOST_OTG_EN
	return OtgHostOpen();
#endif
}


extern VOID*
HcdOpenPipe(
	DWORD		pipeInfo
	)
{
#if USB_HOST_OHCI_EN
	return OhciOpenPipe(pipeInfo);
#endif

#if USB_HOST_OTG_EN
	return OtgHostOpenPipe(pipeInfo);
#endif
}


extern VOID
HcdClosePipe(
	VOID		*pipe
	)
{
#if USB_HOST_OHCI_EN
	OhciClosePipe(pipe);
#endif

#if USB_HOST_OTG_EN
	OtgHostClosePipe(pipe);
#endif
}

extern VOID
HcdSendUrb(VOID)
{
#if USB_HOST_OHCI_EN
	OhciSendUrb();
#endif

#if USB_HOST_OTG_EN
	OtgHostSendUrb();
#endif
}

extern VOID
HcdGetRHPortStatus(
	BOOL		*isConnect,			/* output: have device connection set 1, else set 0 */
	BOOL		*isChange			/* 			device connection change set 1, else set 0 */
	)
{
#if USB_HOST_OHCI_EN
	OhciRootHubPortStatus(isConnect, isChange);
#endif

#if USB_HOST_OTG_EN
	OtgHostRootHubPortStatus(isConnect, isChange);
#endif
}

extern VOID
HcdRHPortReset(VOID)
{
#if USB_HOST_OHCI_EN
	OhciRootHubPortReset();
	HcdDelayMs(ROOT_HUB_PORT_RESET_MS);
#endif

#if USB_HOST_OTG_EN
	OtgHostRootHubPortReset();
#endif
}

extern VOID
HcdRHClearFeature(VOID)
{
#if USB_HOST_OHCI_EN
	OhciRootHubClearPortFeature();
#endif

#if USB_HOST_OTG_EN
	OtgHostRootHubClearPortFeature();
#endif
}


extern VOID
HcdDelayMs(
	WORD		ms
	)
{
#if USB_HOST_OHCI_EN
	WORD		frm1, frm2;

	frm1 = OhciGetFrameNumber();

	do
	{
		frm2 = OhciGetFrameNumber();
	}
	while((frm1 + ms) != frm2);
#endif

#if USB_HOST_OTG_EN
	ms = ms;
#endif
}



