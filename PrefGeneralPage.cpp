/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// PrefGeneralPage.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "VisualText.h"
#include "PrefGeneralPage.h"
#include "UserLogFile.h"

extern CString appDirectoryStr;
extern CString appStrAuthorName;
extern CString appAnaFilePathStr;
extern CVisualTextApp theApp;
extern CString appStrAppsDirectory;
extern CString appAskText;
extern BOOL appBoolTipOfDay;
extern BOOL appBoolKBSafeEditMode;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrefGeneralPage property page

IMPLEMENT_DYNCREATE(CPrefGeneralPage, CPropertyPage)

CPrefGeneralPage::CPrefGeneralPage() : CPropertyPage(CPrefGeneralPage::IDD)
{
	//{{AFX_DATA_INIT(CPrefGeneralPage)
	m_strAuthorName = _T("");
	m_strLastAnalyzerPath = _T("");
	m_boolTipOfDay = FALSE;
	m_boolAutoLoad = FALSE;
	m_boolFullScreen = FALSE;
	m_boolAutoSaveKB = FALSE;
	m_bKBSafeEditMode = FALSE;
	m_strAppsDirectory = _T("");
	m_bGenerateUserLogFile = FALSE;
	//}}AFX_DATA_INIT
}

CPrefGeneralPage::~CPrefGeneralPage()
{
}

void CPrefGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrefGeneralPage)
	DDX_Check(pDX, ID_HELP_TIPOFTHEDAY, m_boolTipOfDay);
	DDX_Check(pDX, IDC_AUTOLOAD, m_boolAutoLoad);
	DDX_Text(pDX, IDC_AUTHOR_NAME, m_strAuthorName);
	DDX_Text(pDX, IDC_LAST_ANALYZER_TEXT, m_strLastAnalyzerPath);
	DDX_Check(pDX, IDC_FULL_SCREEN, m_boolFullScreen);
	DDX_Check(pDX, IDC_AUTOSAVE_KB, m_boolAutoSaveKB);
	DDX_Check(pDX, IDC_KB_SAFE_EDIT_MODE, m_bKBSafeEditMode);
	DDX_Text(pDX, IDC_APPS_DIRECTORY, m_strAppsDirectory);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrefGeneralPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPrefGeneralPage)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefGeneralPage message handlers

BOOL CPrefGeneralPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CString valueStr = theApp.GetProfileString(_T("PREFERENCES"),_T("AUTOLOAD"),_T("false"));
	m_boolAutoLoad = BoolValueVT(valueStr);
	m_strAuthorName = theApp.GetProfileString(_T("PREFERENCES"),_T("AUTHORNAME"),_T("Your Name Here"));
	
	m_strLastAnalyzerPath = theApp.GetProfileString(_T("PREFERENCES"),_T("LASTANALYZER"),_T(""));
	if (m_strLastAnalyzerPath == _T(""))
		m_strLastAnalyzerPath = _T("(none)");

	// Full screen by default. // 05/17/08 AM.
	valueStr = theApp.GetProfileString(_T("PREFERENCES"),_T("FULLSCREEN"),_T("true"));
	m_boolFullScreen = BoolValueVT(valueStr);

	valueStr = theApp.GetProfileString(_T("PREFERENCES"),_T("AUTOSAVEKBLOGOUT"),_T("false"));
	m_boolAutoSaveKB = BoolValueVT(valueStr);

	// KB safe mode off by default.	// 05/17/08 AM.
	valueStr = theApp.GetProfileString(_T("PREFERENCES"),_T("KBSAFEEDITMODE"),_T("false"));
	m_bKBSafeEditMode = BoolValueVT(valueStr);

	valueStr = theApp.GetProfileString(_T("PREFERENCES"),_T("TIPOFDAY"),_T("true"));
	m_boolTipOfDay = BoolValueVT(valueStr);

	m_strAppsDirectory = theApp.GetAppsDir();

	valueStr = theApp.GetProfileString(_T("PREFERENCES"),_T("USERLOGFILE"),_T("false"));
	m_bGenerateUserLogFile = BoolValueVT(valueStr);

	UpdateData(false);
	
	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrefGeneralPage::OnDestroy() 
{
	UpdateData(true);

	theApp.WriteProfileString(_T("PREFERENCES"),_T("AUTOLOAD"),BoolStr(m_boolAutoLoad));
	theApp.WriteProfileString(_T("PREFERENCES"),_T("AUTHORNAME"),m_strAuthorName);
	theApp.WriteProfileString(_T("PREFERENCES"),_T("FULLSCREEN"),BoolStr(m_boolFullScreen));
	theApp.WriteProfileString(_T("PREFERENCES"),_T("TIPOFDAY"),BoolStr(m_boolTipOfDay));

	theApp.WriteProfileString(_T("PREFERENCES"),_T("AUTOSAVEKBLOGOUT"),BoolStr(m_boolAutoSaveKB));
	theApp.WriteProfileString(_T("PREFERENCES"),_T("KBSAFEEDITMODE"),BoolStr(m_bKBSafeEditMode));
	theApp.WriteProfileString(_T("PREFERENCES"),_T("APPSDIRECTORY"),m_strAppsDirectory);
	theApp.WriteProfileString(_T("PREFERENCES"),_T("USERLOGFILE"),BoolStr(m_bGenerateUserLogFile));
	bULFflag = m_bGenerateUserLogFile;
	appStrAppsDirectory = m_strAppsDirectory;
	appBoolKBSafeEditMode = m_bKBSafeEditMode;
	
	CPropertyPage::OnDestroy();
}

void CPrefGeneralPage::OnBrowse() 
{
	CString strDir;
	if (UTLSelectFolder(m_hWnd, _T("Please choose a folder to store Analyzer Projects (e.g. c:\\apps)."), strDir, true)) {
		m_strAppsDirectory = strDir;
		UpdateData(false);
	}
}

