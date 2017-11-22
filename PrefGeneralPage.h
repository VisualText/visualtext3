/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_PREFGENERALPAGE_H__6F4F37D2_651F_11D4_A6CE_00105A99059F__INCLUDED_)
#define AFX_PREFGENERALPAGE_H__6F4F37D2_651F_11D4_A6CE_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrefGeneralPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrefGeneralPage dialog

class CPrefGeneralPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPrefGeneralPage)

// Construction
public:
	CPrefGeneralPage();
	~CPrefGeneralPage();

// Dialog Data
	//{{AFX_DATA(CPrefGeneralPage)
	enum { IDD = IDD_PREF_GENERAL_PAGE };
	BOOL	m_boolTipOfDay;
	BOOL	m_boolAutoLoad;
	CString	m_strAuthorName;
	CString	m_strLastAnalyzerPath;
	BOOL	m_boolFullScreen ;
	BOOL	m_boolAutoSaveKB;
	CString	m_strAppsDirectory;
	BOOL	m_bGenerateUserLogFile;
	BOOL	m_bKBSafeEditMode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPrefGeneralPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPrefGeneralPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFGENERALPAGE_H__6F4F37D2_651F_11D4_A6CE_00105A99059F__INCLUDED_)
