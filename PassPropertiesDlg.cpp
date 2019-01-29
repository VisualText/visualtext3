/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// PassPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "MainFrm.h"
#include "TreeViewAnalyzer.h"
#include "PassPropertiesDlg.h"

extern NLP *nlp;
extern CG *cg;
extern int appButtonReturn;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPassPropertiesDlg dialog


// IF CHANGE HERE, CHANGE IN:  PassPropertiesDlg.cpp

CPassPropertiesDlg::CPassPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPassPropertiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPassPropertiesDlg)
	m_strName = _T("");
	m_strType = _T("");
	m_boolActive = false;
	m_strFrom = _T("");
	m_strTo = _T("");
	m_boolToEnd = FALSE;
	m_boolAll = FALSE;
	//}}AFX_DATA_INIT
}


void CPassPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPassPropertiesDlg)
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_CBString(pDX, IDC_TYPE, m_strType);
	DDX_Check(pDX, IDC_ACTIVE, m_boolActive);
	DDX_CBString(pDX, IDC_FROM, m_strFrom);
	DDX_CBString(pDX, IDC_TO, m_strTo);
	DDX_Check(pDX, IDC_TO_END, m_boolToEnd);
	DDX_Check(pDX, IDC_ALL, m_boolAll);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPassPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CPassPropertiesDlg)
	ON_BN_CLICKED(IDC_TO_END, OnToEnd)
	ON_CBN_SELCHANGE(IDC_FROM, OnSelchangeFrom)
	ON_CBN_SELCHANGE(IDC_TO, OnSelchangeTo)
	ON_BN_CLICKED(IDC_ALL, OnAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPassPropertiesDlg message handlers

void CPassPropertiesDlg::SetData(CONCEPT *concept)
{
	m_concept = concept;
	m_strName = m_strNameOrig = ConceptName(concept);
	m_intPass = KBPassNumber(concept);
	m_typePass = m_typePassOrig = TypeFromString(KBGetValueString(concept,_T("algo")));
	m_strType = StringFromType(m_typePass);
	m_boolActive = KBGetValueBool(concept,_T("active"));
}

void CPassPropertiesDlg::FillCombos()
{
	int passCount = theApp.m_pMainFrame->m_wndAnalyzerView.m_treeCtrl.GetCount();
	FillCombo(IDC_FROM,1,passCount,m_intPass);
	FillCombo(IDC_TO,1,passCount,m_intPass);

	CComboBox *comboBox = (CComboBox *)GetDlgItem(IDC_TYPE);
	comboBox->AddString(_T("Pattern"));
	comboBox->AddString(_T("Recursive"));	// FIX misspelling.  // 10/17/05 AM.

}

void CPassPropertiesDlg::FillCombo(UINT comboID, int from, int to, int selected)
{
	CComboBox *comboBox = (CComboBox *)GetDlgItem(comboID);
	CString numStr;
	for (int i=from; i<=to; i++) {
		numStr.Format(_T("%d"),i);
		comboBox->AddString(numStr);
	}

	numStr.Format(_T("%d"),selected);
	CEdit *ed = (CEdit *)GetDlgItem(comboID);
	ed->SetWindowText(numStr);
}

void CPassPropertiesDlg::OnOK() 
{
	UpdateData(true);
	
	// UPDATE COMBO INFO FOR COMPARING TO ORIG
	CComboBox *typeCombo = (CComboBox *)GetDlgItem(IDC_TYPE);
	typeCombo->GetWindowText(m_strType);
	m_typePass = TypeFromUserString(m_strType);

	if (m_boolTypeable && m_typePass != m_typePassOrig)
		theApp.m_pMainFrame->m_wndAnalyzerView.RetypePass(m_concept,m_typePass);

	if (m_strName != m_strNameOrig) {
		if (!theApp.m_pMainFrame->m_wndAnalyzerView.RenamePass(m_concept,m_strName)) {
			appButtonReturn = 0;
			return;
		}
	}

	if (m_boolTypeable) {
		if (m_boolToEnd) {
			for (CONCEPT *c=m_concept; c; c = cg->Next(c))
				theApp.m_pMainFrame->m_wndAnalyzerView.SetPassActive(c,m_boolActive ? true : false);
		}
		else if (m_boolAll) {
			for (CONCEPT *c=ConceptFirstSibling(m_concept); c; c = cg->Next(c))
				theApp.m_pMainFrame->m_wndAnalyzerView.SetPassActive(c,m_boolActive ? true : false);
		}
		else if (m_strFrom != m_strTo) {
			int fromInt = _ttoi((const _TCHAR *)m_strFrom);
			int toInt = _ttoi((const _TCHAR *)m_strTo);

			CONCEPT *c = NthConcept(m_concept,fromInt);
			for (int pass=fromInt; pass<toInt; pass++) {
				theApp.m_pMainFrame->m_wndAnalyzerView.SetPassActive(c,m_boolActive ? true : false);
				c = cg->Next(c);
			}
		}
		else
			theApp.m_pMainFrame->m_wndAnalyzerView.SetPassActive(m_concept,m_boolActive ? true : false);
	}

	if (m_strName == _T(""))
		AfxMessageBox(_T("You must cancel or enter a name."));
	else {
		appButtonReturn = 1;
		CDialog::OnOK();
	}
}

CString CPassPropertiesDlg::StringFromType(ANAL_PASS_TYPE type)
{
	CString typeStr = _T("");

	if (type == ANAL_PASS_PAT)
		typeStr = _T("Pattern");	// Changed from "Rule".	// 05/19/01 AM.
	else if (type == ANAL_PASS_REC)
		typeStr = _T("Recursive");
	else if (type == ANAL_PASS_FOLDER)
		typeStr = _T("Folder");
	return typeStr;
}

ANAL_PASS_TYPE CPassPropertiesDlg::TypeFromString(CString typeStr)
{
	ANAL_PASS_TYPE type = ANAL_PASS_NONE;
	if (typeStr == _T("nlp") || typeStr == _T("pat"))
		type = ANAL_PASS_PAT;
	else if (typeStr == _T("rec"))
		type = ANAL_PASS_REC;
	else if (typeStr == _T("folder"))
		type = ANAL_PASS_FOLDER;
	return type;
}

ANAL_PASS_TYPE CPassPropertiesDlg::TypeFromUserString(CString typeStr)
{
	ANAL_PASS_TYPE type = ANAL_PASS_NONE;
	if (typeStr == _T("Pattern"))	// Changed from "Rule".	// 05/19/01 AM.
		type = ANAL_PASS_PAT;
	else if (typeStr == _T("Recursive"))
		type = ANAL_PASS_REC;
	else if (typeStr == _T("Folder"))
		type = ANAL_PASS_FOLDER;
	return type;
}

BOOL CPassPropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// SET FOCUS ON ANALYZER NAME
	CEdit *ed = (CEdit *)GetDlgItem(IDC_NAME);
	ed->SetFocus();
	ed->SetSel(0,-1);

	appButtonReturn = 0;
	m_boolTypeable = m_typePass != ANAL_PASS_FOLDER && m_typePass != ANAL_PASS_MAIN;
	
	// GRAY OUT ITEMS IF NOT TYPEABLE
	if (!m_boolTypeable) {
		CWnd *wnd = GetDlgItem(IDC_TYPE);
		wnd->EnableWindow(false);
		wnd = GetDlgItem(IDC_ACTIVE);
		wnd->EnableWindow(false);
	}
	
	FillCombos();

	UpdateData(false);

	return false;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

void CPassPropertiesDlg::OnCancel() 
{
	appButtonReturn = 0;
	
	CDialog::OnCancel();
}

void CPassPropertiesDlg::OnToEnd() 
{
	UpdateData(true);

	CWnd *fromWnd = GetDlgItem(IDC_FROM);
	CWnd *toWnd = GetDlgItem(IDC_TO);

	if (m_boolToEnd) {
		fromWnd->EnableWindow(false);
		toWnd->EnableWindow(false);
	}
	else {
		fromWnd->EnableWindow(true);
		toWnd->EnableWindow(true);
	}

	m_boolAll = false;

	UpdateData(false);
}

void CPassPropertiesDlg::OnSelchangeFrom() 
{
	UpdateData(true);

	CComboBox *fromBox = (CComboBox *)GetDlgItem(IDC_FROM);
	int nIndex = fromBox->GetCurSel();
	CString newFromStr;
	fromBox->GetLBText(nIndex,newFromStr);

	int fromInt = _ttoi((const _TCHAR *)newFromStr);
	int toInt = _ttoi((const _TCHAR *)m_strTo);
	if (fromInt > toInt)
		m_strTo = newFromStr;

	m_strFrom = newFromStr;

	UpdateData(false);
}

void CPassPropertiesDlg::OnSelchangeTo() 
{
	UpdateData(true);

	CComboBox *toBox = (CComboBox *)GetDlgItem(IDC_TO);
	int nIndex = toBox->GetCurSel();
	CString newToStr;
	toBox->GetLBText(nIndex,newToStr);

	int fromInt = _ttoi((const _TCHAR *)m_strFrom);
	int toInt = _ttoi((const _TCHAR *)newToStr);
	if (fromInt > toInt)
		m_strFrom = newToStr;

	m_strTo = newToStr;

	UpdateData(false);
}

void CPassPropertiesDlg::OnAll() 
{
	UpdateData(true);

	CWnd *fromWnd = GetDlgItem(IDC_FROM);
	CWnd *toWnd = GetDlgItem(IDC_TO);

	if (m_boolAll) {
		fromWnd->EnableWindow(false);
		toWnd->EnableWindow(false);
	}
	else {
		fromWnd->EnableWindow(true);
		toWnd->EnableWindow(true);
	}

	m_boolToEnd = false;

	UpdateData(false);
}

