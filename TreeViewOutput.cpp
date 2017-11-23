/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// TreeViewOutput.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "resource.h"

#include "FTPClient.h"
#include "MainFrm.h"
#include "TreeChooseDlg.h"
#include "RecordProperties.h"
#include "OutputItemDlg.h"
#include "AttributeEditor.h"
#include "TreeViewOutput.h"

extern CString appNameStr;
extern CString appAskText;
extern CVisualTextApp theApp;
extern CONCEPT *appPassConcept;

extern CG *cg;
extern bool kbDirty;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeViewOutput

IMPLEMENT_DYNCREATE(CTreeViewOutput, CTreeView)

CTreeViewOutput::CTreeViewOutput()
{
	InitVars();
}

CTreeViewOutput::~CTreeViewOutput()
{
}

void CTreeViewOutput::InitVars()
{
	m_itemSelected = NULL;
	m_conceptSelected = NULL;
	m_conceptOutput = NULL;
	m_strConcept = _T("");
	m_bLDragging = false;
	m_NodeType = OUTPUT_NODE_RECORD;

	m_noDropCursor = AfxGetApp()->LoadStandardCursor(IDC_NO) ;
	m_dropCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW) ;
}

BEGIN_MESSAGE_MAP(CTreeViewOutput, CTreeView)
	//{{AFX_MSG_MAP(CTreeViewOutput)
	ON_COMMAND(ID_OUTPUTMENU_ADDOUTPUT, OnOutputmenuAddoutput)
	ON_WM_CREATE()
	ON_COMMAND(ID_OUTPUTMENU_ARCHIVEOUTPUT, OnOutputmenuArchiveoutput)
	ON_COMMAND(ID_OUTPUTMENU_LOCALARCHIVE, OnOutputmenuLocalarchive)
	ON_COMMAND(ID_OUTPUTMENU_SERVERARCHIVE, OnOutputmenuServerarchive)
	ON_COMMAND(ID_OUTPUTMENU_UPLOADOUTPUT, OnOutputmenuUploadoutput)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_COMMAND(ID_OUTPUTMENU_ADDITEM, OnOutputmenuAdditem)
	ON_COMMAND(ID_OUTPUTMENU_DELETE, OnOutputmenuDelete)
	ON_COMMAND(ID_OUTPUTMENU_EDITATTRIBUTES, OnOutputmenuEditattributes)
	ON_UPDATE_COMMAND_UI(ID_OUTPUTMENU_ADDITEM, OnUpdateOutputmenuAdditem)
	ON_UPDATE_COMMAND_UI(ID_OUTPUTMENU_DELETE, OnUpdateOutputmenuDelete)
	ON_UPDATE_COMMAND_UI(ID_OUTPUTMENU_EDITATTRIBUTES, OnUpdateOutputmenuEditattributes)
	ON_UPDATE_COMMAND_UI(ID_OUTPUTMENU_ARCHIVEOUTPUT, OnUpdateOutputmenuArchiveoutput)
	ON_UPDATE_COMMAND_UI(ID_OUTPUTMENU_UPLOADOUTPUT, OnUpdateOutputmenuUploadoutput)
	ON_UPDATE_COMMAND_UI(ID_OUTPUTMENU_ADDOUTPUT, OnUpdateOutputmenuAddoutput)
	ON_COMMAND(ID_OUTPUTMENU_PROPERTIES, OnOutputmenuProperties)
	ON_UPDATE_COMMAND_UI(ID_OUTPUTMENU_PROPERTIES, OnUpdateOutputmenuProperties)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeViewOutput drawing

void CTreeViewOutput::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CTreeViewOutput diagnostics

#ifdef _DEBUG
void CTreeViewOutput::AssertValid() const
{
	CTreeView::AssertValid();
}

void CTreeViewOutput::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTreeViewOutput message handlers

