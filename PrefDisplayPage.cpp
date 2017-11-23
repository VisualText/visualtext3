/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// PrefDisplayPage.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "VisualText.h"
#include "PrefDisplayPage.h"
#include "MainFrm.h"

extern CVisualTextApp theApp;
extern bool appBoolTabbedWindows;
extern bool appBoolToolbarLabels;
extern bool appBoolAutosizeWindows;
extern bool appBoolSizeDynamically;
extern int appIntMaxWindowWidth;
extern int appIntMaxWindowHeight;
extern int appIntRuleTabSize;
extern int appIntFixedFontSize;
extern CFont appFontFixed;
extern CStringList appStatus;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrefDisplayPage property page

IMPLEMENT_DYNCREATE(CPrefDisplayPage, CPropertyPage)

CPrefDisplayPage::CPrefDisplayPage() : CPropertyPage(CPrefDisplayPage::IDD)
{
	//{{AFX_DATA_INIT(CPrefDisplayPage)
	m_boolTabbedWindows = TRUE;
	m_boolToolbarLabels = TRUE;
	m_boolAutosizeWindows = FALSE;
	m_strRuleTabSize = _T("");
	m_strMaxWindowHeight = _T("");
	m_strMaxWindowWidth = _T("");
	m_strFixedFontSize = _T("");
	m_boolSizeDynamically = FALSE;
	//}}AFX_DATA_INIT
}

CPrefDisplayPage::~CPrefDisplayPage()
{
}

void CPrefDisplayPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrefDisplayPage)
	DDX_Check(pDX, IDC_TABBED_WINDOWS, m_boolTabbedWindows);
	DDX_Check(pDX, IDC_TOOLBAR_LABELS, m_boolToolbarLabels);
	DDX_Check(pDX, IDC_AUTOSIZE_WINDOWS, m_boolAutosizeWindows);
	DDX_Text(pDX, IDC_RULE_TAB_SIZE, m_strRuleTabSize);
	DDX_Text(pDX, IDC_MAX_WINDOW_HEIGHT, m_strMaxWindowHeight);
	DDX_Text(pDX, IDC_MAX_WINDOW_WIDTH, m_strMaxWindowWidth);
	DDX_CBString(pDX, IDC_FONT_SIZE, m_strFixedFontSize);
	DDX_Check(pDX, IDC_DYNAMIC_SIZE, m_boolSizeDynamically);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrefDisplayPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPrefDisplayPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefDisplayPage message handlers

