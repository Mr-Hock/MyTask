#include "stdafx.h"
#include <windows.h>
#include <TlHelp32.h>
#include "MyHead.h"
#include <Psapi.h>

void MyRtlAdjustPrivilege()
{
	const int SE_SHUTDOWN_PRIVILEGE = 20;
	int nAddress = 0;
	typedef int(WINAPI*RtlAdjustPrivilege)(int, bool, bool, int*);
	HMODULE module = LoadLibrary(TEXT("ntdll.dll"));
	RtlAdjustPrivilege pRt = (RtlAdjustPrivilege)GetProcAddress(module, "RtlAdjustPrivilege");
	pRt(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &nAddress);
	FreeLibrary(module);
}

DWORD EnumTaskList(TASKLIST &nTaskList)
{
	BOOL nRet;
	DWORD nIndex = 0;
	PROCESSENTRY32 nPT;
	nPT.dwSize = sizeof(PROCESSENTRY32);
	HANDLE nSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	nRet = Process32First(nSnapShot, &nPT);
	while (nRet)
	{
		nRet = Process32Next(nSnapShot, &nPT);
		nIndex++;
	}

	nTaskList.szExeFile = new CString[nIndex];
	nTaskList.th32ProcessID = new DWORD[nIndex];
	nTaskList.cntThreads = new DWORD[nIndex];
	nTaskList.th32ParentProcessID = new DWORD[nIndex];
	nTaskList.szExeFilePath = new CString[nIndex];
	nTaskList.WorkingSetSize = new CString[nIndex];
	nTaskList.szPriority = new CString[nIndex];

	nIndex = 0;
	nRet = Process32First(nSnapShot, &nPT);
	while (nRet)
	{
		HANDLE nHandle;
		CString nFilePath;
		CString nMenory;
		CString nPriority;
		PROCESS_MEMORY_COUNTERS nMen;
		nHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, nPT.th32ProcessID);
		GetProcessFilePath(nHandle, nFilePath);
		GetProcessMemoryInfo(nHandle,&nMen,sizeof(PROCESS_MEMORY_COUNTERS));
		nMenory.Format(TEXT("%d K"), nMen.WorkingSetSize / 1024);
		GetProcessPriority(nPriority, nHandle);
		CloseHandle(nHandle);

		nTaskList.szExeFile[nIndex] = nPT.szExeFile;
		nTaskList.th32ProcessID[nIndex] = nPT.th32ProcessID;
		nTaskList.cntThreads[nIndex] = nPT.cntThreads;
		nTaskList.th32ParentProcessID[nIndex] = nPT.th32ParentProcessID;
		nTaskList.szExeFilePath[nIndex] = nFilePath;
		nTaskList.WorkingSetSize[nIndex] = nMenory;
		nTaskList.szPriority[nIndex] = nPriority;

		nRet = Process32Next(nSnapShot, &nPT);
		nIndex++;
	}

	CloseHandle(nSnapShot);

	return nIndex;
}
DWORD EnumModuleList(MODULELIST &nModuleList, DWORD nPid)
{
	BOOL nRet;
	DWORD nIndex = 0;
	MODULEENTRY32 nModule;

	nModule.dwSize = sizeof(MODULEENTRY32);
	HANDLE nSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, nPid);

	nRet = Module32First(nSnapShot, &nModule);
	while (nRet)
	{
		nRet = Module32Next(nSnapShot, &nModule);
		nIndex++;
	}

	nModuleList.szModule = new CString[nIndex];
	nModuleList.szExePath = new CString[nIndex];
	nModuleList.modBaseAddr = new DWORD[nIndex];
	nModuleList.dwSize = new DWORD[nIndex];

	nIndex = 0;
	nRet = Module32First(nSnapShot, &nModule);
	while (nRet)
	{
		nModuleList.szModule[nIndex] = nModule.szModule;
		nModuleList.szExePath[nIndex] = nModule.szExePath;
		nModuleList.modBaseAddr[nIndex] = (DWORD)nModule.modBaseAddr;
		nModuleList.dwSize[nIndex] = nModule.dwSize;

		nRet = Module32Next(nSnapShot, &nModule);
		nIndex++;
	}

	CloseHandle(nSnapShot);

	return nIndex;
}
DWORD EnumThreadList(THREADLIST &nThreadList, DWORD nPid)
{
	BOOL nRet;
	DWORD nIndex = 0;
	THREADENTRY32 nThread;

	nThread.dwSize = sizeof(THREADENTRY32);
	HANDLE nSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, nPid);

	nRet = Thread32First(nSnapShot, &nThread);
	while (nRet)
	{
		if ((nThread.th32OwnerProcessID == nPid || nPid==0xABCDEF)&&nThread.th32OwnerProcessID!=0)nIndex++;
		nRet = Thread32Next(nSnapShot, &nThread);
		
	}

	nThreadList.th32ThreadID = new DWORD[nIndex];
	nThreadList.th32OwnerProcessID = new DWORD[nIndex];
	nThreadList.tpBasePri = new DWORD[nIndex];
	nThreadList.szExePath = new CString[nIndex];
	nThreadList.szThreadState = new CString[nIndex];

	nIndex = 0;
	nRet = Thread32First(nSnapShot, &nThread);
	while (nRet)
	{
		if ((nThread.th32OwnerProcessID == nPid || nPid == 0xABCDEF)&&nThread.th32OwnerProcessID != 0) 
		{
			nThreadList.th32ThreadID[nIndex] = nThread.th32ThreadID;
			nThreadList.th32OwnerProcessID[nIndex] = nThread.th32OwnerProcessID;
			nThreadList.tpBasePri[nIndex] = nThread.tpBasePri;

			CString nProcessName;
			GetProcessName(nProcessName, nThread.th32OwnerProcessID);
			nThreadList.szExePath[nIndex] = nProcessName;

			CString nThreadState;
			GetThreadState(nThreadState, nThread.th32ThreadID);
			nThreadList.szThreadState[nIndex] = nThreadState;

			nIndex++;
		}
		nRet = Thread32Next(nSnapShot, &nThread);
	}

	CloseHandle(nSnapShot);

	return nIndex;
}
DWORD EnumHeapList(HEAPLIST &nHeapList)
{
	BOOL nRet;
	DWORD nIndex = 0;
	HEAPLIST32 nHeap{};

	PROCESSENTRY32 nPT;
	nPT.dwSize = sizeof(PROCESSENTRY32);
	HANDLE nSnapShotTask = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	nRet = Process32First(nSnapShotTask, &nPT);
	while (nRet)
	{
		nHeap.dwSize = sizeof(HEAPLIST32);
		HANDLE nSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, nPT.th32ProcessID);
		nRet = Heap32ListFirst(nSnapShot, &nHeap);
		while (nRet)
		{
			nIndex++;
			nRet = Heap32ListNext(nSnapShot, &nHeap);
		}
		CloseHandle(nSnapShot);
		nRet = Process32Next(nSnapShotTask, &nPT);
	}

	nHeapList.th32HeapID = new DWORD[nIndex];
	nHeapList.th32ProcessID = new DWORD[nIndex];
	nHeapList.szExeFile = new CString[nIndex];


	nPT = { sizeof(PROCESSENTRY32) };
	nHeap = { sizeof(HEAPLIST32) };
	nIndex = 0;

	nRet = Process32First(nSnapShotTask, &nPT);
	while (nRet)
	{
		
		HANDLE nSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, nPT.th32ProcessID);
		nRet = Heap32ListFirst(nSnapShot, &nHeap);
		while (nRet)
		{
			nHeapList.th32HeapID[nIndex] = (DWORD)nHeap.th32HeapID;
			nHeapList.th32ProcessID[nIndex] = nPT.th32ProcessID;
			CString nProcessName;
			GetProcessName(nProcessName, nPT.th32ProcessID);
			nHeapList.szExeFile[nIndex] = nProcessName;

			nIndex++;
			nRet = Heap32ListNext(nSnapShot, &nHeap);
		}
		CloseHandle(nSnapShot);
		nRet = Process32Next(nSnapShotTask, &nPT);
	}

	CloseHandle(nSnapShotTask);

	return nIndex;
}
DWORD EnumHeapEntyr(HEAPENTYR &HeapEntry,DWORD nHeapId,DWORD nPid)
{

	BOOL nRet;
	DWORD nIndex = 0;
	HEAPENTRY32 nHeapEntry{};
	nHeapEntry.dwSize = sizeof(HEAPENTRY32);

	HANDLE nSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	nRet = Heap32First( &nHeapEntry, nPid, nHeapId);
	while (nRet)
	{
	nIndex++;
	nRet = Heap32Next (&nHeapEntry);
	}


	HeapEntry.dwAddress = new DWORD[nIndex];
	HeapEntry.dwBlockSize = new DWORD[nIndex];


	nIndex = 0;
	nRet = Heap32First(&nHeapEntry, nPid, nHeapId);
	while (nRet)
	{
		HeapEntry.dwAddress[nIndex] = (DWORD)nHeapEntry.dwAddress;
		HeapEntry.dwBlockSize[nIndex] = (DWORD)nHeapEntry.dwBlockSize;
		nIndex++;
		nRet = Heap32Next(&nHeapEntry);
	}

	return nIndex;
}



