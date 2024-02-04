#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"


 void GameSession::OnConnected()  {
	 GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}
 void GameSession::OnDisconnected() {
	 GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}

int32 GameSession::OnRecvPacket(BYTE* buffer, int32 len)  {
	cout << "OnRecv Len = " << len << endl;

	PacketHeader header = *(reinterpret_cast<PacketHeader*>(buffer));

	cout << "Packet ID: " << header.id << " Size : " << header.size << endl;
	/*
	SendBufferRef sendBuffer = MakeShared<SendBuffer>(4096);
	sendBuffer->CopyData(buffer, len);
	*/
	return len;
}
void GameSession::OnSend(int32 len)  {
	cout << "Send Len = " << len << endl;
}