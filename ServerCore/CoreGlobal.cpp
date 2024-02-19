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
#include "ConsoleLog.h"

ThreadManager* GThreadManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
Memory* GMemory = nullptr;
GlobalQueue* GGlobalQueue = nullptr;
JobTimer* GJobTimer = nullptr;
DBConnectionPool* GDBConnectionPool = nullptr;
ConsoleLog* GConsoleLogger = nullptr;

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
		GConsoleLogger = new ConsoleLog();
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
		if (GConsoleLogger)
			delete GConsoleLogger;
		SocketUtils::Clear();
	}
} GCoreGlobal;
