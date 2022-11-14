#pragma once
#include "Smbios.hpp"
class VmDetector {
public:
	bool isVmPresent();
private:
	//Their is ofc many other way details in the smbios struct to detect.
	CHAR16* BannedVirtualMachines[3] = { (CHAR16*)L"VMware, Inc.", (CHAR16*)L"QEMU",(CHAR16*)L"Acer" };
	SmBios sm;

};

