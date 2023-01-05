#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <Psapi.h>
#include <tchar.h>
#include <wchar.h>
#include <minidumpapiset.h>

// end_access
#define PAGE_NOACCESS           0x01    
#define PAGE_READONLY           0x02    
#define PAGE_READWRITE          0x04    
#define PAGE_WRITECOPY          0x08    
#define PAGE_EXECUTE            0x10    
#define PAGE_EXECUTE_READ       0x20    
#define PAGE_EXECUTE_READWRITE  0x40    
#define PAGE_EXECUTE_WRITECOPY  0x80    
#define PAGE_GUARD             0x100    
#define PAGE_NOCACHE           0x200    
#define PAGE_WRITECOMBINE      0x400    
#define PAGE_GRAPHICS_NOACCESS           0x0800    
#define PAGE_GRAPHICS_READONLY           0x1000    
#define PAGE_GRAPHICS_READWRITE          0x2000    
#define PAGE_GRAPHICS_EXECUTE            0x4000    
#define PAGE_GRAPHICS_EXECUTE_READ       0x8000    
#define PAGE_GRAPHICS_EXECUTE_READWRITE 0x10000    
#define PAGE_GRAPHICS_COHERENT          0x20000    
#define PAGE_GRAPHICS_NOCACHE           0x40000    
#define PAGE_ENCLAVE_THREAD_CONTROL 0x80000000  
#define PAGE_REVERT_TO_FILE_MAP     0x80000000  
#define PAGE_TARGETS_NO_UPDATE      0x40000000  
#define PAGE_TARGETS_INVALID        0x40000000  
#define PAGE_ENCLAVE_UNVALIDATED    0x20000000  
#define PAGE_ENCLAVE_MASK           0x10000000  
#define PAGE_ENCLAVE_DECOMMIT       (PAGE_ENCLAVE_MASK | 0) 
#define PAGE_ENCLAVE_SS_FIRST       (PAGE_ENCLAVE_MASK | 1) 
#define PAGE_ENCLAVE_SS_REST        (PAGE_ENCLAVE_MASK | 2) 
#define MEM_COMMIT                      0x00001000  
#define MEM_RESERVE                     0x00002000  
#define MEM_REPLACE_PLACEHOLDER         0x00004000  
#define MEM_RESERVE_PLACEHOLDER         0x00040000  
#define MEM_RESET                       0x00080000  
#define MEM_TOP_DOWN                    0x00100000  
#define MEM_WRITE_WATCH                 0x00200000  
#define MEM_PHYSICAL                    0x00400000  
#define MEM_ROTATE                      0x00800000  
#define MEM_DIFFERENT_IMAGE_BASE_OK     0x00800000  
#define MEM_RESET_UNDO                  0x01000000  
#define MEM_LARGE_PAGES                 0x20000000  
#define MEM_4MB_PAGES                   0x80000000  
#define MEM_64K_PAGES                   (MEM_LARGE_PAGES | MEM_PHYSICAL)  
#define MEM_UNMAP_WITH_TRANSIENT_BOOST  0x00000001  
#define MEM_COALESCE_PLACEHOLDERS       0x00000001  
#define MEM_PRESERVE_PLACEHOLDER        0x00000002  
#define MEM_DECOMMIT                    0x00004000  
#define MEM_RELEASE                     0x00008000  
#define MEM_FREE                        0x00010000  

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
				//Page Protections
				if (mbi.Protect == 1) {
					std::cout << "Protect? " << "PAGE_NO_ACCESS" << std::endl;
				}
				else if (mbi.Protect == 2) {
					std::cout << "Protect? " << "PAGE_READONLY" << std::endl;
				}
				else if (mbi.Protect == 4) {
					std::cout << "Protect? " << "PAGE_READWRITE" << std::endl;
				}
				else if (mbi.Protect == 8) {
					std::cout << "Protect? " << "PAGE_WRITECOPY" << std::endl;
				}
				//Memory State
				if (mbi.State == 4096) {
					std::cout << "State? " << "MEM_COMMIT" << std::endl;
				}
				else if (mbi.State == 65536) {
					std::cout << "State? " << "MEM_FREE" << std::endl;
				}
				else {
					std::cout << "State?" << mbi.State << std::endl;
				}
				//If memory is ok, scan to buffer
				if (mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS)
				{
					char* buffer = new char[mbi.RegionSize];

					ReadProcessMemory(proc, addr, buffer, mbi.RegionSize, nullptr);
					//std::cout << "data: " << (char)buffer << std::endl;
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