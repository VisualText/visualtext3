/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_FILECHARVIEWERDLG_H__DDA15851_5CC7_11D4_A6BC_00105A99059F__INCLUDED_)
#define AFX_FILECHARVIEWERDLG_H__DDA15851_5CC7_11D4_A6BC_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileCharViewerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileCharViewerDlg dialog

class CFileCharViewerDlg : public CXTPResizeDialog
{
// Construction
public:
	CFileCharViewerDlg(CWnd* pParent = NULL);   // standard constructor

	void SetData(CString filePathStr);

// Dialog Data
	//{{AFX_DATA(CFileCharViewerDlg)
	enum { IDD = IDD_CHAR_VIEWER };
	CString	m_strDisplay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileCharViewerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileCharViewerDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_strFilePath;
	CFont m_font;

	void GenerateDisplayStr();
public:
	afx_msg void OnDestroy();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILECHARVIEWERDLG_H__DDA15851_5CC7_11D4_A6BC_00105A99059F__INCLUDED_)
