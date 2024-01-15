#include "pch.h"
#include "CorePch.h"
#include "CoreMacro.h"
#include <thread>
#include "ThreadManager.h"

#include "PlayerManager.h"
#include "AccountManager.h"

int main()
{
	GThreadManager->Launch([=] {
		while (true) {
			cout << "PlayerThenAccount" << endl;
			GPlayerManager.PlayerThenAccount();
			this_thread::sleep_for(100ms);
		}
		});
	GThreadManager->Launch([=] {
		while (true) {
			cout << "AccountThenPlayer" << endl;
			GAccountManager.AccountThenPlayer();
			this_thread::sleep_for(100ms);
		}
		});

	GThreadManager->Join();
}
