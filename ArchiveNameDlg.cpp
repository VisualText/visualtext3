/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// ArchiveNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VisualText.h"
#include "ArchiveNameDlg.h"

extern CString appAskText;
extern bool appBoolSaveInputFiles;
extern bool appBoolSaveKBFiles;
extern bool appBoolSaveLogFiles;
extern bool appBoolSaveCFiles;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArchiveNameDlg dialog


CArchiveNameDlg::CArchiveNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CArchiveNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CArchiveNameDlg)
	m_boolSaveInputFiles = FALSE;
	m_boolSaveKBFiles = FALSE;
	m_boolSaveLogFiles = false;
	m_boolSaveCFiles = FALSE;
	m_strArchivePath = _T("");
	//}}AFX_DATA_INIT
}


void CArchiveNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArchiveNameDlg)
	DDX_Check(pDX, IDC_INPUT_FILES, m_boolSaveInputFiles);
	DDX_Check(pDX, IDC_KB_FILES, m_boolSaveKBFiles);
	DDX_Check(pDX, IDC_SAVE_LOG_FILES, m_boolSaveLogFiles);
	DDX_Check(pDX, IDC_SAVE_C_FILES, m_boolSaveCFiles);
	DDX_Text(pDX, IDC_ARCHIVE_PATH, m_strArchivePath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArchiveNameDlg, CDialog)
	//{{AFX_MSG_MAP(CArchiveNameDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArchiveNameDlg message handlers

BOOL CArchiveNameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CEdit *ed = (CEdit *)GetDlgItem(IDC_ARCHIVE_PATH);
	ed->SetFocus();
	
	m_strArchivePath = appAskText;
	m_boolSaveInputFiles = appBoolSaveInputFiles;
	m_boolSaveKBFiles = appBoolSaveKBFiles;
	m_boolSaveLogFiles = appBoolSaveLogFiles;
	m_boolSaveCFiles = appBoolSaveCFiles;

	if (appAskText != _T(""))
		ed->SetSel(0,appAskText.GetLength());

	UpdateData(false);

	return false;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

void CArchiveNameDlg::OnOK() 
{
	UpdateData(true);
	appAskText = m_strArchivePath;
	appBoolSaveInputFiles = (m_boolSaveInputFiles ? true : false);
	appBoolSaveKBFiles = (m_boolSaveKBFiles ? true : false);
	appBoolSaveLogFiles = (m_boolSaveLogFiles ? true : false);
	appBoolSaveCFiles = (m_boolSaveCFiles ? true : false);

	CDialog::OnOK();
}

void CArchiveNameDlg::OnCancel() 
{
	appAskText = _T("");
	
	CDialog::OnCancel();
}

void CArchiveNameDlg::OnBrowse() 
{
	CString filterStr = _T("Zip files (*.zip)|*.zip|All files (*.*)|*.*||");
	CFileDialog fileDlg(true,_T("zip"),m_strArchivePath,NULL,filterStr);

	if (fileDlg.DoModal() == IDOK) {
		m_strArchivePath = fileDlg.GetPathName();
		UpdateData(false);
	}
}
