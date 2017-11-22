/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_TEXTINFODLG_H__383713D1_1C67_11D4_A624_00105A99059F__INCLUDED_)
#define AFX_TEXTINFODLG_H__383713D1_1C67_11D4_A624_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextInfoDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CTextInfoDlg dialog

class CTextInfoDlg : public CDialog
{
// Construction
public:
	CTextInfoDlg(CWnd* pParent = NULL);   // standard constructor

	void SetData(CString fullFilePathStr);

// Dialog Data
	//{{AFX_DATA(CTextInfoDlg)
	enum { IDD = IDD_TEXT_INFO };
	CString	m_stringCharacterCount;
	CString	m_stringFileSize;
	CString	m_stringLastModified;
	CString	m_stringLineCount;
	CString	m_stringWordCount;
	CString	m_stringFilePath;
	CString	m_stringCreated;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextInfoDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void GetFileInfo();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTINFODLG_H__383713D1_1C67_11D4_A624_00105A99059F__INCLUDED_)
