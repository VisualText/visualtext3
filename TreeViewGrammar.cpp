/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// TreeViewGrammar.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "resource.h"
#include "Utils.h"

#include "MainFrm.h"
#include "VisualTextDoc.h"
#include "FTPClient.h"
#include "GrammarConceptDlg.h"
#include "ArchiveListDialog.h"
#include "VisualTextView.h"
#include "VisualTextPreferences.h"
#include "TreeViewGrammar.h"
#include "AttributeEditor.h"
#include "TreeViewAnalyzer.h"

extern CG *cg;
extern NLP *nlp;
extern GUI *gui;
extern bool kbDirty;
extern bool appBoolAnalyzerLoaded;
extern bool appBoolHighlightText;
extern bool appBoolVerbose;
extern VT_APP_STATE appState;
extern int appButtonReturn;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString appAskText;
extern CVisualTextApp theApp;
extern CString appNameStr;

RUG *rug = NULL;

#define ID_TIMER_GEN 200
#define ID_TIMER_DRAG 201
/////////////////////////////////////////////////////////////////////////////
// CTreeViewGrammar

IMPLEMENT_DYNCREATE(CTreeViewGrammar, CPaneView)

CTreeViewGrammar::CTreeViewGrammar()
{
	gram = NULL;  // Should never change after init!
	              // And since InitVars is used other places,
				  // don't move this into InitVars - Dd

	m_nToolBarID = IDR_GRAMMAR_TOOLBAR;

	InitVars();
	m_nTimerIDGen = 0;
	m_nTimerIDDrag = 0;
}

void CTreeViewGrammar::InitVars()
{
	m_itemSelected = NULL;
	m_sampleSelectedItem = NULL;
	m_conceptSelected = NULL;
	m_sampleSelectedConcept = NULL;
	m_boolSampleSiblings = false;
	m_GrammarNodeType = GRAM_NODE_NONE;
	
	rug = NULL;

	m_strConcept = _T("");
	m_bLDragging = false;
	m_boolGenerate = false;
	m_intItemCount = 0;

	m_noDropCursor = AfxGetApp()->LoadStandardCursor(IDC_NO) ;
	m_dropCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW) ;
}

CTreeViewGrammar::~CTreeViewGrammar()
{
	if (m_nTimerIDGen) {
		KillTimer(m_nTimerIDGen);
		m_nTimerIDGen = 0;
	}
	if (m_nTimerIDDrag) {
		KillTimer(m_nTimerIDDrag);
		m_nTimerIDDrag = 0;
	}
	delete rug;
}

BOOL CTreeViewGrammar::Create(CWnd* pParentWnd)
{
	if (!m_treeCtrl.Create( WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
		CRect(0,0,0,0), this, 0xFF ))
	{
		TRACE0( "Unable to create tree control.\n" );
		return NULL;
	}

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

BEGIN_MESSAGE_MAP(CTreeViewGrammar, CPaneView)
	//{{AFX_MSG_MAP(CTreeViewGrammar)
	ON_WM_CREATE()
	//ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY(TVN_SELCHANGED, AFX_IDW_PANE_FIRST, OnSelchanged)
	ON_COMMAND(ID_GRAMMAR_ADDCONCEPT, OnGrammarAddconcept)
	ON_COMMAND(ID_GRAMMAR_ADDTOPCONCEPT, OnGrammarAddtopconcept)
	ON_COMMAND(ID_GRAMMAR_EDITEXAMPLES, OnGrammarEditexamples)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_COMMAND(ID_GRAMMAR_DELETECONCEPT, OnGrammarDeleteconcept)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_GRAMMAR_HELP, OnGrammarHelp)
	ON_COMMAND(ID_GRAMMAR_SERVERARCHIVES, OnGrammarDownloadgrammar)
	ON_COMMAND(ID_GRAMMAR_UPLOADGRAMMAR, OnGrammarUploadgrammar)
	ON_COMMAND(ID_GRAMMAR_LOCALARCHIVE, OnGrammarLocalarchive)
	ON_COMMAND(ID_GRAMMAR_ARCHIVEGRAMMAR, OnGrammarArchivegrammar)
	ON_COMMAND(ID_GRAMMAR_EDITRULES, OnGrammarEditrules)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_GRAMMAR_EDITATTRIBUTES, OnGrammarEditattributes)
	ON_UPDATE_COMMAND_UI(ID_GRAMMAR_ADDCONCEPT, OnUpdateGrammarAddconcept)
	ON_UPDATE_COMMAND_UI(ID_GRAMMAR_DELETECONCEPT, OnUpdateGrammarDeleteconcept)
	ON_UPDATE_COMMAND_UI(ID_GRAMMAR_ARCHIVEGRAMMAR, OnUpdateGrammarArchivegrammar)
	ON_UPDATE_COMMAND_UI(ID_GRAMMAR_UPLOADGRAMMAR, OnUpdateGrammarUploadgrammar)
	ON_UPDATE_COMMAND_UI(ID_GRAMMAR_EDITATTRIBUTES, OnUpdateGrammarEditattributes)
	ON_COMMAND(ID_GRAMMAR_PROPERTIES, OnGrammarProperties)
	ON_COMMAND(ID_GRAMMAR_DELETECHILDREN, OnGrammarDeletechildren)
	ON_COMMAND(ID_GRAMMAR_ADDLIST, OnGrammarAddlist)
	ON_COMMAND(ID_GRAMMAR_DELETESAMPLES, OnGrammarDeletesamples)
	ON_COMMAND(ID_GRAMMAR_HIGHLIGHTMATCHES, OnGrammarHighlightmatches)
	ON_COMMAND(ID_GRAMMAR_VIEWTREE, OnGrammarViewtree)
	ON_COMMAND(ID_GRAMMAR_MARKFORGENERATION, OnGrammarMarkforgeneration)
	ON_COMMAND(ID_GRAMMAR_DISASSOCIATEFILE, OnGrammarDisassociatefile)
	ON_COMMAND(ID_GRAMMAR_FIND, OnGrammarFind)
	ON_COMMAND(ID_GRAMMAR_ADD_STUB, OnGrammarAddstub)
	ON_COMMAND(ID_GRAMMAR_SAMPLEFILE_CONSOLIDATE, OnGrammarSamplefileConsolidate)
	ON_COMMAND(ID_GRAMMAR_SAMPLEFILE_OPEN, OnGrammarSamplefileOpen)
	ON_COMMAND(ID_GRAMMAR_SAMPLEFILE_SET, OnGrammarSamplefileSet)
	ON_COMMAND(ID_GRAMMAR_SAMPLEFILE_UPDATE, OnGrammarSamplefileUpdate)
	ON_COMMAND(ID_GRAMMAR_ADD_FOLDER, OnGrammarAddFolder)
	ON_COMMAND(ID_GRAMMAR_ADD_LABEL, OnGrammarAddLabel)
	ON_COMMAND(ID_GRAMMAR_ADD_RULE, OnGrammarAddRule)
	ON_COMMAND(ID_GRAMMAR_SAMPLEFILE_EXPORT, OnGrammarSamplefileExport)
	ON_COMMAND(ID_GRAMMAR_SAMPLEFILE_IMPORT, OnGrammarSamplefileImport)

	ON_COMMAND(ID_DELETE, OnDelete)
	ON_COMMAND(ID_TREE_UP, OnMoveUp)
	ON_COMMAND(ID_TREE_DOWN, OnMoveDown)

	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_RCLICK, AFX_IDW_PANE_FIRST, OnRButtonDown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeViewGrammar diagnostics

#ifdef _DEBUG
void CTreeViewGrammar::AssertValid() const
{
	CPaneView::AssertValid();
}

void CTreeViewGrammar::Dump(CDumpContext& dc) const
{
	CPaneView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTreeViewGrammar message handlers

BOOL CTreeViewGrammar::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= TVS_HASBUTTONS | TVS_EDITLABELS | TVS_LINESATROOT;	
	return CPaneView::PreCreateWindow(cs);
}

CWnd* CTreeViewGrammar::OnCreateView() 
{
	if (!m_treeCtrl.Create( WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
		CRect(0,0,0,0), this, AFX_IDW_PANE_FIRST ))
	{
		TRACE0( "Unable to create tree control.\n" );
		return NULL;
	}
		
	// create the image list for the tree control
	m_ImageList.Create (IDB_GRAMMAR, 16, 1, RGB(0,255,0));
	m_treeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);

	return &m_treeCtrl;
}

void CTreeViewGrammar::FillList()
{
	if (m_treeCtrl.GetCount())
		m_treeCtrl.DeleteAllItems();

	HTREEITEM parentItem = m_treeCtrl.GetRootItem();
	int count = 0;
	FillList(gram,parentItem,count);
	UpdateWindow();
}

