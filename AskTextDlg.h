/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_ASKTEXTDLG_H__6F9BFD41_B224_11D2_8BE9_00105A99059F__INCLUDED_)
#define AFX_ASKTEXTDLG_H__6F9BFD41_B224_11D2_8BE9_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AskTextDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAskTextDlg dialog

class CAskTextDlg : public CDialog
{
// Construction
public:
	CAskTextDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAskTextDlg)
	enum { IDD = IDD_ASK_TEXT_DIALOG };
	CString	m_strMsg;
	CString	m_strUserText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAskTextDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAskTextDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASKTEXTDLG_H__6F9BFD41_B224_11D2_8BE9_00105A99059F__INCLUDED_)
