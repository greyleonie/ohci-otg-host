/****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2007, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	otg.h
*
* Programmer:    	Grey
*
* Created: 	 		09/xx/2008
*
* Description: 		OTG hareware abstract layer head file
*					
*              
*****************************************************************/

#ifndef __OTG_H__
#define	__OTG_H__




/* common USB register */
typedef struct _COMMON_USB_REG
{
	BYTE	FAddr;					
/**************************************************
*	FAddr			address:00h, Reset value:8'h00 
*	FAddr is an 8-bit register that should be written with the 7-bit address of the peripheral part of the transaction.
*	When the core is being used in Host mode (DevCtl.D2=1), this register should be set to the value sent in a SET _ADDRESS
*	command during device enumeration as the address for the peripheral device.
*	When the core is being used in Peripheral mode (DevCtl.D2=0), this register should be written with the address received through a
*	SET_ADDRESS command, which will then be used for decoding the function address in subsequent token packets. Note: The new
*	address will not take effect immediately as the host will still be using the old address for the Status stage of the device request. The
*	core will continue to use the old address for decoding packets until the device request has completed.
**************************************************/
	BYTE	Power;					
/**************************************************
*	Power			address:01h, Reset value:8'h20 
*	Power is an 8-bit register that is used for controlling Suspend and Resume signaling, 
*	and some basic operational aspects of the core.
**************************************************/
	WORD	IntrTx;					
/**************************************************
*	IntrTx			address:02h, Reset value:16'h0000 
*	IntrTx is a 16-bit read-only register that indicates which interrupts are currently active for Endpoint 0 and the Tx Endpoints 1每15.
*	Note: Bits relating to endpoints that have not been configured will always return 0. Note also that all active interrupts are cleared
*	when this register is read.
**************************************************/
	WORD	IntrRx;					
/**************************************************
*	IntrRx			address:04h, Reset value:16'h0000 
*	IntrRx is an 16-bit read-only register that indicates which of the interrupts for Rx Endpoints 1 每 15 are currently active. Note: Bits
*	relating to endpoints that have not been configured will always return 0. Note also that all active interrupts are cleared when this
*	register is read.
**************************************************/
	WORD	IntrTxEn;				
/**************************************************
*	IntrTxE			address:06h, Reset value:16'hFFFF masked with the Tx endpoints implemented
*	IntrTxE is a 16-bit register that provides interrupt enable bits for the interrupts in IntrTx. Where a bit is set to 1, MC_NINT will
*	be asserted on the corresponding interrupt in the IntrTx register becoming set. Where a bit is set to 0, the interrupt in IntrTx is still set
*	but MC_NINT is not asserted. On reset, the bits corresponding to Endpoint 0 and the Tx endpoints included in the design are set to
*	1, while the remaining bits are set to 0. Note: Bits relating to endpoints that have not been configured will always return 0.
**************************************************/
	WORD	IntrRxEn;				
/**************************************************
*	IntrRxE			address:08h, Reset value:16'hFFFE masked with the Rx endpoints implemented
*	IntrRxE is a 16-bit register that provides interrupt enable bits for the interrupts in IntrRx. Where a bit is set to 1, MC_NINT will
*	be asserted on the corresponding interrupt in the IntrRx register becoming set. Where a bit is set to 0, the interrupt in IntrTx is still set
*	but MC_NINT is not asserted. On reset, the bits corresponding to the Rx endpoints included in the design are set to 1, while the
*	remaining bits are set to 0. Note: Bits relating to endpoints that have not been configured will always return 0.
**************************************************/
	BYTE	IntrUSB;				
/**************************************************
*	IntrUSB			address:0Ah, Reset value:8'h00
*	IntrUSB is an 8-bit read-only register that indicates which USB interrupts are currently active. 
*	All active interrupts will be cleared when this register is read.
**************************************************/
	BYTE	IntrUSBE;	
/**************************************************
*	IntrUSBE		address:0Bh, Reset value:8'h06
*	IntrUSBE is an 8-bit register that provides interrupt enable bits for each of the interrupts in IntrUSB.
**************************************************/			
	WORD	Frame;					
/**************************************************
*	Frame		address:0Ch, Reset value:16'h0000
*	Frame is a 16-bit register that holds the last received frame number.
**************************************************/	
	BYTE	Index;					
/**************************************************
*	Index		address:0Eh, Reset value:8'h00
*	Each Tx endpoint and each Rx endpoint have their own set of control/status registers located between 100h 每 1FFh. In addition one
*	set of Tx control/status and one set of Rx control/status registers appear at 10h 每 19h. Index is a 4-bit register that
*	determines which endpoint control/status registers are accessed.
**************************************************/	
	BYTE	TestMode;				
/**************************************************
*	TestMode	address:0Fh, Reset value:8'h00
*	Testmode is an 8-bit register that is primarily used to put the core into one of the four test modes for High-speed operation
*	described in the USB 2.0 specification 每 in response to a SET FEATURE: TESTMODE command. It is not used in norma
*	operation.
**************************************************/
}COMMON_USB_REG;


#define	Power_prw_IsoUpdate				(1 << 7)			
/**************************************************
*	When set by the CPU, the core will wait for an SOF token from the time TxPktRdy is the
*	before sending the packet. If an IN token is received before and SOF token, then a zero length
* data packet will be sent.
*	Note: Only valid in Peripheral Mode. alse, this bit only afects endpoints performing Isochronous
*				transfers.
**************************************************/	

#define	Power_prw_SoftConn				(1 << 6)				
/**************************************************
*	If Soft Connect/Disconnect feature is enabled, then the USB D+/D- lines are enabled when this bit
* is set by the CPU and tri-stated when this bit is cleared by the CPU. (See Section 5.1) 
*	Note: Only valid in Peripheral Mode.
**************************************************/	

#define	Power_prw_HSEnab				(1 << 5)		
#define	Power_hrw_HSEnab				(1 << 5)
/**************************************************
*	When set by the CPU, the core will negotiate for High-speed mode when the device is reset
* by the hub. If not set, the device will only operate in Full-speed mode.
**************************************************/	
		
#define	Power_pr_HSMode					(1 << 4)	
#define	Power_hr_HSMode					(1 << 4)		
/**************************************************
*	When set, this read-only bit indicates High-speed mode successfully negotiated during USB reset.
*	In Peripheral Mode, becomes valid when USB reset completes (as indicated by USB reset interrupt).
*	In Host Mode, becomes valid when Reset bit is cleared. Remains valid for the duration of the session.
*	Note: Allowance is made for Tiny -J signaling in determining the transfer speed to select.
**************************************************/	
	
#define	Power_pr_Reset					(1 << 3)	
#define	Power_hrw_Reset					(1 << 3)	
/**************************************************
*	This bit is set when Reset signaling is present on the bus. 
*	Note: This bit is Read/Write from the CPU in Host Mode but Read-Only in Peripheral Mode.
**************************************************/	

#define	Power_prw_Resume				(1 << 2)	
#define	Power_hrw_Resume				(1 << 2)
/**************************************************
*	Set by the CPU to generate Resume signaling when the function is in Suspend mode. The CPU should clear
*	this bit after 10 ms (a maximum of 15 ms) to end Resume signaling. In Host mode, this bit is also
*	automatically set when Resume signaling from the target is detected while the core is suspended.
**************************************************/	
			
#define	Power_pr_SuspendMode			(1 << 1)	
#define	Power_hs_SuspendMode			(1 << 1)
/**************************************************
*	In Host mode, this bit is set by the CPU to enter Suspend mode. In Peripheral mode, this bit is set on entry
*	into Suspend mode. It is cleared when the CPU reads the interrupt register, or sets the Resume bit above.
**************************************************/	
			
#define	Power_prw_EnSuspendMode			(1 << 0)	
#define	Power_hrw_EnSuspendMode			(1 << 0)		
/**************************************************
*	Set by the CPU to enable the SUSPENDM signal.
**************************************************/	
	



#define	IntUSB_VBusErr					(1 << 7)
/**************************************************
*	Set when VBus drops below the VBus Valid threshold during a session. Only valid when
*	core is ＆A＊ device.
**************************************************/

#define	IntUSB_SessReq					(1 << 6)
/**************************************************
*	Set when Session Request signaling has been detected. Only valid when core is ＆A＊ device.
**************************************************/

#define	IntUSB_DisCon					(1 << 5)
/**************************************************
*	Set in Host mode when a device disconnect is detected. Set in Peripheral mode when a session
*	ends. Valid at all transaction speeds.
**************************************************/

#define	IntUSB_Conn						(1 << 4)
/**************************************************
*	Set when a device connection is detected. Only valid in Host mode. Valid at all transaction speeds.
**************************************************/

#define	IntUSB_SOF						(1 << 3)
/**************************************************
*	Set when a new frame starts.
**************************************************/

