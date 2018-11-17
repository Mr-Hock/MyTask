#pragma once
#include "afxcmn.h"
#include "ClistCtrlEx.h"


// MyHeapDlg 对话框

class MyHeapDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MyHeapDlg)

public:
	MyHeapDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MyHeapDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HEAP_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	ClistCtrlEx m_List_Heap;
	ClistCtrlEx m_List_Heap_Info;
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListHeap(NMHDR *pNMHDR, LRESULT *pResult);
	void Fun();
};
