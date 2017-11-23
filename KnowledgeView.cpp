/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// KnowledgeView.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"

#include "MainFrm.h"
#include "AttributeEditor.h"
#include "PhrasesDlg.h"
#include "KnowledgeDoc.h"
#include "KnowledgeView.h"

extern CG *cg;
extern NLP *nlp;
extern GUI *gui;
extern CONCEPT *appAttrConcept;
extern CVisualTextApp theApp;
extern bool kbDirty;
extern bool appBoolAnalyzerLoaded;
extern bool appBoolHighlightText;
extern bool appBoolVerbose;
extern bool appBoolSizeDynamically;
extern BOOL appBoolKBSafeEditMode;
extern VT_APP_STATE appState;
extern CString appAskText;
extern CString appStrFocusDocumentType;
extern CDocument *appDocFocus;	

#define KB_TREE_HOLDER_STR _T("holderxxx")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_TIMER_DRAG 300

/////////////////////////////////////////////////////////////////////////////
// CKnowledgeView

IMPLEMENT_DYNCREATE(CKnowledgeView, CTreeView)

CKnowledgeView::CKnowledgeView()
{
	InitVars();
}

CKnowledgeView::~CKnowledgeView()
{
}


BEGIN_MESSAGE_MAP(CKnowledgeView, CTreeView)
	//{{AFX_MSG_MAP(CKnowledgeView)
	ON_WM_CREATE()
	ON_COMMAND(ID_KNOWLEDGE_ATTRIBUTES, OnKnowledgeAttributes)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_KNOWLEDGE_DELETE, OnKnowledgeDelete)
	ON_COMMAND(ID_KNOWLEDGE_DELETECHILDREN, OnKnowledgeDeletechildren)
	ON_COMMAND(ID_KNOWLEDGE_ADDCONCEPTS, OnKnowledgeAddconcepts)
	ON_COMMAND(ID_KNOWLEDGE_FIND, OnKnowledgeFind)
	ON_COMMAND(ID_KNOWLEDGE_PHRASES, OnKnowledgePhrases)
	ON_COMMAND(ID_KNOWLEDGE_ADDPHRASE, OnKnowledgeAddphrase)
	ON_COMMAND(ID_KNOWLEDGE_MAKEROOT, OnKnowledgeMakeroot)
	ON_WM_DESTROY()
	ON_COMMAND(ID_KNOWLEDGE_GENERATEALL, OnKnowledgeGenerateall)
	ON_COMMAND(ID_KNOWLEDGE_SAVE, OnKnowledgeSave)
	ON_COMMAND(ID_SAVE_KB, OnSaveKb)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_COMMAND(ID_KB_DOWN, OnKbDown)
	ON_COMMAND(ID_KB_UP, OnKbUp)
	ON_UPDATE_COMMAND_UI(ID_KB_UP, OnUpdateKbUp)
	ON_UPDATE_COMMAND_UI(ID_KB_DOWN, OnUpdateKbDown)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_KNOWLEDGE_UP, OnKnowledgeUp)
	ON_COMMAND(ID_KNOWLEDGE_KBMODULAR_LOAD, OnKnowledgeKbmodularLoad)
	ON_COMMAND(ID_KNOWLEDGE_KBMODULAR_SAVE, OnKnowledgeKbmodularSave)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WMU_NEXT, OnSelectNextItem)
	ON_MESSAGE(WMU_PREV, OnSelectPrevItem)
	ON_MESSAGE(WMU_DOWN, OnSelectDownItem)
	ON_MESSAGE(WMU_UP, OnSelectUpItem)
	ON_MESSAGE(WMU_OPENPATH, OnOpenPath)
//	ON_WM_INITMENU()
END_MESSAGE_MAP()

void CKnowledgeView::InitVars()
{
	m_itemSelected = NULL;
	m_conceptRoot = NULL;
	m_conceptSelected = NULL;
	m_strConcept = _T("");
	m_strFind = _T("");
	m_bLDragging = false;
	m_intItemCount = 0;
	m_noDropCursor = AfxGetApp()->LoadStandardCursor(IDC_NO) ;
	m_dropCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW) ;
	m_boolOneRootNode = false;
	m_boolFillOneLevel = true;
	m_nTimerIDDrag = 0;
	m_boolUpdateAttributeEditor = true;
}

/////////////////////////////////////////////////////////////////////////////
// CKnowledgeView drawing

void CKnowledgeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CKnowledgeView diagnostics

#ifdef _DEBUG
void CKnowledgeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CKnowledgeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CKnowledgeView message handlers

int CKnowledgeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_ImageList.Create(IDB_KNOWLEDGE, 16, 1, RGB(0,255,0));
	GetTreeCtrl().SetImageList(&m_ImageList, TVSIL_NORMAL);

	return 0;
}

void CKnowledgeView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();

	CString titleStr;
	CDocument *doc = GetDocument();
	if (m_conceptRoot)
		titleStr.Format(_T("KB Editor: %s"),ConceptName(m_conceptRoot));
	else
		titleStr = _T("KB Editor");
	doc->SetTitle(titleStr);

	FillList();
	CTreeCtrl *tree = &GetTreeCtrl();
}