#define	IntUSB_p_Reset					(1 << 2)
#define	IntUSB_h_Babble					(1 << 2)
/**************************************************
*	Set in Peripheral mode when Reset signaling is detected on the bus.
*	Set in Host mode when babble is detected. 
*	Note: Only active after first SOF has been sent.
**************************************************/

#define	IntUSB_Resume					(1 << 1)
/**************************************************
*	Set when Resume signaling is detected on the bus while the core is in Suspend mode.
**************************************************/

#define	IntUSB_Suspend					(1 << 0)
/**************************************************
*	Set when Suspend signaling is detected on the bus. Only valid in Peripheral mode.
**************************************************/



/**************************************************
*	TestMode bit define, see 2.2.11
**************************************************/	
#define	TestMode_rw_ForceHost			(1 << 7)
#define	TestMode_rw_FIFOAccess			(1 << 6)
#define	TestMode_rw_ForceFS				(1 << 5)
#define	TestMode_rw_ForceHS				(1 << 4)
#define	TestMode_rw_Packet				(1 << 3)
#define	TestMode_rw_K					(1 << 2)
#define	TestMode_rw_J					(1 << 1)
#define	TestMode_rw_SE0_NAK				(1 << 0)




/* Indexed CSR register */
typedef struct _CSR_REG
{
	WORD	TxMaxP;
/**************************************************
*	TxMaxP		address:10h, Reset value:16'h0000
*	The TxMaxP register defines the maximum amount of data that can be transferred through the selected Tx endpoint in a single
*	operation. There is a TxMaxP register for each Tx endpoint (except Endpoint 0).
*
*	Bits 10:0 define (in bytes) the maximum payload transmitted in a single transaction. The value set can be up to 1024 bytes but is
*	subject to the constraints placed by the USB Specification on packet sizes for Bulk, Interrupt and Isochronous transfers in Fullspeed
*	and High-speed operations.
*
*	Where the option of High-bandwidth Isochronous/Interrupt endpoints or of packet splitting on Bulk endpoints has been taken
*	when the core is configured, the register includes either 2 or 5 further bits that define a multiplier m which is equal to one more than
*	the value recorded.
**************************************************/
	WORD	CSR0_TxCSR;
/**************************************************
*	CSR0		address:12h, Reset value:16'h0000
*	CSR0 is a 16-bit register that provides control and status bits for Endpoint 0. Note: The interpretation of the register depends on
*	whether the core is acting as a peripheral or as a host. Users should also be aware that the value returned when the register is
*	read reflects the status attained e.g. as a result of writing to the register.
**************************************************/
/**************************************************
*	TxCSR		address:12h, Reset value:16'h0000
*	TxCSR is a 16-bit register that provides control and status bits for transfers through the currently-selected Tx endpoint. There is a
*	TxCSR register for each configured Tx endpoint (not including Endpoint 0). 
*	Note: The interpretation of the register depends on whether the core is acting as a peripheral or as a host. 
*	Users should also be aware that the value returned when the register is read reflects the status attained e.g. 
*	as a result of writing to the register.
**************************************************/
	WORD	RxMaxP;
/**************************************************
*	RxMaxP		address:14h, Reset value:16'h0000
*	The RxMaxP register defines the maximum amount of data that can be transferred through the selected Rx endpoint in a single
*	operation. There is a RxMaxP register for each Rx endpoint (except Endpoint 0).
*
*	Bits 10:0 define (in bytes) the maximum payload transmitted in a single transaction. The value set can be up to 1024 bytes but is
*	subject to the constraints placed by the USB Specification on packet sizes for Bulk, Interrupt and Isochronous transfers in Fullspeed
*	and High-speed operations.
*
*	Where the option of High-bandwidth Isochronous/Interrupt endpoints or of combini ng Bulk packets has been taken when the core is
*	configured, the register includes either 2 or 5 further bits that define a multiplier m which is equal to one more than the value recorded.
**************************************************/
	WORD	RxCSR;
/**************************************************
*	RxCSR		address:16h, Reset value:16'h0000
*	RxCSR is an 16-bit register that provides control and status bits for transfers through the currently-selected Rx endpoint. There is an
*	RxCSR register for each configured Rx endpoint (not including Endpoint 0). 
*	Note: The interpretation of the register depends on whether the core is acting as a peripheral or as a host. 
*	Users should also be aware that the value returned when the register is read reflects the status attained e.g. as a result of writing to the register.
**************************************************/
	WORD	Count0_RxCount;
/**************************************************
*	Count0		address:18h, Reset value:7'b0000000
*	Count0 is a 7-bit read-only register that indicates the number of received data bytes in the Endpoint 0 FIFO. The value returned
*	changes as the contents of the FIFO change and is only valid while RxPktRdy (CSR0.D0) is set.
**************************************************/
/**************************************************
*	RxCount		address:18h, Reset value:13'b0000000000000
*	CRxCount is a 13-bit read-only register that holds the number of received data bytes in the packet currently in line to be read from the
*	Rx FIFO. If the packet was transmitted as multiple bulk packets, the number given will be for the combined packet. Note: The value
*	returned changes as the FIFO is unloaded and is only valid while RxPktRdy (RxCSR.D0) is set.
**************************************************/
	BYTE	TxType;
/**************************************************
*	TxType		address:1Ah, Reset value:8'h00(host mode only)
*	TxType is a 6-bit register that should be written with the endpoint number to be targeted by the endpoint in the lower 4 bits, and
*	the transaction protocol to use for the currently-selected Tx endpoint in the upper 2 bits.
*	There is a TxType register for each configured Tx endpoint (except Endpoint 0).
**************************************************/
	BYTE	NAKLimit0_TxInterval;
/**************************************************
*	NAKLimit0		address:1Bh, Reset value:5'b00000(host mode only)
*	NAKLimit0 is a 5-bit register that sets the number of frames/microframes (High-Speed transfers) after which Endpoint 0 should
*	timeout on receiving a stream of NAK responses. (Equivalent settings for other endpoints can be made through their TxInterval
*	and RxInterval registers: see Sections 2.3.11 and 2.3.13.)
*	The number of frames/microframes selected is 2(m-1) (where m is the value set in the register, valid values 2 每 16). If the host receives
*	NAK responses from the target for more frames than the number represented by the Limit set in this register, the endpoint will be
*	halted. Note: A value of 0 or 1 disables the NAK timeout function.
**************************************************/
/**************************************************
*	TxInterval		address:1Bh, Reset value:8'b00000000(host mode only)
*	TxInterval is an 8-bit register that, for Interrupt and Isochronous transfers, defines the polling interval for the currently-selected Tx
*	endpoint. For Bulk endpoints, this register sets the number of frames/microframes after which the endpoint should timeout on
*	receiving a stream of NAK responses. There is a TxInterval register for each configured Tx endpoint (except Endpoint 0).
**************************************************/
	BYTE	RxType;
/**************************************************
*	RxType		address:1Ch, Reset value:8'h00(host mode only)
*	RxType is a 6-bit register that should be written with the endpoint number to be targeted by the endpoint in the lower 4 bits, and
*	the transaction protocol to use for the currently-selected Rx endpoint in the upper 2 bits.
*	There is an RxType register for each configured Rx endpoint (except Endpoint 0).
**************************************************/
	BYTE	RxInterval;
/**************************************************
*	RxInterval		address:1Dh, Reset value:8'b00000000(host mode only)
*	RxInterval is an 8-bit register that, for Interrupt and Isochronous transfers, defines the polling interval for the currently-selected Rx
*	endpoint. For Bulk endpoints, this register sets the number of frames/microframes after which the endpoint should timeout on
*	receiving a stream of NAK responses. There is a RxInterval register for each configured Rx endpoint (except Endpoint 0).
**************************************************/
	BYTE	reserved;
	BYTE	ConfigData_FIFOSize;
/**************************************************
*	ConfigData		address:1Fh, Reset value:Configuration Dependent
*	ConfigData is an 8-bit Read-Only register that returns information about the selected core configuration.
**************************************************/
/**************************************************
*	FIFOSize		address:1Fh, Reset value:Configuration Dependent
*	FIFOSize is an 8-bit Read-Only register that returns the sizes of the FIFOs associated with the selected additional Tx/Rx endpoints.
*	The lower nibble encodes the size of the selected Tx endpoint FIFO; the upper nibble encodes the size of the selected Rx endpoint
*	FIFO. Values of 3 每 13 correspond to a FIFO size of 2n bytes (8 每 8192 bytes). If an endpoint has not been configured, a value of 0
*	will be displayed. Where the Tx and Rx endpoints share the same FIFO, the Rx FIFO size will be encoded as 0xF.
*	Note: The register only has this interpretation when the Index register is set to select one of Endpoints 1 每 15 and Dynamic Sizing
*	is not selected. It has a special interpretation when the Index register is set to select Endpoint 0 (see Section 2.3.3), while the result
*	returned is not valid where Dynamic FIFO sizing is used.
**************************************************/
}CSR_REG;


