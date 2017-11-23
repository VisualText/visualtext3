/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// PrefAnalyzerDependent.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "visualtext.h"
#include "PrefAnalyzerDependent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CVisualTextApp theApp;
extern bool appBoolAnalyzerLoaded;
extern bool appBoolLoadCompiledKB;
extern CString appNameStr;

/////////////////////////////////////////////////////////////////////////////
// CPrefAnalyzerDependent property page

IMPLEMENT_DYNCREATE(CPrefAnalyzerDependent, CPropertyPage)

CPrefAnalyzerDependent::CPrefAnalyzerDependent() : CPropertyPage(CPrefAnalyzerDependent::IDD)
{
	//{{AFX_DATA_INIT(CPrefAnalyzerDependent)
	m_boolLoadCompiledKB = FALSE;
	//}}AFX_DATA_INIT
}

CPrefAnalyzerDependent::~CPrefAnalyzerDependent()
{
}

void CPrefAnalyzerDependent::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrefAnalyzerDependent)
	DDX_Check(pDX, IDC_LOAD_COMPILED_KB, m_boolLoadCompiledKB);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrefAnalyzerDependent, CPropertyPage)
	//{{AFX_MSG_MAP(CPrefAnalyzerDependent)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefAnalyzerDependent message handlers

BOOL CPrefAnalyzerDependent::OnInitDialog() 
{
	if (appBoolAnalyzerLoaded) {
		m_boolLoadCompiledKB = appBoolLoadCompiledKB;
		SetDlgItemText(IDC_ANALYZER, appNameStr);
	}
	else {
		SetDlgItemText(IDC_ANALYZER, _T("No Analyzer Currently Loaded"));
		GetDlgItem(IDC_LOAD_COMPILED_KB)->EnableWindow(false);
	}

	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrefAnalyzerDependent::OnDestroy() 
{
	UpdateData();

	if (appBoolAnalyzerLoaded) {
//		appBoolLoadCompiledKB /*BOOL*/ = m_boolLoadCompiledKB; /*bool*/
//		Substituted the following ONLY to avoid compiler warning because of different types above
		appBoolLoadCompiledKB = (bool)BoolValueVT(BoolStr(m_boolLoadCompiledKB));
		theApp.SaveAnalyzerStatusFile();
	}

	CPropertyPage::OnDestroy();
	
	// TODO: Add your message handler code here
	
}
