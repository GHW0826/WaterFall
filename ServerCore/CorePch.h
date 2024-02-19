#pragma once
#ifndef _COREPCH_
#define _COREPCH_

#include "Types.h"
#include "CoreMacro.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "Container.h"
#include "Memory.h"

#include <Windows.h>
#include <iostream>
using namespace std;

#include <WinSock2.h>
#include <mswsock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "Lock.h"
#include "TypeCast.h"
#include "ObjectPool.h"
#include "SendBuffer.h"
#include "Session.h"
#include "JobQueue.h"
#include "ConsoleLog.h"

#endif