void CKnowledgeView::FillListExpand(HTREEITEM treeItem)
{
	CONCEPT *childConcept = cg->Down((CONCEPT *)GetTreeCtrl().GetItemData(treeItem));

	// IF NO CONCEPT BELOW, THEN ABORT - NOTHING TO EXPAND
	if (!childConcept)
		return;

	// DELETE PLACE HOLDER ITEM
	HTREEITEM childItem = GetTreeCtrl().GetChildItem(treeItem);
	CString childTextStr = GetTreeCtrl().GetItemText(childItem);

	if (!childItem || childTextStr == KB_TREE_HOLDER_STR) {
		if (childItem)
			GetTreeCtrl().DeleteItem(childItem);

		int count = 0;
		StartWaitCursor();
		SetRedraw(false);

		FillList(childConcept,treeItem,0,count);
		StopWaitCursor();
	}

	SetRedraw(true);
}

void CKnowledgeView::FillList(bool deleteFirst)
{
	HTREEITEM parentItem;
	int count = 0;
	StartWaitCursor();
	SetRedraw(false);

	if (deleteFirst) {
		SetRedraw(false);
		GetTreeCtrl().DeleteAllItems();
	}
	if (!m_conceptRoot)
		m_conceptRoot = cg->findRoot();
	parentItem = GetTreeCtrl().GetRootItem();

	FillList(m_conceptRoot,parentItem,0,count);

	parentItem = GetTreeCtrl().GetRootItem();
	GetTreeCtrl().Expand(parentItem,TVE_EXPAND);

	SetRedraw(true);
	StopWaitCursor();
	UpdateWindow();
}

bool CKnowledgeView::FillList(CONCEPT *parentConcept, HTREEITEM parentItem, int level, int &count)
{
	if (!parentConcept)
		return false;

	CONCEPT *concept = parentConcept;

	CString parentConceptStr = ConceptName(parentConcept);
	CString conceptStr;
	CString conceptPathStr;
	CString relPathStr;
	HTREEITEM item;
	bool somethingFound = false;
	bool conceptFound = false;
	bool childFound = false;
	bool phraseFound = false;
	int icon,iconSel;

	while (concept) {
		conceptFound = false;
		conceptStr = ConceptName(concept);

		if (m_strFind != _T("") && conceptStr.Find(m_strFind) >= 0)
			somethingFound = conceptFound = true;

		// BUILD SAMPLE IF ANY
		IconsFromConcept(concept,icon,iconSel);
		item = GetTreeCtrl().InsertItem(conceptStr,icon,iconSel,parentItem);
		m_intItemCount++;
		GetTreeCtrl().SetItemData(item,(DWORD)concept);

		if (m_boolFillOneLevel && parentConcept != m_conceptRoot) {
			if (cg->Down(concept)) {
				GetTreeCtrl().InsertItem(KB_TREE_HOLDER_STR,2,3,item);
			}
		}
		else {
			childFound = FillList(cg->Down(concept),item,level+1,count);
			if (m_strFind != _T("") && !childFound && !conceptFound)
				GetTreeCtrl().DeleteItem(item);
		}

		if (m_boolOneRootNode && parentConcept == m_conceptRoot)
			break;

		count++;
		concept = cg->Next(concept);
	}

	return somethingFound;
}

BOOL CKnowledgeView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;

	return CTreeView::PreCreateWindow(cs);
}

void CKnowledgeView::OnKnowledgeAttributes() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->OpenAttributeEditor(m_conceptSelected, true);
}

void CKnowledgeView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	m_itemSelected = GetTreeCtrl().GetSelectedItem();
	m_conceptSelected = (CONCEPT *)GetTreeCtrl().GetItemData(m_itemSelected);
	m_strConcept = GetTreeCtrl().GetItemText(m_itemSelected);

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CAttributeEditor *attrEditor = wnd->GetAttributeDlg();
	if (attrEditor && m_boolUpdateAttributeEditor) {
		attrEditor->LoadConcept(m_conceptSelected);
		attrEditor->SetActiveWindow();
	}
	
	*pResult = 0;
}

// START DRAG AND DROP STUFF

void CKnowledgeView::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // This code is to prevent accidental drags.
    if((GetTickCount() - m_dwDragStart) < DRAG_DELAY )
		return;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	m_hitemDrag = pNMTreeView->itemNew.hItem;

	// Don't allow for daring samples
	if (m_hitemDrag) {
		int image;
		int imageSel;
		GetTreeCtrl().GetItemImage(m_hitemDrag,image,imageSel);
		if (image == 2)
			return;
	}
	
	m_hitemDrop = NULL;
	GetTreeCtrl().SelectItem(m_hitemDrag);

	m_pDragImage = GetTreeCtrl().CreateDragImage(m_hitemDrag);

	// get the image list for dragging
	// CreateDragImage() returns NULL if no image list
	// associated with the tree view control
	if(!m_pDragImage) return;

	m_bLDragging = true;
	m_pDragImage->BeginDrag(0, CPoint(-15,-30));
	POINT pt = pNMTreeView->ptDrag;
	ClientToScreen(&pt);
	m_pDragImage->DragEnter(NULL, pt);
	SetCapture();

	m_nTimerIDDrag = SetTimer(ID_TIMER_DRAG,75,NULL);
}