CONCEPT *CTreeViewOutput::AddField(CONCEPT *recordConcept, CString fieldNameStr)
{
	HTREEITEM newTreeItem = TreeItemFromConcept(recordConcept,m_treeCtrl.GetRootItem());
	m_conceptSelected = recordConcept;
	return AddConcept(fieldNameStr,newTreeItem,true);
}

CONCEPT *CTreeViewOutput::AddConcept(CString conceptStr, HTREEITEM m_itemSelected, bool isItem)
{
	CStringList conceptList;
	TokensBySeparator(conceptList,conceptStr,',');
	int count = conceptList.GetCount();
	CString conceptAddStr;
	_TCHAR buff[1001];
	HTREEITEM newItem;
	HTREEITEM lastConceptItem;
	CONCEPT *newConcept = NULL;
	bool expand = false;

	SetRedraw(false);

	int i = 0;
	int imageNum = 0;
	if (isItem)
		imageNum += 2;

	CONCEPT *parentConcept = m_conceptSelected;
	if (!parentConcept)
		parentConcept = m_conceptOutput;

	while ((conceptAddStr = NthStringListItem(conceptList,i++)) != _T("")) {

		if (m_itemSelected) {
			if (!(lastConceptItem = FindLastConceptSibling(&m_treeCtrl,m_itemSelected)))
				lastConceptItem = TVI_FIRST;
			newItem = m_treeCtrl.InsertItem(conceptAddStr,imageNum,imageNum+1,m_itemSelected,lastConceptItem);
			expand = true;
		}
		else {
			newItem = m_treeCtrl.InsertItem(conceptAddStr,imageNum,imageNum+1);
		}

		StringToChar(buff,conceptAddStr,1000);
		newConcept = cg->makeConcept(parentConcept,buff);

		m_treeCtrl.SetItemData(newItem,(DWORD)newConcept);
		kbDirty = true;
	}

	if (expand)
		m_treeCtrl.Expand(m_itemSelected,TVE_EXPAND);

	SetRedraw(true);

	return newConcept;
}

int CTreeViewOutput::FillList()
{
	HTREEITEM parentItem = m_treeCtrl.GetRootItem();
	int retValue = FillList(m_conceptOutput,parentItem);
	m_treeCtrl.UpdateWindow();
	return retValue;
}

int CTreeViewOutput::FillList(CONCEPT *parentConcept, HTREEITEM parentItem)
{
	if (!parentConcept)
		return 0;

	SetRedraw(false);

	int count = 0;
	CONCEPT *concept = cg->Down(parentConcept);

	if (concept == parentConcept)
		return 0;

	CString conceptStr;
	HTREEITEM child;
	CONCEPT *childConcept;

	while (concept) {
		conceptStr = ConceptName(concept);
		child = m_treeCtrl.InsertItem(conceptStr,0,1,parentItem);
		childConcept = cg->Down(concept);

		while (childConcept) {
			conceptStr = ConceptName(childConcept);
			m_treeCtrl.InsertItem(conceptStr,2,3,child);
			childConcept = cg->Next(childConcept);
		}
		count++;
		concept = cg->Next(concept);
	}

	SetRedraw(true);

	return count;
}

void CTreeViewOutput::Load()
{
	CONCEPT *root = cg->findRoot();
	m_conceptOutput = cg->findConcept(root, _T("output"));
	if (!m_conceptOutput)
		m_conceptOutput = cg->makeConcept(root, _T("output"));

	FillList();
}

void CTreeViewOutput::Clear()
{
	if (m_treeCtrl) {
		SetRedraw(false);
		m_treeCtrl.DeleteAllItems();
		SetRedraw(true);
	}
	InitVars();
}

void CTreeViewOutput::MoveSelUp()
{
	if (cg->moveNleft(m_conceptSelected)) {
		MoveTreeNodeUp(&m_treeCtrl,m_itemSelected,0,1);
		kbDirty = true;
	}
	else if (cg->moveCleft(m_conceptSelected)) {
		MoveTreeNodeUp(&m_treeCtrl,m_itemSelected,2,3);
		kbDirty = true;
	}
}

