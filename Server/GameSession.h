#pragma once
#ifndef __GAMESESSION__
#define __GAMESESSION__

#include "Session.h"

class GameSession : public PacketSession {
public:
	/* ������ �ڵ忡�� �����ε� */
	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual int32 OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;
};

#endif
