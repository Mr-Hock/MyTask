
// MyTaskDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyTask.h"
#include "MyTaskDlg.h"
#include "afxdialogex.h"
#include "MyHead.h"

#include "MyMoudleDlg.h"
#include "MyThreadDlg.h"
#include "MyWindowsDlg.h"
#include "MyFileDlg.h"
#include "MyClearFileDlg.h"
#include "MyHeapDlg.h"
#include <windows.h>

DWORD g_Pid = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMyTaskDlg::CMyTaskDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MYTASK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TASK, m_List_Task);
	DDX_Control(pDX, IDC_RADIO1, m_Button_Process);
}

BEGIN_MESSAGE_MAP(CMyTaskDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_TASK, &CMyTaskDlg::OnRclickListTask)
	ON_WM_TIMER()
	ON_COMMAND(ID_ENDPROCESS, &CMyTaskDlg::OnEndprocess)
	ON_COMMAND(ID_ENUMMOUDLE, &CMyTaskDlg::OnEnummoudle)
	ON_COMMAND(ID_OPENFILE, &CMyTaskDlg::OnOpenfile)
	ON_COMMAND(ID_SUSPENPROCESS, &CMyTaskDlg::OnSuspenprocess)
	ON_COMMAND(ID_RESUMEPROCESS, &CMyTaskDlg::OnResumeprocess)
	ON_COMMAND(ID_ENUMTHREAD, &CMyTaskDlg::OnEnumthread)
	ON_BN_CLICKED(IDC_RADIO2, &CMyTaskDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CMyTaskDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CMyTaskDlg::OnBnClickedRadio4)
	ON_COMMAND(ID_TOP, &CMyTaskDlg::OnTop)
	ON_UPDATE_COMMAND_UI(ID_TOP, &CMyTaskDlg::OnUpdateTop)
	ON_COMMAND(ID_GUANGJI, &CMyTaskDlg::OnGuangji)
	ON_COMMAND(ID_CHONGQI, &CMyTaskDlg::OnChongqi)
	ON_COMMAND(ID_ZHUXIAO, &CMyTaskDlg::OnZhuxiao)
	ON_WM_HOTKEY()
	ON_BN_CLICKED(IDC_RADIO1, &CMyTaskDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO5, &CMyTaskDlg::OnBnClickedRadio5)
	ON_COMMAND(ID_FILEINFO, &CMyTaskDlg::OnFileinfo)
	ON_COMMAND(ID_END, &CMyTaskDlg::OnEnd)
	ON_WM_HOTKEY()
END_MESSAGE_MAP()