void CKnowledgeView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bLDragging) {
		HTREEITEM	hItem;
		UINT		flags;
		POINT pt = point;

		ClientToScreen(&pt);
		pt.x -= DRAG_X_ADJUST;
		pt.y -= DRAG_Y_ADJUST;
		CImageList::DragMove(pt);

		if ((hItem = GetTreeCtrl().HitTest(point, &flags)) != NULL) {
			CImageList::DragShowNolock(false);
			if ((m_hitemDrop = GetDropTarget(hItem))) {
				GetTreeCtrl().SelectDropTarget(m_hitemDrop);
				if (!IsDropSource(m_hitemDrop))
					m_hitemDrop = NULL;
			}
			CImageList::DragShowNolock(true);
		}
		else
			m_hitemDrop = NULL;

		if (m_hitemDrop)
			SetCursor(m_dropCursor);
		else
			SetCursor(m_noDropCursor);
	}
	
	CTreeView::OnMouseMove(nFlags, point);
}

void CKnowledgeView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CTreeView::OnLButtonUp(nFlags, point);

	if (m_bLDragging) {
		m_bLDragging = false;
		if (m_nTimerIDDrag) {
			KillTimer(m_nTimerIDDrag);
			m_nTimerIDDrag = 0;
		}
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		ReleaseCapture();
		delete m_pDragImage;

		// Remove drop target highlighting
		GetTreeCtrl().SelectDropTarget(NULL);

		if (m_hitemDrag == m_hitemDrop || !IsDropSource(m_hitemDrop))
			return;
		// If Drag item is an ancestor of Drop item then return

		HTREEITEM htiParent = m_hitemDrop;
		while((htiParent = GetTreeCtrl().GetParentItem( htiParent )) != NULL ) {
			if( htiParent == m_hitemDrag )
				return;
		}

		// Move Concept to first position
		CONCEPT *drag = (CONCEPT *)GetTreeCtrl().GetItemData(m_hitemDrag);
		CONCEPT *drop = (CONCEPT *)GetTreeCtrl().GetItemData(m_hitemDrop);
		CString debugDragStr = ConceptName(drag);
		CString debugDropStr = ConceptName(drop);

		cg->rmConcept(drag);
		cg->addConcept(drop,drag,1);
		kbDirty = true;

		// Move interface tree stuff
		GetTreeCtrl().Expand(m_hitemDrop, TVE_EXPAND);
		HTREEITEM htiNew = CopyBranch(&GetTreeCtrl(), m_hitemDrag, m_hitemDrop, TVI_FIRST);
		GetTreeCtrl().DeleteItem(m_hitemDrag);
		m_intItemCount--;
		GetTreeCtrl().SelectItem(htiNew);
	}
}

void CKnowledgeView::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent) {
		case ID_TIMER_DRAG:
		if (m_bLDragging) {

			// Doesn't matter that we didn't initialize m_timerticks
			m_timerticks++;

			POINT pt;
			GetCursorPos(&pt);
			RECT rect;
			GetClientRect(&rect);
			ClientToScreen(&rect);

			// NOTE: Screen coordinate is being used because the call
			// to DragEnter had used the Desktop window.
			POINT ptImage = pt;
			ptImage.x -= DRAG_X_ADJUST;
			ptImage.y -= DRAG_Y_ADJUST;
			CImageList::DragMove(ptImage);

			HTREEITEM hitem = GetTreeCtrl().GetFirstVisibleItem();

			/*
			CString msgStr;
			m_strProcessFile.Format("Point - rect.top:  %d - %d,%d",pt.y,rect.top,rect.bottom);
			UpdateData(false);
			*/

			if (pt.y < rect.top + 10) {
				// We need to scroll up
				// Scroll slowly if cursor near the treeview control
				int slowscroll = 6 - (rect.top + 10 - pt.y) / 20;
				if (0 == ( m_timerticks % (slowscroll > 0? slowscroll : 1))) {
					CImageList::DragShowNolock(false);
					SendMessage( WM_VSCROLL, SB_LINEUP);
					//pTree->SelectDropTarget(hitem);
					m_hitemDrop = hitem;
					CImageList::DragShowNolock(true);
				}
			}
			else if (pt.y > rect.bottom - 10) {
				// We need to scroll down
				// Scroll slowly if cursor near the treeview control
				int slowscroll = 6 - (pt.y - rect.bottom + 10 ) / 20;
				if (0 == (m_timerticks % (slowscroll > 0? slowscroll : 1))) {
					CImageList::DragShowNolock(false);
					SendMessage( WM_VSCROLL, SB_LINEDOWN);
					int nCount = GetTreeCtrl().GetVisibleCount();
					for (int i=0; i<nCount-1; ++i)
						hitem = GetTreeCtrl().GetNextVisibleItem(hitem);
					//if (hitem)
						//pTree->SelectDropTarget(hitem);
					m_hitemDrop = hitem;
					CImageList::DragShowNolock(true);
				}
			}
		}
		break;
	}

	CTreeView::OnTimer(nIDEvent);
}

