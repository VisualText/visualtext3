/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// CWatchBar : header file
/////////////////////////////////////////////////////////////////////////////

#ifndef __WATCHBAR_H
#define __WATCHBAR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WatchBarList.h"

/////////////////////////////////////////////////////////////////////////////
// CWatchBar class

class CWatchBar : public CXTDockWindow
{
	DECLARE_DYNAMIC(CWatchBar)

// Construction / destruction
public:
	CWatchBar();
	virtual ~CWatchBar();

	CWatchBarList	m_sheet1;
	CWatchBarList	m_sheet2;
	CWatchBarList	m_sheet3;
	CWatchBarList	m_sheet4;

// Attributes
protected:
	int				m_cy;
	CXTFlatTabCtrl	m_flatTabCtrl;
	CFont			m_sheetFont;

// Operations
public:
	BOOL CreateTabListCtrl(CWatchBarList& listCtrl, UINT nID);

    void AddWatchVar(CWatchBarList& listCtrl, LPCTSTR pszName, LPCTSTR pszVal);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchBar)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CWatchBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __WATCHBAR_H

