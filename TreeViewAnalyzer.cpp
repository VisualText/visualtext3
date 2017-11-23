/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// TreeViewAnalyzer.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "resource.h"

#include "FTPClient.h"
#include "MainFrm.h"
#include "CustomMsgDlg.h"
#include "VisualTextDoc.h"
#include "VisualTextView.h"
#include "SyntaxEditorDoc.h"
#include "SyntaxEditorView.h"
#include "PassPropertiesDlg.h"
#include "AttributeEditor.h"
#include "TreeViewAnalyzer.h"
#include "TreeViewGrammar.h"
#include "TreeViewText.h"
#include "KnowledgeView.h"

extern CString appAnaFilePathStr;
extern CString appNameStr;
extern CString appDirectoryStr;
extern CString appAskText;
extern CString appRunFilePathStr;
extern CVisualTextApp theApp;
extern VT_APP_STATE appState;
extern bool appBoolAnalyzerLoaded;
extern bool appBoolHighlightText;
extern bool appBoolLogAnalyzer;
extern bool appBoolVerbose;
extern bool appBoolPageMode;
extern bool appBoolBatchMode;
extern bool appBoolCompile;
extern CONCEPT *appGUIConcept;
extern CG *cg;
extern NLP *nlp;
extern bool kbDirty;	// 06/2/03 AM.
extern int appButtonReturn;

//bool m_OkAnalysis;	// 06/19/03 AM.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ANALSEP '\\'

/////////////////////////////////////////////////////////////////////////////
// CTreeViewAnalyzer

#define ID_TIMER_DRAG 100

#define WMU_ANALYZE_OUTPUT_MESSAGE	(WM_USER + 1500)
#define WMU_ANALYZE_OUTPUT_PASS_MESSAGE	(WM_USER + 1501)
#define WMU_POST_ANALYZE_FILE_TASKS	(WM_USER + 1502)
#define WMU_ANALYZE_CLEANUP			(WM_USER + 1503)
#define WMU_POST_ANALYZE_HIGHLIGHT	(WM_USER + 1504)

void ANLZ_GetStrTime(CString& strTime, DWORD ms);

// IF CHANGE HERE, CHANGE IN:  TreeViewAnalyzer.h  // 03/07/02 AM.

_TCHAR *AlgoStrings[] = {
	_T("none"),
	_T("pat"),
	_T("system"),
	_T("stub"),
	_T("end"),
	_T("rec"),
	_T("auto"),
	_T("user"),
	_T("group"),
	_T("main"),
	_T("folder"),
	_T("xxxx")
};

CTreeViewAnalyzer::CTreeViewAnalyzer()
{
	m_nTimerIDDrag = 0;

	m_AnalyzeThreadID = 0;
	m_AnalyzeThread = NULL;
	m_fTerminateAnalyzeThread = false;
	m_fSuspendAnalyzeThread = false;

	m_nToolBarID = IDR_ANALYZER_TOOLBAR;

	InitVars();
}

CTreeViewAnalyzer::~CTreeViewAnalyzer()
{
	KillTimers();
	EndAnalyzeThread();
}

/*
void CTreeViewAnalyzer::OnContextMenu(CWnd* pWnd, CPoint ptMousePos) 
{
	// if Shift-F10
	if (ptMousePos.x == -1 && ptMousePos.y == -1)
		ptMousePos = (CPoint) GetMessagePos();

	ScreenToClient(&ptMousePos);

	UINT uFlags;
	HTREEITEM htItem;
	
	htItem = HitTest( ptMousePos, &uFlags );

	if( htItem == NULL )
		return;
	
	m_hActiveItem = htItem;

	CMenu menu;
	CMenu* pPopup;

	// the font popup is stored in a resource
	menu.LoadMenu(IDR_ANALYZER);
	pPopup = menu.GetSubMenu(0);
	ClientToScreen(&ptMousePos);
	pPopup->TrackPopupMenu( TPM_LEFTALIGN, ptMousePos.x, ptMousePos.y, this );
}
*/

void CTreeViewAnalyzer::KillTimers(void)
{
	if (m_nTimerIDDrag) {
		KillTimer(m_nTimerIDDrag);
		m_nTimerIDDrag = 0;
	}
}

void CTreeViewAnalyzer::InitVars()
{
	nlp = NULL;

	m_noDropCursor = AfxGetApp()->LoadStandardCursor(IDC_NO);
	m_dropCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

	m_conceptSpec = NULL;

	ResetVars();
}

void CTreeViewAnalyzer::ResetVars()
{
	m_bFillingTree = false;
	m_dirtyFileFlag = false;
	m_dirtyFlag = false;
	m_boolInStub = false;
	m_intPass = 0;		// ONE-NUMBERED
	m_intConcept = 0;	// ONE-NUMBERED

	m_strStatus = _T("");
	m_strAlgo = _T("");
	m_strData = _T("");
	m_strComments = _T("");

	m_itemSelected = NULL;
	m_itemParent = NULL;
	m_hitemDrop = NULL;
	m_hitemDrag = NULL;
	m_destinationType = MOVE_NOWHERE;

	m_conceptSelected = NULL;

	m_passType = ANAL_PASS_NONE;

	m_bLDragging = false;

	m_strLastFolderAddPassFromFile.Empty();
	KillTimers();
	EndAnalyzeThread();
}

void CTreeViewAnalyzer::DoDataExchange(CDataExchange* pDX)
{
	CPaneView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTreeCtrlDlg)
	DDX_Control(pDX, IDC_ANALYZER_TREE_SELECT, m_treeCtrl);
	//}}AFX_DATA_MAP
}

IMPLEMENT_DYNCREATE(CTreeViewAnalyzer, CPaneView)

BEGIN_MESSAGE_MAP(CTreeViewAnalyzer, CPaneView)
	//{{AFX_MSG_MAP(CTreeViewAnalyzer)
	ON_WM_CREATE()
	//ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_COMMAND(ID_SEQUENCEMENU_ADDEXISTINGRULES, OnSequencemenuAddexistingrules)
	ON_COMMAND(ID_SEQUENCEMENU_DELETE, OnSequencemenuDelete)
	ON_COMMAND(ID_SEQUENCEMENU_DELETEPASSES, OnSequencemenuDeletepasses)
	ON_COMMAND(ID_SEQUENCEMENU_ATTRIBUTES, OnSequencemenuAttributes)
	ON_COMMAND(ID_SEQUENCEMENU_DUPLICATE, OnSequencemenuDuplicate)
	ON_COMMAND(ID_SEQUENCEMENU_NEW, OnSequencemenuNew)
	ON_COMMAND(ID_SEQUENCEMENU_EDITRULES, OnSequencemenuEditrules)
	ON_COMMAND(ID_SEQUENCEMENU_EDITVIEW, OnSequencemenuEditview)
	ON_COMMAND(ID_SEQUENCEMENU_VIEWFULLTREE, OnSequencemenuViewfulltree)
	ON_COMMAND(ID_SEQUENCEMENU_HELP, OnSequencemenuHelp)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_COMMAND(ID_SEQUENCEMENU_VIEWLOG, OnSequencemenuViewlog)
	ON_COMMAND(ID_SEQUENCEMENU_SERVERARCHIVES, OnSequencemenuServerarchives)
	ON_COMMAND(ID_SEQUENCEMENU_UPLOADRULES, OnSequencemenuUploadrules)
	ON_COMMAND(ID_SEQUENCEMENU_ARCHIVERULES, OnSequencemenuArchiverules)
	ON_COMMAND(ID_SEQUENCEMENU_LOCALARCHIVES, OnSequencemenuLocalarchives)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCEMENU_DELETE, OnUpdateSequencemenuDelete)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCEMENU_EDITRULES, OnUpdateSequencemenuEditrules)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCEMENU_VIEWLOG, OnUpdateSequencemenuViewlog)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCEMENU_VIEWFULLTREE, OnUpdateSequencemenuViewfulltree)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCEMENU_ARCHIVERULES, OnUpdateSequencemenuArchiverules)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCEMENU_UPLOADRULES, OnUpdateSequencemenuUploadrules)
	ON_COMMAND(ID_SEQUENCEMENU_HIGHLIGHTMATCHES, OnSequencemenuHighlightmatches)
	ON_COMMAND(ID_SEQUENCEMENU_VIEWENTIRETREE, OnSequencemenuViewentiretree)
	ON_COMMAND(ID_SEQUENCEMENU_PROPERTIES, OnSequencemenuProperties)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_COMMAND(ID_SEQUENCEMENU_VIEWTREEMATCHES, OnSequencemenuViewtreematches)
	ON_COMMAND(ID_SEQUENCEMENU_VIEWCONCEPTTREE, OnSequencemenuViewconcepttree)
	ON_COMMAND(ID_SEQUENCEMENU_ADD_FOLDER, OnSequencemenuAddFolder)
	ON_COMMAND(ID_SEQUENCEMENU_ADD_MAIN, OnSequencemenuAddMain)
	ON_COMMAND(ID_SEQUENCEMENU_ADD_TOPLEVEL_EXISTINGPASS, OnSequencemenuAddToplevelExistingpass)
	ON_COMMAND(ID_SEQUENCEMENU_ADD_TOPLEVEL_MAIN, OnSequencemenuAddToplevelMain)
	ON_COMMAND(ID_SEQUENCEMENU_ADD_TOPLEVEL_NEW, OnSequencemenuAddToplevelNew)
	ON_COMMAND(ID_SEQUENCEMENU_ADD_TOPLEVEL_FOLDER, OnSequencemenuAddToplevelFolder)
	ON_COMMAND(ID_SEQUENCEMENU_ADD_STUB, OnSequencemenuAddStub)
	ON_WM_KEYDOWN()

	//}}AFX_MSG_MAP
	ON_MESSAGE(WMU_ANALYZE_OUTPUT_MESSAGE, OnAnalyzeOutputMessage)
	ON_MESSAGE(WMU_ANALYZE_OUTPUT_PASS_MESSAGE, OnAnalyzeOutputPassMessage)
	ON_MESSAGE(WMU_POST_ANALYZE_FILE_TASKS, OnPostAnalyzeFileTasks)
	ON_MESSAGE(WMU_ANALYZE_CLEANUP, OnAnalyzeCleanup)
	ON_MESSAGE(WMU_POST_ANALYZE_HIGHLIGHT, OnAnalyzeHighlight)
	ON_COMMAND_RANGE(SPEC_STARTID, SPEC_ENDID, OnChooseSpec)
	ON_COMMAND_RANGE(SPEC_2_STARTID, SPEC_2_ENDID, OnChooseSpec2)
	ON_NOTIFY(NM_RCLICK, AFX_IDW_PANE_FIRST, OnRButtonDown)
	ON_NOTIFY(NM_DBLCLK, AFX_IDW_PANE_FIRST, OnDblclk)
	ON_NOTIFY(TVN_ITEMEXPANDING, AFX_IDW_PANE_FIRST, OnItemexpanding)
	ON_NOTIFY(TVN_SELCHANGED, AFX_IDW_PANE_FIRST, OnSelchanged)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_COMMAND(ID_TREE_UP, OnMoveUp)
	ON_COMMAND(ID_TREE_DOWN, OnMoveDown)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeViewAnalyzer diagnostics

#ifdef _DEBUG
void CTreeViewAnalyzer::AssertValid() const
{
	CPaneView::AssertValid();
}

void CTreeViewAnalyzer::Dump(CDumpContext& dc) const
{
	CPaneView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTreeViewAnalyzer message handlers

void CTreeViewAnalyzer::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

CWnd* CTreeViewAnalyzer::OnCreateView() 
{
	if (!m_treeCtrl.Create( WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
		CRect(0,0,0,0), this, AFX_IDW_PANE_FIRST ))
	{
		TRACE0( "Unable to create tree control.\n" );
		return NULL;
	}
	
	m_ImageList.Create (IDB_ANALYZER, 16, 1, RGB(0,255,0));
	m_treeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);

	/*
	CMenu m_ShortMenu;
	m_ShortMenu.LoadMenu(IDR_ANALYZER);
	ASSERT(m_ShortMenu);

	m_treeCtrl.SetMenu(&m_ShortMenu);
	*/

	return &m_treeCtrl;
}

void CTreeViewAnalyzer::LoadKBAnalyzer(CONCEPT *concept, HTREEITEM parentItem)
{
	SetRedraw(false);

	if (!concept) {
		concept = cg->Down(m_conceptSpec);
		m_treeCtrl.DeleteAllItems();
		parentItem = NULL;
	}

	HTREEITEM newItem;
	CString nameStr;
	int image,imageSelected;
	CONCEPT *childConcept;
	ANAL_PASS_TYPE type
	   = ANAL_PASS_NONE;	// 02/21/02 AM.
	int passNum = 1;
	int kbPassNum, kbActiveFlag;
	long typeLong=0;		// 03/07/02 AM.

	while (concept) {
		nameStr = DisplayPassName(concept);

		kbPassNum = KBPassNumber(concept);
		if (kbPassNum < 0)
			KBReplaceValue(concept,_T("pass"),passNum++,false);
		else
			passNum = kbPassNum;

		kbActiveFlag = KBGetValueLong(concept,_T("active"));
		m_boolActive = kbActiveFlag ? true : false;
		type = ANAL_PASS_NONE;	// 03/07/02 AM.
		ImageFromConcept(concept,image,imageSelected,type);
		newItem = m_treeCtrl.InsertItem(nameStr,image,imageSelected,parentItem,TVI_LAST);
		m_treeCtrl.SetItemData(newItem,(DWORD)concept);

		// RETRO FIT FOR FORGETTING TO PUT IN THE "TYPE" ATTRIBUTE ON THE CONCEPT
		// Type is long, not string.	// 02/21/02 AM.
		typeLong = 0;
		KBFindValue(concept, _T("type"), typeLong);	// 02/21/02 AM.
		if (typeLong <= 0)	// 03/07/02 AM.
			KBReplaceValue(concept, _T("type"), type, false);	// 02/21/02 AM.
//		CString typeStr;
//		KBFindValue(concept,"type",typeStr);
//		if (typeStr == "")
//			KBReplaceValue(concept,"type",type,false);

		childConcept = cg->Down(concept);
		if (childConcept)
			LoadKBAnalyzer(childConcept,newItem);
		concept = cg->Next(concept);
	}

	SetRedraw(true);
	m_treeCtrl.InvalidateRect(NULL);
	m_treeCtrl.UpdateWindow();
}

void CTreeViewAnalyzer::Load()
{
	InitAnalyzer();
	FillListWithSequence();
}

void CTreeViewAnalyzer::Refresh()
{
	bool save = cg->getDirty();	// 07/11/03 AM.
	cg->rmChildren(m_conceptSpec);
//	bool saveKBBased = m_boolKBBased;	// 06/26/03 AM.
	m_boolKBBased = false;
	FillListWithSequence();
//	m_boolKBBased = saveKBBased;	// 06/26/03 AM.
	cg->setDirty(save);	// Kb status unaffected.	// 07/11/03 AM.
}

BOOL CTreeViewAnalyzer::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	//cs.style |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	cs.style |= TVS_HASBUTTONS | TVS_EDITLABELS | TVS_LINESATROOT;
	return CPaneView::PreCreateWindow(cs);
}

void CTreeViewAnalyzer::FillListWithSequence()
{
	if (appAnaFilePathStr == _T(""))
		return;

	if (m_boolKBBased) {
		LoadKBAnalyzer();
		return;
	}

	//return;  // ddxxx
	//SetRedraw(false);  ddxxx
	
	if (m_treeCtrl.GetCount())
		m_treeCtrl.DeleteAllItems();

	APASS *pass = Aseq::firstPass(nlp);

	_TCHAR algo[1001];
	_TCHAR data[1001];
	_TCHAR comment[1001];
	CString statusStr;
	CString algoStr;
	CString dataStr;
	CString commentStr;
	CString passStr;
	CString nameStr;
	bool active;
	int passNumber = 1;
	CMainFrame *mainFrm = (CMainFrame *)AfxGetMainWnd();
	int image;
	int imageSelected;
	int itemCount = 0;
	HTREEITEM item;
	HTREEITEM parentItem = NULL;
	CONCEPT *passConcept;
	_TCHAR passBuff[1001];
	ANAL_PASS_TYPE type;

	// VARS TO MAKE FLAT FOLDER INFO INTO A TREE STRUCTURE
	int folders[100];
	int folderLevel = 0;
	bool downLevel = false;
	int i = 0;

	for (i=0; i<100; i++)
		folders[i] = 0;

	while (pass) {
		Aseq::unpackPass(pass, algo, data, comment, active);
		statusStr = _T("");
		if (!active)
			statusStr = _T("off");
		algoStr = algo;
		dataStr = data;
		commentStr = comment;
		commentStr.TrimLeft();
		commentStr.TrimRight();

		passStr = PassName(algoStr,dataStr);
		ImageType(algoStr,image,imageSelected,type);
		if (!active)	// 08/19/03 AM.
			{
			image = 14;	// 08/19/03 AM.
			imageSelected = 14;	// 08/19/03 AM.
			}

		CString rulesPathStr = RulesPath(passStr);
		if (algoStr == _T("pat") && !FileExists(rulesPathStr))
			mainFrm->AddAnalyzerLine(_T("ERROR>>> no analyzer file"),passStr);

		// ADD CONCEPT TO KB
		StringToChar(passBuff,passStr,1000);
		passConcept = cg->makeConcept(m_conceptSpec,passBuff,0);
//		kbDirty = true;
		KBReplaceValue(passConcept,_T("algo"),algoStr,false);
		KBReplaceValue(passConcept,_T("data"),dataStr,false);
		KBReplaceValue(passConcept,_T("comment"),commentStr,false);
		KBReplaceValue(passConcept,_T("active"),active,false);
		KBReplaceValue(passConcept,_T("type"),type,false);
		//if (type != ANAL_PASS_FOLDER && KBGetValueLong(passConcept,"pass") < 1)
			KBReplaceValue(passConcept,_T("pass"),passNumber,false);

		// ADD ITEM TO DISPLAY TREE
		if (type == ANAL_PASS_FOLDER && IsDigit(commentStr)) {
			itemCount = _ttoi(commentStr);
			KBReplaceValue(passConcept,_T("itemcount"),itemCount,false);
			if (itemCount > 0) {
				folders[folderLevel++] = itemCount;
				downLevel = true;
			}
		}
		//else
			passNumber++;

		nameStr = DisplayPassName(passConcept);
		item = m_treeCtrl.InsertItem(nameStr,image,imageSelected,parentItem,TVI_LAST);
		m_treeCtrl.SetItemData(item,(DWORD)passConcept);

		// TAKE CARE OF FOLDER STUFF
		if (folderLevel) {
			// IF NEW FOLDER, THEN CREATE CREATE A CHILD LEVEL
			int checkLevel = folderLevel;
			if (downLevel) {
				parentItem = item;
				checkLevel--;
			}

			// PROCESS FOLDER COUNTERS
			for (i=0; i<checkLevel; i++) {
				if (--folders[i] == 0) {
					folderLevel--;
					parentItem = m_treeCtrl.GetParentItem(parentItem);
				}
			}

			downLevel = false;
		}

		pass = Aseq::nextPass(pass);
	}

	//Expand(htItem, TVE_EXPAND);

	//SetRedraw(true);
}

