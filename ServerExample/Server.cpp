#include "pch.h"
#include "CorePch.h"
#include "CoreMacro.h"
#include <thread>
#include "ThreadManager.h"

#include "PlayerManager.h"
#include "AccountManager.h"
#include <memory>
#include "Memory.h"
#include "Allocator.h"
#include "LockFreeStack.h"
#include "Job.h"

#include "SocketUtils.h"
#include "Listener.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include "Room.h"
#include "DBConnectionPool.h"
#include "DBBind.h"

class GameSession2 : public PacketSession {
public:
	/* 컨텐츠 코드에서 오버로딩 */
	virtual void OnConnected() {
	}
	virtual void OnDisconnected() {
	}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override {
		cout << "OnRecv Len = " << len << endl;
		/*
		SendBufferRef sendBuffer = MakeShared<SendBuffer>(4096);
		sendBuffer->CopyData(buffer, len);
		*/

		SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
		::memcpy(sendBuffer->Buffer(), buffer, len);
		sendBuffer->Close(len);
		Send(sendBuffer);
	}
	virtual void OnSend(int32 len) override {
		cout << "Send Len = " << len << endl;
	}
};


////////////////////////////////////

enum {
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service) {

	while (true) {
		LEndTickCount = ::GetTickCount64();

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해서)
		service->GetIocpCore()->Dispatch(10);

		// 예약된 일감 처리
		ThreadManager::DistributeReservedJobs();

		// 여기말고 Main 문에 넣으면 
		// 메인 스레드가 예약된 글로벌 큐를 호출한다는 의미
		// 여기에 넣으면 스레드들에게 분배됨.
		ThreadManager::DoGlobalQueueWork();
	}
}

char sendData[] = "Hello World";

/// <summary>
///  Data Source=(localdb)\MSSQLLocalDB;Initial Catalog=ServerDB;Integrated Security=True;Connect Timeout=30;Encrypt=False;Trust Server Certificate=False;Application Intent=ReadWrite;Multi Subnet Failover=False
/// </summary>
/// <returns></returns>
int main() {

	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=ServerDB;Trusted_Connection=Yes;"));

	// create Table
	{
		auto query = L"									\
			DROP TABLE IF EXISTS [dbo].[Gold];			\
			CREATE TABLE [dbo].[Gold]					\
			(											\
				[id] INT NOT NULL PRIMARY KEY IDENTITY, \
				[gold] INT NULL,						\
				[name] NVARCHAR(50) NULL,				\
				[createDate] DATETIME NULL				\
			);";
		DBConnection* dbConn = GDBConnectionPool->Pop();
		ASSERT_CRASH(dbConn->Execute(query));
		GDBConnectionPool->Push(dbConn);
	}


	// Add Data
	for (int32 i = 0; i < 3; i++)
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();

		DBBind<3, 0> dbBind(*dbConn, L"INSERT INTO [dbo].[Gold]([gold], [name], [createDate]) VALUES(?, ?, ?)");

		int32 gold = 100;
		dbBind.BindParam(0, gold);
		WCHAR name[100] = L"루키스";
		dbBind.BindParam(1, name);
		TIMESTAMP_STRUCT ts = { 2021, 6, 5 };
		dbBind.BindParam(2, ts);

		ASSERT_CRASH(dbBind.Execute());

		/*
		// 기존에 바인딩 된 정보 날림
		dbConn->Unbind();

		// 넘길 인자 바인딩
		int32 gold = 100;
		SQLLEN len = 0;

		WCHAR name[100] = L"루키스";
		SQLLEN nameLen = 0;

		TIMESTAMP_STRUCT ts = {};
		ts.year = 2021;
		ts.month = 6;
		ts.day = 5;
		SQLLEN tsLen = 0;

		// 넘길 인자 바인딩
		ASSERT_CRASH(dbConn->BindParam(1, &gold, &len));
		ASSERT_CRASH(dbConn->BindParam(2, name, &nameLen));
		ASSERT_CRASH(dbConn->BindParam(3, &ts, &tsLen));

		// SQL 실행
		ASSERT_CRASH(dbConn->Execute(L"INSERT INTO [dbo].[Gold]([gold], [name], [createDate]) VALUES(?, ?, ?)"));
		*/

		GDBConnectionPool->Push(dbConn);
	}

	// Read
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();

		DBBind<1, 4> dbBind(*dbConn, L"SELECT id, gold, name, createDate FROM [dbo].[Gold] WHERE gold = (?)");

		int32 gold = 100;
		dbBind.BindParam(0, gold);

		int32 outId = 0;
		int32 outGold = 0;
		WCHAR outName[100];
		TIMESTAMP_STRUCT outDate = {};
		dbBind.BindCol(0, OUT outId);
		dbBind.BindCol(1, OUT outGold);
		dbBind.BindCol(2, OUT outName);
		dbBind.BindCol(3, OUT outDate);

		ASSERT_CRASH(dbBind.Execute());

		/*
		// 기존에 바인딩 된 정보 날림
		dbConn->Unbind();

		int32 gold = 100;
		SQLLEN len = 0;
		// 넘길 인자 바인딩
		ASSERT_CRASH(dbConn->BindParam(1, &gold, &len));

		int32 outId = 0;
		SQLLEN outIdLen = 0;
		ASSERT_CRASH(dbConn->BindCol(1, &outId, &outIdLen));

		int32 outGold = 0;
		SQLLEN outGoldLen = 0;
		ASSERT_CRASH(dbConn->BindCol(2, &outGold, &outGoldLen));

		WCHAR outName[100];
		SQLLEN outNameLen = 0;
		ASSERT_CRASH(dbConn->BindCol(3, outName, len32(outName), &outNameLen));

		TIMESTAMP_STRUCT outDate = {};
		SQLLEN outDateLen = 0;
		ASSERT_CRASH(dbConn->BindCol(4, &outDate, &outDateLen));

		// SQL 실행
		ASSERT_CRASH(dbConn->Execute(L"SELECT id, gold, name, createDate FROM [dbo].[Gold] WHERE gold = (?)"));
		*/

		wcout.imbue(locale("kor"));

		while (dbConn->Fetch()) {
			wcout << "Id: " << outId << " Gold : " << outGold << " Name: " << outName << endl;
			wcout << "Date : " << outDate.year << "/" << outDate.month << "/" << outDate.day << endl;
		}

		GDBConnectionPool->Push(dbConn);
	}



	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO Sessionmanager
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; ++i) {
		GThreadManager->Launch([&service]() {
			DoWorkerJob(service);
		});
	}

	DoWorkerJob(service);

	GThreadManager->Join();

	return 0;
}
