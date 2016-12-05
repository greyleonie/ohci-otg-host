/*****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2008, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	ohci.h
*
* Programmer:    	Grey
*
* Created: 	 		04/xx/2008
*
* Description: 		usb ohci host controller head file
*              
*        
* Change History (most recent first):	2008.04.10
*****************************************************************/

#ifndef	__OHCI_H__
#define	__OHCI_H__




/*
 * define ohci host controller buffer
 */

#define	MAX_ED_TD_SIZE		32		//support max ED and TD count 32

#define	OHCI_REG_BASE		(SYS_OHCI_REG_BASE)						//0xAF00 ~ 0xAFFF		(256 bytes)

#define	OHCI_HCCA_BASE		(SYS_OHCI_MEMORY_BASE)					//0xF000 ~ 0xF0FF		(256 bytes)
#define	OHCI_ED_TD_BASE		(OHCI_HCCA_BASE + 256)					//0xF100 ~ 0xF2FF		(512 bytes = 16bytes * 32)
#define	OHCI_SETUP_BASE		(OHCI_ED_TD_BASE + MAX_ED_TD_SIZE * 16)	//0xF300 ~ 0xF307		(8 bytes)
#define	OHCI_BUF_BASE		(OHCI_SETUP_BASE + 8)					//0xF308 ~ 0xF3FF		(248 bytes)



/*
 * define ohci host controller register
 */
typedef struct _HC_OPERATIONAL_REGISTER 
{
	DWORD					HcRevision;
	DWORD					HcControl;
	DWORD					HcCommandStatus;
	DWORD					HcInterruptStatus;
	DWORD					HcInterruptEnable;
	DWORD					HcInterruptDisable;
	DWORD					HcHCCA;
	DWORD					HcPeriodCurrentED;
	DWORD					HcControlHeadED;
	DWORD					HcControlCurrentED;
	DWORD					HcBulkHeadED;
	DWORD					HcBulkCurrentED;
	DWORD					HcDoneHead;
	DWORD					HcFmInterval;
	DWORD					HcFmRemaining;
	DWORD					HcFmNumber;
	DWORD					HcPeriodicStart;
	DWORD					HcLSThreshold;
	DWORD					HcRhDescriptorA;
	DWORD					HcRhDescriptorB;
	DWORD					HcRhStatus;
	DWORD					HcRhPortStatus[4];
} HC_OPERATIONAL_REGISTER;


//#define HcRevision			0x0000
#define HcRevision10		0x10	/* Revision 1.0 */

//#define HcControl			0x0004
#define HcControlCBSR			0x003UL	/* ControlBulkServiceRatio */
#define HcControlPLE			0x004UL	/* PeriodicListEnable */
#define HcControlIE				0x008UL	/* IsochronousEnable */
#define HcControlCLE			0x010UL	/* ControlListEnable */
#define HcControlBLE			0x020UL	/* BulkListEnable */
#define HcControlHCFS			0x0C0UL	/* HostControllerFunctionalState */
#define HcControlHCFS_RESET		0x000
#define HcControlHCFS_RESUME	0x040
#define HcControlHCFS_OPER		0x080
#define HcControlHCFS_SUSPEND	0x0C0

#define HcControlIR				0x100UL	/* InterruptRouting */
#define HcControlRWC			0x200UL	/* RemoteWakeupConnected */
#define HcControlRWE			0x400UL	/* RemoteWakeupEnable */

//#define HcCommandStatus		0x0008
#define HcCommandStatusHCR		0x00001	/* HostControllerReset */
#define HcCommandStatusCLF		0x00002	/* ControlListFilled */
#define HcCommandStatusBLF		0x00004	/* BulkListFilled */
#define HcCommandStatusOCR		0x00008	/* OwnershipChangeRequest */
#define HcCommandStatusSOC		0x30000	/* SchedulingOverrunCount */

/*#define HcInterruptStatus	0x000C
#define HcInterruptEnable	0x0010
#define HcInterruptDisable	0x0014*/
#define HcInterruptSO			0x01	/* SchedulingOverrun */
#define HcInterruptWDH			0x02	/* WritebackDoneHead */
#define HcInterruptSF			0x04	/* StartofFrame */
#define HcInterruptRD			0x08	/* ResumeDetected */
#define HcInterruptUE			0x10	/* UnrecoverableError */
#define HcInterruptFNO			0x20	/* FrameNumberOverflow */
#define HcInterruptRHSC			0x40	/* RootHubStatusChange */
#define HcInterruptOC		0x40000000	/* OwnershipChange */
#define HcInterruptMIE		0x80000000	/* MasterInterruptEnable */

