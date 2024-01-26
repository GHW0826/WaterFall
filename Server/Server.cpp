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

class Knight {
public:
	int32 _hp = rand() % 1000;
};

class Monster {
public:
	int64 _id = 0;
};

SLIST_HEADER* GHeader;
DECLSPEC_ALIGN(16)
class Data {
public:
	SLIST_ENTRY _entry;
	int64 _rand = rand() % 1000;
};

#include "SocketUtils.h"

int main() {

	SOCKET socket = SocketUtils::CreateSocket();

	SocketUtils::BindAnyAddress(socket, 7777);
	SocketUtils::Listen(socket);

	::accept(socket, nullptr, nullptr);

	SOCKET clientSocket = ::accept(socket, nullptr, nullptr);

	cout << "Client Conencted!" << endl;

	GThreadManager->Join();

	return 0;
}
