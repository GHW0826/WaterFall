#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "Memory.h"

ThreadManager* GThreadManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;
Memory* GMemory = nullptr;

class CoreGlobal {
public:
	CoreGlobal() {
		GThreadManager = new ThreadManager();
		GDeadLockProfiler = new DeadLockProfiler();
		GMemory = new Memory();
		SocketUtils::Init();
	}
	~CoreGlobal() {
		if (GThreadManager)
			delete GThreadManager;
		if (GDeadLockProfiler)
			delete GDeadLockProfiler;
		if (GMemory)
			delete GMemory;
		SocketUtils::Clear();
	}
} GCoreGlobal;
