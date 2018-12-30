/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// TreeViewText.cpp : implementation file
//

#include "stdafx.h"
#include <io.h>

#include "resource.h"
#include "kb.h"
#include "Utils.h"
#include "HexEditDlg.h"
#include "ASCIIDlg.h"
#include "FTPclient.h"
#include "MainFrm.h"
#include "TreeViewAnalyzer.h"
#include "TreeViewText.h"
#include "VisualTextDoc.h"
#include "VisualTextView.h"
#include "TextInfoDlg.h"
#include "EmailDlg.h"
#include "AttributeEditor.h"
#include "FileCharViewerDlg.h"
#include "PrefLookupPage.h"
#include "KnowledgeView.h"

extern CString appAskText;
extern CString appNameStr;
extern CString appURL;
extern CString appLastURLSelected;
extern CVisualTextApp theApp;
extern CG *cg;
extern bool appBoolAnalyzerLoaded;
extern bool appBoolLogAnalyzer;
extern bool appBoolPageMode;
extern bool appBoolOpeningTextFile;
extern VT_APP_STATE appState;
extern int appButtonReturn;
extern CONCEPT *appGUIConcept;
//extern bool kbDirty;

static bool textViewSelecting = false;

#define DRAG_X_ADJUST 15
#define DRAG_Y_ADJUST 40
#define DRAG_DELAY 80

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_TIMER_DRAG 400

/*
See ImagesFromType() below.

image imageSel  descr
0     1         Unprocessed text
2     3         Processed text
4     5         Log
6     7         Folder
8     9         Unprocessed html
10    11        Processed html
*/

/////////////////////////////////////////////////////////////////////////////
// CTreeViewText

IMPLEMENT_DYNCREATE(CTreeViewText, CPaneView)

CTreeViewText::CTreeViewText()
{
	InitVars();
	m_nTimerIDDrag = 0;
}

CTreeViewText::~CTreeViewText()
{
	m_strNames.RemoveAll();
	m_strURLs.RemoveAll();
	if (m_nTimerIDDrag) {
		KillTimer(m_nTimerIDDrag);
		m_nTimerIDDrag = 0;
	}
}

BOOL CTreeViewText::Create(CWnd* pParentWnd)
{
	/*  ddxxx
	if (::IsWindow(m_hWnd))
	{
		SetParent(pParentWnd);
	}
	else if (!CPaneView::Create( WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
		CRect(0,0,0,0), pParentWnd, 0xFF ))
	{
		TRACE0( "Unable to create tree control.\n" );
		return false;
	}
	*/

	return true;
}
void CTreeViewText::InitVars()
{
	m_itemSelected = NULL;
	m_itemLastSelected = NULL;
	m_itemToAddTo = NULL;
	m_lastFolderSelected = NULL;
	m_treeItemSlog = NULL;
	m_boolBlockComboAdd = false;
	m_boolSelectedIsLogFile = false;
	m_textTreeType = TEXT_TREE_UNKNOWN;
	m_bOpeningTextFile = false;

	m_conceptText = NULL;
	m_conceptSelected = NULL;
	m_conceptFolder = NULL;

	m_bLDragging = false;
	m_boolLeftButton = false;
	m_boolProcessedText = false;
	m_boolFillOneLevel = true;
	m_boolHasFolder = false;

	if (appState != VT_APP_LOADING)
		m_stringListStates.RemoveAll();

	m_noDropCursor = AfxGetApp()->LoadStandardCursor(IDC_NO);
	m_dropCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
}

BEGIN_MESSAGE_MAP(CTreeViewText, CPaneView)
	//{{AFX_MSG_MAP(CTreeViewText)
	ON_WM_CREATE()
	ON_COMMAND(ID_DIRTREEMENU_ADDEXISTINGTEXT, OnDirtreemenuAddexistingtext)
	ON_COMMAND(ID_DIRTREEMENU_NEWTEXT, OnDirtreemenuNewtext)
	ON_COMMAND(ID_DIRTREEMENU_ADDFOLDER, OnDirtreemenuAddfolder)
	ON_COMMAND(ID_DIRTREEMENU_MOVETOTOPLEVEL, OnDirtreemenuMovetotoplevel)
	ON_COMMAND(ID_DIRTREEMENU_DELETE, OnDirtreemenuDelete)
	ON_COMMAND(ID_DIRTREEMENU_DELETECHILDREN, OnDirtreemenuDeletechildren)
	ON_COMMAND(ID_DIRTREEMENU_EDITVIEW, OnDirtreemenuEditview)
	ON_COMMAND(ID_DIRTREEMENU_RENAME, OnDirtreemenuRename)
	ON_COMMAND(ID_DIRTREEMENU_GOTOWEBPAGE, OnDirtreemenuGotowebpage)
	ON_COMMAND(ID_DIRTREEMENU_UPDATEWEBPAGE, OnDirtreemenuUpdatewebpage)
	ON_COMMAND(ID_DIRTREEMENU_VIEWASWEBPAGE, OnDirtreemenuViewaswebpage)
	ON_COMMAND(ID_DIRTREEMENU_VIEWOUTPUTFILE, OnDirtreemenuViewoutputfile)
	ON_COMMAND(ID_DIRTREEMENU_RUNANALYZER, OnDirtreemenuRunanalyzer)
	ON_COMMAND(ID_DIRTREEMENU_HELP, OnDirtreemenuHelp)
	ON_UPDATE_COMMAND_UI(ID_DIRTREEMENU_ADDEXISTINGTEXT, OnUpdateDirtreemenuAddexistingtext)
	ON_UPDATE_COMMAND_UI(ID_DIRTREEMENU_VIEWASWEBPAGE, OnUpdateDirtreemenuViewaswebpage)
	ON_COMMAND(ID_DIRTREEMENU_SERVERARCHIVES, OnDirtreemenuServerarchives)
	ON_COMMAND(ID_DIRTREEMENU_UPLOADTEXT, OnDirtreemenuUploadtext)
	ON_COMMAND(ID_DIRTREEMENU_LOCALARCHIVE, OnDirtreemenuLocalarchive)
	ON_UPDATE_COMMAND_UI(ID_DIRTREEMENU_MOVETOTOPLEVEL, OnUpdateDirtreemenuMovetotoplevel)
	ON_UPDATE_COMMAND_UI(ID_DIRTREEMENU_DELETECHILDREN, OnUpdateDirtreemenuDeletechildren)
	ON_UPDATE_COMMAND_UI(ID_DIRTREEMENU_EDITVIEW, OnUpdateDirtreemenuEditview)
	ON_UPDATE_COMMAND_UI(ID_DIRTREEMENU_RENAME, OnUpdateDirtreemenuRename)
	ON_UPDATE_COMMAND_UI(ID_DIRTREEMENU_UPDATEWEBPAGE, OnUpdateDirtreemenuUpdatewebpage)
	ON_UPDATE_COMMAND_UI(ID_DIRTREEMENU_ARCHIVETEXT, OnUpdateDirtreemenuArchivetext)
	ON_UPDATE_COMMAND_UI(ID_DIRTREEMENU_UPLOADTEXT, OnUpdateDirtreemenuUploadtext)
	ON_UPDATE_COMMAND_UI(ID_DIRTREEMENU_RUNANALYZER, OnUpdateDirtreemenuRunanalyzer)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	//ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_DIRTREEMENU_DELETELOGS, OnDirtreemenuDeletelogs)
	ON_COMMAND(ID_DIRTREEMENU_REFRESH, OnDirtreemenuRefresh)
	ON_COMMAND(ID_DIRTREEMENU_MASK, OnDirtreemenuMask)
	ON_COMMAND(ID_DIRTREEMENU_DICTIONIZE, OnDirtreemenuDictionize)
	ON_COMMAND(ID_DIRTREEMENU_TOOLS_WORDNET, OnDirtreemenuToolsWordnet)
	ON_COMMAND(ID_DIRTREEMENU_TEXTFILEINFO, OnDirtreemenuTextfileinfo)
	ON_COMMAND(ID_DIRTREEMENU_TOOLS_HEXVIEWER, OnDirtreemenuToolsHexviewer)
	ON_COMMAND(ID_DIRTREEMENU_TOOLS_ASCIITABLE, OnDirtreemenuToolsAsciitable)
	ON_COMMAND(ID_DIRTREEMENU_TOOLS_CHARACTERVIEWER, OnDirtreemenuToolsCharacterviewer)
	ON_COMMAND(ID_DIRTREEMENU_TOOLS_EMAIL, OnDirtreemenuToolsEmail)
	ON_COMMAND(ID_DIRTREEMENU_ADD_TOPLEVEL_EXISTINGTEXTFILE, OnDirtreemenuAddToplevelExistingtextfile)
	ON_COMMAND(ID_DIRTREEMENU_ADD_TOPLEVEL_NEWTEXTFILE, OnDirtreemenuAddToplevelNewtextfile)
	ON_COMMAND(ID_DIRTREEMENU_ADD_TOPLEVEL_FOLDER, OnDirtreemenuAddToplevelFolder)
	ON_COMMAND(ID_DIRTREEMENU_ARCHIVETEXT, OnDirtreemenuArchivetext)
	ON_UPDATE_COMMAND_UI(ID_DIRTREEMENU_DELETE, OnUpdateDirtreemenuDelete)
	ON_COMMAND(ID_DIRTREEMENU_ATTRIBUTES, OnDirtreemenuAttributes)
	ON_UPDATE_COMMAND_UI(ID_DIRTREEMENU_ATTRIBUTES, OnUpdateDirtreemenuAttributes)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_DIRTREEMENU_TOOLS_JUMBLE, OnDirtreemenuToolsJumble)
	ON_COMMAND(ID_DIRTREEMENU_DICTIONARY_ADDWORDS, OnDirtreemenuDictionaryAddwords)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_DICTIONARY_URL_LOOKUP_BASE, ID_DICTIONARY_URL_LOOKUP_MAX, OnDictionaryURLLookup)
	ON_NOTIFY(NM_RCLICK, AFX_IDW_PANE_FIRST, OnRButtonDown)
	ON_NOTIFY(TVN_SELCHANGED, AFX_IDW_PANE_FIRST, OnSelchanged)
	ON_NOTIFY(NM_DBLCLK, AFX_IDW_PANE_FIRST, OnDblclk)
	ON_NOTIFY(TVN_ITEMEXPANDING, AFX_IDW_PANE_FIRST, OnItemexpanding)
	ON_NOTIFY(TVN_BEGINDRAG, AFX_IDW_PANE_FIRST, OnBegindrag)
	ON_COMMAND(ID_ADD_EXISTINGFOLDER, &CTreeViewText::OnAddExistingfolder)
	ON_COMMAND(ID_DIRTREEMENU_EXPLOREDIRECTORY, &CTreeViewText::OnDirtreemenuExploredirectory)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeViewText drawing


/////////////////////////////////////////////////////////////////////////////
// CTreeViewText diagnostics

#ifdef _DEBUG
void CTreeViewText::AssertValid() const
{
	CPaneView::AssertValid();
}

