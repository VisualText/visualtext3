/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_ARCHIVELISTDIALOG_H__B8A2F350_1DF6_11D3_A362_00105A99059F__INCLUDED_)
#define AFX_ARCHIVELISTDIALOG_H__B8A2F350_1DF6_11D3_A362_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArchiveListDialog.h : header file
//
#include "ListCtrlF.h"

enum ARCHIVE_DEST_TYPE {
	ARCHIVE_DEST_SERVER = 0,
	ARCHIVE_DEST_LOCAL,
};

/////////////////////////////////////////////////////////////////////////////
// CArchiveListDialog dialog

class CArchiveListDialog : public CDialog
{
// Construction
public:
	CArchiveListDialog(CWnd* pParent = NULL);   // standard constructor
	void ArchiveStringList(CStringList &archiveList, CString dirStr);
	void SetArchiveType(CString archiveStr);
	void SetDestinationType(ARCHIVE_DEST_TYPE destinationType);

// Dialog Data
	//{{AFX_DATA(CArchiveListDialog)
	enum { IDD = IDD_ARCHIVE_LIST_DIALOG };
	CListCtrlF	m_listCtrlArchive;
	CString	m_strLocalPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArchiveListDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CArchiveListDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkArchiveList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	afx_msg void OnDownload();
	afx_msg void OnRename();
	afx_msg void OnLoad();
	afx_msg void OnBrowse();
	afx_msg void OnColumnclickArchiveList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitemArchiveList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	ARCHIVE_DEST_TYPE m_destinationType;
	CString m_strArchiveType;
	int m_intArchiveSelected;

	void DownloadSelectedArchives(bool upload);
	void LoadServerDirectory();
	void LoadLocalDirectory();
	void LoadArchive(CString localFileStr);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARCHIVELISTDIALOG_H__B8A2F350_1DF6_11D3_A362_00105A99059F__INCLUDED_)