#define	CSR0_ps_FlushFIFO				(1 << 8)
/**************************************************
*	The CPU writes a 1 to this bit to flush the next packet to be transmitted/read from the
*	Endpoint 0 FIFO. The FIFO pointer is reset and the TxPktRdy/RxPktRdy bit (below) is
*	cleared. Note: FlushFIFO should only be used when TxPktRdy/RxPktRdy is set. At other
*	times, it may cause data to be corrupted.
**************************************************/

#define	CSR0_ps_ServicedSetupEnd		(1 << 7)
/**************************************************
*	The CPU writes a 1 to this bit to clear the SetupEnd bit. It is cleared automatically.
**************************************************/

#define	CSR0_ps_ServicedRxPktRdy		(1 << 6)
/**************************************************
*	The CPU writes a 1 to this bit to clear the RxPktRdy bit. It is cleared automatically.
**************************************************/

#define	CSR0_ps_SendStall				(1 << 5)
/**************************************************
*	The CPU writes a 1 to this bit to terminate the current transaction. The STALL handshake
*	will be transmitted and then this bit will be cleared automatically. 
*	Note: The FIFO should be flushed before SendStal is set.
**************************************************/

#define	CSR0_pr_SetupEnd				(1 << 4)
/**************************************************
*	This bit will be set when a control transaction ends before the DataEnd bit has been set.
*	An interrupt will be generated and the FIFO flushed at this time. The bit is cleared by
*	the CPU writing a 1 to the ServicedSetupEnd bit.
**************************************************/

#define	CSR0_ps_DataEnd					(1 << 3)
/**************************************************
*	The CPU sets this bit:
*	1. When setting TxPktRdy for the last data packet.
*	2. When clearing RxPktRdy after unloading the last data packet.
*	3. When setting TxPktRdy for a zero length data packet.
*	It is cleared automatically.
**************************************************/

#define	CSR0_prc_SentStall				(1 << 2)
/**************************************************
*	This bit is set when a STALL handshake is transmitted. The CPU should clear this bit.
**************************************************/

#define	CSR0_prs_TxPktRdy				(1 << 1)
/**************************************************
*	The CPU sets this bit after loading a data packet into the FIFO. It is cleared automatically when the
* data packet has been transmitted. An interrupt is generated (if enabled) when the bit is cleared.
**************************************************/

#define	CSR0_pr_RxPktRdy				(1 << 0)
/**************************************************
*	This bit is set when a data packet has been received. An interrupt is generated when this bit
*	is set. The CPU clears this bit by setting the ServicedRxPktRdy bit.
**************************************************/


#define	CSR0_hrw_DisPing				(1 << 11)
/**************************************************
*	The CPU writes a 1 to this bit to instruct the core not to issue PING tokens in data and status
*	phases of a high-speed Control transfer (for use with devices that do not respond to PING).
**************************************************/

#define	CSR0_hs_FlushFIFO				(1 << 8)
/**************************************************
*	The CPU writes a 1 to this bit to flush the next packet to be transmitted/read from the
*	Endpoint 0 FIFO. The FIFO pointer is reset and the TxPktRdy/RxPktRdy bit (below) is
*	cleared. Note: FlushFIFO should only be used when TxPktRdy/RxPktRdy is set. At other
*	times, it may cause data to be corrupted.
**************************************************/

#define	CSR0_hrc_NAKTimeout				(1 << 7)
/**************************************************
*	This bit will be set when Endpoint 0 is halted following the receipt of NAK responses for
*	longer than the time set by the NAKLimit0 register. The CPU should clear this bit to allow
*	the endpoint to continue.
**************************************************/

#define	CSR0_hrw_StatusPkt				(1 << 6)
/**************************************************
*	The CPU sets this bit at the same time as the TxPktRdy or ReqPkt bit is set, to perform a
*	status stage transaction. Setting this bit ensures that the data toggle is set to 1 so that a
*	DATA1 packet is used for the Status Stage transaction.
**************************************************/

#define	CSR0_hrw_ReqPkt					(1 << 5)
/**************************************************
*	The CPU sets this bit to request an IN transaction. It is cleared when RxPktRdy is set.
**************************************************/

#define	CSR0_hrc_Error					(1 << 4)
/**************************************************
*	This bit will be set when three attempts have been made to perform a transaction with no
*	response from the peripheral. The CPU should clear this bit. An interrupt is generated
*	when this bit is set.
**************************************************/

#define	CSR0_hrw_SetupPkt				(1 << 3)
/**************************************************
*	The CPU sets this bit, at the same time as the TxPktRdy bit is set, to send a SETUP token
*	instead of an OUT token for the transaction.
**************************************************/

#define	CSR0_hrc_RxStall				(1 << 2)
/**************************************************
*	This bit is set when a STALL handshake is received. The CPU should clear this bit.
**************************************************/

#define	CSR0_hrs_TxPktRdy				(1 << 1)
/**************************************************
*	The CPU sets this bit after loading a data packet into the FIFO. It is cleared automatically
*	when the data packet has been transmitted. An interrupt is generated (if enabled) when the
*	bit is cleared.
**************************************************/

#define	CSR0_hrc_RxPktRdy				(1 << 0)
/**************************************************
*	This bit is set when a data packet has been received. An interrupt is generated (if enabled) when
*	this bit is set. The CPU should clear this bit when the packet has been read from the FIFO.
**************************************************/




#define	TxCSR_prw_AutoSet				(1 << 15)
/**************************************************
*	If the CPU sets this bit, TxPktRdy will be automatically set when data of the maximum packet size
*	(value in TxMaxP) is loaded into the Tx FIFO. If a packet of less than the maximum packet size is
*	loaded, then TxPktRdy will have to be set manually. 
*	Note: Should not be set for either high-bandwidth Isochronous endpoints or high-bandwidth Interrupt endpoints.
**************************************************/

#define TxCSR_prw_ISO					(1 << 14)
/**************************************************
*	The CPU sets this bit to enable the Tx endpoint for Isochronous transfers, and clears it to enable
*	the Tx endpoint for Bulk or Interrupt transfers. 
*	Note: This bit only has any efect in Peripheral mode. In Host mode, it always returns zero.
**************************************************/

#define	TxCSR_prw_Mode					(1 << 13)
/**************************************************
*	The CPU sets this bit to enable the endpoint direction as Tx, and clears it to enable the endpoint
*	direction as Rx. 
*	Note: This bit only has any efect where the same endpoint FIFO is used for both Tx and Rx transactions.
**************************************************/

#define	TxCSR_prw_DMAReqEnab			(1 << 12)
/**************************************************
*	The CPU sets this bit to enable the DMA request for the Tx endpoint.
**************************************************/

#define	TxCSR_prw_FrcDataTog			(1 << 11)
/**************************************************
*	The CPU sets this bit to force the endpoint data toggle to switch and the data packet to be cleared
*	from the FIFO, regardless of whether an ACK was received. This can be used by Interrupt Tx
*	endpoints that are used to communicate rate feedback for Isochronous endpoints.
**************************************************/

#define	TxCSR_prw_DMAReqMode			(1 << 10)
/**************************************************
*	The CPU sets this bit to select DMA Mode 1 and clears this bit to select DMA Mode 0.
*	Note: This bit must not be cleared either before or in the same cycle as the above DMAReqEnab bit is cleared.
**************************************************/

#define	TxCSR_prc_IncompTx				(1 << 7)
/**************************************************
*	When the endpoint is being used for high-bandwidth Isochronous/Interrupt transfers, this bit is
*	set to indicate where a large packet has been split into 2 or 3 packets for transmission but
*	insufficient IN tokens have been received to send all the parts. 
*	Note: In anything other than a high-bandwidth transfer, this bit wil always return 0.
**************************************************/

#define	TxCSR_ps_ClrDataTog				(1 << 6)
/**************************************************
*	The CPU writes a 1 to this bit to reset the endpoint data toggle to 0.
**************************************************/

#define	TxCSR_prc_SentStall				(1 << 5)
/**************************************************
*	This bit is set when a STALL handshake is transmitted. The FIFO is flushed and the TxPktRdy bit
*	is cleared (see below). The CPU should clear this bit.
**************************************************/

#define	TxCSR_prw_SendStall				(1 << 4)
/**************************************************
*	The CPU writes a 1 to this bit to issue a STALL handshake to an IN token. The CPU clears this
*	bit to terminate the stall condition. 
*	Note:: (i) The FIFO should be flushed before SendStall is set. (ii) This bit
*	has no efect where the endpoint is being used for Isochronous transfers.
**************************************************/

#define	TxCSR_ps_FlushFIFO				(1 << 3)
/**************************************************
*	The CPU writes a 1 to this bit to flush the latest packet from the endpoint Tx FIFO. The FIFO pointer
*	is reset, the TxPktRdy bit (below) is cleared and an interrupt is generated. May be set simultaneously
*	with TxPktRdy to abort the packet that is currently being loaded into the FIFO. 
*	Note: FlushFIFO should only be used when TxPktRdy is set. At other times, it may cause data to be corrupted. Also note
*	that, if the FIFO is double-buffered, FlushFIFO may need to be set twice to completely clear the FIFO.
**************************************************/