void CTreeViewText::Dump(CDumpContext& dc) const
{
	CPaneView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTreeViewText message handlers


#define FILE_ICON_COUNT 11

void CTreeViewText::FillInputTree()
{
	StartWaitCursor();
	bool save = cg->getDirty();	// 07/11/03 AM.

	m_bFillingTree = true;
	CString treePathStr = CurrentTreeSelection(false);

	StopWaitCursor();

	if (m_treeCtrl.GetCount()) {
		m_treeCtrl.DeleteAllItems();
	}

	SetRedraw(false);

	if (!m_boolKBBased || !cg->Down(m_conceptText))
		FillKBRecurse(AppPath(_T("input")));	
	FillInputTreeRecurse(cg->Down(m_conceptText));

	//FillInputTreeRecurse(AppPath("input"), NULL);	
	//m_treeCtrl.SortChildren(NULL);
	//ExpandFromPath(this,treePathStr);
	m_bFillingTree = false;

	cg->setDirty(save);	// Kb status unaffected.	// 07/11/03 AM.
	StopWaitCursor();
}

void CTreeViewText::FillKBRecurse(CString pathStr, CONCEPT *conceptParent)
{
	// GET FILES AND PUT INTO STRING LIST
	CString dirPath = pathStr + _T("\\*.*");
	CFileFind finder;
	int bWorking = finder.FindFile(dirPath);
	if (!bWorking) return;

	CString fileNameStr;
	CStringList strings;

	while (bWorking) {
		bWorking = finder.FindNextFile();
		fileNameStr = finder.GetFileName();
		if (fileNameStr == _T(".") || fileNameStr == _T("..") ||
			EndsWith(fileNameStr,_T("tmp")) || EndsWith(fileNameStr,_T("zip")))
			continue;
		if (finder.IsDirectory()) fileNameStr += _T("_dir_");
		strings.AddTail(fileNameStr);
	}
	
	POSITION pos = strings.GetHeadPosition();
	CString fileNameLog;
	POSITION foundLogPos;
	CString childPathStr;
	bool dirFlag = false;
	bool processedFlag;
	_TCHAR textBuff[1001];
	CONCEPT *conceptText = NULL;
	if (!conceptParent)
		conceptParent = m_conceptText;

	int image, imageSel;
	TEXT_TREE_TYPE type;

	while (pos && (fileNameStr = strings.GetNext(pos))) {

		dirFlag = false;
		if (EndsWith(fileNameStr,_T("_dir_"))) {
			dirFlag = true;
			ClipRight(fileNameStr,5);
		}
		fileNameLog = fileNameStr + _T("_log_dir_");
		foundLogPos = strings.Find(fileNameLog);

		processedFlag = foundLogPos ? true : false;

		// IF IT IS A LOG FILE, THEN SKIP DISPLAYING
		if (EndsWith(fileNameStr,_T("_log")))
			continue;

		ImageTypeFromFile(pathStr + _T("\\") + fileNameStr,image,imageSel,type);

		// ADD CONCEPT TO KB
		StringToChar(textBuff,fileNameStr,1000);
		conceptText = cg->makeConcept(conceptParent,textBuff,0);
		KBReplaceValue(conceptText,_T("type"),type,false);

		if (processedFlag) {
			childPathStr = pathStr + _T("\\") + fileNameStr + _T("_log");
			FillKBRecurse(childPathStr, conceptText);
		}
		else if (dirFlag) {
			m_boolHasFolder = true;
			childPathStr = pathStr + _T("\\") + fileNameStr;
			FillKBRecurse(childPathStr, conceptText);
		}
	}
	finder.Close();
}

void CTreeViewText::FillTreeExpand(HTREEITEM treeItem)
{
	// DELETE PLACE HOLDER ITEM
	HTREEITEM childItem = m_treeCtrl.GetChildItem(treeItem);
	if (childItem)
		m_treeCtrl.DeleteItem(childItem);

	SetRedraw(false);
	m_bFillingTree = true;

	CString pathStr = InputPath(TreePath(treeItem));
	if (IsChecked(treeItem))
		pathStr += _T("_log"); 

	FillInputTreeRecurse(pathStr,treeItem);
	m_treeCtrl.SortChildren(NULL);
	//ExpandFromPath(this,treePathStr);
	m_bFillingTree = false;

	SetRedraw(true);
}

void CTreeViewText::FillInputTreeRecurse(CONCEPT *concept, HTREEITEM item)
{
	if (!concept)
		return;

	long longType;
	TEXT_TREE_TYPE type;
	HTREEITEM newItem;
	int image = 0;
	int imageSel = 0;
	bool hasChild;
	if (!item)
		m_treeCtrl.GetRootItem();
	CString conceptStr;

	while (concept) {
		KBFindValue(concept,_T("type"),longType);
		conceptStr = ConceptName(concept);
		type = (TEXT_TREE_TYPE)longType;
		if (type == TEXT_TREE_FOLDER)
			m_boolHasFolder = true;
		hasChild = cg->Down(concept) ? true : false;
		ImagesFromType(type,hasChild,image,imageSel);
		newItem = m_treeCtrl.InsertItem(ConceptName(concept),image,imageSel,item);
		m_treeCtrl.SetItemData(newItem,(DWORD)concept);
		if (hasChild)
			FillInputTreeRecurse(cg->Down(concept),newItem);
		concept = cg->Next(concept);
	}
}

void CTreeViewText::FillInputTreeRecurse(CString pathStr, HTREEITEM hParent)
{
	CString dirPath = pathStr + _T("\\*.*");
	CFileFind finder;
	int bWorking = finder.FindFile(dirPath);
	if (!bWorking) return;

	CString fileNameStr;
	CStringList strings;

	while (bWorking) {
		bWorking = finder.FindNextFile();
		fileNameStr = finder.GetFileName();
		if (fileNameStr == _T(".") || fileNameStr == _T("..") ||
			EndsWith(fileNameStr,_T("tmp")) || EndsWith(fileNameStr,_T("zip")))
			continue;
		if (finder.IsDirectory()) fileNameStr += _T("_dir_");
		strings.AddTail(fileNameStr);
	}
	
	POSITION pos = strings.GetHeadPosition();
	CString fileNameLog;
	POSITION foundLogPos;
	CString childPathStr;
	bool dirFlag = false;
	bool processedFlag;
	HTREEITEM hChild;
	HTREEITEM first = NULL;
	_TCHAR textBuff[1001];
	CONCEPT *conceptText = NULL;
	CONCEPT *conceptParent = NULL;
	if (hParent)
		conceptParent = (CONCEPT *)m_treeCtrl.GetItemData(hParent);
	else
		conceptParent = m_conceptText;

	int image, imageSel;
	TEXT_TREE_TYPE type;

	while (pos && (fileNameStr = strings.GetNext(pos))) {

		dirFlag = false;
		if (EndsWith(fileNameStr,_T("_dir_"))) {
			dirFlag = true;
			ClipRight(fileNameStr,5);
		}
		fileNameLog = fileNameStr + _T("_log_dir_");
		foundLogPos = strings.Find(fileNameLog);

		processedFlag = foundLogPos ? true : false;

		// IF IT IS A LOG FILE, THEN SKIP DISPLAYING
		if (EndsWith(fileNameStr,_T("_log")))
			continue;

		ImageTypeFromFile(pathStr + _T("\\") + fileNameStr,image,imageSel,type);

		// ADD CONCEPT TO KB
		StringToChar(textBuff,fileNameStr,1000);
		conceptText = cg->makeConcept(conceptParent,textBuff,0);

		// INSERT NEW TREE ITEM
		hChild = m_treeCtrl.InsertItem(fileNameStr,image,imageSel,hParent);
		m_treeCtrl.SetItemData(hChild,(DWORD)conceptText);
		KBReplaceValue(conceptParent,_T("building"),1,false);  // FOR BLOCKING EXPANSION
		KBReplaceValue(conceptText,_T("type"),TextTreeType(hChild),false);

		if (m_treeCtrl.GetCount() == 1)
			first = hChild;

		if (m_boolFillOneLevel && !first) {
			if (processedFlag || dirFlag) {
				m_treeCtrl.InsertItem(_T("holder"),2,3,hChild);
			}
		}
		else if (processedFlag) {
			childPathStr = pathStr + _T("\\") + fileNameStr + _T("_log");
			FillInputTreeRecurse(childPathStr, hChild);
			m_treeCtrl.SortChildren(hChild);
			m_treeCtrl.Expand(hChild,TVE_COLLAPSE);
		}
		else if (dirFlag) {
			m_boolHasFolder = true;
			childPathStr = pathStr + _T("\\") + fileNameStr;
			FillInputTreeRecurse(childPathStr, hChild);
			m_treeCtrl.SortChildren(hChild);
		}
	}
	finder.Close();
}

void CTreeViewText::ImagesFromType(TEXT_TREE_TYPE type, bool processed, int &image, int &imageSel)
{
	switch (type) {

	case TEXT_TREE_FOLDER:
		image = 6;
		imageSel = 7;
		break;

	case TEXT_TREE_TEXT:
		if (processed) {
			image = 2;
			imageSel = 3;
		}
		else {
			image = 0;
			imageSel = 1;
		}
		break;

	case TEXT_TREE_HTML:
		if (processed) {
			image = 10;
			imageSel = 11;
		}
		else {
			image = 8;
			imageSel = 9;
		}
		break;

	case TEXT_TREE_LOG:
		image = 4;
		imageSel = 5;
		break;

	default:
		image = 0;
		imageSel = 1;
		break;
	}
}

void CTreeViewText::ImageTypeFromFile(CString fullPathStr, int &image, int &imageSel, TEXT_TREE_TYPE &type)
{
	CString logPathStr = fullPathStr + _T("_log");

	if (IsDirectory(logPathStr)) {
		// PROCESSED URL
		if (EndsWith(fullPathStr,_T("htm")) ||
			 EndsWith(fullPathStr,_T("html"))) {
			image = 10;
			imageSel = 11;
			type = TEXT_TREE_HTML;
		}
		else {
			image = 2;
			imageSel = 3;
			type = TEXT_TREE_TEXT;
		}
	}
	else if (IsDirectory(fullPathStr)) {
		image = 6;
		imageSel = 7;
		type = TEXT_TREE_FOLDER;
	}
	else if (EndsWith(fullPathStr,_T("htm")) ||
			 EndsWith(fullPathStr,_T("html"))) {
		image = 8;
		imageSel = 9;
		type = TEXT_TREE_HTML;
	}
	else if (EndsWith(fullPathStr,_T("log"))) {
		image = 4;
		imageSel = 5;
		type = TEXT_TREE_LOG;
	}
	// DEFAULT TO TEXT ICON
	else {
		image = 0;
		imageSel = 1;
		type = TEXT_TREE_TEXT;
	}
}

void CTreeViewText::UpdateTextTreeItem(HTREEITEM treeItem)
{
	DeleteTreeCtrlChildren(&m_treeCtrl,treeItem);
	CString treePathStr = InputPath(TreePath(treeItem) + _T("_log"));

	SetRedraw(false);
	FillInputTreeRecurse(treePathStr,treeItem);
	m_treeCtrl.Expand(treeItem,TVE_COLLAPSE);
	SetRedraw(true);
	m_treeCtrl.InvalidateRect(NULL);
	m_treeCtrl.UpdateWindow();
}

bool CTreeViewText::SelectTextFromPath(CString relPathStr)
{
	HTREEITEM item = TreeItemFromPath(relPathStr);
	if (item) {
		m_treeCtrl.SelectItem(item);
		return true;
	}
	return false;
}

HTREEITEM CTreeViewText::TreeItemFromPath(CString relOrFullPathStr, bool parentFlag)
{
	if (relOrFullPathStr == _T("")) return NULL;

	CString relPathStr = RelativizePath(relOrFullPathStr);

	if (relPathStr.Find(_T("input\\")) == 0)
		ClipLeft(relPathStr,6);

	HTREEITEM item = m_treeCtrl.GetRootItem();
	int i = 0;
	CString itemTextStr;
	CString currentLevel;
	int segDone = NthPathSegment(relPathStr,currentLevel,'\\',i++);
	if (parentFlag && segDone == 0)
		return m_treeCtrl.GetParentItem(item);

	while (item) {
		itemTextStr = m_treeCtrl.GetItemText(item);
		if (itemTextStr.CompareNoCase(currentLevel) == 0) {
			segDone = NthPathSegment(relPathStr,currentLevel,'\\',i++);
			if (segDone < 0 || (parentFlag && segDone == 0)) {
				break;
			}
			//m_treeCtrl.Expand(item,TVE_EXPAND);
			item = m_treeCtrl.GetChildItem(item);
		}
		else
			item = m_treeCtrl.GetNextSiblingItem(item);
	}

	return item;
}

CString CTreeViewText::CurrentTreeSelection(bool fullPathFlag, bool addingToFlag)
{
	HTREEITEM item = m_itemSelected;
	if (addingToFlag)
		item = m_itemToAddTo;
	if (item) {
		if (fullPathFlag)
			return InputPath(TreePath(item));
		return TreePath(item);
	}
	return _T("");
}

CString CTreeViewText::TreePath(HTREEITEM child, bool noCheck)
{
	if (!child) child = m_itemSelected;
	if (!child) return _T("");

	HTREEITEM parent;
	CString pathStr = m_treeCtrl.GetItemText(child);
	CString parentStr;
	
	while (parent = m_treeCtrl.GetParentItem(child)) {
		parentStr = m_treeCtrl.GetItemText(parent);
		if (!noCheck && IsChecked(parent))
			parentStr += _T("_log");
		pathStr = parentStr + _T("\\") + pathStr;
		child = parent;
	}
	return pathStr;
}

CString CTreeViewText::TreePath()
{
	return TreePath(m_itemSelected);
}

bool CTreeViewText::IsChecked(HTREEITEM inItem, bool parentFlag)
{
	if (!inItem)
		return false;

	int image;
	int imageSel;
	HTREEITEM item = inItem;
	if (parentFlag)
		item = m_treeCtrl.GetParentItem(inItem);
	m_treeCtrl.GetItemImage(item,image,imageSel);
	return image == 2 || image == 3 || image == 10 || image == 11;
}

TEXT_TREE_TYPE CTreeViewText::TextTreeType(HTREEITEM item)
{
	int image;
	int imageSel;
	TEXT_TREE_TYPE type;
	m_treeCtrl.GetItemImage(item,image,imageSel);

	switch (image) {
	case 0:
	case 1:
	case 2:
	case 3:
		type = TEXT_TREE_TEXT;
		break;
	case 4:
	case 5:
		type = TEXT_TREE_LOG;
		break;
	case 6:
	case 7:
		type = TEXT_TREE_FOLDER;
		break;
	case 8:
	case 9:
	case 10:
	case 11:
		type = TEXT_TREE_HTML;
		break;
	}

	return type;
}

void CTreeViewText::SetTextTreeType()
{
	m_textTreeType = TextTreeType(m_itemSelected);
	if (m_textTreeType == TEXT_TREE_FOLDER)
		m_lastFolderSelected = m_itemSelected;
}

TEXT_TREE_TYPE CTreeViewText::GetTextTreeType()
{
	return m_textTreeType;
}

static CString csTree[] =
{
	_T("Workspace 'demo': 1 project(s)"),
	_T("Demo files"),
	_T("File Folder")
};

CWnd* CTreeViewText::OnCreateView() 
{
	if (!m_treeCtrl.Create( WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
		CRect(0,0,0,0), this, AFX_IDW_PANE_FIRST ))
	{
		TRACE0( "Unable to create tree control.\n" );
		return NULL;
	}
						
	// create the image list for the tree control
	m_imageList.Create (IDB_TEXT, 16, 1, RGB(0,255,0));
	m_treeCtrl.SetImageList(&m_imageList, TVSIL_NORMAL);

	return &m_treeCtrl;
}

void CTreeViewText::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (textViewSelecting)
		return;
	textViewSelecting = true;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	// UPDATE TREE ITEM POINTERS
	m_itemLastSelected = m_itemSelected;
	m_itemSelected = m_treeCtrl.GetSelectedItem();

	// UPDATE CONCEPT POINTERS
	m_conceptSelected = (CONCEPT *)m_treeCtrl.GetItemData(m_itemSelected);

	// TAKE CARE OF CONCEPT FOLDER
	CONCEPT *nextConceptFolder = m_conceptSelected; 
	while (nextConceptFolder) {
		if (KBGetValueLong(nextConceptFolder,_T("type")) == TEXT_TREE_FOLDER)
			break;
		nextConceptFolder = cg->Up(nextConceptFolder);
	}

	SetTextTreeType();

	HTREEITEM nextToAddTo = m_itemToAddTo;
	nextToAddTo = m_itemSelected;
	m_boolSelectedIsLogFile = false;
	if (m_textTreeType != TEXT_TREE_FOLDER) {
		nextToAddTo = m_treeCtrl.GetParentItem(m_itemSelected);
		if (nextToAddTo) {
			CString possibleLogPathStr = InputPath(TreePath(nextToAddTo)) + _T("_log");
			if (IsDirectory(possibleLogPathStr)) {
				nextToAddTo = m_treeCtrl.GetParentItem(nextToAddTo);
				m_boolSelectedIsLogFile = true;
			}
		}
	}

	// CHECK TO SEE IF IN PAGE MODE AND IN DIFFERENT
	// ADVISE TO TURN OFF PAGE MODE
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	if (appState == VT_APP_NORMAL && appBoolPageMode) {

		CString oldPathDirStr = TreePath(m_itemLastSelected);
		CString newPathDirStr = TreePath(m_itemSelected);

		if (!IsDirectory(InputPath(oldPathDirStr)))
			oldPathDirStr = StripPath(oldPathDirStr,0,true);
		if (oldPathDirStr.Find(_T("_log")) > 0)
			oldPathDirStr = StripPath(oldPathDirStr,0,true);

		if (!IsDirectory(InputPath(newPathDirStr)))
			newPathDirStr = StripPath(newPathDirStr,0,true);
		if (newPathDirStr.Find(_T("_log")) > 0)
			newPathDirStr = StripPath(newPathDirStr,0,true);

		if (oldPathDirStr != newPathDirStr && !m_bFillingTree) {
			theApp.SavePageModeState();

			//CString stateStr = GetItemState(nextToAddTo);
			CString stateStr = KBGetValueString(nextConceptFolder,_T("state"));

			theApp.CloseAllDocuments(false);
			if (stateStr != _T("")) {
				CStringList states;
				StringToStringList(states,stateStr,_T("\r"));
				theApp.RestoreState(states,true);
			}

			/*
			CString msgStr = "You clicked in a different directory while in Page Mode.\n\nWhat do you want to do:";
			AskUser(msgStr,IDR_WAVE_LOSTBOY,2,1,"Turn off Page Mode","Continue","");
			if (appButtonReturn == 1) {
				appBoolPageMode = false;
			*/
		}
	}
	
	CString itemStr;
	itemStr = m_treeCtrl.GetItemText(nextToAddTo);
	m_itemToAddTo = nextToAddTo;
	m_conceptFolder = nextConceptFolder;

	// ADD TO COMBO BOX
	if (!m_boolBlockComboAdd && !IsChecked(m_treeCtrl.GetParentItem(m_itemSelected))) {
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		CString textPathStr = TreePath();
		wnd->AddComboText(textPathStr);
	}

	// IF USER CLICKED ON TEXT, MOVE SLOG POSITION TO CLICKED TEXT
	// Allow unprocessed text to be next item.	// 05/21/08 AM.
//	m_treeItemSlog = NextTreeCtrlItem(this,m_itemSelected,false,2,5);	// 05/21/08 AM.
	m_treeItemSlog = NextTreeCtrlItem(&m_treeCtrl,m_itemSelected,false,0,5);	// 05/21/08 AM.

	// Doesn't work to have next be a folder.	// 05/21/08 AM.
	// Changing this to be a proc/unproc HTML file.	// 05/21/08 AM.
//	if (!m_treeItemSlog)
//		m_treeItemSlog = NextTreeCtrlItem(this,m_itemSelected,false,6,9);
	if (!m_treeItemSlog)
		m_treeItemSlog = NextTreeCtrlItem(&m_treeCtrl,m_itemSelected,false,8,11);

	//if (m_textTreeType == TEXT_TREE_TEXT && m_boolLeftButton)
		//m_treeItemSlog = NULL;
	m_boolLeftButton = false;

	// TAKE CARE OF NEXT TEXT IF BUTTON TOGGLED
	if (appState != VT_APP_LOADING && appBoolPageMode) {
		if (m_textTreeType == TEXT_TREE_TEXT)
			MoveToProcessedText();
		else if (m_textTreeType == TEXT_TREE_HTML)
			MoveToProcessedText(true);
	}

	// SETUP INFO FOR BLOCKING RUNNING ON AN EMPTY FOLDER
	m_boolFolderEmpty = false;
	CString fullPathStr = CurrentTreeSelectionDir(true);
	if (m_textTreeType == TEXT_TREE_FOLDER && IsDirectoryEmpty(fullPathStr))
		m_boolFolderEmpty = true;

	if (m_textTreeType == TEXT_TREE_HTML)
		appLastURLSelected = appURL;

	m_boolProcessedText = true;

	textViewSelecting = false;

	*pResult = 0;
}