/*#define HcHCCA				0x0018
#define HcPeriodCurrentED	0x001C
#define HcControlHeadED		0x0020
#define HcControlCurrentED	0x0024
#define HcBulkHeadED		0x0028
#define HcBulkCurrentED		0x002C
#define HcDoneHead			0x0030

#define HcFmInterval		0x0034*/
#define HcFmIntervalFIRST		0x2EDF		/* FrameInterval reset value */
#define HcFmIntervalFIT			0x80000000	/* FrameIntervalToggle */

//#define HcFmRemaining		0x0038
#define HcFmRemainingFRT		0x80000000	/* FrameRemainingToggle */

/*#define HcFmNumber		0x003C
#define HcPeriodicStart		0x0040
#define HcLSThreshold		0x0044*/

//#define HcRhDescriptorA		0x0048
#define HcRhDescriptorNDP		0x000000FF	// NumberDownstreamPorts
#define HcRhDescriptorPSM		0x00000100	// PowerSwitchingMode
#define HcRhDescriptorNPS		0x00000200	// NoPowerSwitching
#define HcRhDescriptorDT		0x00000400	// DeviceType
#define HcRhDescriptorOCPM		0x00000800	// OverCurrentProtectionMode
#define HcRhDescriptorNOCP		0x00001000	// NoOverCurrentProtection
#define HcRhDescriptorPOTPGT	0xF0000000	// PowerOnToPowerGoodTime

//#define HcRhDescriptorB		0x004C
#define HcRhDescriptorDR		0x0000FFFF	// DeviceRemovable
#define HcRhDescriptorPPCM		0xFFFF0000	// PortPowerControlMask


//#define HcRhStatus			0x0050
#define HcRhStatusLPS			0x00000001	// LocalPowerStatus
#define HcRhStatusOCI			0x00000002	// OverCurrentIndicator
#define HcRhStatusDRWE			0x00008000	// DeviceRemoteWakeupEnable
#define HcRhStatusLPSC			0x00010000	// LocalPowerStatusChange
#define HcRhStatusOCIC			0x00020000	// OverCurentIndicatorChange
#define HcRhStatusCRWE			0x80000000	// ClearRemoteWakeupEnable

//#define HcRhPortStatus		0x0054
#define HcRhPortStatusCCS		0x00000001	// CurrentConnectStatus
#define HcRhPortStatusPES		0x00000002	// PortEnableStatus
#define HcRhPortStatusPSS		0x00000004	// PortSuspendStatus
#define HcRhPortStatusPOCI		0x00000008	// PortOverCurrentIndicator
#define HcRhPortStatusPRS		0x00000010	// PortResetStatus
#define HcRhPortStatusPPS		0x00000100	// PortPowerStatus
#define HcRhPortStatusLSDA		0x00000200	// LowSpeedDeviceAttached
#define HcRhPortStatusCSC		0x00010000	// ConnectStatusChange
#define HcRhPortStatusPESC		0x00020000	// PortEnableStatusChange
#define HcRhPortStatusPSSC		0x00040000	// PortSuspendStatusChange
#define HcRhPortStatusOCIC		0x00080000	// PortOvercurrentIndicatorChange
#define HcRhPortStatusPRSC		0x00100000	// PortResetStatusChange


/*typedef struct _HC_ENDPOINT_CONTROL {
	DWORD	FunctionAddress:	7;
	DWORD	EndpointNumber:		4;
	DWORD	Direction:			2;		// 00b/11b Get direction From TD
										// 01b OUT
										// 10b IN
	DWORD	Speed:				1;		// Full = 0, Low = 1
	DWORD	sKip:				1;		// Skip = 1
	DWORD	Format:				1;		// ISO = 1, others = 0
	DWORD	MaximumPacketSize:	11;
	DWORD	Reserved:			5;
} HC_ENDPOINT_CONTROL*/

#define OHCI_ED_CONTROL_FunctionAddress		0x0000007F
#define OHCI_ED_CONTROL_EndpointNumber		0x00000780
#define OHCI_ED_CONTROL_Direction			0x00001800
#define OHCI_ED_CONTROL_Speed				0x00002000
#define OHCI_ED_CONTROL_sKip				0x00004000
#define OHCI_ED_CONTROL_Format				0x00008000
#define OHCI_ED_CONTROL_MaximumPacketSize	0x07FF0000
#define OHCI_ED_CONTROL_Reserved			0xF8000000

#define OHCI_ED_CONTROL_MASK_FunctionAddress	0xFFFFFF80
#define OHCI_ED_CONTROL_MASK_MaximumPacketSize	0xF800FFFF


//
// Host Controller Endpoint Descriptor, refer to Section 4.2, Endpoint Descriptor 
// This structure should be 16 bytes aligned
// 
typedef struct _OHCI_ENDPOINT_DESCRIPTOR 
{
	volatile DWORD /*HC_ENDPOINT_CONTROL*/	Control;	// dword 0
	volatile DWORD							TailP;		// physical pointer to OHCI_TRANSFER_DESCRIPTOR
	volatile DWORD							HeadP;		// flags + phys ptr to OHCI_TRANSFER_DESCRIPTOR
	volatile DWORD							NextED;		// phys ptr to HC_ENDPOINT_DESCRIPTOR
} OHCI_ENDPOINT_DESCRIPTOR;


