/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// TreeChooseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"

#include "MainFrm.h"
#include "TreeChooseDlg.h"

extern CG *cg;
extern CVisualTextApp theApp;
extern CString appAskText;
extern CONCEPT *appPassConcept;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeChooseDlg dialog


CTreeChooseDlg::CTreeChooseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTreeChooseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTreeChooseDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	appAskText = _T("");

	// NOT CURRENTLY USED
	m_boolSingleChoice = false;
	m_itemSelected = NULL;
	m_prevItemSelected = NULL;
}


void CTreeChooseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTreeChooseDlg)
	DDX_Control(pDX, IDC_TREE_CHOICES, m_TreeCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTreeChooseDlg, CDialog)
	//{{AFX_MSG_MAP(CTreeChooseDlg)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_CHILDREN, OnChildren)
	ON_BN_CLICKED(IDC_UNCHECK, OnUncheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeChooseDlg message handlers

BOOL CTreeChooseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	// create the image list for the tree control
	m_ImageList.Create (IDB_GRAMMAR, 16, 1, RGB(0,255,0));
	m_TreeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);

	FillTree();
	SetWindowText(m_strConceptName);

	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

void CTreeChooseDlg::FillTree()
{
	CStringList conceptList;
	ConceptHierarchyToStringList(conceptList,m_concept,0,false);
	CString conceptStr;
	HTREEITEM child;
	HTREEITEM parents[20];

	int i = 0;
	int indent = 0;

	while ((conceptStr = NthStringListItem(conceptList,i++)) != _T("")) {
		indent = StripIndent(conceptStr);
		indent = indent / CONCEPT_MENU_INDENT;

		if (indent)
			child = m_TreeCtrl.InsertItem(conceptStr,0,1,parents[indent-1]);
		else
			child = m_TreeCtrl.InsertItem(conceptStr,0,1);

		parents[indent] = child;
	}

	HTREEITEM root = m_TreeCtrl.GetRootItem();
	m_TreeCtrl.Expand(root,TVE_EXPAND);
}

void CTreeChooseDlg::SetConcept(CString conceptStr, CONCEPT *concept, bool singleChoice)
{
	m_concept = concept;
	m_strConceptName = conceptStr;
	m_boolSingleChoice = singleChoice;
}

void CTreeChooseDlg::OnUp() 
{
	CONCEPT *parentConcept = cg->Up(m_concept);
	CString conceptStr = ConceptName(parentConcept);

	if (conceptStr == _T("root"))
		AfxMessageBox(_T("At Top!"));
	else {
		StartWaitCursor();
		m_concept = parentConcept;
		SetRedraw(false);
		m_TreeCtrl.DeleteAllItems();
		SetRedraw(true);
		FillTree();
		StopWaitCursor();
	}
}

void CTreeChooseDlg::OnOK() 
{
	appPassConcept = m_concept;
	CString moose = ConceptName(appPassConcept);
	appAskText = StateString(&m_TreeCtrl,m_TreeCtrl.GetRootItem(),STATE_GET_CHECKED);

	CDialog::OnOK();
}

void CTreeChooseDlg::OnChildren() 
{
	SetChildrenChecked(m_TreeCtrl.GetSelectedItem());	
}

void CTreeChooseDlg::SetChildrenChecked(HTREEITEM item) 
{
	HTREEITEM child = m_TreeCtrl.GetChildItem(item);
	
	while (child) {
		m_TreeCtrl.SetCheck(child,true);
		child = m_TreeCtrl.GetNextItem(child,TVGN_NEXT);
	}
}

void CTreeChooseDlg::OnUncheck() 
{
	CheckAll(m_TreeCtrl.GetRootItem(),false);	
}

void CTreeChooseDlg::CheckAll(HTREEITEM item, bool value) 
{
	if (!item)
		return;

	m_TreeCtrl.SetCheck(item,value);
	HTREEITEM sibling = m_TreeCtrl.GetChildItem(item);
	
	while (sibling) {
		CheckAll(sibling,value);
		sibling = m_TreeCtrl.GetNextItem(sibling,TVGN_NEXT);
	}
}