void PrintTaskList(ClistCtrlEx &m_List_Task,TASKLIST &nTaskList, DWORD nTaskNum)
{
	HIMAGELIST nHiml = { 0 };//ͼ���б�
	SHFILEINFO nPsfi = { 0 };//�ļ���Ϣ
	ImageList_Destroy(nHiml);//���ͼ���б�
	nHiml = ImageList_Create(16, 16, ILC_COLOR32, 0, 0);//����ͼ����
	ImageList_SetBkColor(nHiml, m_List_Task.GetBkColor());//����ͼ���б��ɫ
	m_List_Task.SendMessage(LVM_SETIMAGELIST, 1, (LPARAM)nHiml);//���ó����б���ʾͼ��


	for (DWORD i = 0; i < nTaskNum; i++)
	{
		DWORD nIndex = m_List_Task.GetItemCount();

		DWORD nFileAttributes = SHGFI_SYSICONINDEX | SHGFI_SMALLICON;//��ȡ�ļ���Ϣ
		if (!SHGetFileInfo(nTaskList.szExeFilePath[nIndex], nFileAttributes, &nPsfi, sizeof(SHFILEINFO), SHGFI_ICON))
			 SHGetFileInfo(nTaskList.szExeFilePath[nIndex], nFileAttributes, &nPsfi, sizeof(SHFILEINFO), SHGFI_ICON| SHGFI_USEFILEATTRIBUTES| SHGFI_SMALLICON);
		DWORD nIco = ImageList_AddIcon(nHiml, nPsfi.hIcon);//����ͼ�귵��ͼ���±�

		m_List_Task.InsertItem(nIndex, nTaskList.szExeFile[nIndex],nIco);
		m_List_Task.SetItemText(nIndex, 1, nTaskList.th32ProcessID[nIndex]);
		m_List_Task.SetItemText(nIndex, 2, nTaskList.szPriority[nIndex]);
		m_List_Task.SetItemText(nIndex, 3, nTaskList.WorkingSetSize[nIndex]);
		m_List_Task.SetItemText(nIndex, 4, nTaskList.cntThreads[nIndex]);
		m_List_Task.SetItemText(nIndex, 5, nTaskList.th32ParentProcessID[nIndex]);
		m_List_Task.SetItemText(nIndex, 6, nTaskList.szExeFilePath[nIndex]);
	}

}
void RefershTaskList(ClistCtrlEx &m_List_Task, TASKLIST &nTaskList, DWORD nTaskNum)
{
	static HIMAGELIST nHiml = { 0 };//ͼ���б�
	static SHFILEINFO nPsfi = { 0 };//�ļ���Ϣ
	//ImageList_Destroy(nHiml);//���ͼ���б�
	static BOOL nOn = FALSE;
	if (!nOn)
	{
		nOn = !nOn;
		nHiml = ImageList_Create(16, 16, ILC_COLOR32, 0, 0);//����ͼ����
		ImageList_SetBkColor(nHiml, m_List_Task.GetBkColor());//����ͼ���б��ɫ
		m_List_Task.SendMessage(LVM_SETIMAGELIST, 1, (LPARAM)nHiml);//���ó����б���ʾͼ��
	}

	DWORD nListNum = m_List_Task.GetItemCount();

	if (nListNum > nTaskNum)
	{
		DWORD nSub = nListNum - nTaskNum;

		for (DWORD i = 0; i < nSub; i++)
		{
			m_List_Task.DeleteItem(m_List_Task.GetItemCount()-1);
		}
	}
	else if(nTaskNum > nListNum)
	{
		DWORD nAdd = nTaskNum - nListNum;
		for (DWORD i = 0; i < nAdd; i++)
		{
			m_List_Task.InsertItem(m_List_Task.GetItemCount(),L"");
		}
	}

	ImageList_SetImageCount(nHiml, nListNum);

	DWORD nIndex = 0;

	for (DWORD i = 0; i < nTaskNum; i++)
	{
		DWORD nFileAttributes = SHGFI_SYSICONINDEX | SHGFI_SMALLICON;//��ȡ�ļ���Ϣ
		if (!SHGetFileInfo(nTaskList.szExeFilePath[nIndex], nFileAttributes, &nPsfi, sizeof(SHFILEINFO), SHGFI_ICON))
			SHGetFileInfo(nTaskList.szExeFilePath[nIndex], nFileAttributes, &nPsfi, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON);
		//DWORD nIco = ImageList_AddIcon(nHiml, nPsfi.hIcon);//����ͼ�귵��ͼ���±�
		ImageList_ReplaceIcon(nHiml,i, nPsfi.hIcon);
		DestroyIcon(nPsfi.hIcon);

		m_List_Task.SetItem(nIndex, 0,2,TEXT(""),i,0,0,0);
		m_List_Task.SetItemText(nIndex, 0, nTaskList.szExeFile[nIndex]);
		m_List_Task.SetItemText(nIndex, 1, nTaskList.th32ProcessID[nIndex]);
		m_List_Task.SetItemText(nIndex, 2, nTaskList.szPriority[nIndex]);
		m_List_Task.SetItemText(nIndex, 3, nTaskList.WorkingSetSize[nIndex]);
		m_List_Task.SetItemText(nIndex, 4, nTaskList.cntThreads[nIndex]);
		m_List_Task.SetItemText(nIndex, 5, nTaskList.th32ParentProcessID[nIndex]);
		m_List_Task.SetItemText(nIndex, 6, nTaskList.szExeFilePath[nIndex]);

		nIndex++;
	}

}

void CMyTaskDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_REFRESHTASKLIST)
	{
		
/*
		TASKLIST nTaskList = {}; 
		DWORD nNum = EnumTaskList(nTaskList);
		ClearTaskList(nTaskList);

		CString nStr;
		nStr.Format(TEXT("��������%d"), nNum);
		m_Status.SetPaneText(0, nStr);
		GetCpuState(nStr);
		nStr.Format(TEXT("CPU ʹ���ʣ�%s%%"), nStr);
		m_Status.SetPaneText(1, nStr);
		nNum = GetMemState();
		nStr.Format(TEXT("�����ڴ棺%d%%"), nNum);
		m_Status.SetPaneText(2, nStr);*/

	}

	CDialogEx::OnTimer(nIDEvent);
}

