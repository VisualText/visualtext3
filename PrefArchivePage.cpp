/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// PrefArchivePage.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "PrefArchivePage.h"

extern CVisualTextApp theApp;
extern CString appStrArchiveFTP;
extern CString appStrArchiveDirectoryPath;
extern CString appStrArchiveUsername;
extern CString appStrArchivePassword;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrefArchivePage property page

IMPLEMENT_DYNCREATE(CPrefArchivePage, CPropertyPage)

CPrefArchivePage::CPrefArchivePage() : CPropertyPage(CPrefArchivePage::IDD)
{
	//{{AFX_DATA_INIT(CPrefArchivePage)
	m_strFTP = _T("");
	m_strArchiveName = _T("");
	m_strPassword = _T("");
	m_strUsername = _T("");
	m_strDirectoryPath = _T("");
	//}}AFX_DATA_INIT
}

CPrefArchivePage::~CPrefArchivePage()
{
}

void CPrefArchivePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrefArchivePage)
	DDX_Text(pDX, IDC_ARCHIVE_FTP, m_strFTP);
	DDX_Text(pDX, IDC_ARCHIVE_NAME, m_strArchiveName);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_USERNAME, m_strUsername);
	DDX_Text(pDX, IDC_DIRECTORY_PATH, m_strDirectoryPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrefArchivePage, CPropertyPage)
	//{{AFX_MSG_MAP(CPrefArchivePage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefArchivePage message handlers

BOOL CPrefArchivePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_strArchiveName = theApp.GetProfileString(_T("PREFERENCES"),_T("ARCHIVEAUTHOR"),_T("Your Name Here"));
	m_strFTP = theApp.GetProfileString(_T("PREFERENCES"),_T("ARCHIVEFTP"),_T(""));
	m_strDirectoryPath = theApp.GetProfileString(_T("PREFERENCES"),_T("ARCHIVEDIRECTORY"),_T(""));
	m_strUsername = theApp.GetProfileString(_T("PREFERENCES"),_T("ARCHIVEUSERNAME"),_T(""));
	m_strPassword = theApp.GetProfileString(_T("PREFERENCES"),_T("ARCHIVEPASSWORD"),_T(""));

	UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrefArchivePage::OnDestroy() 
{
	UpdateData(true);

	appStrArchiveFTP = m_strFTP;
	appStrArchiveDirectoryPath = m_strDirectoryPath;
	appStrArchiveUsername = m_strUsername;
	appStrArchivePassword = m_strPassword;

	theApp.WriteProfileString(_T("PREFERENCES"),_T("ARCHIVEAUTHOR"),m_strArchiveName);
	m_strArchiveName;
	theApp.WriteProfileString(_T("PREFERENCES"),_T("ARCHIVEFTP"),m_strFTP);
	theApp.WriteProfileString(_T("PREFERENCES"),_T("ARCHIVEDIRECTORY"),m_strDirectoryPath);
	theApp.WriteProfileString(_T("PREFERENCES"),_T("ARCHIVEUSERNAME"),m_strUsername);
	theApp.WriteProfileString(_T("PREFERENCES"),_T("ARCHIVEPASSWORD"),m_strPassword);
	
	CPropertyPage::OnDestroy();
}
