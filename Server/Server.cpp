#include "pch.h"
#include "CorePch.h"
#include "CoreMacro.h"
#include <thread>
#include "ThreadManager.h"

#include "PlayerManager.h"
#include "AccountManager.h"
#include <memory>
#include "Memory.h"
#include "Allocator.h"
#include "LockFreeStack.h"

#include "SocketUtils.h"
#include "Listener.h"
#include "Service.h"
#include "Session.h"

int main() {

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<Session>, // TODO Sessionmanager
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; ++i) {
		GThreadManager->Launch([=]() {
			while (true) {
				service->GetIocpCore()->Dispatch();
			}
		});
	}

	GThreadManager->Join();

	return 0;
}
