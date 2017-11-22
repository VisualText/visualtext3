#pragma once
#include "controls\list\xtplistctrlview.h"

class ListCtrlWMenuLog :
	public CXTPListCtrl
{
public:
	int m_menuID;

	void SetMenuID(int menuID);

	ListCtrlWMenuLog(void);
	~ListCtrlWMenuLog(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLogmenuOpen();

private:
	void FindInAnalyzerTab();
	int GetSelectedFile1(CString &fileStr, CString &findStr, CString &fileOriginStr, int &intVal1, int &intVal2);

public:
	afx_msg void OnLogmenuClear();
	afx_msg void OnAnalyzelogmenuClear();
};
