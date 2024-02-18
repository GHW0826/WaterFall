#pragma once
#ifndef __IOCPCORE__
#define __IOCPCORE__

// IOCP�� ��� ������ ������Ʈ (���� ����)
class IocpObject : public enable_shared_from_this<IocpObject> {
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};

class IocpCore {
public:
	IocpCore();
	~IocpCore();

	FORCEINLINE HANDLE GetHandle() { return _IocpHandle; }

	bool Register(IocpObjectRef iocpObject);
	bool Dispatch(uint32 timeoutMs = INFINITE);

private:
	HANDLE _IocpHandle;
};

#endif