//ˢ�½���
void CMyTaskDlg::Fun()
{
	TASKLIST nTaskList = {};
	DWORD nNum = EnumTaskList(nTaskList);
	RefershTaskList(m_List_Task, nTaskList, nNum);
	ClearTaskList(nTaskList);

}
DWORD WINAPI RefershThread(LPVOID lpThreadParameter)
{
	CMyTaskDlg *nDlg = (CMyTaskDlg*)lpThreadParameter;
	while (true)
	{
		//ˢ�½���
		nDlg->Fun();

		//ˢ��״̬��
		TASKLIST nTaskList = {};
		DWORD nNum = EnumTaskList(nTaskList);
		ClearTaskList(nTaskList);

		CString nStr;
		nStr.Format(TEXT("��������%d"), nNum);
		nDlg->m_Status.SetPaneText(0, nStr);
		GetCpuState(nStr);
		nStr.Format(TEXT("CPU ʹ���ʣ�%s%%"), nStr);
		nDlg->m_Status.SetPaneText(1, nStr);
		nNum = GetMemState();
		nStr.Format(TEXT("�����ڴ棺%d%%"), nNum);
		nDlg->m_Status.SetPaneText(2, nStr);
		Sleep(1000);
	}
}

DWORD WINAPI MyHotKey(LPVOID lpThreadParameter)
{
	HWND nHwnd= (HWND)lpThreadParameter;
	while (true)
	{
		//F12
		if (GetKey(123)) 
		{ 
			ShowWindow(nHwnd, !IsWindowVisible(nHwnd));
			SetForegroundWindow(nHwnd);
			Sleep(300);
		}
		Sleep(10);
	}
}
void CMyTaskDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);

	if (nHotKeyId == m_HotKey && nKey1 == (MOD_SHIFT | MOD_CONTROL) && nKey2 == 'H')
	{
		ShowWindow(!IsWindowVisible());
		SetForegroundWindow();
	}
}


BOOL CMyTaskDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��


	m_Menu.LoadMenu(IDR_MENU1);
	SetMenu(&m_Menu);

	m_Status.Create(this);
	UINT nSatusId[] = { STATUS_0,STATUS_1,STATUS_2 ,STATUS_3 };
	m_Status.SetIndicators(nSatusId, _countof(nSatusId));
	m_Status.SetPaneInfo(0, STATUS_0, SBPS_NORMAL, 120);
	m_Status.SetPaneInfo(1, STATUS_1, SBPS_NORMAL, 120);
	m_Status.SetPaneInfo(2, STATUS_2, SBPS_NORMAL, 300);
	m_Status.SetPaneInfo(3, STATUS_3, SBPS_NORMAL, 10000);

	m_List_Task.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_Task.InsertColumn(0, TEXT("��������"),0,150);
	m_List_Task.InsertColumn(1, TEXT("����PID"), 0, 60);
	m_List_Task.InsertColumn(2, TEXT("���ȼ�"), 0, 60);
	m_List_Task.InsertColumn(3, TEXT("�ڴ�ʹ��"), 0, 80);
	m_List_Task.InsertColumn(4, TEXT("�߳�����"), 0, 60);
	m_List_Task.InsertColumn(5, TEXT("������PID"), 0, 80);
	m_List_Task.InsertColumn(6, TEXT("����·��"), 0, 390);
	m_Button_Process.SetCheck(1);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);


	MyRtlAdjustPrivilege();//����Ȩ��

	TASKLIST nTaskList = {};
	DWORD nNum = EnumTaskList(nTaskList);//ö�ٽ����б�
	PrintTaskList(m_List_Task, nTaskList, nNum);//��ӡ���̵��б�
	ClearTaskList(nTaskList);

	//SetTimer(TIMER_REFRESHTASKLIST,1000, NULL);//���ö�ʱˢ��

	CString nStr;
	nStr.Format(TEXT("��������%d"), nNum);
	m_Status.SetPaneText(0, nStr);
	nStr.Format(TEXT("CPU ʹ���ʣ�%d%%"), 0);
	m_Status.SetPaneText(1, nStr);
	nNum = GetMemState();
	nStr.Format(TEXT("�����ڴ棺%d%%"), nNum);
	m_Status.SetPaneText(2, nStr);
	m_Status.SetPaneText(3, TEXT("�ϰ��:F12 | Ctrl+Shift+H"));

	
	CreateThread(NULL, NULL, RefershThread, (LPVOID)this, NULL, NULL);
	CreateThread(NULL, NULL, MyHotKey, (LPVOID)m_hWnd, NULL, NULL);
	RegisterHotKey(m_hWnd, m_HotKey,MOD_SHIFT|MOD_CONTROL,'H');

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE


}


void CMyTaskDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CMyTaskDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//�Ҽ������˵�
void CMyTaskDlg::OnRclickListTask(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	if (m_List_Task.GetSelectionMark()!=-1)
	{
		CMenu *nMenu = m_Menu.GetSubMenu(2);
		POINT pos;
		GetCursorPos(&pos);
		nMenu->TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this);
	}
}

//��������
void CMyTaskDlg::OnEndprocess()
{
	DWORD nNow = m_List_Task.GetSelectionMark();
	CString szExeName = m_List_Task.GetItemText(nNow, 0);
	CString szPid = m_List_Task.GetItemText(nNow, 1);

	DWORD nPid;
	_stscanf_s(szPid,TEXT("%d"), &nPid);
	EndProcess(nPid);
}

//�鿴ģ��
void CMyTaskDlg::OnEnummoudle()
{
	DWORD nNow = m_List_Task.GetSelectionMark();
	CString szPid = m_List_Task.GetItemText(nNow, 1);
	_stscanf_s(szPid, TEXT("%d"), &g_Pid);

	MyMoudleDlg nMoudleDlg;
	nMoudleDlg.DoModal();
}

//�鿴�߳�
void CMyTaskDlg::OnEnumthread()
{
	DWORD nNow = m_List_Task.GetSelectionMark();
	CString szPid = m_List_Task.GetItemText(nNow, 1);
	_stscanf_s(szPid, TEXT("%d"), &g_Pid);

	MyThreadDlg nThreadDlg;
	nThreadDlg.DoModal();
}

//���ļ�λ��
void CMyTaskDlg::OnOpenfile()
{
	DWORD nNow = m_List_Task.GetSelectionMark();

	CString szExeName = m_List_Task.GetItemText(nNow, 6);
	OpenFile(szExeName);
}

//��ͣ����
void CMyTaskDlg::OnSuspenprocess()
{
	DWORD nNow = m_List_Task.GetSelectionMark();
	CString szPid = m_List_Task.GetItemText(nNow, 1);
	DWORD nPid;
	_stscanf_s(szPid, TEXT("%d"), &nPid);
	SetProcessState(nPid,TRUE);
}

//�ָ�����
void CMyTaskDlg::OnResumeprocess()
{
	DWORD nNow = m_List_Task.GetSelectionMark();
	CString szPid = m_List_Task.GetItemText(nNow, 1);
	DWORD nPid;
	_stscanf_s(szPid, TEXT("%d"), &nPid);
	SetProcessState(nPid, FALSE);
}

//����
void CMyTaskDlg::OnFileinfo()
{
	DWORD nNow = m_List_Task.GetSelectionMark();
	CString szExeName = m_List_Task.GetItemText(nNow, 6); 
	OpenFileInfo(this->GetSafeHwnd(),szExeName);
	// TODO: �ڴ���������������
}

//ǰ����ʾ
void CMyTaskDlg::OnTop()
{

	CRect rcMy;
	GetWindowRect(rcMy); //�õ���ǰ�Ĵ���λ��

	m_CheckMenu = !m_CheckMenu;

	if (m_CheckMenu)
	{	//��Ϊ������ǰ
		SetWindowPos(&wndTopMost, rcMy.left, rcMy.top, rcMy.Size().cx, rcMy.Size().cy, SWP_SHOWWINDOW);
	}
	else
	{
		//ȡ��������ǰ
		SetWindowPos(&wndNoTopMost, rcMy.left, rcMy.top, rcMy.Size().cx, rcMy.Size().cy, SWP_SHOWWINDOW);
	}
}


void CMyTaskDlg::OnUpdateTop(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(TRUE);


	// TODO: �ڴ������������û����洦��������
}


//�ػ�
void CMyTaskDlg::OnGuangji()
{
	ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 65535);
	// TODO: �ڴ���������������
}


//����
void CMyTaskDlg::OnChongqi()
{
	ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 65535);
	// TODO: �ڴ���������������
}


//ע��
void CMyTaskDlg::OnZhuxiao()
{
	ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, 65535);
	// TODO: �ڴ���������������
}

//�˳����������
void CMyTaskDlg::OnEnd()
{
	PostQuitMessage(0);
}


