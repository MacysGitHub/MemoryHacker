#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <Psapi.h>
#include <tchar.h>
#include <wchar.h>
#include <minidumpapiset.h>


DWORD FindTargetProcess(DWORD dwProcessID, wchar_t* processName)
{
	HANDLE hProcess;
	BOOL bEnumProcessModule = FALSE;
	HMODULE hMod;
	DWORD bytesneeded = 0;
	wchar_t chProcessName[MAX_PATH] = L"UNKNOWN";
	wchar_t chProcessPath[MAX_PATH] = L"UNKNOWN PATH";
	hProcess = OpenProcess(PROCESS_ALL_ACCESS,
		FALSE, dwProcessID);
	if (NULL != hProcess) {
		bEnumProcessModule = K32EnumProcessModulesEx(hProcess,
			&hMod,
			sizeof(hMod),
			&bytesneeded,
			LIST_MODULES_ALL);
		if (bEnumProcessModule == TRUE) {
			K32GetModuleBaseNameW(hProcess,
				hMod,
				chProcessName,
				sizeof(chProcessName) / sizeof(wchar_t));

			//std::wcout << "Process Name: " << chProcessName << std::endl;
			if (_wcsicmp(chProcessName, processName) == 0) {
				return dwProcessID;
			}
			K32GetModuleFileNameExW(
				hProcess,
				hMod,
				chProcessPath,
				sizeof(chProcessPath) / sizeof(wchar_t));
			//std::wcout << "Process Path: " << chProcessPath << std::endl;
		}
	}
}

HANDLE OpenProcessByName(wchar_t* processName)
{
	PROCESSENTRY32 entry;
	HANDLE proc = NULL;
	entry.dwSize = sizeof(PROCESSENTRY32);
	BOOL enumbool = FALSE;
	DWORD processIds[2048] = { 0 };
	DWORD bytesneeded = 0;
	DWORD TotalProcesses = 0;
	DWORD targetProcess = 0;


	enumbool = K32EnumProcesses(processIds,
		sizeof(processIds),
		&bytesneeded);

	if (enumbool == TRUE)
	{
		TotalProcesses = bytesneeded / sizeof(DWORD);
		std::cout << "Total Processes Running: " << TotalProcesses << std::endl;
	}

	for (size_t i = 0; i < TotalProcesses; i++)
	{
		targetProcess = FindTargetProcess(processIds[i], processName);

		wchar_t name[MAX_PATH];
		DWORD obtainedName;


		//Get Process List and Open Handle to Process
		proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetProcess);

		if (proc != NULL)
		{
			std::cout << "Successfully found and opened handle to process!" << std::endl;
			obtainedName = K32GetProcessImageFileNameW(proc, name, sizeof(name));
			std::cout << "processid: " << targetProcess << std::endl;
			std::wcout << "Sanity Check: " << name << std::endl;
			HMODULE modbase = { 0 };
			MODULEINFO modInfo;
			MEMORY_BASIC_INFORMATION pmembi = { 0 };
			BOOL retMod;
			size_t progSize;
			retMod = K32GetModuleInformation(proc, modbase, &modInfo, sizeof(MODULEINFO));
			LPVOID entry = modInfo.EntryPoint;
			size_t pageSize = 0;

			//Get Page Size for Program
			/*pageSize = VirtualQueryEx(proc, entry, &pmembi, sizeof(MEMORY_BASIC_INFORMATION));

			if (pageSize == 0) {
				std::cout << "Error!: " << GetLastError() << std::endl;
			}

			std::cout << "Region Size: " << pmembi.RegionSize << std::endl;*/

			PROCESS_MEMORY_COUNTERS pmc;
			K32GetProcessMemoryInfo(proc, &pmc, sizeof(pmc));

			MEMORY_BASIC_INFORMATION meminfo;
			char* addr = 0;


			MEMORY_BASIC_INFORMATION mbi;

			char buffer[0x1000];

			while (VirtualQueryEx(proc, addr, &mbi, sizeof(mbi)))
			{

				std::cout << "Protect? " << mbi.Protect << std::endl;
				std::cout << "state? " << mbi.State << std::endl;

				if (mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS)
				{
					char* buffer = new char[mbi.RegionSize];

					ReadProcessMemory(proc, addr, buffer, mbi.RegionSize, nullptr);
					std::cout << "data: " << (char)buffer << std::endl;
				}
				addr += mbi.RegionSize;
			}

			CloseHandle(proc);

			//	HANDLE dump_file = CreateFile("dump.dmp", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
			//	if (dump_file == INVALID_HANDLE_VALUE) {
			//		std::cerr << "Error creating dump file: " << GetLastError() << std::endl;
			//	}

			//	//Create a MINIDUMP_EXCEPTION_INFORMATION structure to describe the exception that occurred, if any.
			//	MINIDUMP_EXCEPTION_INFORMATION exception_info;
			//	EXCEPTION_POINTERS ep;
			//	exception_info.ThreadId = GetCurrentThreadId();
			//	exception_info.ExceptionPointers = GetExceptionInformation();
			//	exception_info.ClientPointers = TRUE;

			//	// Call MiniDumpWriteDump to create the minidump.
			//	BOOL success = MiniDumpWriteDump(proc, targetProcess, dump_file, MiniDumpWithFullMemory, &exception_info, nullptr, nullptr);
			//	if (!success) {
			//		std::cerr << "Error writing minidump: " << GetLastError() << std::endl;

			//	}
			//	
			//	std::cout << "Minidump created successfully!" << std::endl;

			//}
			return proc;
		}
	}
}