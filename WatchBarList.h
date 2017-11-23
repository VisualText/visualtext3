/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_WATCHBARLIST_H__CA025B27_A591_499B_9F3C_ABAFBD14905C__INCLUDED_)
#define AFX_WATCHBARLIST_H__CA025B27_A591_499B_9F3C_ABAFBD14905C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WatchBarList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWatchBarList window

class CWatchBarList : public CListCtrl
{
// Construction
public:
	CWatchBarList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchBarList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWatchBarList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWatchBarList)
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATCHBARLIST_H__CA025B27_A591_499B_9F3C_ABAFBD14905C__INCLUDED_)
