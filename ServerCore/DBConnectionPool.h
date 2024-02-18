#pragma once
#ifndef __DBCONNECTIONPOOL__
#define __DBCONNECTIONPOOL__

#include "DBConnection.h"

class DBConnectionPool {
public:
	DBConnectionPool();
	~DBConnectionPool();

	bool Connect(int32 connectionCount, const WCHAR* connectionString);
	void Clear();

	// 단순히 풀에서 재사용할것이기 때문에 raw 포인터 사용
	DBConnection* Pop();
	void Push(DBConnection* connection);

private:
	USE_LOCK;
	SQLHENV _environment = SQL_NULL_HANDLE;
	Vector<DBConnection*> _connections;
};

#endif