void CTreeViewGrammar::FillList(CONCEPT *parentConcept, HTREEITEM parentItem, int &count, bool parentHasSamples)
{
	if (!parentConcept)
		return;

	CONCEPT *concept = cg->Down(parentConcept);

	if (concept == parentConcept) {
		AfxMessageBox(_T("Error in sample hierarchy."));	// 03/21/01 AM.
		return;
	}

	SetRedraw(false);

	CString parentConceptStr = ConceptName(parentConcept);
	CString conceptStr;
	CString conceptPathStr;
	CString relPathStr;
	HTREEITEM child;
	HTREEITEM sample;
	HTREEITEM newItem;
	PHRASE *phrase;
	_TCHAR valBuff[1001];
	_TCHAR newBuff[1001];
	CString sampleFileStr = _T("");
	int sampleLength;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

//	valBuff[1001] = '\0';	// Causes .NET crash.	// 07/22/03 AM.
//	newBuff[1001] = '\0';	// Causes .NET crash.	// 07/22/03 AM.
	valBuff[0] = '\0';	// 07/22/03 AM.
	newBuff[0] = '\0';	// 07/22/03 AM.

	cg->findVal(parentConcept,_T("sampfile"),valBuff);
	bool isParentSample = valBuff[0] != '\0';
	bool isSample;
	bool isTopLevel = ((parentConcept == gram) ? true : false);	// Explicit nesting.	// 03/21/01 AM.
	int image = 0;
	int imageSel = 0;
	GRAM_NODE_TYPE type;
	GRAM_NODE_TYPE parentType;
	CString typeStr;
	CString parentTypeStr;

	while (concept) {
		isSample = !isParentSample;
		sampleFileStr = _T("");
		sampleLength = 0;

		conceptStr = ConceptName(concept);
		phrase = cg->findPhrase(concept);

		typeStr = KBGetValueString(concept,_T("type"));
		parentTypeStr = KBGetValueString(cg->Up(concept),_T("type"));
		if (parentTypeStr != _T(""))
			parentType = TypeFromString(parentTypeStr);
		else
			parentType = GRAM_NODE_NONE;

		// IF THE CONCEPT HAS A TYPE ATTRIBUTE USE THAT
		if (typeStr != _T("")) {
			type = TypeFromString(typeStr);
			ImageFromType(type,image,imageSel);
		}

		// ...IF NOT, FIGURE OUT WHAT IT SHOULD BE
		else {
			if (isTopLevel) {
				image = 0;
				imageSel = 1;
				type = GRAM_NODE_STUB;
			}
			// If parent has no samples and concept has none, then it's a folder.	// 03/21/01 AM.
			else if (!parentHasSamples && !phrase
				// && cg->Down(concept)		// 03/21/01 AM.
				) {
				image = 4;
				imageSel = 5;
				type = GRAM_NODE_FOLDER;
			}
			// Label can't have children.					// 03/21/01 AM.
			else if (parentType == GRAM_NODE_RULE
				// || parentType == GRAM_NODE_LABEL		// 03/21/01 AM.
				) {
				image = 6;
				imageSel = 7;
				type = GRAM_NODE_LABEL;
			}
			else if (parentType == GRAM_NODE_LABEL) {	// 03/21/01 AM.
				AfxMessageBox(_T("Internal error. Can't display sample hierarchy."));		// 03/21/01 AM.
				return;																	// 03/21/01 AM.
			}
			else {
				image = 8;
				imageSel = 9;
				type = GRAM_NODE_RULE;
			}
			ConceptAttributeUpdate(concept,_T("type"),TypeString(type));
		}

		child = m_treeCtrl.InsertItem(conceptStr,image,imageSel,parentItem);
		m_intItemCount++;
		m_treeCtrl.SetItemData(child,(DWORD)concept);

		FillList(concept,child,count,phrase ? true : false);

		// BUILD SAMPLE IF ANY
		if (phrase) {
			sample = child;

			while (phrase) {
				conceptStr = ConceptName(phrase);
				//if (conceptStr.Find("Dr. Corn") == 0) {
					//phrase = cg->Next(phrase);
					//wnd->AddDebugLine("Debug Dave:",conceptStr);
					//continue;
				//}
				newItem = m_treeCtrl.InsertItem(conceptStr,2,3,sample);
				m_intItemCount++;
				m_treeCtrl.SetItemData(newItem,(DWORD)phrase);

				cg->findVal(phrase,_T("text"),valBuff);
				conceptPathStr = valBuff;

				// Force path to relative if not
				if (valBuff[1] == ':') {
					relPathStr = RelativizePath(conceptPathStr);
					StringToChar(newBuff,relPathStr,1000);
					cg->attrChange(concept,_T("text"),valBuff,newBuff);
					wnd->AddDebugLine(_T("Relativizing path:"),conceptPathStr);
					kbDirty = true;
				}

				phrase = cg->Next(phrase);
			}
			m_treeCtrl.Expand(child,TVE_COLLAPSE);
		}

		// IF SAMPLE ASSOCIATE CONCEPT WITH THE FILE
		//if (isSample && SamplesInOneFile(child,sampleFileStr)) {	// Commented out.	// 03/21/01 AM.
			//conceptStr = ConceptName(concept);
			// CString msgStr;													// Commented out.	// 03/21/01 AM.
			//explicit cast per VC++ docs PD 02/14/2001
			//Commented out to eliminate bug complate 02/19/2001 
			//Uncomment if we want the source of samples to be explicitly mentioned on load
			/* msgStr.Format("%s with \"%s\"",(LPCTSTR)ConceptName(concept),(LPCTSTR)sampleFileStr);
			wnd->AddDebugLine("Possible sample file:",msgStr);
			*/
			//ConceptAttributeUpdate(concept,"sampfile",sampleFileStr);
			//kbDirty = true;
		// }																			// Commented out.	// 03/21/01 AM.

		count++;
		concept = cg->Next(concept);
	}

	SetRedraw(true);
	m_treeCtrl.InvalidateRect(NULL);
	m_treeCtrl.UpdateWindow();
}

bool CTreeViewGrammar::SamplesInOneFile(HTREEITEM treeItem, CString &sampleFileStr)
{
	if (treeItem) {
		CONCEPT *concept = (CONCEPT *)m_treeCtrl.GetItemData(treeItem);
		sampleFileStr = _T("");
		int sampleLength = 0;
		return SamplesInOneFile(concept,sampleFileStr,true,sampleLength);
	}
	else
		return false;
}

bool CTreeViewGrammar::SamplesInOneFile(CONCEPT *concept, CString &sampleFileStr, bool topLevel, int &sampleLength)
{
	PHRASE *phrase;
	CString conceptPathStr;
	CString conceptStr;
	_TCHAR valBuff[1001];

	// LOOP THROUGH PHRASES
	if ((phrase = cg->findPhrase(concept))) {

		while (phrase) {
			conceptStr = ConceptName(phrase);
			cg->findVal(phrase,_T("text"),valBuff);
			conceptPathStr = valBuff;

			if (sampleFileStr == _T(""))
				sampleFileStr = conceptPathStr;
			else if (conceptPathStr != sampleFileStr)
				return false;

			if (topLevel)
				sampleLength += conceptStr.GetLength() + 1;

			phrase = cg->Next(phrase);
		}
	}

	// LOOP THROUGH CONCEPTS
	concept = cg->Down(concept);
	while (concept) {
		if (!SamplesInOneFile(concept,sampleFileStr,false,sampleLength))
			return false;
		concept = cg->Next(concept);
	}

	// IF SAMPLE ASSOCIATE CONCEPT WITH THE FILE
	if (sampleLength && topLevel) {
		_TCHAR valBuff[1001];
		valBuff[0] = '\0';
		int fileLength = 0;

		if (sampleLength) {
			fileLength = GetFileLength(AppPath(sampleFileStr));
			cg->findVal(concept,_T("sampfile"),valBuff);
		}
		else
			return false;

		return fileLength <= sampleLength + 3 && valBuff[0] == '\0';
		}
	else if (!topLevel)
		return true;

	return false;
}

void CTreeViewGrammar::GenerateSamples(HTREEITEM item)
{
	if (!item)
		item = m_treeCtrl.GetRootItem();
	CString debugStr;
	HTREEITEM child;

	while (item) {
		debugStr = m_treeCtrl.GetItemText(item);
		child = m_treeCtrl.GetChildItem(item);

		// CALL FUNCTION AND CHECK FOR RECURSE
		if (!GenerateSampleFromConcept(item) && child)
			GenerateSamples(child);

		item = m_treeCtrl.GetNextSiblingItem(item);
	}
}

bool CTreeViewGrammar::GenerateSampleFromConcept(HTREEITEM treeItem)
{
	if (!treeItem)
		treeItem = m_itemSelected;

	CONCEPT *concept = (CONCEPT *)m_treeCtrl.GetItemData(treeItem);
	CStringList sampleList;
	return GenerateSampleFromConcept(concept,_T(""),sampleList);
}

bool CTreeViewGrammar::GenerateSampleFromConcept(CONCEPT *concept, CString sampleFileStr,
												 CStringList &sampleList)
{
	if (!concept)
		return false;

	bool added = false;
	PHRASE *phrase;
	CString conceptPathStr;
	CString conceptStr;
	_TCHAR buff[1001];
	int sampleLength = 0;
	bool topLevel = false;

	// GET SAMPLE FILE
	if (sampleFileStr == _T("")) {
		cg->findVal(concept,_T("sampfile"),buff);
		sampleFileStr = buff;
		cg->findVal(concept,_T("dirty"),buff);
		BOOL cgDirtyFlag = BoolValueVT(buff);

		// IF NO SAMPLE FILE OR NOT MARKED DIRTY, RETURN
		if (!cgDirtyFlag || sampleFileStr == _T(""))
			return false;

		topLevel = true;
	}

	if ((phrase = cg->findPhrase(concept))) {

		long start = 0;
		long end = 0;
		long phraseStart;
		long phraseEnd;
		int strLen;
		CString sampleContentStr = _T("");
		CString sampleStr;
		CString msgStr;
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		CString phraseConceptPathStr;
		long newStart = 0;

		// LOOP THROUGH PHRASE SAMPLES
		while (phrase) {

			if (topLevel) {
				conceptStr = ConceptName(phrase);
				cg->findVal(phrase,_T("text"),buff);
				conceptPathStr = buff;
				strLen = conceptStr.GetLength();

				cg->findVal(phrase,_T("start"),phraseStart);
				cg->findVal(phrase,_T("end"),phraseEnd);

				// CHANGE TO NEW START AND END
				end = start + strLen - 1;
				ConceptAttributeUpdate(phrase,_T("start"),start);
				ConceptAttributeUpdate(phrase,_T("end"),end);
				ConceptAttributeUpdate(phrase,_T("text"),sampleFileStr);
				added = true;

				// LOG IN LOG WINDOW
				msgStr.Format(_T("\"%s\" to \"%s\" from \"%s\""),
					conceptStr,StripPath(sampleFileStr,0),conceptPathStr);
				wnd->AddDebugLine(_T("Adding sample:"),msgStr);

				// SAVE FOR ORPHAN LABELS TO FIND
				if (topLevel) {
					sampleStr.Format(_T("%s:%d:%d:%d"),conceptPathStr,start,phraseStart,phraseEnd);
					sampleList.AddTail(sampleStr);

					sampleContentStr += conceptStr + _T("\n");
				}

				start += strLen + 1;
			}

			// LOOP THROUGH SAMPLES UNDER LABELS
			else {
				conceptStr = ConceptName(phrase);
				cg->findVal(phrase,_T("text"),buff);
				conceptPathStr = buff;

				POSITION pos = sampleList.GetHeadPosition();
				cg->findVal(phrase,_T("start"),start);
				cg->findVal(phrase,_T("end"),end);

				// LOOP THROUGH NON-SAMPLE FILE PHRASES TO ADJUST LABEL OFFSET
				while (pos && (sampleStr = sampleList.GetNext(pos))) {
					NthPathSegment(sampleStr,phraseConceptPathStr,':',0);
					NthPathSegmentLong(sampleStr,newStart,':',1);
					NthPathSegmentLong(sampleStr,phraseStart,':',2);
					NthPathSegmentLong(sampleStr,phraseEnd,':',3);

					// TEST IF IN THE SAME TEXT FILE AND BETWEEN PHRASE OFFSETS
					if (conceptPathStr == phraseConceptPathStr &&
						phraseStart <= start && start <= phraseEnd &&
						phraseStart <= end && end <= phraseEnd) {

						ConceptAttributeUpdate(phrase,_T("start"),newStart + start - phraseStart);
						ConceptAttributeUpdate(phrase,_T("end"),newStart + end - phraseStart);
						ConceptAttributeUpdate(phrase,_T("text"),sampleFileStr);
						added = true;

						// LOG IN LOG WINDOW
						msgStr.Format(_T("\"%s\" to \"%s\" from \"%s\""),
							conceptStr,StripPath(sampleFileStr,0),conceptPathStr);
						wnd->AddDebugLine(_T("Adding label sample:"),msgStr);

						// FOUND, SO BREAK OUT
						break;
					}
				}
			}

			phrase = cg->Next(phrase);
		}

		// LOOP THROUGH NON-ASSOCIATED LABEL CONCEPTS ADJUSTING START & END
		concept = cg->Down(concept);
		CString debugStr;

		while (concept) {
			debugStr = ConceptName(concept);

			// RECURSE
			GenerateSampleFromConcept(concept,sampleFileStr,sampleList);
			concept = cg->Next(concept);
		}

		if (topLevel)
			sampleList.RemoveAll();

		// WRITE NEW CONENT TO SAMPLE FILE
		if (topLevel && sampleContentStr != _T("")) {
			NLPWriteFile(AppPath(sampleFileStr),sampleContentStr);
			theApp.UpdateDocFromPath(AppPath(sampleFileStr));
		}
	}

	if (added) {
		kbDirty = true;
		m_boolGenerate = true;
	}

	return added;
}

