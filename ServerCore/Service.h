#pragma once
#ifndef __SERVICE__
#define __SERVICE__

#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include <functional>

enum class ServiceType : uint8 {
	Server,
	Client
};

using SessionFactory = function<SessionRef(void)>;

class Service : public enable_shared_from_this<Service> {
public:
	Service (
		ServiceType type,
		NetAddress address,
		IocpCoreRef core,
		SessionFactory factory,
		int32 maxSessionCount = 1
	);
	virtual ~Service();

	virtual bool Start() abstract;
	bool CanStart() { return _sessionFactory != nullptr; }

	virtual void CloseService();

	SessionRef CreateSession();
	void Broadcast(SendBufferRef sendBuffer);
	void AddSession(SessionRef session);
	void ReleaseSession(SessionRef session);
public:
	FORCEINLINE int32 GetCurrentSessionCount() { return _sessionCount; }
	FORCEINLINE int32 GetMaxSessionCount() { return _maxSessionCount; }
	FORCEINLINE void SetSessionFactory(SessionFactory func) { _sessionFactory = func; }
	FORCEINLINE ServiceType GetServiceType() { return _type; }
	FORCEINLINE NetAddress GetNetAddress() { return _netAddress; }
	FORCEINLINE IocpCoreRef& GetIocpCore() { return _iocpCore; }
protected:
	USE_LOCK;
	ServiceType _type;
	NetAddress _netAddress = {};
	IocpCoreRef _iocpCore;

	Set<SessionRef> _sessions;
	int32 _sessionCount = 0;
	int32 _maxSessionCount = 0;
	SessionFactory _sessionFactory;
};



class ClientService : public Service {
public:
	ClientService(
		NetAddress address,
		IocpCoreRef core,
		SessionFactory factory,
		int32 maxSessionCount = 1
	);
	virtual ~ClientService() {}

	virtual bool Start() override;
};

class ServerService : public Service {
public:
	ServerService(
		NetAddress address,
		IocpCoreRef core,
		SessionFactory factory,
		int32 maxSessionCount = 1
	);
	virtual ~ServerService() {}

	virtual bool Start() override;
	virtual void CloseService();
private:
	ListenerRef _listener = nullptr;
};



#endif
