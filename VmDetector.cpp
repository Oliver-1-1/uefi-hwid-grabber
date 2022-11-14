#include "VmDetector.h"

bool VmDetector::isVmPresent() {
	Information inf = sm.GrabHWID(1);
	Print((CHAR16*)L"%s\n", inf.f1);

	for (int i = 0; i < 3; i++) {
		Print((CHAR16*)L"%s\n", BannedVirtualMachines[i]);

		if (StrCmp(BannedVirtualMachines[i], inf.f1) == 0) {
			return true;
		}
	}
	sm.clear();
	return false;
}