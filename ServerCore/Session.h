#pragma once
#ifndef __SESSION__
#define __SESSION__

#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

// listner와 마찬가지로 IocpCore에 등록할 대상
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
	/* 외부에서 사용 */
	void Send(SendBufferRef sendBuffer);
	bool Connect();
	void Disconnect(const WCHAR* cause);

	FORCEINLINE std::shared_ptr<Service> GetService() { return _service.lock(); }
	FORCEINLINE void SetService(std::shared_ptr<Service> service) { _service = service; }
public:
	/* 정보 관련 */
	FORCEINLINE void SetNetAddress(NetAddress address) { _netAddress = address; }
	FORCEINLINE NetAddress GetAddress() { return _netAddress; }
	FORCEINLINE SOCKET GetSocket() { return _socket; }
	FORCEINLINE bool IsConnected() { return _connected; }
	FORCEINLINE SessionRef GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
private:
	/* 전송 관련 */
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
	/* 컨텐츠 코드에서 오버라이딩 */
	virtual void OnConnected() {}
	virtual int32 OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void OnSend(int32 len) {}
	virtual void OnDisconnected() {}
private:
	/* 인터페이스 구현 */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
private:
	std::weak_ptr<Service> _service;
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	Atomic<bool> _connected = false;
private:
	USE_LOCK;

	/* 수신 관련 */
	RecvBuffer _recvBuffer;

	/* 송신 관련 */
	Queue<SendBufferRef> _sendQueue;
	Atomic<bool> _sendRegistered = false;
private:
	/* IocpEvent 재사용 */
	ConnectEvent _connectEvent;
	DisconnectEvent _disconnectEvent;
	RecvEvent _recvEvent;
	SendEvent _sendEvent;
};


// OnRecv가 끝날때 전체 패킷이 전송 되었는지 판단되는 여부
// 프로토콜로 정함
// 특수 문자가 오면 패킷 완료로 보는 방법도 있지만 구림.
// MMO 정석은 헤더를 만들어 추가해서 넣어줌

// [size(2)][id(2)][data....][size(2)][id(2)][data....][size(2)][id(2)][data....]
struct PacketHeader {
	uint16 size;	// 전체 패킷 사이즈
	uint16 id;		// 프로토콜 ID
};

// 헤더 파싱 후 사이즈 추출후 완성 시킴
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
