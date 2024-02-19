#pragma once
#ifndef __FILEUTILS__
#define __FILEUTILS__

#include <vector>
#include "Types.h"

class FileUtils {
public:
	static Vector<BYTE>		ReadFile(const WCHAR* path);
	static String			Convert(string str);
};

#endif

