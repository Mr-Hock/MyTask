#pragma once


// ClistCtrlEx

class ClistCtrlEx : public CListCtrl
{
	DECLARE_DYNAMIC(ClistCtrlEx)

public:
	ClistCtrlEx();
	virtual ~ClistCtrlEx();

	BOOL SetItemText(_In_ int nItem, _In_ int nSubItem, _In_z_ LPCTSTR lpszText)
	{
		return CListCtrl::SetItemText(nItem,nSubItem,lpszText);
	}

	BOOL SetItemText(_In_ int nItem, _In_ int nSubItem, DWORD Dword)
	{
		CString buf;
		buf.Format(L"%d", Dword);
		return CListCtrl::SetItemText(nItem, nSubItem, buf);
	}

	BOOL SetItemText(_In_ int nItem, _In_ int nSubItem, FILETIME time)
	{
		// 将文件时间转换成本地的文件时间
		FileTimeToLocalFileTime(&time, &time);
		SYSTEMTIME st = { 0 };
		// 将文件时间转换系统时间
		CString str;
		FileTimeToSystemTime(&time, &st);
		str.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"),
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		return CListCtrl::SetItemText(nItem, nSubItem, str);
	}

	int InsertItem( int nItem,  DWORD lpszItem)
	{
		CString buf;
		buf.Format(L"%d", lpszItem);
		return 	CListCtrl::InsertItem(nItem, buf);
	}

	int InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem)
	{
		return 	CListCtrl::InsertItem(nItem, lpszItem);
	}

	int InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem, _In_ int nImage)
	{
		return 	CListCtrl::InsertItem(nItem,lpszItem,nImage);
	}
protected:
	DECLARE_MESSAGE_MAP()
};


