/****************************************************************
*                      MT View Silicon Tech. Inc.
*
*    Copyright 2007, MT View Silicon Tech. Inc., ShangHai, China
*                    All rights reserved.
*
*
* Filename:      	usb_ch9.h
*
* Programmer:    	Grey
*
* Created: 	 		12/xx/2007
*
* Description: 		
* This file holds USB constants and structures that are needed for USB
* device APIs.  These are used by the USB device model, which is defined
* in chapter 9 of the USB 2.0 specification.  
*
* USB 2.0 adds an additional "On The Go" (OTG) mode, which lets systems
* act either as a USB master/host or as a USB slave/device.  That means
* the master and slave side APIs will benefit from working well together.
*              
*****************************************************************/

#ifndef	__USB_CH9_H__
#define	__USB_CH9_H__

/* CONTROL REQUEST SUPPORT */

/*
 * USB directions
 *
 * This bit flag is used in endpoint descriptors' bEndpointAddress field.
 * It's also one of three fields in control requests bRequestType.
 */
#define USB_DIR_OUT			0					/* host to device */
#define USB_DIR_IN			0x80				/* device to host */

/*
 * USB types, the second of three bRequestType fields
 */
#define USB_TYPE_MASK			(0x03 << 5)
#define USB_TYPE_STANDARD		(0x00 << 5)
#define USB_TYPE_CLASS			(0x01 << 5)
#define USB_TYPE_VENDOR			(0x02 << 5)
#define USB_TYPE_RESERVED		(0x03 << 5)

/*
 * USB recipients, the third of three bRequestType fields
 */
#define USB_RECIP_MASK			0x1F
#define USB_RECIP_DEVICE		0x00
#define USB_RECIP_INTERFACE		0x01
#define USB_RECIP_ENDPOINT		0x02
#define USB_RECIP_OTHER			0x03

/*
 * Standard requests, for the bRequest field of a SETUP packet.
 *
 * These are qualified by the bRequestType field, so that for example
 * TYPE_CLASS or TYPE_VENDOR specific feature flags could be retrieved
 * by a GET_STATUS request.
 */
#define USB_REQ_GET_STATUS				0
#define USB_REQ_CLEAR_FEATURE			1
#define USB_REQ_SET_FEATURE				3
#define USB_REQ_SET_ADDRESS				5
#define USB_REQ_GET_DESCRIPTOR			6
#define USB_REQ_SET_DESCRIPTOR			7
#define USB_REQ_GET_CONFIGURATION		8
#define USB_REQ_SET_CONFIGURATION		9
#define USB_REQ_GET_INTERFACE			10
#define USB_REQ_SET_INTERFACE			11
#define USB_REQ_SYNCH_FRAME				12

/*
 * USB feature flags are written using USB_REQ_{CLEAR,SET}_FEATURE, and
 * are read as a bit array returned by USB_REQ_GET_STATUS.  (So there
 * are at most sixteen features of each type.)
 */ 
#define USB_DEVICE_SELF_POWERED			0	/* (read only) from GET_STATUS request */
#define USB_DEVICE_REMOTE_WAKEUP		1	/* dev may initiate wakeup */
#define USB_DEVICE_TEST_MODE			2	/* (high speed only) */
#define USB_DEVICE_B_HNP_ENABLE			3	/* dev may initiate HNP */
#define USB_DEVICE_A_HNP_SUPPORT		4	/* RH port supports HNP */
#define USB_DEVICE_A_ALT_HNP_SUPPORT	5	/* other RH port does */
#define USB_DEVICE_DEBUG_MODE			6	/* (special devices only) */

#define USB_ENDPOINT_HALT			0	/* IN/OUT will STALL */


