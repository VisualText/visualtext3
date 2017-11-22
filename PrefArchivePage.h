/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_PREFARCHIVEPAGE_H__D2161AE1_6708_11D4_A6D4_00105A99059F__INCLUDED_)
#define AFX_PREFARCHIVEPAGE_H__D2161AE1_6708_11D4_A6D4_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrefArchivePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrefArchivePage dialog

class CPrefArchivePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPrefArchivePage)

// Construction
public:
	CPrefArchivePage();
	~CPrefArchivePage();

// Dialog Data
	//{{AFX_DATA(CPrefArchivePage)
	enum { IDD = IDD_PREF_ARCHIVE };
	CString	m_strFTP;
	CString	m_strArchiveName;
	CString	m_strPassword;
	CString	m_strUsername;
	CString	m_strDirectoryPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPrefArchivePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPrefArchivePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFARCHIVEPAGE_H__D2161AE1_6708_11D4_A6D4_00105A99059F__INCLUDED_)
