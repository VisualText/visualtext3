/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_SEMPOPUP_H__A13494EB_A748_4281_BCBA_0CE98EC671FE__INCLUDED_)
#define AFX_SEMPOPUP_H__A13494EB_A748_4281_BCBA_0CE98EC671FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SemPopup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSemPopup window
#define WMU_DESTROY_SEMPOPUP (WM_USER + 1400)

class CSemPopup : public CWnd
{
// Construction
public:
	CSemPopup();
	CSemPopup(CParseTreeItemInfo* pInfo, CTreeView* pView);

// Attributes
public:
	CTreeView* m_pView;
	CParseTreeItemInfo* m_pInfo;
	CRect m_cRCreate;
	CRect m_cRDraw;
	CRect m_cRDrawBk;
	CString m_strText;
	CFont m_Font;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSemPopup)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSemPopup();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSemPopup)
	afx_msg void OnPaint();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEMPOPUP_H__A13494EB_A748_4281_BCBA_0CE98EC671FE__INCLUDED_)
