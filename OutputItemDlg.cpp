/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// OutputItemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "MainFrm.h"
#include "VisualText.h"
#include "OutputItemDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CG *cg;
extern bool kbDirty;
extern CONCEPT *appPassConcept;
extern CString appAskText;

/////////////////////////////////////////////////////////////////////////////
// COutputItemDlg dialog


COutputItemDlg::COutputItemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COutputItemDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COutputItemDlg)
	m_strType = _T("");
	m_strNormalForm = _T("");
	m_strPrintName = _T("");
	m_strFieldName = _T("");
	//}}AFX_DATA_INIT
}


void COutputItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COutputItemDlg)
	DDX_CBString(pDX, IDC_TYPE, m_strType);
	DDX_CBString(pDX, IDC_NORMAL_FORM, m_strNormalForm);
	DDX_Text(pDX, IDC_PRINT_NAME, m_strPrintName);
	DDX_Text(pDX, IDC_FIELD_NAME, m_strFieldName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COutputItemDlg, CDialog)
	//{{AFX_MSG_MAP(COutputItemDlg)
	ON_EN_CHANGE(IDC_FIELD_NAME, OnChangeFieldName)
	ON_EN_KILLFOCUS(IDC_FIELD_NAME, OnKillfocusFieldName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputItemDlg message handlers

void COutputItemDlg::SetData(CONCEPT *concept) 
{
	m_concept = concept;
	m_strFieldName = ConceptName(concept);
}

BOOL COutputItemDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetConceptAttributes();
	SetWindowText(m_strFieldName);
	UpdateData(false);

	CEdit *ed = (CEdit *)GetDlgItem(IDC_FIELD_NAME);
	ed->SetFocus();
	ed->SetSel(0,m_strFieldName.GetLength());

	m_strType = _T("alpha");
	m_boolFirstTimeField = true;

	UpdateData(false);
	
	return false;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

void COutputItemDlg::GetConceptAttributes()
{
	if (!m_concept)
		return;

	m_strFieldName = origStrFieldName = ConceptName(m_concept);

	ATTR *attrs = cg->findAttrs(m_concept);
	ATTR *attr;
	VAL *vals;
	_TCHAR val[1001];
	_TCHAR buff[1001];

	CString attrStr;
	CString valStr;
	int count = 0;

	while (attrs) {
		attr = cg->popAttr(attrs);
		cg->attrName(attr,buff,1000);
		attrStr = buff;
		if (KBIsSystemAttribute(attrStr))
			continue;

		valStr = _T("");
		vals = cg->attrVals(attr);

		while (vals) {
			val[0] = '\0';
			cg->popSval(vals,val);
			valStr = val;
			break;
		}

		if (attrStr == _T("printname"))
			m_strPrintName = origStrPrintName = valStr;

		else if (attrStr == _T("type"))
			m_strType = origStrType = valStr;

		count++;
	}
}

void COutputItemDlg::SetConceptAttributes()
{
	UpdateData(true);
	if (m_strFieldName != origStrFieldName) {
		_TCHAR buff[1001];
		StringToChar(buff,m_strFieldName,1000);
		cg->renameConcept(m_concept, buff);
		appAskText = m_strFieldName;  // PASSED TO CALLING FUNCTION
		kbDirty = true;
	}

	if (m_strPrintName != origStrPrintName) {
		_TCHAR buff[1001];
		StringToChar(buff,m_strPrintName,1000);
		ConceptAttributeUpdate(m_concept,_T("printname"),m_strPrintName);
		kbDirty = true;
	}

	CString typeStr;
	CComboBox *combo = (CComboBox *)GetDlgItem(IDC_TYPE);
	int chosen = combo->GetCurSel();
	if (chosen >= 0) {
		combo->GetLBText(chosen,typeStr);
		if (typeStr != origStrType) {
			ConceptAttributeUpdate(m_concept,_T("type"),typeStr);
			kbDirty = true;
		}
	}
}

void COutputItemDlg::OnOK() 
{
	SetConceptAttributes();
	
	CDialog::OnOK();
}

void COutputItemDlg::OnChangeFieldName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	if (m_boolFirstTimeField) {
		UpdateData(true);
		m_strPrintName = m_strFieldName + _T(":");
		UpdateData(false);
	}
	
}

void COutputItemDlg::OnKillfocusFieldName() 
{
	m_boolFirstTimeField = false;
}
