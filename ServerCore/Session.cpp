#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

Session::Session() 
	:_recvBuffer(BUFFER_SIZE) {
	_socket = SocketUtils::CreateSocket();
}

Session::~Session() {
	SocketUtils::Close(_socket);
}

void Session::Send(SendBufferRef sendBuffer) {

	if (IsConnected() == false)
		return;

	bool registerSend = false;
	// 현재 RegisterSend가 걸리지 않은 상태라면 걸어준다.
	{
		WRITE_LOCK;

		_sendQueue.push(sendBuffer);

		if (_sendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		RegisterSend();
}

bool Session::Connect() {
	return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause) {

	if (_connected.exchange(false) == false)
		return;

	// TEMP
	wcout << "Disconnected : " << cause << endl;

	RegisterDisconnect();
}

HANDLE Session::GetHandle() {
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes) {

	// TODO
	switch (iocpEvent->eventType) {
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		//ProcessSend(static_cast<SendEvent*>(iocpEvent), numOfBytes);
		break;
	default:
		break;
	}
}


bool Session::RegisterConnect() {
	if (IsConnected())
		return false;

	if (GetService()->GetServiceType() != ServiceType::Client)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::BindAnyAddress(_socket, 0/* 남은거 */) == false)
		return false;

	_connectEvent.Init();
	_connectEvent.owner = shared_from_this(); // Add_Ref

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (false == SocketUtils::ConnectEx(
		_socket,
		reinterpret_cast<SOCKADDR*>(&sockAddr),
		sizeof(sockAddr),
		nullptr, 0,
		&numOfBytes, &_connectEvent)) {
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING) {
			_connectEvent.owner = nullptr; // Release_ref
			return false;
		}
	}

	return true;
}

bool Session::RegisterDisconnect() {

	_disconnectEvent.Init();
	_disconnectEvent.owner = shared_from_this(); // add_ref

	if (false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0)) {
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING) {
			_disconnectEvent.owner = nullptr; // release_ref
			return false;
		}
	}

	return true;
}

void Session::RegisterRecv() {
	if (IsConnected() == false)
		return;

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this();

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr)) {
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING) {
			HandleError(errorCode);
			_recvEvent.owner = nullptr; // RELEASE_REF
		}
	}
}

void Session::RegisterSend() {

	if (IsConnected() == false)
		return;

	_sendEvent.Init();
	_sendEvent.owner = shared_from_this(); // add_ref

	// 보낼 데이터를 sendEvent에 등록
	{
		WRITE_LOCK;
		int32 writeSize = 0;
		while (_sendQueue.empty() == false) {
			SendBufferRef sendBuffer = _sendQueue.front();

			writeSize += sendBuffer->WriteSize();
			// TODO: 예외 체크

			_sendQueue.pop();
			_sendEvent.sendBuffers.push_back(sendBuffer);
		}
	}

	// Scatter-Gather (흩어져 있는 데이터들을 모아서 한방에 보낸다.)
	Vector<WSABUF> wsaBufs;
	wsaBufs.reserve(_sendEvent.sendBuffers.size());
	for (SendBufferRef sendBuffer : _sendEvent.sendBuffers) {
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	// WSASEND는 순서 보장 안된다.
	if (SOCKET_ERROR == ::WSASend(
		_socket, 
		wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), 
		OUT &numOfBytes, 0, &_sendEvent, nullptr)
		) {
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING) {
			HandleError(errorCode);
			_sendEvent.owner = nullptr; // RELEASE_REF
			_sendEvent.sendBuffers.clear();
			_sendRegistered.store(false);
		}
	}
}

void Session::ProcessConnect() {

	_connectEvent.owner = nullptr; // release_ref

	// 세션 등록
	_connected.store(true);
	GetService()->AddSession(GetSessionRef());

	// 컨텐츠 코드에서 오버로딩
	OnConnected();

	// 수신 등록
	RegisterRecv();
}

void Session::ProcessDisconnect() {

	_disconnectEvent.owner = nullptr; // release_ref

	OnDisconnected(); // 컨텐츠 코드에서 오버라이드
	GetService()->ReleaseSession(GetSessionRef());
}

void Session::ProcessRecv(int32 numOfBytes) {
	
	// 명확하게 owner의 생명주기를 명시하기 위해 
	// 생성자에 넣어도 되긴함.
	_recvEvent.owner = nullptr; // RELEASE_REF

	if (numOfBytes == 0) {
		Disconnect(L"Recv 0");
		return;
	}

	// TODO
	//cout << "Recv Data Len = " << numOfBytes << endl;

	if (_recvBuffer.OnWrite(numOfBytes) == false) {
		Disconnect(L"OnWrite Overflow");
		return;
	}
	
	// 받은 데이터 (dataSize)
	int32 dataSize = _recvBuffer.DataSize();
	// 처리한 데이터의 갯수 (processLen)
	int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);
	if (processLen < 0 || 
		dataSize < processLen || 
		_recvBuffer.OnRead(processLen) == false) {
		Disconnect(L"OnRead Overflow");
		return;
	}

	// 커서 정리
	_recvBuffer.Clean();

	// 수신 등록
	RegisterRecv();
}

void Session::ProcessSend(int32 numOfBytes) {

	_sendEvent.owner = nullptr; // RELEASE_REF
	_sendEvent.sendBuffers.clear(); // release_ref

	if (numOfBytes == 0) {
		Disconnect(L"Send 0");
		return;
	}

	// 컨텐츠 코드에서 오버로딩
	OnSend(numOfBytes);

	if (_sendQueue.empty())
		_sendRegistered.store(false);
	else
		RegisterSend();
}

void Session::HandleError(int32 errorCode) {
	switch (errorCode) {
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		// TODO Log
		// 전문 스레드에 넘기는 경우도 있음
		cout << "Handle error: " << errorCode << endl;
		break;
	}
}




PacketSession::PacketSession() {
}

PacketSession::~PacketSession() {
}

// [size(2)][id(2)][data....][size(2)][id(2)][data....][size(2)][id(2)][data....]
int32 PacketSession::OnRecv(BYTE* buffer, int32 len) {

	int32 processLen = 0;
	while (true) {
		int32 dataSize = len - processLen;
		// 최소한 헤더는 파싱할 수 있어야 함.
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		// 헤더에 기록된 패킷 크기를 파싱할 수 있어야 함
		if (dataSize < header.size)
			break;

		// 패킷 조립 성공
		OnRecvPacket(&buffer[processLen], header.size);
		processLen += header.size;
	}

	return processLen;
}