void CTreeViewText::OnDirtreemenuAddexistingtext() 
{
	AddExistingTextFile();
	SetRedraw(true);  // STRANGE BUG FIX FOR WHEN ADDING FIRST TIME TO AN EMPTY TEXT TAB
}

void CTreeViewText::AddExistingFolder(CString folderPathStr, bool topLevelFlag) {
	CFileFind finder;
	BOOL bWorking = finder.FindFile(folderPathStr+_T("\\*.*"));
	CString filenamePathStr, filenameStr, fullPathStr, folderPathStrNew;


	while (bWorking) {
		bWorking = finder.FindNextFile();
		filenameStr = finder.GetFileName();
		if (filenamePathStr == "." || filenamePathStr == "..") continue;

		filenamePathStr = finder.GetFilePath();
		if (folderPathStrNew == "") {
			folderPathStrNew = CurrentTreeSelectionDir(true,true) + _T("\\") + StripPath(folderPathStr,0,true);
			if (!IsDirectory(folderPathStrNew))
				CreateDirectory(folderPathStrNew, NULL);
		}

		if (m_itemToAddTo && !topLevelFlag)
			fullPathStr = CurrentTreeSelectionDir(true,true) + _T("\\") + filenameStr;
		else {
			fullPathStr = InputPath(filenameStr);
		}

		NLPCopyFile(filenamePathStr,fullPathStr);
	}

}

void CTreeViewText::AddExistingTextFile(bool topLevelFlag) 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	CString filterStr = _T("Text files (*.txt)|*.txt|Html files (*.htm;*.html)|*.htm|All files (*.*)|*.*||");
	CFileDialog fileDlg(true,_T("txt"),InputPath()+_T("\\*.txt"),OFN_ALLOWMULTISELECT,filterStr);
	_TCHAR filenameBuff[10001];
	filenameBuff[0] = '\0';
	fileDlg.m_ofn.lpstrFile = filenameBuff;
	fileDlg.m_ofn.nMaxFile = 10000;

	if (fileDlg.DoModal() == IDOK) {
		POSITION pos = fileDlg.GetStartPosition();
		CString filenameStr,filenamePathStr,fullPathStr;
		HTREEITEM item;

		while (pos) {
			filenamePathStr = fileDlg.GetNextPathName(pos);
			filenameStr = StripPath(filenamePathStr,0);

			item = m_itemToAddTo;

			//if (m_treeCtrl.GetCount())
			if (m_itemToAddTo && !topLevelFlag)
				fullPathStr = CurrentTreeSelectionDir(true,true) + _T("\\") + filenameStr;
			else {
				item = NULL;
				fullPathStr = InputPath(filenameStr);
			}

			NLPCopyFile(filenamePathStr,fullPathStr);
			HTREEITEM newItem = InsertItemAlphabetically(fullPathStr,item);
			CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
			if (pView)
				pView->Restore();
		}

	}
}

CString CTreeViewText::CurrentTreeSelectionDir(bool fullPathFlag, bool addingToFlag)
{
	CString pathStr = CurrentTreeSelection(fullPathFlag,addingToFlag);
	if (FileExists(pathStr))
		pathStr = StripPath(pathStr,0,true);
	if (pathStr.Find(_T("_log")) > 0)
		pathStr = StripPath(pathStr,0,true);
	return pathStr;
}

void CTreeViewText::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// PostMessage so that focus ends up on open document, not back here in the tree control
	m_bOpeningTextFile = true;
	PostMessage(WM_COMMAND, ID_DIRTREEMENU_EDITVIEW);

	*pResult = 1;
}

void CTreeViewText::OpenDirectoryTreeFile() 
{
	if (IsChecked(m_itemSelected) || !m_treeCtrl.ItemHasChildren(m_itemSelected)) {

		CString fullFilePathStr = InputPath(TreePath(m_itemSelected));

		if (IsDirectory(fullFilePathStr)) {
			return;
		}

		CString msgStr;
		int fileLength = GetFileLength(fullFilePathStr);
		msgStr.Format(_T("This file is very large (%s).  Opening will take a lot of time.\n\nDo you wish to proceed?"),PrettySize(fileLength));

		if (fileLength < LARGE_TEXT_SIZE || (fileLength >= LARGE_TEXT_SIZE && AfxMessageBox(msgStr, MB_YESNO) == IDYES)) {
			
			AfxGetApp()->OpenDocumentFile(fullFilePathStr);
		}
	}
}

void CTreeViewText::Load()
{
	InitKB();
	FillInputTree();
}

void CTreeViewText::Clear()
{
	SetRedraw(false);
	m_treeCtrl.DeleteAllItems();
	cg->rmChildren(m_conceptText);
	SetRedraw(true);
	RedrawWindow();
	InitVars();
}

#ifdef OLD_030626_
/*
void CTreeViewText::InitKB() 
{
	// INIT KB FOR TEXT TAB
	if (cg) {
		m_boolKBBased = false;
		m_conceptText = cg->findConcept(appGUIConcept, "text");
		if (!m_conceptText) {
			m_conceptText = cg->makeConcept(appGUIConcept, "text");
//			kbDirty = true;
		}
		else if (cg->Down(m_conceptText))
			m_boolKBBased = true;
	}
}
*/
#endif

void CTreeViewText::InitKB() 
{
	// INIT KB FOR TEXT TAB
	m_boolKBBased = false;
	if (cg) {
		m_conceptText = cg->findConcept(appGUIConcept, _T("text"));
		if (m_conceptText)	// 06/26/03 AM.
			cg->rmConcept(m_conceptText);	// 06/26/03 AM.
		m_conceptText = cg->makeConcept(appGUIConcept, _T("text"));
	}
}

