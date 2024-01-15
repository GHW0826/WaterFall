#pragma once
#ifndef _PCH_
#define _PCH_

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore\\ServerCore.lib")
#else
#pragma comment(lib, "Release\\ServerCore\\ServerCore.lib")
#endif

#include "CorePch.h"

#include <vector>
#include <stack>

#endif 