void CTreeViewOutput::MoveSelDown()
{
	if (cg->moveNright(m_conceptSelected)) {
		MoveTreeNodeDown(&m_treeCtrl,m_itemSelected,0,1);
		kbDirty = true;
	}
	else if (cg->moveCright(m_conceptSelected)) {
		MoveTreeNodeDown(&m_treeCtrl,m_itemSelected,2,3);
		kbDirty = true;
	}
}

void CTreeViewOutput::SelectUpdate()
{
	int count = m_treeCtrl.GetCount();
	if (count == 0)
		return;

	m_itemSelected = m_treeCtrl.GetSelectedItem();

	m_conceptSelected = GetParentConcept(m_itemSelected,true);

	m_strConcept = m_treeCtrl.GetItemText(m_itemSelected);

	int image;
	int imageSel;
	m_treeCtrl.GetItemImage(m_itemSelected,image,imageSel);
	switch (image) {
	case 0:
	case 1:
		m_NodeType = OUTPUT_NODE_RECORD;
		break;
	case 2:
	case 3:
		m_NodeType = OUTPUT_NODE_ITEM;
		break;
	}
}

CONCEPT *CTreeViewOutput::GetParentConcept(HTREEITEM childItem, bool selected)
{
	if (!childItem)
		return m_conceptOutput;

	HTREEITEM item = childItem;
	int positions[20];
	int depth = 0;
	int pos;
	CString str;

	while (item && depth < 20) {
		str = m_treeCtrl.GetItemText(item);
		pos = TreeCtrlItemPosition(&m_treeCtrl,item);
		positions[depth++] = pos;
		item = m_treeCtrl.GetParentItem(item);
	}
	positions[depth] = 0;

	CONCEPT *concept = m_conceptOutput;
	CONCEPT *lastConcept = NULL;
	CONCEPT *child;
	PHRASE *phrase;
	bool isPhrase = false;

	int j = 0;
	_TCHAR buff[1001];

	for (j=depth-1; j>=0; j--) {
		pos = positions[j];
		concept = cg->findConcept(concept,pos);
		cg->conceptName(concept,buff);

		if (!concept) {
			concept = lastConcept;

			// PASS CONCEPTS TO GET TO SAMPLES
			child = cg->Down(concept);
			pos -= ConceptCount(child);

			phrase = cg->findPhrase(concept);
			concept = cg->findNode(phrase,pos);
			break;
		}

		lastConcept = concept;
	}

	if (!concept)
		concept = m_conceptOutput;

	cg->conceptName(concept,buff);

	return concept;
}

void CTreeViewOutput::OnOutputmenuAddoutput() 
{
 	if (AskForText(_T("Enter Record Name:\n(more than one separated by commas)"))) {
		AddConcept(appAskText);
	}	
}

int CTreeViewOutput::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
			
	// TODO: Add your specialized creation code here
	//m_treeCtrl = &GetTreeCtrl();
	
	// create the image list for the tree control
	m_ImageList.Create(IDB_OUTPUT, 16, 1, RGB(0,255,0));
	m_treeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);
	
	return 0;
}

CString CTreeViewOutput::GetState()
{
	return StateString(&m_treeCtrl,m_treeCtrl.GetRootItem(),STATE_GET_EXPANDED);
}
void CTreeViewOutput::SetState(CString stateStr)
{
	//SetRedraw(false);
	StateFromString(&(GetTreeCtrl()),m_treeCtrl.GetRootItem(),STATE_EXPAND_COLLAPSE,stateStr);
	//SetRedraw(true);
}

CString CTreeViewOutput::GetSelection()
{
	return StateString(&m_treeCtrl,m_treeCtrl.GetRootItem(),STATE_GET_SELECTED);
}

void CTreeViewOutput::SetSelection(CString selStr)
{
	//SetRedraw(false);
	StateFromString(&(GetTreeCtrl()),m_treeCtrl.GetRootItem(),STATE_SELECT,selStr);
	//SetRedraw(true);
}

