#pragma once
#ifndef __ROOM__
#define __ROOM__

#include "Job.h"

class Room : public JobQueue {
public:
	// �̱� ������ ȯ���� ���� �ڵ�
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);
private:
	map<uint64, PlayerRef> _players;
};

extern shared_ptr<Room> GRoom;

#endif
