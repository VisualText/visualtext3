/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_GRAMMARCONCEPTDLG_H__2FF340E1_4601_11D3_A3DE_00105A99059F__INCLUDED_)
#define AFX_GRAMMARCONCEPTDLG_H__2FF340E1_4601_11D3_A3DE_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GrammarConceptDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGrammarConceptDlg dialog

class CGrammarConceptDlg : public CDialog
{
// Construction
public:
	CGrammarConceptDlg(CWnd* pParent = NULL);   // standard constructor

public:
	void SetConcept(CONCEPT *concept);

// Dialog Data
	//{{AFX_DATA(CGrammarConceptDlg)
	enum { IDD = IDD_GRAM_CONCEPT };
	CString	m_strName;
	BOOL	m_boolClosed;
	BOOL	m_boolClosedInherited;
	BOOL	m_boolConstrained;
	BOOL	m_boolConstrainedInherited;
	BOOL	m_boolGeneralized;
	BOOL	m_boolGeneralizedInherited;
	BOOL	m_boolLiteral;
	BOOL	m_boolLiteralInherited;
	BOOL	m_boolSplit;
	BOOL	m_boolSplitInherited;
	BOOL	m_boolWhitespace;
	BOOL	m_boolWhitespaceInherited;
	BOOL	m_boolLocked;
	BOOL	m_boolLockedInherited;
	CString	m_strSampleFile;
	CString	m_strLayered;
	BOOL	m_boolQuickSemantics;
	BOOL	m_boolQuickSemanticsInherited;
	BOOL	m_boolRaw;
	BOOL	m_boolRawInherited;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrammarConceptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGrammarConceptDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClosedInherited();
	afx_msg void OnConstrainedInherited();
	afx_msg void OnGeneralizedInherited();
	afx_msg void OnSplitInherited();
	afx_msg void OnLiteralInherited();
	afx_msg void OnWhitespaceInherited();
	afx_msg void OnHelpRuleGeneration();
	afx_msg void OnLockedInherited();
	afx_msg void OnBrowse();
	afx_msg void OnLocked();
	afx_msg void OnClosed();
	afx_msg void OnConstrained();
	afx_msg void OnGeneralized();
	afx_msg void OnSplit();
	afx_msg void OnLiteral();
	afx_msg void OnWhitespace();
	afx_msg void OnRaw();
	afx_msg void OnRawInherited();
	afx_msg void OnQuicksem();
	afx_msg void OnQuicksemInherited();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CONCEPT *m_concept;
	CString	origSampleStr;
	CString	origLayeredStr;
	BOOL	origBoolClosedInherited;
	BOOL	origBoolConstrainedInherited;
	BOOL	origBoolGeneralizedInherited;
	BOOL	origBoolLiteralInherited;
	BOOL	origBoolRawInherited;
	BOOL	origBoolQuickSemanticsInherited;
	BOOL	origBoolSplitInherited;
	BOOL	origBoolWhitespaceInherited;
	BOOL	origBoolLockedInherited;
	bool m_boolDirtyFlag;	

	bool SetConceptAttributes();
	void GetConceptAttributes();
	void GetValue(CString valStr, BOOL &value, BOOL &inherited, UINT dlgItem);
	void UpdateInherited(UINT dlgItemID, BOOL &value, BOOL inherited);
	CString AttrStringFromID(UINT dlgItemID);
	void SetConceptAttribute(CString attrStr, int conVal, int inheritedVal, int origInheritedVal);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAMMARCONCEPTDLG_H__2FF340E1_4601_11D3_A3DE_00105A99059F__INCLUDED_)
