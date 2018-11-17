// MyFileDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyTask.h"
#include "MyFileDlg.h"
#include "afxdialogex.h"


// MyFileDlg �Ի���

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


	if (dir == "" || dir == "�����")
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
			case DRIVE_UNKNOWN: nList = nList + TEXT("δ֪");				break;
			case DRIVE_NO_ROOT_DIR: nList = nList + TEXT("�������ĸ�·����Ч");  break;
			case DRIVE_REMOVABLE: nList = nList + TEXT("���ƶ�������������");	 	break;
			case DRIVE_FIXED: nList = nList + TEXT("�����Ƴ���������");		  	break;
			case DRIVE_REMOTE: nList = nList + TEXT("����������");			    break;
			case DRIVE_CDROM: nList = nList + TEXT("CD - ROM������");		 	break;
			case DRIVE_RAMDISK: nList = nList + TEXT("RAM������");			  	break;
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
			m_List_File.InsertItem(0, TEXT("���ļ���"));
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
	if (nTempName.GetLength()>=5)m_List_File.InsertItem(0, TEXT("�����"));

	static HIMAGELIST nHiml = { 0 };//ͼ���б�
	static SHFILEINFO nPsfi = { 0 };//�ļ���Ϣ
									//ImageList_Destroy(nHiml);//���ͼ���б�
	static BOOL nOn = FALSE;
	if (!nOn)
	{
		nOn = !nOn;
		nHiml = ImageList_Create(32, 32, ILC_COLOR32, 0, 0);//����ͼ����
		ImageList_SetBkColor(nHiml, m_List_File.GetBkColor());//����ͼ���б��ɫ
		m_List_File.SendMessage(LVM_SETIMAGELIST, 1, (LPARAM)nHiml);//���ó����б���ʾͼ��
	}

	DWORD nListNum = m_List_File.GetItemCount();

	ImageList_SetImageCount(nHiml, nListNum);

	for (DWORD i=0;i<nListNum;i++)
	{
		DWORD nFileAttributes = SHGFI_SYSICONINDEX | SHGFI_SMALLICON;//��ȡ�ļ���Ϣ
		if (!SHGetFileInfo(m_List_File.GetItemText(i, 0), nFileAttributes, &nPsfi, sizeof(SHFILEINFO), SHGFI_ICON))
			SHGetFileInfo(m_List_File.GetItemText(i, 0), nFileAttributes, &nPsfi, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON);
		ImageList_ReplaceIcon(nHiml, i, nPsfi.hIcon);
		DestroyIcon(nPsfi.hIcon);

		m_List_File.SetItem(i, 0, 2, TEXT(""), i, 0, 0, 0);
	}



}


// MyFileDlg ��Ϣ�������


BOOL MyFileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_List_File.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_File.InsertColumn(0, TEXT("�ļ���"), 0, 480);
	m_List_File.InsertColumn(1, TEXT("����ʱ��"), 0, 150);
	m_List_File.InsertColumn(2, TEXT("�޸�ʱ��"), 0, 150);
	m_List_File.InsertColumn(3, TEXT("�ļ���С"), 0, 100);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CString nDir;
	PrintFile(m_List_File, nDir);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
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
	else if (nDir == "�����")
	{
		PrintFile(m_List_File, nDir);
	}
	else
	{
		ShellExecute(nullptr, TEXT("open"), nTemp, nullptr, nullptr, SW_SHOW);
	}

	//PrintFile(m_List_File, nDir);
}