bool CKnowledgeView::IsDropSource(HTREEITEM hItem)
{
	int image;
	int imageSel;
	GetTreeCtrl().GetItemImage(hItem,image,imageSel);
	return image == 0 || image == 1;  // all items are valid sources
}

HTREEITEM CKnowledgeView::GetDropTarget(HTREEITEM hItem)
{
	// inhibit drop on the drop source or its parent
	if (hItem == m_hitemDrag || hItem == GetTreeCtrl().GetParentItem(m_hitemDrag))
		return NULL;
	return hItem;
}

void CKnowledgeView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_dwDragStart = GetTickCount();
	CTreeView::OnLButtonDown(nFlags, point);
}

BOOL CKnowledgeView::InKBSafeEditMode(void)
{
	if (appBoolKBSafeEditMode && m_conceptSelected) {
		CString conceptPathStr = ConceptPathStr(m_conceptSelected);
		CString strConcept = _T("\"concept\" ");
		int pos = conceptPathStr.Find(strConcept);
		if (0 == pos) {
			conceptPathStr = conceptPathStr.Mid(strConcept.GetLength());
			int posSys = conceptPathStr.Find(_T("\"sys\""));
			int posGram = conceptPathStr.Find(_T("\"gram\""));
			return 0 == posSys || 0 == posGram;
		}
	}
	return false;
}

void CKnowledgeView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	logmsg(_T("CKnowledgeView::OnRbuttonDown"));	// 05/14/08 AM.
	TreeViewSelectItem(this, point); // SRP 010504

	// TAKE CARE OF GRAYING ITEMS
	UINT disables[100];
	for (int i=0; i<100; i++)
		disables[i] = 0;
	int count = 0;

	m_itemSelected = GetTreeCtrl().GetSelectedItem();

	int image;
	int selImage;
	GetTreeCtrl().GetItemImage(m_itemSelected,image,selImage);

	disables[count++] = ID_KNOWLEDGE_FIND;
	if (m_conceptRoot == cg->findRoot()) {
		disables[count++] = ID_KNOWLEDGE_UP;
		disables[count++] = ID_KNOWLEDGE_GENERATEALL;
	}
	if (NULL == m_itemSelected) {
		disables[count++] = ID_KNOWLEDGE_KBMODULAR_SAVE;
		disables[count++] = ID_KNOWLEDGE_KBMODULAR_LOAD;
	}
	if (InKBSafeEditMode()) {
		disables[count++] = ID_KNOWLEDGE_DELETE;
		disables[count++] = ID_KNOWLEDGE_DELETECHILDREN;
		disables[count++] = ID_KNOWLEDGE_ADDCONCEPTS;
		disables[count++] = ID_KNOWLEDGE_ADDPHRASE;
	}

	// DO MENU	
	ClientToScreen(&point);
	PopupContextMenu(this,IDR_KNOWLEDGE,point,disables);	
}

void CKnowledgeView::DeleteSelected() 
{
	CString selectedStr = GetTreeCtrl().GetItemText(m_itemSelected);
	CString msgStr = _T("You sure you want to delete \"");
	msgStr += selectedStr + _T("\"");
	if (GetTreeCtrl().GetChildItem(m_itemSelected)) msgStr += _T(" and its children");
	msgStr += _T("?");

	if (m_itemSelected && AfxMessageBox(msgStr, MB_YESNO) == IDYES) {
		cg->rmConcept(m_conceptSelected);
		GetTreeCtrl().DeleteItem(m_itemSelected);
		m_intItemCount--;
		kbDirty = true;
	}
}

void CKnowledgeView::OnKnowledgeDelete() 
{
	DeleteSelected();	
}

void CKnowledgeView::OnKnowledgeDeletechildren() 
{
	if (!m_itemSelected || !GetTreeCtrl().GetChildItem(m_itemSelected))
		return;

	CString selectedStr = GetTreeCtrl().GetItemText(m_itemSelected);
	CString msgStr = _T("You sure you want to delete the children of \"");
	msgStr += selectedStr + _T("\"?");

	if (AfxMessageBox(msgStr, MB_YESNO) == IDYES) {
		cg->rmChildren(m_conceptSelected);
		Agram::makeDirty(m_conceptSelected,cg);
		DeleteTreeCtrlChildren(&GetTreeCtrl(),m_itemSelected);
		kbDirty = true;
	}	
}

