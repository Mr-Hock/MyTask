// MyFileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTask.h"
#include "MyFileDlg.h"
#include "afxdialogex.h"


// MyFileDlg 对话框

IMPLEMENT_DYNAMIC(MyFileDlg, CDialogEx)

MyFileDlg::MyFileDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FILEINFO_DIALOG, pParent)
{
	Create(IDD_FILEINFO_DIALOG);
}

MyFileDlg::~MyFileDlg()
{
}

void MyFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILE, m_List_File);
}


BEGIN_MESSAGE_MAP(MyFileDlg, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE, &MyFileDlg::OnDblclkListFile)
END_MESSAGE_MAP()


void PrintFile(ClistCtrlEx &m_List_File, CString &dir)
{
	m_List_File.DeleteAllItems();


	if (dir == "" || dir == "计算机")
	{
		TCHAR nDrive[MAX_PATH];
		GetLogicalDriveStrings(100, (LPWSTR)nDrive);
		TCHAR* pName = nDrive;
		while (*pName != 0)
		{
			DWORD nIndex = m_List_File.GetItemCount();
			CString nList = pName;
			DWORD dwType = GetDriveType(pName);
			/*
			switch (dwType)
			{
			case DRIVE_UNKNOWN: nList = nList + TEXT("未知");				break;
			case DRIVE_NO_ROOT_DIR: nList = nList + TEXT("驱动器的根路径无效");  break;
			case DRIVE_REMOVABLE: nList = nList + TEXT("可移动介质类驱动器");	 	break;
			case DRIVE_FIXED: nList = nList + TEXT("不可移除的驱动器");		  	break;
			case DRIVE_REMOTE: nList = nList + TEXT("网络驱动器");			    break;
			case DRIVE_CDROM: nList = nList + TEXT("CD - ROM驱动器");		 	break;
			case DRIVE_RAMDISK: nList = nList + TEXT("RAM驱动器");			  	break;
			}*/
			m_List_File.InsertItem(nIndex, nList);
			pName += _tcslen(pName) + 1;
		}
	}

	else {

		CString nFilename;
		WIN32_FIND_DATA fData = { 0 };
		HANDLE hFind = INVALID_HANDLE_VALUE;
		hFind = FindFirstFile(dir + "\\*", &fData);
		if (hFind == INVALID_HANDLE_VALUE) {
			m_List_File.InsertItem(0, TEXT("无文件！"));
			return;
		}

		do {
			if (CString(fData.cFileName) == TEXT(".."))
			{
				CString nTempName = dir;
				int i = nTempName.ReverseFind('\\');
				nTempName.GetBufferSetLength(i + 1);

				CString nTempName2;
				nTempName2 = m_List_File.GetItemText(0, 0);
				if (nTempName == nTempName2)continue;

				DWORD nIndex = m_List_File.GetItemCount();
				m_List_File.InsertItem(nIndex, nTempName);
				continue;
			}
			else if (CString(fData.cFileName) == TEXT("."))
			{
				CString nTempName = dir;
				int i = nTempName.Find('\\');
				nTempName.GetBufferSetLength(i + 1);
				DWORD nIndex = m_List_File.GetItemCount();
				m_List_File.InsertItem(nIndex, nTempName);
				continue;
			}

			nFilename = dir + TEXT("\\");
			nFilename = nFilename + fData.cFileName;
			if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				nFilename = nFilename + TEXT("\\");


			DWORD nIndex = m_List_File.GetItemCount();
			m_List_File.InsertItem(nIndex, nFilename);
			m_List_File.SetItemText(nIndex, 1, fData.ftCreationTime);
			m_List_File.SetItemText(nIndex, 2, fData.ftLastWriteTime);
			CString nFileSize;
			if (fData.nFileSizeLow<1024) nFileSize.Format(TEXT("%d B"), fData.nFileSizeLow);
			else nFileSize.Format(TEXT("%d Kb"), (DWORD)(fData.nFileSizeLow / 1024.0));
			if(fData.nFileSizeLow) m_List_File.SetItemText(nIndex, 3, nFileSize);

		} while (FindNextFile(hFind, &fData));

	}

	CString nTempName;
	nTempName = m_List_File.GetItemText(0, 0);
	if (nTempName.GetLength()>=5)m_List_File.InsertItem(0, TEXT("计算机"));

	static HIMAGELIST nHiml = { 0 };//图标列表
	static SHFILEINFO nPsfi = { 0 };//文件信息
									//ImageList_Destroy(nHiml);//清除图标列表
	static BOOL nOn = FALSE;
	if (!nOn)
	{
		nOn = !nOn;
		nHiml = ImageList_Create(32, 32, ILC_COLOR32, 0, 0);//订制图标框架
		ImageList_SetBkColor(nHiml, m_List_File.GetBkColor());//设置图标列表底色
		m_List_File.SendMessage(LVM_SETIMAGELIST, 1, (LPARAM)nHiml);//设置超级列表显示图标
	}

	DWORD nListNum = m_List_File.GetItemCount();

	ImageList_SetImageCount(nHiml, nListNum);

	for (DWORD i=0;i<nListNum;i++)
	{
		DWORD nFileAttributes = SHGFI_SYSICONINDEX | SHGFI_SMALLICON;//获取文件信息
		if (!SHGetFileInfo(m_List_File.GetItemText(i, 0), nFileAttributes, &nPsfi, sizeof(SHFILEINFO), SHGFI_ICON))
			SHGetFileInfo(m_List_File.GetItemText(i, 0), nFileAttributes, &nPsfi, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON);
		ImageList_ReplaceIcon(nHiml, i, nPsfi.hIcon);
		DestroyIcon(nPsfi.hIcon);

		m_List_File.SetItem(i, 0, 2, TEXT(""), i, 0, 0, 0);
	}



}


// MyFileDlg 消息处理程序


BOOL MyFileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_List_File.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_File.InsertColumn(0, TEXT("文件名"), 0, 480);
	m_List_File.InsertColumn(1, TEXT("创建时间"), 0, 150);
	m_List_File.InsertColumn(2, TEXT("修改时间"), 0, 150);
	m_List_File.InsertColumn(3, TEXT("文件大小"), 0, 100);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	// TODO:  在此添加额外的初始化
	CString nDir;
	PrintFile(m_List_File, nDir);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void MyFileDlg::OnDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CString nDir;
	DWORD nIndex = m_List_File.GetSelectionMark();
	if (nIndex == -1)return;
	nDir = m_List_File.GetItemText(nIndex, 0);


	TCHAR nTemp[MAX_PATH] = {};

	_tcscpy_s(nTemp, MAX_PATH, nDir);


	if (nTemp[_tcslen(nTemp) - 1] == '\\')
	{
		nTemp[_tcslen(nTemp) - 1] = 0;
		PrintFile(m_List_File, CString(nTemp));
	}
	else if (nTemp[_tcslen(nTemp) - 1] == '.')
	{
		PrintFile(m_List_File, CString(nTemp));
	}
	else if (nDir == "计算机")
	{
		PrintFile(m_List_File, nDir);
	}
	else
	{
		ShellExecute(nullptr, TEXT("open"), nTemp, nullptr, nullptr, SW_SHOW);
	}

	//PrintFile(m_List_File, nDir);
}
