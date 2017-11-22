/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_SPLASHDLG_H__86FA2971_87DD_11D3_A496_00105A99059F__INCLUDED_)
#define AFX_SPLASHDLG_H__86FA2971_87DD_11D3_A496_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplashDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplashDlg dialog

class CSplashDlg : public CDialog
{
// Construction
public:
	CSplashDlg(CWnd* pParent = NULL);   // standard constructor

	void UpdateMessage(CString messageStr);
	void Init();


// Dialog Data
	//{{AFX_DATA(CSplashDlg)
	enum { IDD = IDD_SPLASH_DLG };
	CString	m_strMessage;
	CString	m_strVersion;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSplashDlg)
	afx_msg void OnDestroy();
	afx_msg void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message); // SRP 01-01-29
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CStringList m_stringListMessages;
	CString m_strLastMessage;
	CTime m_firstTime;
	CTime m_lastTime;

	void CSplashDlg::DisplayLoadingStats();
	void AddMessageToList(CString messageStr=_T(""));

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLASHDLG_H__86FA2971_87DD_11D3_A496_00105A99059F__INCLUDED_)