void CTreeViewText::OnDirtreemenuNewtext() 
{
	AddNewText();
	SetRedraw(true);  // STRANGE BUG FIX FOR WHEN ADDING FIRST TIME TO AN EMPTY TEXT TAB
}

void CTreeViewText::AddNewText(bool topLevelFlag) 
{
	if (AskForText(_T("Enter name of new text file"))) {

		CString treePath;
		if ((m_textTreeType != TEXT_TREE_FOLDER && !m_boolHasFolder) || topLevelFlag)
			treePath = _T("");
		else
			treePath = CurrentTreeSelectionDir(false,true);

		if (treePath != _T(""))
			treePath += _T("\\") + appAskText;
		else
			treePath = appAskText;

		if (appAskText.Find(_T("."),0) < 0)
			treePath += _T(".txt");

		CString inputPath = InputPath(treePath);

		AddNewText(inputPath,_T(""),topLevelFlag);
	}
}

void CTreeViewText::AddNewText(CString filePathStr, CString contentStr, bool topLevelFlag) 
{
	// CHECK TO SEE IF THE FILE ALREADY EXISTS
	bool insertFlag = true;

	if (FileExists(filePathStr)) {
		CString msgStr;
		msgStr.Format(_T("The file \"%s\" already exists.\nDo you want to overwrite this file?"),filePathStr);
		if (AfxMessageBox(msgStr,MB_YESNO) == IDYES) {
			DeleteFile(filePathStr);
			insertFlag = false;
		}
		else
			return;
	}

	// ADD TO VISUAL TREE
	HTREEITEM item;
	if (topLevelFlag)
		item = NULL;
	else
		item = TreeItemFromPath(filePathStr,true);
	if (insertFlag) {
		InsertItemAlphabetically(filePathStr,item);
		CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
		if (pView)
			pView->Restore();
	}

	// ADD REAL FILE TO DIRECTORY
	NLPWriteFile(filePathStr,contentStr);
	CVisualTextDoc *doc = (CVisualTextDoc *)AfxGetApp()->OpenDocumentFile(filePathStr);
	if (!insertFlag) {
		doc->UpdateDocviewContent();
	}
}

void CTreeViewText::OnDirtreemenuAddfolder() 
{
	AddFolder();
	SetRedraw(true);  // STRANGE BUG FIX FOR WHEN ADDING FIRST TIME TO AN EMPTY TEXT TAB
}

void CTreeViewText::AddFolder(bool topLevelFlag) 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	if (AskForText(_T("Enter name of folder:"))) {

		CString fullPathStr;
		HTREEITEM item = m_itemToAddTo;

		// MAKE DIRECTORY
		if (!m_itemToAddTo || topLevelFlag) {
			fullPathStr = InputPath(appAskText);
			MakeDirectory(fullPathStr);
			item = NULL;
		}
		else {
			CString relPath = CurrentTreeSelection(false,true) + _T("\\") + appAskText;
			fullPathStr = InputPath(relPath);
			MakeDirectory(fullPathStr);
		}

		// ADD ITEM TO TREE AND KB
		HTREEITEM treeItem = m_itemToAddTo;
		if (topLevelFlag)
			treeItem = NULL;
		HTREEITEM newItem = InsertItemAlphabetically(fullPathStr,treeItem);
		CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
		if (pView)
			pView->Restore();
	}		
}

void CTreeViewText::OnDirtreemenuMovetotoplevel() 
{
	CString fromStr = InputPath(TreePath());
	CString nameStr = StripPath(fromStr,0);
	CString toStr = InputPath(nameStr);

	// BLOCK BROWSE DURING MOVE
	//SetRedraw(false);

	// MOVE FILE
	BOOL hold = appBoolPageMode;
	appBoolPageMode = false;
	MoveFile(fromStr,toStr);
	appBoolPageMode = (hold ? true : false);

	// MOVE KB CONCEPT
	cg->rmConcept(m_conceptSelected);
	KBInsertConceptAlphabetically(m_conceptText,m_conceptSelected,false);

	// MOVE TREE ITEM
	TreeItemMove(&m_treeCtrl,m_itemSelected,NULL,true);

	SetRedraw(true);

}

void CTreeViewText::MoveLogFiles(CString fileName)
{
	//SetRedraw(false);
	bool save = cg->getDirty();	// 07/11/03 AM.

	DeleteChildren(fileName,false);

	CString logDir = fileName + _T("_log");
	RecurseRemoveLogs(logDir,false);
	MakeLogDirectory(fileName);

	CFileFind finder;
	CString pathStr = OutputPath(_T("*.*"));
	BOOL bWorking = finder.FindFile(pathStr);
	CString logName;
	CString origFile;
	CString newFile;
	HTREEITEM item = TreeItemFromPath(fileName);

	while (bWorking) {
		bWorking = finder.FindNextFile();
		logName = finder.GetFileName();
		if (logName != _T(".") && logName != _T("..") && logName != _T("rfa.log") &&
			logName != _T("def.log")) {
			origFile = OutputPath(logName);
			newFile = logDir + _T("\\") + logName;

			// COPY DOS FILE
			CopyFile(origFile,newFile,false);

			// ADD TREE ITEM & KB CONCEPT
			InsertItemAlphabetically(newFile,item,true,false);
		}
	}
	finder.Close();

	cg->setDirty(save);	// Kb status unaffected.	// 07/11/03 AM.
	//SetRedraw(true);
}

void CTreeViewText::OnDirtreemenuDelete() 
{
	DeleteSelected();
}

void CTreeViewText::DeleteSelected() 
{
	CString fileName = CurrentTreeSelection(false);
	CString fileNameFullPath = CurrentTreeSelection(true);

	CString msgStr = _T("Do you want to delete \"") + fileName + _T("\"");

	if (AfxMessageBox(msgStr, MB_YESNO) == IDYES) {

		HTREEITEM nextItem = m_treeCtrl.GetNextSiblingItem(m_itemSelected);

		// REMOVE ACTUAL FILES FROM DIRECTORY
		if (IsDirectory(fileNameFullPath+_T("_log"))) {
			if (!RecursiveRemove(fileNameFullPath+_T("_log"),false,0))
				return;
		}

		if (!RecursiveRemove(fileNameFullPath,false,0))
			return;

		// REMOVE FROM KB
		cg->rmConcept(m_conceptSelected);

		// REMOVE FROM TEXT TAB
		SetRedraw(false);
		m_treeCtrl.DeleteItem(m_itemSelected);
		DeleteClearIfNone();

		// IF LAST TEXT, CLEAR PROCESSED TEXT VARIABLE
		if (m_treeCtrl.GetCount() == 0)
			theApp.m_pMainFrame->m_wndTextView.SelectTextFromPath(_T(""));

		if (nextItem)
			m_treeCtrl.SelectItem(nextItem);

		SetRedraw(true);
		m_treeCtrl.InvalidateRect(NULL);
		m_treeCtrl.UpdateWindow();

		CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
		if (pView)
			pView->Restore();
	}	
}

void CTreeViewText::DeleteClearIfNone() 
{
	if (!m_treeCtrl.GetCount()) {
		m_itemToAddTo = NULL;
		m_itemSelected = NULL;
	}
}

void CTreeViewText::OnDirtreemenuDeletechildren() 
{
	DeleteChildren();
}

void CTreeViewText::DeleteChildren(CString fullPathStr, bool askToConfirmFlag) 
{
	CString relPathStr;
	CONCEPT *concept = m_conceptSelected;
	HTREEITEM item = m_itemSelected;

	if (fullPathStr == _T("")) {
		relPathStr = CurrentTreeSelection(false);
		fullPathStr = CurrentTreeSelection(true);
	}
	else {
		item = TreeItemFromPath(fullPathStr);
		if (!item)	// 11/07/02 AM.
			{
			CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();	// 11/07/02 AM.
			CString strDebug;	// 11/07/02 AM.
			strDebug.Format(_T("Try Refresh in the Text Tab."));	// 11/07/02 AM.
			wnd->AddDebugLine(_T("Error:"), strDebug, true);	// 11/07/02 AM.
			return;	// 11/07/02 AM.
			}
		concept = (CONCEPT *)m_treeCtrl.GetItemData(item);
		relPathStr = RelativizePath(fullPathStr);
	}

	CString msgStr = _T("Do you want to delete the children for \"") + relPathStr + _T("\"");

	if (!askToConfirmFlag || AfxMessageBox(msgStr, MB_YESNO) == IDYES) {

		// DELETE FROM DOS
		if (IsDirectory(fullPathStr+_T("_log"))) {
			CString logPathStr = fullPathStr+_T("_log");
			if (!RecursiveRemove(logPathStr,true))
				return;
			if (!RemoveDirectory(logPathStr))
				return;
		}
		else if (!RecursiveRemove(fullPathStr,true))
			return;

		// DELETE FROM TEXT TAB
		DeleteTreeItemChild(item);

		// REMOVE FROM KB
		cg->rmChildren(concept);
		CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
		if (pView)
			pView->Restore();
	}
}

void CTreeViewText::DeleteTreeItemChild(HTREEITEM item) 
{
	if (!item)
		item = m_itemSelected;
	DeleteTreeCtrlChildren(&m_treeCtrl,item);
	DeleteClearIfNone();
	RetypeItem(item);
}

void CTreeViewText::OnDirtreemenuEditview() 
{
	if (m_bOpeningTextFile)
		appBoolOpeningTextFile = true;
	OpenDirectoryTreeFile();
	if (m_bOpeningTextFile)
		appBoolOpeningTextFile = false;
	m_bOpeningTextFile = false; 
}

void CTreeViewText::OnDirtreemenuRename() 
{
	CString fileName = CurrentTreeSelection(false);
	fileName = StripPath(fileName,0);
	CString fileNameFullPath = CurrentTreeSelection(true);
	CString filePathMinusOne = StripPath(fileNameFullPath,0,true);

	if (AskForText(_T("Enter new text file name:"),fileName)) {
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		CString oldPathStr = TreePath();
		CString newFileStr = filePathMinusOne + _T("\\") + appAskText;

		if ((FileExists(fileNameFullPath) || IsDirectory(fileNameFullPath)) &&
			!UTL_Rename(fileNameFullPath,newFileStr))
			return;

		// IF THE RENAMED TEXT WINDOW IS OPEN, CHANGE THE TITLE
		CDocument *doc;
		if ((doc = theApp.IsVisualTextDocumentOpen(_T("VisualText"),fileNameFullPath))) {
			doc->SetPathName(newFileStr,false);
			CView *view;
			view = GetFirstView(doc);
			view->UpdateWindow();
		}

		_TCHAR buff[1001];
		StringToChar(buff,appAskText,1000);
		cg->renameConcept(m_conceptSelected,buff);
		m_treeCtrl.SetItemText(m_itemSelected,appAskText);

		if (m_treeCtrl.ItemHasChildren(m_itemSelected)) {
			CString oldLogPathStr = fileNameFullPath+_T("_log");
			CString newLogPathStr = newFileStr+_T("_log");
			if (_taccess(oldLogPathStr, 0) == 0 && IsDirectory(oldLogPathStr) ) {
				UTL_Rename(oldLogPathStr,newLogPathStr);
			}
		}

		CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
		if (pView)
			pView->Restore();
	}	
}

void CTreeViewText::OnDirtreemenuGotowebpage() 
{
	CString htmlTextPathStr = InputPath(TreePath());
	appURL = GetUrlFromUrlFile(htmlTextPathStr);
	theApp.OpenNewDocument(_T("Browser"),appURL);
}

void CTreeViewText::OnDirtreemenuUpdatewebpage() 
{
	CString htmlTextPathStr = InputPath(TreePath());
	CString urlStr = GetUrlFromUrlFile(htmlTextPathStr);
	
	CString htmlTextStr = _T("");
	if (FetchURLString(urlStr,htmlTextStr))
		NLPWriteFile(htmlTextPathStr,htmlTextStr);	
}

void CTreeViewText::OnDirtreemenuViewaswebpage() 
{
	CString fileName = InputPath(TreePath());
	if (EndsWith(fileName,_T("htm")) ||
		EndsWith(fileName,_T("html")))
		theApp.OpenNewDocument(_T("Browser"),fileName);
}

void CTreeViewText::OnDirtreemenuViewoutputfile() 
{
	OpenOutputFile();
}

bool CTreeViewText::OpenOutputFile() 
{
	CString currentTextStr = CurrentTreeSelection(true);
	CString selectTextStr = currentTextStr;

	if (selectTextStr.Find(_T("\\output.txt")) >= 0) 	// IF SELECTION IS "output.txt" ITSELF
		currentTextStr = _T("");
	else if (IsDirectory(selectTextStr+_T("_log")))
		selectTextStr += _T("_log\\output.txt");
	else if (EndsWith(selectTextStr,_T("_log")))
		selectTextStr += _T("\\output.txt");
	else if (selectTextStr.Find(_T("_log\\")) > 0)
		selectTextStr = StripPath(selectTextStr,0,true) + _T("\\output.txt");
	
	if (selectTextStr == currentTextStr || !FileExists(selectTextStr))
		return false;

	//if (openFile)
		AfxGetApp()->OpenDocumentFile(selectTextStr);

	return true;
}

