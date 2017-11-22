/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_LISTCTRLF_H__EAA95F04_EBED_11D1_A2FB_C2D44F7999BA__INCLUDED_)
#define AFX_LISTCTRLF_H__EAA95F04_EBED_11D1_A2FB_C2D44F7999BA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ListCtrlF.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListCtrlF window

class CListCtrlF : public CListCtrl
{
// Construction
public:
	CListCtrlF();

// Attributes
public:
	UINT m_nIDCR;
	int m_nCharCol;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlF)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListCtrlF();
	int GetCurSel(void);
	BOOL AutoLoad(UINT nID, CWnd* pParent, UINT nIDCR, int nCharCol);

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlF)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLF_H__EAA95F04_EBED_11D1_A2FB_C2D44F7999BA__INCLUDED_)