CString CTreeViewOutput::TreePath(HTREEITEM child)
{
	if (!child)
		child = m_treeCtrl.GetSelectedItem();
	if (!child) 
		return _T("");

	HTREEITEM parent;
	CString pathStr = m_treeCtrl.GetItemText(child);
	CString parentStr;
	
	while (parent = m_treeCtrl.GetParentItem(child)) {
		parentStr = m_treeCtrl.GetItemText(parent);
		pathStr = parentStr + OUTPUTCONSEP + pathStr;
		child = parent;
	}
	return pathStr;
}

CString CTreeViewOutput::Archive()
{
	CString pathStr = KBPath(_T("user"));
	return ArchiveFiles(pathStr,_T("Grammar"),_T("*.kb"));
}

void CTreeViewOutput::OnOutputmenuArchiveoutput() 
{
	Archive();
}

void CTreeViewOutput::OnOutputmenuLocalarchive() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->LocalArchive(_T("Output"));
}

void CTreeViewOutput::OnOutputmenuServerarchive() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->ServerArchive(_T("Output"));
}

void CTreeViewOutput::OnOutputmenuUploadoutput() 
{
	CString localFileStr = Archive();
	CString remoteFileStr = StripPath(localFileStr,0);
	UploadArchive(_T("Output"),remoteFileStr,localFileStr);
}

void CTreeViewOutput::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	SelectUpdate();
	
	*pResult = 0;
}

void CTreeViewOutput::GrammarPopupStringList(CStringList &menuList)
{
	if (!m_itemSelected || !cg)
		return;

	CString conceptStr;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CONCEPT *concept = wnd->GrammarConceptSelected();

	// ADD SELECTED CONCEPT
	menuList.RemoveAll();
	conceptStr = ConceptName(concept);
	menuList.AddTail(conceptStr);

	// ADD ITS CHILDREN CONCEPTS
	CONCEPT *child = cg->Down(concept);
	ConceptHierarchyToStringList(menuList,child,1);
}

void CTreeViewOutput::OnOutputmenuAdditem() 
{
	if (!m_conceptSelected) {
		AfxMessageBox(_T("No record selected!"));
		return;
	}

	CString seedStr = _T("untitled");

	// ADD CONCEPT TO HIERARCHY
	_TCHAR buff[1001];
	StringToChar(buff,seedStr,1000);
	CONCEPT *newConcept = cg->makeConcept(m_conceptSelected,buff);

	COutputItemDlg dlg;
	dlg.SetData(newConcept);

	if (dlg.DoModal() == IDCANCEL) {
		cg->rmConcept(newConcept);
	}
	else if (m_itemSelected) {
		HTREEITEM lastConceptItem;
		if (!(lastConceptItem = FindLastConceptSibling(&m_treeCtrl,m_itemSelected)))
			lastConceptItem = TVI_FIRST;
		HTREEITEM newItem = m_treeCtrl.InsertItem(appAskText,2,3,m_itemSelected,lastConceptItem);
		
	}

	/*
	//CString name = ConceptName(concept);

	CTreeChooseDlg dlg;
	dlg.SetConcept(m_strConcept,concept);

	if (dlg.DoModal() == IDOK) {
		CStringList stateList;
		TokensBySeparator(stateList,appAskText,':');
		CString numStr;
		int i = 0;
		UINT index;
		while ((numStr = NthStringListItem(stateList,i++)) != "") {
			index = atoi((const char *)numStr);
			AddConceptItem(index);
		}
	}
	*/
}

void CTreeViewOutput::OnItemAdd(UINT idIn) 
{
	int adjust = 13391;  // THIS IS THE LOWORD OF SAMPLE_ID_START
	                    // Makes it 1-numbered for since UINT can't be -1

	AddConceptItem(idIn - adjust);
}

