/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#pragma once


// COutputBarList

enum DEBUG_TYPE {
	DEBUG_NOOP = 0,
	DEBUG_ERROR,
	DEBUG_FIND,
	DEBUG_PASS_LINE,
	DEBUG_RULE,
};

class COutputBarList : public CListCtrl
{
	DECLARE_DYNAMIC(COutputBarList)

public:
	COutputBarList();
	virtual ~COutputBarList();

protected:
	DECLARE_MESSAGE_MAP()

private:
	void FindInFindTab();
	void FindInAnalyzerTab();
	DEBUG_TYPE GetSelectedFile(CString &fileStr, CString &findStr, CString &fileOriginStr, int &intVal, int &intVal2);
	DEBUG_TYPE GetSelectedFile1(CString &fileStr, CString &findStr, CString &fileOriginStr, int &intVal1, int &intVal2);

private:
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnFindmenuClear();
	afx_msg void OnFindmenuOpen();
};


