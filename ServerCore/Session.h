#pragma once
#ifndef __SESSION__
#define __SESSION__

#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

// listner�� ���������� IocpCore�� ����� ���
class Session : public IocpObject {

	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum { 
		BUFFER_SIZE = 0x10000 // 64KB 
	};

public:
	Session();
	virtual ~Session();
public:
	/* �ܺο��� ��� */
	void Send(SendBufferRef sendBuffer);
	bool Connect();
	void Disconnect(const WCHAR* cause);

	FORCEINLINE std::shared_ptr<Service> GetService() { return _service.lock(); }
	FORCEINLINE void SetService(std::shared_ptr<Service> service) { _service = service; }
public:
	/* ���� ���� */
	FORCEINLINE void SetNetAddress(NetAddress address) { _netAddress = address; }
	FORCEINLINE NetAddress GetAddress() { return _netAddress; }
	FORCEINLINE SOCKET GetSocket() { return _socket; }
	FORCEINLINE bool IsConnected() { return _connected; }
	FORCEINLINE SessionRef GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
private:
	/* ���� ���� */
	bool RegisterConnect();
	bool RegisterDisconnect();
	void RegisterRecv();
	void RegisterSend();

	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(int32 numOfBytes);
	void ProcessSend(int32 numOfBytes);

	void HandleError(int32 errorCode);
protected:
	/* ������ �ڵ忡�� �������̵� */
	virtual void OnConnected() {}
	virtual int32 OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void OnSend(int32 len) {}
	virtual void OnDisconnected() {}
private:
	/* �������̽� ���� */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
private:
	std::weak_ptr<Service> _service;
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	Atomic<bool> _connected = false;
private:
	USE_LOCK;

	/* ���� ���� */
	RecvBuffer _recvBuffer;

	/* �۽� ���� */
	Queue<SendBufferRef> _sendQueue;
	Atomic<bool> _sendRegistered = false;
private:
	/* IocpEvent ���� */
	ConnectEvent _connectEvent;
	DisconnectEvent _disconnectEvent;
	RecvEvent _recvEvent;
	SendEvent _sendEvent;
};


// OnRecv�� ������ ��ü ��Ŷ�� ���� �Ǿ����� �ǴܵǴ� ����
// �������ݷ� ����
// Ư�� ���ڰ� ���� ��Ŷ �Ϸ�� ���� ����� ������ ����.
// MMO ������ ����� ����� �߰��ؼ� �־���

// [size(2)][id(2)][data....][size(2)][id(2)][data....][size(2)][id(2)][data....]
struct PacketHeader {
	uint16 size;	// ��ü ��Ŷ ������
	uint16 id;		// �������� ID
};

// ��� �Ľ� �� ������ ������ �ϼ� ��Ŵ
class PacketSession : public Session {
public:
	PacketSession();
	virtual ~PacketSession();

	PacketSessionRef GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual int32 OnRecv(BYTE* buffer, int32 len) sealed;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) abstract;
};


#endif
