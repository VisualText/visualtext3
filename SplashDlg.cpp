/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// SplashDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "kb.h"
#include "Utils.h"
#include "BrowserDoc.h"
#include "SplashDlg.h"
#include "MainFrm.h"

extern CVisualTextApp theApp;
extern VT_APP_STATE appState;
extern NLP *nlp;
extern CString appStrVersion;
extern CString appStrCopyright;
extern CFont appFontFixed;
extern CFont appFontCopyright;

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

/////////////////////////////////////////////////////////////////////////////
// CSplashDlg dialog


CSplashDlg::CSplashDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSplashDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSplashDlg)
	m_strMessage = _T("");
	m_strVersion = _T("");
	//}}AFX_DATA_INIT
	
	Init();
}

void CSplashDlg::Init()
{
	m_strMessage = _T("Starting VisualText");
	m_strVersion = appStrCopyright + _T(" - ") + appStrVersion;
	m_firstTime = CTime::GetCurrentTime();
	m_lastTime = m_firstTime;
	m_strLastMessage = _T("Initializing time");
}


void CSplashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSplashDlg)
	DDX_Text(pDX, IDC_MESSAGE, m_strMessage);
	DDX_Text(pDX, IDC_VERSION, m_strVersion);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSplashDlg, CDialog)
	//{{AFX_MSG_MAP(CSplashDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_WM_SETCURSOR() // SRP 01-01-29
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplashDlg message handlers

void CSplashDlg::UpdateMessage(CString messageStr)
{
	m_strMessage = messageStr + _T("...");
	AddMessageToList(messageStr);
	UpdateData(false);
}

void CSplashDlg::AddMessageToList(CString messageStr) 
{
	CTime currentTime = CTime::GetCurrentTime();
	CTimeSpan timeSpan = currentTime - m_lastTime;

	CString logStr;
	logStr.Format(_T("%s - %s"),m_strLastMessage,timeSpan.Format(_T("%M:%S")));

	m_stringListMessages.AddTail(logStr);

	m_strLastMessage = messageStr;
	m_lastTime = currentTime;
}

void CSplashDlg::OnDestroy() 
{
	AddMessageToList(m_strLastMessage);
	m_lastTime = m_firstTime;
	m_strLastMessage = _T("Total time:");
	AddMessageToList();
	DisplayLoadingStats();
	CDialog::OnDestroy();
}

void CSplashDlg::DisplayLoadingStats() 
{
	int i = 0;
	CString loadingStatStr;
	CMainFrame *wnd  = theApp.m_pMainFrame;

	while ((loadingStatStr = NthStringListItem(m_stringListMessages,i++)) != _T("")) {
		if (!loadingStatStr.IsEmpty())
			wnd->AddDebugLine(_T("Loading stat:"),loadingStatStr,true);
	}
}

void CSplashDlg::OnCancel() 
{
	if (appState == VT_APP_LOADING) {
#ifdef GEODUCK_
		if (nlp)
			nlp->gui->SetMessage(GUI_MESSAGE_ABORT);
#endif
		appState = VT_APP_STOP_COMMAND;
	}
}

BOOL CSplashDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_VERSION)->SetFont(&appFontCopyright,true);

	// Dd 010402
	UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// SRP 01-01-29
BOOL CSplashDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	// TODO: Add your message handler code here and/or call default
	CWnd* pWndCancel = GetDlgItem(IDC_CANCEL);
	if (pWndCancel) {
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		CRect r;
		pWndCancel->GetWindowRect(&r);
		ScreenToClient(&r);
		if (r.PtInRect(pt)) {
			SetCursor(theApp.LoadStandardCursor(IDC_ARROW));
			return true;
		}
	}
	if (theApp.DoHourGlass())
		return true;
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}
