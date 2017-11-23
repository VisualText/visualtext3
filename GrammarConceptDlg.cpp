/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// GrammarConceptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "MainFrm.h"
#include "GrammarConceptDlg.h"
#include "TreeViewGrammar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CG *cg;
extern CONCEPT *appPassConcept;
extern CString appAskText;

/////////////////////////////////////////////////////////////////////////////
// CGrammarConceptDlg dialog


CGrammarConceptDlg::CGrammarConceptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGrammarConceptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGrammarConceptDlg)
	m_strName = _T("");
	m_boolClosed = false;
	m_boolClosedInherited = false;
	m_boolConstrained = false;
	m_boolConstrainedInherited = false;
	m_boolGeneralized = false;
	m_boolGeneralizedInherited = false;
	m_boolLiteral = false;
	m_boolLiteralInherited = false;
	m_boolSplit = false;
	m_boolSplitInherited = false;
	m_boolWhitespace = false;
	m_boolWhitespaceInherited = false;
	m_boolLocked = false;
	m_boolLockedInherited = false;
	m_strSampleFile = _T("");
	m_strLayered = _T("");
	m_boolQuickSemantics = false;
	m_boolQuickSemanticsInherited = false;
	m_boolRaw = false;
	m_boolRawInherited = false;
	//}}AFX_DATA_INIT

	m_boolDirtyFlag = false;
	origSampleStr = _T("");
}


void CGrammarConceptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGrammarConceptDlg)
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Check(pDX, IDC_CLOSED, m_boolClosed);
	DDX_Check(pDX, IDC_CLOSED_INHERITED, m_boolClosedInherited);
	DDX_Check(pDX, IDC_CONSTRAINED, m_boolConstrained);
	DDX_Check(pDX, IDC_CONSTRAINED_INHERITED, m_boolConstrainedInherited);
	DDX_Check(pDX, IDC_GENERALIZED, m_boolGeneralized);
	DDX_Check(pDX, IDC_GENERALIZED_INHERITED, m_boolGeneralizedInherited);
	DDX_Check(pDX, IDC_LITERAL, m_boolLiteral);
	DDX_Check(pDX, IDC_LITERAL_INHERITED, m_boolLiteralInherited);
	DDX_Check(pDX, IDC_SPLIT, m_boolSplit);
	DDX_Check(pDX, IDC_SPLIT_INHERITED, m_boolSplitInherited);
	DDX_Check(pDX, IDC_WHITESPACE, m_boolWhitespace);
	DDX_Check(pDX, IDC_WHITESPACE_INHERITED, m_boolWhitespaceInherited);
	DDX_Check(pDX, IDC_LOCKED, m_boolLocked);
	DDX_Check(pDX, IDC_LOCKED_INHERITED, m_boolLockedInherited);
	DDX_Text(pDX, IDC_SAMPLE_FILE, m_strSampleFile);
	DDX_Text(pDX, IDC_LAYER_STRING, m_strLayered);
	DDX_Check(pDX, IDC_QUICKSEM, m_boolQuickSemantics);
	DDX_Check(pDX, IDC_QUICKSEM_INHERITED, m_boolQuickSemanticsInherited);
	DDX_Check(pDX, IDC_RAW, m_boolRaw);
	DDX_Check(pDX, IDC_RAW_INHERITED, m_boolRawInherited);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGrammarConceptDlg, CDialog)
	//{{AFX_MSG_MAP(CGrammarConceptDlg)
	ON_BN_CLICKED(IDC_CLOSED_INHERITED, OnClosedInherited)
	ON_BN_CLICKED(IDC_CONSTRAINED_INHERITED, OnConstrainedInherited)
	ON_BN_CLICKED(IDC_GENERALIZED_INHERITED, OnGeneralizedInherited)
	ON_BN_CLICKED(IDC_SPLIT_INHERITED, OnSplitInherited)
	ON_BN_CLICKED(IDC_LITERAL_INHERITED, OnLiteralInherited)
	ON_BN_CLICKED(IDC_WHITESPACE_INHERITED, OnWhitespaceInherited)
	ON_BN_CLICKED(IDC_HELP_RULE_GENERATION, OnHelpRuleGeneration)
	ON_BN_CLICKED(IDC_LOCKED_INHERITED, OnLockedInherited)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_LOCKED, OnLocked)
	ON_BN_CLICKED(IDC_CLOSED, OnClosed)
	ON_BN_CLICKED(IDC_CONSTRAINED, OnConstrained)
	ON_BN_CLICKED(IDC_GENERALIZED, OnGeneralized)
	ON_BN_CLICKED(IDC_SPLIT, OnSplit)
	ON_BN_CLICKED(IDC_LITERAL, OnLiteral)
	ON_BN_CLICKED(IDC_WHITESPACE, OnWhitespace)
	ON_BN_CLICKED(IDC_RAW, OnRaw)
	ON_BN_CLICKED(IDC_RAW_INHERITED, OnRawInherited)
	ON_BN_CLICKED(IDC_QUICKSEM, OnQuicksem)
	ON_BN_CLICKED(IDC_QUICKSEM_INHERITED, OnQuicksemInherited)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGrammarConceptDlg message handlers

