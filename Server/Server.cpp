#include "pch.h"
#include "CorePch.h"
#include "CoreMacro.h"
#include <thread>
#include "ThreadManager.h"

#include "PlayerManager.h"
#include "AccountManager.h"
#include <memory>
#include "Memory.h"


int main() {

	// 가상 메모리 기본
	SYSTEM_INFO info;
	::GetSystemInfo(&info);

	info.dwPageSize;
	info.dwAllocationGranularity; 

	// 
	//
	int* test =(int*)::VirtualAlloc(nullptr, 4, MEM_RELEASE | MEM_COMMIT, PAGE_READWRITE);

	return 0;
}