//**********************************��ʾ�����б�**********************************
void CMyTaskDlg::OnBnClickedRadio1()
{
	m_List_Task.ShowWindow(SW_SHOW);
	if (m_PWindowsDlg != nullptr)m_PWindowsDlg->ShowWindow(SW_HIDE);
	if (m_PFileDlg != nullptr)m_PFileDlg->ShowWindow(SW_HIDE);
	if (m_PClearFileDlg != nullptr)m_PClearFileDlg->ShowWindow(SW_HIDE);
	if (m_PHeapDlg != nullptr)m_PHeapDlg->ShowWindow(SW_HIDE);
}


//**********************************��ʾ������Ϣ**********************************
void CMyTaskDlg::OnBnClickedRadio2()
{

	if (m_PWindowsDlg==nullptr)
	{
		CRect nWindow;
		m_PWindowsDlg = new MyWindowsDlg;
		m_PWindowsDlg->SetParent(this);
		m_PWindowsDlg->GetClientRect(nWindow);
		nWindow.top = 33;
		nWindow.left = 11;
		m_PWindowsDlg->MoveWindow(nWindow);
	}
	else m_PWindowsDlg->PrintWindowsList();

	m_PWindowsDlg->ShowWindow(SW_SHOW);
	if (m_PFileDlg != nullptr)m_PFileDlg->ShowWindow(SW_HIDE);
	if (m_PClearFileDlg != nullptr)m_PClearFileDlg->ShowWindow(SW_HIDE);
	if (m_PHeapDlg != nullptr)m_PHeapDlg->ShowWindow(SW_HIDE);
	m_List_Task.ShowWindow(SW_HIDE);

}


//**********************************��ʾ�ļ���Ϣ**********************************
void CMyTaskDlg::OnBnClickedRadio3()
{
	if (m_PFileDlg == nullptr)
	{
		CRect nFile;
		m_PFileDlg = new MyFileDlg;
		m_PFileDlg->SetParent(this);
		m_PFileDlg->GetClientRect(nFile);
		nFile.top = 33;
		nFile.left = 11;
		m_PFileDlg->MoveWindow(nFile);
	}
	m_PFileDlg->ShowWindow(SW_SHOW);
	if (m_PWindowsDlg != nullptr)m_PWindowsDlg->ShowWindow(SW_HIDE);
	if (m_PClearFileDlg != nullptr)m_PClearFileDlg->ShowWindow(SW_HIDE);
	if (m_PHeapDlg != nullptr)m_PHeapDlg->ShowWindow(SW_HIDE);
	m_List_Task.ShowWindow(SW_HIDE);


}


//**********************************��ʾ�ļ�����**********************************
void CMyTaskDlg::OnBnClickedRadio4()
{
	if (m_PClearFileDlg == nullptr)
	{
		CRect nClearFile;
		m_PClearFileDlg = new MyClearFileDlg;
		m_PClearFileDlg->SetParent(this);
		m_PClearFileDlg->GetClientRect(nClearFile);
		nClearFile.top = 33;
		nClearFile.left = 11;
		m_PClearFileDlg->MoveWindow(nClearFile);
	}

	m_PClearFileDlg->ShowWindow(SW_SHOW);

	if (m_PWindowsDlg != nullptr)m_PWindowsDlg->ShowWindow(SW_HIDE);
	if (m_PFileDlg != nullptr)m_PFileDlg->ShowWindow(SW_HIDE);
	if (m_PHeapDlg != nullptr)m_PHeapDlg->ShowWindow(SW_HIDE);
	m_List_Task.ShowWindow(SW_HIDE);
}


//**********************************��ʾ���б�**********************************
void CMyTaskDlg::OnBnClickedRadio5()
{
	if (m_PHeapDlg == nullptr)
	{
		CRect nHeap;
		m_PHeapDlg = new MyHeapDlg;
		m_PHeapDlg->SetParent(this);
		m_PHeapDlg->GetClientRect(nHeap);
		nHeap.top = 33;
		nHeap.left = 11;
		m_PHeapDlg->MoveWindow(nHeap);
	}
	else m_PHeapDlg->Fun();

	m_PHeapDlg->ShowWindow(SW_SHOW);

	if (m_PWindowsDlg != nullptr)m_PWindowsDlg->ShowWindow(SW_HIDE);
	if (m_PFileDlg != nullptr)m_PFileDlg->ShowWindow(SW_HIDE);
	if (m_PClearFileDlg != nullptr)m_PClearFileDlg->ShowWindow(SW_HIDE);
	m_List_Task.ShowWindow(SW_HIDE);
}