void CKnowledgeView::OnKnowledgeAddconcepts() 
{
	if (GetTreeCtrl().GetCount() && !m_itemSelected)
		AfxMessageBox(_T("No concept chosen"));
	else if (AskForText(_T("Enter Concept Name:\n(more than one separated by commas)")))
		AddConcept(appAskText,m_itemSelected);	
}

void CKnowledgeView::OnKnowledgeFind() 
{
	CString msgStr = _T("Enter string to find:");

	if (AskForText(msgStr)) {
		m_strFind = appAskText;
		SetRedraw(false);
		GetTreeCtrl().DeleteAllItems();
		SetRedraw(true);
		FillList();
		m_strFind = _T("");
	}	
}

void CKnowledgeView::AddConcept(CString conceptStr, HTREEITEM itemSelected)
{
	CStringList conceptList;
	TokensBySeparator(conceptList,conceptStr,',');
	int count = conceptList.GetCount();
	CString conceptAdd;
	CONCEPT *newConcept;
	_TCHAR buff[1001];
	HTREEITEM newItem;
	HTREEITEM lastSiblingItem;
	bool expand = false;
	CONCEPT *selectedConcept = m_conceptSelected;

	SetRedraw(false);

	int i = 0;

	// SELECT THE FIRST CHILD IF EXISTS ITEM
	newItem = GetTreeCtrl().GetChildItem(m_itemSelected);
	if (newItem)
		GetTreeCtrl().SelectItem(newItem);

	while ((conceptAdd = NthStringListItem(conceptList,i++)) != _T("")) {

		if (itemSelected) {
			// Insert after last concept, not sample!
			lastSiblingItem = TreeCtrlLastSibling(&GetTreeCtrl(),newItem);
			newItem = GetTreeCtrl().InsertItem(conceptAdd,0,1,itemSelected,lastSiblingItem);
			expand = true;
		}
		else {
			newItem = GetTreeCtrl().InsertItem(conceptAdd,0,1);
		}

		// ADD CONCEPT TO KB
		StringToChar(buff,conceptAdd,1000);
		if (GetTreeCtrl().GetCount() == 1)	// Equals 1 because first added
			newConcept = cg->makeConcept(m_conceptRoot,buff);
		else
			newConcept = cg->makeConcept(selectedConcept,buff);

		// ADD CONCEPT AS USER DATA FOR INTERNAL USE
		GetTreeCtrl().SetItemData(newItem,(DWORD)newConcept);

		m_intItemCount++;
		kbDirty = true;
	}

	if (expand && itemSelected)
		GetTreeCtrl().Expand(itemSelected,TVE_EXPAND);

	if (newItem)
		GetTreeCtrl().SelectItem(newItem);

	SetRedraw(true);
}

void CKnowledgeView::OnKnowledgePhrases() 
{
	CPhrasesDlg dlg;
	dlg.m_bReadOnlyMode = InKBSafeEditMode();
	dlg.SetData(m_conceptSelected);
	dlg.DoModal();
}

void CKnowledgeView::OnKnowledgeAddphrase() 
{
	CString msgStr;
	msgStr.Format(_T("Node to add to \"%s\":"),ConceptName(m_conceptSelected));

	if (AskForText(msgStr)) {
		_TCHAR buff[1001];
		StringToChar(buff,appAskText,1000);
		Agram::addSample(m_conceptSelected,buff,cg);
		GetTreeCtrl().SetItemImage(m_itemSelected,2,3);
		kbDirty = true;
	}	
}

void CKnowledgeView::MoveSelUp()
{
	if (cg->moveCleft(m_conceptSelected)) {
		MoveTreeNodeUp(&GetTreeCtrl(),m_itemSelected,0,2);
		kbDirty = true;
	}
}

void CKnowledgeView::MoveSelDown()
{
	if (cg->moveCright(m_conceptSelected)) {
		MoveTreeNodeDown(&GetTreeCtrl(),m_itemSelected,0,2);
		kbDirty = true;
	}
}

bool CKnowledgeView::CanMoveUp()
{
	HTREEITEM firstSibling = TreeCtrlFirstSibling(&GetTreeCtrl(),m_itemSelected);
	return firstSibling != m_itemSelected;
}

bool CKnowledgeView::CanMoveDown()
{
	bool canMove = GetTreeCtrl().GetNextSiblingItem(m_itemSelected) ? true : false;
	HTREEITEM lastSibling = FindLastConceptSibling(&GetTreeCtrl(),m_itemSelected);
	return m_itemSelected != lastSibling;
}

void CKnowledgeView::OnKnowledgeMakeroot() 
{
	m_conceptRoot = m_conceptSelected;
	m_boolOneRootNode = true;
	FillList(true);
}

void CKnowledgeView::SetState(CString stateStr, STATE_TYPE state)
{
	SetRedraw(false);
	StateFromString(&GetTreeCtrl(),GetTreeCtrl().GetRootItem(),state,stateStr);
	SetRedraw(true);
}