CONCEPT *CTreeViewGrammar::GetParentConcept(HTREEITEM childItem, bool selected)
{
	if (!childItem)
		return gram;

	HTREEITEM item = childItem;
	int positions[20];
	int depth = 0;
	int pos;
	CString str;

	if (selected)
		m_sampleSelectedItem = childItem;

	while (item && depth < 20) {
		str = m_treeCtrl.GetItemText(item);
		pos = TreeCtrlItemPosition(&m_treeCtrl,item);
		positions[depth++] = pos;
		item = m_treeCtrl.GetParentItem(item);
	}
	positions[depth] = 0;

	CONCEPT *concept = gram;
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
			if (concept) {
				isPhrase = true;
			}
			break;
		}

		lastConcept = concept;
	}

	if (!concept)
		concept = gram;

	if (!isPhrase && selected)
		m_sampleSelectedConcept = concept;

	cg->conceptName(concept,buff);

	return concept;
}

void CTreeViewGrammar::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	SelectUpdate();
	
	*pResult = 0;
}

void CTreeViewGrammar::SelectUpdate()
{
	m_itemSelected = m_treeCtrl.GetSelectedItem();
	//m_conceptSelected = GetParentConcept(m_itemSelected,true);
	m_conceptSelected = (CONCEPT *)m_treeCtrl.GetItemData(m_itemSelected);
	CString conceptStr = ConceptName(m_conceptSelected);

	m_intPass = KBGetValueLong(m_conceptSelected,_T("passnum"));
	m_strConcept = m_treeCtrl.GetItemText(m_itemSelected);

	m_GrammarNodeType = NodeType(m_itemSelected);

	m_sampleSelectedItem = NULL;
	m_sampleSelectedConcept = NULL;
	m_boolSampleSiblings = false;

	if (m_GrammarNodeType == GRAM_NODE_SAMPLE) {
		m_sampleSelectedItem = m_treeCtrl.GetParentItem(m_itemSelected);
		m_sampleSelectedConcept = (CONCEPT *)m_treeCtrl.GetItemData(m_sampleSelectedItem);
	}
	else if (m_GrammarNodeType == GRAM_NODE_STUB || m_GrammarNodeType == GRAM_NODE_FOLDER) {
		m_boolSampleSiblings = true;
	}

	// Select analyzer path corresponding to the concept
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	if (wnd->GetActiveTab() == WORKSPACE_GRAMMAR && appBoolHighlightText) {
		long pass;
		if (m_GrammarNodeType == GRAM_NODE_SAMPLE) {
//			CONCEPT *parent = cg->nodeConcept(m_conceptSelected);	// 12/03/02 AM.
			CONCEPT *parent = cg->nodeOwner(m_conceptSelected);	// 12/03/02 AM.
			cg->findVal(parent,_T("passnum"),pass);
		}
		else
			cg->findVal(m_conceptSelected,_T("passnum"),pass);

		if (m_intPass >= 0 &&
			(m_GrammarNodeType == GRAM_NODE_SAMPLE || m_GrammarNodeType == GRAM_NODE_RULE)) {
			//wnd->SelectPass((int)pass-1);	// VISUAL IS ONE-NUMBERED, INTERNAL ZERO	
			GrammarHighlightmatches(false);
			theApp.UpdateParseTrees(m_intPass);
		}
		else
			theApp.DoAllViews(NULL,_T("VisualText"),_T(""),DOC_ACTION_UNHIGHLIGHT_ALL);
	}
}

GRAM_NODE_TYPE CTreeViewGrammar::NodeType(HTREEITEM item)
{
	GRAM_NODE_TYPE type = GRAM_NODE_STUB;

	int image;
	int imageSel;

	m_treeCtrl.GetItemImage(item,image,imageSel);
	switch (image) {
	case 0:
	case 1:
		type = GRAM_NODE_STUB;
		break;
	case 2:
	case 3:
		type = GRAM_NODE_SAMPLE;
		break;
	case 4:
	case 5:
		type = GRAM_NODE_FOLDER;
		break;
	case 6:
	case 7:
		type = GRAM_NODE_LABEL;
		break;
	case 8:
	case 9:
		type = GRAM_NODE_RULE;
		break;
	}

	return type;
}

void CTreeViewGrammar::ImageFromType(GRAM_NODE_TYPE type, int &image, int &imageSel)
{
	switch (type) {
	case GRAM_NODE_STUB:
		image = 0;
		imageSel = 1;
		break;
	case GRAM_NODE_SAMPLE:
		image = 2;
		imageSel = 3;
		break;
	case GRAM_NODE_FOLDER:
		image = 4;
		imageSel = 5;
		break;
	case GRAM_NODE_LABEL:
		image = 6;
		imageSel = 7;
		break;
	case GRAM_NODE_RULE:
		image = 8;
		imageSel = 9;
		break;
	}
}

CString CTreeViewGrammar::TypeString(GRAM_NODE_TYPE type)
{
	CString typeStr = _T("");
	switch (type) {
	case GRAM_NODE_NONE:
		typeStr = _T("");
		break;
	case GRAM_NODE_STUB:
		typeStr = _T("stub");
		break;
	case GRAM_NODE_SAMPLE:
		typeStr = _T("sample");
		break;
	case GRAM_NODE_FOLDER:
		typeStr = _T("folder");
		break;
	case GRAM_NODE_LABEL:
		typeStr = _T("label");
		break;
	case GRAM_NODE_RULE:
		typeStr = _T("rule");
		break;
	}
	return typeStr;
}

GRAM_NODE_TYPE CTreeViewGrammar::TypeFromString(CString typeStr)
{
	GRAM_NODE_TYPE type = GRAM_NODE_STUB;

	if (typeStr == _T(""))
		type = GRAM_NODE_NONE;
	else if (typeStr == _T("stub"))
		type = GRAM_NODE_STUB;
	else if (typeStr == _T("sample"))
		type = GRAM_NODE_SAMPLE;
	else if (typeStr == _T("folder"))
		type = GRAM_NODE_FOLDER;
	else if (typeStr == _T("label"))
		type = GRAM_NODE_LABEL;
	else if (typeStr == _T("rule"))
		type = GRAM_NODE_RULE;

	return type;
}

void CTreeViewGrammar::OnGrammarAddconcept() 
{
	if (m_treeCtrl.GetCount() && !m_itemSelected)
		AfxMessageBox(_T("No concept chosen"));
	else if (m_GrammarNodeType == GRAM_NODE_SAMPLE)
		AfxMessageBox(_T("Can't add a concept to a sample"));
	else if (AskForText(_T("Enter Concept Name:\n(more than one separated by commas)"))) {
		AddConcept(appAskText,m_itemSelected);
	}
}

void CTreeViewGrammar::OnGrammarAddtopconcept() 
{
 	if (AskForText(_T("Enter Top Concept Name:\n(more than one separated by commas)"))) {
		AddTopLevelConcept(appAskText,false);
	}	
}

void CTreeViewGrammar::AddTopLevelConcept(CString conceptStr, bool addStub) 
{
	if (conceptStr != _T("") ) {
		if (TopLevelConceptExists(conceptStr)) {
			CString msgStr;
			if (addStub)
				msgStr.Format(_T("The Stub \"%s\" already exists!"),conceptStr);
			else
				msgStr.Format(_T("The pass \"%s\" already exists!"),conceptStr);

			AfxMessageBox(msgStr);
		}
		else {
			AddConcept(conceptStr,m_itemSelected,GRAM_NODE_STUB);
			if (addStub)
				theApp.m_pMainFrame->m_wndAnalyzerView.AddStub(conceptStr,true);
		}
	}	
}

bool CTreeViewGrammar::TopLevelConceptExists(CString conceptStr) 
{
	if (conceptStr == _T(""))
		return false;

	HTREEITEM item = m_treeCtrl.GetRootItem();
	CString itemTextStr;
	
	while (item) {
		itemTextStr = m_treeCtrl.GetItemText(item);
		if (itemTextStr == conceptStr)
			return true;
		item = m_treeCtrl.GetNextSiblingItem(item);
	}
	return false;
}

void CTreeViewGrammar::OnGrammarEditexamples() 
{
	if (m_GrammarNodeType == GRAM_NODE_SAMPLE) {

		_TCHAR buff[1001];
		cg->conceptName(m_conceptSelected,buff);
		cg->findVal(m_conceptSelected, _T("text"), buff);
		CString relPathStr = buff;
		CString filePathStr = FullPath(relPathStr);

		if (!FileExists(filePathStr)) {
			CString msgStr;
			msgStr.Format(_T("Couldn't find file: \"%s\""), filePathStr);
			AfxMessageBox(msgStr);
			return;
		}

		long start;
		long end;
		cg->findVal(m_conceptSelected, _T("start"), start);
		cg->findVal(m_conceptSelected, _T("end"), end);

		//int lineCount = FileLineNumberFromIndex(filePathStr,start);
		//start += lineCount;
		//end += lineCount;

		CVisualTextDoc *doc =
			(CVisualTextDoc *)theApp.OpenNewDocument(_T("VisualText"),filePathStr);
		CVisualTextView *view = (CVisualTextView *)GetFirstView(doc);

		CRichEditCtrl &edit = view->GetRichEditCtrl();
		end++;  // Adjusted for select function
		edit.SetSel(start,end);
	}
}

