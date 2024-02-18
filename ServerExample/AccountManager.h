#pragma once
#ifndef _ACCOUNTMANAGER_
#define _ACCOUNTMANAGER_

class AccountManager
{
	USE_LOCK;
public:
	void AccountThenPlayer();
	void Lock();
};

extern AccountManager GAccountManager;

#endif