#define	TxCSR_prc_UnderRun				(1 << 2)
/**************************************************
*	The USB sets this bit if an IN token is received when the TxPktRdy bit not set. The CPU should
*	clear this bit.
**************************************************/

#define	TxCSR_prc_FIFONotEmpty			(1 << 1)
/**************************************************
*	The USB sets this bit when there is at least 1 packet in the Tx FIFO.
**************************************************/

#define	TxCSR_prs_TxPktRdy				(1 << 0)
/**************************************************
*	The CPU sets this bit after loading a data packet into the FIFO. It is cleared automatically when a data
*	packet has been transmitted. An interrupt is also generated at this point (if enabled). TxPktRdy is also
*	automatically cleared prior to loading a second packet into a double-buffered FIFO.
**************************************************/


#define	TxCSR_hrw_AutoSet				(1 << 15)
/**************************************************
*	If the CPU sets this bit, TxPktRdy will be automatically set when data of the maximum packet size
*	(value in TxMaxP) is loaded into the Tx FIFO. If a packet of less than the maximum packet size is
*	loaded, then TxPktRdy will have to be set manually. 
*	Note: Should not be set for either high-bandwidth I sochronous endpoints or high-bandwidth Interrupt endpoints.
**************************************************/

#define	TxCSR_hrw_Mode					(1 << 13)
/**************************************************
*	The CPU sets this bit to enable the endpoint direction as Tx, and clears it to enable the endpoint direction
*	as Rx. 
*	Note: This bit only has any efect where the same endpoint FIFO is used for both Tx and Rx transactions.
**************************************************/

#define	TxCSR_hrw_DMAReqEnab			(1 << 12)
/**************************************************
*	The CPU sets this bit to enable the DMA request for the Tx endpoint.
**************************************************/

#define	TxCSR_hrw_FrcDataTog			(1 << 11)
/**************************************************
*	The CPU sets this bit to force the endpoint data toggle to switch and the data packet to be cleared
*	from the FIFO, regardless of whether an ACK was received. This can be used by Interrupt Tx
*	endpoints that are used to communicate rate feedback for Isochronous endpoints.
**************************************************/

#define	TxCSR_hrw_DMAReqMode			(1 << 10)
/**************************************************
*	The CPU sets this bit to select DMA Mode 1 and clears this bit to select DMA Mode 0.
*	Note: This bit must not be cleared either before or in the same cycle as the above DMAReqEnab bit is cleared.
**************************************************/

#define	TxCSR_hrc_NAKTimeout_IncompTx	(1 << 7)
/**************************************************
*	Bulk endpoints only: This bit will be set when the Tx endpoint is halted following the receipt of
*	NAK responses for longer than the time set as the NAK Limit by the TxInterval register. The
*	CPU should clear this bit to allow the endpoint to continue.
*
*	High-bandwidth Interrupt endpoints only: This bit will be set if no response is received from the device to
*	which the packet is being sent.
**************************************************/

#define	TxCSR_hs_ClrDataTog				(1 << 6)
/**************************************************
*	The CPU writes a 1 to this bit to reset the endpoint data toggle to 0.
**************************************************/

#define	TxCSR_hrc_RxStall				(1 << 5)
/**************************************************
*	This bit is set when a STALL handshake is received. When this bit is set, any DMA request that is
*	in progress is stopped, the FIFO is completely flushed and the TxPktRdy bit is cleared (see below).
*	The CPU should clear this bit.
**************************************************/

#define	TxCSR_hs_FlushFIFO				(1 << 3)
/**************************************************
*	The CPU writes a 1 to this bit to flush the latest packet from the endpoint Tx FIFO. The FIFO pointer
*	is reset, the TxPktRdy bit (below) is cleared and an interrupt is generated. May be set simultaneously
*	with TxPktRdy to abort the packet that is currently being loaded into the FIFO. 
*	Note: FlushFIFO should only be used when TxPktRdy is set. At other times, it may cause data to be corrupted. Also note
*	that, if the FIFO is double-buffered, FlushFIFO may need to be set twice to completely clear the FIFO.
**************************************************/

#define	TxCSR_hrc_Error					(1 << 2)
/**************************************************
*	The USB sets this bit when 3 attempts have been made to send a packet and no handshake packet has
*	been received. When the bit is set, an interrupt is generated, TxPktRdy is cleared and the FIFO
*	completely flushed. The CPU should clear this bit. Valid only when the endpoint is operating in Bulk or
*	Interrupt mode.
**************************************************/

#define	TxCSR_hrc_FIFONotEmpty			(1 << 1)
/**************************************************
*	The USB sets this bit when there is at least 1 packet in the Tx FIFO.
**************************************************/

#define	TxCSR_hrs_TxPktRdy				(1 << 0)
/**************************************************
*	The CPU sets this bit after loading a data packet into the FIFO. It is cleared automatically when a data
*	packet has been transmitted. An interrupt is also generated at this point (if enabled). TxPktRdy is also
*	automatically cleared prior to loading a second packet into a double-buffered FIFO.
**************************************************/





#define	RxCSR_prw_AutoClear				(1 << 15)
/**************************************************
*	If the CPU sets this bit then the RxPktRdy bit will be automatically cleared when a packet of
*	RxMaxP bytes has been unloaded from the Rx FIFO. When packets of less than the maximum
*	packet size are unloaded, RxPktRdy will have to be cleared manually. 
*	Note: Should not be set for high-bandwidth Isochronous endpoints.
**************************************************/

#define	RxCSR_prw_ISO					(1 << 14)
/**************************************************
*	The CPU sets this bit to enable the Rx endpoint for Isochronous transfers, and clears it to enable
*	the Rx endpoint for Bulk/Interrupt transfers.
**************************************************/

#define	RxCSR_prw_DMAReqEnab			(1 << 13)
/**************************************************
*	The CPU sets this bit to enable the DMA request for the Rx endpoint.
**************************************************/

#define	RxCSR_prw_DisNyet_pr_PIDError	(1 << 12)
/**************************************************
*	Bulk/Interrupt Transactions: The CPU sets this bit to disable the sending of NYET handshakes.
*	When set, all successfully received Rx packets are ACK'd including at the point at which the FIFO
*	becomes full. Note: This bit only has any efect in High-speed mode, in which mode it should be set for al
*	Interrupt endpoints.
*
*	ISO Transactions: The core sets this bit to indicate a PID error in the received packet.
**************************************************/

#define	RxCSR_prw_DMAReqMode			(1 << 11)
/**************************************************
*	The CPU sets this bit to select DMA Mode 1 and clears this bit to select DMA Mode 0.
*	Note: This bit should not be cleared in the same cycle as RxPktRdy is cleared.
**************************************************/

#define	RxCSR_prc_IncompRx				(1 << 8)
/**************************************************
*	This bit is set in a high-bandwidth Isochronous/Interrupt transfer if the packet in the Rx FIFO is
*	incomplete because parts of the data were not received. It is cleared when RxPktRdy is cleared. 
*	Note: In anything other than a high-bandwidth transfer, this bit wil always return 0.
**************************************************/

#define	RxCSR_ps_ClrDataTog				(1 << 7)
/**************************************************
*	The CPU writes a 1 to this bit to reset the endpoint data toggle to 0.
**************************************************/

#define	RxCSR_prc_SentStall				(1 << 6)
/**************************************************
*	This bit is set when a STALL handshake is transmitted. The CPU should clear this bit.
**************************************************/

#define	RxCSR_prw_SendStall				(1 << 5)
/**************************************************
*	The CPU writes a 1 to this bit to issue a STALL handshake. The CPU clears this bit to terminate
*	the stall condition. 
*	Note:: (i) The FIFO should be flushed before SendStal is set. 
*			(ii) This bit has no efect where the endpoint is being used for Isochronous tran sfers.
**************************************************/

#define	RxCSR_ps_FlushFIFO				(1 << 4)
/**************************************************
*	The CPU writes a 1 to this bit to flush the next packet to be read from the endpoint Rx FIFO.
*	The FIFO pointer is reset and the RxPktRdy bit (below) is cleared. 
*	Note: FlushFIFO should only be used when RxPktRdy is set. At other times, it may cause data to be corrupted. Also note that, if the
*	FIFO is double-buffered, FlushFIFO may need to be set twice to completely clear the FIFO..
**************************************************/

#define	RxCSR_pr_DataError				(1 << 3)
/**************************************************
*	This bit is set when RxPktRdy is set if the data packet has a CRC or bit-stuff error. It is cleared
*	when RxPktRdy is cleared. Note: This bit is only valid when the endpoint is operating in ISO mode. In Bulk
*	mode, it always returns zero.
**************************************************/

