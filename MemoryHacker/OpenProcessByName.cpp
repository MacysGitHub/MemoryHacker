#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <Psapi.h>
#include <tchar.h>
#include <wchar.h>
#include <minidumpapiset.h>


DWORD FindTargetProcess(DWORD dwProcessID, wchar_t* processName) {
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

HANDLE OpenProcessByName(wchar_t* processName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    BOOL enumbool = FALSE;
    DWORD processIds[2048] = { 0 };
    DWORD bytesneeded = 0;
    DWORD TotalProcesses = 0;
    DWORD targetProcess = 0;
    

    enumbool = K32EnumProcesses(processIds, 
        sizeof(processIds), 
        &bytesneeded);

    if (enumbool == TRUE) {
        TotalProcesses = bytesneeded / sizeof(DWORD);
        std::cout << "Total Processes Running: " << TotalProcesses << std::endl;
    }

    for (size_t i = 0; i < TotalProcesses; i++) {
        targetProcess = FindTargetProcess(processIds[i], processName);
        HANDLE proc = NULL;
        wchar_t name[MAX_PATH];
        DWORD obtainedName;

        
        //Get Process List and Open Handle to Process
        proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetProcess);
        
        if (proc != NULL) {
            std::cout << "Successfully found and opened handle to process!" << std::endl;
            obtainedName = K32GetProcessImageFileNameW(proc, name, sizeof(name));
            std::cout << "processid: " << targetProcess << std::endl;
            std::wcout << "Sanity Check: " << name << std::endl;
            HMODULE modbase = {0};
            MODULEINFO modInfo;
            MEMORY_BASIC_INFORMATION pmembi = { 0 };
            BOOL retMod;
            size_t progSize;
            retMod = K32GetModuleInformation(proc, modbase, &modInfo, sizeof(MODULEINFO));
            LPVOID entry = modInfo.EntryPoint;
            size_t pageSize;

            //Get Page Size for Program
            pageSize = VirtualQueryEx(proc, entry, &pmembi, sizeof(MEMORY_BASIC_INFORMATION));
            
            if (pageSize == 0) {
                std::cout << "Error!: " << GetLastError();
            }

            std::cout << "Region Size: " << pmembi.RegionSize;

            PROCESS_MEMORY_COUNTERS pmc;
            K32GetProcessMemoryInfo(proc, &pmc, sizeof(pmc));

           

            /*OFSTRUCT ofs = { 0 };
            HFILE hFile = OpenFile("C:\\ProcDump.bin", &ofs, OF_CREATE);
            PMINIDUMP_EXCEPTION_INFORMATION miniException = { 0 };
            PMINIDUMP_USER_STREAM_INFORMATION miniStream = { 0 };
            PMINIDUMP_CALLBACK_INFORMATION callBack = { 0 };

            BOOL dumped = MiniDumpWriteDump(proc, targetProcess, &hFile, MiniDumpWithFullMemory, miniException, miniStream, callBack);

            std::cout << "Dump success?: " << dumped << std::endl;
            if (!dumped) {
                std::cout << "MiniDump Fail: " << GetLastError() << std::endl;
            }*/
            //SYSTEM_INFO sysInfo;
            //GetSystemInfo(&sysInfo);
            //LPVOID procMem[4096];
            //std::cout << "Page Size: " << sysInfo.dwPageSize << std::endl;
            //std::cout << "protected? " << pmembi.Protect << std::endl;
            //ReadProcessMemory(proc, entry, procMem, sysInfo.dwPageSize, &progSize);
            ////Determine 
            //std::cout << "mem page size: " << pmembi.RegionSize << std::endl;
            //for (int i = 0; i < ((sysInfo.dwPageSize/8)); i++) {
            //    //std::cout << procMem[i] << std::endl;
            //    i++;
            //}
        }
    }

    //HANDLE hProcess = new HANDLE;
    //HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    //DWORD procId;
    //HMODULE hMod = { 0 };
    //LPWSTR filename = { 0 };

    //if (Process32First(snapshot, &entry) == TRUE)
    //{
    //    while (Process32Next(snapshot, &entry) == TRUE)
    //    {
    //        std::cout << "Scanning process..." << std::endl;
    //        if (_wcsicmp(entry.szExeFile, L"Notepad.exe"))
    //        {
    //            hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
    //            std::cout << "Process Id: " << entry.th32ProcessID << std::endl;
    //            //CloseHandle(hProcess); We'll close in other part of our program
    //        }
    //    }
    //}

    //CloseHandle(snapshot);

    //if (hProcess == NULL) {
    //    return INVALID_HANDLE_VALUE;
    //    CloseHandle(hProcess);
    //}
    //else {
    //    return hProcess;
    //    CloseHandle(hProcess);
    //}
    //return 0;
    return 0;
}