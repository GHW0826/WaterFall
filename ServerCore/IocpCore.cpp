#include "pch.h"
#include "IOCPCore.h"
#include "IocpEvent.h"


IocpCore::IocpCore() {
	_IocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_IocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore() {
	::CloseHandle(_IocpHandle);
}


// CreateIoCompletionPort에 일감이 아니라 다양하게 사용 가능
bool IocpCore::Register(IocpObjectRef iocpObject) {
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _IocpHandle, 
		/* key (session)reinterpret_cast<ULONG_PTR>(iocpObject) */ 0, 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs) {

	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	IocpObject* iocpObject = nullptr;
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_IocpHandle, OUT &numOfBytes, OUT &key,
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs) == true) {

		IocpObjectRef iocpObject = iocpEvent->owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else {
		// fail
		// 타임 관련 에러도 있음
		int32 errCode = ::WSAGetLastError();
		switch (errCode) {
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO 로그찍기
			IocpObjectRef iocpObject = iocpEvent->owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}