#define	RxCSR_prc_OverRun				(1 << 2)
/**************************************************
*	This bit is set if an OUT packet cannot be loaded into the Rx FIFO. The CPU should clear this bit.
*	Note: This bit is only valid when the endpoint is operating in ISO mode. In Bulk mode, it always returns zero.
**************************************************/

#define	RxCSR_pr_FIFOFull				(1 << 1)
/**************************************************
*	This bit is set when no more packets can be loaded into the Rx FIFO.
**************************************************/

#define	RxCSR_prc_RxPktRdy				(1 << 0)
/**************************************************
*	This bit is set when a data packet has been received. The CPU should clear this bit when the packet
*	has been unloaded from the Rx FIFO. An interrupt is generated when the bit is set.
**************************************************/


#define	RxCSR_hrw_AutoClear				(1 << 15)
/**************************************************
*	If the CPU sets this bit then the RxPktRdy bit will be automatically cleared when a packet of
*	RxMaxP bytes has been unloaded from the Rx FIFO. When packets of less than the maximum
*	packet size are unloaded, RxPktRdy will have to be cleared manually. 
*	Note: Should not be set for high-bandwidth Isochronous endpoints.
**************************************************/

#define	RxCSR_hrw_AutoReq				(1 << 14)
/**************************************************
*	If the CPU sets this bit, the ReqPkt bit will be automatically set when the RxPktRdy bit is cleared.
*	Note: This bit is automatically cleared when a short packet is received.
**************************************************/

#define	RxCSR_hrw_DMAReqEnab			(1 << 13)
/**************************************************
*	The CPU sets this bit to enable the DMA request for the Rx endpoint.
**************************************************/

#define	RxCSR_hr_PIDError				(1 << 12)
/**************************************************
*	ISO Transactions Only: The core sets this bit to indicate a PID error in the received packet.
*	Bulk/Interrupt Transactions: The setting of this bit is ignored.
**************************************************/

#define	RxCSR_hrw_DMAReqMode			(1 << 11)
/**************************************************
*	The CPU sets this bit to select DMA Mode 1 and clears this bit to select DMA Mode 0. 
*	Note: This bit should not be cleared in the same cycle as RxPktRdy is cleared.
**************************************************/

#define	RxCSR_hrc_IncompRx				(1 << 8)
/**************************************************
*	This bit will be set in a high-bandwidth Isochronous/Interrupt transfer if the packet received is
*	incomplete. It will be cleared when RxPktRdy is cleared. 
*	Note: If USB protocols are folowed correctly, this bit should never be set. 
*	The bit becoming set indicates a failure of the associated Peripheral device to behave correctly. 
*	(In anything other than a high-bandwidth transfer, this bit wil always return 0.)
**************************************************/

#define	RxCSR_hs_ClrDataTog				(1 << 7)
/**************************************************
*	The CPU writes a 1 to this bit to reset the endpoint data toggle to 0.
**************************************************/

#define	RxCSR_hrc_RxStall				(1 << 6)
/**************************************************
*	The CPU writes a 1 to this bit to reset the endpoint data toggle to 0.
**************************************************/

#define	RxCSR_hrw_ReqPkt				(1 << 5)
/**************************************************
*	The CPU writes a 1 to this bit to request an IN transaction. It is cleared when RxPktRdy is set.
**************************************************/

#define	RxCSR_hs_FlushFIFO				(1 << 4)
/**************************************************
*	The CPU writes a 1 to this bit to flush the next packet to be read from the endpoint Rx FIFO.
*	The FIFO pointer is reset and the RxPktRdy bit (below) is cleared. 
*	Note: FlushFIFO should only be used when RxPktRdy is set. At other times, it may cause data to be corrupted. 
*	Also note that, if the FIFO is double-buffered, FlushFIFO may need to be set twice to completely clear the FIFO.
**************************************************/

#define	RxCSR_hrc_DataError_NAKTimeout	(1 << 3)
/**************************************************
*	When operating in ISO mode, this bit is set when RxPktRdy is set if the data packet has a CRC or
*	bit-stuff error and cleared when RxPktRdy is cleared. In Bulk mode, this bit will be set when the Rx
*	endpoint is halted following the receipt of NAK responses for longer than the time set as the NAK
*	Limit by the RxInterval register. The CPU should clear this bit to allow the endpoint to continue.
**************************************************/

#define	RxCSR_hrc_Error					(1 << 2)
/**************************************************
*	The USB sets this bit when 3 attempts have been made to receive a packet and no data packet has
*	been received. The CPU should clear this bit. An interrupt is generated when the bit is set. Note: This
*	bit is only valid when the Tx endpoint is operating in Bulk or Interrupt mode. In ISO mode, it always returns zero.
**************************************************/

#define	RxCSR_hr_FIFOFull				(1 << 1)
/**************************************************
*	This bit is set when no more packets can be loaded into the Rx FIFO.
**************************************************/

#define	RxCSR_hrc_RxPktRdy				(1 << 0)
/**************************************************
*	This bit is set when a data packet has been received. The CPU should clear this bit when the packet
*	has been unloaded from the Rx FIFO. An interrupt is generated when the bit is set.
**************************************************/




#define	TxRxType_rw_Protocol				(3 << 4)
#define	TxRxType_Protocol_Illegal			(0 << 4)
#define	TxRxType_Protocol_Isochronous		(1 << 4)
#define	TxRxType_Protocol_Bulk				(2 << 4)
#define	TxRxType_Protocol_Interrupt			(3 << 4) 
/**************************************************
*	The CPU should set this to select the required protocol for the Tx/Rx endpoint:
*	00: Illegal
*	01: Isochronous
*	10: Bulk
*	11: Interrupt
**************************************************/	

#define	TxType_rw_TargetEndpointNumber	(15 << 0)
/**************************************************
*	The CPU should set this value to the endpoint number contained in the Tx endpoint descriptor
*	returned to the during device enumeration.
**************************************************/	




//#define	RxType_rw_Protocol				(3 << 4)
//#define	RxType_Protocol_Illegal			(0 << 4)
//#define	RxType_Protocol_Isochronous		(1 << 4)
//#define	RxType_Protocol_Bulk			(2 << 4)
//#define	RxType_Protocol_Interrupt		(3 << 4) 
/**************************************************
*	The CPU should set this to select the required protocol for the Rx endpoint:
*	00: Illegal
*	01: Isochronous
*	10: Bulk
*	11: Interrupt
**************************************************/	

#define	RxType_rw_TargetEndpointNumber	(15 << 0)
/**************************************************
*	The CPU should set this value to the endpoint number contained in the Rx endpoint descriptor
*	returned to the during device enumeration.
**************************************************/	



/**************************************************
*	config bit define, see 2.3.3
**************************************************/	
#define	ConfigData_r_MPRxE				(1 << 7)
#define	ConfigData_r_MPTxE				(1 << 6)
#define	ConfigData_r_BigEndian			(1 << 5)
#define	ConfigData_r_HBRxE				(1 << 4)
#define	ConfigData_r_HBTxE				(1 << 3)
#define	ConfigData_r_DynFIFOSizing		(1 << 2)
#define	ConfigData_r_SoftConE			(1 << 1)
#define	ConfigData_r_UTMIDataWidth		(1 << 0)





/* Additional control and FIFO register */
typedef struct _CTRL_FIFO_REG
{
	BYTE	DevCtl;
/**************************************************
*	DevCtl		address:60h, Reset value:8'h80
*	DevCtl is an 8-bit register that is used to select whether the core is operating in Peripheral mode or in Host mode, and for
*	controlling and monitoring the USB VBus line.
**************************************************/
	BYTE	reserved1;
	BYTE	TxFIFOsz;
	BYTE	RxFIFOsz;
	WORD	TxFIFOadd;
	WORD	RxFIFOadd;
	DWORD	VControl_VStatus;		/* don't care */
	WORD	HWVers;
/**************************************************
*	HWVers		address:6Ch, Reset value:Version dependent
*	HWVers register is a 16-bit read-only register that returns information about the version of the RTL from which the core hardware
*	was generated, in particular the RTL version number (vxx.y).
**************************************************/
	BYTE	reserved2;
	BYTE	reserved3;
}CTRL_FIFO_REG;					



#define	DevCtl_r_B_Device				(1 << 7)
/**************************************************
*	This Read-only bit indicates whether the core is operating as the ＆A＊ device or the ＆B＊ device. 0 ?
*	＆A＊ device; 1 ? ＆B＊ device. Only valid while a session is in progr
*	Note: If the core is in Force_Host mode (i.e. a session has been started with Testmode.D7 = 1), this bit
*	will indicate the state of the HOSTDISCON input signal from the PHY.
*	Bit Name Function
**************************************************/

