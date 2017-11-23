/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// PrefEmailPage.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "PrefEmailPage.h"

extern int appIntMaxWindowWidth;
extern int appIntMaxWindowHeight;
extern CVisualTextApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrefEmailPage property page

IMPLEMENT_DYNCREATE(CPrefEmailPage, CPropertyPage)

CPrefEmailPage::CPrefEmailPage() : CPropertyPage(CPrefEmailPage::IDD)
{
	//{{AFX_DATA_INIT(CPrefEmailPage)
	m_strSMTPServer = _T("");
	m_strFromEmail = _T("");
	//}}AFX_DATA_INIT
}

CPrefEmailPage::~CPrefEmailPage()
{
}

void CPrefEmailPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrefEmailPage)
	DDX_Text(pDX, IDC_SMTP_SERVER, m_strSMTPServer);
	DDX_Text(pDX, IDC_FROM_EMAIL, m_strFromEmail);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrefEmailPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPrefEmailPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefEmailPage message handlers

BOOL CPrefEmailPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_strSMTPServer = theApp.GetProfileString(_T("PREFERENCES"),_T("SMTPSERVER"),_T(""));
	m_strFromEmail = theApp.GetProfileString(_T("PREFERENCES"),_T("FROMEMAIL"),_T(""));

	UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrefEmailPage::OnDestroy() 
{
	UpdateData(true);

	theApp.WriteProfileString(_T("PREFERENCES"),_T("SMTPSERVER"),m_strSMTPServer);
	theApp.WriteProfileString(_T("PREFERENCES"),_T("FROMEMAIL"),m_strFromEmail);

	CPropertyPage::OnDestroy();
}
