#pragma once
#include <Windows.h>

class OpenProcessByName {
private:
	LPCWSTR processName;
public:
	HANDLE hProcess;
};

HANDLE OpenProcessByName(wchar_t* processName);