void CGrammarConceptDlg::SetConcept(CONCEPT *concept) 
{
	m_concept = concept;
	m_strName = ConceptName(concept);
}

bool CGrammarConceptDlg::SetConceptAttributes()
{
	CString oldName = ConceptName(m_concept);
	UpdateData(true);

	if (m_strName == _T("")) {
		m_strName = oldName;
		UpdateData(false);
		return false;
	}

	if (oldName != m_strName) {
		_TCHAR buff[1001];
		StringToChar(buff,m_strName,1000);
		//cg->renameConcept(m_concept,buff);		// 08/28/99 AM.
		Agram::renameConcept(m_concept,buff,cg);	// 08/28/99 AM.
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		theApp.m_pMainFrame->m_wndGrammarView.ChangeConceptName(m_strName);
	}

	SetConceptAttribute(_T("locked"),m_boolLocked,m_boolLockedInherited,origBoolLockedInherited);
	SetConceptAttribute(_T("closed"),m_boolClosed,m_boolClosedInherited,origBoolClosedInherited);
	SetConceptAttribute(_T("lit"),m_boolLiteral,m_boolLiteralInherited,origBoolLiteralInherited);
	SetConceptAttribute(_T("raw"),m_boolRaw,m_boolRawInherited,origBoolRawInherited);
	SetConceptAttribute(_T("quicksem"),m_boolQuickSemantics,m_boolQuickSemanticsInherited,origBoolQuickSemanticsInherited);
	SetConceptAttribute(_T("split"),m_boolSplit,m_boolSplitInherited,origBoolSplitInherited);
	SetConceptAttribute(_T("constrain"),m_boolConstrained,m_boolConstrainedInherited,origBoolConstrainedInherited);
	SetConceptAttribute(_T("gen"),m_boolGeneralized,m_boolGeneralizedInherited,origBoolGeneralizedInherited);
	SetConceptAttribute(_T("xwhite"),m_boolWhitespace,m_boolWhitespaceInherited,origBoolWhitespaceInherited);

	if (origSampleStr != m_strSampleFile)	
		ConceptAttributeUpdate(m_concept,_T("sampfile"),m_strSampleFile);

	if (origLayeredStr != m_strLayered)	
		ConceptAttributeUpdate(m_concept,_T("layer"),m_strLayered);

	return true;
}

void CGrammarConceptDlg::SetConceptAttribute(CString attrStr, int conVal, int inheritedVal, int origInheritedVal)
{
	if (m_boolDirtyFlag) {
		if (!inheritedVal)
			ConceptAttributeUpdate(m_concept,attrStr,BoolStr(conVal));
		else if (inheritedVal != origInheritedVal)
			RemoveAttribute(m_concept,attrStr);
	}
}

