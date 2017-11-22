/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_SEMPOPUPDLG_H__1CB847FB_74E5_4641_B24E_2BA5C548B4B6__INCLUDED_)
#define AFX_SEMPOPUPDLG_H__1CB847FB_74E5_4641_B24E_2BA5C548B4B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SemPopupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSemPopupDlg dialog

class CSemPopupDlg : public CDialog
{
// Construction
public:
	CSemPopupDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSemPopupDlg)
	enum { IDD = IDD_SEMPOPUP };
	CString	m_strText;
	//}}AFX_DATA
	CRect m_cR;
	CParseTreeItemInfo* m_pInfo;
	CFont m_Font;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSemPopupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSemPopupDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEMPOPUPDLG_H__1CB847FB_74E5_4641_B24E_2BA5C548B4B6__INCLUDED_)
