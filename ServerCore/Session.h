#pragma once
#ifndef __SESSION__
#define __SESSION__

#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"

// listner와 마찬가지로 IocpCore에 등록할 대상
class Session : public IocpObject {
public:
	Session();
	virtual ~Session();
public:
	/* 정보 관련 */
	FORCEINLINE void SetNetAddress(NetAddress address) { _netAddress = address; }
	FORCEINLINE NetAddress GetAddress() { return _netAddress; }
	FORCEINLINE SOCKET GetSocket() { return _socket; }
public:
	/* 인터페이스 구현 */
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