void CTreeViewText::OnDirtreemenuRunanalyzer() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->Run();
}

void CTreeViewText::OnDirtreemenuHelp() 
{
	HelpTopic(_T("Text_Tab_Popup"));	// 04/27/01 AM.
}

void CTreeViewText::SelectText(CDocument *doc)
{
	CString pathStr = doc->GetPathName();
	pathStr = pathStr.Right(pathStr.GetLength()-InputPath().GetLength()-1);
	SelectTextFromPath(pathStr);
}

CString CTreeViewText::AddTextFile(CString fileNameStr, CString contentStr, CString extStr, bool updateDisplay)
{
	CString path = CurrentTreeSelectionDir(true);
	if (extStr == _T(""))
		extStr = _T("txt");
	if (path == _T(""))
		path = InputPath();
	fileNameStr = fileNameStr + _T(".") + extStr;
	CString treePath = path + _T("\\") + fileNameStr;
	NLPWriteFile(treePath,contentStr);

	// Add Item
	if (updateDisplay) {
		InsertItemAlphabetically(treePath,m_itemToAddTo);
		SetRedraw(true);  // PATCH FOR MFC BUG
		CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
		if (pView)
			pView->Restore();
	}

	return treePath;
}

HTREEITEM CTreeViewText::InsertItemAlphabetically(CString fullPathStr, HTREEITEM parentItem,
												  bool dontOpenFlag, bool selectNewItemFlag)
{
	HTREEITEM child = NULL;

	if (parentItem)
		child = m_treeCtrl.GetChildItem(parentItem);

	HTREEITEM childBefore = NULL;
	HTREEITEM newItem = NULL;
	CString itemTextStr;
	CString nameStr = StripPath(fullPathStr,0);
	int image,imageSel;
	TEXT_TREE_TYPE type;

	ImageTypeFromFile(fullPathStr,image,imageSel,type);

	// FIND INSERT POINT
	childBefore = AlphabeticalInsertPoint(&m_treeCtrl,parentItem,nameStr);
	//childBefore = AlphabeticalInsertPoint(this,child,nameStr);

	// ADD TO TREE AND KB
	if (childBefore || !child) {
		CONCEPT *parentConcept;
		if (!parentItem) {
			parentItem = TVI_ROOT;
			parentConcept = m_conceptText;
		}
		else
			parentConcept = (CONCEPT *)m_treeCtrl.GetItemData(parentItem);

		// ADD TO DISPLAY TREE
		newItem = m_treeCtrl.InsertItem(nameStr,image,imageSel,parentItem,childBefore);

		// INSERT INTO KB
		CONCEPT *newConcept = KBInsertConceptAlphabetically(parentConcept,nameStr,false);
		KBReplaceValue(newConcept,_T("type"),type,false);

		// ATTACH CONCEPT TO ITEM
		m_treeCtrl.SetItemData(newItem,(DWORD)newConcept);

		// TAKE CARE OF DISPLAYING CORRECTLY
		if (!dontOpenFlag)
			m_treeCtrl.EnsureVisible(newItem);
		if (selectNewItemFlag || (!dontOpenFlag && m_treeCtrl.GetCount() == 1))
			m_treeCtrl.SelectItem(newItem);
	}

	return newItem;
}

CString CTreeViewText::GetSelection()
{
	if (!m_treeCtrl) return "";
	return StateString(&m_treeCtrl,m_treeCtrl.GetRootItem(),STATE_GET_SELECTED);
}

void CTreeViewText::SetSelection(CString selStr)
{
	m_boolFillOneLevel = false;
	if (appState != VT_APP_LOADING)	// 03/16/07 AM.
		SetRedraw(false);
	StateFromString(&m_treeCtrl,m_treeCtrl.GetRootItem(),STATE_SELECT,selStr);
	if (appState != VT_APP_LOADING)	// 03/16/07 AM.
		SetRedraw(true);
	m_boolFillOneLevel = true;
}

CString CTreeViewText::GetState()
{
	return StateString(&m_treeCtrl,m_treeCtrl.GetRootItem(),STATE_GET_EXPANDED);
}

void CTreeViewText::SetState(CString stateStr)
{
	if (appState != VT_APP_LOADING)	// 03/16/07 AM.
		SetRedraw(false);
	StateFromString(&m_treeCtrl,m_treeCtrl.GetRootItem(),STATE_EXPAND_COLLAPSE,stateStr);
	if (appState != VT_APP_LOADING)	// 03/16/07 AM.
		SetRedraw(true);
}

void CTreeViewText::OnUpdateDirtreemenuAddexistingtext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_textTreeType == TEXT_TREE_FOLDER || !m_boolHasFolder || m_textTreeType == TEXT_TREE_TEXT);	
}

void CTreeViewText::OnUpdateDirtreemenuViewaswebpage(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_textTreeType == TEXT_TREE_HTML);			
}

void CTreeViewText::OnDirtreemenuServerarchives() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->ServerArchive(_T("Text"));
}

CString CTreeViewText::ArchiveText()
{
	CString pathStr = InputPath();
	return ArchiveFiles(pathStr,_T("Text"),_T("*.*"));
}

void CTreeViewText::OnDirtreemenuUploadtext() 
{
	CString localFileStr = ArchiveText();
	if (localFileStr != _T("")) {
		CString remoteFileStr = StripPath(localFileStr,0);
		UploadArchive(_T("Text"),remoteFileStr,localFileStr);
	}
}

void CTreeViewText::OnDirtreemenuLocalarchive() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->LocalArchive(_T("Text"));
}

void CTreeViewText::OnDirtreemenuArchivetext() 
{
	ArchiveText();	
}

void CTreeViewText::OnUpdateDirtreemenuMovetotoplevel(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_treeCtrl.GetCount() > 0);	
}

void CTreeViewText::OnUpdateDirtreemenuDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_treeCtrl.GetCount() > 0);	
}

void CTreeViewText::OnUpdateDirtreemenuDeletechildren(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_treeCtrl.GetCount() > 0);	
}

void CTreeViewText::OnUpdateDirtreemenuEditview(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_treeCtrl.GetCount() > 0);	
}

void CTreeViewText::OnUpdateDirtreemenuRename(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_treeCtrl.GetCount() > 0);	
}

void CTreeViewText::OnUpdateDirtreemenuUpdatewebpage(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_treeCtrl.GetCount() > 0);	
}

void CTreeViewText::OnUpdateDirtreemenuArchivetext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_treeCtrl.GetCount() > 0);	
}

void CTreeViewText::OnUpdateDirtreemenuUploadtext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_treeCtrl.GetCount() > 0);	
}

void CTreeViewText::OnUpdateDirtreemenuRunanalyzer(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_treeCtrl.GetCount() > 0);	
}

void CTreeViewText::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // This code is to prevent accidental drags.
    if ((GetTickCount() - m_dwDragStart) < DRAG_DELAY)
		return;


	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	m_hitemDrag = pNMTreeView->itemNew.hItem;
	m_hitemDrop = NULL;
	m_treeCtrl.SelectItem(m_hitemDrag);

	// Check to see if you should be able to move a file
	CString pathStr = TreePath(m_itemSelected);
	if (pathStr.Find(_T("_log\\")) > 0) {
		AfxMessageBox(_T("Visual Text doesn't allow moving of analyzer log and dump files."));
		return;
	}

	m_pDragImage = m_treeCtrl.CreateDragImage(m_hitemDrag);

	// get the image list for dragging
	// CreateDragImage() returns NULL if no image list
	// associated with the tree view control
	if(!m_pDragImage)
		return;

	m_bLDragging = true;
	m_pDragImage->BeginDrag(0, CPoint(-15,-30));
	POINT pt = pNMTreeView->ptDrag;
	ClientToScreen(&pt);
	m_pDragImage->DragEnter(NULL, pt);
	SetCapture();

	if (0 == m_nTimerIDDrag)
		m_nTimerIDDrag = SetTimer(ID_TIMER_DRAG, 75, NULL);
}

void CTreeViewText::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bLDragging) {
		HTREEITEM	hItem;
		UINT		flags;
		POINT pt = point;

		ClientToScreen(&pt);
		pt.x -= DRAG_X_ADJUST;
		pt.y -= DRAG_Y_ADJUST;
		CImageList::DragMove(pt);

		if ((hItem = m_treeCtrl.HitTest(point, &flags)) != NULL) {
			CImageList::DragShowNolock(false);
			m_hitemDrop = GetDropTarget(hItem);
			m_treeCtrl.SelectDropTarget(m_hitemDrop);
			if (!IsDropSource(m_hitemDrop))
				m_hitemDrop = NULL;
			CImageList::DragShowNolock(true);
		}
		else
			m_hitemDrop = NULL;

		if (m_hitemDrop)
			SetCursor(m_dropCursor);
		else
			SetCursor(m_noDropCursor);
	}
	
	CPaneView::OnMouseMove(nFlags, point);
}

void CTreeViewText::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CPaneView::OnLButtonUp(nFlags, point);

	// SRP 010502: Ordering of some statements has changed - old ordering caused hanging of Release Version when drop was prohibited.
	if (m_bLDragging) {
		if (m_nTimerIDDrag) {
			KillTimer(m_nTimerIDDrag);
			m_nTimerIDDrag = 0;
		}
		m_bLDragging = false;
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		ReleaseCapture();
		delete m_pDragImage;

		if (m_hitemDrag == m_hitemDrop || !IsDropSource(m_hitemDrop)) {
			// Remove drop target highlighting
			m_treeCtrl.SelectDropTarget(NULL); // SRP 010502 This must follow IsDropSource()
			return;
		}

		// Remove drop target highlighting
		m_treeCtrl.SelectDropTarget(NULL); // SRP 010502

		// If Drag item is an ancestor of Drop item then return
		HTREEITEM htiParent = m_hitemDrop;
		while((htiParent = m_treeCtrl.GetParentItem( htiParent )) != NULL ) {
			if( htiParent == m_hitemDrag ) return;
		}

		// Move File
		CString fileNameStr = m_treeCtrl.GetItemText(m_hitemDrag);
		CString dragPathStr = InputPath(TreePath(m_hitemDrag));
		CString dropPathStr = InputPath(TreePath(m_hitemDrop))+ _T("\\") + fileNameStr;
		MoveFile(dragPathStr,dropPathStr);

		// Change reference if Sample File
		_TCHAR attrBuff[1001];
		_TCHAR ovalBuff[1001];
		_TCHAR nvalBuff[1001];
		CONCEPT *concept = cg->findConcept(cg->findRoot(),_T("gram"));
		StringToChar(attrBuff,_T("text"),1000);
		StringToChar(ovalBuff,RelativizePath(dragPathStr),1000);
		StringToChar(nvalBuff,RelativizePath(dropPathStr),1000);
		cg->attrChange(concept,attrBuff,ovalBuff,nvalBuff);

		// MOVE KB TEXT CONCEPT
		CONCEPT *dragConcept = (CONCEPT *)m_treeCtrl.GetItemData(m_hitemDrag);
		CONCEPT *dropConcept = (CONCEPT *)m_treeCtrl.GetItemData(m_hitemDrop);
		CString dragStr = ConceptName(dragConcept);
		CString dropStr = ConceptName(dropConcept);
		cg->rmConcept(dragConcept);
		KBInsertConceptAlphabetically(dropConcept,dragConcept,false);
//		kbDirty = true;

		// Move interface tree stuff
		TreeItemMove(&m_treeCtrl,m_hitemDrag,m_hitemDrop);
	}
}

void CTreeViewText::OnTimer(UINT nIDEvent) 
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

			HTREEITEM hitem = m_treeCtrl.GetFirstVisibleItem();

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
					//pTree->m_treeCtrl.SelectDropTarget(hitem);
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
					int nCount = m_treeCtrl.GetVisibleCount();
					for (int i=0; i<nCount-1; ++i)
						hitem = m_treeCtrl.GetNextVisibleItem(hitem);
					//if (hitem)
						//pTree->m_treeCtrl.SelectDropTarget(hitem);
					m_hitemDrop = hitem;
					CImageList::DragShowNolock(true);
				}
			}
		}
		break;
	}

	CPaneView::OnTimer(nIDEvent);
}

bool CTreeViewText::IsDropSource(HTREEITEM hItem)
{
	int image;
	int imageSel;
	m_treeCtrl.GetItemImage(hItem,image,imageSel);
	return image == 6 || image == 7;
}