#define	DevCtl_r_FSDev					(1 << 6)
/**************************************************
*	This Read-only bit is set when a full-speed or high-speed device has been detected being connected to
*	the port. (High-speed devices are distinguished from full-speed by checking for high-speed chirps
*	when the device is reset.) Only valid in Host mode.
**************************************************/

#define	DevCtl_r_LSDev					(1 << 5)
/**************************************************
*	This Read-only bit is set when a low-speed device has been detected being connected to the port.
*	Only valid in Host mode.
**************************************************/

#define DevCtl_r_VBus					(3 << 3)
#define	DevCtl_VBus_bSE					(0 << 3)
#define	DevCtl_VBus_aSE_bAV				(1 << 3)
#define	DevCtl_VBus_aAV_bVBus			(2 << 3)
#define	DevCtl_VBus_aVBus				(3 << 3)
/**************************************************
These Read-only bits encode the current VBus level as follows:
*	D4 D3 Meaning
*	0 0 Below SessionEnd
*	0 1 Above SessionEnd, below AValid
*	1 0 Above AValid, below VBusValid
*	1 1 Above VBusValid
**************************************************/

#define	DevCtl_r_HostMode				(1 << 2)
/**************************************************
*	This Read-only bit is set when the core is acting as a Host.
**************************************************/

#define	DevCtl_rw_HostReq				(1 << 1)
/**************************************************
*	When set, the core will initiate the Host Negotiation when Suspend mode is entered. It is
*	cleared when Host Negotiation is completed. See Section 15. (＆B＊ device onl
**************************************************/

#define	DevCtl_rw_Session				(1 << 0)
/**************************************************
*	When operating as an ＆A＊ device, this bit is set or cleared by the CPU to start or end a session.
*	When operating as a ＆B＊ device, this bit is set/cleared by the core when a session starts/ends.
*	It is also set by the CPU to initiate the Session Request Protocol, or cleared by the CPU when in
*	Suspend mode to perform a software disconnect. Note: Clearing this bit when the core is not
*	suspended will result in undefined behavior.
**************************************************/




#define	TxFIFOsz_rw_DPB					(1 << 4)
#define	RxFIFOsz_rw_DPB					(1 << 4)
/**************************************************
*	Defines whether double-packet buffering supported. When ＆1＊, double-packet buffering is
*	supported. When ＆0＊, only single-packet buffering is supporte
**************************************************/


/* ULPI register */
typedef struct _ULPI_REG
{
	BYTE	CarKitCtrl;
	BYTE	VBusCtrl;
	BYTE	IntSrc;
	BYTE	IntMask;
	BYTE	RegAddr;
	BYTE	RegData;
	BYTE	RawData;
	BYTE	RegCtrl;
}ULPI_REG;


/* Additional config register */
typedef struct _CONFIG_REG
{
	BYTE	EPInfo;
/**************************************************
*	EPInfo		address:78h, Reset value:Implementation dependent
*	This 8-bit read-only register allows read-back of the number of Tx and Rx endpoints included in the design.
**************************************************/
	BYTE	RAMInfo;
/**************************************************
*	RAMInfo		address:79h, Reset value:Implementation dependent
*	This 8-bit read-only register provides information about the width of the RAM and the number of DMA channels.
**************************************************/
	BYTE	LinkInfo;
/**************************************************
*	LinkInfo		address:7Ah, Reset value:8'h5C
*	This 8-bit register allows some delays to be specified.
**************************************************/
	BYTE	VPLen;
/**************************************************
*	VPLen		address:7Bh, Reset value:8'h3C
*	This 8-bit register sets the duration of the VBus pulsing charge.
**************************************************/
	BYTE	HS_EOF;
/**************************************************
*	HS_EOF		address:7Ch, Reset value:8'h80
*	This 8-bit register sets the minimum time gap that is to be allowed between the start of the last transaction and the EOF for
*	High-speed transactions.
**************************************************/
	BYTE	FS_EOF;
/**************************************************
*	FS_EOF		address:7Dh, Reset value:8'h77
*	This 8-bit register sets the minimum time gap that is to be allowed between the start of the last transaction and the EOF for
*	Full-speed transactions.
**************************************************/
	BYTE	LS_EOF;
/**************************************************
*	LS_EOF		address:7Eh, Reset value:8'h72
*	This 8-bit register sets the minimum time gap that is to be allowed between the start of the last transaction and the EOF for
*	Low-speed transactions.
**************************************************/
	BYTE	reserved;
}CONFIG_REG; 


/*
 * DMA control register
 */
typedef struct _DMA_CTRL
{
	DWORD	Intr;
	DWORD	Ctrl;
	DWORD	Addr;
	DWORD	Count;
}DMA_CTRL;


typedef enum _OTG_ERROR
{
	OTG_NO_ERR = 0,
	RX_STALL,
	ERROR,
	NAK
	
}OTG_ERROR;

 /* OTG register map, OTG spec page 8 */
//#define	SYS_OTG_REG_BASS				0x9C00	
/* define OTG register map for debug */
typedef struct _OTG_REG
{
	COMMON_USB_REG 	CommonUSB;
	CSR_REG 		IndexedCSR;
	DWORD			EndpointFIFO[16];
	CTRL_FIFO_REG 	CtrlFIFO;
	ULPI_REG		ULPI;
	CONFIG_REG 		Config;
	BYTE			reserved1[128];
	CSR_REG			CSR[16];
	DMA_CTRL		DMAChl;
	BYTE			reserved2[240];			/* reserved for DMA channel */
	DWORD			RqPktCount[16];		/* RqPktCount[0] do not care for endpoint 0 */
}OTG_REG;


/*--------------------------------------------------------------------------------------------*/

/*
 *	FAddr			address:00h, Reset value:8'h00 
 *	FAddr is an 8-bit register that should be written with the 7-bit address of the peripheral part of the transaction.
 *	When the core is being used in Host mode (DevCtl.D2=1), this register should be set to the value sent in a SET _ADDRESS
 *	command during device enumeration as the address for the peripheral device.
 *	When the core is being used in Peripheral mode (DevCtl.D2=0), this register should be written with the address received through a
 *	SET_ADDRESS command, which will then be used for decoding the function address in subsequent token packets. Note: The new
 *	address will not take effect immediately as the host will still be using the old address for the Status stage of the device request. The
 *	core will continue to use the old address for decoding packets until the device request has completed.
 */
#define	otg_CommonUSB_FAddr			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x00))

/*
 *	Power			address:01h, Reset value:8'h20 
 *	Power is an 8-bit register that is used for controlling Suspend and Resume signaling, 
 *	and some basic operational aspects of the core.
 */
#define	otg_CommonUSB_Power			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x01))

/*
 *	IntrTx			address:02h, Reset value:16'h0000 
 *	IntrTx is a 16-bit read-only register that indicates which interrupts are currently active for Endpoint 0 and the Tx Endpoints 1每15.
 *	Note: Bits relating to endpoints that have not been configured will always return 0. Note also that all active interrupts are cleared
 *	when this register is read.
 */
#define	otg_CommonUSB_IntrTx		((volatile WORD XDATA *)(SYS_OTG_REG_BASS + 0x02))

/*
 *	IntrRx			address:04h, Reset value:16'h0000 
 *	IntrRx is an 16-bit read-only register that indicates which of the interrupts for Rx Endpoints 1 每 15 are currently active. Note: Bits
 *	relating to endpoints that have not been configured will always return 0. Note also that all active interrupts are cleared when this
 *	register is read.
 */
#define	otg_CommonUSB_IntrRx		((volatile WORD XDATA *)(SYS_OTG_REG_BASS + 0x04))

/*
 *	IntrTxE			address:06h, Reset value:16'hFFFF masked with the Tx endpoints implemented
 *	IntrTxE is a 16-bit register that provides interrupt enable bits for the interrupts in IntrTx. Where a bit is set to 1, MC_NINT will
 *	be asserted on the corresponding interrupt in the IntrTx register becoming set. Where a bit is set to 0, the interrupt in IntrTx is still set
 *	but MC_NINT is not asserted. On reset, the bits corresponding to Endpoint 0 and the Tx endpoints included in the design are set to
 *	1, while the remaining bits are set to 0. Note: Bits relating to endpoints that have not been configured will always return 0.
 */
#define	otg_CommonUSB_IntrTxEn		((volatile WORD XDATA *)(SYS_OTG_REG_BASS + 0x06))

/*
 *	IntrRxE			address:08h, Reset value:16'hFFFE masked with the Rx endpoints implemented
 *	IntrRxE is a 16-bit register that provides interrupt enable bits for the interrupts in IntrRx. Where a bit is set to 1, MC_NINT will
 *	be asserted on the corresponding interrupt in the IntrRx register becoming set. Where a bit is set to 0, the interrupt in IntrTx is still set
 *	but MC_NINT is not asserted. On reset, the bits corresponding to the Rx endpoints included in the design are set to 1, while the
 *	remaining bits are set to 0. Note: Bits relating to endpoints that have not been configured will always return 0.
 */
