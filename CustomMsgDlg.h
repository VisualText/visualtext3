/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_CUSTOMMSGDLG_H__89CF7351_6EE2_11D4_A6E6_00105A99059F__INCLUDED_)
#define AFX_CUSTOMMSGDLG_H__89CF7351_6EE2_11D4_A6E6_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomMsgDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomMsgDlg dialog

class CCustomMsgDlg : public CDialog
{
// Construction
public:
	CCustomMsgDlg(CWnd* pParent = NULL);   // standard constructor

	void SetData(CString msgStr, int numButtons, int defaultChoice,
		CString butt1Str, CString butt2Str, CString butt3Str);

// Dialog Data
	//{{AFX_DATA(CCustomMsgDlg)
	enum { IDD = IDD_CUSTOM_MSG_DLG };
	CString	m_strMsg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomMsgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCustomMsgDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonPushed1();
	afx_msg void OnButtonPushed2();
	afx_msg void OnButtonPushed3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CFont m_fontButton;
	CDC *m_pDC;
	TEXTMETRIC m_tm;

	CString m_strButton1;
	CString m_strButton2;
	CString m_strButton3;
	int m_intNumButtons;
	int m_intDefaultChoice;

	CButton m_button1;
	CButton m_button2;
	CButton m_button3;

	void CreateButtons();
	int ButtonWidthFromStr(CString buttonStr);
	void OuttaHere(int buttonNum);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMMSGDLG_H__89CF7351_6EE2_11D4_A6E6_00105A99059F__INCLUDED_)