/*
 * struct usb_ctrlrequest - SETUP data for a USB device control request
 * @bRequestType: matches the USB bmRequestType field
 * @bRequest: matches the USB bRequest field
 * @wValue: matches the USB wValue field (le16 byte order)
 * @wIndex: matches the USB wIndex field (le16 byte order)
 * @wLength: matches the USB wLength field (le16 byte order)
 *
 * This structure is used to send control requests to a USB device.  It matches
 * the different fields of the USB 2.0 Spec section 9.3, table 9-2.  See the
 * USB spec for a fuller description of the different fields, and what they are
 * used for.
 *
 * Note that the driver for any interface can issue control requests.
 * For most devices, interfaces don't coordinate with each other, so
 * such requests may be made at any time.
 */
typedef struct _USB_REQUEST 
{
	BYTE	bmRequestType;
	BYTE	bRequest;
	WORD	wValue;
	WORD	wIndex;
	WORD	wLength;
}USB_REQUEST;

/*-------------------------------------------------------------------------*/

/*
 * STANDARD DESCRIPTORS ... as returned by GET_DESCRIPTOR, or
 * (rarely) accepted by SET_DESCRIPTOR.
 *
 * Note that all multi-byte values here are encoded in little endian
 * byte order "on the wire".  But when exposed through Linux-USB APIs,
 * they've been converted to cpu byte order.
 */

/*
 * Descriptor types ... USB 2.0 spec table 9.5
 */
#define USB_DT_DEVICE					1
#define USB_DT_CONFIG					2
#define USB_DT_STRING					3
#define USB_DT_INTERFACE				4
#define USB_DT_ENDPOINT					5
#define USB_DT_DEVICE_QUALIFIER			6
#define USB_DT_OTHER_SPEED_CONFIG		7
#define USB_DT_INTERFACE_POWER			8
/* these are from a minor usb 2.0 revision (ECN) */
#define USB_DT_OTG						9
#define USB_DT_DEBUG					10
#define USB_DT_INTERFACE_ASSOCIATION	11

/* conventional codes for class-specific descriptors */
#define USB_DT_CS_DEVICE		0x21
#define USB_DT_CS_CONFIG		0x22
#define USB_DT_CS_STRING		0x23
#define USB_DT_CS_INTERFACE		0x24
#define USB_DT_CS_ENDPOINT		0x25

/* All standard descriptors have these 2 fields at the beginning */
typedef struct _USB_DESCRIPTOR_HEADER 
{
	BYTE  	bLength;
	BYTE  	bDescriptorType;
}USB_DESCRIPTOR_HEADER;


/*-------------------------------------------------------------------------*/

/* USB_DT_DEVICE: Device descriptor */
typedef struct _USB_DEVICE_DESCRIPTOR 
{
	BYTE 	bLength;
	BYTE  	bDescriptorType;
	WORD 	bcdUSB;
	BYTE  	bDeviceClass;
	BYTE  	bDeviceSubClass;
	BYTE  	bDeviceProtocol;
	BYTE  	bMaxPacketSize0;
	WORD	idVendor;
	WORD	idProduct;
	WORD	bcdDevice;
	BYTE  	iManufacturer;
	BYTE  	iProduct;
	BYTE  	iSerialNumber;
	BYTE  	bNumConfigurations;
}USB_DEVICE_DESCRIPTOR;

#define USB_DT_DEVICE_SIZE		18

/*
 * Device and/or Interface Class codes
 * as found in bDeviceClass or bInterfaceClass
 * and defined by www.usb.org documents
 */
#define USB_CLASS_PER_INTERFACE		0	/* for DeviceClass */
#define USB_CLASS_AUDIO				1
#define USB_CLASS_COMM				2
#define USB_CLASS_HID				3
#define USB_CLASS_PHYSICAL			5
#define USB_CLASS_STILL_IMAGE		6
#define USB_CLASS_PRINTER			7
#define USB_CLASS_MASS_STORAGE		8
#define USB_CLASS_HUB				9
#define USB_CLASS_CDC_DATA			10
#define USB_CLASS_CSCID				11	/* chip+ smart card */
#define USB_CLASS_CONTENT_SEC		13	/* content security */
#define USB_CLASS_VIDEO				14
#define USB_CLASS_APP_SPEC			0xFE
#define USB_CLASS_VENDOR_SPEC		0xFF

