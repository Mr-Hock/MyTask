#pragma once
#include "afxcmn.h"
#include "ClistCtrlEx.h"


// MyFileDlg �Ի���

class MyFileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyFileDlg)

public:
	MyFileDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~MyFileDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILEINFO_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	ClistCtrlEx m_List_File;
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult);
};
