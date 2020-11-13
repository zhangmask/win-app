#pragma once

#include <fwpsk.h>
#include <fwpmtypes.h>

typedef struct INJECTION_DATA_
{
	ADDRESS_FAMILY              addressFamily;
	FWP_DIRECTION               direction;
	BOOLEAN                     isIPsecSecured;
	HANDLE                      injectionHandle;
	HANDLE                      injectionContext;
	FWPS_PACKET_INJECTION_STATE injectionState;
	VOID* pContext;
	BYTE* pControlData;
	UINT32                      controlDataLength;
}INJECTION_DATA, * PINJECTION_DATA;

typedef struct CLASSIFY_DATA_
{
	const FWPS_INCOMING_VALUES* pClassifyValues;
	const FWPS_INCOMING_METADATA_VALUES* pMetadataValues;
	VOID* pPacket;               /// NET_BUFFER_LIST | FWPS_STREAM_CALLOUT_IO_PACKET
	const VOID* pClassifyContext;
	const FWPS_FILTER* pFilter;
	UINT64                               flowContext;
	FWPS_CLASSIFY_OUT* pClassifyOut;
	UINT64                               classifyContextHandle;
	BOOLEAN                              chainedNBL;
	UINT32                               numChainedNBLs;
}CLASSIFY_DATA, * PCLASSIFY_DATA;

typedef struct BASIC_PACKET_INJECTION_COMPLETION_DATA_
{
	KSPIN_LOCK                  spinLock;
	INT32                       refCount;
	BOOLEAN                     performedInline;
	CLASSIFY_DATA* pClassifyData;
	INJECTION_DATA* pInjectionData;
	FWPS_TRANSPORT_SEND_PARAMS* pSendParams;
}BASIC_PACKET_INJECTION_COMPLETION_DATA, * PBASIC_PACKET_INJECTION_COMPLETION_DATA;

typedef struct
{
	UINT8  HdrLength : 4;
	UINT8  Version : 4;
	UINT8  TOS;
	UINT16 Length;
	UINT16 Id;
	UINT16 FragOff0;
	UINT8  TTL;
	UINT8  Protocol;
	UINT16 Checksum;
	UINT32 SrcAddr;
	UINT32 DstAddr;
} WINDIVERT_IPHDR, * PWINDIVERT_IPHDR;

typedef struct
{
	UINT16 SrcPort;
	UINT16 DstPort;
	UINT16 Length;
	UINT16 Checksum;
} WINDIVERT_UDPHDR, * PWINDIVERT_UDPHDR;

typedef struct
{
	UINT16 transaction_id;
	UINT16 flags;
	UINT16 questions;
	UINT16 answers;
	UINT16 authorities;
	UINT16 additional;
} DNSHEADER, * PDNSHEADER;

typedef struct
{
	WINDIVERT_IPHDR ip;
	WINDIVERT_UDPHDR udp;
	DNSHEADER dns;
} DNSPACKETV4, * PDNSPACKETV4;

VOID NTAPI CompleteBasicPacketInjection(_In_ VOID* pContext,
	_Inout_ NET_BUFFER_LIST* pNetBufferList,
	_In_ BOOLEAN dispatchLevel);

NTSTATUS RegisterCallout(
	_In_ PDEVICE_OBJECT deviceObject,
	_In_ const GUID& key,
	_In_ FWPS_CALLOUT_CLASSIFY_FN classifyFn);

NTSTATUS UnregisterCallout(_In_ const GUID& key);

void NTAPI RedirectConnection(
	IN const FWPS_INCOMING_VALUES* inFixedValues,
	IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
	IN OUT VOID* layerData,
	IN const void* classifyContext,
	IN const FWPS_FILTER* filter,
	IN UINT64 flowContext,
	IN OUT FWPS_CLASSIFY_OUT* classifyOut
);

void NTAPI RedirectUDPFlow(
	IN const FWPS_INCOMING_VALUES* inFixedValues,
	IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
	IN OUT VOID* layerData,
	IN const void* classifyContext,
	IN const FWPS_FILTER* filter,
	IN UINT64 flowContext,
	IN OUT FWPS_CLASSIFY_OUT* classifyOut
);

void NTAPI BlockDnsBySendingServerFailPacket(
	IN const FWPS_INCOMING_VALUES* inFixedValues,
	IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
	IN OUT VOID* packet,
	IN const void*,
	IN const FWPS_FILTER*,
	IN UINT64,
	IN OUT FWPS_CLASSIFY_OUT* classifyOut
);

#define BYTESWAP16(x)                   \
    ((((x) >> 8) & 0x00FFu) | (((x) << 8) & 0xFF00u))
#define ntohs(x)                        BYTESWAP16(x)
#define htons(x)                        BYTESWAP16(x)
#define RtlUshortByteSwap(_x)    _byteswap_ushort((USHORT)(_x))