void CTreeViewGrammar::AddConcept(CString conceptStr, HTREEITEM itemSelected, GRAM_NODE_TYPE type)
{
	CStringList conceptList;
	TokensBySeparator(conceptList,conceptStr,',');
	int count = conceptList.GetCount();
	CString conceptAdd;
	CONCEPT *newConcept;
	_TCHAR buff[1001];
	HTREEITEM newItem;
	HTREEITEM lastConceptItem;
	bool expand = false;
	CONCEPT *selectedConcept = m_conceptSelected;

	SetRedraw(false);

	int i = 0;
	int image, imageSel;
	ImageFromType(type,image,imageSel);

	// SELECT THE FIRST CHILD IF EXISTS ITEM
	newItem = m_treeCtrl.GetChildItem(m_itemSelected);
	if (newItem)
		m_treeCtrl.SelectItem(newItem);

	while ((conceptAdd = NthStringListItem(conceptList,i++)) != _T("")) {

		if (itemSelected && type != GRAM_NODE_STUB) {
			// Insert after last concept, not sample!
			if (!(lastConceptItem = FindLastConceptSibling(&m_treeCtrl,newItem)))
				lastConceptItem = TVI_FIRST;
			newItem = m_treeCtrl.InsertItem(conceptAdd,image,imageSel,itemSelected,lastConceptItem);
			expand = true;
		}
		else if (itemSelected && type == GRAM_NODE_STUB) {
			newItem = m_treeCtrl.InsertItem(conceptAdd,image,imageSel,TVI_ROOT,itemSelected);
		}
		else {
			newItem = m_treeCtrl.InsertItem(conceptAdd,image,imageSel);
		}

		// ADD CONCEPT TO KB
		StringToChar(buff,conceptAdd,1000);
		if (type == GRAM_NODE_STUB || m_treeCtrl.GetCount() == 1)	// Equals 1 because first added
			newConcept = cg->makeConcept(gram,buff);
		else
			newConcept = cg->makeConcept(selectedConcept,buff);
		ConceptAttributeUpdate(newConcept,_T("type"),TypeString(type));

		// ADD CONCEPT AS USER DATA FOR INTERNAL USE
		m_treeCtrl.SetItemData(newItem,(DWORD)newConcept);

		m_treeCtrl.SelectItem(newItem);
		m_intItemCount++;
		kbDirty = true;
		m_boolGenerate = true;
	}

	if (expand && itemSelected)
		m_treeCtrl.Expand(itemSelected,TVE_EXPAND);

	SetRedraw(true);
	m_treeCtrl.InvalidateRect(NULL);
	m_treeCtrl.UpdateWindow();
}

CString CTreeViewGrammar::TreePath(HTREEITEM child)
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
		pathStr = parentStr + GRAMCONSEP + pathStr;
		child = parent;
	}
	return pathStr;
}

void CTreeViewGrammar::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	CString newNameStr = pTVDispInfo->item.pszText;
	_TCHAR buff[1001];
	StringToChar(buff,AppPath(),1000);
	int ret = 1;

	if (m_GrammarNodeType == GRAM_NODE_SAMPLE) {
		AfxMessageBox(_T("Cannot change samples"));
		ret = 0;
	}
	else if (newNameStr == _T("")) {
		ret = 0;
	}
	else {
		StringToChar(buff,newNameStr,1000);
		Agram::renameConcept(m_conceptSelected,buff,cg);
		kbDirty = true;
		m_boolGenerate = true;
	}

	*pResult = ret;
}

CString CTreeViewGrammar::GetSelection()
{
	return StateString(&m_treeCtrl,m_treeCtrl.GetRootItem(),STATE_GET_SELECTED);
}

void CTreeViewGrammar::SetSelection(CString selStr)
{
	//SetRedraw(false);
	StateFromString(&m_treeCtrl,m_treeCtrl.GetRootItem(),STATE_SELECT,selStr);
	//SetRedraw(true);
}

void CTreeViewGrammar::AddConceptSample(CString newExampleStr, CString textPath,
										long start, long end,
										UINT menuID, CONCEPT *concept)
{
	if (newExampleStr == _T(""))
		return;

	_TCHAR buff[1001];
	CString conceptNameStr;
	CONCEPT *sampleConcept = NULL;
	HTREEITEM treeItemToAddTo = NULL;

	if (concept) {
		treeItemToAddTo = TreeItemFromConcept(concept,m_treeCtrl.GetRootItem());

		if (menuID == 1) {
			conceptNameStr = ConceptName(concept);
			sampleConcept = concept;
		}
		else {
			int menuIndex = menuID;
			treeItemToAddTo = TreeItemFromSampleMenu(menuIndex, treeItemToAddTo);
			sampleConcept = (CONCEPT *)m_treeCtrl.GetItemData(treeItemToAddTo);
			conceptNameStr = ConceptName(sampleConcept);
		}
	}
	else if (menuID > 0 && menuID < 10000) {  // menuID == 10000 for Converting samps to files
		int menuIndex = menuID;
		treeItemToAddTo = TreeItemFromSampleMenu(menuIndex);
		if (treeItemToAddTo) {
			sampleConcept = (CONCEPT *)m_treeCtrl.GetItemData(treeItemToAddTo);
			conceptNameStr = ConceptName(sampleConcept);
		}
		else {
			return; // Disaster, otherwise! SRP 010801
		}
	}
	else {
		conceptNameStr = m_strConcept;
		sampleConcept = m_conceptSelected;
		treeItemToAddTo = m_itemSelected;
	}

	StringToChar(buff,newExampleStr,1000);
	HTREEITEM newItem;

	if (!IsConceptSampleRepeat(sampleConcept,textPath,start,end)) {
		// This will set DIRTY attributes appropriately.
		CONCEPT *phraseConcept = Agram::addSample(sampleConcept,buff,cg);

		cg->findVal(sampleConcept, _T("sampfile"), buff);
		CString sampleFileStr = buff;

		if (menuID > 0 && sampleFileStr != _T("") &&
			textPath != AppPath(sampleFileStr)) {  // menuID to block when regenning

			AddToSampleFile(sampleFileStr,newExampleStr,start,end);		// 12/09/99 AM.
			textPath = AppPath(sampleFileStr);
			theApp.UpdateDocFromPath(textPath);
		}

		cg->addVal(phraseConcept,_T("start"),start);
		cg->addVal(phraseConcept,_T("end"),end);
		StringToChar(buff,RelativizePath(textPath),1000);
		cg->addVal(phraseConcept,_T("text"),buff);

		// Now add it to the tree live
		if (menuID < 10000) {
			newItem = m_treeCtrl.InsertItem(newExampleStr,2,3,treeItemToAddTo);
			m_treeCtrl.Expand(treeItemToAddTo,TVE_EXPAND);
			m_treeCtrl.SetItemData(newItem,(DWORD)phraseConcept);
			m_intItemCount++;
		}

		kbDirty = true;
		m_boolGenerate = true;
	}
	else {
		CString msgStr;
		msgStr = _T("\"") + newExampleStr +
			_T("\" already exists under the concept \"") + conceptNameStr +
			_T("\"\nfor file \"") + textPath + _T("\"");
		AfxMessageBox(msgStr);
	}
}

void CTreeViewGrammar::Load()
{
	CONCEPT *root = cg->findRoot();
	gram = cg->findConcept(root, _T("gram"));
	if (!gram)
		gram = cg->makeConcept(root, _T("gram"));
	RUG::rugInit(gram,cg);

	FillList();
}

void CTreeViewGrammar::Clear()
{
	SetRedraw(false);
	m_treeCtrl.DeleteAllItems();
	SetRedraw(true);
	m_treeCtrl.InvalidateRect(NULL);
	m_treeCtrl.UpdateWindow();
	if (rug)
		delete rug;
	InitVars();
}

void CTreeViewGrammar::OnDelete()
{
	DeleteSelected();
}

void CTreeViewGrammar::OnMoveUp()
{
	MoveSelUp();
}

void CTreeViewGrammar::OnMoveDown()
{
	MoveSelDown();
}

void CTreeViewGrammar::MoveSelUp()
{
	if (m_GrammarNodeType == GRAM_NODE_SAMPLE && cg->moveNleft(m_conceptSelected)) {
		MoveTreeNodeUp(&m_treeCtrl,m_itemSelected,0,1);
		kbDirty = true;
		m_boolGenerate = true;
	}
	else if (cg->moveCleft(m_conceptSelected)) {
		MoveTreeNodeUp(&m_treeCtrl,m_itemSelected,2,3);
		kbDirty = true;
		m_boolGenerate = true;
	}
}

void CTreeViewGrammar::MoveSelDown()
{
	if (m_GrammarNodeType == GRAM_NODE_SAMPLE && cg->moveNright(m_conceptSelected)) {
		MoveTreeNodeDown(&m_treeCtrl,m_itemSelected,0,1);
		kbDirty = true;
		m_boolGenerate = true;
	}
	else if (cg->moveCright(m_conceptSelected)) {
		MoveTreeNodeDown(&m_treeCtrl,m_itemSelected,2,3);
		kbDirty = true;
		m_boolGenerate = true;
	}
}

void CTreeViewGrammar::OnGrammarDeleteconcept() 
{
	DeleteSelectedAndAnalyzerPass();
}

void CTreeViewGrammar::DeleteSelectedAndAnalyzerPass() 
{
	CString selectedStr = m_treeCtrl.GetItemText(m_itemSelected);

	// DeleteSelected WIPES OUT THE VARS SO KEEP TYPE FOR SECOND IF STATEMENT
	bool isStubFlag = m_GrammarNodeType == GRAM_NODE_STUB ? true : false;

	if (!DeleteSelected())
		return;

	// REMOVE IN ANALYZER TAB IF STUB
	if (isStubFlag)
		theApp.m_pMainFrame->m_wndAnalyzerView.DeleteStubByName(selectedStr);
}

bool CTreeViewGrammar::DeleteSelected(bool dontAsk) 
{
	CString selectedStr = m_treeCtrl.GetItemText(m_itemSelected);
	CString msgStr = _T("You sure you want to delete \"");
	msgStr += selectedStr + _T("\"");
	if (m_treeCtrl.GetChildItem(m_itemSelected)) msgStr += _T(" and its children");
	msgStr += _T("?");

	if (dontAsk || m_itemSelected && AfxMessageBox(msgStr, MB_YESNO) == IDYES) {
		CString debugStr = ConceptName(m_conceptSelected);
		if (m_GrammarNodeType == GRAM_NODE_SAMPLE)
			Agram::rmSample(m_conceptSelected,cg);
		else
			cg->rmConcept(m_conceptSelected);
		SetRedraw(false);
		m_treeCtrl.DeleteItem(m_itemSelected);

		// IF NONE LEFT, THEN INITIALIZE VARIABLES
		if (m_treeCtrl.GetCount() == 0)
			InitVars();

		SetRedraw(true);
		m_treeCtrl.InvalidateRect(NULL);
		m_treeCtrl.UpdateWindow();

		m_intItemCount--;
		kbDirty = true;
		m_boolGenerate = true;
		return true;
	}
	return false;
}

void CTreeViewGrammar::SamplePopup(CWnd *wnd, CPoint point)
{
	CStringList menu;
	SamplePopupStringList(menu);
	PopupFromStringList(wnd,point,menu,TEXTVIEW_ID_START);
}

void CTreeViewGrammar::SampleViewPopup(CWnd *wnd, CPoint point, CONCEPT *concept)
{
	CStringList menu;
	SampleViewPopupStringList(menu,concept);
	PopupFromStringList(wnd,point,menu,SAMPLE_ID_START);
}

void CTreeViewGrammar::SamplePopupStringList(CStringList &menuList)
{
	if (!m_itemSelected || !cg)
		return;

	CString conceptStr;
	menuList.RemoveAll();

	menuList.AddTail(_T("Forward"));
	menuList.AddTail(_T("Backward"));
	menuList.AddTail(_T("Display Tree"));
	//menuList.AddTail("Form");
	menuList.AddTail(_T("View Output"));
	menuList.AddTail(_T("- - - - -"));

	CONCEPT *concept = m_conceptSelected;
	bool siblingsFlag = false;

	if (m_GrammarNodeType == GRAM_NODE_SAMPLE)
		concept = SafeConceptSelected();
	else if (m_GrammarNodeType == GRAM_NODE_STUB || m_GrammarNodeType == GRAM_NODE_FOLDER) {
		concept = cg->Down(concept);
		siblingsFlag = true;
	}
	//AddConceptHierarchyToStringList(menuList,concept,siblingsFlag);
	ConceptHierarchyToStringList(menuList,concept,0,siblingsFlag);
}