CString CTreeViewAnalyzer::PassName(CString algoStr, CString dataStr)
{
	ANAL_PASS_TYPE type = PassType(algoStr);

	CString passStr = dataStr;
	if (type == ANAL_PASS_STUB || type == ANAL_PASS_END)
		passStr = algoStr + _T("_") + dataStr;
	else if (type != ANAL_PASS_PAT && type != ANAL_PASS_REC && type != ANAL_PASS_FOLDER)
		passStr = algoStr;

	return passStr;
}

CString CTreeViewAnalyzer::DisplayPassName(CONCEPT *concept)
{
	CString nameStr = ConceptName(concept);
	int pass = KBPassNumber(concept);
	ANAL_PASS_TYPE type = ConceptType(concept);

	//if (type != ANAL_PASS_FOLDER && type != ANAL_PASS_MAIN)
		nameStr.Format(_T("%d %s"),pass,ConceptName(concept));

	return nameStr;
}

ANAL_PASS_TYPE CTreeViewAnalyzer::ConceptType(CONCEPT *concept)
{
	return (ANAL_PASS_TYPE)KBGetValueLong(concept,_T("type"));
}

void CTreeViewAnalyzer::InitAnalyzer()
{
	logmsg(_T("Enter InitAnalyzer:"));	// 07/24/03 AM.
	if (!nlp) {
		AfxGetApp()->BeginWaitCursor();

		_TCHAR appDir[1001];
		_tcscpy_s(appDir,(const _TCHAR *)AppPath());
		_TCHAR analDir[1001];
		_tcscpy_s(analDir,(const _TCHAR *)AnalyzerSequencePath());

		if (FileExists(AnalyzerSequencePath())) {
			if (nlp) {
				logmsg(_T("  Nlp nonempty."));	// 07/24/03 AM.
//				Aseq::writeFile(nlp);	// QUERY USER ONLY.	// 06/26/03 AM.
				nlp->setCG(0);	// So it won't be deleted.		// 07/18/03 AM.
//				delete nlp;
				nlp = 0;	// 07/18/03 AM.
				logmsg(_T("  Nlp cleared cg."));	// 07/24/03 AM.
				logcounts();	// 07/18/03 AM.
				logmsg(_T("  Past logcounts."));	// 07/24/03 AM.
			}
//			nlp = new NLP(appDir,true);
			// Create analyzer with dummy name. (Should get its name.) // 07/18/03 AM.
			logmsg(_T("  Before makeNLP"));
			nlp = vtrun->makeNLP(appDir,_T("ananame"));	// 07/18/03 AM.
			logmsg(_T("  After makeNLP"));
			if (!nlp)	// 07/24/03 AM.
				{
				logmsg(_T("  FAILED to makeNLP"));
				}
			else	// 07/24/03 AM.
				{
				nlp->setCG(cg);
				nlp->setFinteractive(true); // Analyzer inside a GUI.	// 05/06/02 AM.
				logmsg(_T("  Before make_analyzer"));
				m_dirtyFlag =	!nlp->make_analyzer(	// 05/26/01 AM.
					analDir, appDir, true, false, 0, appBoolCompile, false, true);	// 06/17/02 AM.
				logmsg(_T("  After make_analyzer"));
				}
			appBoolCompile = 0;
		}
		
		// INIT KB FOR ANALYZER SEQUENCE
		m_boolKBBased = false;
		m_conceptSpec = cg->findConcept(appGUIConcept, _T("spec"));
		if (!m_conceptSpec) {
			bool save = cg->getDirty();	// 07/11/03 AM.
			m_conceptSpec = cg->makeConcept(appGUIConcept, _T("spec"));
			cg->setDirty(save);	// Kb status unaffected.	// 07/11/03 AM.
//			kbDirty = true;
		}
		else {
//			m_boolKBBased = true;	// 06/26/03 AM.
		}

		AfxGetApp()->EndWaitCursor();
	}
	logmsg(_T("Leave InitAnalyzer:"));	// 07/24/03 AM.
}

CDocument *CTreeViewAnalyzer::EditRulesFile(LPCTSTR lpRulesPath)
{
	CVisualTextDoc *doc = NULL;

	if (lpRulesPath || (m_strAlgo == _T("pat") || m_strAlgo == _T("rec")) && m_strData != _T("")) {
		CString rulesPathStr = lpRulesPath ? lpRulesPath : PassPathStr();

		if (rulesPathStr != _T("")) {
//			CString msgStr = "Do you want to create a new rule file called: \"" + m_strData + "\""; // SRP 010321
			if (FileExists(rulesPathStr)) {
				doc = (CVisualTextDoc *)AfxGetApp()->OpenDocumentFile(rulesPathStr);
			}			
//			else if (AfxMessageBox(msgStr, MB_YESNO) == IDYES) { // SRP 010321 Don't ask the question, per AM. Superfluous.
			else { // SRP 010321
				InitFile(rulesPathStr,m_strData,m_strComments);
				doc = (CVisualTextDoc *)AfxGetApp()->OpenDocumentFile(rulesPathStr);
			}
		}
		if (doc) {
			if (lpRulesPath) {
				CString strFileName = StripPath(rulesPathStr,0);
				ClipRight(strFileName,4);
				int pass = PassFromFile(strFileName) + 1;
				HTREEITEM hItem = TreeItemFromPass(pass);
				doc->m_concept = NULL;
				if (hItem) {
					doc->m_concept = (CONCEPT *)m_treeCtrl.GetItemData(hItem);
					doc->SetTitle(CleanPassName(hItem));
				}
			}
			else {
				doc->m_concept = m_conceptSelected;
				doc->SetTitle(CleanPassName(m_itemSelected));
			}
		}
	}
	else if (m_passType != ANAL_PASS_FOLDER) {
		AfxMessageBox(_T("Selected analyzer pass has no pass file."));
	}

	return doc;
}

CString CTreeViewAnalyzer::PassPathStr(CONCEPT *concept, bool noExt, CString newNameStr,
									   bool directoryFlag)
{
	if (!concept)
		concept = m_conceptSelected;
	if (!concept)
		return CString(_T(""));
	CString pathStr = newNameStr;
	CONCEPT *c = concept;
	bool first = true;

	while (c) {
		if (c == m_conceptSpec)
			break;
		if (pathStr != _T(""))
			pathStr = _T("\\") + pathStr;
		if (!first || !directoryFlag)
			pathStr = ConceptName(c) + pathStr;
		c = cg->Up(c);
		first = false;
	}

	if (pathStr.GetAt(0) == '\\')
		ClipLeft(pathStr,1);
	CString rulesPathStr = RulesPath(pathStr,noExt);
	return rulesPathStr;
}

CString CTreeViewAnalyzer::PassDirStr(CString fileNameStr, CONCEPT *concept)
{
	if (!concept)
		concept = m_conceptSelected;
	CString pathStr;
	if (fileNameStr != _T(""))
		pathStr = fileNameStr;
	else
		pathStr = _T("");
	CONCEPT *c = concept;

	while (c) {
		if (c == m_conceptSpec)
			break;
		if (c != concept) {
			if (pathStr != _T(""))
				pathStr = _T("\\") + pathStr;
			pathStr = ConceptName(c) + pathStr;
		}
		c = cg->Up(c);
	}

	CString dirPathStr = RulesPath(pathStr,true);
	return dirPathStr;
}

void CTreeViewAnalyzer::OpenSelectedRulesFile()
{
	EditRulesFile(); // SRP 010620
#if 0
	CString rulesPathStr = RulesPath(m_strData);
	if (FileExists(rulesPathStr))
		AfxGetApp()->OpenDocumentFile(rulesPathStr);
	else
		AfxMessageBox(_T("No file associated with chosen analyzer path!"));
#endif
}

HTREEITEM CTreeViewAnalyzer::SelectPass(int pass)
{
	int i = 0;
	HTREEITEM item = m_treeCtrl.GetRootItem();
	CString debugStr;
	
	while (item) {
		debugStr = CleanPassName(item);

		if (i == pass) {
			m_treeCtrl.SelectItem(item);
			m_treeCtrl.EnsureVisible(item);
			return item;
		}
		item = m_treeCtrl.GetNextSiblingItem(item);
		i++;
	}

	return NULL;
}

void CTreeViewAnalyzer::RenumberPasses(CONCEPT *concept)
{
	if (!concept)
		concept = ConceptFirstSibling(m_conceptSelected);
	if (!concept)
		return;

	RenumberPasses(KBPassNumber(concept),m_treeCtrl.GetCount());
}

void CTreeViewAnalyzer::RenumberPasses(int fromPass, int toPass)
{
	CString debugStr;

	if (fromPass <= 0)
		fromPass = 1;
	int passCount = m_treeCtrl.GetCount();
	if (toPass <= 0)
		toPass = passCount;

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	CONCEPT *c = cg->Down(m_conceptSpec);
	int passNumber = 1;
	HTREEITEM item;

	while (c) {
		debugStr = ConceptName(c);

		if (passNumber >= fromPass) {
			if (passNumber > toPass)
				break;
			KBReplaceValue(c,_T("pass"),passNumber,false);
			item = TreeItemFromConcept(c);
			m_treeCtrl.SetItemText(item,DisplayPassName(c));
		}
		passNumber++;

		c = cg->Next(c);
	}

	theApp.UpdateRuleFileTitles();
}

int CTreeViewAnalyzer::RecalculateFolders(HTREEITEM item)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString debugStr;

	if (item == NULL)
		item = m_treeCtrl.GetRootItem();

	CONCEPT *concept = NULL;
	HTREEITEM child = NULL;
	int itemCount = 0;
	int childCount = 0;
	int grandchildCount = 0;
	CString commentStr;
	_TCHAR commentBuff[101];

	while (item) {
		concept = (CONCEPT *)m_treeCtrl.GetItemData(item);
		debugStr = ConceptName(concept);
		child = m_treeCtrl.GetChildItem(item);
		grandchildCount = (int)KBGetValueLong(concept,_T("itemcount"));

		if (child)
			grandchildCount = RecalculateFolders(child);
		else if (grandchildCount > 0)
			grandchildCount = 0;
		else
			grandchildCount = -1;

		if (grandchildCount >= 0) {
			KBReplaceValue(concept,_T("itemcount"),grandchildCount,false);
			childCount += grandchildCount;

			// PUT ITEM COUNT FOR FOLDER TEMPORARILY IN THE COMMENT FILE
			commentStr.Format(_T("%d"),grandchildCount);
			StringToChar(commentBuff,commentStr,100);
			Aseq::setComment(nlp,KBPassNumber(concept),commentBuff);
			KBReplaceValue(concept,_T("comment"),commentStr,false);
		}

		item = m_treeCtrl.GetNextSiblingItem(item);
		childCount++;
	}

	return childCount;
}

void CTreeViewAnalyzer::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// PostMessage so that focus ends up in newly-opened document, not back in the TreeCtrl
	PostMessage(WM_COMMAND, ID_SEQUENCEMENU_EDITRULES);
	//EditRulesFile();
	
	*pResult = 0;
}

void CTreeViewAnalyzer::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if (appState != VT_APP_EXITING) {
		if (nlp)
			GetPassInfo();
		if (appBoolPageMode && appState != VT_APP_LOADING
			&& appBoolLogAnalyzer)	// If logs toggle is on.	// 09/25/03 AM.

			theApp.UpdateParseTrees(m_intPass);
	}
	
	*pResult = 0;
}

bool CTreeViewAnalyzer::InStub(HTREEITEM itemIn) 
{
	if (!itemIn)
		itemIn = m_itemSelected;

	CString itemStr = m_treeCtrl.GetItemText(itemIn);
	if (itemStr.Find(_T(" stub_")) > 0)
		return true;

	HTREEITEM item = itemIn;

	while (item) {
		itemStr = m_treeCtrl.GetItemText(item);
		if (itemStr.Find(_T(" end_")) > 0)
			return true;
		if (itemStr.Find(_T(" stub_")) > 0)
			return false;
		item = m_treeCtrl.GetNextSiblingItem(item);
	}
	return false;
}

bool CTreeViewAnalyzer::GetStubBoundingItems(HTREEITEM selectedItem, HTREEITEM &startItem, HTREEITEM &endItem) 
{
	CString itemStr;
	HTREEITEM item;
	startItem = NULL;
	endItem = NULL;

	// SEARCH UP
	item = selectedItem;
	while (!startItem && item) {
		itemStr = m_treeCtrl.GetItemText(item);
		if (!startItem && itemStr.Find(_T(" stub_")) > 0)
			startItem = item;
		item = m_treeCtrl.GetPrevSiblingItem(item);
	}

	// SEARCH DOWN
	item = selectedItem;
	while (!endItem && item) {
		itemStr = m_treeCtrl.GetItemText(item);
		if (!endItem && itemStr.Find(_T(" end_")) > 0)
			endItem = item;
		item = m_treeCtrl.GetNextSiblingItem(item);
	}

	return startItem && endItem ? true : false;
}

int CTreeViewAnalyzer::GetPassType(int pass) 
{
	HTREEITEM item = TreeItemFromPass(pass);
	CONCEPT* concept = (CONCEPT *)m_treeCtrl.GetItemData(item);
	if (!concept)
		return ANAL_PASS_NONE;
	CString strAlgo;
	KBFindValue(concept,_T("algo"),strAlgo);
	return PassType(strAlgo);
}

void CTreeViewAnalyzer::GetPassInfo(int passNum) 
{
	//APASS *pass = Aseq::nthPass(nlp,m_intPass+1);

	m_itemSelected = m_treeCtrl.GetSelectedItem();
	m_itemParent = m_treeCtrl.GetParentItem(m_itemSelected);
	m_boolInStub = InStub();
	m_conceptSelected = (CONCEPT *)m_treeCtrl.GetItemData(m_itemSelected);
	CString debugStr = ConceptName(m_conceptSelected);
	m_intPass = passNum < 0 ? KBPassNumber(m_conceptSelected) : passNum;
	m_intConcept = ConceptNumber(m_conceptSelected);

	if (m_conceptSelected) {
		long active;

		//Aseq::unpackPass(pass, algo, data, comment, active);
		m_strStatus = _T("");
		cg->findVal(m_conceptSelected,_T("active"),active);
		if (!active)
			m_strStatus = _T("off");
		KBFindValue(m_conceptSelected,_T("algo"),m_strAlgo);
		KBFindValue(m_conceptSelected,_T("data"),m_strData);
		KBFindValue(m_conceptSelected,_T("comment"),m_strComments);
		m_strComments.TrimLeft();
		m_strComments.TrimRight();

		if (m_strAlgo == _T("folder"))
			m_itemParent = m_itemSelected;

		if (!m_itemParent)
			m_itemParent = TVI_ROOT;

		m_passType = PassType(m_strAlgo);
		if (m_passType == ANAL_PASS_FOLDER)
			m_itemParent = m_itemSelected;
	}
	else {
		m_strAlgo = _T("");
		m_strData = _T("");
		m_strComments = _T("");
		m_strStatus = _T("");
	}

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	if (wnd->GetActiveTab() == WORKSPACE_ANALYZER) {

		// SELECT CORRESPONDING GRAMMAR CONCEPT
		// REMOVE THIS.  TOO TIGHTLY COUPLED  99/12/08 DD
		//if (m_strAlgo == "pat")
			//wnd->SelectGrammarConcept(m_strData);

		// HIGHLIGHT IF TOGGLE ON, TEXT PROCESSED, AND TEXT VISIBLE
		if (appBoolHighlightText &&
			wnd->TextHasAnaLogFiles() &&
			wnd->IsSelectedTextOpen()
			)
			HighlightMatches(m_intPass);
	}
}

void CTreeViewAnalyzer::OnSequencemenuAddexistingrules() 
{
	AddExistingPass();
}

