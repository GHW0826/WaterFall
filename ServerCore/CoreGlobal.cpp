#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "Memory.h"
#include "SendBuffer.h"
#include "GlobalQueue.h"
#include "JobTimer.h"
#include "DBConnectionPool.h"

ThreadManager* GThreadManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
Memory* GMemory = nullptr;
GlobalQueue* GGlobalQueue = nullptr;
JobTimer* GJobTimer = nullptr;
DBConnectionPool* GDBConnectionPool = nullptr;

class CoreGlobal {
public:
	CoreGlobal() {
		GThreadManager = new ThreadManager();
		GDeadLockProfiler = new DeadLockProfiler();
		GSendBufferManager = new SendBufferManager();
		GMemory = new Memory();
		GGlobalQueue = new GlobalQueue();
		GJobTimer = new JobTimer();
		GDBConnectionPool = new DBConnectionPool();
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
		if (GGlobalQueue)
			delete GGlobalQueue;		
		if (GJobTimer)
			delete GJobTimer;
		if (GDBConnectionPool)
			delete GDBConnectionPool;
		SocketUtils::Clear();
	}
} GCoreGlobal;