HTREEITEM CTreeViewText::GetDropTarget(HTREEITEM hItem)
{
	// inhibit drop on the drop source or its parent
	if (hItem == m_hitemDrag || hItem == m_treeCtrl.GetParentItem(m_hitemDrag))
		return NULL;
	return hItem;
}

void CTreeViewText::OnLButtonDown(UINT nFlags, CPoint point) 
{
	logmsg(_T("CTreeViewText::OnLbuttonDown"));	// 05/14/08 AM.
	m_dwDragStart = GetTickCount();
	m_boolLeftButton = true;
	CPaneView::OnLButtonDown(nFlags, point);
}

void CTreeViewText::OnRButtonDown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint point, ptScreen;
	::GetCursorPos(&ptScreen);
	
	point = ptScreen;
	m_treeCtrl.ScreenToClient(&point);

	logmsg(_T("CTreeViewText::OnRbuttonDown"));	// 05/14/08 AM.
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	if (!wnd->AnalyzerCreated())
		return;

	// TAKE CARE OF GRAYING ITEMS
	UINT disables[100];
	for (int i=0; i<100; i++)
		disables[i] = 0;
	int count = 0;
	
	if (m_treeCtrl.GetCount() == 0) {
		disables[count++] = ID_DIRTREEMENU_MOVETOTOPLEVEL;
		disables[count++] = ID_DIRTREEMENU_DELETE;
		disables[count++] = ID_DIRTREEMENU_DELETECHILDREN;
		disables[count++] = ID_DIRTREEMENU_DELETELOGS;
		disables[count++] = ID_DIRTREEMENU_EDITVIEW;
		disables[count++] = ID_DIRTREEMENU_RENAME;
		disables[count++] = ID_DIRTREEMENU_GOTOWEBPAGE;
		disables[count++] = ID_DIRTREEMENU_UPDATEWEBPAGE;
		disables[count++] = ID_DIRTREEMENU_VIEWASWEBPAGE;
		disables[count++] = ID_DIRTREEMENU_ARCHIVETEXT;
		disables[count++] = ID_DIRTREEMENU_LOCALARCHIVE;
		disables[count++] = ID_DIRTREEMENU_SERVERARCHIVES;
		disables[count++] = ID_DIRTREEMENU_UPLOADTEXT;
		disables[count++] = ID_DIRTREEMENU_RUNANALYZER;
		disables[count++] = ID_DIRTREEMENU_ATTRIBUTES;

		disables[count++] = ID_DIRTREEMENU_TOOLS_ASCIITABLE;
		disables[count++] = ID_DIRTREEMENU_DICTIONIZE;
		disables[count++] = ID_DIRTREEMENU_TOOLS_HEXVIEWER;
		disables[count++] = ID_DIRTREEMENU_MASK;
		disables[count++] = ID_DIRTREEMENU_TOOLS_CHARACTERVIEWER;
		disables[count++] = ID_DIRTREEMENU_TOOLS_EMAIL;
		disables[count++] = ID_DIRTREEMENU_TEXTFILEINFO;
		disables[count++] = ID_DIRTREEMENU_TOOLS_WORDNET;

		disables[count++] = ID_DIRTREEMENU_ADD_TOPLEVEL_EXISTINGTEXTFILE;
		disables[count++] = ID_DIRTREEMENU_ADD_TOPLEVEL_FOLDER;
		disables[count++] = ID_DIRTREEMENU_ADD_TOPLEVEL_NEWTEXTFILE;
	}
	else if (m_textTreeType == TEXT_TREE_TEXT || m_textTreeType == TEXT_TREE_FOLDER) {
		disables[count++] = ID_DIRTREEMENU_GOTOWEBPAGE;
		disables[count++] = ID_DIRTREEMENU_UPDATEWEBPAGE;
		disables[count++] = ID_DIRTREEMENU_VIEWASWEBPAGE;
		disables[count++] = ID_DIRTREEMENU_EDITVIEW;
	}

	if (m_conceptParent == m_conceptText) {
		disables[count++] = ID_DIRTREEMENU_MOVETOTOPLEVEL;
	}

	if (m_textTreeType == TEXT_TREE_TEXT &&
		!OpenProcessedFiles(_T(".txt"),false) &&
		!OpenProcessedFiles(_T(".log"),false)) {
		disables[count++] = ID_DIRTREEMENU_ADDEXISTINGFOLDER;
		disables[count++] = ID_DIRTREEMENU_ADDFOLDER;
		disables[count++] = ID_DIRTREEMENU_ADDEXISTINGTEXT;
		disables[count++] = ID_DIRTREEMENU_NEWTEXT;
		disables[count++] = ID_DIRTREEMENU_DELETECHILDREN;
		disables[count++] = ID_DIRTREEMENU_DELETELOGS;
	}

	if (m_treeCtrl.GetCount() && !TreeCtrlIsToplevelItem(&m_treeCtrl,m_itemSelected) && !m_itemToAddTo) {
		disables[count++] = ID_DIRTREEMENU_ADD_TOPLEVEL_EXISTINGTEXTFILE;
		disables[count++] = ID_DIRTREEMENU_ADD_TOPLEVEL_FOLDER;
		disables[count++] = ID_DIRTREEMENU_ADD_TOPLEVEL_NEWTEXTFILE;
	}

	if (m_boolSelectedIsLogFile) {
		disables[count++] = ID_DIRTREEMENU_ADDEXISTINGFOLDER;
		disables[count++] = ID_DIRTREEMENU_ADDFOLDER;
		disables[count++] = ID_DIRTREEMENU_ADDEXISTINGTEXT;
		disables[count++] = ID_DIRTREEMENU_NEWTEXT;
		disables[count++] = ID_DIRTREEMENU_DELETECHILDREN;
		disables[count++] = ID_DIRTREEMENU_DELETELOGS;
		disables[count++] = ID_DIRTREEMENU_RENAME;
		disables[count++] = ID_DIRTREEMENU_GOTOWEBPAGE;
		disables[count++] = ID_DIRTREEMENU_UPDATEWEBPAGE;
		disables[count++] = ID_DIRTREEMENU_VIEWASWEBPAGE;
		disables[count++] = ID_DIRTREEMENU_MOVETOTOPLEVEL;
	}

	if (m_boolFolderEmpty) {
		disables[count++] = ID_DIRTREEMENU_RUNANALYZER;
	}

	if (!m_itemSelected) {
		disables[count++] = ID_DIRTREEMENU_DICTIONARY_ADDWORDS;
	}

	CMenu menu, menuLookup;
	menu.LoadMenu(IDR_TREEVIEWTEXT);
	CMenu *context = menu.GetSubMenu(0);
	int nPos;
	if (FindMenuPosFromString(context, _T("Dictionary"), nPos)) {
		CMenu* pMenuDict = context->GetSubMenu(nPos);
		CString strLookup = _T("&Lookup");
		if (FindMenuPosFromString(pMenuDict, strLookup, nPos)) {
			CMenu menuLookup;
			CPrefLookupPage::GetDictionaryLookupMenu(menuLookup, ID_DICTIONARY_URL_LOOKUP_BASE, m_strNames, m_strURLs);
			pMenuDict->DeleteMenu(nPos,MF_BYPOSITION);
			pMenuDict->InsertMenu(nPos,MF_ENABLED | MF_BYPOSITION | MF_POPUP | MF_STRING,
				(UINT)menuLookup.GetSafeHmenu(),strLookup);
			menuLookup.Detach();
		}
	}
	ClientToScreen(&point);
	PopupContextMenu(this,context,point,disables);
			
	//CPaneView::OnRButtonDown(nFlags, point);
}

CString CTreeViewText::NextSlog(bool backward) 
{
	m_boolLeftButton = false;

	// Don't know who commented the next three lines or why. Redo is below them. // 05/21/08 AM.
//	if (!m_treeItemSlog)
//		m_treeItemSlog = m_itemSelected;
//	m_treeItemSlog = NextTreeCtrlItem(this,m_itemSelected,backward,2,5);

	// Ok, want to allow unprocessed text also, and HTML files.	// 05/21/08 AM.
	m_treeItemSlog = NextTreeCtrlItem(&m_treeCtrl,m_itemSelected,backward,0,5);	// 05/21/08 AM.
	if (!m_treeItemSlog)	// 05/21/08 AM.
		m_treeItemSlog = NextTreeCtrlItem(&m_treeCtrl,m_itemSelected,backward,8,11);	// 05/21/08 AM.

	CString slogPathStr = _T("");

	if (m_treeItemSlog) {
		m_boolBlockComboAdd = true;
		slogPathStr = TreePath(m_treeItemSlog);
		m_treeCtrl.SelectItem(m_treeItemSlog);
		m_boolBlockComboAdd = false;
	}

	return slogPathStr;
}

void CTreeViewText::SelectSlog() 
{
	m_boolLeftButton = false;

	m_boolBlockComboAdd = true;
	m_treeCtrl.SelectItem(m_treeItemSlog);
	UpdateTextTreeItem(m_treeItemSlog);
	UpdateWindow();
	m_boolBlockComboAdd = false;
}

void CTreeViewText::SetToReslog() 
{
	m_treeItemSlog = NULL;
}

void CTreeViewText::Reload(CString fullPathStr) 
{
	SetRedraw(false);

	HTREEITEM item = TreeItemFromPath(fullPathStr);
	if (item) {
		DeleteTreeCtrlChildren(&m_treeCtrl,item);
		CString logPathStr = fullPathStr + _T("_log");
		if (IsDirectory(fullPathStr))
			FillInputTreeRecurse(fullPathStr,item);
		else
			FillInputTreeRecurse(logPathStr,item);
		RetypeItem(item,fullPathStr);
	}

	SetRedraw(true);
	m_treeCtrl.InvalidateRect(NULL);
	m_treeCtrl.UpdateWindow();
}

void CTreeViewText::SetTextAsAnalyzed(CString fullPathStr)
{
	CString inputRelPathStr = RelativizePath(fullPathStr);
	HTREEITEM item = TreeItemFromPath(inputRelPathStr);
	int image,imageSel;
	TEXT_TREE_TYPE type;
	ImageTypeFromFile(fullPathStr,image,imageSel,type);
	
	// IF TEXT IMAGE WITHOUT CHECKMARK, THEN PUT CHECKMARK
	if (image == 0) {
		image = 2;
		imageSel = 3;
	}
	// IF URL IMAGE WITHOUT CHECKMARK, THEN PUT CHECKMARK
	else if (image == 8) {
		image = 10;
		imageSel = 11;
	}
	m_treeCtrl.SetItemImage(item,image,imageSel);
}

void CTreeViewText::RetypeItem(CString fullPathStr)
{
	CString inputRelPathStr = RelativizePath(fullPathStr);
	HTREEITEM item = TreeItemFromPath(inputRelPathStr);
	if (item)
		RetypeItem(item,fullPathStr);
}

void CTreeViewText::RetypeItem(HTREEITEM item, CString fullPathStr)
{
	int image,imageSel;
	TEXT_TREE_TYPE type;
	if (fullPathStr == _T(""))
		fullPathStr = InputPath(TreePath(item));
	ImageTypeFromFile(fullPathStr,image,imageSel,type);
	m_treeCtrl.SetItemImage(item,image,imageSel);
}

void CTreeViewText::OnDirtreemenuDeletelogs() 
{
	StartWaitCursor();

	CString fileName = CurrentTreeSelection(false);
	CString fileNameFullPath = CurrentTreeSelection(true);

	CString msgStr = _T("Do you want to delete all log files below \"") + fileName + _T("\"");

	if (AfxMessageBox(msgStr, MB_YESNO) == IDYES) {
		CString logPathStr = fileNameFullPath+_T("_log");
		if (IsDirectory(logPathStr))
			fileNameFullPath = logPathStr;

		// REMOVE FROM DOS && KB - together since this is very selective.
		RecurseRemoveLogs(fileNameFullPath);
		// SRP 010808
		CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
		if (pView)
			pView->Restore();
//		kbDirty = true;
	}
	StopWaitCursor();
}

void CTreeViewText::DeleteTreeItemRecursive(HTREEITEM item, bool parentProcessed)
{
	if (item == NULL)
		item = m_treeCtrl.GetChildItem(m_itemSelected);
	HTREEITEM child;
	HTREEITEM nextItem;
	CString itemTextStr;
	int image, imageSel;
	bool processed;
	CONCEPT *itemConcept;

	while (item) {
		m_treeCtrl.GetItemImage(item,image,imageSel);
		nextItem = m_treeCtrl.GetNextSiblingItem(item);
		itemTextStr = m_treeCtrl.GetItemText(item);
		child = m_treeCtrl.GetChildItem(item);
		processed = image == 4 || image == 5 ? true : false;

		if (child)
			DeleteTreeItemRecursive(child,processed);

		if (parentProcessed) {
			itemConcept = (CONCEPT *)m_treeCtrl.GetItemData(item);
			cg->rmConcept(itemConcept);
			m_treeCtrl.DeleteItem(item);
			DeleteClearIfNone();
		}
		else if (4 == image) {
			m_treeCtrl.SetItemImage(item,2,3);
		}
		else if (8 == image) {
			m_treeCtrl.SetItemImage(item,6,7);
		}
		item = nextItem;
	}
}

