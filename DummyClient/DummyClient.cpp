#include "pch.h"
#include <iostream>
#include "Service.h"
#include "Session.h"
#include "ThreadManager.h"

// char sendData[] = "Hello World";
class ServerSession : public PacketSession {
public:
	/* 컨텐츠 코드에서 오버로딩 */
	virtual void OnConnected() override {

		cout << "Connected To Server" << endl;
		/*
		SendBufferRef sendBuffer = MakeShared<SendBuffer>(4096);
		sendBuffer->CopyData(sendData, sizeof(sendData));
		SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
		::memcpy(sendBuffer->Buffer(), sendData, sizeof(sendData));
		sendBuffer->Close(sizeof(sendData));
		Send(sendBuffer);
		*/
	}

	virtual void OnDisconnected() override {
		cout << "Discounted" << endl;
	}

	virtual int32 OnRecvPacket(BYTE* buffer, int32 len) override {

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(buffer));
		cout << "Packet ID: " << header.id << " Size : " << header.size << endl;

		char recvBuffer[4096];
		::memcpy(recvBuffer, &buffer[4], header.size - sizeof(PacketHeader));
		cout << "Msg : " << recvBuffer << endl;

		return len;
	}
	virtual void OnSend(int32 len) override {
		cout << "Send Len = " << len << endl;
	}
};

int main() {
	this_thread::sleep_for(1s);

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>, // TODO Sessionmanager
		5);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; ++i) {
		GThreadManager->Launch([=]() {
			while (true) {
				service->GetIocpCore()->Dispatch();
			}
		});
	}

	GThreadManager->Join();
}