void CTreeViewGrammar::SampleViewPopupStringList(CStringList &menuList, CONCEPT *concept)
{
	if (!m_itemSelected || !cg)
		return;
	bool siblingsFlag = false;

	if (m_GrammarNodeType == GRAM_NODE_SAMPLE)
		concept = SafeConceptSelected();
	else if (m_GrammarNodeType == GRAM_NODE_STUB || m_GrammarNodeType == GRAM_NODE_FOLDER) {
		concept = cg->Down(concept);
		siblingsFlag = true;
	}

	CString conceptStr = ConceptName(concept);
	menuList.RemoveAll();

	//menuList.AddTail("Form");
	menuList.AddTail(_T("Generate Samples"));
	menuList.AddTail(_T("Sort to Unique"));
	menuList.AddTail(_T("- - - - -"));

	//AddConceptHierarchyToStringList(menuList,concept,siblingsFlag);
	ConceptHierarchyToStringList(menuList,concept,0,siblingsFlag);
}

void CTreeViewGrammar::SelectFromStringList(CStringList &heirList)
{
	CONCEPT *concept = ConceptFromStringList(heirList);
	if (concept) {
		HTREEITEM item = TreeItemFromConcept(concept,m_treeCtrl.GetRootItem());
		m_treeCtrl.SelectItem(item);
	}
}

CONCEPT *CTreeViewGrammar::ConceptFromStringList(CStringList &conceptList)
{
	CString conceptStr;
	_TCHAR buff[1001];
	CONCEPT *concept = gram;
	int len = conceptList.GetCount();

	for (int i=0; i<len; i++) {
		conceptStr = NthStringListItem(conceptList,i);
		if (conceptStr != _T("")) {
			StringToChar(buff,conceptStr,1000);
			concept = cg->findConcept(concept,buff);
		}
	}
	return concept;
}

void CTreeViewGrammar::StringListFromTreeItem(CStringList &treeList, HTREEITEM treeItem)
{
	CString treeStr;
	CONCEPT *root = cg->findRoot();
	HTREEITEM item = treeItem;

	while (item) {
		treeStr = m_treeCtrl.GetItemText(item);
		treeList.AddHead(treeStr);
		item = m_treeCtrl.GetParentItem(item);
	}
}

HTREEITEM CTreeViewGrammar::TreeItemFromSampleMenu(int menuIndex, HTREEITEM startItem)
{
	int count = 1;  // ONE NUMBERED
	if (!startItem) {
		if (m_itemSelected) {
			startItem = m_itemSelected;
			if (m_boolSampleSiblings)
				startItem = m_treeCtrl.GetChildItem(startItem);
		}
		else if (m_sampleSelectedItem)
			startItem = m_sampleSelectedItem;
	}

	return TreeItemFromNumber(&m_treeCtrl,startItem,count,menuIndex,m_boolSampleSiblings,true);
}

CONCEPT *CTreeViewGrammar::ConceptFromMenuIndex(int menuIndex)
{
	int count = 1;  // ONE NUMBERED
	HTREEITEM item = TreeItemFromNumber(&m_treeCtrl,m_sampleSelectedItem,count,menuIndex);
	//return GetParentConcept(item);
	return (CONCEPT *)m_treeCtrl.GetItemData(item);
}

HTREEITEM CTreeViewGrammar::TreeItemFromNumber(CXTPTreeCtrl *treeCtrl, const HTREEITEM &currentItem, int &count,
	const int nth, const bool includeSiblingsFlag, bool bAddingSamples)
{
	GRAM_NODE_TYPE type;
	GRAM_NODE_TYPE childType;
	HTREEITEM child;
	HTREEITEM returnItem;
	CString nameStr;
	HTREEITEM item = currentItem ;

	while (item) {
		nameStr = treeCtrl->GetItemText(item);

		// SKIP SAMPLES
		type = NodeType(item);
		if (type == GRAM_NODE_SAMPLE) {
			if (bAddingSamples) {
				return treeCtrl->GetParentItem(item); // SRP 010801
			}
			else {
				item = treeCtrl->GetNextSiblingItem(item);
				continue;
			}
		}

		if (count == nth)
			break;
		count++;

		if ((child = treeCtrl->GetChildItem(item))) {
			childType = NodeType(child);
			if (childType != GRAM_NODE_SAMPLE &&
				(returnItem = TreeItemFromNumber(treeCtrl,child,count,nth,includeSiblingsFlag))) {
				item = returnItem;
				break;
			}
		}

		if (count == 1 && !includeSiblingsFlag)
			break;
		item = treeCtrl->GetNextSiblingItem(item);
	}
	
	return item;
}

void CTreeViewGrammar::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	int ret = 0;

	if (m_GrammarNodeType == GRAM_NODE_SAMPLE) {
		ret = 1;
	}

	*pResult = ret;
}

// START DRAG AND DROP STUFF

void CTreeViewGrammar::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // This code is to prevent accidental drags.
    if((GetTickCount() - m_dwDragStart) < DRAG_DELAY )
		return;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	m_hitemDrag = pNMTreeView->itemNew.hItem;
	if (!m_hitemDrag)
		return;
	// SRP 010504
	// Example:  Create Stub 1, Create Stub 2, Create Rule A under Stub 1.
	// Now begin to drag Stub 1.  SelectUpdate() is called on LButtonUp!
	// At this point in the code, Rule A is still the selected item.
	// Therefore, need to update before testing whether drag is allowed (below).
	m_treeCtrl.SelectItem(m_hitemDrag);
	SelectUpdate();

	// Don't allow for dragging samples or stubs
	if (m_GrammarNodeType == GRAM_NODE_STUB || m_GrammarNodeType == GRAM_NODE_SAMPLE)
		return;
	
	m_hitemDrop = NULL;
	m_treeCtrl.SelectItem(m_hitemDrag);

	m_pDragImage = m_treeCtrl.CreateDragImage(m_hitemDrag);

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

	if (0 == m_nTimerIDDrag)
		m_nTimerIDDrag = SetTimer(ID_TIMER_DRAG,75,NULL);
}

void CTreeViewGrammar::OnMouseMove(UINT nFlags, CPoint point) 
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
			if ((m_hitemDrop = GetDropTarget(hItem))) {
				m_treeCtrl.SelectDropTarget(m_hitemDrop);
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
	
	CPaneView::OnMouseMove(nFlags, point);
}

BOOL CTreeViewGrammar::ForbiddenDragDropCombination(HTREEITEM hitemDrag, HTREEITEM hitemDrop)
{
	if (hitemDrag == 0 || hitemDrop == 0)
		return true;

	if (hitemDrag == hitemDrop || !IsDropSource(hitemDrop))
		return true;

	// If Drag item is an ancestor of Drop item then return
	HTREEITEM htiParent = m_hitemDrop;
	while((htiParent = m_treeCtrl.GetParentItem( htiParent )) != NULL ) {
		if( htiParent == m_hitemDrag )
			return true;
	}

	return false;
}

void CTreeViewGrammar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CPaneView::OnLButtonUp(nFlags, point);

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

		if (ForbiddenDragDropCombination(m_hitemDrag, m_hitemDrop)) {
			// Remove drop target highlighting
			m_treeCtrl.SelectDropTarget(NULL);
			return;
		}

		// Remove drop target highlighting
		m_treeCtrl.SelectDropTarget(NULL);

		// Move Concept to first position
		CONCEPT *drag = (CONCEPT *)m_treeCtrl.GetItemData(m_hitemDrag);
		CONCEPT *drop = (CONCEPT *)m_treeCtrl.GetItemData(m_hitemDrop);
		CString debugDragStr = ConceptName(drag);
		CString debugDropStr = ConceptName(drop);

		cg->rmConcept(drag);
		cg->addConcept(drop,drag,1);
		kbDirty = true;
		m_boolGenerate = true;

		// Move interface tree stuff
		m_treeCtrl.Expand(m_hitemDrop, TVE_EXPAND);
		HTREEITEM htiNew = CopyBranch(&m_treeCtrl, m_hitemDrag, m_hitemDrop, TVI_FIRST);
		m_treeCtrl.DeleteItem(m_hitemDrag);
		// SRP 010506 Why is this here?
		// ALL OnUpdateGrammarXXX() depend upon m_intItemCount being positive.
//		m_intItemCount--;
		m_treeCtrl.SelectItem(htiNew);
	}
}

void CTreeViewGrammar::OnTimer(UINT nIDEvent) 
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

		case ID_TIMER_GEN:
		// This timer function is also used for getting info back from
		// the library NLP functions and displaying it in the debug window
#ifdef GEODUCK_
		if ((appState == VT_APP_GENERATING || appState == VT_APP_STOP_COMMAND) &&
				 nlp->gui->IsMessage(GUI_MESSAGE_WAITING)) {

			_TCHAR labelBuff[GUI_STRING_MAX];
			_TCHAR valueBuff[GUI_STRING_MAX];
			nlp->gui->SetMessage(GUI_MESSAGE_RECEIVED);
			nlp->gui->ReceiveMessage(labelBuff,valueBuff);
			CString labelStr = labelBuff;
			CString valueStr = valueBuff;
			CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
			wnd->AddDebugLine(labelStr,valueStr,true);
			nlp->gui->SetMessage(GUI_MESSAGE_DISPLAYED);
		}
#endif
		break;
	}

	CPaneView::OnTimer(nIDEvent);
}

bool CTreeViewGrammar::IsDropSource(HTREEITEM hItem)
{
	GRAM_NODE_TYPE type = NodeType(hItem);
	return type == GRAM_NODE_FOLDER || type == GRAM_NODE_STUB;  // all items are valid sources
}

HTREEITEM CTreeViewGrammar::GetDropTarget(HTREEITEM hItem)
{
	// inhibit drop on the drop source or its parent
	if (hItem == m_hitemDrag || hItem == m_treeCtrl.GetParentItem(m_hitemDrag))
		return NULL;
	return hItem;
}

void CTreeViewGrammar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_dwDragStart = GetTickCount();
	CPaneView::OnLButtonDown(nFlags, point);
}

CString CTreeViewGrammar::GetState()
{
	if (!m_treeCtrl) return "";
  	return StateString(&m_treeCtrl,m_treeCtrl.GetRootItem(),STATE_GET_EXPANDED);
}

void CTreeViewGrammar::SetState(CString stateStr)
{
	//SetRedraw(false);
	StateFromString(&m_treeCtrl,m_treeCtrl.GetRootItem(),STATE_EXPAND_COLLAPSE,stateStr);
	//SetRedraw(true);
}

void CTreeViewGrammar::OnGrammarHelp() 
{
	HelpTopic(_T("Gram_Tab_Popup"));
}

