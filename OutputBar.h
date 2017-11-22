/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// OutputBar.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUTBAR_H__72D0AF4C_6245_4390_BA52_23CBC02A79B9__INCLUDED_)
#define AFX_OUTPUTBAR_H__72D0AF4C_6245_4390_BA52_23CBC02A79B9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OutputBarList.h"
#include "OutputBarEdit.h"
#include "OutputBarTree.h"

/////////////////////////////////////////////////////////////////////////////
// COutputBar dock window class

class COutputBar : public CXTDockWindow
{
	DECLARE_DYNAMIC(COutputBar)

// Construction / destruction
public:
	COutputBar();
	virtual ~COutputBar();

	COutputBarList m_sheet1;
	COutputBarList m_sheet2;
	COutputBarList m_sheet3;
	COutputBarList m_sheet4;

	CXTFlatTabCtrl m_flatTabCtrl;

// Attributes
protected:
	int            m_cy;
	CFont          m_sheetFont;

// Operations
public:
	BOOL CreateTabOutputWnd(COutputBarList& wndOutput, CString column1Str, CString column2Str, UINT nID);
	BOOL CreateTabOutputWnd(COutputBarEdit& wndOutput, UINT nID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputBar)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(COutputBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTBAR_H__72D0AF4C_6245_4390_BA52_23CBC02A79B9__INCLUDED_)