void CGrammarConceptDlg::GetConceptAttributes()
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

	bool locked = false;
	bool closed = false;
	bool lit = false;
	bool constrain = false;
	bool gen = false;
	bool split = false;
	bool quickSem = false;
	bool raw = false;
	bool whitespace = false;

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

		if (attrStr == _T("locked")) {
			GetValue(valStr,m_boolLocked,m_boolLockedInherited,IDC_LOCKED);
			locked = true;
		}
		else if (attrStr == _T("closed")) {
			GetValue(valStr,m_boolClosed,m_boolClosedInherited,IDC_CLOSED);
			closed = true;
		}
		else if (attrStr == _T("lit")) {
			GetValue(valStr,m_boolLiteral,m_boolLiteralInherited,IDC_LITERAL);
			lit = true;
		}
		else if (attrStr == _T("raw")) {
			GetValue(valStr,m_boolRaw,m_boolRawInherited,IDC_RAW);
			raw = true;
		}
		else if (attrStr == _T("quicksem")) {
			GetValue(valStr,m_boolQuickSemantics,m_boolQuickSemanticsInherited,IDC_QUICKSEM);
			quickSem = true;
		}
		else if (attrStr == _T("split")) {
			GetValue(valStr,m_boolSplit,m_boolSplitInherited,IDC_SPLIT);
			split = true;
		}
		else if (attrStr == _T("constrain")) {
			GetValue(valStr,m_boolConstrained,m_boolConstrainedInherited,IDC_CONSTRAINED);
			constrain = true;
		}
		else if (attrStr == _T("gen")) {
			GetValue(valStr,m_boolGeneralized,m_boolGeneralizedInherited,IDC_GENERALIZED);
			gen = true;
		}
		else if (attrStr == _T("xwhite")) {
			GetValue(valStr,m_boolWhitespace,m_boolWhitespaceInherited,IDC_WHITESPACE);
			whitespace = true;
		}
		else if (attrStr == _T("sampfile")) {
			m_strSampleFile = valStr;
		}
		else if (attrStr == _T("layer")) {
			m_strLayered = valStr;
		}

		count++;
	}

	if (!locked) {
		m_boolLockedInherited = true;
		UpdateInherited(IDC_LOCKED,m_boolLocked,m_boolLockedInherited);
	}
	if (!closed) {
		m_boolClosedInherited = true;
		UpdateInherited(IDC_CLOSED,m_boolClosed,m_boolClosedInherited);
	}
	if (!constrain) {
		m_boolConstrainedInherited = true;
		UpdateInherited(IDC_CONSTRAINED,m_boolConstrained,m_boolConstrainedInherited);
	}
	if (!lit) {
		m_boolLiteralInherited = true;
		UpdateInherited(IDC_LITERAL,m_boolLiteral,m_boolLiteralInherited);
	}
	if (!raw) {
		m_boolRawInherited = true;
		UpdateInherited(IDC_RAW,m_boolRaw,m_boolRawInherited);
	}
	if (!quickSem) {
		m_boolQuickSemanticsInherited = true;
		UpdateInherited(IDC_QUICKSEM,m_boolQuickSemantics,m_boolQuickSemanticsInherited);
	}
	if (!split) {
		m_boolSplitInherited = true;
		UpdateInherited(IDC_SPLIT,m_boolSplit,m_boolSplitInherited);
	}
	if (!gen) {
		m_boolGeneralizedInherited = true;
		UpdateInherited(IDC_GENERALIZED,m_boolGeneralized,m_boolGeneralizedInherited);
	}
	if (!whitespace) {
		m_boolWhitespaceInherited = true;
		UpdateInherited(IDC_WHITESPACE,m_boolWhitespace,m_boolWhitespaceInherited);
	}

	origBoolClosedInherited = m_boolClosedInherited;
	origBoolConstrainedInherited = m_boolConstrainedInherited;
	origBoolGeneralizedInherited = m_boolGeneralizedInherited;
	origBoolLiteralInherited = m_boolLiteralInherited;
	origBoolRawInherited = m_boolRawInherited;
	origBoolQuickSemanticsInherited = m_boolQuickSemanticsInherited;
	origBoolSplitInherited = m_boolSplitInherited;
	origBoolWhitespaceInherited = m_boolWhitespaceInherited;
	origBoolLockedInherited = m_boolLockedInherited;

	origSampleStr = m_strSampleFile;
	origLayeredStr = m_strLayered;
}

void CGrammarConceptDlg::GetValue(CString valStr, BOOL &value, BOOL &inherited, UINT dlgItem) 
{
	int valTri = TriStateValue(valStr);
	CWnd *wnd = (CWnd *)GetDlgItem(dlgItem);

	switch (valTri) {
	case 0:
		value = false;
		inherited = true;
		wnd->EnableWindow(false);
		break;
	case 1:
		value = true;
		inherited = false;
		wnd->EnableWindow(true);
		break;
	case 2:
		value = false;
		inherited = false;
		wnd->EnableWindow(true);
		break;
	}
}

BOOL CGrammarConceptDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetConceptAttributes();
	UpdateData(false);
	
	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