void CTreeViewGrammar::GenerateAnalyzer(GRAM_GEN_TYPE type, bool askConfirm) 
{
	CString questionStr = _T("Do you want to overwrite the existing pass files?");

	if (!askConfirm || AfxMessageBox(questionStr, MB_YESNO) == IDYES) {

		StartWaitCursor();

		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		wnd->ClearOutput();
		GenerateSamples();

		VT_APP_STATE prevAppState = appState;
		appState = VT_APP_GENERATING;

		if (0 == m_nTimerIDGen)
			m_nTimerIDGen = SetTimer(ID_TIMER_GEN,75,NULL);
		wnd->AddDebugLine(_T("Generating rules: "),_T("start"));

		wnd->EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_GEN_ANALYZER,false);
		wnd->EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_STOP_PROCESS);
		wnd->EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_GEN_DIRTY_AND_RUN,false);
		wnd->EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_RUN,false);
		wnd->EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_RUN_NEXT,false);
		wnd->EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_GENERATELOGS,false);
		wnd->EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_VERBOSE_MODE,false);

		PeekHere();

#ifdef GEODUCK_
		if (appBoolVerbose)
			nlp->gui->SetStatus(GUI_STATUS_GENERATING);
#endif

		CString appdir = AppPath();
		_TCHAR buff[1001];
		StringToChar(buff,appdir,1000);
		bool ok = true;   // Track rule generation.   // 05/21/09 AM.
		if (rug == NULL)
			rug = new RUG(gram, nlp, buff, cg);
		switch (type) {
		case GRAM_GEN_ALL:
			ok = rug->rugGenerate(false,false);
			break;
		case GRAM_GEN_UNLOCKED:
			ok = rug->rugGenerate(false,true);
			break;
		case GRAM_GEN_DIRTY:
			ok = rug->rugGenerate(true);
			break;
		}

		theApp.m_pMainFrame->m_wndAnalyzerView.Refresh();
		theApp.m_pMainFrame->m_wndAnalyzerView.SetPassDirty();
		StopWaitCursor();
		m_boolGenerate = false;

#ifdef GEODUCK_
		if (appBoolVerbose) {
			nlp->gui->SetStatus(GUI_STATUS_NORMAL);
		}
#endif
		// These don't depend on verbose flag.  Moved them out.	// 05/06/01 AM.
		if (appState == VT_APP_STOP_COMMAND)
			wnd->AddDebugLine(_T("Generating rules: "),_T("aborted by user"));
		else if (!ok)	// 05/21/09 AM.
			wnd->AddDebugLine(_T("Generating rules: "),_T("ERRORS DETECTED"));
		else
			wnd->AddDebugLine(_T("Generating rules: "),_T("done"));

		if (m_nTimerIDGen) {
			KillTimer(m_nTimerIDGen);
			m_nTimerIDGen = 0;
		}
		appState = prevAppState;
	}
}

bool CTreeViewGrammar::IsGenerate()
{
	return m_boolGenerate;
}

void CTreeViewGrammar::OnGrammarDownloadgrammar() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->ServerArchive(_T("Grammar"));
}

CString CTreeViewGrammar::ArchiveGrammar()
{
	CString pathStr = KBPath(_T("user"));
	return ArchiveFiles(pathStr,_T("Grammar"),_T("*.kb"));
}

void CTreeViewGrammar::OnGrammarUploadgrammar() 
{
	CString localFileStr = ArchiveGrammar();
	CString remoteFileStr = StripPath(localFileStr,0);
	UploadArchive(_T("Grammar"),remoteFileStr,localFileStr);
}

void CTreeViewGrammar::OnGrammarLocalarchive() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->LocalArchive(_T("Grammar"));
}

void CTreeViewGrammar::OnGrammarArchivegrammar() 
{
	ArchiveGrammar();	
}

void CTreeViewGrammar::OnGrammarEditrules() 
{
	CString rulesPathStr = RulesPath(m_strConcept);

	if (FileExists(rulesPathStr)) {
		CVisualTextDoc *doc = (CVisualTextDoc *)AfxGetApp()->OpenDocumentFile(rulesPathStr);
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		theApp.m_pMainFrame->m_wndAnalyzerView.EditRulesFile(rulesPathStr);
		doc->m_concept = m_conceptSelected;
	}
	else
		AfxMessageBox(_T("No pass file for this concept."));
}

void CTreeViewGrammar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (m_GrammarNodeType == GRAM_NODE_SAMPLE)
		OnGrammarEditexamples();
	else if (m_GrammarNodeType == GRAM_NODE_RULE)
		OnGrammarEditrules();
	else
		OnGrammarEditattributes();
}

bool CTreeViewGrammar::IsConceptSampleRepeat(CONCEPT *sampleConcept, CString textPath,
											 long start, long end) 
{
	_TCHAR buff[1001];
	CString otherText;
	CONCEPT *phrase;
	CONCEPT *node;
	long otherStart;
	long otherEnd;

	if ((phrase = cg->findPhrase(sampleConcept))) {
		node = cg->firstNode(phrase);

		while (node) {
			cg->findVal(node, _T("text"), buff);
			otherText = buff;
			cg->findVal(node, _T("start"), otherStart);
			cg->findVal(node, _T("end"), otherEnd);

			if (otherText == textPath && otherStart == start && otherEnd == end)
				return true;
			node = cg->Next(node);
		}
	}

	return false;
}

void CTreeViewGrammar::OnGrammarEditattributes() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->OpenAttributeEditor(m_conceptSelected, false);
}

void CTreeViewGrammar::OnUpdateGrammarAddconcept(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CTreeViewGrammar::OnUpdateGrammarDeleteconcept(CCmdUI* pCmdUI) 
{
//	pCmdUI->Enable(appBoolAnalyzerLoaded && m_intItemCount > 0);		
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_treeCtrl.GetCount() && m_itemSelected);// SRP 010506
}

void CTreeViewGrammar::OnUpdateGrammarArchivegrammar(CCmdUI* pCmdUI) 
{
//	pCmdUI->Enable(appBoolAnalyzerLoaded && m_intItemCount > 0);
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_treeCtrl.GetCount() && m_itemSelected);// SRP 010506
}

void CTreeViewGrammar::OnUpdateGrammarUploadgrammar(CCmdUI* pCmdUI) 
{
//	pCmdUI->Enable(appBoolAnalyzerLoaded && m_intItemCount > 0);	
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_treeCtrl.GetCount() && m_itemSelected);// SRP 010506
}

void CTreeViewGrammar::OnUpdateGrammarEditattributes(CCmdUI* pCmdUI) 
{
//	pCmdUI->Enable(appBoolAnalyzerLoaded && m_intItemCount > 0);
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_treeCtrl.GetCount() && m_itemSelected);// SRP 010506
}

CONCEPT *CTreeViewGrammar::ConceptSelected()
{
	return m_conceptSelected;
}

CONCEPT *CTreeViewGrammar::SafeConceptSelected()
{
	if (m_GrammarNodeType == GRAM_NODE_SAMPLE && m_sampleSelectedConcept)
		return m_sampleSelectedConcept;
	return m_conceptSelected;
}

void CTreeViewGrammar::OnGrammarProperties() 
{
	CGrammarConceptDlg dlg;
	dlg.SetConcept(m_conceptSelected);
	dlg.DoModal();	
}

void CTreeViewGrammar::ChangeConceptName(CString newConceptNameStr) 
{
	m_treeCtrl.SetItemText(m_itemSelected,newConceptNameStr);
}

void CTreeViewGrammar::OnRButtonDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint point, ptScreen;
	::GetCursorPos(&ptScreen);
	
	point = ptScreen;
	m_treeCtrl.ScreenToClient(&point);

	logmsg(_T("CTreeViewGrammar::OnRbuttonDown"));	// 05/14/08 AM.
	//TreeViewSelectItem(this, point); // SRP 010207

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	if (!wnd->AnalyzerCreated())
		return;

	// TAKE CARE OF GRAYING ITEMS
	UINT disables[100];
	for (int i=0; i<100; i++)
		disables[i] = 0;
	int count = 0;

	if (m_treeCtrl.GetCount() == 0) {
		disables[count++] = ID_GRAMMAR_ADD_FOLDER;
		disables[count++] = ID_GRAMMAR_ADD_LABEL;
		disables[count++] = ID_GRAMMAR_ADD_RULE;

		disables[count++] = ID_GRAMMAR_DELETECONCEPT;
		disables[count++] = ID_GRAMMAR_DELETESAMPLES;
		disables[count++] = ID_GRAMMAR_DELETECHILDREN;
		disables[count++] = ID_GRAMMAR_FIND;

		disables[count++] = ID_GRAMMAR_SAMPLEFILE_IMPORT;
		disables[count++] = ID_GRAMMAR_SAMPLEFILE_EXPORT;

		disables[count++] = ID_GRAMMAR_EDITATTRIBUTES;
		disables[count++] = ID_GRAMMAR_HIGHLIGHTMATCHES;
		disables[count++] = ID_GRAMMAR_MARKFORGENERATION;
		disables[count++] = ID_GRAMMAR_PROPERTIES;
		disables[count++] = ID_GRAMMAR_EDITRULES;
		disables[count++] = ID_GRAMMAR_VIEWTREE;
	}
	else {
		CONCEPT *concept = NULL;
		if (m_itemSelected)
			concept = (CONCEPT *)m_treeCtrl.GetItemData(m_itemSelected);
		// bool boolHasSamples = ConceptHasSamples(concept);	// Moved from here.	// 03/21/01 AM.
		CString sampleFileStr;
		bool boolSampleInOneFile = SamplesInOneFile(m_itemSelected,sampleFileStr);

		CString rulesPathStr = RulesPath(m_strConcept);
		bool boolHasPassFile = FileExists(rulesPathStr);

		bool boolTextHasLogFiles = wnd->TextHasAnaLogFiles();

		// BLOCK ACCORDING TO TYPE
		GRAM_NODE_TYPE type =
			TypeFromString(KBGetValueString(concept,_T("type")));
		GRAM_NODE_TYPE parentType =
			TypeFromString(KBGetValueString(cg->Up(concept),_T("type")));
		bool boolHasSamples = false;												// 03/21/01 AM.
		if (type == GRAM_NODE_RULE)												// 03/21/01 AM.
			boolHasSamples = ConceptHasSamples(concept);	// Moved here.	// 03/21/01 AM.

		if (type != GRAM_NODE_STUB && type != GRAM_NODE_FOLDER)
			disables[count++] = ID_GRAMMAR_ADD_FOLDER;
		if (type != GRAM_NODE_RULE)
			disables[count++] = ID_GRAMMAR_ADD_LABEL;
		if (type == GRAM_NODE_LABEL || type == GRAM_NODE_RULE)
			disables[count++] = ID_GRAMMAR_ADD_RULE;

		// DISABLE EXPORT IF NOT A RULE CONCEPT OR HAS NO SAMPLES.		// 03/21/01 AM.
		if (type != GRAM_NODE_RULE || !boolHasSamples)						// 03/21/01 AM.
			disables[count++] = ID_GRAMMAR_SAMPLEFILE_EXPORT;

		// DISABLE IMPORT IF NOT A RULE CONCEPT.								// 03/21/01 AM.
		if (type != GRAM_NODE_RULE)												// 03/21/01 AM.
			disables[count++] = ID_GRAMMAR_SAMPLEFILE_IMPORT;					// 03/21/01 AM.

		// IF SAMPLE, DISABLE THESE:
		if (m_GrammarNodeType == GRAM_NODE_SAMPLE) {
			disables[count++] = ID_GRAMMAR_ADDCONCEPT;
			disables[count++] = ID_GRAMMAR_HIGHLIGHTMATCHES;
			disables[count++] = ID_GRAMMAR_MARKFORGENERATION;
			disables[count++] = ID_GRAMMAR_PROPERTIES;
			disables[count++] = ID_GRAMMAR_EDITRULES;
			disables[count++] = ID_GRAMMAR_VIEWTREE;
			disables[count++] = ID_GRAMMAR_ADD_RULE; // SRP 010504
		}

		if (!boolTextHasLogFiles) {
			disables[count++] = ID_GRAMMAR_HIGHLIGHTMATCHES;
			disables[count++] = ID_GRAMMAR_VIEWTREE;
		}

		if (!boolHasPassFile) {
			disables[count++] = ID_GRAMMAR_EDITRULES;
			disables[count++] = ID_GRAMMAR_VIEWTREE;
		}
		// SRP 010506
		if (!m_itemSelected || !m_treeCtrl.GetChildItem(m_itemSelected)) {
			disables[count++] = ID_GRAMMAR_DELETECHILDREN;
			disables[count++] = ID_GRAMMAR_DELETESAMPLES;
		}
	}

	// DO MENU	
	ClientToScreen(&point);
	PopupContextMenu(this,IDR_GRAMMAR,point,disables);

	//CPaneView::OnRButtonDown(nFlags, point);
}

