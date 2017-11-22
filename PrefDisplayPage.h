/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_PREFDISPLAYPAGE_H__6F4F37D1_651F_11D4_A6CE_00105A99059F__INCLUDED_)
#define AFX_PREFDISPLAYPAGE_H__6F4F37D1_651F_11D4_A6CE_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrefDisplayPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrefDisplayPage dialog

class CPrefDisplayPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPrefDisplayPage)

// Construction
public:
	CPrefDisplayPage();
	~CPrefDisplayPage();

// Dialog Data
	//{{AFX_DATA(CPrefDisplayPage)
	enum { IDD = IDD_PREF_DISPLAY_PAGE };
	BOOL	m_boolTabbedWindows;
	BOOL	m_boolToolbarLabels;
	BOOL	m_boolAutosizeWindows;
	CString	m_strRuleTabSize;
	CString	m_strMaxWindowHeight;
	CString	m_strMaxWindowWidth;
	CString	m_strFixedFontSize;
	BOOL	m_boolSizeDynamically;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPrefDisplayPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPrefDisplayPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFDISPLAYPAGE_H__6F4F37D1_651F_11D4_A6CE_00105A99059F__INCLUDED_)
