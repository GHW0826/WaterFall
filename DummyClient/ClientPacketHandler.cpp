#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len) {

	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id) {
	case S_TEST:
		Handle_S_TEST(buffer, len);
		break;
	}
}

struct BuffData {
	uint64 buffId;
	float remainTime;
};

// 패킷 설계 temp
struct PKT_S_TEST {
	uint16 packetSize;
	uint16 packetId;
	uint64 id;
	uint32 hp;
	uint16 attack;
	//vector<BuffData> buffs;
};

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len) {

	BufferReader br(buffer, len);

	if (len < sizeof(PKT_S_TEST))
		return;

	PacketHeader header;
	br >> header;

	uint64 id;
	uint32 hp;
	uint16 attack;
	br >> id >> hp >> attack;
	cout << " ID: " << id << " HP : " << hp << "ATT: " << attack << endl;

	vector<BuffData> buffs;
	uint16 buffCount;
	br >> buffCount;
	buffs.resize(buffCount);
	for (int32 i = 0; i < buffCount; ++i) {
		br >> buffs[i].buffId >> buffs[i].remainTime;
	}
	cout << "buff count : " << buffCount << endl;
	for (int32 i = 0; i < buffCount; ++i) {
		cout << i << " : " << buffs[i].buffId << buffs[i].remainTime << endl;
	}

	//PacketHeader header = *(reinterpret_cast<PacketHeader*>(buffer));
	cout << "Packet ID: " << header.id << " Size : " << header.size << endl;

	char recvBuffer[4096];
	br.Read(recvBuffer, header.size - sizeof(PacketHeader) - 8 - 4 - 2);
	::memcpy(recvBuffer, &buffer[4], header.size - sizeof(PacketHeader));
	cout << "Msg : " << recvBuffer << endl;
}
