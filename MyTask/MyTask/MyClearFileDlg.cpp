// MyClearFileDlg.cpp : 实现文件


#include "stdafx.h"
#include "MyTask.h"
#include "MyClearFileDlg.h"
#include "afxdialogex.h"
#include <vector>
using std::vector;

vector<CString>g_FilePath;
DWORD g_FilePathSize;
vector<CString>g_DeleteFilePath;
DWORD g_DeleteFilePathSize;


IMPLEMENT_DYNAMIC(MyClearFileDlg, CDialogEx)

MyClearFileDlg::MyClearFileDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CLEARFILE_DIALOG, pParent)
{
	Create(IDD_CLEARFILE_DIALOG);
}

MyClearFileDlg::~MyClearFileDlg()
{
}

void MyClearFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLEARFILE, m_List_ClearFile);
	DDX_Control(pDX, IDC_EDIT_ROUTE, m_Edit_Route);
	DDX_Control(pDX, IDC_EDIT_NAME, m_Edit_Name);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_Edit_FileName);
	DDX_Control(pDX, IDC_EDIT_DELETE_INFO, m_Edit_DeleteInfo);
}


BEGIN_MESSAGE_MAP(MyClearFileDlg, CDialogEx)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_GETINFO, &MyClearFileDlg::OnBnClickedButtonGetinfo)
	ON_BN_CLICKED(IDC_BUTTON_START, &MyClearFileDlg::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// MyClearFileDlg 消息处理程序


BOOL MyClearFileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_List_ClearFile.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_ClearFile.InsertColumn(0, TEXT("文件路径"), 0, 300);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_Edit_Name.SetWindowText(TEXT(".obj\\.tlog\\.lastbuildstate\\.idb\\.pdb\\.pch\\.res\\.ilk\\.exe\\.sdf\\.ipch\\.log\\"));

	return TRUE;  
}


void MyClearFileDlg::OnDropFiles(HDROP hDropInfo)
{
	CDialogEx::OnDropFiles(hDropInfo);

	CString buff;
	buff.GetBufferSetLength(MAX_PATH);

	DragQueryFile(hDropInfo,0,(TCHAR*)(const TCHAR*)buff,MAX_PATH);

	//m_Edit_Route.SetWindowText(buff);
	//m_List_ClearFile.DeleteAllItems();

	DWORD nIndex = m_List_ClearFile.GetItemCount();
	m_List_ClearFile.InsertItem(nIndex, buff);

	DragFinish(hDropInfo);
}

BOOL IsFileName(CString &nTemp, CString &nFile)
{
	DWORD i = nFile.ReverseFind('.');
	nFile=nFile.Mid(i);
	if (nTemp.Find(nFile) != -1)return TRUE;
	return FALSE;
}

void PrintFile(CEdit &m_Edit_FileName,CString& dir, CString& name)
{
	// 1. 先将文件夹路径拼接成一个包含通配符的路径
	// 2. 调用FindFirstFile找到这个目录下的第一个文件/文件夹
	WIN32_FIND_DATA fData = { 0 };
	HANDLE hFind = INVALID_HANDLE_VALUE;
	hFind = FindFirstFile(dir + L"\\*", &fData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return;
	}
	// 3. 再循环调用FindNextFile得到后续所有的文件/文件夹
	do {
		if (CString(fData.cFileName) ==  L"." || CString(fData.cFileName) == L"..")
			continue;
		
		if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{
			PrintFile(m_Edit_FileName,dir + L"\\" + fData.cFileName, name);
		}
		else 
		{ 
			DWORD nLength = m_Edit_FileName.GetWindowTextLength();
			m_Edit_FileName.SetSel(-1);
			m_Edit_FileName.ReplaceSel(dir + L"\\" + fData.cFileName + L"\n");

			if (IsFileName(name, dir + L"\\" + fData.cFileName))
			{
				g_DeleteFilePath.push_back(dir + L"\\" + fData.cFileName);
				g_DeleteFilePathSize = g_DeleteFilePathSize + fData.nFileSizeLow;
			}

			g_FilePath.push_back(dir + L"\\" + fData.cFileName); 
			g_FilePathSize = g_FilePathSize + fData.nFileSizeLow;

		}

	} while (FindNextFile(hFind, &fData));

}

void MyClearFileDlg::OnBnClickedButtonGetinfo()
{
	g_DeleteFilePath.swap(vector<CString>());
	g_FilePath.swap(vector<CString>());
	g_FilePathSize = 0;
	g_DeleteFilePathSize = 0;

	CString nTemp1;
	TCHAR nTemp2[MAX_PATH];
	DWORD nCount;
	m_Edit_Name.GetWindowText(nTemp2, MAX_PATH);
	nCount = m_List_ClearFile.GetItemCount();
	if (!nCount)return;

	for (DWORD i = 0; i < nCount; i++)
	{
		nTemp1 = m_List_ClearFile.GetItemText(i, 0);
		m_Edit_Route.SetWindowText(nTemp1);
		PrintFile(m_Edit_FileName, nTemp1, CString(nTemp2));
	}
	
	CString nTemp;
	nTemp.Format(TEXT("总大小：%lfMB 文件总数：%d\n"), ((DOUBLE)g_FilePathSize / 1024.0 / 1024.0), g_FilePath.size());
	m_Edit_FileName.SetSel(-1);
	m_Edit_FileName.ReplaceSel(nTemp);

	nTemp.Format(TEXT("可删除大小：%lfMB 可删除个数：%d\n") , ((DOUBLE)g_DeleteFilePathSize / 1024.0 / 1024.0), g_DeleteFilePath.size());
	m_Edit_FileName.SetSel(-1);
	m_Edit_FileName.ReplaceSel(nTemp);
}


void MyClearFileDlg::OnBnClickedButtonStart()
{
	for (auto &val: g_DeleteFilePath)
	{
		//DeleteFile(val);
		m_Edit_DeleteInfo.SetSel(-1);
		m_Edit_DeleteInfo.ReplaceSel(val + L"\n");
	}

	m_Edit_DeleteInfo.SetSel(-1);
	m_Edit_DeleteInfo.ReplaceSel(TEXT("删除成功！\n"));
}
