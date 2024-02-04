#pragma once
#ifndef __IOCPEVENT__
#define __IOCPEVENT__

class Session;

enum class EventType : uint8 {
	Connect,
	Disconnect,
	Accept,
	// PreRecv -> Zero byte receive
	Recv,
	Send
};

class IocpEvent :public OVERLAPPED {
public:
	IocpEvent(EventType type);

	void Init();

public:
	EventType eventType;
	IocpObjectRef owner;
};



class ConnectEvent : public IocpEvent {
public:
	ConnectEvent()
		: IocpEvent(EventType::Connect)
	{}
};

class DisconnectEvent : public IocpEvent {
public:
	DisconnectEvent()
		: IocpEvent(EventType::Disconnect)
	{}
};

class AcceptEvent : public IocpEvent {
public:
	AcceptEvent()
		: IocpEvent(EventType::Accept)
	{}

public:
	SessionRef session;
};

class RecvEvent : public IocpEvent {
public:
	RecvEvent()
		: IocpEvent(EventType::Recv)
	{}
};

class SendEvent : public IocpEvent {
public:
	SendEvent()
		: IocpEvent(EventType::Send)
	{}

	// TEMP
	vector<SendBufferRef> sendBuffers;
};

#endif

