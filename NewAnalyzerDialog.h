/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_NEWANALYZERDIALOG_H__59DAB361_2A78_11D3_A38A_00105A99059F__INCLUDED_)
#define AFX_NEWANALYZERDIALOG_H__59DAB361_2A78_11D3_A38A_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewAnalyzerDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewAnalyzerDialog dialog

class CNewAnalyzerDialog : public CDialog
{
// Construction
public:
	CNewAnalyzerDialog(CWnd* pParent = NULL);   // standard constructor

	void EnableTemplates(bool enable);

// Dialog Data
	//{{AFX_DATA(CNewAnalyzerDialog)
	enum { IDD = IDD_NEW_ANALYZER };
	CListCtrl	m_listCtrlType;
	CString	m_strAnalyzerName;
	CString	m_strAnalyzerLocation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewAnalyzerDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewAnalyzerDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool m_boolEnableTemplates;
	
	void LoadAnalyzerTypes();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWANALYZERDIALOG_H__59DAB361_2A78_11D3_A38A_00105A99059F__INCLUDED_)