void ClearTaskList(TASKLIST &nTaskList)
{
	delete[]nTaskList.szExeFile;
	delete[]nTaskList.th32ProcessID;
	delete[]nTaskList.cntThreads;
	delete[]nTaskList.th32ParentProcessID;
	delete[]nTaskList.szExeFilePath;
	delete[]nTaskList.WorkingSetSize;
	delete[]nTaskList.szPriority;

}
void ClearModuleList(MODULELIST &nModuleList)
{
	delete[]nModuleList.szModule;
	delete[]nModuleList.szExePath;
	delete[]nModuleList.modBaseAddr;
	delete[]nModuleList.dwSize;

}
void ClearThreadList(THREADLIST &nThreadList)
{
	delete[]nThreadList.th32ThreadID;
	delete[]nThreadList.th32OwnerProcessID;
	delete[]nThreadList.tpBasePri;
	delete[]nThreadList.szExePath;
	delete[]nThreadList.szThreadState;
}
void ClearHeapList(HEAPLIST &nHeapList)
{
	delete[]nHeapList.th32HeapID;
	delete[]nHeapList.th32ProcessID;
	delete[]nHeapList.szExeFile;
}
void ClearHeapEntyr(HEAPENTYR &nHeapEntyr)
{
	delete[]nHeapEntyr.dwAddress;
	delete[]nHeapEntyr.dwBlockSize;

}