void CTreeViewAnalyzer::AddExistingPass(CString filePathStr) 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CStringArray strFiles;
	strFiles.Add(filePathStr);

	if (wnd->AnalyzerCreated()) {
		if (m_strLastFolderAddPassFromFile.IsEmpty())
			m_strLastFolderAddPassFromFile = theApp.GetProfileString(_T("PREFERENCES"),_T("ANA_LASTFOLDER_ADDPASSFROMFILE"),RulesPath());
		if (filePathStr == _T("")) {
			strFiles.RemoveAll();
			CString filterStr = _T("Pass files (*.pat)|*.pat|All files (*.*)|*.*||");
			CFileDialog fileDlg(true,_T("pat"),m_strLastFolderAddPassFromFile+_T("\\*.pat"),OFN_ALLOWMULTISELECT,filterStr);
			if (fileDlg.DoModal() == IDOK) {
				filePathStr = fileDlg.GetPathName();
				if (IsDirectory(filePathStr)) {
					m_strLastFolderAddPassFromFile = filePathStr;
					POSITION pos = fileDlg.GetStartPosition();
					while (pos)
						strFiles.Add(fileDlg.GetNextPathName(pos));
				}
				else {
					m_strLastFolderAddPassFromFile = StripPath(filePathStr,0,true);
					strFiles.Add(filePathStr);
				}
				theApp.WriteProfileString(_T("PREFERENCES"),_T("ANA_LASTFOLDER_ADDPASSFROMFILE"),m_strLastFolderAddPassFromFile);
			}
		}

		int max = strFiles.GetSize();
		for (int n = max - 1; n >= 0; --n) { // To preserve picked order...
			filePathStr = strFiles.GetAt(n);
			if (EndsWith(filePathStr,_T("pat"))) {
				AddExistingPassFile(filePathStr);
			}
			else if (IsDirectory(filePathStr) && EndsWith(filePathStr,_T("_pats"))) {
				CStringList dirFiles;
				DirectoryFiles(dirFiles,filePathStr,true);
				SortStringList(dirFiles);
				POSITION pos = dirFiles.GetHeadPosition();

				// ADD FOLDER
				CString folderNameStr = StripPath(filePathStr,0);
				folderNameStr = folderNameStr.Mid(0,folderNameStr.GetLength()-5);
				AddFolder(folderNameStr);

				CString fileStr;
				while (pos && (fileStr = dirFiles.GetNext(pos)))
					AddExistingPassFile(fileStr,false);
			}
			else if (filePathStr != _T("")) {
				CString msgStr = _T("\"") + filePathStr + _T("\" is not a pass file!");
				AfxMessageBox(msgStr);
			}
		}
	}

	strFiles.RemoveAll();
}

void CTreeViewAnalyzer::AddExistingPassFile(CString filePathStr, bool editLabelFlag, bool renumberFlag)
{
	CString ruleName = filePathStr;
	ClipRight(ruleName,4);
	ruleName = StripPath(ruleName,0);
	RemovePassOrder(ruleName);
	CString fullRuleNamePathStr = PassPathStr(m_conceptSelected,false,ruleName,true);
	if (filePathStr.CompareNoCase(fullRuleNamePathStr) && FileExists(fullRuleNamePathStr)) {
		CString msgStr;
		msgStr.Format(_T("The pass file \"%s\" already exists.\nDo you want to overwrite it?"),filePathStr);
		if (AfxMessageBox(msgStr,MB_YESNO) != IDYES)
			return;
	}
	CopyFile(filePathStr,fullRuleNamePathStr,false);
	InsertPass(_T("pat"), ruleName, _T("comment"), true, true, editLabelFlag, renumberFlag);
}

void CTreeViewAnalyzer::RemovePassOrder(CString &passNameStr)
{
	int i = 0;
	_TCHAR c;
	while (_istdigit(c = passNameStr.GetAt(i)))
		i++;
	if (c == '_')
		ClipLeft(passNameStr,++i);
}

CONCEPT *CTreeViewAnalyzer::InsertPass(CString algoStr, CString dataStr, CString commentStr,
									   bool active, bool treeInsert, bool editLabelFlag, bool renumberFlag)
{
	// IF USER HAS SELECTED ANYWHERE IN A STUB AREA, SELECT THE
	// THE END STUB AS WHERE TO INSERT THE NEW PASS

	if (m_boolInStub) {
		HTREEITEM startItem, endItem;
		GetStubBoundingItems(m_itemSelected, startItem, endItem);
		if (endItem != m_itemSelected)
			m_treeCtrl.SelectItem(endItem);
	}

	m_dirtyFlag = true;
	_TCHAR buff[1001];
	CONCEPT *itemConcept;

	// GET NAME OF PASS
	CString passStr = PassName(algoStr,dataStr);

	StringToChar(buff,passStr,1000);	
	ANAL_PASS_TYPE type;
	int image,imageSelected;
	ImageType(algoStr,image,imageSelected,type);
	
	// GET LAST PASS CONCEPT IF USER SELECTED IN ODD PLACE
	CONCEPT *c = m_conceptSelected;
	CString debugStr = ConceptName(c);
	CONCEPT *lastPassConcept = LastFolderPassConcept(c);
	bool folderEmpty = (int)KBGetValueLong(c,_T("itemcount")) <= 0 ? true : false;

	// ADJUST CONCEPT FOR FOLDER AND KB WHICH IS FLAT
	int aseqPass = KBPassNumber(c);
	int passNum = aseqPass + 1;
	if (m_passType == ANAL_PASS_FOLDER && !folderEmpty) {
		c = lastPassConcept;
		aseqPass = KBPassNumber(c);
	}

	// ADD TO INTERNAL LITE LIBRARY SEQUENCE
	_TCHAR algoBuff[1001];
	_TCHAR dataBuff[1001];
	_TCHAR commentBuff[1001];
	_tcscpy_s(algoBuff,(const _TCHAR *)algoStr);
	_tcscpy_s(dataBuff,(const _TCHAR *)dataStr);
	_tcscpy_s(commentBuff,(const _TCHAR *)commentStr);

	APASS *currentPass = Aseq::nthPass(nlp,aseqPass);
	if (currentPass)
		Aseq::insertAfter(nlp,currentPass,algoBuff,dataBuff,commentBuff,active);
	else {
		currentPass = Aseq::makePass(nlp,algoBuff,dataBuff,commentBuff,active);
		Aseq::insertPass(nlp,currentPass,0);
	}

	// ADD CONCEPT TO KB
	bool save = cg->getDirty();	// 07/11/03 AM.
	int conceptNumber = ConceptNumber(c)+1;
	itemConcept = cg->makeConcept(m_conceptSpec,buff,conceptNumber);
	KBReplaceValue(itemConcept,_T("algo"),algoStr,false);
	KBReplaceValue(itemConcept,_T("data"),dataStr,false);
	KBReplaceValue(itemConcept,_T("comment"),commentStr,false);
	KBReplaceValue(itemConcept,_T("active"),active,false);
	KBReplaceValue(itemConcept,_T("type"),type,false);
	//if (type != ANAL_PASS_FOLDER)
		KBReplaceValue(itemConcept,_T("pass"),passNum,false);
	cg->setDirty(save);	// Kb status unaffected.	// 07/11/03 AM.

	// ADD TREE ITEM TO ANALYZER TAB
	if (treeInsert) {

		HTREEITEM newItem,afterItem;

		// TAKE CARE OF WHERE TO ADD IT!
		afterItem = m_itemSelected;
		if (m_passType == ANAL_PASS_FOLDER) {
			if (m_boolTopLevel) {
				m_itemParent = m_treeCtrl.GetParentItem(m_itemSelected);
				afterItem = m_itemSelected;
			}
			else if (lastPassConcept) {
				HTREEITEM firstItem =
					TreeCtrlFirstSibling(&m_treeCtrl,m_treeCtrl.GetChildItem(m_itemSelected));
				afterItem = TreeItemFromConcept(lastPassConcept,firstItem);
			}
			else
				afterItem = NULL;
		}
		else if (m_itemSelected)
			afterItem = m_itemSelected;
		else
			afterItem = TVI_FIRST;

		// ADD IT
		CString textStr = DisplayPassName(itemConcept);
		newItem = m_treeCtrl.InsertItem(textStr,image,imageSelected,m_itemParent,afterItem);
		m_treeCtrl.SetItemData(newItem,(DWORD)itemConcept);
		m_treeCtrl.SelectItem(newItem);

		if (renumberFlag) {
			RenumberPasses(itemConcept);
			RecalculateFolders();
		}

		m_dirtyFileFlag = true;

		m_itemSelected = newItem;
		if (editLabelFlag)
			m_treeCtrl.EditLabel(newItem);
	}
	else
		m_intPass++;

	return itemConcept;
}

CONCEPT *CTreeViewAnalyzer::LastFolderPassConcept(CONCEPT *concept)
{
	if (!concept)
		return NULL;

	int itemCount = (int)KBGetValueLong(concept,_T("itemcount"));
	if (itemCount < 0)
		return NULL;

	CONCEPT *c = concept;

	for (int i=0; i<itemCount; i++)
		c = cg->Next(c);

	return c;
}

CONCEPT *CTreeViewAnalyzer::ConceptFromName(CString nameStr)
{
	if (nameStr == _T(""))
		return NULL;

	CONCEPT *c = ConceptFirstSibling(cg->Down(m_conceptSpec));
	CString namStr;

	while (c) {
		namStr = ConceptName(c);
		if (namStr.CompareNoCase(nameStr) == 0)
			break;
		c = cg->Next(c);
	}

	return c;
}

void CTreeViewAnalyzer::OnDelete()
{
	DeleteSelected();
}

void CTreeViewAnalyzer::OnMoveUp()
{
	MoveSelUp();
}

void CTreeViewAnalyzer::OnMoveDown()
{
	MoveSelDown();
}

void CTreeViewAnalyzer::DeleteSelected(bool dontConfirm)
{
	CString msgStr;

	// ENTIRE STUB
	if (m_boolInStub) {
		HTREEITEM startItem, endItem;
		GetStubBoundingItems(m_itemSelected, startItem, endItem);

		msgStr.Format(_T("You have chosen a stub area.\nThis will also delete ALL the corresponding concepts in the Grammar Tab.\n\nAre you sure you want to do this???"));

		if (dontConfirm || AfxMessageBox(msgStr, MB_YESNO) == IDYES) {
			DeleteGrammarStub();
			DeleteStub();
		}
	}

	// SINGLE PASS DELETION
	else {
		msgStr.Format(_T("Are you sure you want to delete the \"%s\" analyzer step?"),
			CleanPassName(m_itemSelected));

		if (dontConfirm || AfxMessageBox(msgStr, MB_YESNO) == IDYES) {
			DeletePass();
			// DeleteGrammarStub(); //  SRP 010207
		}
	}
}

void CTreeViewAnalyzer::DeleteGrammarStub()
{
	HTREEITEM startItem,endItem;
	GetStubBoundingItems(m_itemSelected,startItem,endItem);
	CString itemStr = CleanPassName(startItem);

	theApp.m_pMainFrame->m_wndGrammarView.DeleteStubByName(itemStr);
}

void CTreeViewAnalyzer::DeletePass()
{
	// TO BE TEMPORARY STUFF
	APASS *pass = Aseq::excisePass(nlp,m_intPass);
	Aseq::deletePass(pass);

	// REMOVE FROM KB
	CONCEPT *prevConcept = cg->Prev(m_conceptSelected);
	cg->rmConcept(m_conceptSpec,ConceptNumber(m_conceptSelected));
	
	// REMOVE FROM ANALYZER TAB
	m_treeCtrl.DeleteItem(m_itemSelected);
	if (m_treeCtrl.GetCount() == 0)
		ResetVars();
	else
		RenumberPasses(prevConcept);

	RecalculateFolders();

	m_dirtyFlag = true;
	m_dirtyFileFlag = true;
}

void CTreeViewAnalyzer::DeleteStub()
{
	HTREEITEM startItem, endItem, prevItem;
	GetStubBoundingItems(m_itemSelected, startItem, endItem);
	HTREEITEM item = endItem;

	SetRedraw(false);

	while (item) {
		prevItem = m_treeCtrl.GetPrevSiblingItem(item);
		m_treeCtrl.SelectItem(item);
		DeletePass();
		if (item == startItem)
			break;
		item = prevItem;
	}

	SetRedraw(true);
	m_treeCtrl.InvalidateRect(NULL);
	m_treeCtrl.UpdateWindow();
}

void CTreeViewAnalyzer::OnSequencemenuDelete() 
{
	OnDelete();	
}

void CTreeViewAnalyzer::OnSequencemenuDeletepasses()
{
	int passCount = GetPassCount(m_intPass);
	_TCHAR numStr[101];                                          // 08/29/02 AM.
	_stprintf_s(numStr,_T("%d"),passCount);               // 08/29/02 AM.

	if (AskForText(_T("Number of passes to delete"), numStr)) {
		if (appAskText == _T("")) {
			AfxMessageBox(_T("Must enter a number"));
		} else {
			int i = StringToInteger(appAskText);
			if (i > passCount) {
				AfxMessageBox(_T("Not that many passes to delete"));
			} else {
				for (int j=0; j<i; j++) {
					DeletePass();
				}
			}
		}
	}	
}

void CTreeViewAnalyzer::OnSequencemenuAttributes() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->OpenAttributeEditor(m_conceptSelected, false);
}

void CTreeViewAnalyzer::OnSequencemenuDuplicate() 
{
	CString m_strData2 = m_strData;
	int len = m_strData.GetLength();
	if (_istdigit(m_strData.GetAt(len - 1)) ) {
		int pos = len - 1;
		int numDecimalPlaces = 1;
		while (pos > 0 && _istdigit(m_strData.GetAt(pos-1)) ) {
			--pos;
			numDecimalPlaces++;
		}
		CString strNumber = m_strData.Mid(pos);
		CString strFormat;
		strFormat.Format(_T("%%0%d.%dd"), numDecimalPlaces, numDecimalPlaces);
		strNumber.Format(strFormat,_ttoi(strNumber) + 1);
		m_strData2 = m_strData.Left(pos);
		m_strData2 += strNumber;
	}
	else {
		m_strData2 += _T("2");
	}
	CString oldFile = PassPathStr(m_conceptSpec,false,m_strData);
	CString newFile = PassPathStr(m_conceptSpec,false,m_strData2);
	if (FileExists(newFile)) {
		CString msgStr;
		msgStr.Format(_T("A pass file with the name \"%s\" already exists!\nDo you want to overwrite it?"),newFile);
		if (AfxMessageBox(msgStr, MB_YESNO) != IDYES)
			return;
	}
	CopyFile(oldFile,newFile,false);
	CString commentStr = _T("Copy of ");
	commentStr += m_strData;
	InsertPass(_T("pat"),m_strData2,commentStr,true);
}

void CTreeViewAnalyzer::OnSequencemenuNew() 
{
	bool prevDirtyFlag = m_dirtyFlag;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	if (appBoolAnalyzerLoaded) {
		InsertPass(_T("pat"),_T("unnamed"),_T("comment"),true);	// 08/08/03 AM.
	}
	else if (wnd->AnalyzerCreated()) {
		InitAnalyzer();
		InsertPass(_T("pat"),_T("unnamed"),_T("comment"),true);	// 08/08/03 AM.
	}

	if (!SequenceProperties()) {
		m_dirtyFlag = prevDirtyFlag;
		DeleteSelected(true);
	}
}

void CTreeViewAnalyzer::OnSequencemenuEditrules() 
{
	EditRulesFile();
}

void CTreeViewAnalyzer::OnSequencemenuEditview() 
{
	ViewPassLog(m_intPass);
}

CDocument *CTreeViewAnalyzer::ViewPassLog(int pass, bool selectFlag) 
{
	CDocument *doc = NULL;
	if (selectFlag && pass >= 0 && pass != m_intPass)
		SelectPass(pass);
	CString logPathStr;
	if (GetLogFile(m_intPass,logPathStr,true))
		doc = AfxGetApp()->OpenDocumentFile(logPathStr);
	return doc;
}

bool CTreeViewAnalyzer::GetLogFile(CString &logPathStr) 
{
	return GetLogFile(m_intPass,logPathStr);
}

bool CTreeViewAnalyzer::GetLogFile(CString &logPathStr, bool dontAsk) 
{
	return GetLogFile(m_intPass,logPathStr,false,dontAsk);
}

bool CTreeViewAnalyzer::GetLogFile(int pass, CString &logPathStr, bool force, bool dontAsk) 
{
	logPathStr = _T("");
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString processText = wnd->GetProcessText();
	CString processTextFullPath = InputPath(processText);
	CString logFilePathStr = processTextFullPath + _T("_log");
	bool abortedRun = false;

	if (force ||
		 (m_strStatus != _T("off") &&
		  (m_strAlgo == _T("tokenize") || m_strAlgo == _T("lines") ||
			 m_strAlgo == _T("dicttokz") ||      // 08/16/11 AM.
			 m_strAlgo == _T("dicttok") ||       // 08/09/11 AM.
			 m_strAlgo == _T("cmltokenize") ||   // 08/09/11 AM.
		   ((m_strAlgo == _T("pat") || m_strAlgo == _T("rec")) && m_strData != _T(""))))) {

		if (IsDirectory(logFilePathStr)) {
			logPathStr = OutAnaPath(logFilePathStr,pass);
		}
		else if (!dontAsk && appState == VT_APP_NORMAL &&
				 !IsDirectory(processTextFullPath) &&
				 // IF HIGHLIGHTING, THEN MUST BE OPEN????
				 (!appBoolHighlightText ||
				  theApp.IsVisualTextDocumentOpen(_T("VisualText"),processTextFullPath))
				 ) {
			CString msgStr;
			msgStr.Format(_T("You have concept highlighting on and the text\n\"%s\" has not been processed.\nChoose one of the following options:"),
				processText);

			AskUser(msgStr,IDR_WAVE_STUPID,3,2,_T("Run Analyzer"),_T("Turn off Highlighting"),_T("Continue"));

			if (appButtonReturn == 1) {
				wnd->Run();
				logPathStr = OutAnaPath(logFilePathStr,pass+1);
			}
			else if (appButtonReturn == 2) {
				appBoolHighlightText = false;
			}
			else
				abortedRun = true;
			return false;
		}
	}

	if (!abortedRun) {
		CString finalLogPathStr = logFilePathStr + _T("\\final.log");

		// DEFAULT TO FINAL.LOG IF NO SPECIFIC LOG FILE
		if (!FileExists(logPathStr) && FileExists(finalLogPathStr)) {
			logPathStr = finalLogPathStr;
			return true;
		}
		else if (IsDirectory(processTextFullPath)) {	// Get rid of annoying bell.	// 06/19/03 AM.
			return false;	// 06/19/03 AM.
		}
		else if (!dontAsk && force && !FileExists(logPathStr)) {
			// SRP 010620
			CString msgStr1, msgStr2;
			CString strFileName = StripPathReverse(processText,0,true);
			if (!strFileName.IsEmpty() && !_istupper(strFileName.GetAt(0)))
				strFileName.SetAt(0, _totupper(strFileName.GetAt(0)));
			msgStr1.Format(_T("%s: No log file exists for selected pass \"%d %s\""),
				strFileName, pass, CleanPassNameFromPass(pass));
			msgStr2 = _T("To View Any Highlighting, Text File must be Run with Logs Turned On!");
//			msgStr2 = "To see the log file, click on Generate Logs and rerun the analyzer";
			wnd->AddDebugLine(msgStr1, msgStr2);
			WindowBell();
			return false;
		}
		// In a rule file, right click to Highlight Matches causes crash without this line // SRP 010625
		else if (logPathStr.IsEmpty())
			return false;
	}

	return true;
}

