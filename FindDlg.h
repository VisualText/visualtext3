/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_FINDDLG_H__F898C441_8E50_11D3_A4A6_00105A99059F__INCLUDED_)
#define AFX_FINDDLG_H__F898C441_8E50_11D3_A4A6_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindDlg dialog

class CFindDlg : public CDialog
{
// Construction
public:
	CFindDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindDlg)
	enum { IDD = IDD_FIND_DLG };
	CListCtrl	m_listCtrlRuleDir;
	CString	m_strFind;
	BOOL	m_boolMatchCase;
	CString	m_strRuleDir;
	BOOL	m_boolLogFiles;
	BOOL	m_boolRuleFiles;
	BOOL	m_boolConcept;
	BOOL	m_boolTextFiles;
	BOOL	m_boolAppend;
	BOOL	m_boolOutputFiles;
	BOOL	m_boolFind2;
	//}}AFX_DATA

public:
	void SetData(CString findStr, CVisualTextDoc *doc);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CVisualTextDoc *m_doc;
	void FillDirectoryCombo();
	CString GetOtherRulesStr();
	CString GetStateStr();
	void StateFromStr();

public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDDLG_H__F898C441_8E50_11D3_A4A6_00105A99059F__INCLUDED_)
