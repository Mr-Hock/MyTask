#pragma once

typedef struct _TASKLIST
{
	CString *szExeFile;
	DWORD *th32ProcessID;
	DWORD *cntThreads;
	DWORD *th32ParentProcessID;
	CString *szExeFilePath;
	CString *WorkingSetSize;
	CString *szPriority;

}TASKLIST, *PTASKLIST;
typedef struct _MODULELIST
{
	CString *szModule;
	CString *szExePath;
	DWORD *modBaseAddr;
	DWORD *dwSize;
}MODULELIST, *PMOUDLELIST;
typedef struct _THREADLIST
{
	DWORD *th32ThreadID;
	DWORD *th32OwnerProcessID;
	DWORD *tpBasePri;
	CString *szExePath;
	CString *szThreadState;

}THREADLIST, *PTHREADLIST;
typedef struct _HEAPLIST
{
	DWORD *th32HeapID;
	DWORD *th32ProcessID;
	CString *szExeFile;

}HEAPLIST, *PHEAPLIST;
typedef struct _HEAPENTYR
{
	DWORD *dwAddress;//堆块的线性起始地址
	DWORD *dwBlockSize;//堆块的大小

}HEAPENTYR, *PHEAPENTYR;

void MyRtlAdjustPrivilege();
DWORD EnumTaskList(TASKLIST &nTaskList);
DWORD EnumModuleList(MODULELIST &nModuleList, DWORD nPid);
DWORD EnumThreadList(THREADLIST &nThreadList, DWORD nPid);
DWORD EnumHeapList(HEAPLIST &nHeapList);
DWORD EnumHeapEntyr(HEAPENTYR &nHeapEntry, DWORD nHeapId, DWORD nPid);
void ClearTaskList(TASKLIST &nTaskList);
void ClearModuleList(MODULELIST &nModuleList);
void ClearThreadList(THREADLIST &nThreadList);
void ClearHeapList(HEAPLIST &nHeapList);
void ClearHeapEntyr(HEAPENTYR &nHeapEntyr);
void EndProcess(DWORD nPid);
void EndThread(DWORD nThreadid);
void GetProcessFilePath(HANDLE hProcess, CString& sFilePath);
void SetProcessState(DWORD nPid, BOOL nSuspend);
void SetThreadState(DWORD nThreadid, BOOL nSuspend);
DWORD GetMemState();
void OpenFile(CString &nFileNamePath);
void GetCpuState(CString& nStr);
bool GetKey(int vk_code);
void GetProcessName(CString &nProcessName, DWORD nPid);
void GetThreadState(CString &nState, DWORD nThreadid);
void GetProcessPriority(CString &nState, HANDLE nHandle);
void OpenFileInfo(HWND nHwnd, CString &nFileRoute);

