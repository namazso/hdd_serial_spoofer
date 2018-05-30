#pragma once
#include <ntddk.h>

EXTERN_C_START

extern NTKERNELAPI
NTSTATUS
ObReferenceObjectByName(
	IN PUNICODE_STRING ObjectName,
	IN ULONG Attributes,
	IN PACCESS_STATE PassedAccessState,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_TYPE ObjectType,
	IN KPROCESSOR_MODE AccessMode,
	IN OUT PVOID ParseContext,
	OUT PVOID * Object
);

extern POBJECT_TYPE *IoDriverObjectType;

#define SMART_RCV_DRIVE_DATA \
  CTL_CODE(IOCTL_DISK_BASE, 0x0022, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define  DFP_SEND_DRIVE_COMMAND   \
  CTL_CODE(IOCTL_DISK_BASE, 0x0021, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define  DFP_RECEIVE_DRIVE_DATA   \
  CTL_CODE(IOCTL_DISK_BASE, 0x0022, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)


#define  IDE_ATA_IDENTIFY           0xEC

typedef struct _IDINFO
{
	USHORT	wGenConfig;
	USHORT	wNumCyls;
	USHORT	wReserved;
	USHORT	wNumHeads;
	USHORT	wBytesPerTrack;
	USHORT	wBytesPerSector;
	USHORT	wNumSectorsPerTrack;
	USHORT	wVendorUnique[3];
	CHAR	sSerialNumber[20];
	USHORT	wBufferType;
	USHORT	wBufferSize;
	USHORT	wECCSize;
	CHAR	sFirmwareRev[8];
	CHAR	sModelNumber[40];
	USHORT	wMoreVendorUnique;
	USHORT	wDoubleWordIO;
	struct {
		USHORT	Reserved	: 8;
		USHORT	DMA			: 1;
		USHORT	LBA			: 1;
		USHORT	DisIORDY	: 1;
		USHORT	IORDY		: 1;
		USHORT	SoftReset	: 1;
		USHORT	Overlap		: 1;
		USHORT	Queue		: 1;
		USHORT	InlDMA		: 1;
	} wCapabilities;
	USHORT	wReserved1;
	USHORT	wPIOTiming;
	USHORT	wDMATiming;
	struct {
		USHORT	CHSNumber	: 1;
		USHORT	CycleNumber	: 1;
		USHORT	UnltraDMA	: 1;
		USHORT	Reserved	: 13;
	} wFieldValidity;
	USHORT	wNumCurCyls;
	USHORT	wNumCurHeads;
	USHORT	wNumCurSectorsPerTrack;
	USHORT	wCurSectorsLow;
	USHORT	wCurSectorsHigh;
	struct {
		USHORT	CurNumber	: 8;
		USHORT	Multi		: 1;
		USHORT	Reserved	: 7;
	} wMultSectorStuff;
	ULONG	dwTotalSectors;
	USHORT	wSingleWordDMA;
	struct {
		USHORT	Mode0		: 1;
		USHORT	Mode1		: 1;
		USHORT	Mode2		: 1;
		USHORT	Reserved1	: 5;
		USHORT	Mode0Sel	: 1;
		USHORT	Mode1Sel	: 1;
		USHORT	Mode2Sel	: 1;
		USHORT	Reserved2	: 5;
	} wMultiWordDMA;
	struct {
		USHORT	AdvPOIModes	: 8;
		USHORT	Reserved	: 8;
	} wPIOCapacity;
	USHORT	wMinMultiWordDMACycle;
	USHORT	wRecMultiWordDMACycle;
	USHORT	wMinPIONoFlowCycle;
	USHORT	wMinPOIFlowCycle;
	USHORT	wReserved69[11];
	struct {
		USHORT	Reserved1	: 1;
		USHORT	ATA1		: 1;
		USHORT	ATA2		: 1;
		USHORT	ATA3		: 1;
		USHORT	ATA4		: 1;
		USHORT	ATA5		: 1;
		USHORT	ATA6		: 1;
		USHORT	ATA7		: 1;
		USHORT	ATA8		: 1;
		USHORT	ATA9		: 1;
		USHORT	ATA10		: 1;
		USHORT	ATA11		: 1;
		USHORT	ATA12		: 1;
		USHORT	ATA13		: 1;
		USHORT	ATA14		: 1;
		USHORT	Reserved2	: 1;
	} wMajorVersion;
	USHORT	wMinorVersion;
	USHORT	wReserved82[6];
	struct {
		USHORT	Mode0		: 1;
		USHORT	Mode1		: 1;
		USHORT	Mode2		: 1;
		USHORT	Mode3		: 1;
		USHORT	Mode4		: 1;
		USHORT	Mode5		: 1;
		USHORT	Mode6		: 1;
		USHORT	Mode7		: 1;
		USHORT	Mode0Sel	: 1;
		USHORT	Mode1Sel	: 1;
		USHORT	Mode2Sel	: 1;
		USHORT	Mode3Sel	: 1;
		USHORT	Mode4Sel	: 1;
		USHORT	Mode5Sel	: 1;
		USHORT	Mode6Sel	: 1;
		USHORT	Mode7Sel	: 1;
	} wUltraDMA;
	USHORT	wReserved89[167];
} IDINFO, *PIDINFO;

EXTERN_C_END