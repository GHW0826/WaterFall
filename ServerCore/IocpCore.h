#pragma once
#ifndef __IOCPCORE__
#define __IOCPCORE__

// IOCP에 등록 가능한 오브젝트 (세션 느낌)
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