#define	otg_CommonUSB_IntrRxEn		((volatile WORD XDATA *)(SYS_OTG_REG_BASS + 0x08))

/*
 *	IntrUSB			address:0Ah, Reset value:8'h00
 *	IntrUSB is an 8-bit read-only register that indicates which USB interrupts are currently active. 
 *	All active interrupts will be cleared when this register is read.
 */
#define	otg_CommonUSB_IntrUSB		((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x0A))

/*
 *	IntrUSBE		address:0Bh, Reset value:8'h06
 *	IntrUSBE is an 8-bit register that provides interrupt enable bits for each of the interrupts in IntrUSB.
 */	
#define	otg_CommonUSB_IntrUSBE		((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x0B))

/*
 *	Frame		address:0Ch, Reset value:16'h0000
 *	Frame is a 16-bit register that holds the last received frame number.
 */	
#define	otg_CommonUSB_Frame			((volatile WORD XDATA *)(SYS_OTG_REG_BASS + 0x0C))

/*
 *	Index		address:0Eh, Reset value:8'h00
 *	Each Tx endpoint and each Rx endpoint have their own set of control/status registers located between 100h 每 1FFh. In addition one
 *	set of Tx control/status and one set of Rx control/status registers appear at 10h 每 19h. Index is a 4-bit register that
 *	determines which endpoint control/status registers are accessed.
 */	
#define	otg_CommonUSB_Index			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x0E))

/* 
 *	TestMode	address:0Fh, Reset value:8'h00
 *	Testmode is an 8-bit register that is primarily used to put the core into one of the four test modes for High-speed operation
 *	described in the USB 2.0 specification 每 in response to a SET FEATURE: TESTMODE command. It is not used in norma
 *	operation.
 */
#define	otg_CommonUSB_TestMode		((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x0F))


/*-------------------------------------------------------------------------------------------------*/

/*
 *	TxMaxP		address:10h, Reset value:16'h0000
 *	The TxMaxP register defines the maximum amount of data that can be transferred through the selected Tx endpoint in a single
 *	operation. There is a TxMaxP register for each Tx endpoint (except Endpoint 0).
 *
 *	Bits 10:0 define (in bytes) the maximum payload transmitted in a single transaction. The value set can be up to 1024 bytes but is
 *	subject to the constraints placed by the USB Specification on packet sizes for Bulk, Interrupt and Isochronous transfers in Fullspeed
 *	and High-speed operations.
 *
 *	Where the option of High-bandwidth Isochronous/Interrupt endpoints or of packet splitting on Bulk endpoints has been taken
 *	when the core is configured, the register includes either 2 or 5 further bits that define a multiplier m which is equal to one more than
 *	the value recorded.
 */
#define	otg_IndexedCSR_TxMaxP		((volatile WORD XDATA *)(SYS_OTG_REG_BASS + 0x10))

/*
 *	CSR0		address:12h, Reset value:16'h0000
 *	CSR0 is a 16-bit register that provides control and status bits for Endpoint 0. Note: The interpretation of the register depends on
 *	whether the core is acting as a peripheral or as a host. Users should also be aware that the value returned when the register is
 *	read reflects the status attained e.g. as a result of writing to the register.
 */
#define	otg_IndexedCSR_CSR0			((volatile WORD XDATA *)(SYS_OTG_REG_BASS + 0x12))

/*
 *	TxCSR		address:12h, Reset value:16'h0000
 *	TxCSR is a 16-bit register that provides control and status bits for transfers through the currently-selected Tx endpoint. There is a
 *	TxCSR register for each configured Tx endpoint (not including Endpoint 0). 
 *	Note: The interpretation of the register depends on whether the core is acting as a peripheral or as a host. 
 *	Users should also be aware that the value returned when the register is read reflects the status attained e.g. 
 *	as a result of writing to the register.
 */
#define	otg_IndexedCSR_TxCSR		((volatile WORD XDATA *)(SYS_OTG_REG_BASS + 0x12))

/*
 *	RxMaxP		address:14h, Reset value:16'h0000
 *	The RxMaxP register defines the maximum amount of data that can be transferred through the selected Rx endpoint in a single
 *	operation. There is a RxMaxP register for each Rx endpoint (except Endpoint 0).
 *
 *	Bits 10:0 define (in bytes) the maximum payload transmitted in a single transaction. The value set can be up to 1024 bytes but is
 *	subject to the constraints placed by the USB Specification on packet sizes for Bulk, Interrupt and Isochronous transfers in Fullspeed
 *	and High-speed operations.
 *
 *	Where the option of High-bandwidth Isochronous/Interrupt endpoints or of combini ng Bulk packets has been taken when the core is
 *	configured, the register includes either 2 or 5 further bits that define a multiplier m which is equal to one more than the value recorded.
 */
#define	otg_IndexedCSR_RxMaxP		((volatile WORD XDATA *)(SYS_OTG_REG_BASS + 0x14))

/*
 *	RxCSR		address:16h, Reset value:16'h0000
 *	RxCSR is an 16-bit register that provides control and status bits for transfers through the currently-selected Rx endpoint. There is an
 *	RxCSR register for each configured Rx endpoint (not including Endpoint 0). 
 *	Note: The interpretation of the register depends on whether the core is acting as a peripheral or as a host. 
 *	Users should also be aware that the value returned when the register is read reflects the status attained e.g. as a result of writing to the register.
 */
#define	otg_IndexedCSR_RxCSR		((volatile WORD XDATA *)(SYS_OTG_REG_BASS + 0x16))

/*
 *	Count0		address:18h, Reset value:7'b0000000
 *	Count0 is a 7-bit read-only register that indicates the number of received data bytes in the Endpoint 0 FIFO. The value returned
 *	changes as the contents of the FIFO change and is only valid while RxPktRdy (CSR0.D0) is set.
 */
#define	otg_IndexedCSR_Count0		((volatile WORD XDATA *)(SYS_OTG_REG_BASS + 0x18))

/*
 *	RxCount		address:18h, Reset value:13'b0000000000000
 *	CRxCount is a 13-bit read-only register that holds the number of received data bytes in the packet currently in line to be read from the
 *	Rx FIFO. If the packet was transmitted as multiple bulk packets, the number given will be for the combined packet. Note: The value
 *	returned changes as the FIFO is unloaded and is only valid while RxPktRdy (RxCSR.D0) is set.
 */
#define	otg_IndexedCSR_RxCount		((volatile WORD XDATA *)(SYS_OTG_REG_BASS + 0x18))

/*
 *	TxType		address:1Ah, Reset value:8'h00(host mode only)
 *	TxType is a 6-bit register that should be written with the endpoint number to be targeted by the endpoint in the lower 4 bits, and
 *	the transaction protocol to use for the currently-selected Tx endpoint in the upper 2 bits.
 *	There is a TxType register for each configured Tx endpoint (except Endpoint 0).
 */
#define	otg_IndexedCSR_host_TxType		((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x1A))

/*
 *	NAKLimit0		address:1Bh, Reset value:5'b00000(host mode only)
 *	NAKLimit0 is a 5-bit register that sets the number of frames/microframes (High-Speed transfers) after which Endpoint 0 should
 *	timeout on receiving a stream of NAK responses. (Equivalent settings for other endpoints can be made through their TxInterval
 *	and RxInterval registers: see Sections 2.3.11 and 2.3.13.)
 *	The number of frames/microframes selected is 2(m-1) (where m is the value set in the register, valid values 2 每 16). If the host receives
 *	NAK responses from the target for more frames than the number represented by the Limit set in this register, the endpoint will be
 *	halted. Note: A value of 0 or 1 disables the NAK timeout function.
 */
#define	otg_IndexedCSR_host_NAKLimit0	((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x1B))

/*
 *	TxInterval		address:1Bh, Reset value:8'b00000000(host mode only)
 *	TxInterval is an 8-bit register that, for Interrupt and Isochronous transfers, defines the polling interval for the currently-selected Tx
 *	endpoint. For Bulk endpoints, this register sets the number of frames/microframes after which the endpoint should timeout on
 *	receiving a stream of NAK responses. There is a TxInterval register for each configured Tx endpoint (except Endpoint 0).
 */
#define	otg_IndexedCSR_host_TxInterval	((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x1B))

/*
 *	RxType		address:1Ch, Reset value:8'h00(host mode only)
 *	RxType is a 6-bit register that should be written with the endpoint number to be targeted by the endpoint in the lower 4 bits, and
 *	the transaction protocol to use for the currently-selected Rx endpoint in the upper 2 bits.
 *	There is an RxType register for each configured Rx endpoint (except Endpoint 0).
 */
#define	otg_IndexedCSR_host_RxType		((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x1C))