void CTreeViewGrammar::OnGrammarDeletechildren() 
{
	if (!m_itemSelected || !m_treeCtrl.GetChildItem(m_itemSelected))
		return;

	CString selectedStr = m_treeCtrl.GetItemText(m_itemSelected);
	CString msgStr = _T("You sure you want to delete the children of \"");
	msgStr += selectedStr + _T("\"?");

	if (AfxMessageBox(msgStr, MB_YESNO) == IDYES) {
		cg->rmChildren(m_conceptSelected);
		Agram::makeDirty(m_conceptSelected,cg);
		DeleteTreeCtrlChildren(&m_treeCtrl,m_itemSelected);
		kbDirty = true;
		m_boolGenerate = true;
	}
}

void CTreeViewGrammar::OnGrammarAddlist() 
{
	CFileDialog fileDlg(true,_T("txt"),InputPath()+_T("\\*.txt"));
	if (fileDlg.DoModal() == IDOK) {
		CString filePathStr = fileDlg.GetPathName();
		GrammarAddlist(filePathStr);
	}
}

void CTreeViewGrammar::GrammarAddlist(CString filePathStr) 
{
	CStringList itemList;
	ReadFileToStringList(itemList,filePathStr);

	int i = 0;
	long start = 0L;
	long end = 0L;
	CString sampleStr;
	POSITION pos = itemList.GetHeadPosition();

	while (pos && (sampleStr = itemList.GetNext(pos))) {
		end = start + sampleStr.GetLength() - 1;
		AddConceptSample(sampleStr,filePathStr,start,end);
		start += sampleStr.GetLength() + 1;
	}
}

void CTreeViewGrammar::AddToSampleFile(CString filePathStr, CString newSampleStr,
									   long &start, long &end) 
{
	CString fullPathStr = AppPath(filePathStr);

	if (!FileExists(fullPathStr)) {
		CString msgStr;
		msgStr.Format(_T("Couldn't open up sample file: \"%s\""),fullPathStr);
		AfxMessageBox(msgStr);
		return;
	}

	AppendToFile(fullPathStr,newSampleStr);
	end = (long)GetFileLength(fullPathStr)-2;
	start = end - newSampleStr.GetLength()+1;
}

void CTreeViewGrammar::OnGrammarDeletesamples() 
{
	if (!m_itemSelected || !m_treeCtrl.GetChildItem(m_itemSelected))
		return;

	CString selectedStr = m_treeCtrl.GetItemText(m_itemSelected);
	CString msgStr = _T("You sure you want to delete all samples under \"");
	msgStr += selectedStr + _T("\"?");

	if (AfxMessageBox(msgStr, MB_YESNO) == IDYES)
		GrammarDeleteSamplesRecursive(m_itemSelected);
}

void CTreeViewGrammar::GrammarDeleteSamplesRecursive(HTREEITEM item) 
{
	HTREEITEM child = m_treeCtrl.GetChildItem(item);

	if (child) {
		while (child) {
			GrammarDeleteSamplesRecursive(child);
			child = m_treeCtrl.GetNextSiblingItem(child);
		}
	}

	if (item)
		GrammarDeleteSamples(item,true);
}

void CTreeViewGrammar::GrammarDeleteSamples(HTREEITEM item, bool oneLevel) 
{
	CONCEPT *concept = (CONCEPT *)m_treeCtrl.GetItemData(item);
	Agram::makeDirty(concept,cg);
	cg->prunePhrases(concept);
	DeleteTreeCtrlChildren(&m_treeCtrl,item,2,3,oneLevel);
	kbDirty = true;
	m_boolGenerate = true;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->ReIconKB(concept);
}

void CTreeViewGrammar::AdjustLabelAttributesToSampleFile(CString textPathStr) 
{
	HTREEITEM childItem = m_treeCtrl.GetChildItem(m_itemSelected);
	CString itemStr;
	long start = 0;
	long end = 0;
	int img;
	int imgSel;
	CONCEPT *concept;

	while (childItem) {
		itemStr = m_treeCtrl.GetItemText(childItem);
		m_treeCtrl.GetItemImage(childItem,img,imgSel);

		// IF A LABEL
		if (img == 2) {
			concept = (CONCEPT *)m_treeCtrl.GetItemData(childItem);
			end = start + itemStr.GetLength() - 1;
			AdjustLabelAttributeToSampleFile(concept,start,end,textPathStr);
			AddConceptSample(itemStr,textPathStr,start,end,10000);
			start += itemStr.GetLength() + 1;
		}
		childItem = m_treeCtrl.GetNextSiblingItem(childItem);
	}
}

void CTreeViewGrammar::AdjustLabelAttributeToSampleFile(CONCEPT *concept, int newStart,
														int newEnd, CString textPathStr) 
{
	HTREEITEM conceptItem = TreeItemFromConcept(concept,m_treeCtrl.GetRootItem());
	HTREEITEM parent = m_treeCtrl.GetParentItem(conceptItem);
	HTREEITEM item = m_treeCtrl.GetChildItem(parent);
	HTREEITEM sample;
	int image;
	int selImage;
	CONCEPT *sampleConcept;
	long start;
	long end;
	long conceptStart;
	long conceptEnd;
	cg->findVal(concept,_T("start"),conceptStart);
	cg->findVal(concept,_T("end"),conceptEnd);
	CString conceptStr = ConceptName(concept);
	CString debugStr;

	while (item) {
		m_treeCtrl.GetItemImage(item,image,selImage);
		debugStr = m_treeCtrl.GetItemText(item);

		if (image == 0) {
			sample = m_treeCtrl.GetChildItem(item);

			while (sample) {
				sampleConcept = (CONCEPT *)m_treeCtrl.GetItemData(sample);
				debugStr = m_treeCtrl.GetItemText(sample);

				if (sampleConcept) {
					cg->findVal(sampleConcept,_T("start"),start);
					cg->findVal(sampleConcept,_T("end"),end);

					if (start >= conceptStart && end <= conceptEnd) {
						ConceptAttributeUpdate(sampleConcept,_T("start"),newStart+start-conceptStart);
						ConceptAttributeUpdate(sampleConcept,_T("end"),newEnd+end-conceptEnd);
						ConceptAttributeUpdate(sampleConcept,_T("text"),textPathStr);
					}
				}
				sample = m_treeCtrl.GetNextSiblingItem(sample);
			}
		}
		item = m_treeCtrl.GetNextSiblingItem(item);
	}
}

void CTreeViewGrammar::RegenerateSampleFile(CONCEPT *concept, CString filePathStr) 
{
	StartWaitCursor();
	CString conceptStr = ConceptName(concept);
	HTREEITEM conceptTreeItem = TreeItemFromConcept(concept,m_treeCtrl.GetRootItem());
	m_treeCtrl.SelectItem(conceptTreeItem);
	GrammarDeleteSamples(m_itemSelected);
	GrammarAddlist(AppPath(filePathStr));
	StopWaitCursor();
}

HTREEITEM CTreeViewGrammar::TreeItemFromConcept(CONCEPT *concept, HTREEITEM item)
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

void CTreeViewGrammar::OnGrammarHighlightmatches() 
{
	GrammarHighlightmatches();
	appBoolHighlightText = 1;
}

void CTreeViewGrammar::GrammarHighlightmatches(bool warning) 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString textProcessStr = wnd->GetProcessText();

	if (wnd->IsSelectedTextOpen()) {
		if (textProcessStr != _T(""))
			wnd->HighlightMatches(m_intPass);
		else if (warning)
			AfxMessageBox(_T("No processed text chosen"));
	}
}

CString CTreeViewGrammar::GetSelectedConcept() 
{
	return m_strConcept;
}

void CTreeViewGrammar::OnGrammarViewtree() 
{
	CString logPathStr;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->GetLogPath(m_intPass,logPathStr);
	if (logPathStr != _T(""))
		wnd->OpenParseTree(m_strConcept,logPathStr);
}

void CTreeViewGrammar::SelectConcept(CString conceptStr) 
{
	HTREEITEM conceptItem = TreeItemFromName(&m_treeCtrl,m_treeCtrl.GetRootItem(),conceptStr);
	if (conceptItem)
		m_treeCtrl.SelectItem(conceptItem);
}

CONCEPT *CTreeViewGrammar::ConceptFromName(CString conceptStr) 
{
	HTREEITEM conceptItem = TreeItemFromName(&m_treeCtrl,m_treeCtrl.GetRootItem(),conceptStr);
	CONCEPT *concept = NULL;
	if (conceptItem)
		concept = (CONCEPT *)m_treeCtrl.GetItemData(conceptItem);

	return concept;
}

void CTreeViewGrammar::OnGrammarMarkforgeneration() 
{
	Agram::makeDirty(m_conceptSelected,cg);
}

CONCEPT *CTreeViewGrammar::ConceptWithAttribute(CONCEPT *concept, CString attrStr,
												CString valStr)
{
	if (!concept)
		return NULL;

	_TCHAR valBuff[1001];
	_TCHAR attrBuff[1001];
	StringToChar(attrBuff,attrStr,1000);

	CString vStr;
	CString conceptStr;
	CONCEPT *childConcept;
	CONCEPT *returnConcept;

	while (concept) {
		conceptStr = ConceptName(concept);
		cg->findVal(concept,attrBuff,valBuff);
		vStr = valBuff;

		if (valStr == vStr)
			return concept;

		if ((childConcept = cg->Down(concept))) {
			returnConcept = ConceptWithAttribute(childConcept,attrStr,valStr);
			if (returnConcept)
				return returnConcept;
		}
		concept = cg->Next(concept);
	}

	return NULL;
}