void CKnowledgeView::SetData(CString conceptPathStr, bool oneRootNode) 
{
	m_conceptRoot = ConceptFromPath(conceptPathStr);
	m_boolOneRootNode = oneRootNode;
	FillList(true);
}

void CKnowledgeView::SaveState() 
{
	theApp.AddStatus(_T("kbopen"),StateStr());
}

CString CKnowledgeView::StateStr() 
{
	int scrollPos = 0;

 	CString conceptPathStr = ConceptPathStr(m_conceptRoot);
	CString selectedStr = StateString(&GetTreeCtrl(),GetTreeCtrl().GetRootItem(),STATE_GET_SELECTED);
	CString stateStr = StateString(&GetTreeCtrl(),GetTreeCtrl().GetRootItem(),STATE_GET_EXPANDED);

	WINDOWPLACEMENT wp;
	GetParentFrame()->GetWindowPlacement(&wp);

	CString posStr;
	posStr.Format(_T("%s%c%d%c%d%c%d%c%d%c%d%c%s%c%s%c%d"),
		conceptPathStr,STATUSSEP,
		m_boolOneRootNode,STATUSSEP,
		wp.rcNormalPosition.top,STATUSSEP,
		wp.rcNormalPosition.bottom,STATUSSEP,
		wp.rcNormalPosition.left,STATUSSEP,
		wp.rcNormalPosition.right,STATUSSEP,
		selectedStr,STATUSSEP,
		stateStr,STATUSSEP,
		scrollPos
	);

	return posStr;
}

void CKnowledgeView::OnDestroy() 
{
	if (m_nTimerIDDrag) {
		KillTimer(m_nTimerIDDrag);
		m_nTimerIDDrag = 0;
	}
	//SaveState();     called twice. 10/11/2011 Dd.
	//DeleteItemData();
	CTreeView::OnDestroy();	
	if (theApp.GetNumberOfOpenViews() == 1)
		appDocFocus = NULL; // SRP 010505
}

void CKnowledgeView::OnKnowledgeGenerateall() 
{
	m_conceptRoot = cg->findRoot();
	m_boolOneRootNode = false;
	FillList(true);
}

void CKnowledgeView::ForceSaveKB() 
{
	kbDirty = true;
	if (!AutoSaveKB())
		SaveKB();
}

void CKnowledgeView::OnKnowledgeSave() 
{
	ForceSaveKB();
}

void CKnowledgeView::OnSaveKb() 
{
	ForceSaveKB();
}

void CKnowledgeView::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM item = pNMTreeView->itemNew.hItem;

	if (cg->findRoot() != (CONCEPT *)GetTreeCtrl().GetItemData(item))
		FillListExpand(item);
	
	*pResult = 0;
}

void CKnowledgeView::OnKbDown() 
{
	MoveSelDown();	
}

void CKnowledgeView::OnKbUp() 
{
	MoveSelUp();	
}

void CKnowledgeView::OnUpdateKbUp(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && CanMoveUp());
}

void CKnowledgeView::OnUpdateKbDown(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && CanMoveDown());
}

void CKnowledgeView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnKnowledgeAttributes();
	*pResult = -1;
}

void CKnowledgeView::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if (appBoolSizeDynamically)
		SetWindowSize(this,&GetTreeCtrl(),true);

	m_strSelected = StateString(&GetTreeCtrl(),GetTreeCtrl().GetRootItem(),STATE_GET_EXPANDED);

	*pResult = 0;
}

void CKnowledgeView::Restore() 
{
	CString prevSelectedStr = m_strSelected;
	FillList(true);
	SetState(prevSelectedStr,STATE_EXPAND);
}

void CKnowledgeView::OnSetFocus(CWnd* pOldWnd) 
{
	CTreeView::OnSetFocus(pOldWnd);
	
	appStrFocusDocumentType = _T("Knowledge");
	appDocFocus = GetDocument();	
}

void CKnowledgeView::OnKnowledgeUp() 
{
	m_conceptRoot = cg->Up(m_conceptRoot);
	FillList(true);
}

long CKnowledgeView::OnSelectNextItem(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_itemSelected) return 0L;
	HTREEITEM hItem = GetTreeCtrl().GetNextItem(m_itemSelected, TVGN_NEXT);

	if (hItem) {
		CONCEPT *concept = (CONCEPT *)GetTreeCtrl().GetItemData(hItem);
		if (appAttrConcept != concept) {
			while (hItem && concept && concept != appAttrConcept) {
				hItem = GetTreeCtrl().GetNextItem(hItem, TVGN_NEXT);
				if (hItem)
					concept = (CONCEPT *)GetTreeCtrl().GetItemData(hItem);
			}
		}
	}

	if (hItem)
		GetTreeCtrl().SelectItem(hItem);
	return 0L;
}

