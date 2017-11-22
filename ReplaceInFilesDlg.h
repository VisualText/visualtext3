/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_REPLACEINFILESDLG_H__3A3645F8_E3A4_4434_B2BC_FC2E439AC15F__INCLUDED_)
#define AFX_REPLACEINFILESDLG_H__3A3645F8_E3A4_4434_B2BC_FC2E439AC15F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReplaceInFilesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReplaceInFilesDlg dialog

class CReplaceInFilesDlg : public CDialog
{
// Construction
public:
	CReplaceInFilesDlg(CWnd* pParent = NULL);   // standard constructor
	int SearchReplaceInFile(LPCTSTR strFilePath);
	int DoReplaceInFiles(LPCTSTR strFolder, LPCTSTR strPattern, BOOL bRecurse);
	void EnableReplaceButton(void);
	void GetLineText(LPCTSTR lpStart, int offset, int& line, CString& strLineText);

// Dialog Data
	//{{AFX_DATA(CReplaceInFilesDlg)
	enum { IDD = IDD_REPLACE_IN_FILES };
	BOOL	m_bMatchCase;
	BOOL	m_bMatchWholeWord;
	BOOL	m_bRuleFiles;
	BOOL	m_bTextFiles;
	CString	m_strDirectory;
	CString	m_strFind;
	CString	m_strReplace;
	BOOL	m_DirectoryRecursive;
	BOOL	m_RulesRecursive;
	BOOL	m_TextRecursive;
	CString	m_strFileSpec;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReplaceInFilesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReplaceInFilesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	virtual void OnOK();
	afx_msg void OnChangeFind();
	afx_msg void OnUpdateFind();
	afx_msg void OnRuleFiles();
	afx_msg void OnTextFiles();
	afx_msg void OnChangeDirectory();
	afx_msg void OnEditchangeFilespec();
	afx_msg void OnSelendokFilespec();
	//}}AFX_MSG
	afx_msg LRESULT OnEnableReplaceButton(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPLACEINFILESDLG_H__3A3645F8_E3A4_4434_B2BC_FC2E439AC15F__INCLUDED_)
