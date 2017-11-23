/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// RecordProperties.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "MainFrm.h"
#include "TreeChooseDlg.h"
#include "RecordProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CG *cg;
extern CONCEPT *appPassConcept;
extern CString appAskText;

/////////////////////////////////////////////////////////////////////////////
// CRecordProperties dialog


CRecordProperties::CRecordProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CRecordProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRecordProperties)
	m_strFirst = _T("");
	m_boolIncludeLast = false;
	m_strRecordName = _T("");
	m_strZoneConcept = _T("");
	m_strLabel = _T("");
	//}}AFX_DATA_INIT
}


void CRecordProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecordProperties)
	DDX_Text(pDX, IDC_FIRST, m_strFirst);
	DDX_Check(pDX, IDC_INCLUDE_LAST, m_boolIncludeLast);
	DDX_Text(pDX, IDC_RECORD_NAME, m_strRecordName);
	DDX_Text(pDX, IDC_ZONE, m_strZoneConcept);
	DDX_Text(pDX, IDC_LABEL, m_strLabel);
	DDX_Control(pDX, IDC_ORDER_BY, m_combo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRecordProperties, CDialog)
	//{{AFX_MSG_MAP(CRecordProperties)
	ON_BN_CLICKED(IDC_ZONE_BUTTON, OnZoneButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecordProperties message handlers

void CRecordProperties::SetData(CONCEPT *concept) 
{
	m_concept = concept;
	m_strRecordName = ConceptName(concept);
}

void CRecordProperties::SetConceptAttributes()
{
	CString oldName = ConceptName(m_concept);
	UpdateData(true);

	if (oldName != m_strRecordName) {
		_TCHAR buff[1001];
		StringToChar(buff,m_strRecordName,1000);
		cg->renameConcept(m_concept,buff);
	}

	ConceptAttributeUpdate(m_concept,_T("zone"),m_strZoneConcept);
	ConceptAttributeUpdate(m_concept,_T("includelast"),BoolStr(m_boolIncludeLast));
	ConceptAttributeUpdate(m_concept,_T("first"),m_strFirst);
	ConceptAttributeUpdate(m_concept,_T("label"),m_strLabel);

	// Order By
	CString orderStr;
	int chosen = m_combo.GetCurSel();
	if (chosen >= 0) {
		m_combo.GetLBText(chosen,orderStr);
		ConceptAttributeUpdate(m_concept,_T("orderby"),orderStr);
	}
}

void CRecordProperties::GetConceptAttributes()
{
	if (!m_concept)
		return;

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

		if (attrStr == _T("includelast"))
			m_boolIncludeLast = BoolValueVT(valStr);
		else if (attrStr == _T("zone"))
			m_strZoneConcept = valStr;
		else if (attrStr == _T("first"))
			m_strFirst = valStr;
		else if (attrStr == _T("label"))
			m_strLabel = valStr;
		else if (attrStr == _T("orderby"))
			m_combo.SelectString(0,valStr);

		count++;
	}
}

int CRecordProperties::FillOrderBy()
{
	if (!m_concept)
		return 0;

	int count = 0;
	CONCEPT *items = cg->Down(m_concept);

	CString itemStr;

	while (items) {
		itemStr = ConceptName(items);
		m_combo.AddString(itemStr);
		count++;
		items = cg->Next(items);
	}

	return count;
}

BOOL CRecordProperties::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	FillOrderBy();
	GetConceptAttributes();
	SetWindowText(m_strRecordName);
	UpdateData(false);
	
	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

void CRecordProperties::OnOK() 
{
	SetConceptAttributes();
	
	CDialog::OnOK();
}

void CRecordProperties::OnZoneButton() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CONCEPT *concept = wnd->GrammarConceptSelected(true);

	CTreeChooseDlg dlg;
	dlg.SetConcept(m_strRecordName+_T(" Zone"),concept,true);

	if (dlg.DoModal() == IDOK) {
		CStringList stateList;
		TokensBySeparator(stateList,appAskText,':');
		CString numStr = NthStringListItem(stateList,0);

		if (numStr != _T("")) {
			int num = _ttoi((const _TCHAR *)numStr);
			int count = 0;
			CONCEPT *concept = NumberToConcept(appPassConcept,num,count);
			_TCHAR buff[1001];
			cg->conceptPath(concept,buff);
			m_strZoneConcept = buff;
			UpdateData(false);
		}
	}
}
