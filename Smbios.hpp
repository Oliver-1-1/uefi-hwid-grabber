#pragma once
#include "General.h"
//https://blog.fpmurphy.com/2012/08/display-uefi-firmware-version-vendor-revision-and-build-date.html
struct Information {
	CHAR16* f1;
	CHAR16* f2;
	CHAR16* f3;
};
class SmBios {
public:
	Information GrabHWID(CHAR8 TableNumber);
	VOID clear() const;
	SmBios();

private:
	CHAR8* LibGetSmbiosString(SMBIOS_STRUCTURE_POINTER* Smbios, UINT16 StringNumber);
	EFI_STATUS LibGetSmbiosStructure(UINT16* Handle, UINT8** Buffer, UINT16* Length);

	SMBIOS_TABLE_ENTRY_POINT* mSmbiosTable;
	SMBIOS_STRUCTURE_POINTER m_SmbiosStruct{};
	SMBIOS_STRUCTURE_POINTER* mSmbiosStruct;
	Information info{};

};