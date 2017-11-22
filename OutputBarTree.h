/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_OUTPUTBARTREE_H__925ECC9F_1339_43E4_96B1_B4BEEB996549__INCLUDED_)
#define AFX_OUTPUTBARTREE_H__925ECC9F_1339_43E4_96B1_B4BEEB996549__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutputBarTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COutputBarTree window

class COutputBarTree : public CTreeCtrl
{
// Construction
public:
	COutputBarTree();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputBarTree)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COutputBarTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(COutputBarTree)
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	void CommonInit();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTBARTREE_H__925ECC9F_1339_43E4_96B1_B4BEEB996549__INCLUDED_)