CString CTreeViewAnalyzer::GetFinalLogFile() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString processText = wnd->GetProcessText();
	CString processTextFullPath = InputPath(processText);
	CString logFilePathStr = processTextFullPath + _T("_log\\final.log");
	return logFilePathStr;
}

void CTreeViewAnalyzer::OnSequencemenuViewfulltree() 
{
	CString logFilePathStr;
	if (GetLogFile(m_intPass,logFilePathStr))
		ViewFullTree(logFilePathStr);
}

void CTreeViewAnalyzer::ViewFullTree(CString logFilePathStr)
{
	if (logFilePathStr != _T("")) {
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		wnd->OpenParseTree(m_strData,logFilePathStr);
	}
}

void CTreeViewAnalyzer::OnSequencemenuHelp() 
{
	HelpTopic(_T("Ana_Tab_Popup"));	// 04/27/01 AM.
}

int CTreeViewAnalyzer::PassFromFile(CString fileName)
{

	// NOTE!!!! - THIS FUNCTION RELIES ON THE UNIQUENESS OF
	//            THE NAME.  IN THE FUTURE, IT SHOULD GET THE
	//            PASS NUMBER FROM AN ATTRIBUTE IN THE KB.

	CString itemStr;
	HTREEITEM item = m_treeCtrl.GetRootItem();

	// TAKE CARE IF THERE IS OTHER STUFF AFTER THE FILE NAME
	CString fileStr = fileName;
	int spaceStart = -1;
	if ((spaceStart = fileName.Find(_T(" "),0)) > 0)
		fileStr = fileName.Mid(0,spaceStart);

	for (int i=0; item && i < 10000; i++) {
		if (item) {
			itemStr = CleanPassName(item);
			if (itemStr.CompareNoCase(fileStr) == 0)
				return i;
		}

		item = m_treeCtrl.GetNextSiblingItem(item);
	}
	return -1;
}

int CTreeViewAnalyzer::GetPassCount(int start)
{
	CString itemStr;
	int i = 0;
	int count = 0;
	HTREEITEM item = m_treeCtrl.GetRootItem();
	for (i=0; item && i <= 10000; i++) {
		if (i >= start) count++;
		item = m_treeCtrl.GetNextSiblingItem(item);
	}
	return count;
}

CString CTreeViewAnalyzer::CleanPassName(HTREEITEM item)
{
	CString cleanNameStr = m_treeCtrl.GetItemText(item);
	int pos = cleanNameStr.Find(' ');
	if (pos >= 0)
		ClipLeft(cleanNameStr,pos+1);
	return cleanNameStr;
}

CString CTreeViewAnalyzer::CleanPassNameFromPass(int pass)
{
	HTREEITEM item = TreeItemFromPass(pass);
	if (item)
		return CleanPassName(item);
	return _T("");
}

HTREEITEM CTreeViewAnalyzer::TreeItemFromPass(int pass)
{
	CString itemStr;
	HTREEITEM item = m_treeCtrl.GetRootItem();

	for (int i=1; item && i < 10000; i++) {
		itemStr = CleanPassName(item);
		if (i == pass)
			return item;
		item = m_treeCtrl.GetNextSiblingItem(item);
	}
	return NULL;
}

int CTreeViewAnalyzer::PassNumberFromTreeItem(HTREEITEM item)
{
	CString numStr = m_treeCtrl.GetItemText(item);
	int pos = numStr.Find(' ');
	if (pos >= 0) {
		ClipRight(numStr,numStr.GetLength()-pos);
		return _ttoi(numStr);
	}
	return -1;
}
void CTreeViewAnalyzer::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // This code is to prevent accidental drags.
    if ((GetTickCount() - m_dwDragStart) < DRAG_DELAY)
		return;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	m_hitemDrag = pNMTreeView->itemNew.hItem;
	m_hitemDrop = NULL;
	m_treeCtrl.SelectItem(m_hitemDrag);
	m_itemParent = m_treeCtrl.GetParentItem(m_hitemDrag);

	// Check to see if you should be able to move a file
	ANAL_PASS_TYPE type = TypeFromItem(m_itemSelected);
	CString pathStr = TreePath(m_itemSelected);
	if (pathStr.Find(_T("_log\\")) > 0) {
		AfxMessageBox(_T("Visual Text doesn't allow moving of analyzer log and dump files."));
		return;
	}
	else if (type == ANAL_PASS_SYSTEM) {
		AfxMessageBox(_T("Currently, Visual Text doesn't allow moving the tokenize pass."));
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

#define MOVE_LINE_LENGTH 300

void CTreeViewAnalyzer::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bLDragging) {
		HTREEITEM	hItem;
		UINT		flags;
		POINT pt = point;

		ClientToScreen(&pt);
		pt.x -= DRAG_X_ADJUST;
		pt.y -= DRAG_Y_ADJUST;
		CImageList::DragMove(pt);

		hItem = m_treeCtrl.HitTest(point, &flags);
		int lastDestinationType = m_destinationType;
		m_destinationType = MOVE_BELOW;

		HTREEITEM item = hItem;
		HTREEITEM lastItem = item;
		while (item) {
			lastItem = item;
			if (item == m_hitemDrag)
				m_destinationType = MOVE_ABOVE;
			item = m_treeCtrl.GetNextItem(item);
		}

		if (hItem) {
			CImageList::DragShowNolock(false);
			HTREEITEM lastDropItem = m_hitemDrop;
			m_hitemDrop = GetDropTarget(hItem);

			if (IsDropSource(m_hitemDrop)) {
				bool drawLine = true;
				m_treeCtrl.SelectDropTarget(m_hitemDrop);
				ANAL_PASS_TYPE type = TypeFromItem(m_hitemDrop);
				CRect rect;	
				m_treeCtrl.GetItemRect(hItem,rect,false);

				bool topSensitive = false;
				bool bottomSensitive = false;
				if (type == ANAL_PASS_FOLDER) {
					topSensitive = true;
					bottomSensitive = true;
				}
				else if (m_destinationType == MOVE_BELOW && m_treeCtrl.GetPrevItem(m_hitemDrop) != m_treeCtrl.GetPrevSiblingItem(m_hitemDrop))
					topSensitive = true;
				else if (m_destinationType == MOVE_ABOVE && m_treeCtrl.GetNextItem(m_hitemDrop) != m_treeCtrl.GetNextSiblingItem(m_hitemDrop))
					bottomSensitive = true;

				if (m_hitemDrop != lastDropItem || topSensitive || bottomSensitive) {

					if (topSensitive && abs(point.y - rect.top) <= 3)
						m_destinationType = MOVE_ABOVE;
					else if (bottomSensitive && abs(point.y - rect.bottom) <= 3) {
						if (m_treeCtrl.GetItemState(m_hitemDrop,TVIS_EXPANDED) & TVIS_EXPANDED && m_treeCtrl.GetChildItem(m_hitemDrop))
							m_destinationType = MOVE_BELOW;
						else
							m_destinationType = MOVE_PAST_FOLDER;
					}
					else if (type == ANAL_PASS_FOLDER)
						m_destinationType = MOVE_INTO;

					if (m_hitemDrop) {
						switch (m_destinationType) {
						case MOVE_ABOVE:
						case MOVE_BELOW:
						case MOVE_PAST_FOLDER:
							DrawSeparateLine(m_destinationType == MOVE_ABOVE ? rect.top : rect.bottom, RGB(255,0,0));
							DrawSeparateLine(m_destinationType == MOVE_ABOVE ? rect.bottom : rect.top, RGB(255,255,255));
							break;
						default:
							DrawSeparateLine(rect.bottom, RGB(255,255,255));
							DrawSeparateLine(rect.top, RGB(255,255,255));
							break;
						}
					}
				}
			}
			else
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

void CTreeViewAnalyzer::DrawSeparateLine(int y, COLORREF color)
{
	CDC* pDC = GetDC();
	CPen pen(PS_SOLID, 2, color);
	CPen *oldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(2,y);
	pDC->LineTo(MOVE_LINE_LENGTH,y);
	pDC->SelectObject(oldPen);
}

bool CTreeViewAnalyzer::IsDropSource(HTREEITEM hItem)
{
	if (hItem == NULL)
		return 0;

	ANAL_PASS_TYPE type = TypeFromItem(hItem);
	return type != ANAL_PASS_SYSTEM && type != ANAL_PASS_STUB;
}

HTREEITEM CTreeViewAnalyzer::GetDropTarget(HTREEITEM hItem)
{
	if (hItem == m_hitemDrag)
		return NULL;
	return hItem;
}

void CTreeViewAnalyzer::OnLButtonUp(UINT nFlags, CPoint point) 
{
	bool save = cg->getDirty();	// 07/11/03 AM.
	CPaneView::OnLButtonUp(nFlags, point);

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
		m_treeCtrl.SelectDropTarget(NULL);

		if (m_hitemDrag == m_hitemDrop || !IsDropSource(m_hitemDrop))
			return;
		// If Drag item is an ancestor of Drop item then return

		HTREEITEM htiParent = m_hitemDrop;
		while((htiParent = m_treeCtrl.GetParentItem( htiParent )) != NULL )
			if( htiParent == m_hitemDrag ) return;

		MoveTreeItem(m_hitemDrag,m_hitemDrop,m_treeCtrl.GetParentItem(m_hitemDrop));
	}
	cg->setDirty(save);	// Kb status unaffected.	// 07/11/03 AM.
}

ANAL_PASS_TYPE CTreeViewAnalyzer::TypeFromItem(HTREEITEM item)
{
	CONCEPT *concept = (CONCEPT *)m_treeCtrl.GetItemData(item);
	return ConceptType(concept);
}

void CTreeViewAnalyzer::RunAnalyzer(CString filePathStr, int level)
{
	CMainFrame *wnd = theApp.m_pMainFrame;
	wnd->m_paneManager.ShowPane(IDR_PANE_ANALYZE);

	StartWaitCursor();	// Trying this here.	// 06/24/02 AM.
	wnd->ClearOutput();			// Trying this here.	// 06/24/02 AM.

	// RE-INTERN THE ANALYZER SEQUENCE HERE.	// 05/19/01 AM.
	if (m_dirtyFlag) {
		if (Aseq::internSeq(nlp))				// 05/19/01 AM.
			m_dirtyFlag = false;

		// DISPLAY RE-INTERNING MESSAGES.	// 05/19/01 AM.
		theApp.DisplayAnalyzerLoadStats(false);	// 05/26/01 AM.

		if (m_dirtyFlag) {	// Didn't intern correctly.	// 05/19/01 AM.
			wnd->AddAnalyzerLine(_T(">>>SYNTAX ERRORS"),_T("NOT ANALYZING TEXT")); // 05/19/01AM.
			// TODO: REMOVE FILES FROM _LOG FOLDER.	// 05/19/01 AM. 
			StopWaitCursor();	// 06/24/02 AM.
			return;		// DON'T ANALYZE TEXT!	// 05/26/01 AM.
		}
	}

	m_fSuspendAnalyzeThread = false;
	m_AnalyzePathStr = filePathStr;
	m_AnalyzeOutputFinalMsgStr.Empty();
	m_prevAppState = appState;

	if (IsDirectory(filePathStr) && !EndsWith(filePathStr,_T("_log")))  //DIRECTORY
		appState = VT_APP_DIR_ANALYZING;
	else
		appState = VT_APP_ANALYZING;

	wnd->SetAnalyzingToolbarButtons(false);

	m_AnalyzeFileIndex = 0;
	m_AnalyzePass = 0;
	m_AnalyzeFiles.RemoveAll();
	m_AnalyzeOutputStrFilesTime.RemoveAll();
	for (int n = 0; n < 2; n++) {
		m_AnalyzeOutputStrDirs[n].RemoveAll();
		m_AnalyzeOutputStrFiles[n].RemoveAll();
	}
	CollectFilesToAnalyze(filePathStr,level);
	if (m_AnalyzeFiles.GetSize() > 0) {
//		StartWaitCursor();		// Starts too late.	// 06/24/02 AM.
		DispatchAnalyzeThread();
	}
	else	// 06/24/02 AM.
		StopWaitCursor();	// 06/24/02 AM.

	wnd->m_paneManager.ShowPane(IDR_PANE_ANALYZE);
}

void CTreeViewAnalyzer::CollectFilesToAnalyze(const CString& filePathStr, int level)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	CString msgStr;
	if (IsDirectory(filePathStr)) {
		if (EndsWith(filePathStr,_T("_log")))
			return;
		msgStr = IndentStr(level*3,0) + _T("Analyzing ");
		msgStr += level ? _T("subdirectory") : _T("directory");
		msgStr += _T(": ");
		m_AnalyzeOutputStrDirs[0].Add(msgStr);
		m_AnalyzeOutputStrDirs[1].Add(StripPath(filePathStr,level,false));

		CFileFind finder;
		BOOL bWorking = finder.FindFile(filePathStr+_T("\\*.*"));
		CString fileName;
		CString fullPathStr;

		while (bWorking) {
			bWorking = finder.FindNextFile();
			if (finder.IsDots())
				continue;
			fileName = finder.GetFileName();
			fullPathStr = filePathStr + _T("\\") + fileName;
			CollectFilesToAnalyze(fullPathStr,level+1);
		}
		finder.Close();
	}
	else {
		CString strIndent = IndentStr(level*3,0);
		m_AnalyzeFiles.Add(filePathStr);
		msgStr.Format(_T("%sAnalyzing (%d)"),strIndent,m_AnalyzeFiles.GetSize());
		m_AnalyzeOutputStrFiles[0].Add(msgStr);
		m_AnalyzeOutputStrFiles[1].Add(StripPath(filePathStr,0,false));
		m_AnalyzeOutputStrFilesTime.Add(strIndent);
		if (m_AnalyzeOutputStrDirs[0].GetSize() < m_AnalyzeOutputStrFiles[0].GetSize() ) {
			m_AnalyzeOutputStrDirs[0].Add(_T(""));
			m_AnalyzeOutputStrDirs[1].Add(_T(""));
		}
	}

	//FillListWithSequence();
}

LRESULT CTreeViewAnalyzer::OnAnalyzeOutputMessage(WPARAM wParam, LPARAM lParam)
{
	int index = (int)wParam;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	if (index < m_AnalyzeOutputStrDirs[0].GetSize() && !m_AnalyzeOutputStrDirs[0][index].IsEmpty())
		wnd->AddAnalyzerLine(m_AnalyzeOutputStrDirs[0][index], m_AnalyzeOutputStrDirs[1][index], true);
	if (index < m_AnalyzeOutputStrFiles[0].GetSize() && !m_AnalyzeOutputStrFiles[0][index].IsEmpty())
		wnd->AddAnalyzerLine(m_AnalyzeOutputStrFiles[0][index], m_AnalyzeOutputStrFiles[1][index], true);
	return 0L;
}

LRESULT CTreeViewAnalyzer::OnAnalyzeOutputPassMessage(WPARAM wParam, LPARAM lParam)
{
	int pass = (int)wParam;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString passStr,passFileStr;
	passFileStr = StripPath(PassFileFromNumber(pass),0);

	int index = m_AnalyzeFileIndex;
	int count = m_AnalyzeOutputStrFiles[0].GetSize() - 1;
	if (index > count)
		index = count;
	CString fileStr = m_AnalyzeOutputStrFiles[0].GetAt(index);
	int indent = StripIndent(fileStr);
	CString indentStr = IndentStr(indent+3,0);
	CString msgStr;
	msgStr.Format(_T("%sAnalyzing pass:"),indentStr);

	passStr.Format(_T("%d %s"),pass,passFileStr);
	if (passStr != _T("") && pass)
		wnd->AddAnalyzerLine(msgStr, passStr, true);
	return 0L;
}

