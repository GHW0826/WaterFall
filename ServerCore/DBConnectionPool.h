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

	// �ܼ��� Ǯ���� �����Ұ��̱� ������ raw ������ ���
	DBConnection* Pop();
	void Push(DBConnection* connection);

private:
	USE_LOCK;
	SQLHENV _environment = SQL_NULL_HANDLE;
	Vector<DBConnection*> _connections;
};

#endif
