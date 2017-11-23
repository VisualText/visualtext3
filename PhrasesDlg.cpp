/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// PhrasesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "MainFrm.h"
#include "AttributeEditor.h"
#include "PhrasesDlg.h"

extern CString appAskText;
extern CG *cg;
extern NLP *nlp;
extern bool kbDirty;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhrasesDlg dialog


CPhrasesDlg::CPhrasesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPhrasesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPhrasesDlg)
	//}}AFX_DATA_INIT
}


void CPhrasesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPhrasesDlg)
	DDX_Control(pDX, IDC_LIST, m_listCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPhrasesDlg, CDialog)
	//{{AFX_MSG_MAP(CPhrasesDlg)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_ATTRIBUTES, OnAttributes)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_BN_CLICKED(IDC_DRAG_UP, OnDragUp)
	ON_BN_CLICKED(IDC_DRAG_DOWN, OnDragDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhrasesDlg message handlers

void CPhrasesDlg::SetData(CONCEPT *concept)
{
	m_concept = concept;
}

void CPhrasesDlg::FillPhraseList()
{
	PHRASE *phrase;
	CString conceptStr;
	int i = 0;

	if ((phrase = cg->findPhrase(m_concept))) {

		while (phrase) {
			conceptStr = ConceptName(phrase);
			m_listCtrl.InsertItem(i,conceptStr);
			m_listCtrl.SetItemData(i,(DWORD)phrase);
			phrase = cg->Next(phrase);
			i++;
		}
		if (i > 0)
			SelectAndFocus(0);
	}
}

BOOL CPhrasesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_bReadOnlyMode = false;

	VERIFY(bitmapButtonUp.AutoLoad(IDC_DRAG_UP,this));
	VERIFY(bitmapButtonDown.AutoLoad(IDC_DRAG_DOWN,this));

	FillPhraseList();
	CString strWindowTitle;
	strWindowTitle.Format(_T("Phrase Tool (%s)"), ConceptName(m_concept));
	SetWindowText(strWindowTitle);

	if (m_bReadOnlyMode) {
		Enable(IDC_DELETE,false);
		Enable(IDC_ADD,false);
		Enable(IDC_DRAG_UP,false);
		Enable(IDC_DRAG_DOWN,false);
	}
	else {
		SetButtonStates();
	}

	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

void CPhrasesDlg::OnDelete() 
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();

	if (pos == NULL) {
		AfxMessageBox(_T("No nodes selected!"));
		return;
	}

	CString msgStr = _T("Are you sure you want to delete the selected node(s)?");

	if (AfxMessageBox(msgStr,MB_YESNO) == IDYES) {

		CString phrase;
		CONCEPT *conceptSelected;
		int nItem;

		while ((nItem = m_listCtrl.GetNextSelectedItem(pos)) >= 0) {
			conceptSelected = (CONCEPT *)m_listCtrl.GetItemData(nItem);
			Agram::rmSample(conceptSelected,cg);
			m_listCtrl.DeleteItem(nItem);
			kbDirty = true;
			SetButtonStates();
		}
	}
}

int CPhrasesDlg::GetFirstSelectedItemNumber(bool warnFlag) 
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();

	if (pos == NULL) {
		if (warnFlag)
			AfxMessageBox(_T("No nodes selected!"));
		return -1;
	}

	int currentSelection = -1;
	int firstSelected = -1;

	while ((currentSelection = m_listCtrl.GetNextSelectedItem(pos)) >= 0) {
		if (firstSelected == -1)
			firstSelected = currentSelection;
	}

	return firstSelected;
}

void CPhrasesDlg::OnAttributes() 
{
	InvokeAttributeEditor();
}

void CPhrasesDlg::InvokeAttributeEditor() 
{
	int itemNumber = -1;
	if ((itemNumber = GetFirstSelectedItemNumber()) >= 0) {
		CONCEPT *conceptSelected = (CONCEPT *)m_listCtrl.GetItemData(itemNumber);
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		wnd->OpenAttributeEditor(conceptSelected, false);
	}
}

void CPhrasesDlg::OnAdd() 
{
	CString msgStr;
//	msgStr.Format("Node to add to \"%s\":",ConceptName(m_concept));
	msgStr.Format(_T("Enter name of node:"));

	if (AskForText(msgStr)) {
		_TCHAR buff[1001];
		StringToChar(buff,appAskText,1000);
		CONCEPT *phraseConcept = Agram::addSample(m_concept,buff,cg);
		kbDirty = true;

		int count = m_listCtrl.GetItemCount();
		m_listCtrl.InsertItem(count,appAskText);
		m_listCtrl.SetItemData(count,(DWORD)phraseConcept);
		SetButtonStates();
	}	
}

void CPhrasesDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeAttributeEditor();
	
	*pResult = 0;
}

void CPhrasesDlg::Enable(UINT id, BOOL value)
{
	CWnd *wnd = (CWnd *)GetDlgItem(id);
	wnd->EnableWindow(value);
}

void CPhrasesDlg::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetButtonStates();	
	*pResult = 0;
}

void CPhrasesDlg::SetButtonStates() 
{
	if (m_bReadOnlyMode)
		return;

	if (m_listCtrl.GetItemCount() == 0) {
		Enable(IDC_DELETE,false);
		Enable(IDC_ATTRIBUTES,false);
	}

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->ReIconKB(m_concept);

	int firstSelected = GetFirstSelectedItemNumber(false);

	// SRP 010505
	if (-1 == firstSelected) {
		Enable(IDC_DRAG_UP,false);
		Enable(IDC_DRAG_DOWN,false);
		return;
	}

	if (0 == firstSelected)
		Enable(IDC_DRAG_UP,false);
	else
		Enable(IDC_DRAG_UP,true);

	if (m_listCtrl.GetItemCount() - 1 == firstSelected)
		Enable(IDC_DRAG_DOWN,false);
	else
		Enable(IDC_DRAG_DOWN,true);
}

void CPhrasesDlg::OnDragUp() 
{
	int first = GetFirstSelectedItemNumber();
	CONCEPT *concept = (CONCEPT *)m_listCtrl.GetItemData(first);

	if (cg->moveNleft(concept)) {
		CString itemStr = m_listCtrl.GetItemText(first,0);
		m_listCtrl.DeleteItem(first--);
		int newItem = m_listCtrl.InsertItem(first,itemStr);
		m_listCtrl.SetItemData(newItem,(DWORD)concept);
	}

	SelectAndFocus(first);
}

void CPhrasesDlg::OnDragDown() 
{
	int first = GetFirstSelectedItemNumber();
	CONCEPT *concept = (CONCEPT *)m_listCtrl.GetItemData(first);

	if (cg->moveNright(concept)) {
		CString itemStr = m_listCtrl.GetItemText(first,0);
		m_listCtrl.DeleteItem(first++);
		int newItem = m_listCtrl.InsertItem(first,itemStr);
		m_listCtrl.SetItemData(newItem,(DWORD)concept);
	}

	SelectAndFocus(first);
}

void CPhrasesDlg::SelectAndFocus(int item)
{
	m_listCtrl.SetItemState(item,
		LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED);
	m_listCtrl.SetFocus();
	m_listCtrl.EnsureVisible(item,false);

	SetButtonStates();
}
