#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "Memory.h"
#include "SendBuffer.h"

ThreadManager* GThreadManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
Memory* GMemory = nullptr;

class CoreGlobal {
public:
	CoreGlobal() {
		GThreadManager = new ThreadManager();
		GDeadLockProfiler = new DeadLockProfiler();
		GSendBufferManager = new SendBufferManager();
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
		if (GSendBufferManager)
			delete GSendBufferManager;
		SocketUtils::Clear();
	}
} GCoreGlobal;
