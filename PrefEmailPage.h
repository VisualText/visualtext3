/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_PREFEMAILPAGE_H__6F4F37D3_651F_11D4_A6CE_00105A99059F__INCLUDED_)
#define AFX_PREFEMAILPAGE_H__6F4F37D3_651F_11D4_A6CE_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrefEmailPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrefEmailPage dialog

class CPrefEmailPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPrefEmailPage)

// Construction
public:
	CPrefEmailPage();
	~CPrefEmailPage();

// Dialog Data
	//{{AFX_DATA(CPrefEmailPage)
	enum { IDD = IDD_PREF_EMAIL_PAGE };
	CString	m_strSMTPServer;
	CString	m_strFromEmail;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPrefEmailPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPrefEmailPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFEMAILPAGE_H__6F4F37D3_651F_11D4_A6CE_00105A99059F__INCLUDED_)