void CTreeViewGrammar::OnGrammarDisassociatefile() 
{
	_TCHAR buff[1001];
	cg->findVal(m_conceptSelected,_T("sampfile"),buff);
	CString relFilePathStr = buff;
	CString msgStr;
	msgStr.Format(_T("Do you want to dissassociate the sample file '\"%s\"\nattached to the concept: \"%s\"?"), relFilePathStr, m_strConcept);
	if (AfxMessageBox(msgStr, MB_YESNO) == IDYES) {
		_TCHAR buff[1001];
		_tcscpy_s(buff,_T("sampfile"));
		Agram::rmAttr(m_conceptSelected,buff,cg);		// 08/28/99 AM.
		kbDirty = true;
		ChangeDocType(relFilePathStr,DOC_FILE);
	}
}

void CTreeViewGrammar::ChangeDocType(CString relFilePathStr, DOC_TYPE docType) 
{	
	CVisualTextDoc *doc = (CVisualTextDoc *)
		theApp.IsVisualTextDocumentOpen(_T("VisualText"),AppPath(relFilePathStr));
	if (doc)
		doc->ChangeType(docType);
}

void CTreeViewGrammar::OnGrammarFind() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->FindWord(m_strConcept);
}

bool CTreeViewGrammar::CanMoveSelectedUp() 
{
	HTREEITEM firstSibling = TreeCtrlFirstSibling(&m_treeCtrl,m_itemSelected);
	return firstSibling != m_itemSelected;
}

bool CTreeViewGrammar::CanMoveSelectedDown() 
{
	bool canMove = m_treeCtrl.GetNextSiblingItem(m_itemSelected) ? true : false;
	HTREEITEM lastSibling = FindLastConceptSibling(&m_treeCtrl,m_itemSelected);
	return m_itemSelected != lastSibling;
}

void CTreeViewGrammar::OnGrammarAddstub() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	int pass = theApp.m_pMainFrame->m_wndAnalyzerView.SelectedPass();
	CString msgStr;
	BOOL bOK = true;
	if (0 == pass) {
		msgStr = _T("To add a stub, you need to first select a pass in the Analyzer Tab.\n\nWould you like to do one of the following:");
		bOK = false;
	}
	else {
		int numPasses = theApp.m_pMainFrame->m_wndAnalyzerView.m_treeCtrl.GetCount();
		if (1 == pass && numPasses >= 2) {
			// If a system lines pass (2) is present, disallow!
			CString strPassName = theApp.m_pMainFrame->m_wndAnalyzerView.CleanPassNameFromPass(2);
			if (_T("lines") == strPassName && theApp.m_pMainFrame->m_wndAnalyzerView.GetPassType(2) == ANAL_PASS_SYSTEM) {
				msgStr = _T("You are trying to add a stub between system passes.\n\nWould you like to do one of the following:");
				bOK = false;
			}
		}
	}
	if (!bOK) {
		AskUser(msgStr,IDR_WAVE_WHOOPS,2,1,_T("Go to Analyzer Tab"),_T("Cancel"),_T(""));
		if (appButtonReturn == 1)
			wnd->SelectTab(WORKSPACE_ANALYZER);
		return;
	}

 	if (AskForText(_T("Enter a Single Stub Concept Name:"))) {
		int pos = appAskText.Find(',');
		if (pos != -1)
			appAskText = appAskText.Left(pos);
		AddTopLevelConcept(appAskText,true);
	}
}

int CTreeViewGrammar::TreeItemCount()
{
	return m_treeCtrl.GetCount();
}

void CTreeViewGrammar::OnGrammarSamplefileConsolidate() 
{
	CString sampleFilePathStr;
	CString msgStr;

	// LET USER CHOOSE LOCATION OF THE NEW FILE OR CHOOSE AN EXISTING FILE
	CString filterStr = _T("Text files (*.txt)|*.txt|All files (*.*)|*.*||");
	CFileDialog fileDlg(true,_T("txt"),InputPath()+_T("\\unnamed.txt"),NULL,filterStr);
	if (fileDlg.DoModal() == IDOK) {
		sampleFilePathStr = fileDlg.GetPathName();
	}
	else
		return;

	// GET SAMPLES FROM SELECTED CONCEPT AND MAKE A TEXT FILE
	msgStr.Format(_T("The file \"%s\" already exists.\nDo you want to overwrite it?"),
		sampleFilePathStr);
	if (FileExists(sampleFilePathStr) && AfxMessageBox(msgStr,MB_YESNO) != IDYES)
		return;

	CString filePathStr = RelativizePath(sampleFilePathStr);
	ConceptAttributeUpdate(m_conceptSelected,_T("sampfile"),filePathStr);
	GenerateSampleFromConcept();
}

void CTreeViewGrammar::OnGrammarSamplefileOpen() 
{
	_TCHAR buff[1001];
	cg->findVal(m_conceptSelected,_T("sampfile"),buff);
	CString relFilePathStr = buff;

	if (relFilePathStr != _T("")) {
		CString fullPathStr = AppPath(relFilePathStr);
		theApp.OpenDocumentFile(fullPathStr);
	}
}

void CTreeViewGrammar::OnGrammarSamplefileSet() 
{
	CString relPathStr;
	CString msgStr;
	bool acceptAutoSampleDetect = false;

	// CHECK TO SEE IF THERE ALREADY IS ONE
	if (SamplesInOneFile(m_itemSelected,relPathStr)) {
		msgStr.Format(_T("\"%s\" already appears to be your sample file.\nDo you want to associate this file?"),
			relPathStr);
		if (AfxMessageBox(msgStr,MB_YESNO) == IDYES)
			acceptAutoSampleDetect = true;
	}

	// ASK USER TO CHOOSE DEPENDING
	if (!acceptAutoSampleDetect) {
		CString filterStr = _T("Text files (*.txt)|*.txt|All files (*.*)|*.*||");
		CFileDialog fileDlg(true,_T("txt"),InputPath()+_T("\\*.txt"),NULL,filterStr);
		if (fileDlg.DoModal() == IDOK) {
			CString filePathStr = fileDlg.GetPathName();
			relPathStr = RelativizePath(filePathStr);
		}
		else
			return;
	}

	msgStr.Format(_T("You're associating the file \"%s\" with the concept \"%s\".\nAny existing samples below this concept will be deleted.\nProceed?"),
		relPathStr,m_strConcept);

	if (acceptAutoSampleDetect || AfxMessageBox(msgStr,MB_YESNO) == IDYES) {
		ConceptAttributeUpdate(m_conceptSelected,_T("sampfile"),relPathStr);
		if (!acceptAutoSampleDetect)
			RegenerateSampleFile(m_conceptSelected, relPathStr);
		ChangeDocType(relPathStr,DOC_SAMPLE);
	}
}

void CTreeViewGrammar::OnGrammarSamplefileUpdate() 
{
	StartWaitCursor();
	GenerateSampleFromConcept(m_itemSelected);
	StopWaitCursor();
}

void CTreeViewGrammar::OnGrammarAddFolder() 
{
	if (AskForText(_T("Enter Folder Name(s):\n(more than one separated by commas)"))) {
		AddConcept(appAskText,m_itemSelected,GRAM_NODE_FOLDER);
	}
}

void CTreeViewGrammar::OnGrammarAddLabel() 
{
	if (AskForText(_T("Enter Label Name(s):\n(more than one separated by commas)"))) {
		AddConcept(appAskText,m_itemSelected,GRAM_NODE_LABEL);
	}
}

void CTreeViewGrammar::OnGrammarAddRule() 
{
	if (AskForText(_T("Enter Rule Name(s):\n(more than one separated by commas)"))) {
		AddConcept(appAskText,m_itemSelected,GRAM_NODE_RULE);
	}
}

void CTreeViewGrammar::OnGrammarSamplefileExport() 
{
	CString filterStr = _T("Text files (*.txt)|*.txt|All files (*.*)|*.*||");
	CString fileNameStr = InputPath() + _T("\\") + m_strConcept + _T(".txt");
	CFileDialog fileDlg(true,_T("txt"),fileNameStr,NULL,filterStr);

	if (fileDlg.DoModal() == IDOK) {
		CString contentStr = _T("");
		CString filePathStr = fileDlg.GetPathName();
		CString conceptStr;
		CONCEPT *concept = m_conceptSelected;
		PHRASE *phrase = cg->findPhrase(concept);

		while (phrase) {
			conceptStr = ConceptName(phrase);
			if (contentStr != _T(""))
				contentStr += _T("\n");
			contentStr += conceptStr;
			phrase = cg->Next(phrase);
		}

		if (contentStr != _T(""))
			NLPWriteFile(filePathStr,contentStr);
	}
}

void CTreeViewGrammar::OnGrammarSamplefileImport() 
{
//	if (AfxMessageBox("Loading samples from a file deletes all current samples in the KB.\nDo you want to continue?",MB_YESNO)
	if (AfxMessageBox(_T("Importing samples from a file deletes all current samples in the selected concept.\nDo you want to continue?"),MB_YESNO) // 03/21/01 AM
		== IDYES) {

		CString filterStr = _T("Text files (*.txt)|*.txt|All files (*.*)|*.*||");
		CString fileNameStr = InputPath() + _T("\\*.txt");
		CFileDialog fileDlg(true,_T("txt"),fileNameStr,NULL,filterStr);

		if (fileDlg.DoModal() == IDOK) {
			CString filePathStr = fileDlg.GetPathName();
			CStringList words;
			ReadFileToStringList(words,filePathStr);

			if (words.GetCount() == 0) {
				AfxMessageBox(_T("This file is empty. Aborting load."));
			}
			else {
				GrammarDeleteSamplesRecursive(m_itemSelected);
				DeleteTreeCtrlChildren(&m_treeCtrl,m_itemSelected);
				POSITION pos = words.GetHeadPosition();
				CString wordStr;
				long start = 0;
				long end = 0;

				while (pos && (wordStr = words.GetNext(pos))) {
					end = start + wordStr.GetLength() - 1;
					AddConceptSample(wordStr,filePathStr,start,end);
					start = end + 2;
				}
			}
		}
	}
}

void CTreeViewGrammar::DeleteStubByName(CString stubNameStr) 
{
	HTREEITEM item = m_treeCtrl.GetRootItem();
	CString itemStr;
	CString stubNameCleanStr = stubNameStr;
	if (stubNameCleanStr.Find(_T("stub_")) == 0) {
		ClipLeft(stubNameCleanStr,5);
		stubNameCleanStr.MakeLower();
	}

	while (item) {
		itemStr = m_treeCtrl.GetItemText(item);
		itemStr.MakeLower();
		
		if (itemStr == stubNameCleanStr) {
			m_treeCtrl.SelectItem(item);
			DeleteSelected(true);
		}

		item = m_treeCtrl.GetNextSiblingItem(item);
	}
}

int CTreeViewGrammar::PassNumber() 
{
	return m_intPass;
}