LRESULT CTreeViewAnalyzer::OnPostAnalyzeFileTasks(WPARAM wParam, LPARAM lParam)
{
//	Sleep(10);	// TESTING THEORY!!!	// 11/01/02 AM.
//	logmsg("OnPostAnalyzeFileTasks:");
	CMainFrame *wnd = theApp.m_pMainFrame;
	//CTreeViewAnalyzer *pView = &(theApp.m_pMainFrame->m_wndAnalyzerView);
	int index = (int)wParam;

//	if (m_OkAnalysis)	// 06/19/03 AM.
	ProcessLog(m_AnalyzeFiles[index]);
	wnd->m_wndTextView.SetTextAsAnalyzed(m_AnalyzeFiles[index]);

	if (appBoolVerbose && index < m_AnalyzeOutputStrFiles[0].GetSize() && !m_AnalyzeOutputStrFiles[0][index].IsEmpty()) {
		CString strTime;
		ANLZ_GetStrTime(strTime, GetTickCount() - m_dwTimeStartFile);
		m_AnalyzeOutputStrFilesTime[index] += _T("Time = ") + strTime;
		wnd->AddAnalyzerLine(m_AnalyzeOutputStrFilesTime[index], m_AnalyzeOutputStrFiles[1][index], true);
	}

	m_AnalyzePass = 0;
	DispatchAnalyzeThread(); // Resume Thread
	return 0L;
}

LRESULT CTreeViewAnalyzer::OnAnalyzeHighlight(WPARAM wParam, LPARAM lParam)
{
	HighlightMatches(m_intPass);
	return 0L;
}

LRESULT CTreeViewAnalyzer::OnAnalyzeCleanup(WPARAM wParam, LPARAM lParam)
{
//	logmsg("OnAnalyzeCleanup:");
	BOOL bCompleted = (BOOL)wParam;

	m_AnalyzeFileIndex = 0;
	m_AnalyzePass = 0;
	m_AnalyzeFiles.RemoveAll();
	for (int n = 0; n < 2; n++) {
		m_AnalyzeOutputStrDirs[n].RemoveAll();
		m_AnalyzeOutputStrFiles[n].RemoveAll();
	}

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->SetAnalyzingToolbarButtons(true);
	if (!kbDirty)	// 06/21/03 AM.
		kbDirty = cg->getDirty();	// 06/21/03 AM.

	appState = m_prevAppState;

	if (bCompleted) {
		wnd->AddAnalyzerLine(m_AnalyzeOutputFinalMsgStr, m_AnalyzePathStr);
		wnd->AddErrorToDebug();
	}

	theApp.UpdateAnalyzerWindows(true,false,true);
	theApp.DoAllViews(NULL,"VisualText",".log",DOC_ACTION_UPDATE);
	theApp.DoAllViews(NULL,"VisualText",".txt",DOC_ACTION_UPDATE);

	wnd->m_wndAnalyzePane.m_listCtrl.AutoSizeColumn();

	StopWaitCursor();

	return 0L;
}

void CTreeViewAnalyzer::ProcessLog(CString fileName)
{
	CMainFrame *wnd = theApp.m_pMainFrame;
	CTreeViewText *pView = &(wnd->m_wndTextView);
	if (!appBoolBatchMode)
		pView->MoveLogFiles(fileName);
	RecursiveRemove(OutputPath(),true,0,false);
}

void CTreeViewAnalyzer::MoveLogFiles(CString fileName, CString logDir)
{
	CFileFind finder;
	CString pathStr = OutputPath(_T("*.*"));
	BOOL bWorking = finder.FindFile(pathStr);
	CString logName;
	CString origFile;
	CString newFile;

	while (bWorking) {
		bWorking = finder.FindNextFile();
		logName = finder.GetFileName();
		if (logName != _T(".") && logName != _T("..") && logName != _T("rfa.log") &&
			logName != _T("def.log")) {
			origFile = OutputPath(logName);
			newFile = logDir + _T("\\") + logName;
			CopyFile(origFile,newFile,false);
		}
	}
	finder.Close();
}

CDocument *CTreeViewAnalyzer::GetLogFromDocument(CDocument *doc)
{	
	return ViewPassLog(PassFromDocument(doc));
}

int CTreeViewAnalyzer::PassFromDocument(CDocument *doc)
{
	CString filePath = doc->GetPathName();
	CString dataStr = StripPath(filePath,0);
	ClipRight(dataStr,4);
	return PassFromFile(dataStr);
}

void CTreeViewAnalyzer::SetPassDirty(CString rulePathStr)
{
	if (rulePathStr == _T("")) {
		m_dirtyFlag = true;
		m_dirtyFileFlag = true;
	}
	else if (EndsWith(rulePathStr,_T("pat"))) {
		CString ruleStr = StripPath(rulePathStr,0);
		ClipRight(ruleStr,4);
		int pass = PassFromFile(ruleStr) + 1;
		CMainFrame *mainFrm = (CMainFrame *)AfxGetMainWnd();
		mainFrm->AddDebugLine(_T("Modified:"),rulePathStr);
		Aseq::setDirty(nlp,Aseq::nthPass(nlp,pass));
		m_dirtyFlag = true;
	}
}

CString CTreeViewAnalyzer::GetSelection()
{
	return StateString(&m_treeCtrl,m_treeCtrl.GetRootItem(),STATE_GET_SELECTED);
}

void CTreeViewAnalyzer::SetSelection(CString selStr)
{
	//SetRedraw(false);
	StateFromString(&m_treeCtrl,m_treeCtrl.GetRootItem(),STATE_SELECT,selStr);
	//SetRedraw(true);
}

CString CTreeViewAnalyzer::TreePath(HTREEITEM child)
{
	if (!child)
		child = m_treeCtrl.GetSelectedItem();
	if (!child)
		return _T("");

	HTREEITEM parent;
	CString pathStr = CleanPassName(child);
	CString parentStr;
	
	while (parent = m_treeCtrl.GetParentItem(child)) {
		parentStr = CleanPassName(parent);
		pathStr = parentStr + ANALSEP + pathStr;
		child = parent;
	}
	return pathStr;
}

void CTreeViewAnalyzer::Clear()
{
	logmsg(_T("Enter ClearAnalyzer"));
	SetRedraw(false);
	m_treeCtrl.DeleteAllItems();
	SetRedraw(true);
	m_treeCtrl.InvalidateRect(NULL);
	m_treeCtrl.UpdateWindow();

	if (!cg)	// 07/24/03 AM.
		logmsg(_T("  EMPTY CG."));
	else	// 07/24/03 AM.
		cg->rmChildren(m_conceptSpec);
	if (nlp) {
//		Aseq::writeFile(nlp);	// QUERY USER ONLY.	// 06/26/03 AM.
		logmsg(_T("  Before NLP zero CG"));
		nlp->setCG(0);	// So it won't be deleted.		// 07/18/03 AM.
		logmsg(_T("  After NLP zero CG"));
//		delete nlp;
		logmsg(_T("  Before deletNLP"));
		vtrun->deleteNLP(nlp);	// 07/18/03 AM.
		logmsg(_T("  After deletNLP"));
		nlp = 0;	// 07/18/03 AM.
	}
	else	// 07/24/03 AM.
		logmsg(_T("  EMPTY NLP OBJECT"));	// 07/24/03 AM.

	InitVars();
	logmsg(_T("Leave ClearAnalyzer"));
}

void CTreeViewAnalyzer::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	if (m_strData == MAINSTR)
		*pResult = -1;
	else {
		CString itemStr = pTVDispInfo->item.pszText;
		int pos = itemStr.Find(_T(" "),0);
		if (pos > 0) {
			CEdit* pEdit = m_treeCtrl.GetEditControl();
			ClipLeft(itemStr,pos+1);
			pEdit->SetWindowText(itemStr);
		}
		*pResult = 0;
	}
}


void CTreeViewAnalyzer::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	//RenamePass(m_intPass,pTVDispInfo->item.pszText);
	if (m_itemSelected) {
		CONCEPT *concept = (CONCEPT *)m_treeCtrl.GetItemData(m_itemSelected);
		RenamePass(concept,pTVDispInfo->item.pszText);
	}
	*pResult = 0;
}

bool CTreeViewAnalyzer::RenamePass(CONCEPT *concept, CString newNameStr) 
{
	if (newNameStr == _T("") || concept == NULL)
		return false;

	// RENAME IN OLD SYSTEM
	_TCHAR data[1001];
	if (IsConceptFirstChild(concept,m_conceptSpec)) {
		_tcscpy_s(data,Aseq::getData(nlp,m_intPass));
	}

	if (newNameStr == MAINSTR) {
		CString msgStr;
		msgStr.Format(_T("You cannot name a pass \"%s\"."),MAINSTR);
		AfxMessageBox(msgStr);
		return false;
	}

	//CONCEPT *concept = NthConcept(m_conceptSelected,intPass);
	CString conceptNameStr = ConceptName(concept);

	// GET OLD AND NEW PATHS
	ANAL_PASS_TYPE type = ConceptType(concept);
	CString oldPath,newPath;
	bool noExt = type == ANAL_PASS_FOLDER ? true : false;
	oldPath = PassPathStr(cg->Up(concept),noExt,conceptNameStr);
	newPath = PassPathStr(cg->Up(concept),noExt,newNameStr);
	if (oldPath == newPath)
		return false;

	CString msgStr;

	if (FileExists(newPath)) {
		msgStr.Format(_T("There is already a pass file with the name \"%s\".\nDo you want to replace it?"),newPath);
		if (AfxMessageBox(msgStr, MB_YESNO) != IDYES)
			return false;
		DeleteFile(newPath);
	}
//	if (FileExists(oldPath) && !UTL_Rename(oldPath,newPath))	// 12/14/06 AM.
//		return false;	// 12/14/06 AM.

	// RENAME IN KB
	_TCHAR nameBuff[1001];
	StringToChar(nameBuff,newNameStr,1000);
	cg->renameConcept(m_conceptSelected,nameBuff);
	KBReplaceValue(m_conceptSelected,_T("data"),newNameStr,false);

	m_dirtyFlag = true;
	m_dirtyFileFlag = true;

	// RENAME IN TREE STRUCTURE
	CString debugStr = m_treeCtrl.GetItemText(m_itemSelected);
	CString displayNameStr = DisplayPassName(m_conceptSelected);
	m_treeCtrl.SetItemText(m_itemSelected,displayNameStr);

	// IF RULE FILE OPEN, UPDATE
	if (theApp.IsVisualTextDocumentOpen(_T("VisualText"),oldPath)) {
		// Note: This call crashes if oldPath file isn't around. // 12/14/06 AM.
		CDocument *doc = theApp.OpenDocumentFile(oldPath);
		doc->SetPathName(newPath);
		CView *view = GetFirstView(doc);
		view->UpdateWindow();
	}

	// FIX. Moving this after trying to access old name.	// 12/14/06 AM.
	if (FileExists(oldPath) && !UTL_Rename(oldPath,newPath))
		return false;

	// RENAME IN ANALYZER SEQUENCE
	StringToChar(data,newNameStr,1000);
	m_strData = newNameStr;
	Aseq::setData(nlp,m_intPass,data);

	return true;
}

void CTreeViewAnalyzer::SetPassActive(CONCEPT *concept, bool activeFlag) 
{
	ANAL_PASS_TYPE cType = ConceptType(concept);

	bool active = activeFlag ? true : false;
	int pass = KBPassNumber(concept);
	bool prevActive = KBGetValueBool(concept,_T("active"));	// 08/19/03 AM.
	if (active != prevActive)	// 08/19/03 AM.
		{
		m_dirtyFlag = true;	// 08/19/03 AM.
		m_dirtyFileFlag = true;	// 08/19/03 AM.
		}

	Aseq::setActive(nlp,KBPassNumber(concept),active);
	KBReplaceValue(concept,_T("active"),active,false);

	// SET THE CORRECT IMAGE TYPE
	HTREEITEM conceptTreeItem = TreeItemFromConcept(concept);

	if (activeFlag) {
		int image,imageSelected;
		ImageFromType(cType,active,image,imageSelected);
		m_treeCtrl.SetItemImage(conceptTreeItem,image,imageSelected);
	}
	else
		m_treeCtrl.SetItemImage(conceptTreeItem,14,14);
}

void CTreeViewAnalyzer::RetypePass(CONCEPT *concept, ANAL_PASS_TYPE type) 
{
	ANAL_PASS_TYPE cType = ConceptType(concept);
	if (cType == type)
		return;

	CString algoStr = PassTypeStr(type);
	KBReplaceValue(concept,_T("algo"),algoStr,false);
	KBReplaceValue(concept,_T("type"),type,false); // Seems forgotten.	// 10/17/05 AM.

	if (type != ANAL_PASS_FOLDER
	// && IsConceptFirstChild(concept,m_conceptSpec)  // Looks wrong.  // 10/17/05 AM.
	) {
		_TCHAR algo[1001];
		StringToChar(algo,algoStr,1000);
		int pass = KBPassNumber(concept);
		Aseq::setAlgo(nlp,pass,algo);
		_tcscpy_s(algo,Aseq::getAlgo(nlp,pass));
	}

	m_dirtyFlag = true;
	m_dirtyFileFlag = true;
//	kbDirty = true;
}

HTREEITEM CTreeViewAnalyzer::TreeItemFromConcept(CONCEPT *concept, HTREEITEM item)
{
	if (!item)
		item = m_treeCtrl.GetRootItem();

	HTREEITEM foundItem,child;
	CONCEPT *c;
	CString debugStr;

	while (item) {
		c = (CONCEPT *)m_treeCtrl.GetItemData(item);
		debugStr = m_treeCtrl.GetItemText(item);
		if (c == concept)
			return item;
		if ((child = m_treeCtrl.GetChildItem(item)) &&
			(foundItem = TreeItemFromConcept(concept,child)))
			return foundItem;
		item = m_treeCtrl.GetNextSiblingItem(item);
	}
	return NULL;
}

void CTreeViewAnalyzer::MoveSelUp()
{
	if (CanMoveSelectedUp()) {
		m_destinationType = MOVE_ABOVE;
		MoveTreeItem(m_itemSelected,m_treeCtrl.GetPrevSiblingItem(m_itemSelected));
	}
//		MovePass(false);
}

void CTreeViewAnalyzer::MoveSelDown()
{
	if (CanMoveSelectedDown()) {
		m_destinationType = MOVE_BELOW;
		HTREEITEM toItem = m_treeCtrl.GetNextSiblingItem(m_itemSelected);
		if (TypeFromItem(toItem) == ANAL_PASS_FOLDER)
			m_destinationType = MOVE_PAST_FOLDER;
		MoveTreeItem(m_itemSelected,toItem,m_treeCtrl.GetParentItem(toItem));
	}
//		MovePass(true);
}

void CTreeViewAnalyzer::Save(bool askUserFlag)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	if (wnd->AnalyzerCreated()) {
		CString questionStr = _T("Do you want to save the current analyzer?");
		if (m_dirtyFileFlag && (!askUserFlag || AfxMessageBox(questionStr, MB_YESNO) == IDYES)) {
			Aseq::writeFile(nlp);
			m_dirtyFileFlag = false;
		}
	}
}

void CTreeViewAnalyzer::MovePass(bool down)
{
	HTREEITEM startItem,endItem;
	CONCEPT *startConcept;

	if (down) {
		if (m_boolInStub) {
			HTREEITEM afterEndItem;

			GetStubBoundingItems(m_itemSelected,startItem,endItem);
			afterEndItem = m_treeCtrl.GetNextSiblingItem(endItem);

			if (InStub(afterEndItem)) {
				HTREEITEM sItem,eItem;
				GetStubBoundingItems(afterEndItem,sItem,eItem);
				startConcept = (CONCEPT *)m_treeCtrl.GetItemData(sItem);
				MoveTreeNodesDownPastGroup(startItem,sItem,eItem);
			}
			else {
				startConcept = (CONCEPT *)m_treeCtrl.GetItemData(startItem);
				MoveTreeNodesDown(startItem,endItem);
				if (cg->Prev(startConcept))
					startConcept = cg->Prev(startConcept);
			}
			RenumberPasses(startConcept);
		}
		else {
			HTREEITEM nextItem = m_treeCtrl.GetNextSiblingItem(m_itemSelected);

			if (InStub(nextItem)) {
				GetStubBoundingItems(nextItem,startItem,endItem);
				MoveTreeNodesUp(startItem,endItem);
				startConcept = (CONCEPT *)m_treeCtrl.GetItemData(startItem);
				startConcept = cg->Prev(startConcept);
				RenumberPasses(startConcept);
				nextItem = m_treeCtrl.GetNextSiblingItem(endItem);
				SelectPass(TreeItemNumberFromItem(&m_treeCtrl,nextItem)-1);
			}
			else {
				cg->moveCright(m_conceptSelected);
				Aseq::moveDown(nlp,m_intPass);
				MoveTreeNodeDown(&m_treeCtrl,m_itemSelected);
				startConcept = cg->Prev(m_conceptSelected);
				RenumberPasses(startConcept);
			}
		}
	}
	else {
		if (m_boolInStub) {
			HTREEITEM beforeStartItem;

			GetStubBoundingItems(m_itemSelected,startItem,endItem);
			beforeStartItem = m_treeCtrl.GetPrevSiblingItem(startItem);

			if (InStub(beforeStartItem)) {
				HTREEITEM sItem,eItem;
				GetStubBoundingItems(beforeStartItem,sItem,eItem);
				MoveTreeNodesDownPastGroup(sItem,startItem,endItem);
			}
			else
				MoveTreeNodesUp(startItem,endItem);

			RenumberPasses();  // zero numbered
		}
		else {
			HTREEITEM prevItem = m_treeCtrl.GetPrevSiblingItem(m_itemSelected);

			if (InStub(prevItem)) {
				GetStubBoundingItems(prevItem,startItem,endItem);
				MoveTreeNodesDown(startItem,endItem);
				RenumberPasses();
				prevItem = m_treeCtrl.GetPrevSiblingItem(startItem);
				SelectPass(TreeItemNumberFromItem(&m_treeCtrl,prevItem)-1);
			}
			else {
				cg->moveCleft(m_conceptSelected);
				Aseq::moveUp(nlp,m_intPass);
				MoveTreeNodeUp(&m_treeCtrl,m_itemSelected);
				RenumberPasses();
			}
		}
	}

	m_dirtyFileFlag = true;
}