long CKnowledgeView::OnSelectPrevItem(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_itemSelected) return 0L;
	HTREEITEM hItem = GetTreeCtrl().GetNextItem(m_itemSelected, TVGN_PREVIOUS);

	if (hItem) {
		CONCEPT *concept = (CONCEPT *)GetTreeCtrl().GetItemData(hItem);
		if (appAttrConcept != concept) {
			while (hItem && concept && concept != appAttrConcept) {
				hItem = GetTreeCtrl().GetNextItem(hItem, TVGN_PREVIOUS);
				if (hItem)
					concept = (CONCEPT *)GetTreeCtrl().GetItemData(hItem);
			}
		}
	}

	if (hItem)
		GetTreeCtrl().SelectItem(hItem);
	return 0L;
}

long CKnowledgeView::OnSelectDownItem(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_itemSelected) return 0L;
	HTREEITEM hItem = GetTreeCtrl().GetChildItem(m_itemSelected);

	if (hItem) {
		CONCEPT *concept = (CONCEPT *)GetTreeCtrl().GetItemData(hItem);
		m_itemSelected = hItem;
		GetTreeCtrl().SelectItem(hItem);
	}
	return 0L;
}

long CKnowledgeView::OnSelectUpItem(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_itemSelected) return 0L;
	HTREEITEM hItem = GetTreeCtrl().GetParentItem(m_itemSelected);

	if (hItem) {
		CONCEPT *concept = (CONCEPT *)GetTreeCtrl().GetItemData(hItem);
		GetTreeCtrl().SelectItem(hItem);
	}
	return 0L;
}

long CKnowledgeView::OnOpenPath(WPARAM wParam, LPARAM lParam)
{
	CString pathStr = (LPTSTR)wParam;
	CStringList paths;
	StringToWordList(paths,pathStr);
	POSITION pos = paths.GetHeadPosition();
	CString word = _T("");
	CStringArray strNodes;
	while (pos && (word = paths.GetNext(pos))) {
		word.TrimLeft('\"');
		word.TrimRight('\"');
		strNodes.Add(word);
	}
	paths.RemoveAll();

	int nSize = strNodes.GetSize();
	if (nSize > 0) {
		if (m_conceptRoot != cg->findRoot())
			SendMessage(WM_COMMAND, ID_KNOWLEDGE_GENERATEALL);
		int n = 0;
		HTREEITEM hNode = GetTreeCtrl().GetRootItem();
		while (hNode != NULL && n < nSize) {
			CString strConcept = GetTreeCtrl().GetItemText(hNode);
			if (strConcept.Compare(strNodes[n])) {
				hNode = GetTreeCtrl().GetNextSiblingItem(hNode);
				continue;
			}
			GetTreeCtrl().SelectItem(hNode);
			if (++n < nSize) {
				UINT state = GetTreeCtrl().GetItemState(hNode, TVIS_EXPANDED);
				if (!(state & TVIS_EXPANDED))
					GetTreeCtrl().Expand(hNode, TVE_EXPAND);
				hNode = GetTreeCtrl().GetChildItem(hNode);
			}
		}
	}
	strNodes.RemoveAll();
	return 0L;
}

bool CKnowledgeView::SelectFromConcept(CONCEPT *concept, bool updateAttributeEditorFlag)
{
	bool returnVal = false;

	HTREEITEM item = TreeItemFromConcept(concept);
	bool oldFlag = m_boolUpdateAttributeEditor;
	m_boolUpdateAttributeEditor = updateAttributeEditorFlag;

	if (item) {
		GetTreeCtrl().SelectItem(item);
		GetTreeCtrl().EnsureVisible(item);
		returnVal = true;
	}

	SetRedraw(true);
	StopWaitCursor();
	m_boolUpdateAttributeEditor = oldFlag;

	return returnVal;
}

HTREEITEM CKnowledgeView::TreeItemFromConcept(CONCEPT *concept)
{
	CONCEPT *concepts[100];
	for (int i=0; i<100; i++)
		concepts[i] = NULL;

	CONCEPT *con = concept;
	CONCEPT *root = cg->findRoot();
	if (con == root)
		return GetTreeCtrl().GetRootItem();

	int level = 0;
	while (con &&  // FIX. // Because KBNODES don't have parents. // 10/08/05 AM.
			con != root) {
		concepts[level++] = con;
		con = cg->Up(con);
	}

	HTREEITEM startItem = GetTreeCtrl().GetChildItem(GetTreeCtrl().GetRootItem());
	HTREEITEM item = TreeItemFromConcept(concepts,startItem,level-1);

	return item;
}
	
HTREEITEM CKnowledgeView::TreeItemFromConcept(CONCEPT *concepts[], HTREEITEM item, int level)
{
	HTREEITEM child;
	HTREEITEM returnItem;
	CString itemStr,childItemStr;
	CONCEPT *treeConcept;

	while (item) {
		treeConcept = (CONCEPT *)GetTreeCtrl().GetItemData(item);
		if (treeConcept == concepts[0])
			break;
		itemStr = GetTreeCtrl().GetItemText(item);

		if (treeConcept == concepts[level]) {
			child = GetTreeCtrl().GetChildItem(item);
			childItemStr = GetTreeCtrl().GetItemText(child);

			if (childItemStr == KB_TREE_HOLDER_STR) {
				FillListExpand(item);
				child = GetTreeCtrl().GetChildItem(item);
			}

			if ((returnItem = TreeItemFromConcept(concepts,child,level-1))) {
				item = returnItem;
				break;
			}
		}

		item = GetTreeCtrl().GetNextSiblingItem(item);
	}
	
	return item;
}