/*-------------------------------------------------------------------------*/

/* USB_DT_CONFIG: Configuration descriptor information.
 *
 * USB_DT_OTHER_SPEED_CONFIG is the same descriptor, except that the
 * descriptor type is different.  Highspeed-capable devices can look
 * different depending on what speed they're currently running.  Only
 * devices with a USB_DT_DEVICE_QUALIFIER have any OTHER_SPEED_CONFIG
 * descriptors.
 */
typedef struct _USB_CONFIG_DESCRIPTOR 
{
	BYTE  	bLength;
	BYTE  	bDescriptorType;
	WORD  	wTotalLength;
	BYTE  	bNumInterfaces;
	BYTE  	bConfigurationValue;
	BYTE  	iConfiguration;
	BYTE  	bmAttributes;
	BYTE  	bMaxPower;
}USB_CONFIG_DESCRIPTOR;

#define USB_DT_CONFIG_SIZE		9

/* from config descriptor bmAttributes */
#define USB_CONFIG_ATT_ONE			(1 << 7)	/* must be set */
#define USB_CONFIG_ATT_SELFPOWER	(1 << 6)	/* self powered */
#define USB_CONFIG_ATT_WAKEUP		(1 << 5)	/* can wakeup */

/*-------------------------------------------------------------------------*/

/* USB_DT_STRING: String descriptor */
typedef struct _USB_STRING_DESCRIPTOR 
{
	BYTE  	bLength;
	BYTE  	bDescriptorType;

	WORD 	wData[1];		/* UTF-16LE encoded */
}USB_STRING_DESCRIPTOR;

/* note that "string" zero is special, it holds language codes that
 * the device supports, not Unicode characters.
 */

/*-------------------------------------------------------------------------*/

/* USB_DT_INTERFACE: Interface descriptor */
typedef struct _USB_INTERFACE_DESCRIPTOR 
{
	BYTE  	bLength;
	BYTE  	bDescriptorType;
	BYTE  	bInterfaceNumber;
	BYTE  	bAlternateSetting;
	BYTE  	bNumEndpoints;
	BYTE  	bInterfaceClass;
	BYTE  	bInterfaceSubClass;
	BYTE  	bInterfaceProtocol;
	BYTE  	iInterface;
}USB_INTERFACE_DESCRIPTOR;

#define USB_DT_INTERFACE_SIZE		9

/*-------------------------------------------------------------------------*/

/* USB_DT_ENDPOINT: Endpoint descriptor */
typedef struct _USB_ENDPOINT_DESCRIPTOR 
{
	BYTE  	bLength;
	BYTE  	bDescriptorType;
	BYTE  	bEndpointAddress;
	BYTE  	bmAttributes;
	WORD 	wMaxPacketSize;
	BYTE  	bInterval;
	// NOTE:  these two are _only_ in audio endpoints.
	// use USB_DT_ENDPOINT*_SIZE in bLength, not sizeof.
	BYTE  	bRefresh;
	BYTE  	bSynchAddress;
}USB_ENDPOINT_DESCRIPTOR;

#define USB_DT_ENDPOINT_SIZE		7
#define USB_DT_ENDPOINT_AUDIO_SIZE	9	/* Audio extension */


/*
 * Endpoints
 */
#define USB_ENDPOINT_NUMBER_MASK	0x0F	/* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK		0x80

#define USB_ENDPOINT_XFERTYPE_MASK	0x03	/* in bmAttributes */
#define USB_ENDPOINT_XFER_CONTROL	0
#define USB_ENDPOINT_XFER_ISOC		1
#define USB_ENDPOINT_XFER_BULK		2
#define USB_ENDPOINT_XFER_INT		3


