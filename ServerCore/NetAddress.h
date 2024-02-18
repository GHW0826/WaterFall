#pragma once
#ifndef __NETADDRESS__
#define __NETADDRESS__

class NetAddress {
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(wstring ip, uint16 port);

	FORCEINLINE SOCKADDR_IN& GetSockAddr() { return _sockAddr; }
	FORCEINLINE uint16 GetPort() { return ::ntohs(_sockAddr.sin_port); }
	wstring GetIpAddress();
public:
	static IN_ADDR Ip2Address(const WCHAR* ip);
private:
	SOCKADDR_IN _sockAddr = {};
};


#endif