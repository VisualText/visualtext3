/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/
// AboutDlg.cpp : implementation file
//
// This file is a part of the Xtreme Toolkit for MFC.
// ©1998-2003 Codejock Software, All Rights Reserved.
//
// This source code can only be used under the terms and conditions
// outlined in the accompanying license agreement.
//
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "AboutDlg.h"

extern CString appStrVersion;
extern CString appStrCopyright;
extern CString appStrCopyright2;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_ABOUTBOX, pParent)
{
	//{{AFX_DATA_INIT(CAboutDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_TXT_COPYRIGHT, m_txtCopyright);
	DDX_Control(pDX, IDC_TXT_URL, m_txtURL);
	DDX_Control(pDX, IDC_VERSION, m_txtVersion);
	DDX_Control(pDX, IDC_TXT_EMAIL, m_txtEmail);
	DDX_Control(pDX, IDOK, m_btnOk);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg message handlers

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// get a pointer to CWinApp.
	CWinApp* pApp = AfxGetApp( );
	ASSERT( pApp != NULL );

	// construct the about title.
	CString strAppName;
//	strAppName.Format( _T( "%s Sample" ), pApp->m_pszAppName );	// 01/04/09 AM.
	strAppName.Format( _T( "%s" ), pApp->m_pszAppName );	// 01/04/09 AM.

	// set the about dialog's title.
	CString strWindowText;
	strWindowText.Format( _T("About %s" ), strAppName );
	SetWindowText( strWindowText );

	// construct the copyright text.
	CString strCopyright;
	strCopyright.Format( _T( "©1998-%d Codejock Software, All Rights Reserved" ),
		CTime::GetCurrentTime( ).GetYear( ) );

	strCopyright = appStrCopyright + _T(" -- ") + appStrCopyright2 + _T(" -- ") + strCopyright;
	m_txtCopyright.SetWindowText( strCopyright );

	m_txtVersion.SetWindowText(appStrVersion);

	// define the url for our hyperlinks.
	m_txtURL.SetWindowText( _T( "http://www.textanalysis.com" ) );
	m_txtURL.SetURL( _T( "http://www.textanalysis.com" ) );
	m_txtURL.SetUnderline( false );
	
	m_txtEmail.SetWindowText( _T( "support@textanalysis.com" ) );
	m_txtEmail.SetURL( _T( "mailto:support@textanalysis.com" ) );
	m_txtEmail.SetUnderline( false );

	// set OK button style.
	//m_btnOk.SetXButtonStyle( BS_XT_SEMIFLAT | BS_XT_HILITEPRESSED );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
