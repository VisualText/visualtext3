/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_ASCIIDLG_H__17E0BDB2_4DE7_11D4_A69E_00105A99059F__INCLUDED_)
#define AFX_ASCIIDLG_H__17E0BDB2_4DE7_11D4_A69E_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ASCIIDlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CASCIIDlg dialog

class CASCIIDlg : public CXTPResizeDialog
{
// Construction
public:
	CASCIIDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CASCIIDlg)
	enum { IDD = IDD_ASCII_DLG };
	CString	m_asciiStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CASCIIDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CASCIIDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CFont m_font;

	void FillAsciiString();

public:
	afx_msg void OnDestroy();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASCIIDLG_H__17E0BDB2_4DE7_11D4_A69E_00105A99059F__INCLUDED_)