/*
 *	RxInterval		address:1Dh, Reset value:8'b00000000(host mode only)
 *	RxInterval is an 8-bit register that, for Interrupt and Isochronous transfers, defines the polling interval for the currently-selected Rx
 *	endpoint. For Bulk endpoints, this register sets the number of frames/microframes after which the endpoint should timeout on
 *	receiving a stream of NAK responses. There is a RxInterval register for each configured Rx endpoint (except Endpoint 0).
 */
#define	otg_IndexedCSR_host_RxInterval	((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x1D))

/*
 *	ConfigData		address:1Fh, Reset value:Configuration Dependent
 *	ConfigData is an 8-bit Read-Only register that returns information about the selected core configuration.
 */
#define	otg_IndexedCSR_dev_ConfigData	((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x1F))

/*
 *	FIFOSize		address:1Fh, Reset value:Configuration Dependent
 *	FIFOSize is an 8-bit Read-Only register that returns the sizes of the FIFOs associated with the selected additional Tx/Rx endpoints.
 *	The lower nibble encodes the size of the selected Tx endpoint FIFO; the upper nibble encodes the size of the selected Rx endpoint
 *	FIFO. Values of 3 每 13 correspond to a FIFO size of 2n bytes (8 每 8192 bytes). If an endpoint has not been configured, a value of 0
 *	will be displayed. Where the Tx and Rx endpoints share the same FIFO, the Rx FIFO size will be encoded as 0xF.
 *	Note: The register only has this interpretation when the Index register is set to select one of Endpoints 1 每 15 and Dynamic Sizing
 *	is not selected. It has a special interpretation when the Index register is set to select Endpoint 0 (see Section 2.3.3), while the result
 *	returned is not valid where Dynamic FIFO sizing is used.
 */
#define	otg_IndexedCSR_dev_FIFOSize		((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x1F))


/*------------------------------------------------------------------------------------------------*/


/*
 *	EndpointFIFO		address:20h
 *	FIFOs for Endpoints 0 每 1
 */
#define	otg_EndpointFIFO			((volatile DWORD XDATA *)(SYS_OTG_REG_BASS + 0x20))


/*-------------------------------------------------------------------------------------------------*/

/*
 *	DevCtl		address:60h, Reset value:8'h80
 *	DevCtl is an 8-bit register that is used to select whether the core is operating in Peripheral mode or in Host mode, and for
 *	controlling and monitoring the USB VBus line.
 */
#define	otg_CtrlFIFO_DevCtl			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x60))


#define	otg_CtrlFIFO_TxFIFOSz		((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x62))
#define	otg_CtrlFIFO_RxFIFOSz		((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x63))
#define	otg_CtrlFIFO_TxFIFOadd		((volatile WORD XDATA *)(SYS_OTG_REG_BASS + 0x64))
#define	otg_CtrlFIFO_RxFIFOadd		((volatile WORD XDATA *)(SYS_OTG_REG_BASS + 0x66))
#define	otg_CtrlFIFO_VControlStatus	((volatile DWORD XDATA *)(SYS_OTG_REG_BASS + 0x68))


/*
 *	HWVers		address:6Ch, Reset value:Version dependent
 *	HWVers register is a 16-bit read-only register that returns information about the version of the RTL from which the core hardware
 *	was generated, in particular the RTL version number (vxx.y).
 */
#define	otg_CtrlFIFO_HWVers			((volatile WORD XDATA *)(SYS_OTG_REG_BASS + 0x6C))


/*--------------------------------------------------------------------------------------------------*/

/*
 * ULPI interface register map
 * do not use in this system
 */

#define	otg_ULPI_CarKitCtrl			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x70))
#define	otg_ULPI_VBusCtrl			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x71))
#define	otg_ULPI_IntSrc				((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x72))
#define	otg_ULPI_IntMask			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x73))
#define	otg_ULPI_RegAddr			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x74))
#define	otg_ULPI_RegData			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x75))
#define	otg_ULPI_RawData			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x76))
#define	otg_ULPI_RegCtrl			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x77))


/*---------------------------------------------------------------------------------------------------*/

/*
 * OTG core configuration register group
 */

/*
 *	EPInfo		address:78h, Reset value:Implementation dependent
 *	This 8-bit read-only register allows read-back of the number of Tx and Rx endpoints included in the design.
 */
#define	otg_Config_EPInfo			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x78))

/*
 *	RAMInfo		address:79h, Reset value:Implementation dependent
 *	This 8-bit read-only register provides information about the width of the RAM and the number of DMA channels.
 */
#define	otg_Config_RAMInfo			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x79))

/*
 *	LinkInfo		address:7Ah, Reset value:8'h5C
 *	This 8-bit register allows some delays to be specified.
 */
#define	otg_Config_LinkInfo			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x7A))

/*
 *	VPLen		address:7Bh, Reset value:8'h3C
 *	This 8-bit register sets the duration of the VBus pulsing charge.
 */
#define	otg_Config_VPLen			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x7B))

/*
 *	HS_EOF		address:7Ch, Reset value:8'h80
 *	This 8-bit register sets the minimum time gap that is to be allowed between the start of the last transaction and the EOF for
 *	High-speed transactions.
 */
#define	otg_Config_HS_EOF			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x7C))

/*
 *	FS_EOF		address:7Dh, Reset value:8'h77
 *	This 8-bit register sets the minimum time gap that is to be allowed between the start of the last transaction and the EOF for
 *	Full-speed transactions.
 */
#define	otg_Config_FS_EOF			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x7D))

/*
 *	LS_EOF		address:7Eh, Reset value:8'h72
 * 	This 8-bit register sets the minimum time gap that is to be allowed between the start of the last transaction and the EOF for
 *	Low-speed transactions.
 */
#define	otg_Config_LS_EOF			((volatile BYTE XDATA *)(SYS_OTG_REG_BASS + 0x7E))


/*----------------------------------------------------------------------------------------*/

/*
 * 	DMA control reisger group
 * 	only use channel 1 register
 */

#define	otg_DMA_Intr				((volatile DWORD XDATA *)(SYS_OTG_REG_BASS + 0x200))
#define	otg_DMA_Ctrl				((volatile DWORD XDATA *)(SYS_OTG_REG_BASS + 0x204))
#define	otg_DMA_Addr				((volatile DWORD XDATA *)(SYS_OTG_REG_BASS + 0x208))
#define	otg_DMA_Count				((volatile DWORD XDATA *)(SYS_OTG_REG_BASS + 0x20C))

/*
 * 	Indicate pending DMA interrupt, one bit per DMA channel implemented.
 * 	D0 for DMA channel 1
 */
#define	DMA_Intr_Ch1				(1 << 0)

/*
 *	DMA channel 1 control register bit mask 
 */
#define	DMA_Ctrl_EnDMA				(1 << 0)
#define	DMA_Ctrl_Direction			(1 << 1)
#define	DMA_Ctrl_Mode				(1 << 2)
#define	DMA_Ctrl_IntrEn				(1 << 3)
#define	DMA_Ctrl_EpNum				(0xF << 4)
#define	DMA_Ctrl_BusErr				(1 << 8)
#define	DMA_Ctrl_BurstMode			(3 << 9)

/*-----------------------------------------------------------------------------------------*/


/*
 *	RqPktCount		address:300h
 *	Number of requested packets for Receive Endpoint n (Endpoints 1 每 15 only)
 *	Note: these register must be 4 bytes width, not 2 bytes width
 * 	the address = 0x300 + 4 * n. (n = endpoint number)
 */
#define	otg_RqPktCount				((volatile DWORD XDATA *)(SYS_OTG_REG_BASS + 0x300))

/*-----------------------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------------------*/

/*
 * define OTG endpoint descriptor, used for configuration endpoint information
 */
typedef struct _OTG_ENDPOINT_DESCRIPTOR
{
	BYTE	FuncAddr;			/* function address */
	BYTE	EndPointNum;		/* endpoint number */
	BYTE	TransferType;		/* endpoint type: ctrl, bulk, interrupt, isochronous */
	BYTE	Direction;			/* endpoint transfer direction: 2 - in, 1 - out, 0 - urb set */
	WORD	MaxPacketSize;		/* max packet size */
	BYTE	HcEpIndex;			/* host controller endpoint index */
	WORD	HcEpBufferSize;		/* host controller endpoint buffer size in bytes */	
}OTG_ENDPOINT_DESCRIPTOR;




extern BOOL
OtgHostOpen(VOID);

extern VOID*
OtgHostOpenPipe(
	DWORD		pipeInfo
	);

extern VOID
OtgHostClosePipe(
	VOID		*pipe
	);

extern VOID
OtgHostSendUrb(VOID);

extern VOID
OtgHostRootHubPortStatus(
	BOOL		*isConnect,			/* output: have device connection set 1, else set 0 */
	BOOL		*isChange			/* 			device connection change set 1, else set 0 */
	);

extern VOID
OtgHostRootHubPortReset(VOID);

extern VOID
OtgHostRootHubClearPortFeature(VOID);

extern WORD
OtgHostGetFrameNumber(VOID);


#endif/* __OTG_H__ */