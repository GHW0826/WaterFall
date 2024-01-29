#pragma once
#ifndef __SESSION__
#define __SESSION__

#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"

// listner�� ���������� IocpCore�� ����� ���
class Session : public IocpObject {
public:
	Session();
	virtual ~Session();
public:
	/* ���� ���� */
	FORCEINLINE void SetNetAddress(NetAddress address) { _netAddress = address; }
	FORCEINLINE NetAddress GetAddress() { return _netAddress; }
	FORCEINLINE SOCKET GetSocket() { return _socket; }
public:
	/* �������̽� ���� */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// temp
	char _recvBuffer[100];
private:
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	Atomic<bool> _connected = false;
};

#endif
