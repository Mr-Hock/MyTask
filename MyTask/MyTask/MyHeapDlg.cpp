// MyHeapDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyTask.h"
#include "MyHeapDlg.h"
#include "afxdialogex.h"
#include "MyHead.h"


// MyHeapDlg �Ի���

IMPLEMENT_DYNAMIC(MyHeapDlg, CDialogEx)

MyHeapDlg::MyHeapDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_HEAP_DIALOG, pParent)
{
	Create(IDD_HEAP_DIALOG);
}

MyHeapDlg::~MyHeapDlg()
{
}

void MyHeapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_HEAP, m_List_Heap);
	DDX_Control(pDX, IDC_LIST_HEAP_INFO, m_List_Heap_Info);
}


BEGIN_MESSAGE_MAP(MyHeapDlg, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST_HEAP, &MyHeapDlg::OnClickListHeap)
END_MESSAGE_MAP()


// MyHeapDlg ��Ϣ�������
void PrintHeapList(ClistCtrlEx &m_List_Heap, HEAPLIST &nHeapList, DWORD nHeapListNum)
{
	m_List_Heap.DeleteAllItems();


	static HIMAGELIST nHiml = { 0 };//ͼ���б�
	static SHFILEINFO nPsfi = { 0 };//�ļ���Ϣ
									//ImageList_Destroy(nHiml);//���ͼ���б�
	static BOOL nOn = FALSE;
	if (!nOn)
	{
		nOn = !nOn;
		nHiml = ImageList_Create(16, 16, ILC_COLOR32, 0, 0);//����ͼ����
		ImageList_SetBkColor(nHiml, m_List_Heap.GetBkColor());//����ͼ���б��ɫ
		m_List_Heap.SendMessage(LVM_SETIMAGELIST, 1, (LPARAM)nHiml);//���ó����б���ʾͼ��
	}

	ImageList_SetImageCount(nHiml, nHeapListNum);



	for (DWORD i = 0; i < nHeapListNum; i++)
	{
		DWORD nIndex = m_List_Heap.GetItemCount();

		m_List_Heap.InsertItem(nIndex, nHeapList.szExeFile[nIndex]);
		m_List_Heap.SetItemText(nIndex, 1, nHeapList.th32ProcessID[nIndex]);
		m_List_Heap.SetItemText(nIndex, 2, nHeapList.th32HeapID[nIndex]);



		CString szFileRoute, szPid;
		DWORD nPid;
		HANDLE nHandle;
		szPid = m_List_Heap.GetItemText(i, 1);
		_stscanf_s(szPid, TEXT("%d"), &nPid);
		nHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, nPid);
		GetProcessFilePath(nHandle, szFileRoute);
		CloseHandle(nHandle);

		DWORD nFileAttributes = SHGFI_SYSICONINDEX | SHGFI_SMALLICON;//��ȡ�ļ���Ϣ
		if (!SHGetFileInfo(szFileRoute, nFileAttributes, &nPsfi, sizeof(SHFILEINFO), SHGFI_ICON))
			SHGetFileInfo(szFileRoute, nFileAttributes, &nPsfi, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON);
		ImageList_ReplaceIcon(nHiml, i, nPsfi.hIcon);
		DestroyIcon(nPsfi.hIcon);
		m_List_Heap.SetItem(nIndex, 0, 2, TEXT(""), i, 0, 0, 0);
	}

}


void PrintHeapEntry(ClistCtrlEx &m_List_Heap_Info, HEAPENTYR &nHeapEntry, DWORD nHeapEntryNum)
{
	m_List_Heap_Info.DeleteAllItems();

	for (DWORD i = 0; i < nHeapEntryNum; i++)
	{
		DWORD nIndex = m_List_Heap_Info.GetItemCount();
		CString szAddress;
		szAddress.Format(TEXT("%p"), nHeapEntry.dwAddress[nIndex]);
		m_List_Heap_Info.InsertItem(nIndex, szAddress);
		m_List_Heap_Info.SetItemText(nIndex, 1, nHeapEntry.dwBlockSize[nIndex]);
	}

}

void MyHeapDlg::Fun()
{
	HEAPLIST nHeapList = {};
	DWORD nNum = EnumHeapList(nHeapList);//ö�ٶ��б�
	PrintHeapList(m_List_Heap, nHeapList, nNum);//��ӡ���б�
	ClearHeapList(nHeapList);
}

BOOL MyHeapDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_List_Heap.SetExtendedStyle(LVS_EX_FULLROWSELECT );
	m_List_Heap.InsertColumn(0, TEXT("������"), 0, 200);
	m_List_Heap.InsertColumn(1, TEXT("����ID"), 0, 100); 
	m_List_Heap.InsertColumn(2, TEXT("��ID"), 0, 100);

	m_List_Heap_Info.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_Heap_Info.InsertColumn(0, TEXT("�ѿ���ʼ��ַ"), 0, 150);
	m_List_Heap_Info.InsertColumn(1, TEXT("�ѿ��С"), 0, 150);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	Fun();
	return TRUE;  
}


void MyHeapDlg::OnClickListHeap(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	DWORD nNow = m_List_Heap.GetSelectionMark();
	if (nNow != -1)
	{
		CString szHeapId, szPid;
		DWORD nHeapId, nPid;
		
		szPid = m_List_Heap.GetItemText(nNow, 1);
		szHeapId = m_List_Heap.GetItemText(nNow, 2);

		_stscanf_s(szPid, TEXT("%d"), &nPid);
		_stscanf_s(szHeapId, TEXT("%d"), &nHeapId);


		HEAPENTYR nHeapEntry = {};
		DWORD nNum = EnumHeapEntyr(nHeapEntry, nHeapId, nPid);//ö�ٶ��б�
		PrintHeapEntry(m_List_Heap_Info, nHeapEntry, nNum);//��ӡ���б�
		ClearHeapEntyr(nHeapEntry);
	}


}