HTREEITEM CTreeViewAnalyzer::MoveTreeItem(HTREEITEM fromItem, HTREEITEM toItem, HTREEITEM toParent)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString msgStr;
	CONCEPT *fromConcept, *toConcept, *concept;
	int fromPass, toPass;
	ANAL_PASS_TYPE fromType,toType;
	int fromItemCount,toItemCount;

	fromConcept = GetItemInfo(fromItem,fromPass,fromType,fromItemCount);
	toConcept = GetItemInfo(toItem ? toItem : toParent,toPass,toType,toItemCount);

	if (toItem == NULL)
		toItem = toParent;

	bool insertFirstFlag = false;
	HTREEITEM toConceptItem = toItem;

	// MOVE TO ITEM TO PREVIOUS IF DESTINATION TYPE IS ABOVE
	if (m_destinationType == MOVE_PAST_FOLDER) {
		toItem = m_treeCtrl.GetNextSiblingItem(toItem);
		if (toItem) {
			toConcept = GetItemInfo(toItem,toPass,toType,toItemCount);
			m_destinationType = MOVE_ABOVE;
		}
		else
			toPass = m_treeCtrl.GetCount();
	}

	if (m_destinationType == MOVE_ABOVE) {
		HTREEITEM prevItem = m_treeCtrl.GetPrevSiblingItem(toItem);
		m_destinationType = MOVE_BELOW;
		if (prevItem) {
			toConceptItem = m_treeCtrl.GetPrevItem(toItem);  // THE CONCEPT NEEDS THE IMMEDIATELY PREVIOUS TREE ITEM
			toItem = prevItem;
			int saveToPass = toPass;
			toConcept = GetItemInfo(toItem,toPass,toType,toItemCount);
			toPass = saveToPass - 1;
		}
		// INSERTING AFTER A FOLDER
		else {
			concept = cg->Prev(toConcept);
			if ((int)KBGetValueLong(concept,_T("itemcount")) > 0) {
				insertFirstFlag = true;
				toConceptItem = m_treeCtrl.GetParentItem(toItem);
			}
		}
	}
	else if (m_destinationType == MOVE_BELOW && toItemCount > 0 && toType == ANAL_PASS_FOLDER) {
		toItem = m_treeCtrl.GetChildItem(toItem);
		toConcept = GetItemInfo(toItem,toPass,toType,toItemCount);
		insertFirstFlag = true;
	}

	// IF PASS NUMBERS ARE -1 BECAUSE OF FOLDERS, THEN FIND NEAREST PASS NUMBER
	if (toType == ANAL_PASS_FOLDER && toPass == -1)
		toPass = KBPassNumber(cg->Next(toConcept));

	// IF MOVING ABOVE, THEN SWITCH TO AND FROM PASS NUMBERS FOR RENUMBERING
	if (fromPass > toPass) {
		int savePass = fromPass;
		fromPass = toPass;
		toPass = savePass;
		if (fromType == ANAL_PASS_FOLDER)
			toPass += fromItemCount - 1;
	}

	if (toItem)
		toParent = m_treeCtrl.GetParentItem(toItem);

	// ADJUST CONCEPT TO BE THE CORRECT ONE FOR INSERTING AFTER (ALWAYS AFTER)
	if (m_destinationType == MOVE_INTO || m_destinationType == MOVE_PAST_FOLDER) {
		for (int i=0; i<toItemCount; i++) {
			toPass++;
			toConceptItem = m_treeCtrl.GetNextItem(toConceptItem);
		}
	}

	// MOVING A FOLDER AND ITS CONTENTS IN THE KB
	if (fromType == ANAL_PASS_FOLDER) {
		HTREEITEM item = fromItem;
		for (int i=0; i<fromItemCount; i++)
			item = m_treeCtrl.GetNextItem(item);
		for (int i=0; i<=fromItemCount; i++) {
			MoveConcept(item,toConceptItem,toParent);
			item = m_treeCtrl.GetPrevItem(item);
		}
	}
	else
		MoveConcept(m_itemSelected,toConceptItem,toParent);

	// MOVE GUI TREE
	HTREEITEM fromCopy = NULL;
	UINT isExpanded = m_treeCtrl.GetItemState(fromItem,TVIS_EXPANDED) & TVIS_EXPANDED;

	if (insertFirstFlag)
		toItem = TVI_FIRST;
	if (m_destinationType == MOVE_PAST_FOLDER && toType == ANAL_PASS_FOLDER)
		fromCopy = CopyBranch(&m_treeCtrl,fromItem,toParent,TVI_LAST);
	else if (m_destinationType == MOVE_INTO && toType == ANAL_PASS_FOLDER)
		fromCopy = CopyBranch(&m_treeCtrl,fromItem,toItem,TVI_LAST);
	else
		fromCopy = CopyBranch(&m_treeCtrl,fromItem,toParent,toItem);
	m_treeCtrl.DeleteItem(fromItem);

	m_dirtyFileFlag = true;

	RecalculateFolders();

	// RENUMBER
	RenumberPasses(fromPass,toPass);

	m_treeCtrl.SelectItem(fromCopy);
	if (isExpanded)
		m_treeCtrl.Expand(fromCopy,TVE_EXPAND);

	UpdateKB();

	return fromCopy;
}

int CTreeViewAnalyzer::GetFolderItemCount(CONCEPT *concept)
{
	int itemCount = (int)KBGetValueLong(concept,_T("itemcount"));
	return itemCount == -1 ? 0 : itemCount;
}

CONCEPT *CTreeViewAnalyzer::GetItemInfo(HTREEITEM item, int &pass, ANAL_PASS_TYPE &type, int &count)
{
	CONCEPT *concept = (CONCEPT *)m_treeCtrl.GetItemData(item);
	pass = KBPassNumber(concept);
	type = TypeFromItem(item);
	count = GetFolderItemCount(concept);

	return concept;
}

void CTreeViewAnalyzer::MoveConcept(HTREEITEM fromItem, HTREEITEM toItem, HTREEITEM toParent)
{
	CONCEPT *fromConcept = (CONCEPT *)m_treeCtrl.GetItemData(fromItem);
	CONCEPT *toConcept = (CONCEPT *)m_treeCtrl.GetItemData(toItem ? toItem : toParent);

	int fromPass = KBPassNumber(fromConcept);
	int fromInt = ConceptNumber(fromConcept);
	int toInt = ConceptNumber(toConcept);
	int count = abs(fromInt - toInt) - 1;
	bool down = fromInt > toInt ? true : false;
	if (!down)
		count++;

	for (int i=0; i<count; i++) {
		if (down) {
			cg->moveCleft(fromConcept);
			Aseq::moveUp(nlp,fromPass);
			fromPass--;
		}
		else {
			cg->moveCright(fromConcept);
			Aseq::moveDown(nlp,fromPass);
			fromPass++;
		}
	}
}

void CTreeViewAnalyzer::MoveTreeNodesDown(HTREEITEM startItem, HTREEITEM endItem)
{
	HTREEITEM beforeStartItem = m_treeCtrl.GetPrevSiblingItem(startItem);
	HTREEITEM afterEndItem = m_treeCtrl.GetNextSiblingItem(endItem);
	MoveTreeItem(afterEndItem,beforeStartItem);
}

void CTreeViewAnalyzer::MoveTreeNodesUp(HTREEITEM startItem, HTREEITEM endItem)
{
	HTREEITEM beforeStartItem = m_treeCtrl.GetPrevSiblingItem(startItem);
	MoveTreeItem(beforeStartItem,endItem);
}

void CTreeViewAnalyzer::MoveTreeNodesDownPastGroup(HTREEITEM startItem, HTREEITEM sItem, HTREEITEM eItem)
{
	HTREEITEM beforeStartItem = m_treeCtrl.GetPrevSiblingItem(startItem);
	HTREEITEM item = eItem;
	HTREEITEM prevItem;

	while (item) {
		prevItem = m_treeCtrl.GetPrevSiblingItem(item);
		MoveTreeItem(item,beforeStartItem);
		if (item == sItem)
			break;
		item = prevItem;
	}
}

bool CTreeViewAnalyzer::IsAnalyzerFileDirty() 
{
	// Reinstating dirty analyzer flag.	// 06/24/03 AM.
//	return false;	// 06/24/03 AM.
	return m_dirtyFileFlag;	// 06/24/03 AM.
}

void CTreeViewAnalyzer::SetAnalyzerFileDirty(bool flag)	// 06/24/03 AM.
{
	m_dirtyFileFlag = flag;	// 06/24/03 AM.
}

CString CTreeViewAnalyzer::GetState()
{
	return StateString(&m_treeCtrl,m_treeCtrl.GetRootItem(),STATE_GET_EXPANDED);
}

void CTreeViewAnalyzer::SetState(CString stateStr)
{
	//SetRedraw(false);
	StateFromString(&m_treeCtrl,m_treeCtrl.GetRootItem(),STATE_EXPAND_COLLAPSE,stateStr);
	//SetRedraw(true);
}

CString CTreeViewAnalyzer::ArchiveAnalyzer()
{
	CString pathStr = RulesPath();
	return ArchiveFiles(pathStr,_T("Rules"),_T("*.*"));
}

void CTreeViewAnalyzer::OnSequencemenuViewlog() 
{
	CString logPathStr;
	if (GetLogFile(m_intPass,logPathStr))
		theApp.OpenNewDocument(_T("VisualText"),logPathStr);
}

void CTreeViewAnalyzer::OnSequencemenuServerarchives() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->ServerArchive(_T("Rules"));	
}

void CTreeViewAnalyzer::OnSequencemenuUploadrules() 
{
	CString localFileStr = ArchiveAnalyzer();
	if (localFileStr != _T("")) {
		CString remoteFileStr = StripPath(localFileStr,0);
		UploadArchive(_T("Rules"),remoteFileStr,localFileStr);
	}
}

void CTreeViewAnalyzer::OnSequencemenuArchiverules() 
{
	ArchiveAnalyzer();	
}

void CTreeViewAnalyzer::OnSequencemenuLocalarchives() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->LocalArchive(_T("Rules"));
}

void CTreeViewAnalyzer::ImageFromConcept(CONCEPT *concept, int &image, int &imageSelected,
										 ANAL_PASS_TYPE &type)
{
	type = ConceptType(concept);
	ImageFromType(type,m_boolActive,image,imageSelected);
}

void CTreeViewAnalyzer::ImageFromType(ANAL_PASS_TYPE type, bool activeFlag,
									  int &image, int &imageSelected)
{
	if (!activeFlag) {
		image = 14;
		imageSelected = 14;
		return;
	}

	switch (type) {
	case ANAL_PASS_PAT:
		image = 0;
		imageSelected = 1;
		break;

	case ANAL_PASS_STUB:
		image = 2;
		imageSelected = 3;
		break;

	case ANAL_PASS_END:
		image = 2;
		imageSelected = 3;
		break;

	case ANAL_PASS_REC:
		image = 4;
		imageSelected = 5;
		break;

	case ANAL_PASS_GROUP:
		image = 8;
		imageSelected = 9;
		break;

	case ANAL_PASS_SYSTEM:
		image = 6;
		imageSelected = 7;
		break;

	case ANAL_PASS_FOLDER:
		image = 10;
		imageSelected = 11;
		break;

	case ANAL_PASS_MAIN:
		image = 12;
		imageSelected = 13;
		break;
	}
}

void CTreeViewAnalyzer::ImageType(CString algoStr, int &image, int &imageSelected,
								  ANAL_PASS_TYPE &type)
{
	if (algoStr == _T("pat")) {
		image = 0;
		imageSelected = 1;
		type = ANAL_PASS_PAT;
	}
	else if (algoStr == _T("stub")) {
		image = 2;
		imageSelected = 3;
		type = ANAL_PASS_STUB;
	}
	else if (algoStr == _T("end")) {
		image = 2;
		imageSelected = 3;
		type = ANAL_PASS_END;
	}
	else if (algoStr == _T("rec")) {
		image = 4;
		imageSelected = 5;
		type = ANAL_PASS_REC;
	}
	else if (algoStr == _T("group")) {
		image = 8;
		imageSelected = 9;
		type = ANAL_PASS_GROUP;
	}
	else if (algoStr == _T("folder")) {
		image = 10;
		imageSelected = 11;
		type = ANAL_PASS_FOLDER;
	}
	else {
		image = 6;
		imageSelected = 7;
		type = ANAL_PASS_SYSTEM;
	}
}

ANAL_PASS_TYPE CTreeViewAnalyzer::PassType(CString algoStr) 
{
	if (algoStr == _T(""))
		algoStr = m_strAlgo;

	ANAL_PASS_TYPE type = ANAL_PASS_SYSTEM;
	CString aStr;

	for (int i=0; i < 1000; i++) {
		aStr = AlgoStrings[i];
		if (aStr == _T("xxxx"))
			break;
		else if (aStr.CompareNoCase(algoStr) == 0) {
			type = (ANAL_PASS_TYPE)i;
			break;
		}
	}

	return type;
}

// Variant function that requires algoStr to be specified.  For static use.	// 06/14/03 AM.
ANAL_PASS_TYPE CTreeViewAnalyzer::ConceptPassType(CString algoStr) 
{
	if (algoStr == _T(""))
		return ANAL_PASS_NONE;

	ANAL_PASS_TYPE type = ANAL_PASS_SYSTEM;
	CString aStr;

	for (int i=0; i < 1000; i++) {
		aStr = AlgoStrings[i];
		if (aStr == _T("xxxx"))
			break;
		else if (aStr.CompareNoCase(algoStr) == 0) {
			type = (ANAL_PASS_TYPE)i;
			break;
		}
	}

	return type;
}

CString CTreeViewAnalyzer::PassTypeStr(ANAL_PASS_TYPE type) 
{
	CString aStr;

	for (int i=0; i < 1000; i++) {
		aStr = AlgoStrings[i];
		if (type == (ANAL_PASS_TYPE)i)
			break;
	}

	return aStr;
}