void CTreeViewText::RecurseRemoveLogs(CString pathStr, bool displayMsgFlag)
{
	if (!pathStr || pathStr == _T(""))
		return;

	bool save = cg->getDirty();	// 07/11/03 AM.
	CFileFind finder;
	CString findPathStr = pathStr+_T("\\*.*");
	BOOL bWorking = finder.FindFile(findPathStr);
	CString fileName = _T("");
	CString fullPathStr = _T("");

	while (bWorking) {
		bWorking = finder.FindNextFile();
		fileName = finder.GetFileName();
		if (fileName == _T(".") || fileName == _T("..")) continue;

		fullPathStr = pathStr + _T("\\") + fileName;
		if (IsDirectory(fullPathStr))
			RecurseRemoveLogs(fullPathStr,displayMsgFlag);
		else
			DeleteFile(fullPathStr);
	}
	finder.Close();

	if (EndsWith(pathStr,_T("_log")) && IsDirectory(pathStr)) {
		CString conceptPathStr = pathStr;
		ClipRight(conceptPathStr,4);

		// REMOVE FROM KB
		CONCEPT *concept = ConceptFromTextPath(conceptPathStr);
		cg->rmChildren(concept);

		// REMOVE FROM DOS
		if (!RecursiveRemove(pathStr,false,0,displayMsgFlag))
			return;

		// REMOVE FROM TEXT TAB
		TEXT_TREE_TYPE type = (TEXT_TREE_TYPE)KBGetValueLong(concept,_T("type"));
		CString pStr = pathStr;
		ClipRight(pStr,4);
		HTREEITEM item = TreeItemFromPath(pStr);
		if (type == TEXT_TREE_FOLDER)
			DeleteTreeItemRecursive(item);
		else
			DeleteTreeItemChild(item);
	}
	cg->setDirty(save);	// Kb status unaffected.	// 07/11/03 AM.
}

CONCEPT *CTreeViewText::ConceptFromTextPath(CString pathStr, bool fromGuiFlag)
{
	if (pathStr.IsEmpty())
		return NULL;

	CONCEPT *concept = m_conceptText;
	CONCEPT *con = m_conceptText;

	if (fromGuiFlag) {
		CONCEPT *sys = cg->findConcept(cg->findRoot(),_T("sys"));
		CONCEPT *gui = cg->findConcept(sys,_T("gui"));
		CONCEPT *text = cg->findConcept(gui,_T("spec"));
		concept = con = text;
	}

	CString relPathStr = RelativizePath(pathStr);
	CStringList conStrList;
	StringToStringList(conStrList,relPathStr,_T("\\"));
	POSITION pos = conStrList.GetHeadPosition();
	if (pos) {
		conStrList.GetNext(pos);  //  SKIP "input"
		CString conceptStr;
		_TCHAR nameBuff[1001];

		while (con && pos && (conceptStr = conStrList.GetNext(pos))) {
			if (conceptStr == _T(""))
				continue;
			if (EndsWith(conceptStr,_T(".nlp")) || EndsWith(conceptStr, _T(".pat")))
				ClipRight(conceptStr,4);
			StringToChar(nameBuff,conceptStr,1000);
			con = cg->findConcept(con,nameBuff);
		}
	}

	return con;
}

void CTreeViewText::Refresh(void)
{
	bool save = cg->getDirty();	// 07/11/03 AM.
	m_boolBlockComboAdd = true;
	cg->rmChildren(m_conceptText);
	FillInputTree();
	m_boolBlockComboAdd = false;
	cg->setDirty(save);	// Kb status unaffected.	// 07/11/03 AM.
}

void CTreeViewText::OnDirtreemenuRefresh() 
{
	CString msgStr = _T("Refreshing will cause all knowledge information associated with text files to be lost.\nDo you want to continue?");
	if (AfxMessageBox(msgStr,MB_YESNO) == IDYES)
		Refresh();
}

bool CTreeViewText::HasAnaLogFiles(CString filePathStr) 
{
	CString fullPathStr;
	fullPathStr = filePathStr + _T("_log\\ana001.log");
	if (FileExists(fullPathStr))
		return true;
	return false;
}

bool CTreeViewText::HasNextText()
{
	return m_treeItemSlog ? true : false;
}

void CTreeViewText::PrevText()
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	if (PrevTextFromPath(wnd->GetProcessText()))
		MoveToProcessedText();
}

void CTreeViewText::NextText()
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	if (NextTextFromPath(wnd->GetProcessText()))
		MoveToProcessedText();
}

void CTreeViewText::MoveToProcessedText(bool changeBrowser)
{
	StartWaitCursor();
	theApp.UpdateAnalyzerWindows(true,changeBrowser);
	StopWaitCursor();
}

bool CTreeViewText::PrevTextFromPath(CString currentPathStr)
{
	HTREEITEM currentItem = TreeItemFromPath(currentPathStr);
	HTREEITEM prevItem = m_treeCtrl.GetPrevSiblingItem(currentItem);
	
	if (prevItem) {
		m_treeCtrl.SelectItem(prevItem);
		return true;
	}
	else {
		HTREEITEM lastItem = TreeCtrlLastSibling(&m_treeCtrl,currentItem);
		if (lastItem) {
			m_treeCtrl.SelectItem(lastItem);
			return true;
		}
	}
	return false;
	
	/*
	bool found = false;
	CString debugStr;
	while (!found) {
		if (!prevItem && !found)
			prevItem = TreeCtrlLastSibling(this,currentItem);
		debugStr = m_treeCtrl.GetItemText(prevItem);

		if (IsChecked(prevItem)) {
			m_treeCtrl.SelectItem(prevItem);
			found = true;
			break;
		}

		prevItem = GetPrevSiblingItem(prevItem);
		if (prevItem == currentItem)
			break;
	}

	return found;
	*/
}

bool CTreeViewText::NextTextFromPath(CString currentPathStr)
{
	HTREEITEM currentItem = TreeItemFromPath(currentPathStr);
	HTREEITEM nextItem = m_treeCtrl.GetNextSiblingItem(currentItem);

	if (nextItem) {
		m_treeCtrl.SelectItem(nextItem);
		return true;
	}
	else {
		HTREEITEM firstItem = TreeCtrlFirstSibling(&m_treeCtrl,currentItem);
		if (firstItem) {
			m_treeCtrl.SelectItem(firstItem);
			return true;
		}
	}
	return false;

	/*
	bool found = false;
	CString debugStr;
	
	while (!found) {
		if (!nextItem && !found)
			nextItem = TreeCtrlFirstSibling(this,currentItem);
		debugStr = m_treeCtrl.GetItemText(nextItem);

		if (IsChecked(nextItem)) {
			m_treeCtrl.SelectItem(nextItem);
			found = true;
			break;
		}

		if (nextItem == currentItem)
			break;
		nextItem = m_treeCtrl.GetNextSiblingItem(nextItem);
	}

	return found;
	*/
}

void CTreeViewText::OnDirtreemenuMask() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString fileName = TreePath(m_itemSelected);

	if (m_textTreeType == TEXT_TREE_FOLDER) {
		CString msgStr;
		msgStr.Format(_T("Do you want to mask all the text files under \"%s\"?"),
			fileName);
		if (AfxMessageBox(msgStr,MB_YESNO) != IDYES)
			return;

		HTREEITEM item = m_treeCtrl.GetChildItem(m_itemSelected);
		int i = 0;
		CString itemTextStr;

		while (item) {
			itemTextStr = m_treeCtrl.GetItemText(item);
			theApp.MaskText(fileName + _T("\\") + itemTextStr);
			item = m_treeCtrl.GetNextSiblingItem(item);
		}
	}
	else {
		theApp.MaskText(fileName);
	}

}

bool CTreeViewText::OpenProcessedFiles(CString extStr, bool openFiles, bool excludeOutputTxtFlag)
{
	if (appState == VT_APP_LOADING)
		return false;

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString fullPathStr = ProcessedFileStr(extStr);
	if (fullPathStr == _T(""))
		return false;

	CFileFind find;
	bool found = find.FindFile(fullPathStr) ? true : false;

	if (!openFiles && !excludeOutputTxtFlag)
		return found;
	found = false;

	CString fileName = _T("");
	CStringList dumps;
	ProcessedFiles(dumps,extStr);
	POSITION pos = dumps.GetHeadPosition();
	CString dumpFileStr = _T("");

	while (pos && (dumpFileStr = dumps.GetNext(pos))) {
		if (!excludeOutputTxtFlag || !EndsWith(dumpFileStr, _T("output.txt"))) {
			if (openFiles)
				AfxGetApp()->OpenDocumentFile(dumpFileStr);
			found = true;
		}
	}

	return found;
}

CString CTreeViewText::ProcessedFileStr(CString extStr, CString processPathStr)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	if (processPathStr == _T(""))
		processPathStr = wnd->GetProcessTextFile();
	if (processPathStr == _T(""))
		return _T("");

	CString fileStr = _T("\\");
	if (extStr == _T(""))
		fileStr += _T("*.txt");
	else if (extStr.Mid(0,1) = _T("."))
		fileStr += _T("*") + extStr;
	else if (extStr.Find(_T("."),0) < 0)
		fileStr += _T("*") + extStr;
	else
		fileStr += extStr;

	return processPathStr+fileStr;
}

void CTreeViewText::ProcessedFiles(CStringList &processedFiles, CString extStr, CString processPathStr)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	if (processPathStr == _T(""))
		processPathStr = wnd->GetProcessTextFile();
	CString fileStr = _T("\\");
	if (extStr == _T(""))
		fileStr += _T("*.txt");
	else if (extStr.Mid(0,1) = _T("."))
		fileStr += _T("*") + extStr;
	else if (extStr.Find(_T("."),0) < 0)
		fileStr += _T("*.") + extStr;
	else
		fileStr += extStr;

	CFileFind finder;
	BOOL bWorking = finder.FindFile(processPathStr+fileStr);
	bool found = bWorking ? true : false;

	CString fileName = _T("");
	CString fullPathStr = _T("");

	while (bWorking) {
		bWorking = finder.FindNextFile();
		fileName = finder.GetFileName();
		if (fileName == _T(".") || fileName == _T(".."))
			continue;

		fullPathStr = finder.GetFilePath();
		processedFiles.AddTail(fullPathStr);
	}
	finder.Close();
}

void CTreeViewText::OnDirtreemenuDictionize() 
{
	theApp.Dictionize(CurrentTreeSelection(true));
	Refresh();
}

void CTreeViewText::RunDictionaryLookupOnSelection(LPCTSTR lpURL, LPCTSTR lpName)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString fileName = TreePath(m_itemSelected);

	if (m_textTreeType == TEXT_TREE_FOLDER) {
		CString msgStr;
		msgStr.Format(_T("Do you want to run Dictionary Lookup for all the text files under \"%s\"?"),
			fileName);
		if (AfxMessageBox(msgStr,MB_YESNO) != IDYES)
			return;
	}

	wnd->ClearOutput();
	DictionaryLookupRecurse(m_itemSelected, lpURL, lpName);
	Refresh();

	/*
	if (m_textTreeType == TEXT_TREE_FOLDER) {
		CString msgStr;
		msgStr.Format("Do you want to run Dictionary Lookup for all the text files under \"%s\"?",
			fileName);
		if (AfxMessageBox(msgStr,MB_YESNO) != IDYES)
			return;

		HTREEITEM item = m_treeCtrl.GetChildItem(m_itemSelected);
		int i = 0;
		CString itemTextStr;

		while (item) {
			wnd->ClearOutput();
			itemTextStr = m_treeCtrl.GetItemText(item);
			//SelectTextFile(itemTextStr);
			theApp.RunWordNet(fileName + "\\" + itemTextStr);
			item = m_treeCtrl.GetNextSiblingItem(item);
		}
	}
	else
		theApp.RunWordNet(fileName);
	*/
}

void CTreeViewText::OnDirtreemenuToolsWordnet() 
{
	RunDictionaryLookupOnSelection();
}