void CTreeViewOutput::AddConceptItem(UINT menuID)
{
	if (menuID <= 0)
		return;
	else {
		_TCHAR buff[1001];
		int count = 0;
		CONCEPT *concept = NumberToConcept(appPassConcept,menuID,count);

		CString conceptNameStr = ConceptName(concept);
		CONCEPT *newConcept = AddConcept(conceptNameStr,m_itemSelected,true);
		if (newConcept) {
			cg->conceptPath(concept,buff);
			cg->addVal(newConcept,_T("grammar"),buff);
			StringToChar(buff,BoolStr(true),1000);
			cg->addVal(newConcept,_T("required"),buff);
			cg->addVal(newConcept,_T("type"),_T("alpha"));
		}
	}
}

BOOL CTreeViewOutput::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= TVS_HASBUTTONS | TVS_EDITLABELS | TVS_LINESATROOT;	
	return CTreeView::PreCreateWindow(cs);
}

void CTreeViewOutput::OnOutputmenuDelete() 
{
	DeleteSelected();
}

void CTreeViewOutput::DeleteSelected() 
{
	CString selectedStr = m_treeCtrl.GetItemText(m_itemSelected);
	CString msgStr = _T("You sure you want to delete \"");
	msgStr += selectedStr + _T("\"");
	if (m_treeCtrl.GetChildItem(m_itemSelected)) msgStr += _T(" and its children");
	msgStr += _T("?");

	if (m_itemSelected && AfxMessageBox(msgStr, MB_YESNO) == IDYES) {
		cg->rmConcept(m_conceptSelected);
		m_treeCtrl.DeleteItem(m_itemSelected);
		kbDirty = true;
	}
}

void CTreeViewOutput::OnOutputmenuEditattributes() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->OpenAttributeEditor(m_conceptSelected, false);
}

void CTreeViewOutput::OnUpdateOutputmenuAdditem(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_treeCtrl.GetCount() && m_NodeType == OUTPUT_NODE_RECORD);	
}

void CTreeViewOutput::OnUpdateOutputmenuDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_treeCtrl.GetCount());		
}

void CTreeViewOutput::OnUpdateOutputmenuEditattributes(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_treeCtrl.GetCount());		
}

void CTreeViewOutput::OnUpdateOutputmenuArchiveoutput(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_treeCtrl.GetCount());	
}

void CTreeViewOutput::OnUpdateOutputmenuUploadoutput(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_treeCtrl.GetCount());		
}

void CTreeViewOutput::OnUpdateOutputmenuAddoutput(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_NodeType == OUTPUT_NODE_RECORD);	
}

void CTreeViewOutput::OnOutputmenuProperties() 
{
	if (m_NodeType == OUTPUT_NODE_RECORD) {
		CRecordProperties dlg;
		dlg.SetData(m_conceptSelected);
		dlg.DoModal();
	}
	else if (m_NodeType == OUTPUT_NODE_ITEM) {
		COutputItemDlg dlg;
		dlg.SetData(m_conceptSelected);
		dlg.DoModal();
	}
}

void CTreeViewOutput::OnUpdateOutputmenuProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_treeCtrl.GetCount());	
}

void CTreeViewOutput::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	CString newNameStr = pTVDispInfo->item.pszText;
	_TCHAR buff[1001];
	StringToChar(buff,newNameStr,1000);
	cg->renameConcept(m_conceptSelected,buff);
	kbDirty = true;

	*pResult = 0;
}

void CTreeViewOutput::ChangeRecordName(CString newConceptNameStr) 
{
	m_treeCtrl.SetItemText(m_itemSelected,newConceptNameStr);
}

void CTreeViewOutput::AddItemsFromStateStr(CString stateStr) 
{
	CStringList stateList;
	TokensBySeparator(stateList,stateStr,':');
	CString numStr;

	int i = 0;
	UINT index;
	while ((numStr = NthStringListItem(stateList,i++)) != _T("")) {
		index = _ttoi((const _TCHAR *)numStr);
		AddConceptItem(index);
	}
}

