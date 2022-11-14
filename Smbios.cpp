#include "Smbios.hpp"

#define INVALID_HANDLE  (UINT16) (-1)
#define DMI_INVALID_HANDLE  0x83
#define DMI_SUCCESS 0x00

int Strlen(const char* str)
{
    const char* s;

    for (s = str; *s; ++s)
        ;
    return (s - str);
}


CHAR16* ASCII_to_UCS2(const char* s, int len)
{
    CHAR16* ret = nullptr;
    int i;

    ret = (CHAR16*)AllocateZeroPool(len * 2 + 2);
    if (!ret)
        return nullptr;

    for (i = 0; i < len; i++)
        ret[i] = s[i];

    return ret;
}
//This might be the wrong struct tho. this is table 0
/*typedef struct {
  SMBIOS_STRUCTURE             Hdr;
  SMBIOS_TABLE_STRING          Vendor;
  SMBIOS_TABLE_STRING          BiosVersion;
  UINT16                       BiosSegment;
  SMBIOS_TABLE_STRING          BiosReleaseDate;
  UINT8                        BiosSize;
  MISC_BIOS_CHARACTERISTICS    BiosCharacteristics;
  UINT8                        BIOSCharacteristicsExtensionBytes[2];
  UINT8                        SystemBiosMajorRelease;
  UINT8                        SystemBiosMinorRelease;
  UINT8                        EmbeddedControllerFirmwareMajorRelease;
  UINT8                        EmbeddedControllerFirmwareMinorRelease;
  //
  // Add for smbios 3.1.0
  //
  EXTENDED_BIOS_ROM_SIZE       ExtendedBiosSize;
} SMBIOS_TABLE_TYPE0;*/

Information SmBios::GrabHWID(CHAR8 TableNumber){
    //clear();
    UINTN   Index;
    UINT16  Length;
    UINT8* Buffer;
    EFI_GUID gEfiSmbiosTableGuid = SMBIOS_TABLE_GUID;

    SMBIOS_STRUCTURE_POINTER SmbiosStruct;
    EFI_STATUS Status = EFI_SUCCESS;
    Status = EfiGetSystemConfigurationTable(&gEfiSmbiosTableGuid, (VOID**)&mSmbiosTable); // Check for right guid. gefi3smbiostable guid should work for 64bit?
    if ((Status != EFI_SUCCESS || mSmbiosTable == NULL) || CompareMem(mSmbiosTable->AnchorString, "_SM_", 4) != 0) {
        Print((CHAR16*)L"SMBIOS table not found. \n");
        return Information();
    }

    mSmbiosStruct->Raw = (UINT8*)(UINTN)(mSmbiosTable->TableAddress);

    UINT16 Handle = INVALID_HANDLE;
    LibGetSmbiosStructure(&Handle, nullptr, nullptr);

    for (Index = 0; Index < mSmbiosTable->NumberOfSmbiosStructures; Index++) {
        if (Handle == INVALID_HANDLE) {
            break;
        }
        if (LibGetSmbiosStructure(&Handle, &Buffer, &Length) != DMI_SUCCESS) {
            break;
        }
        //InitSmbiosTableStatistics
        SmbiosStruct.Raw = Buffer;
        if (SmbiosStruct.Hdr->Type == TableNumber) { // Check for what table to look at
            CHAR8* str = LibGetSmbiosString(&SmbiosStruct, 1);
            info.f1 = ASCII_to_UCS2(str, Strlen(str));

            str = LibGetSmbiosString(&SmbiosStruct, 2);
            info.f2 = ASCII_to_UCS2(str, Strlen(str));

            str = LibGetSmbiosString(&SmbiosStruct, 3);
            info.f3 = ASCII_to_UCS2(str, Strlen(str));
            return info;
        }
    }
    return Information();
}
// https://github.com/tianocore/edk2/blob/master/ShellPkg/Library/UefiShellDebug1CommandsLib/SmbiosView/SmbiosView.c
CHAR8* SmBios::LibGetSmbiosString(SMBIOS_STRUCTURE_POINTER* Smbios,UINT16 StringNumber){
    UINT16  Index;
    CHAR8* String;

    ASSERT(Smbios != NULL);

    String = (CHAR8*)(Smbios->Raw + Smbios->Hdr->Length);

    for (Index = 1; Index <= StringNumber; Index++) {
        if (StringNumber == Index) {
            return String;
        }
        for (; *String != 0; String++)
            ;
        String++;

        if (*String == 0) {
            Smbios->Raw = (UINT8*)++String;
            return nullptr;
        }
    }
    return nullptr;
}
// https://github.com/tianocore/edk2/blob/master/ShellPkg/Library/UefiShellDebug1CommandsLib/SmbiosView/SmbiosView.c
EFI_STATUS SmBios::LibGetSmbiosStructure(UINT16* Handle,UINT8** Buffer,UINT16* Length){
    SMBIOS_STRUCTURE_POINTER  Smbios;
    SMBIOS_STRUCTURE_POINTER  SmbiosEnd;
    UINT8* Raw;

    if (*Handle == INVALID_HANDLE) {
        *Handle = mSmbiosStruct->Hdr->Handle;
        return DMI_INVALID_HANDLE;
    }

    if ((Buffer == NULL) || (Length == NULL)) {
        Print((CHAR16*)L"Invalid handle\n");
        return DMI_INVALID_HANDLE;
    }

    *Length = 0;
    Smbios.Hdr = mSmbiosStruct->Hdr;
    SmbiosEnd.Raw = Smbios.Raw + mSmbiosTable->TableLength;
    while (Smbios.Raw < SmbiosEnd.Raw) {
        if (Smbios.Hdr->Handle == *Handle) {
            Raw = Smbios.Raw;
            LibGetSmbiosString(&Smbios, (UINT16)(-1));
            *Length = (UINT16)(Smbios.Raw - Raw);
            *Buffer = Raw;
            if (Smbios.Raw < SmbiosEnd.Raw) {
                *Handle = Smbios.Hdr->Handle;
            }
            else {
                *Handle = INVALID_HANDLE;
            }
            return DMI_SUCCESS;
        }
        LibGetSmbiosString(&Smbios, (UINT16)(-1));
    }

    *Handle = INVALID_HANDLE;

    return DMI_INVALID_HANDLE;
}

SmBios::SmBios() {
    mSmbiosTable = nullptr;
    mSmbiosStruct = &m_SmbiosStruct;
}

void SmBios::clear() const{
    FreePool(info.f1);
    FreePool(info.f2);
    FreePool(info.f3);
}
