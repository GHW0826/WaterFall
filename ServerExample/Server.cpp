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
#include "XmlParser.h"
#include "DBSynchronizer.h"
#include "GenProcedures.h"

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

	DBConnection* dbConn = GDBConnectionPool->Pop();
	DBSynchronizer dbSync(*dbConn);
	dbSync.Synchronize(L"GameDB.xml");


	{
		WCHAR name[] = L"TEST";
		SP::InsertGold insertGold(*dbConn);
		insertGold.In_Gold(100);
		insertGold.In_Name(name);
		insertGold.In_CreateDate(TIMESTAMP_STRUCT {2020, 6, 8 });
		insertGold.Execute();
	}

	{
		SP::GetGold getGold(*dbConn);
		getGold.In_Gold(100);

		int32 id = 0;
		int32 gold = 0;
		WCHAR name[100];
		TIMESTAMP_STRUCT date;

		getGold.Out_Id(OUT id);
		getGold.Out_Gold(OUT gold);
		getGold.Out_Name(OUT name);
		getGold.Out_CreateDate(OUT date);

		getGold.Execute();

		while (getGold.Fetch())
		{
			GConsoleLogger->WriteStdOut(Color::BLUE,
				L"ID[%d] Gold[%d] Name[%s]\n", id, gold, name);
		}
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
