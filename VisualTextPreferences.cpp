/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// VisualTextPreferences.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "VisualTextPreferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString appDirectoryStr;
extern CString appStrAuthorName;
extern CString appAnaFilePathStr;
extern CVisualTextApp theApp;
extern int appIntMaxWindowWidth;
extern int appIntMaxWindowHeight;

/////////////////////////////////////////////////////////////////////////////
// CVisualTextPreferences dialog


CVisualTextPreferences::CVisualTextPreferences(CWnd* pParent /*=NULL*/)
	: CDialog(CVisualTextPreferences::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVisualTextPreferences)
	//}}AFX_DATA_INIT
}


void CVisualTextPreferences::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVisualTextPreferences)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVisualTextPreferences, CDialog)
	//{{AFX_MSG_MAP(CVisualTextPreferences)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVisualTextPreferences message handlers

BOOL CVisualTextPreferences::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_dlgPropSheet.AddPage(&m_pageGeneral);
	m_dlgPropSheet.AddPage(&m_pageDisplay);
	m_dlgPropSheet.AddPage(&m_pageEmail);
	m_dlgPropSheet.AddPage(&m_pageArchive);
	m_dlgPropSheet.AddPage(&m_pageLookup);
	m_dlgPropSheet.AddPage(&m_pageAnalyzerDependent);

	m_dlgPropSheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
	m_dlgPropSheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_dlgPropSheet.ModifyStyle(0, WS_TABSTOP);

	CRect rcSheet;
	GetDlgItem(IDC_PREF_PROP_SHEET)->GetWindowRect(&rcSheet);
	ScreenToClient(&rcSheet);
	m_dlgPropSheet.SetWindowPos(NULL, rcSheet.left-7, rcSheet.top-7, 0, 0, 
			SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

	return false;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

