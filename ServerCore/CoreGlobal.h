#pragma once
#ifndef _COREGLOBAL_
#define _COREGLOBAL_

extern class Memory* GMemory;
extern class ThreadManager* GThreadManager;
extern class SendBufferManager* GSendBufferManager;
extern class DeadLockProfiler* GDeadLockProfiler;
extern class GlobalQueue* GGlobalQueue;
extern class JobTimer* GJobTimer;
extern class DBConnectionPool* GDBConnectionPool;
extern class ConsoleLog* GConsoleLogger;

#endif