void CTreeViewAnalyzer::OnUpdateSequencemenuDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CTreeViewAnalyzer::OnUpdateSequencemenuEditrules(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CTreeViewAnalyzer::OnUpdateSequencemenuViewlog(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CTreeViewAnalyzer::OnUpdateSequencemenuViewfulltree(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CTreeViewAnalyzer::OnUpdateSequencemenuArchiverules(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CTreeViewAnalyzer::OnUpdateSequencemenuUploadrules(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CTreeViewAnalyzer::OnRButtonDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint point, ptScreen;
	::GetCursorPos(&ptScreen);
	
	point = ptScreen;
	m_treeCtrl.ScreenToClient(&point);

	logmsg(_T("CTreeViewAnalyzer::OnRbuttonDown"));	// 05/14/08 AM.
	// TAKE CARE OF GRAYING ITEMS
	UINT disables[100];
	for (int i=0; i<100; i++)
		disables[i] = 0;
	int count = 0;
	int i = 0;

	if ((m_passType != ANAL_PASS_PAT && m_passType != ANAL_PASS_REC) // 05/09/07 AM.
		 || !theApp.m_pMainFrame->m_wndTextView.OpenProcessedFiles(_T("log"),false)) {
		disables[count++] = ID_SEQUENCEMENU_HIGHLIGHTMATCHES;
		disables[count++] = ID_SEQUENCEMENU_VIEWCONCEPTTREE;
		disables[count++] = ID_SEQUENCEMENU_VIEWTREEMATCHES;

		if (m_passType != ANAL_PASS_SYSTEM) {
			disables[count++] = ID_SEQUENCEMENU_VIEWLOG;
			disables[count++] = ID_SEQUENCEMENU_VIEWFULLTREE;
			disables[count++] = ID_SEQUENCEMENU_VIEWENTIRETREE;
		}
	}

	/*
	if (!CanMoveSelectedUp() && !CanMoveSelectedDown()) {
		disables[count++] = ID_SEQUENCEMENU_ADDEXISTINGRULES;
		disables[count++] = ID_SEQUENCEMENU_ADD_STUB;
		disables[count++] = ID_SEQUENCEMENU_DELETE;
		disables[count++] = ID_SEQUENCEMENU_DUPLICATE;
		disables[count++] = ID_SEQUENCEMENU_NEW;
	}
	*/

	if (m_boolInStub) {
		//disables[count++] = ID_SEQUENCEMENU_ADDEXISTINGRULES;
		//disables[count++] = ID_SEQUENCEMENU_ADD_STUB;
		disables[count++] = ID_SEQUENCEMENU_DUPLICATE;
		//disables[count++] = ID_SEQUENCEMENU_NEW;
		disables[count++] = ID_SEQUENCEMENU_ADD_TOPLEVEL_EXISTINGPASS;
		disables[count++] = ID_SEQUENCEMENU_ADD_TOPLEVEL_NEW;
		disables[count++] = ID_SEQUENCEMENU_DELETE;
	}
	else {
		int pass = SelectedPass();
		BOOL bDisallow = false;
		if (pass <= 1) {
			// pass == tokenize (1) or nothing selected (0)
			bDisallow = true;
		}
		else {
			if (2 == pass) {
				// If a system lines pass (2) is present, disallow!
				CString strPassName = CleanPassNameFromPass(2);
				if (_T("lines") == strPassName && GetPassType(2) == ANAL_PASS_SYSTEM)
					bDisallow = true;
			}
		}
		if (bDisallow) {
			disables[count++] = ID_SEQUENCEMENU_DELETE;
		}
	}

	
	if (m_passType != ANAL_PASS_PAT && m_passType != ANAL_PASS_REC &&
		m_passType != ANAL_PASS_FOLDER) {

		disables[count++] = ID_SEQUENCEMENU_PROPERTIES;
	}

	if (m_passType != ANAL_PASS_PAT && m_passType != ANAL_PASS_REC) {
		disables[count++] = ID_SEQUENCEMENU_EDITRULES;
	}

	if (m_treeCtrl.GetCount() == 0) {
		disables[count++] = ID_SEQUENCEMENU_DELETE;
		disables[count++] = ID_SEQUENCEMENU_DUPLICATE;
	}

	// ADD SPEC PASSES TO MENU

	ClientToScreen(&point);
	//PopupContextMenu(this,IDR_ANALYZER,point,disables);

	CMenu menu;
	menu.LoadMenu(IDR_ANALYZER);
	CMenu *context = menu.GetSubMenu(0);
	CMenu *add = context->GetSubMenu(0);
	CMenu *AddSpec = add->GetSubMenu(1);
	CMenu *top = add->GetSubMenu(5);
	CMenu *AddTopSpec = top->GetSubMenu(1);

	// ADD SPEC ITEMS
	size_t plen;

#ifdef _UNICODE
	wchar_t taiPath[500]; 
	_wgetenv_s( &plen, taiPath, 500, L"VISUALTEXT" ); 
#else
	_TCHAR *taiPath = NULL;
	_dupenv_s(&taiPath, &plen, _T("VISUALTEXT"));
#endif

	CString dirPathStr = taiPath;
	dirPathStr += _T("\\data\\spec");
	int pathLevel = CharCount(dirPathStr,'\\') + 1;
	CStringList files,dirFiles;
	DirectoryFiles(files,dirPathStr,true,_T(""),true);
	CString fileStr,fullPathStr,niceStr,nicePatsStr;
	int len = files.GetCount();
	int level;
	int lastLevel = -1;
	CMenu popups[20];
	CMenu popupsTop[20];
	bool created[20];
	for (i=0; i<20; i++) {
		popups[i].DestroyMenu();
		popupsTop[i].DestroyMenu();
		created[i] = false;
	}
	bool patsDir = false;

	for (i=0; i<len; i++) {
	    fullPathStr = NthStringListItem(files,i);
		level = CharCount(fullPathStr,'\\')-pathLevel;

		if (lastLevel >= 0 && lastLevel > level) {
			for (int j=lastLevel; j>=level; j--) {
				popups[j].DestroyMenu();
				popupsTop[j].DestroyMenu();
				created[j] = false;
			}
		}
		else if (patsDir && lastLevel+1 == level)
			continue;
		patsDir = false;

		fileStr = StripPath(fullPathStr,0);
		if (EndsWith(fileStr,_T(".pat")))
			ClipRight(fileStr,4);
		CString niceStr = SeparateByCaps(fileStr);

		if (niceStr != _T("") && IsDirectory(fullPathStr) && !EndsWith(fullPathStr,_T("_pats"))) {
			if (!created[level]) {
				popups[level].CreatePopupMenu();
				popupsTop[level].CreatePopupMenu();
				created[level] = true;
			}

			if (level == 0) {
				AddSpec->AppendMenu(MF_POPUP,(UINT)popups[level].GetSafeHmenu(),niceStr);
				AddTopSpec->AppendMenu(MF_POPUP,(UINT)popupsTop[level].GetSafeHmenu(),niceStr);
			}
			else {
				popups[level-1].AppendMenu(MF_POPUP,(UINT)popups[level].GetSafeHmenu(),niceStr);
				popupsTop[level-1].AppendMenu(MF_POPUP,(UINT)popupsTop[level].GetSafeHmenu(),niceStr);
			}
		}
		else if (niceStr != _T("")) {
			nicePatsStr = niceStr;
			if (EndsWith(fullPathStr,_T("_pats"))) {
				dirFiles.RemoveAll();
				DirectoryFiles(dirFiles,fullPathStr);
				ClipRight(niceStr,5);
				nicePatsStr.Format(_T("%s (%d)"),niceStr,dirFiles.GetCount());
				patsDir = true;
			}
			if (level == 0) {
				AddSpec->AppendMenu(MF_STRING,SPEC_STARTID + (UINT)i,nicePatsStr);
				AddTopSpec->AppendMenu(MF_STRING,SPEC_2_STARTID + (UINT)i,nicePatsStr);
			}
			else {
				popups[level-1].AppendMenu(MF_STRING,SPEC_STARTID + (UINT)i,nicePatsStr);
				popupsTop[level-1].AppendMenu(MF_STRING,SPEC_2_STARTID + (UINT)i,nicePatsStr);
			}
		}
		lastLevel = level;
	}
	AddSpec->DeleteMenu(0,MF_BYPOSITION);
	AddTopSpec->DeleteMenu(0,MF_BYPOSITION);

	PopupContextMenu(this,context,point,disables);
}

void CTreeViewAnalyzer::OnSequencemenuAddStub() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	if (wnd->AnalyzerCreated() && AskForText(_T("Enter stub name:")))
		theApp.m_pMainFrame->m_wndAnalyzerView.AddStub(appAskText);
}

void CTreeViewAnalyzer::AddStub(CString stubNameStr, bool addAfterFlag) 
{
	if (stubNameStr != _T("")) {
		//if (m_boolInStub && !addAfterFlag) {
			//AfxMessageBox("You cannot add stubs within stubs.");
		//} else
		if (StubAlreadyExists(stubNameStr)) {
			CString msgStr;
			msgStr.Format(_T("The Stub \"%s\" already exists!"),stubNameStr);
			AfxMessageBox(msgStr);
		}
		else {
			//SetRedraw(false);
			if (addAfterFlag && m_boolInStub) {
				HTREEITEM startStubItem,endStubItem;
				GetStubBoundingItems(m_itemSelected,startStubItem,endStubItem);
				m_treeCtrl.SelectItem(endStubItem);
			}
			InsertPass(_T("stub"),stubNameStr,_T("comment"),true);

			// PREVENT FROM THINKING THE SELECTION IS IN A STUB
			// THUS MESSING UP THE ADDING OF THE END STUB PASS
			m_boolInStub = false;

			InsertPass(_T("end"),stubNameStr,_T("comment"),true);

			CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
			if (wnd->GetActiveTab() == WORKSPACE_ANALYZER)
				theApp.m_pMainFrame->m_wndGrammarView.AddTopLevelConcept(stubNameStr,false);
			m_dirtyFileFlag = true;
			//SetRedraw(true);
		}
	}
}

bool CTreeViewAnalyzer::StubAlreadyExists(CString conceptStr) 
{
	if (conceptStr == _T(""))
		return false;

	HTREEITEM item = m_treeCtrl.GetRootItem();
	CString itemTextStr;
	int image;
	int selImage;

	while (item) {
		itemTextStr = CleanPassName(item);
		m_treeCtrl.GetItemImage(item,image,selImage);
		if (itemTextStr == conceptStr && image == 2)
			return true;
		item = m_treeCtrl.GetNextSiblingItem(item);
	}
	return false;
}

void CTreeViewAnalyzer::OnSequencemenuHighlightmatches() 
{
	HighlightMatches(m_intPass);
	appBoolHighlightText = 1;
}

void CTreeViewAnalyzer::HighlightMatches() 
{
	HighlightMatches(m_intPass);
}

void CTreeViewAnalyzer::HighlightMatches(int pass) 
{
	CString logPathStr;
	if (GetLogFile(pass,logPathStr,true)) {
		AfxGetApp()->BeginWaitCursor();
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		CStringList selList;
		HighlightRuleMatches(selList,logPathStr);
		wnd->HighlightMatchesSelections(selList);
		AfxGetApp()->EndWaitCursor();
	}
}

void CTreeViewAnalyzer::HighlightRuleMatches(CStringList &selList, CString logPathStr, CString conceptStr) 
{
	bool firstFound = false;

	CString cStr = NLPReadFile(logPathStr);

	CString rulesPathStr;
	if (conceptStr == _T("")) {
		int len = logPathStr.GetLength();
		CString numStr = logPathStr.Mid(len-7,3);
		int pass = _ttoi((const _TCHAR *)numStr);
		rulesPathStr = PassFileFromNumber(pass);
	}
	else
		rulesPathStr = RulesPath(conceptStr);

	CString ruleTextStr = _T("");
	if (rulesPathStr != _T(""))
		ruleTextStr = NLPReadFile(rulesPathStr);
		//ruleTextStr = NLPReadFile(InputPath(rulesPathStr));

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString textStr = NLPReadInputFile(InputPath(wnd->GetProcessText()));
	CString lineStr;
	CString conStr;
	CString rangeStr;
	CString semStr;
	CString semVars;	// OPT.	// 12/01/06 AM.
	CString dataStr;
	bool firedFlag;
	bool builtFlag;
	int indent = 0;
	int lineCount = 0;
	long startSel = 0;
	long endSel = 0;
	long lastStart = -1;
	long lastEnd = -1;
	int passNum = 0;
	int lineNum = 0;
    
	CStringList parseList;
	CString selStr,colorStr,lastColorStr;
	CString inbetweenStr;
	bool rootFlag = false;
	bool forceSameColorFlag = false;

	int len = cStr.GetLength();
	int lineIndex = 0;
	int lineIndex2 = 0;

	while (lineIndex2 >= 0) {
		lineNum++;
		lineIndex2 = cStr.Find(_T("\n"), lineIndex);
		if (lineIndex2 == -1)
			lineStr = cStr.Mid(lineIndex);
		else
			lineStr = cStr.Mid(lineIndex, lineIndex2 - lineIndex);
		lineIndex = lineIndex2 + 1;

		if (!lineStr || lineStr == _T(""))
			continue;

		// MUST SKIP PAST HEADER!!!
		if (!rootFlag)
			rootFlag = lineStr.Find(_T("_ROOT")) >= 0 ? true : false;
		if (!rootFlag)
			continue;

		ParseLogLine(ruleTextStr,lineStr,lineCount,startSel,endSel,passNum,lineNum,
			indent,conStr,semStr,semVars,rangeStr,firedFlag,builtFlag,
				false	// Not fast run.	// FIX.	// 03/26/07 AM.
				);

		if (conStr.GetAt(0) != '_')
			forceSameColorFlag = true;

		if ((firedFlag || builtFlag) && startSel > lastEnd) {
			inbetweenStr = _T("");
			int len = textStr.GetLength();
			if (startSel - lastEnd > 1 && startSel < len) {
				inbetweenStr = textStr.Mid(lastEnd+1,startSel-lastEnd-1);
				TrimString(inbetweenStr);
			}
			conceptStr = NthStringListItem(parseList,0);
			if (builtFlag) {
				if (inbetweenStr.IsEmpty() && lastColorStr == _T("blue"))
					colorStr = _T("dark blue");
				else
					colorStr = _T("blue");
			}
			else {	// firedFlag
				if (inbetweenStr.IsEmpty() && lastColorStr == _T("green"))
					colorStr = _T("dark green");
				else
					colorStr = _T("green");
			}
			selStr.Format(_T("%d,%d,%s"),startSel,endSel,colorStr);
			selList.AddTail(selStr);
			lastEnd = endSel;
			lastColorStr = colorStr;
			forceSameColorFlag = false;
		}
	}
}

void CTreeViewAnalyzer::HighlightMatches(CONCEPT *concept) 
{
	HighlightMatches(SelectPassFromConcept(concept));
}

int CTreeViewAnalyzer::SelectPassFromConcept(CONCEPT *concept) 
{
	CString debugStr = ConceptName(concept);
	HTREEITEM item = TreeItemFromConcept(concept);
	int passNumber = 0;

	if (item) {
		m_treeCtrl.SelectItem(TreeItemFromConcept(concept));
		passNumber = KBPassNumber(concept);
	}

	return passNumber;
}

void CTreeViewAnalyzer::OnSequencemenuViewentiretree() 
{
	CString logPathStr = GetFinalLogFile();
	if (logPathStr != _T("")) {
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		wnd->OpenParseTree(_T(""),logPathStr);
	}
}

bool CTreeViewAnalyzer::CanMoveSelectedUp() 
{
	if (m_intPass < 3)
		return false;

	// CHECK FOR STUB MOVEMENT
	if (m_boolInStub) {
		HTREEITEM startItem,endItem,firstItem;
		GetStubBoundingItems(m_itemSelected,startItem,endItem);
		firstItem = TreeCtrlFirstSibling(&m_treeCtrl,startItem);
		if (firstItem == startItem)
			return false;
	}

	// DEFAULT CHECK
	HTREEITEM firstSibling = TreeCtrlFirstSibling(&m_treeCtrl,m_itemSelected);
	return firstSibling != m_itemSelected;
}

bool CTreeViewAnalyzer::CanMoveSelectedDown() 
{
	if (m_intPass == 1)
		return false;

	// CHECK FOR STUB MOVEMENT
	if (m_boolInStub) {
		HTREEITEM startItem,endItem,lastItem;
		GetStubBoundingItems(m_itemSelected,startItem,endItem);
		lastItem = TreeCtrlLastSibling(&m_treeCtrl,endItem);
		if (lastItem == endItem)
			return false;
	}

	bool canMove = m_treeCtrl.GetNextSiblingItem(m_itemSelected) ? true : false;
	return canMove;
}

int CTreeViewAnalyzer::SelectedPass() 
{
	return m_intPass;
}

void CTreeViewAnalyzer::OnSequencemenuProperties() 
{
	SequenceProperties();	
}

bool CTreeViewAnalyzer::SequenceProperties(CONCEPT *concept) 
{
	if (!concept)
		concept = m_conceptSelected;
	CPassPropertiesDlg dlg;
	dlg.SetData(concept);
	dlg.DoModal();
	if (appButtonReturn)
		UpdateKB();
	return appButtonReturn ? true : false;
}

void CTreeViewAnalyzer::UpdateKB()
{
	CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
	if (pView)
		pView->Restore();
}

void CTreeViewAnalyzer::OnTimer(UINT nIDEvent) 
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
					int nCount = m_treeCtrl.GetVisibleCount();
					for (int i=0; i<nCount-1; ++i)
						hitem = m_treeCtrl.GetNextVisibleItem(hitem);
					//if (hitem)
						//pTree->SelectDropTarget(hitem);
					m_hitemDrop = hitem;
					CImageList::DragShowNolock(true);
				}
			}
		}
		break;
	}

	CPaneView::OnTimer(nIDEvent);
}

void CTreeViewAnalyzer::OnSequencemenuViewtreematches() 
{
	CString logPathStr;
	CStringList selList;

	if (GetLogFile(m_intPass,logPathStr)) {
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		HighlightRuleMatches(selList,logPathStr);
		wnd->OpenParseTree(m_strData,logPathStr,selList);
	}
}

void CTreeViewAnalyzer::OnSequencemenuViewconcepttree() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->OpenParseTree(true);
}

void CTreeViewAnalyzer::OnChooseSpec(UINT idIn) 
{
	bool closePrevious = !appBoolAnalyzerLoaded ? 0 : 1;

	int adjust = 13868;  // THIS IS THE LOWORD OF SAMPLE_ID_START
	                    // Makes it 1-numbered for since UINT can't be -1

	int analyzerMenuNum = idIn - adjust;

	// ADD SPEC ITEMS
	size_t len;

#ifdef _UNICODE
	wchar_t taiPath[500]; 
	_wgetenv_s( &len, taiPath, 500, L"VISUALTEXT" ); 
#else
	_TCHAR *taiPath = NULL;
	_dupenv_s(&taiPath, &len, _T("VISUALTEXT"));
#endif

	CString dirPathStr = taiPath;
	dirPathStr += _T("\\data\\spec");
	CStringList files;
	DirectoryFiles(files,dirPathStr,true,_T(""),true);

	CString fileStr = NthStringListItem(files,analyzerMenuNum);

	AddExistingPass(fileStr);
}

void CTreeViewAnalyzer::OnChooseSpec2(UINT idIn) 
{
	bool closePrevious = !appBoolAnalyzerLoaded ? 0 : 1;

	int adjust = 13968;  // THIS IS THE LOWORD OF SAMPLE_ID_START
	                    // Makes it 1-numbered for since UINT can't be -1

	int analyzerMenuNum = idIn - adjust;

	// ADD SPEC ITEMS
	size_t len;

#ifdef _UNICODE
	wchar_t taiPath[500]; 
	_wgetenv_s( &len, taiPath, 500, L"VISUALTEXT" ); 
#else
	_TCHAR *taiPath = NULL;
	_dupenv_s(&taiPath, &len, _T("VISUALTEXT"));
#endif

	CString dirPathStr = taiPath;
	dirPathStr += _T("\\data\\spec");
	CStringList files;
	DirectoryFiles(files,dirPathStr,true,_T(""),true);

	CString fileStr = NthStringListItem(files,analyzerMenuNum);

	m_boolTopLevel = true;
	AddExistingPass(fileStr);
	m_boolTopLevel = false;
}

void CTreeViewAnalyzer::OnSequencemenuAddFolder()
{
	AddFolder();
}

void CTreeViewAnalyzer::AddFolder(CString folderNameStr)
{
	bool prevDirtyFlag = m_dirtyFlag;

	// ADD FOLDER TO DISPLAY TREE
	CString nameStr = folderNameStr;
	if (nameStr.IsEmpty())
		nameStr = _T("unamed");
	CONCEPT *folderConcept = InsertPass(_T("folder"),nameStr,_T("0"),true);	// 08/08/03 AM.

	if (folderNameStr.IsEmpty() && !SequenceProperties(folderConcept)) {
		m_dirtyFlag = prevDirtyFlag;
		DeleteSelected(true);
	}
}

void CTreeViewAnalyzer::OnSequencemenuAddMain()
{
	AddMain(m_conceptSpec,m_itemParent);
}