void EndProcess(DWORD nPid)
{
	HANDLE nHandle = OpenProcess(PROCESS_TERMINATE, FALSE, nPid);

	TerminateProcess(nHandle, 0);

	CloseHandle(nHandle);
}
void EndThread(DWORD nThreadid)
{
	HANDLE nHandle = OpenThread(PROCESS_TERMINATE, FALSE, nThreadid);

	TerminateThread(nHandle, 0);

	CloseHandle(nHandle);
}


void GetProcessFilePath(HANDLE hProcess, CString& sFilePath)
{
	sFilePath = _T("");
	TCHAR tsFileDosPath[MAX_PATH + 1];
	ZeroMemory(tsFileDosPath, sizeof(TCHAR)*(MAX_PATH + 1));
	if (0 == GetProcessImageFileName(hProcess, tsFileDosPath, MAX_PATH + 1))
	{
		return;
	}

	// 获取Logic Drive String长度
	
	SIZE_T uiLen = GetLogicalDriveStrings(0, NULL);
	if (0 == uiLen)
	{
		return;
	}

	PTSTR pLogicDriveString = new TCHAR[uiLen + 1];
	ZeroMemory(pLogicDriveString, uiLen + 1);
	uiLen = GetLogicalDriveStrings((DWORD)uiLen, pLogicDriveString);
	if (0 == uiLen)
	{
		delete[]pLogicDriveString;
		return;
	}

	TCHAR szDrive[3] = TEXT(" :");
	PTSTR pDosDriveName = new TCHAR[MAX_PATH];
	PTSTR pLogicIndex = pLogicDriveString;

	do
	{
		szDrive[0] = *pLogicIndex;
		uiLen = QueryDosDevice(szDrive, pDosDriveName, MAX_PATH);
		if (0 == uiLen)
		{
			if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
			{
				break;
			}

			delete[]pDosDriveName;
			pDosDriveName = new TCHAR[uiLen + 1];
			uiLen = QueryDosDevice(szDrive, pDosDriveName, (DWORD)uiLen + 1);
			if (0 == uiLen)
			{
				break;
			}
		}

		uiLen = _tcslen(pDosDriveName);
		if (0 == _tcsnicmp(tsFileDosPath, pDosDriveName, uiLen))
		{
			sFilePath.Format(_T("%s%s"), szDrive, tsFileDosPath + uiLen);
			break;
		}

		while (*pLogicIndex++);
	} while (*pLogicIndex);

	delete[]pLogicDriveString;
	delete[]pDosDriveName;
}