void CTreeViewOutput::OnRButtonDown(UINT nFlags, CPoint point) 
{
	logmsg(_T("CTreeViewOutput::OnRbuttonDown"));	// 05/14/08 AM.
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	if (!wnd->AnalyzerCreated())
		return;

	//ClientToScreen(&point);
	//PopupContextMenu(this,IDR_OUTPUT,point);
	
	//CTreeView::OnRButtonDown(nFlags, point);
}

void CTreeViewOutput::ConvertTextToOutput(CString outputPathStr)
{
	if (FileExists(outputPathStr)) {
		CStringList lines;
		ReadFileToStringList(lines,outputPathStr);
		POSITION pos = lines.GetHeadPosition();
		CString lineStr;
		CString fieldNameStr;
		CString valueStr;
		CString printNameStr;
		CString conceptNameStr;
		CString recordNameStr;
		CONCEPT *recordConcept = NULL;
		CONCEPT *fieldConcept = NULL;
		int colon = -1;
		_TCHAR buff[1001];
		bool lastLineBlank = false;

		while (pos && (lineStr = lines.GetNext(pos))) {
			TrimString(lineStr);
			if (lineStr == _T("")) {
				lastLineBlank = true;
				continue;
			}

			colon = NthPathSegment(lineStr,fieldNameStr,':',0);
			NthPathSegment(lineStr,valueStr,':',1);
			TrimString(fieldNameStr);
			TrimString(valueStr);
			printNameStr = fieldNameStr + _T(":");
			conceptNameStr = fieldNameStr;
			conceptNameStr.Remove(' ');
			conceptNameStr.Remove(':');
			StringToChar(buff,conceptNameStr,1000);

			if (lastLineBlank && AllUpper(fieldNameStr) && valueStr == _T("")) {
				recordConcept = cg->findConcept(m_conceptOutput,buff);
				if (!recordConcept) {
					m_conceptSelected = recordConcept;
					recordConcept = AddConcept(conceptNameStr);
					StringToChar(buff,printNameStr,1000);
					cg->addVal(recordConcept,_T("printname"),buff);
				}
				recordNameStr = conceptNameStr;
			}
			else if (recordConcept && fieldNameStr != _T("") && colon >= 0 &&
				     !cg->findConcept(recordConcept,buff)) {
				fieldConcept = AddField(recordConcept,conceptNameStr);
				cg->addVal(fieldConcept,_T("type"),_T("alpha"));
				StringToChar(buff,printNameStr,1000);
				cg->addVal(fieldConcept,_T("printname"),buff);
			}
			else {
				recordConcept = NULL;
			}

			lastLineBlank = false;
		}
	}
}

HTREEITEM CTreeViewOutput::TreeItemFromConcept(CONCEPT *concept, HTREEITEM item)
{
	HTREEITEM child;
	HTREEITEM returnItem;
	CString debugStr;
	CONCEPT *treeConcept;

	while (item) {
		treeConcept = (CONCEPT *)m_treeCtrl.GetItemData(item);
		debugStr = m_treeCtrl.GetItemText(item);

		if (treeConcept == concept)
			break;

		if ((child = m_treeCtrl.GetChildItem(item))) {
			if ((returnItem = TreeItemFromConcept(concept,child))) {
				item = returnItem;
				break;
			}
		}

		item = m_treeCtrl.GetNextSiblingItem(item);
	}
	
	return item;
}

void CTreeViewOutput::OutputPopup(CWnd *wnd, CPoint point)
{
	//CStringList menu;
	//PopupFromStringList(menu,wnd,point,menu,OUTPUT_ID_START);
}

void CTreeViewOutput::OutputPopupStringList(CStringList &menuList)
{
	if (!m_itemSelected || !cg)
		return;

	//menuList->AddTail("Sort to Unique");
	//menuList->AddTail("- - - - -");

	AddConceptHierarchyToStringList(menuList,m_conceptSelected);
}

void CTreeViewOutput::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class
	GetTreeCtrl().ModifyStyle(0, TVS_SHOWSELALWAYS); // SRP 010207
	
}