void CKnowledgeView::ReIcon(CONCEPT *concept)
{
	HTREEITEM item = TreeItemFromConcept(concept);
	if (item) {
		int icon,iconSel;
		IconsFromConcept(concept,icon,iconSel);
		GetTreeCtrl().SetItemImage(item,icon,iconSel);
	}
}

void CKnowledgeView::IconsFromConcept(CONCEPT *concept, int &icon, int &iconSel)
{
	if (cg->findPhrase(concept)) {
		icon = 2;
		iconSel = 3;
	}
	else if (!cg->findAttrs(concept)) {
		icon = 4;
		iconSel = 5;
	}
	else {
		icon = 0;
		iconSel = 1;
	}
}

void CKnowledgeView::OnKnowledgeKbmodularLoad()
{
	if (m_itemSelected && m_conceptSelected) {
		CString fileDir = AppPath();
		fileDir += _T("\\*.kb");
		CFileDialog fileDlg(true,_T("kb"),fileDir,OFN_HIDEREADONLY,_T("KB Modular Files (*.kb)|*.kb|All files (*.*)|*.*||"));
		if (fileDlg.DoModal() == IDOK) {
			CONCEPT* conceptRemove = m_conceptSelected;
			CString strConcept = ConceptPathStr(m_conceptSelected);
			CString strMsg1, strMsg2;
			BOOL bFileOK = false;
			FILE *fp;

			errno_t err;
			if (!(err = _tfopen_s(&fp,fileDlg.GetPathName(), _T("r") )) != 0 ) {
				_TCHAR line[1000 + 1];
				if (_fgetts(line,1000,fp)) {
					fclose(fp);
					CString strLine = line;
					strLine.TrimRight();
					strLine.TrimLeft();
					int pos = strLine.Find(_T("add hier \""));
					if (0 != pos) {
						strMsg1.Format(_T("%s\nis not a KB modular file!"), fileDlg.GetPathName());
						AfxMessageBox(strMsg1);
						return;
					}
					if (-1 != (pos = strLine.Find('\"')) )
						strLine = strLine.Mid(pos);
				 	CString conceptPathStr = ConceptPathStr(m_conceptSelected);
					if (strLine.CompareNoCase(conceptPathStr)) {
						conceptRemove = ConceptFromPath(strLine);
						strConcept = strLine;
					}
				}
			}
			else {
				strMsg1.Format(_T("Could not open the file:\n%s"), fileDlg.GetPathName());
				AfxMessageBox(strMsg1);
				return;
			}
			if (conceptRemove) {
				strMsg1.Format(_T("Do you want to delete the KB concept: %s\n")
					_T("and reconstruct it from the file: %s"),
					strConcept, fileDlg.GetFileName());
				if (IDNO == AfxMessageBox(strMsg1, MB_YESNO))
					return;
				cg->rmConcept(conceptRemove);
				kbDirty = true;
			}
			if (cg->readFile((LPTSTR)(LPCTSTR)fileDlg.GetPathName()) ) {
				strMsg1.Format(_T("Loaded concept: %s"), strConcept);
				strMsg2.Format(_T("From File: %s"), fileDlg.GetPathName());
				kbDirty = true;
			}
			else {
				strMsg1.Format(_T("FAILED to load concept: %s"), strConcept);
				strMsg2.Format(_T("From File: %s"), fileDlg.GetPathName());
			}
			CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
			wnd->AddDebugLine(strMsg1, strMsg2);
			Restore();
		}
	}
}

void CKnowledgeView::OnKnowledgeKbmodularSave() 
{
	if (m_itemSelected && m_conceptSelected && !m_strConcept.IsEmpty()) {
		CString fileDir = AppPath();
		fileDir += _T("\\*.kb");
		CFileDialog fileDlg(false,_T("kb"),fileDir,OFN_HIDEREADONLY,_T("KB Modular Files (*.kb)|*.kb|All files (*.*)|*.*||"));
		if (fileDlg.DoModal() == IDOK) {
			CString strMsg1, strMsg2;
			if (cg->dumpTree(m_conceptSelected, (LPTSTR)(LPCTSTR)fileDlg.GetPathName()) ) {
				strMsg1.Format(_T("Saved concept: %s"), m_strConcept);
				strMsg2.Format(_T("To File: %s"), fileDlg.GetPathName());
			}
			else {
				strMsg1.Format(_T("FAILED to save concept: %s"), m_strConcept);
				strMsg2.Format(_T("To File: %s"), fileDlg.GetPathName());
			}
			CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
			wnd->AddDebugLine(strMsg1, strMsg2);
		}
	}
}