/*-------------------------------------------------------------------------*/

/* USB_DT_DEVICE_QUALIFIER: Device Qualifier descriptor */
typedef struct _USB_QUALIFIER_DESCRIPTOR 
{
	BYTE  	bLength;
	BYTE  	bDescriptorType;
	WORD	bcdUSB;
	BYTE  	bDeviceClass;
	BYTE  	bDeviceSubClass;
	BYTE  	bDeviceProtocol;
	BYTE  	bMaxPacketSize0;
	BYTE  	bNumConfigurations;
	BYTE  	bRESERVED;
}USB_QUALIFIER_DESCRIPTOR;


/*-------------------------------------------------------------------------*/

/* USB_DT_OTG (from OTG 1.0a supplement) */
typedef struct _USB_OTG_DESCRIPTOR 
{
	BYTE  	bLength;
	BYTE  	bDescriptorType;
	BYTE  	bmAttributes;	/* support for HNP, SRP, etc */
}USB_OTG_DESCRIPTOR;

/* from usb_otg_descriptor.bmAttributes */
#define USB_OTG_SUPPORT_SRP		(1 << 0)
#define USB_OTG_SUPPORT_HNP		(1 << 1)	/* swap host/device roles */

/*-------------------------------------------------------------------------*/

/* USB_DT_DEBUG:  for special highspeed devices, replacing serial console */
typedef struct _USB_DEBUG_DESCRIPTOR 
{
	BYTE  	bLength;
	BYTE  	bDescriptorType;
	/* bulk endpoints with 8 byte maxpacket */
	BYTE  	bDebugInEndpoint;
	BYTE  	bDebugOutEndpoint;
}USB_DEBUG_DESCRIPTOR;

/*-------------------------------------------------------------------------*/

/* USB_DT_INTERFACE_ASSOCIATION: groups interfaces */
typedef struct _USB_INTERFACE_ASSOC_DESCRIPTOR 
{
	BYTE  	bLength;
	BYTE  	bDescriptorType;
	BYTE  	bFirstInterface;
	BYTE  	bInterfaceCount;
	BYTE  	bFunctionClass;
	BYTE  	bFunctionSubClass;
	BYTE  	bFunctionProtocol;
	BYTE  	iFunction;
}USB_INTERFACE_ASSOC_DESCRIPTOR;


/*-------------------------------------------------------------------------*/

/* USB 2.0 defines three speeds */

typedef enum _USB_DEVICE_SPEED 
{
	USB_SPEED_UNKNOWN = 0,			/* enumerating */
	USB_SPEED_LOW, 
	USB_SPEED_FULL,				/* usb 1.1 */
	USB_SPEED_HIGH				/* usb 2.0 */
}USB_DEVICE_SPEED;

typedef enum _USB_DEVICE_STATE 
{
	/* NOTATTACHED isn't in the USB spec, and this state acts
	 * the same as ATTACHED ... but it's clearer this way.
	 */
	USB_STATE_NOTATTACHED = 0,

	/* the chapter 9 device states */
	USB_STATE_ATTACHED,
	USB_STATE_POWERED,
	USB_STATE_DEFAULT,			/* limited function */
	USB_STATE_ADDRESS,
	USB_STATE_CONFIGURED,			/* most functions */

	USB_STATE_SUSPENDED

	/* NOTE:  there are actually four different SUSPENDED
	 * states, returning to POWERED, DEFAULT, ADDRESS, or
	 * CONFIGURED respectively when SOF tokens flow again.
	 */
}USB_DEVICE_STATE;

typedef enum _USB_ENDPOINT_STATE 
{
	USB_EP_STATE_IDLE = 0,
	USB_EP_STATE_RX,
	USB_EP_STATE_TX,
	USB_EP_STATE_HALT

}USB_ENDPOINT_STATE;


#endif