BOOL CPrefDisplayPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_boolTabbedWindows = BoolValueVT(theApp.GetProfileString(_T("PREFERENCES"),_T("TABBEDWINDOWS"),_T("false")));
	m_boolToolbarLabels = BoolValueVT(theApp.GetProfileString(_T("PREFERENCES"),_T("TOOLBARLABELS"),_T("false")));
	m_boolAutosizeWindows = BoolValueVT(theApp.GetProfileString(_T("PREFERENCES"),_T("AUTOSIZEWINDOWS"),_T("false")));
	m_boolSizeDynamically = BoolValueVT(theApp.GetProfileString(_T("PREFERENCES"),_T("SIZEDYNAMICALLY"),_T("false")));
	m_strMaxWindowWidth = theApp.GetProfileString(_T("PREFERENCES"),_T("MAXWINDOWWIDTH"),_T("500"));
	m_strMaxWindowHeight = theApp.GetProfileString(_T("PREFERENCES"),_T("MAXWINDOWHEIGHT"),_T("300"));
	m_strRuleTabSize = theApp.GetProfileString(_T("PREFERENCES"),_T("RULETABSIZE"),_T("4"));
	m_strFixedFontSize = theApp.GetProfileString(_T("PREFERENCES"),_T("FIXEDFONTSIZE"),_T("12"));

	UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrefDisplayPage::OnDestroy() 
{
	CString prevFixedFontSizeStr = m_strFixedFontSize;
	bool beforeTabbed = appBoolTabbedWindows;
	bool beforeToolbarLabels = appBoolToolbarLabels;
	BOOL beforeAutoSize = m_boolAutosizeWindows;

	UpdateData(true);

	theApp.WriteProfileString(_T("PREFERENCES"),_T("TABBEDWINDOWS"),BoolStr(m_boolTabbedWindows));
	appBoolTabbedWindows = (m_boolTabbedWindows ? true : false);

	theApp.WriteProfileString(_T("PREFERENCES"),_T("TOOLBARLABELS"),BoolStr(m_boolToolbarLabels));
	appBoolToolbarLabels = (m_boolToolbarLabels ? true : false);

	theApp.WriteProfileString(_T("PREFERENCES"),_T("AUTOSIZEWINDOWS"),BoolStr(m_boolAutosizeWindows));
	appBoolAutosizeWindows = (m_boolAutosizeWindows ? true : false);

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	if (beforeAutoSize != m_boolAutosizeWindows && m_boolAutosizeWindows) {
		wnd->AutosizeAll();
	}
		
	if (beforeTabbed != appBoolTabbedWindows) {
		theApp.SaveAllStates();
		theApp.CloseAllViews(true);

		wnd->SetTabbedWindows(appBoolTabbedWindows);

		theApp.RestoreViews(appStatus, true);

		if (!appBoolTabbedWindows)
			wnd->MDICascade();

	}
	
	if (beforeToolbarLabels != appBoolToolbarLabels) {

		//Main
		if (appBoolToolbarLabels == true) {
			wnd->m_mainToolBar->GetControls()->Add(xtpControlLabel, 0, 0, 0)->SetCaption(_T("Main:"));
		}
		else {
			wnd->m_mainToolBar->GetControls()->Remove(0);
		}
		wnd->m_mainToolBar->UpdateWindow();

		// KB
		if (appBoolToolbarLabels == true) {
			wnd->m_knowledgeToolBar->GetControls()->Add(xtpControlLabel, 0, 0, 0)->SetCaption(_T("Knowledge:"));
		}
		else {
			wnd->m_knowledgeToolBar->GetControls()->Remove(0);
		}
		wnd->m_knowledgeToolBar->UpdateWindow();

		// Browser
		if (appBoolToolbarLabels == true) {
			wnd->m_browserToolBar->GetControls()->Add(xtpControlLabel, 0, 0, 0)->SetCaption(_T("Browser:"));
		}
		else {
			wnd->m_browserToolBar->GetControls()->Remove(0);
		}
		wnd->m_browserToolBar->UpdateWindow();

		// Debug
		if (appBoolToolbarLabels == true) {
			wnd->m_debugToolBar->GetControls()->Add(xtpControlLabel, 0, 0, 0)->SetCaption(_T("Debug:"));
		}
		else {
			wnd->m_debugToolBar->GetControls()->Remove(0);
		}
		wnd->m_debugToolBar->UpdateWindow();

		wnd->SaveCommandBars(_T("CommandBars"));
	}

	theApp.WriteProfileString(_T("PREFERENCES"),_T("SIZEDYNAMICALLY"),BoolStr(m_boolSizeDynamically));
	appBoolSizeDynamically = (m_boolSizeDynamically ? true : false);
	
	theApp.WriteProfileString(_T("PREFERENCES"),_T("MAXWINDOWWIDTH"),m_strMaxWindowWidth);
	appIntMaxWindowWidth = StringToInteger(m_strMaxWindowWidth);
	theApp.WriteProfileString(_T("PREFERENCES"),_T("MAXWINDOWHEIGHT"),m_strMaxWindowHeight);
	appIntMaxWindowHeight = StringToInteger(m_strMaxWindowHeight);
	theApp.WriteProfileString(_T("PREFERENCES"),_T("RULETABSIZE"),m_strRuleTabSize);
	appIntRuleTabSize = StringToInteger(m_strRuleTabSize);
	theApp.WriteProfileString(_T("PREFERENCES"),_T("FIXEDFONTSIZE"),m_strFixedFontSize);
	appIntFixedFontSize = StringToInteger(m_strFixedFontSize);

	if (prevFixedFontSizeStr != m_strFixedFontSize)
		theApp.DoAllViews(NULL,_T("VisualText"),_T(""),DOC_ACTION_UPDATE_FONT);

	CPropertyPage::OnDestroy();
}