void CTreeViewAnalyzer::AddMain(CONCEPT *conceptParent, HTREEITEM itemParent)
{
	if (!conceptParent)
		conceptParent = m_conceptSpec;

	// ADD MAIN TO KB
	_TCHAR nameBuff[1001];
	StringToChar(nameBuff,MAINSTR,1000);
	CONCEPT *mainConcept = cg->findConcept(conceptParent,nameBuff);

	if (!mainConcept) {
		// ADD TO KB
		mainConcept = cg->makeConcept(conceptParent,MAINSTR,1);
//		kbDirty = true;
		KBReplaceValue(mainConcept,_T("algo"),_T("pat"),false);
		KBReplaceValue(mainConcept,_T("data"),MAINSTR,false);
		
		// ADD MAIN TO DISPLAY TREE
		HTREEITEM item;
		bool wasEmpty = m_treeCtrl.GetChildItem(itemParent) ? false : true;
		item = m_treeCtrl.InsertItem(MAINSTR,12,13,itemParent,TVI_FIRST);
		m_treeCtrl.SetItemData(item,(DWORD)mainConcept);
		if (wasEmpty)
			m_treeCtrl.Expand(m_itemParent,TVE_EXPAND);

		m_treeCtrl.SelectItem(item);

		// ADD TO OLD SYSTEM
		if (conceptParent == m_conceptSpec) {
			_TCHAR algoBuff[1001];
			_TCHAR dataBuff[1001];
			_TCHAR commentBuff[1001];
			StringToChar(algoBuff,_T("folder"),1000);
			StringToChar(dataBuff,MAINSTR,1000);
			StringToChar(commentBuff,MAINSTR,1000);
			APASS *newPass = Aseq::makePass(nlp,algoBuff,dataBuff,commentBuff,true);
			Aseq::insertPass(nlp,newPass,0);
		}
	}
	else
		AfxMessageBox(_T("Main already exists at this level!"));
}

bool CTreeViewAnalyzer::CanDelete()
{
	bool canDeleteFlag = true;

	if (m_passType == ANAL_PASS_STUB || m_passType == ANAL_PASS_END || InStub())
		canDeleteFlag = false;

	return canDeleteFlag;
}

void CTreeViewAnalyzer::OnSequencemenuAddToplevelExistingpass() 
{
	m_boolTopLevel = true;
	AddExistingPass();
	m_boolTopLevel = false;
}

void CTreeViewAnalyzer::OnSequencemenuAddToplevelMain() 
{
	AddMain(m_conceptSpec,NULL);
}

void CTreeViewAnalyzer::OnSequencemenuAddToplevelNew() 
{
	m_boolTopLevel = true;
	OnSequencemenuNew();
	m_boolTopLevel = false;
}

void CTreeViewAnalyzer::OnSequencemenuAddToplevelFolder() 
{
	m_boolTopLevel = true;
	AddFolder();	
	m_boolTopLevel = false;
}

CString CTreeViewAnalyzer::LogFileRule(CString textPathStr, long start, long end)
{
	CString ruleStr = _T("");

	if (start >= end)
		return ruleStr;

	bool firstFound = false;
	CString logPathStr;
	GetLogFile(logPathStr);

	CString cStr = NLPReadFile(logPathStr);
	CString textStr = _T("");
	if (textPathStr != _T(""))
		textStr = NLPReadFile(InputPath(textPathStr));

	int indent = 0;
	int lastIndent = 0;
	bool indentChanged = false;
	int subIndentMax = 10000;
	int subIndent = subIndentMax;
	CString lineStr;
	CString conStr;
	CString rangeStr;
	CString semStr;
	CString semVars;	// OPT.	// 12/01/06 AM.
	CString dataStr;
	bool inside = false;
	bool inRange = false;
	bool firstInRange = false;
	bool firedFlag,builtFlag;
	int lastOne = false;
	long startSel = 0;
	long endSel = 0;
	int passNum = 0;
	int lineNum = 0;
	int ruleIndent = 0;

    int iItem = 0;
	int level = 0;
	int lineCount = 0;
	int ruleItemCount = 0;
    
	CStringList concepts;
	CStringList indents;
	CString indentStr;

	int len = cStr.GetLength();
	int lineIndex = 0;
	int lineIndex2 = 0;

	while (lineIndex2 >= 0) {
		lineNum++;
		lineIndex2 = cStr.Find(_T("\n"), lineIndex);
		if (lineIndex2 == -1)
			lineStr = cStr.Mid(lineIndex);
		else
			lineStr = cStr.Mid(lineIndex, lineIndex2 - lineIndex);
		lineIndex = lineIndex2 + 1;

		if (!lineStr || lineStr == _T(""))
			continue;

		// MUST SKIP PAST HEADER!!!
		if (!inside && lineStr.Find(_T("_ROOT")) != 0)
			continue;
		inside = true;

		// PARSE THE LOG LINE
		indentChanged = lastIndent != indent ? true : false;
		lastIndent = indent;
		ParseLogLine(textStr,lineStr,lineCount,startSel,endSel,passNum,lineNum,
			indent,conStr,semStr,semVars,rangeStr,firedFlag,builtFlag);

		//PeekHere(10);

		// CHECK TO SEE IF LINE IS WITHIN START AND END SELECTION
		if (inRange && (!firstInRange || !indentChanged) && endSel > end)
			break;
		else if (!inRange && startSel >= start) {
			inRange = true;
			ruleIndent = indent;
			firstInRange = true;
			lastIndent = 0;
		}
		else if (!inRange)
			continue;

		// USED TO MOVE DOWN FIRST NESTED NODE
		if (firstInRange && indent < lastIndent)
			firstInRange = false;

		if (!firstInRange && indent < subIndent)
			subIndent = indent;

		concepts.AddTail(conStr);
		indentStr.Format(_T("%d"),indent);
		indents.AddTail(indentStr);
	}

	POSITION pos = concepts.GetHeadPosition();
	POSITION posIndent = indents.GetHeadPosition();
	ruleStr = _T("@RULES\n_xNIL <-\n");
	if (subIndent == subIndentMax)
		subIndent = indent;

	while (pos && (conStr = concepts.GetNext(pos))) {
		indentStr = indents.GetNext(posIndent);
		indent = _ttoi((const _TCHAR *)indentStr);

		// ADD LINE FOR RULE
		if (indent == subIndent) {
#ifdef UNICODE
			if (conStr.GetAt(0) < 127 && conStr.GetLength() == 1 && !IsDigit(conStr) && !IsAlpha(conStr))
#else
			if (conStr.GetLength() == 1 && !IsDigit(conStr) && !IsAlpha(conStr))
#endif
				conStr = _T("\\") + conStr;
			if (conStr == _T("\\_"))
				lineStr.Format(_T("\t_xWHITE [s star]\t### (%d)\n"),++ruleItemCount);
			else
				lineStr.Format(_T("\t%s [s]\t### (%d)\n"),conStr,++ruleItemCount);
			ruleStr += lineStr;
		}
	}

	ruleStr += _T("\t@@");

	return ruleStr;
}

void CTreeViewAnalyzer::AppendToRuleFile(CString ruleStr)
{
	CString rulesPathStr = PassPathStr();
	if (rulesPathStr != _T("") && FileExists(rulesPathStr)) {
		CSyntaxEditorDoc *doc = (CSyntaxEditorDoc *)AfxGetApp()->OpenDocumentFile(rulesPathStr);
		CSyntaxEditorView *view = (CSyntaxEditorView *)GetFirstView(doc);
		view->AppendToText(ruleStr,2);
	}
}

void CTreeViewAnalyzer::DeleteStubByName(CString stubNameStr)
{
	SelectItemFromStubName(stubNameStr);
	DeleteStub();
}

HTREEITEM CTreeViewAnalyzer::SelectItemFromStubName(CString stubNameStr)
{
	int i = 0;
	HTREEITEM item = m_treeCtrl.GetRootItem();
	CString passNameStr;
	CString lowerStubNameStr = stubNameStr;
	lowerStubNameStr.MakeLower();
	
	while (item) {
		passNameStr = CleanPassName(item);

		if (passNameStr.Find(_T("stub_")) == 0) {
			ClipLeft(passNameStr,5);
			passNameStr.MakeLower();
			if (passNameStr == lowerStubNameStr) {
				m_treeCtrl.SelectItem(item);
				m_treeCtrl.EnsureVisible(item);
				return item;
			}
		}
		item = m_treeCtrl.GetNextSiblingItem(item);
		i++;
	}

	return NULL;
}

CString CTreeViewAnalyzer::PassFileFromNumber(int passNum)
{
	int pass = 0;
	CONCEPT *c = cg->Down(m_conceptSpec);
	ANAL_PASS_TYPE type = ANAL_PASS_NONE;

	while (c) {
		pass = KBPassNumber(c);
		if (pass == passNum)
			break;
		c = cg->Next(c);
	}
	return PassPathStr(c);
}


////////////////////////////////////////
void ANLZ_GetStrTime(CString& strTime, DWORD ms)
{
	DWORD sec = ms / 1000L;
	ms = ms % 1000L;
	if (sec > 0)
		strTime.Format(_T("%d sec, %d msec"), sec, ms);
	else
		strTime.Format(_T("%d msec"), ms);
}

// Thread-controlling function.
UINT Analyze(LPVOID pParam)
{
//	char buf[1001];	// 10/29/02 AM.
	CTreeViewAnalyzer *pView = &(theApp.m_pMainFrame->m_wndAnalyzerView);

	if (pView == NULL) return 0;

	void* parse = 0;
	DWORD suspendCount = 0;
	DWORD dwTimeStartRun;
//	logmsg("Analyze:");

	while (true) {
//		logmsg("Analyze: while loop.");
		BOOL bDone = pView->m_AnalyzeFiles.GetSize() <= pView->m_AnalyzeFileIndex;
		if (bDone || pView->m_fSuspendAnalyzeThread) {
			CString strTime;
			ANLZ_GetStrTime(strTime, GetTickCount() - dwTimeStartRun);
			pView->m_AnalyzeOutputFinalMsgStr.Format(_T("Analysis done (%s)"), strTime);
			pView->PostMessage(WMU_ANALYZE_CLEANUP, bDone);
//			logmsg("Analyze: (analysis done,calling cleanup) suspending1");

			pView->PostMessage(WMU_POST_ANALYZE_HIGHLIGHT, bDone);
				
			suspendCount = pView->m_AnalyzeThread->SuspendThread();
//			sprintf_s(buf,"End suspend1. maxdword=%ld,suspendcount=%ld",
//					MAXDWORD,suspendCount);
//         logmsg(buf);


			if (MAXDWORD == suspendCount)
				goto _endThread;

		}

		if (pView->m_fTerminateAnalyzeThread) {
_endThread:
				

//            logmsg("Ending thread.");	// 10/29/02 AM.
			if (parse) {
				nlp->apiCleanAnalyze(parse);
				parse = 0;
				pView->PostMessage(WMU_ANALYZE_CLEANUP, bDone);
			}
			AfxEndThread(0);
//            logmsg("Ended thread.");	// 10/29/02 AM.
			return 0;
		}

		_TCHAR appDir[1024];
		_TCHAR infile[1024];
		_TCHAR outfile[1024];
		_TCHAR buff[1024];
		_TCHAR *popupType;
		_TCHAR *popupMessage;
		CString textStr;
		bool response = false;
		_tcscpy_s(appDir,AppPath());
		_tcscpy_s(infile, pView->m_AnalyzeFiles[pView->m_AnalyzeFileIndex]);
		_tcscpy_s(outfile, OutputPath(_T("output.txt")));

		nlp->setFbatchstart(0 == pView->m_AnalyzeFileIndex);
		if (0 == pView->m_AnalyzeFileIndex)
			pView->m_dwTimeStartFile = dwTimeStartRun = GetTickCount();
		else
			pView->m_dwTimeStartFile = GetTickCount();
 		if (parse = nlp->apiInitAnalyze(infile, outfile, appDir, appBoolLogAnalyzer)) {
//			m_OkAnalysis = true;	// 06/19/03 AM.
			pView->PostMessage(WMU_ANALYZE_OUTPUT_MESSAGE, pView->m_AnalyzeFileIndex);
//			logmsg("Analyze: StepAnalyze.");
			while (nlp->apiStepAnalyze(parse) && !pView->m_fSuspendAnalyzeThread && !pView->m_fTerminateAnalyzeThread) {
//				logmsg("Analyze: In step loop.");
				if (appBoolVerbose)
					pView->PostMessage(WMU_ANALYZE_OUTPUT_PASS_MESSAGE, pView->m_AnalyzePass++);

				// HANDLE POPUP BOXES
				popupType = nlp->getPopuptyp();

				if (popupType) {
					response = false;
					popupMessage = nlp->getPopupmsg();
					if (_tcscmp(popupType,_T("askfortext")) == 0)
						response = AskForText(popupMessage);
					else if (_tcscmp(popupType,_T("yesno")) == 0)
						response = AfxMessageBox(popupMessage,MB_YESNO) == IDYES ? true: false;
					else
						AfxMessageBox(popupMessage);
					if (response) {
						StringToChar(buff,appAskText,1023);
						nlp->apiSetpopupdata(buff);
					}
					nlp->setPopupmsg(0);
					nlp->setPopuptyp(0);
				}
			}
			// PULLED CODE OUT OF HERE.	// 06/19/03 AM.
			nlp->apiCleanAnalyze(parse);
//            logmsg("Done apiCleanAnalyze.");	// 10/29/02 AM.

			parse = 0;
			// Move thread suspend from here.	// 09/23/07 AM.

		}
		else {	// Couldn't parse current file.	// Continue on.	// 06/19/03 AM.
//			m_OkAnalysis = false;	// 06/19/03 AM.
			// Possible reason is an empty input file.
			// May want to flag fatal init errors also.
		}

		// Move thread suspend to here.	// 09/23/07 AM.
		if (!pView->m_fSuspendAnalyzeThread && !pView->m_fTerminateAnalyzeThread) {
			pView->PostMessage(WMU_POST_ANALYZE_FILE_TASKS, pView->m_AnalyzeFileIndex);
			pView->m_AnalyzeFileIndex++;
			// Suspend at this point to allow the Post Analyze File Tasks to complete!
			// Must maintain synchronicity because files must be moved out of output area
			// in order to get ready for next file to be analyzed.
//				logmsg("Analyze: (post analyze tasks) suspending2");
			suspendCount = pView->m_AnalyzeThread->SuspendThread();
			if (MAXDWORD == suspendCount) {
//					logmsg("Going to end thread.");	// 10/29/02 AM.
				goto _endThread;
			}
			else {	// 10/29/02 AM.
					_TCHAR buf[1001];	// 09/20/10 AM.
#ifdef UNICODE
					wsprintf(buf,_T("Completing thread. maxdword=%ld,suspendcount=%ld"),
							MAXDWORD,suspendCount);
#else
					sprintf_s(buf,_T("Completing thread. maxdword=%ld,suspendcount=%ld"),
							MAXDWORD,suspendCount);
#endif
                    logmsg(buf);	// 10/29/02 AM.
			}

		}

		// AND MOVED IT HERE.	// 06/19/03 AM.

	}

}

void CTreeViewAnalyzer::EndAnalyzeThread(void)
{
//logmsg("EndAnalyzeThread:");
	if (m_AnalyzeThread)
	{
		m_fTerminateAnalyzeThread = true;
		m_AnalyzeThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
		m_AnalyzeThread->ResumeThread();

		// wait for the thread to end
		WaitForSingleObject(m_AnalyzeThreadID, INFINITE);
		
		m_AnalyzeThreadID = 0;
		m_fTerminateAnalyzeThread = false;
		m_fSuspendAnalyzeThread = false;
		m_AnalyzeThread = NULL;

		m_AnalyzeFileIndex = 0;
		m_AnalyzePass = 0;
		m_AnalyzeFiles.RemoveAll();
		m_AnalyzeOutputStrFilesTime.RemoveAll();
		for (int n = 0; n < 2; n++) {
			m_AnalyzeOutputStrDirs[n].RemoveAll();
			m_AnalyzeOutputStrFiles[n].RemoveAll();
		}
	}
}

void CTreeViewAnalyzer::DispatchAnalyzeThread(void)
{
//	logmsg("DispatchAnalyzeThread:");
	DWORD suspendCount = 0;	// 11/01/02 AM.
	if (m_AnalyzeThread) {
//		logmsg("DispatchAnalyzeThread: ResumeThread");
		// Controlling thread resumption more carefully.	// 11/01/02 AM.
//		m_AnalyzeThread->ResumeThread();
		for (;;)	// Forever.	// 11/01/02 AM.
			{
			switch (suspendCount = m_AnalyzeThread->ResumeThread())
				{
				case 1:	// Thread had been suspended and is now resumed.
					return;	// OK!
				case 0:	// Thread is not yet suspended.
					break;	// Wait for it to be suspended.
				case MAXDWORD:
					logmsg(_T("ERROR: DispatchAnalyzeThread: Couldn't resume thread"));
					return;
				default:	// Suspended and remains suspended.
					logmsg(_T("ERROR: DispatchAnalyzeThread: Thread still suspended."));
					return;
				}
			}
	}
	else {
//		logmsg("DispatchAnalyzeThread: AfxBeginThread");
		m_AnalyzeThread = AfxBeginThread(Analyze, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		m_AnalyzeThreadID = m_AnalyzeThread->m_hThread;
		m_AnalyzeThread->ResumeThread();
	}
}

////////////////////////////////////////

void CTreeViewAnalyzer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (VK_DELETE == nChar && m_itemSelected && !m_boolInStub &&
		m_treeCtrl.GetCount() > 0) {
		PostMessage(WM_COMMAND,ID_SEQUENCEMENU_DELETE);
	}
	CPaneView::OnKeyDown(nChar, nRepCnt, nFlags);
}


