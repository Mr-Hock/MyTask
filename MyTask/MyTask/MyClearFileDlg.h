#pragma once
#include "afxcmn.h"
#include "ClistCtrlEx.h"
#include "afxwin.h"


// MyClearFileDlg 对话框

class MyClearFileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyClearFileDlg)

public:
	MyClearFileDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MyClearFileDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLEARFILE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	ClistCtrlEx m_List_ClearFile;
	virtual BOOL OnInitDialog();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CEdit m_Edit_Route;
	CEdit m_Edit_Name;
	CEdit m_Edit_FileName;
	CEdit m_Edit_DeleteInfo;
	afx_msg void OnBnClickedButtonGetinfo();
	afx_msg void OnBnClickedButtonStart();
};
