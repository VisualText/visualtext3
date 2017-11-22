/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_EMAILDLG_H__C7C92391_64F3_11D4_A6CD_00105A99059F__INCLUDED_)
#define AFX_EMAILDLG_H__C7C92391_64F3_11D4_A6CD_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EmailDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEmailDlg dialog

class CEmailDlg : public CDialog
{
// Construction
public:
	CEmailDlg(CWnd* pParent = NULL);   // standard constructor

	void SetData(CString strTo, CString strSubject, CString strBody);
	void SetFileData(CString filePathStr);

// Dialog Data
	//{{AFX_DATA(CEmailDlg)
	enum { IDD = IDD_EMAIL_DLG };
	CString	m_strBody;
	CString	m_strSubject;
	CString	m_strTo;
	CString	m_strEmailList;
	CString	m_strStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEmailDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEmailDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnEmailListChoose();
	afx_msg void OnPreviousEmail();
	afx_msg void OnNextEmail();
	afx_msg void OnSendCurrent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CStringList m_strListEmails;
	void CurrentEmail();
	int m_intEmailListCount;
	int m_intCurrentEmail;
	CFont m_font;

	bool SendOneEmail();
	bool SendToEmailList();
	bool SendEmail();


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMAILDLG_H__C7C92391_64F3_11D4_A6CD_00105A99059F__INCLUDED_)
