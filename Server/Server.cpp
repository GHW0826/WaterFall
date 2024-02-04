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
#include "GameSession.h"
#include "GameSessionManager.h"

class GameSession2 : public PacketSession {
public:
	/* 컨텐츠 코드에서 오버로딩 */
	virtual void OnConnected() {
	}
	virtual void OnDisconnected() {
	}

	virtual int32 OnRecvPacket(BYTE* buffer, int32 len) override {
		cout << "OnRecv Len = " << len << endl;
		/*
		SendBufferRef sendBuffer = MakeShared<SendBuffer>(4096);
		sendBuffer->CopyData(buffer, len);
		*/

		SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
		::memcpy(sendBuffer->Buffer(), buffer, len);
		sendBuffer->Close(len);
		Send(sendBuffer);
		return len;
	}
	virtual void OnSend(int32 len) override {
		cout << "Send Len = " << len << endl;
	}
};

char sendData[] = "Hello World";

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

	while (true) {
		SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

		BYTE* buffer = sendBuffer->Buffer();

		reinterpret_cast<PacketHeader*>(buffer)->size = sizeof(sendData) + sizeof(PacketHeader);
		reinterpret_cast<PacketHeader*>(buffer)->id = 1; // 1 : Hello msg

		::memcpy(&buffer[4], buffer, sizeof(sendData));
		sendBuffer->Close(sizeof(sendData) + sizeof(PacketHeader));

		GSessionManager.BroadCast(sendBuffer);
		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();

	return 0;
}