void CGrammarConceptDlg::OnOK() 
{
	if (SetConceptAttributes())
		CDialog::OnOK();
}

CString CGrammarConceptDlg::AttrStringFromID(UINT dlgItemID) 
{
	CString attrStr = _T("");

	switch (dlgItemID) {
	case IDC_LOCKED: attrStr = _T("locked"); break;
	case IDC_CLOSED: attrStr = _T("closed"); break;
	case IDC_LITERAL: attrStr = _T("lit"); break;
	case IDC_RAW: attrStr = _T("raw"); break;
	case IDC_QUICKSEM: attrStr = _T("quicksem"); break;
	case IDC_SPLIT: attrStr = _T("split"); break;
	case IDC_GENERALIZED: attrStr = _T("gen"); break;
	case IDC_WHITESPACE: attrStr = _T("xwhite"); break;
	case IDC_CONSTRAINED: attrStr = _T("constrain"); break;
	}
	
	return attrStr;
}

void CGrammarConceptDlg::UpdateInherited(UINT dlgItemID, BOOL &value, BOOL inherited) 
{
	CWnd *closedItem = (CWnd *)GetDlgItem(dlgItemID);

	if (inherited) {
		closedItem->EnableWindow(false);
		value = false;

		_TCHAR attrBuff[1001];
		_TCHAR valBuff[1001];
		CONCEPT *gram = cg->findConcept(cg->findRoot(), _T("gram"));
		CString attrStr = AttrStringFromID(dlgItemID);
		StringToChar(attrBuff,attrStr,1000);
		if (cg->inheritVal(m_concept,attrBuff,gram,valBuff)) {
			CString valStr = valBuff;
			value = BoolValueVT(valStr);
		}
	}
	else
		closedItem->EnableWindow(true);

	UpdateData(false);
}

void CGrammarConceptDlg::OnClosedInherited() 
{
	UpdateData(true);
	UpdateInherited(IDC_CLOSED,m_boolClosed,m_boolClosedInherited);
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnConstrainedInherited() 
{
	UpdateData(true);
	UpdateInherited(IDC_CONSTRAINED,m_boolConstrained,m_boolConstrainedInherited);
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnGeneralizedInherited() 
{
	UpdateData(true);
	UpdateInherited(IDC_GENERALIZED,m_boolGeneralized,m_boolGeneralizedInherited);
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnSplitInherited() 
{
	UpdateData(true);
	UpdateInherited(IDC_SPLIT,m_boolSplit,m_boolSplitInherited);
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnLiteralInherited() 
{
	UpdateData(true);
	UpdateInherited(IDC_LITERAL,m_boolLiteral,m_boolLiteralInherited);
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnWhitespaceInherited() 
{
	UpdateData(true);
	UpdateInherited(IDC_WHITESPACE,m_boolWhitespace,m_boolWhitespaceInherited);
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnHelpRuleGeneration() 
{
	HelpTopic(_T("Setting_Rule_Generation_Properties"));
}

void CGrammarConceptDlg::OnLockedInherited() 
{
	UpdateData(true);
	UpdateInherited(IDC_LOCKED,m_boolLocked,m_boolLockedInherited);
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnBrowse() 
{
	CFileDialog fileDlg(true,_T("txt"),InputPath()+_T("\\*.txt"));
	if (fileDlg.DoModal() == IDOK) {
		CString filePathStr = fileDlg.GetPathName();
		m_strSampleFile = RelativizePath(filePathStr);
		UpdateData(false);
		m_boolDirtyFlag = true;	
	}
}

void CGrammarConceptDlg::OnLocked() 
{
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnClosed() 
{
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnConstrained() 
{
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnGeneralized() 
{
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnSplit() 
{
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnLiteral() 
{
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnWhitespace() 
{
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnRaw() 
{
	m_boolDirtyFlag = true;		
}

void CGrammarConceptDlg::OnRawInherited() 
{
	UpdateData(true);
	UpdateInherited(IDC_RAW,m_boolRaw,m_boolRawInherited);
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnQuicksem() 
{
	m_boolDirtyFlag = true;	
}

void CGrammarConceptDlg::OnQuicksemInherited() 
{
	UpdateData(true);
	UpdateInherited(IDC_RAW,m_boolQuickSemantics,m_boolQuickSemantics);
	m_boolDirtyFlag = true;	
}