bool CTreeViewText::DictionaryLookupRecurse(HTREEITEM item, LPCTSTR lpURL, LPCTSTR lpName) 
{
	TEXT_TREE_TYPE treeType = TextTreeType(item);
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString fileName = TreePath(item);
	CString itemStr;

	if (treeType == TEXT_TREE_FOLDER) {
		CString msgStr;

		HTREEITEM childItem = m_treeCtrl.GetChildItem(item);
		TEXT_TREE_TYPE childType;
		itemStr = m_treeCtrl.GetItemText(item);
		if (EndsWith(itemStr,_T("_wn")))
			return true;
			
		int i = 0;
		CString itemTextStr;

		while (childItem) {
			childType = TextTreeType(childItem);
			if (childType == TEXT_TREE_FOLDER) {
				itemStr = m_treeCtrl.GetItemText(item);
				if (EndsWith(itemStr,_T("_wn")))
					return true;
				if (!DictionaryLookupRecurse(childItem, lpURL, lpName))
					return false;
			}
			wnd->ClearOutput();
			itemTextStr = m_treeCtrl.GetItemText(childItem);
			if (!theApp.RunDictionaryLookup(fileName + _T("\\") + itemTextStr), lpURL, lpName)
				return false;
			childItem = m_treeCtrl.GetNextSiblingItem(childItem);
		}
	}
	else
		return theApp.RunDictionaryLookup(fileName, lpURL, lpName);

	return true;
}

void CTreeViewText::OnDirtreemenuTextfileinfo() 
{
	CTextInfoDlg dlg;
	CString fullPathStr = CurrentTreeSelection(true);
	dlg.SetData(fullPathStr);
	dlg.DoModal();
}

void CTreeViewText::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	/*
	HTREEITEM item = pNMTreeView->itemNew.hItem;
	CONCEPT *concept = (CONCEPT *)m_treeCtrl.GetItemData(item);
	long building = KBGetValueLong(concept,"building");

	if (pNMTreeView->action == 2 && building < 0) {
		StartWaitCursor();
		FillTreeExpand(item);
		StopWaitCursor();
	}
	*/

	*pResult = 0;
}

void CTreeViewText::OnDirtreemenuToolsHexviewer() 
{
	CString selectedFilePathStr = CurrentTreeSelection(true);
	CHexEditDlg dlg;
	//dlg.SetFileData(selectedFilePathStr);
	dlg.DoModal();
}

void CTreeViewText::OnDirtreemenuToolsAsciitable() 
{
	CASCIIDlg dlg;
	dlg.DoModal();
}

void CTreeViewText::OnDirtreemenuToolsCharacterviewer() 
{
	CString filePathStr = CurrentTreeSelection(true);
	CFileCharViewerDlg dlg;
	dlg.SetData(filePathStr);
	dlg.DoModal();
}

void CTreeViewText::OnDirtreemenuToolsEmail() 
{
	CString filePathStr = CurrentTreeSelection(true);
	CEmailDlg dlg;
	dlg.SetFileData(filePathStr);
	dlg.DoModal();
}

void CTreeViewText::OnDirtreemenuAddToplevelExistingtextfile() 
{
	AddExistingTextFile(true);
}

void CTreeViewText::OnDirtreemenuAddToplevelFolder() 
{
	AddFolder(true);
}

void CTreeViewText::OnDirtreemenuAddToplevelNewtextfile() 
{
	AddNewText(true);
}

void CTreeViewText::SetPageModeState(CString stateStr, bool appendFlag)
{
	if (m_conceptFolder) {
		//CString newStateStr = GetItemState(m_itemToAddTo);

		CString newStateStr = KBGetValueString(m_conceptFolder,_T("state"));

		if (!appendFlag)
			newStateStr = _T("");
		else if (newStateStr != _T(""))
			newStateStr += _T("\r");
		newStateStr += stateStr;

		KBReplaceValue(m_conceptFolder,_T("state"),newStateStr,false);

		//SetItemState(m_itemToAddTo,newStateStr);
	}
}

CString CTreeViewText::GetPageModeState()
{
	CString stateStr = _T("");
	if (m_itemToAddTo) {
		stateStr = GetItemState(m_itemToAddTo);
	}
	return stateStr;
}

CString CTreeViewText::GetItemState(HTREEITEM item)
{
	CString dirPathStr = TreePath(item);
	POSITION pos = m_stringListStates.GetHeadPosition();
	CString stateStr = _T("");
	CString returnStr = _T("");
	CString dirStr;

	while (pos && (stateStr = m_stringListStates.GetNext(pos))) {
		NthPathSegment(stateStr,dirStr,'\t',0);
		if (dirStr == dirPathStr) {
			returnStr = StripPathReverse(stateStr,0,true,'\t');
			break;
		}
	}
	return returnStr;
}

void CTreeViewText::SetItemState(HTREEITEM item, CString newStateStr)
{
	CString dirPathStr = TreePath(item);
	POSITION pos = m_stringListStates.GetHeadPosition();
	POSITION lastPos = pos;
	CString stateStr = _T("");
	CString setStr = _T("");
	CString dirStr;
	bool found = false;

	while (pos && (stateStr = m_stringListStates.GetNext(pos))) {
		NthPathSegment(stateStr,dirStr,'\t',0);
		if (dirStr == dirPathStr) {
			setStr = dirPathStr + _T("\t") + newStateStr;
			m_stringListStates.SetAt(lastPos,setStr);
			found = true;
			break;
		}
		lastPos = pos;
	}
	if (!found) {
		setStr = dirPathStr + _T("\t") + newStateStr;
		m_stringListStates.AddTail(setStr);
	}
}

CString CTreeViewText::GetPageModeStateStr()
{
	POSITION pos = m_stringListStates.GetHeadPosition();
	CString stateStr = _T("");
	CString sStr,numStr,valueStr;

	while (pos && (sStr = m_stringListStates.GetNext(pos))) {
		valueStr = StripPathReverse(sStr,0,true,'\t');
		if (valueStr != _T("")) {
			if (stateStr != _T(""))
				stateStr += _T("<<SEP>>");
			stateStr += sStr;
		}
	}
	return stateStr;
}

void CTreeViewText::RestorePageModeState(CString stateStr, bool clearFirst)
{
	if (clearFirst)
		m_stringListStates.RemoveAll();

	CString valueStr = StripPathReverse(stateStr,0,true,'\t');
	if (valueStr != _T(""))
		m_stringListStates.AddTail(stateStr);
}

void CTreeViewText::OnDirtreemenuAttributes() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->OpenAttributeEditor(m_conceptSelected, false);
}

void CTreeViewText::OnUpdateDirtreemenuAttributes(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_treeCtrl.GetCount() > 0);	
}

int CTreeViewText::TreeItemCount() 
{
	return m_treeCtrl.GetCount();
}

bool CTreeViewText::IsSelectedEmptyFolder()
{
	return m_boolFolderEmpty;
}

void CTreeViewText::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CPaneView::OnKeyDown(nChar, nRepCnt, nFlags);
	if (VK_DELETE == nChar) {
		if (m_itemSelected)
			PostMessage(WM_COMMAND, ID_DIRTREEMENU_DELETE);
	}
}

void CTreeViewText::OnDirtreemenuToolsJumble() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString fileName = TreePath(m_itemSelected);

	if (m_textTreeType == TEXT_TREE_FOLDER) {
		CString msgStr;
		msgStr.Format(_T("Do you want to jumble all the text files under \"%s\"?"),
			fileName);
		if (AfxMessageBox(msgStr,MB_YESNO) != IDYES)
			return;

		HTREEITEM item = m_treeCtrl.GetChildItem(m_itemSelected);
		int i = 0;
		CString itemTextStr;

		while (item) {
			itemTextStr = m_treeCtrl.GetItemText(item);
			theApp.JumbleText(fileName + _T("\\") + itemTextStr);
			item = m_treeCtrl.GetNextSiblingItem(item);
		}
	}
	else {
		theApp.JumbleText(fileName);
	}
}

void CTreeViewText::AddWordsFromFile(LPCTSTR lpPath, int level)
{
	CString strPath = lpPath;
	if (IsDirectory(strPath)) {
		CFileFind finder;
		strPath += _T("\\*.*");
		BOOL bFound = finder.FindFile(strPath);
		while (bFound) {
			bFound = finder.FindNextFile();
			if (finder.IsDots())
				continue;
			if (finder.IsDirectory() && EndsWith(finder.GetFilePath(),_T("_log")))
				continue;
			AddWordsFromFile(finder.GetFilePath(),level + 1);
		}
		finder.Close();
	}
	else {
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		CFile obFile;
		if (obFile.Open(strPath, CFile::modeReadWrite, NULL)) {
			DWORD fileLen = (DWORD)obFile.GetLength();
			HGLOBAL h;
			LPTSTR lpStart;
			if (h = GlobalAlloc (GHND, fileLen + 1)) {
				if (lpStart = (LPTSTR)GlobalLock (h)) {
					if (obFile.Read (lpStart, fileLen) == fileLen) {
						CString strText = lpStart;
						strText.Replace(_T("\r\n"),_T("\n"));
						// Ensure CR terminates the string in order to
						// pick up the last word in while loop below
						if (strText.GetAt(strText.GetLength()-1) != '\n')
							strText += '\n';

						CString strDebug;
						int nLine = 1;
						int nTotal = 0;
						CString strFileName = IndentStr(level*3,0) + StripPath(strPath,level,false);
						int posEnd, posStart = 0;
						while (-1 != (posEnd = strText.Find(_T("\n"), posStart)) ) {
							CString strWord = strText.Mid(posStart, posEnd - posStart);
							strWord.TrimLeft();
							int pos = strWord.FindOneOf(_T("\r\n\t "));
							if (pos != -1)
								strWord = strWord.Left(pos);
							if (!strWord.IsEmpty() && cg->addWord((LPTSTR)(LPCTSTR)strWord)) {
								nTotal++;
								m_bRedrawKB = true;
								strDebug.Format(_T("%s (Line %d)"), strFileName, nLine);
								wnd->AddDebugLine(strDebug, strWord, true);
							}
							posStart = posEnd + 1;
							nLine++;
						}
						if (nTotal > 0) {
							strDebug.Format(_T("Total words added: %d"), nTotal);
							wnd->AddDebugLine(strFileName, strDebug, true);
						}
					}
					GlobalUnlock (h);
				}
				GlobalFree (h);
			}
			obFile.Close();
		}
	}
}

void CTreeViewText::OnDirtreemenuDictionaryAddwords() 
{
	if (m_itemSelected) {
		CString strPath = InputPath(TreePath(m_itemSelected));
		if (IsDirectory(strPath)) {
			CString msgStr;
			msgStr.Format(_T("%s\n\n\tis a directory.")
				_T("\n\nIf you continue, you will recursively add")
				_T("\nall words in all files in all sub-directories.")
				_T("\n\nContinue?"), strPath);
			if (IDNO == AfxMessageBox(msgStr, MB_YESNO))
				return;
		}
		m_bRedrawKB = false;
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		CString strDebug = IsDirectory(strPath) ? _T("Directory: ") : _T("File: ");
		strDebug += TreePath(m_itemSelected);
		wnd->AddDebugLine(_T(""), _T(""), true); // Add a blank line for ease of reading
		wnd->AddDebugLine(_T("Add Words to Dictionary:"), strDebug, true);
		AddWordsFromFile(strPath, 0);
		if (m_bRedrawKB) {
			CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
			if (pView)
				pView->Restore();
		}
	}
}

void CTreeViewText::OnDictionaryURLLookup(UINT uID)
{
	UINT idx = uID - ID_DICTIONARY_URL_LOOKUP_BASE;
	RunDictionaryLookupOnSelection(m_strURLs[idx], m_strNames[idx]);
}

bool CTreeViewText::CanMoveSelectedDown()
{
	bool returnVal = false;
	if (m_conceptSelected)
		returnVal = cg->Next(m_conceptSelected) ? true : false;
	return returnVal;
}

bool CTreeViewText::CanMoveSelectedUp()
{
	bool returnVal = false;
	if (m_conceptSelected)
		returnVal = cg->Prev(m_conceptSelected) ? true : false;
	return returnVal;
}

void CTreeViewText::MoveSelDown()
{
	NextText();
}

void CTreeViewText::MoveSelUp()
{
	PrevText();
}

void CTreeViewText::OnAddExistingfolder()
{
	CXTBrowseDialog dlg;
	if (dlg.DoModal() == IDOK) {
		CString str = dlg.GetSelPath();
		AddExistingFolder(str);
	}
}

void CTreeViewText::OnDirtreemenuExploredirectory()
{
	CString fromStr = InputPath(TreePath());
	ShellExecute(0, _T("open"), fromStr, NULL, NULL, SW_NORMAL);
}

void CTreeViewText::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CPaneView::OnRButtonDown(nFlags, point);
}