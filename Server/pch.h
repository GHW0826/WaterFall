#pragma once
#ifndef _PCH_
#define _PCH_

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore\\ServerCore.lib")
#else
#pragma comment(lib, "Release\\ServerCore\\ServerCore.lib")
#endif

#include "CorePch.h"

#include <vector>
#include <stack>

#endif 
