#pragma once
#include "controls\list\xtplistctrlview.h"

class ListCtrlWMenu :
	public CXTPListCtrl
{
public:
	int m_menuID;

	void SetMenuID(int menuID);
	void DoFindType();

	ListCtrlWMenu(void);
	~ListCtrlWMenu(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFindmenuClear();
	afx_msg void OnFindmenuOpen();

private:
	void FindInFindTab();
	int GetSelectedFile(CString &fileStr, CString &findStr, CString &fileOriginStr, int &intVal, int &intVal2);

};
