/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_ARCHIVENAMEDLG_H__2F64DC60_D8EF_11D3_A59E_00105A99059F__INCLUDED_)
#define AFX_ARCHIVENAMEDLG_H__2F64DC60_D8EF_11D3_A59E_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArchiveNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CArchiveNameDlg dialog

class CArchiveNameDlg : public CDialog
{
// Construction
public:
	CArchiveNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CArchiveNameDlg)
	enum { IDD = IDD_ARCHIVE_NAME_DLG };
	BOOL	m_boolSaveKBFiles;
	BOOL	m_boolSaveInputFiles;
	BOOL	m_boolSaveLogFiles;
	BOOL	m_boolSaveCFiles;
	CString	m_strArchivePath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArchiveNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CArchiveNameDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARCHIVENAMEDLG_H__2F64DC60_D8EF_11D3_A59E_00105A99059F__INCLUDED_)
