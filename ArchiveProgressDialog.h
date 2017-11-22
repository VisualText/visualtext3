/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_ARCHIVEPROGRESSDIALOG_H__8DF7D200_1F51_11D3_A367_00105A99059F__INCLUDED_)
#define AFX_ARCHIVEPROGRESSDIALOG_H__8DF7D200_1F51_11D3_A367_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArchiveProgressDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CArchiveProgressDialog dialog

class CDialogThread : public CWinThread
{
	DECLARE_DYNCREATE(CDialogThread)
	CDialogThread() {};
	virtual BOOL InitInstance();
};

class CArchiveProgressDialog : public CDialog
{
// Construction
public:
	CArchiveProgressDialog(CWnd* pParent = NULL);   // standard constructor
	void SetProgressMessage(CString msgStr);
	void SetArchiveMessage(CString archiveMsgStr);
	void SetRange(int lower, int upper);
	void SetStep(int step);
	bool StepIt();
	void SetFileSize(long fileSize);

// Dialog Data
	//{{AFX_DATA(CArchiveProgressDialog)
	enum { IDD = IDD_ARCHIVE_PROGRESS };
	CProgressCtrl	m_progressCtrl;
	CString	m_strArchiveMessage;
	CString	m_strProgressMessage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArchiveProgressDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CArchiveProgressDialog)
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool m_boolCancel;
	long m_longFileSize;
	int m_intLastPercent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARCHIVEPROGRESSDIALOG_H__8DF7D200_1F51_11D3_A367_00105A99059F__INCLUDED_)
