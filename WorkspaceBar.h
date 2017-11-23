/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_TABDOCKWND_H__7EB3CA66_917F_438F_984A_D4DAAC07D8A1__INCLUDED_)
#define AFX_TABDOCKWND_H__7EB3CA66_917F_438F_984A_D4DAAC07D8A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkspaceBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar window

class CWorkspaceBar : public CXTTabCtrlBar
{
// Construction
public:
	CWorkspaceBar();

// Attributes
protected:
	CImageList	m_imageList;

// Operations
public:
	void RefreshTabDisplay();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkspaceBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorkspaceBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWorkspaceBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABDOCKWND_H__7EB3CA66_917F_438F_984A_D4DAAC07D8A1__INCLUDED_)