void SetProcessState(DWORD nPid, BOOL nSuspend)
{
	HANDLE nHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, nPid);

	if (nSuspend)
	{
		typedef int(WINAPI*ZwSuspendProcess)(HANDLE);
		HMODULE module = LoadLibrary(TEXT("ntdll.dll"));
		ZwSuspendProcess ZwSuspend = (ZwSuspendProcess)GetProcAddress(module, "ZwSuspendProcess");
		ZwSuspend(nHandle);
		FreeLibrary(module);
	}
	else
	{
		typedef int(WINAPI*ZwResumeProcess)(HANDLE);
		HMODULE module = LoadLibrary(TEXT("ntdll.dll"));
		ZwResumeProcess ZwResume = (ZwResumeProcess)GetProcAddress(module, "ZwResumeProcess");
		ZwResume(nHandle);
		FreeLibrary(module);
	}
	CloseHandle(nHandle);

}
void SetThreadState(DWORD nThreadid, BOOL nSuspend)
{
	HANDLE nHandle = OpenThread(PROCESS_ALL_ACCESS, FALSE, nThreadid);

	if (nSuspend)
	{
		SuspendThread(nHandle);
	}
	else
	{
		ResumeThread(nHandle);
	}
	CloseHandle(nHandle);
}
DWORD GetMemState()
{
	MEMORYSTATUSEX nStatex;

	nStatex.dwLength = sizeof(nStatex);

	GlobalMemoryStatusEx(&nStatex);

	DOUBLE nAll = (DOUBLE)nStatex.ullTotalPhys / 1048576;
	DOUBLE nHave = (DOUBLE)nStatex.ullAvailPhys / 1048576;
	return (DWORD)((nAll - nHave) / nAll * 100);
}

void OpenFile(CString &nFileNamePath)
{
	LPITEMIDLIST nItem;

	nItem = ILCreateFromPath(nFileNamePath);

	SHOpenFolderAndSelectItems(nItem, 0, 0, 0);

	ILFree(nItem);

}

void GetCpuState(CString& nStr)
{
	static LONGLONG nFree{}, nNt{}, nUser{};
	static LONGLONG nFreet{}, nNtt{}, nUsert{};
	static DOUBLE nHave{};
	GetSystemTimes((PFILETIME)&nFree, (PFILETIME)&nNt, (PFILETIME)&nUser);
	nHave = (DOUBLE)(nFree - nFreet) / (DOUBLE)(nNt - nNtt + nUser - nUsert);
	nStr.Format(L"%d", DWORD(100.0 - nHave * 100));
	nFreet = nFree;
	nNtt = nNt;
	nUsert = nUser;
}

bool GetKey(int vk_code)
{
	return (GetAsyncKeyState(vk_code) & 0x8000) ? true : false;
}

void GetProcessName(CString &nProcessName,DWORD nPid)
{
	PROCESSENTRY32 nPT;
	nPT.dwSize = sizeof(nPT);
	HANDLE nSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	BOOL nRet = Process32First(nSnapShot, &nPT);
	while (nRet)
	{
		if (nPid==nPT.th32ProcessID)
		{
			nProcessName = nPT.szExeFile;
			return;
		}
		nRet = Process32Next(nSnapShot, &nPT);
	}
}

void GetThreadState(CString &nState, DWORD nThreadid)
{
	HANDLE nHandle = OpenThread(PROCESS_ALL_ACCESS, FALSE, nThreadid);

	DWORD WaitCode;
	WaitCode = WaitForSingleObject(nHandle, 0);

	CloseHandle(nHandle);

	switch (WaitCode)
	{
	case 258:
		nState = "正在运行";
		return;
	case -1:
		nState = "已销毁";
		return;
	}
	nState = "已结束";


}

void GetProcessPriority(CString &nState, HANDLE nHandle)
{
	if ((DWORD)nHandle == -1)
	{
		nState = "无权限";
		return;
	}

	DWORD nClass;
	nClass = GetPriorityClass(nHandle);

	switch (nClass)
	{
	case 256:
		nState = "实时";
		return;
	case 128:
		nState = "最高";
		return;
	case 32768:
		nState = "高于标准";
		return;
	case 32:
		nState = "标准";
		return;
	case 16384:
		nState = "低于标准";
		return;
	case 64:
		nState = "低";
		return;
	case 0:
		nState = "最高";
		return;
	}
	nState = "无权限";
}

void OpenFileInfo(HWND nHwnd,CString &nFileRoute)
{
	SHELLEXECUTEINFO   sei;
	sei.hwnd = nHwnd;
	sei.lpVerb = TEXT("properties");
	sei.lpFile = nFileRoute;
	sei.lpDirectory = NULL;
	sei.lpParameters = NULL;
	sei.nShow = SW_SHOWNORMAL;
	sei.fMask = SEE_MASK_INVOKEIDLIST;
	sei.lpIDList = NULL;
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	ShellExecuteEx(&sei);


}