#define HcEDHeadP_HALT	0x00000001	//hardware stopped bit
#define HcEDHeadP_CARRY	0x00000002	//hardware toggle carry bit


/*typedef struct _OHCI_TRANSFER_CONTROL {
	DWORD	Reserved: 18;
	DWORD	bufferRounding: 1;
	DWORD	DirectionPID: 2;
	DWORD	DelayInterrupt: 3;
	DWORD	DataToggle: 2;
	DWORD	ErrorCount: 2;
	DWORD	ConditionCode: 4;
} OHCI_TRANSFER_CONTROL, *POHCI_TRANSFER_CONTROL;*/

#define OHCI_TD_CONTROL_Reserved			0x0003FFFF
#define OHCI_TD_CONTROL_bufferRounding		0x00040000
#define OHCI_TD_CONTROL_DirectionPID		0x00180000
#define OHCI_TD_CONTROL_DelayInterrupt		0x00E00000
#define OHCI_TD_CONTROL_DataToggle			0x03000000
#define OHCI_TD_CONTROL_ErrorCount			0x0C000000
#define OHCI_TD_CONTROL_ConditionCode		0xF0000000

#define TD_CONTROL_Reserved				0
#define TD_CONTROL_bufferRounding		18
#define TD_CONTROL_DirectionPID			19
#define TD_CONTROL_DelayInterrupt		21
#define TD_CONTROL_DataToggle			24
#define TD_CONTROL_ErrorCount			26
#define TD_CONTROL_ConditionCode		28



//
// Host Controller Transfer Descriptor, refer to Section 4.3, Transfer Descriptors
// This structure should be 16 bytes aligned
//
typedef struct _OHCI_TRANSFER_DESCRIPTOR 
{
	volatile DWORD /*OHCI_TRANSFER_CONTROL*/	Control;	// dword 0
	DWORD										CBP;		// FIXIT!!! PVOID
	volatile DWORD								NextTD;		// phys ptr to OHCI_TRANSFER_DESCRIPTOR
	DWORD										BE;			// FIXIT!!! PVOID
} OHCI_TRANSFER_DESCRIPTOR;


typedef struct _OHCI_HCCA_BLOCK 
{
	DWORD		HccaInterruptTable[32];		// Pointers to Interrupt ED
	//WORD		HccaFrameNumber;			// Current frame number
	//WORD		HccaPad1;					// Set to 0 when each FmNum updated
	DWORD		HccaFrameNumber;			// Current frame number
	DWORD		HccaDoneHead;				// Info returned for an interrupt
	DWORD		Reserved[29];				// Reserved for use by HC
} OHCI_HCCA_BLOCK;



//
// The different ED lists are as follows.  
//
#define	ED_INTERRUPT_1ms			0
#define	ED_INTERRUPT_2ms			1
#define	ED_INTERRUPT_4ms			3
#define	ED_INTERRUPT_8ms			7
#define	ED_INTERRUPT_16ms			15
#define	ED_INTERRUPT_32ms			31
#define	ED_CONTROL					63
#define	ED_BULK						64
#define	ED_ISOCHRONOUS				0		// same as 1ms interrupt queue 
#define	NO_ED_LIST					65
#define	ED_EOF						0xff


/*
 * DirectionPID Value
 */
#define DirSETUP		0
#define DirOUT			1
#define DirIN			2

/*
 * data toggle value
 */
#define DATA0				0
#define DATA1				1


extern VOID
OhciWrite(
	DWORD			*reg,
	DWORD			val
	);

extern DWORD
OhciRead(
	DWORD			*reg
	);


extern VOID*
OhciAllocEDTD(VOID);

extern VOID
OhciFreeEDTD(
	VOID	*ed_td
	);


extern BOOL
OhciQueueGeneralRequest(
	OHCI_ENDPOINT_DESCRIPTOR	*HcED
	);

extern VOID
OhciProcessDoneQueue(
	DWORD			hccaDoneHead
	);

extern BOOL
OhciInterruptService(VOID);

extern BOOL
OhciOpen(VOID);

extern VOID
OhciSendUrb(VOID);


extern VOID
OhciRootHubPortStatus(
	BOOL		*isConnect,
	BOOL		*isChange
	);

extern VOID
OhciRootHubPortReset(VOID);

extern VOID
OhciRootHubClearPortFeature(VOID);

extern VOID*
OhciOpenPipe(
	DWORD			pipeInfo
	);

extern VOID
OhciClosePipe(
	VOID			*pipe
	);

extern WORD
OhciGetFrameNumber(VOID);



#endif