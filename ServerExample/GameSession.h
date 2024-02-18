#pragma once
#ifndef __GAMESESSION__
#define __GAMESESSION__

#include "Session.h"

class GameSession : public PacketSession {
public:
	~GameSession() {
		cout << "~GameSession" << endl;
	}

	/* ������ �ڵ忡�� �����ε� */
	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;
public:
	Vector<PlayerRef> _players;

	PlayerRef _currentPlayer;
	std::weak_ptr<class Room> _room;
};

#endif
