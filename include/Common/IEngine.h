#pragma once
#ifndef __IENGINE__
#define __IENGINE__

class ISessionService;

class IEngine
{
public:
	IEngine(void) {}
	virtual ~IEngine(void) {}

	virtual ISessionService* CreateSessionService(_SessionDesc& desc) = 0;

	//세션이 생성되었을 때
	// return value : 해당 클라이언트의 고유 시리얼
	virtual bool OnConnect(int Serial, _SessionDesc& desc) = 0;

	//세션이 종료되었을 때
	// Arg1 : 종료되는 클라이언트의 고유 시리얼
	virtual bool OnDisconnect(int Serial, _SessionDesc& desc) = 0;

	//타이머 이벤트가 발생할 때
	virtual bool OnTimer(const void* arg) { return false; }

	int GetMaxUserAccept()						{ return maxUserAccept_; }
	void SetMaxUserAccept(int maxUserAccept)	{ maxUserAccept_ = maxUserAccept; }
private:
	int maxUserAccept_ = 5000;
};

#endif // #ifndef __IENGINE__