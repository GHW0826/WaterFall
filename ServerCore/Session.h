#pragma once
#ifndef __SESSION__
#define __SESSION__

#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"

class Service;

// listner�� ���������� IocpCore�� ����� ���
class Session : public IocpObject {

	friend class Listener;
	friend class IocpCore;
	friend class Service;

public:
	Session();
	virtual ~Session();
public:

	void Disconnect(const WCHAR* cause);

	FORCEINLINE shared_ptr<Service> GetService() { return _service.lock(); }
	FORCEINLINE void SetService(shared_ptr<Service> service) { _service = service; }
public:
	/* ���� ���� */
	FORCEINLINE void SetNetAddress(NetAddress address) { _netAddress = address; }
	FORCEINLINE NetAddress GetAddress() { return _netAddress; }
	FORCEINLINE SOCKET GetSocket() { return _socket; }
	FORCEINLINE bool IsConnected() { return _connected; }
	FORCEINLINE SessionRef GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
private:
	/* ���� ���� */
	void RegisterConnect();
	void RegisterRecv();
	void RegisterSend();

	void ProcessConnect();
	void ProcessRecv(int32 numOfBytes);
	void ProcessSend(int32 numOfBytes);

	void HandleError(int32 errorCode);
protected:
	/* ������ �ڵ忡�� �����ε� */
	virtual void OnConnected() {}
	virtual int32 OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void OnSend(int32 len) {}
	virtual void OnDisconnected() {}
private:
	/* �������̽� ���� */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// temp
	char _recvBuffer[100];
private:
	weak_ptr<Service> _service;
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	Atomic<bool> _connected = false;
private:
	USE_LOCK;

	/* ���� ���� */


	/* �۽� ���� */
private:
	/* IocpEvent ���� */
	RecvEvent _recvEvent;
};

#endif
