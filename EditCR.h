/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_EDITCR_H__6B6DD1C2_EFEA_11D1_A2FB_444553540000__INCLUDED_)
#define AFX_EDITCR_H__6B6DD1C2_EFEA_11D1_A2FB_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EditCR.h : header file
//

#define ID_EDITCTRL_IN_COMBOBOX	1001
/////////////////////////////////////////////////////////////////////////////
// CEditCR window

class CEditCR : public CEdit
{
// Construction
public:
	CEditCR();

// Attributes
public:
	UINT m_nIDCR;

// Operations
public:
	BOOL AutoLoad(UINT, CWnd*, UINT);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditCR)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditCR();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditCR)
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITCR_H__6B6DD1C2_EFEA_11D1_A2FB_444553540000__INCLUDED_)
