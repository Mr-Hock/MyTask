#pragma once
#include "afxcmn.h"
#include "ClistCtrlEx.h"


// MyFileDlg 对话框

class MyFileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyFileDlg)

public:
	MyFileDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MyFileDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILEINFO_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	ClistCtrlEx m_List_File;
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult);
};
