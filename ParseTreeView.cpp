/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// ParseTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"

#include "MainFrm.h"
#include "AttributeEditor.h"
#include "ParseTreeItemInfo.h"
#include "VisualTextView.h"
#include "TreeViewAnalyzer.h"
#include "SyntaxEditorDoc.h"
#include "SyntaxEditorView.h"
#include "ParseTreeDoc.h"
#include "ParseTreeView.h"
#include "SemPopup.h"

extern CG *cg;
extern VT_APP_STATE appState;
extern bool appBoolSizeDynamically;
extern bool appBoolTreeOffsets;
extern bool appBoolNodeSemantics;
extern bool appBoolTreeText;	// 07/12/08 AM.

#ifdef UNICODE
extern CFont appFontFixed;
#endif
extern CFont appFontFixed;	// 09/09/08 AM.
extern CFont appFontFixedSmall;	// 09/09/08 AM.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString appURL;
extern CVisualTextApp theApp;
extern VT_APP_STATE appState;

#define ID_TIMER_SEMPOPUP 100
#define ID_SEMPOPUP		4000

// Enlarging max parse tree size from 10000.	// 01/21/02 AM.
// Enlarging from 20000	// 11/05/02 AM.
#define BUFFER_SIZE 100000

// Line length should not be buffer size.	// 11/05/02 AM.
#define LINEBUF_SIZE 2048

#define TREE_PARENT_LIMIT 100

// For this session, sizes of trees user will tolerate.	// 01/24/02 AM.
// Global.  May want it in mainfrm.h or something like that.	// 01/24/02 AM.
static int g_askLength = BUFFER_SIZE;	// 01/24/02 AM.

/////////////////////////////////////////////////////////////////////////////
// CParseTreeView

IMPLEMENT_DYNCREATE(CParseTreeView, CTreeView)

LPTSTR m_lpToolTipText = NULL;
LPWSTR m_lpWToolTipText = NULL;

CParseTreeView::CParseTreeView()
{
	InitVars();
}

CParseTreeView::~CParseTreeView()
{
	DeleteItemData();
	if (m_lpToolTipText) {
		free(m_lpToolTipText);
		m_lpToolTipText = NULL;
	}
	if (m_lpWToolTipText) {
		free(m_lpWToolTipText);
		m_lpWToolTipText = NULL;
	}
}

void CParseTreeView::InitVars()
{
	// Print the text from the tree, with whitespace added as needed.	// 07/11/08 AM.
//	m_boolPrettyText = appBoolTreeText;	// 07/11/08 AM.

	m_boolConceptsOnly = false;
	m_boolFillOneLevel = false;
	m_boolEntireTree = false;
	m_boolProgramExpanding = false;
	m_intLevel = 0;
	m_itemParent = NULL;
	m_itemInfoRoot = NULL;
	m_lastItemInfo = NULL;
	m_lpToolTipText = NULL;
	m_lpWToolTipText = NULL;
	m_SemPopupTimerID = 0;
	m_SemPopupItem = NULL;
	m_pSemPopup = NULL;
}

BEGIN_MESSAGE_MAP(CParseTreeView, CTreeView)
	//{{AFX_MSG_MAP(CParseTreeView)
	ON_COMMAND(ID_PARSETREE_OPENALL, OnParsetreeOpenall)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_PARSETREE_CLOSEALL, OnParsetreeCloseall)
	ON_COMMAND(ID_PARSETREE_OPENITEMALL, OnParsetreeOpenitemall)
	ON_COMMAND(ID_PARSETREE_MAKEITEMROOT, OnParsetreeMakeitemroot)
	ON_COMMAND(ID_PARSETREE_DISPLAYENTIRETREE, OnParsetreeDisplayentiretree)
	ON_COMMAND(ID_PARSETREE_OPENONELEVEL, OnParsetreeOpenonelevel)
	ON_COMMAND(ID_PARSETREE_FIND, OnParsetreeFind)
	ON_COMMAND(ID_PARSETREE_HIGHLIGHTTEXT, OnParsetreeHighlighttext)
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_WM_CREATE()
	ON_COMMAND(ID_PARSETREE_EDITDICTIONARYENTRY, OnParsetreeEditdictionaryentry)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_UPDATE_COMMAND_UI(ID_PARSETREE_DISPLAYENTIRETREE, OnUpdateParsetreeDisplayentiretree)
	ON_UPDATE_COMMAND_UI(ID_PARSETREE_OPENITEMALL, OnUpdateParsetreeOpenitemall)
	ON_UPDATE_COMMAND_UI(ID_PARSETREE_MAKEITEMROOT, OnUpdateParsetreeMakeitemroot)
	ON_UPDATE_COMMAND_UI(ID_PARSETREE_HIGHLIGHTTEXT, OnUpdateParsetreeHighlighttext)
	ON_UPDATE_COMMAND_UI(ID_PARSETREE_FIND, OnUpdateParsetreeFind)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_NCMOUSEMOVE()
	ON_COMMAND(ID_PARSETREE_VIEWRULEFIRED, OnParsetree)
	ON_UPDATE_COMMAND_UI(ID_PARSETREE_VIEWRULEFIRED, OnUpdateParsetree)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WMU_DESTROY_SEMPOPUP, OnDestroySemPopup)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNeedText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNeedText)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParseTreeView drawing

void CParseTreeView::OnDraw(CDC* pDC)
{
	logmsg(_T("ParseTreeView::OnDraw"));
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CParseTreeView diagnostics

#ifdef _DEBUG
void CParseTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CParseTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CParseTreeView message handlers

#define TREE_ICON_COUNT 2

void CParseTreeView::Redisplay()
{
	logmsg(_T("ParseTreeView::Redisplay"));
	FillParseTree();
	ExpandOneLevel();
}

CString CParseTreeView::GetLogPathStr()
{
	return m_strLogPath;
}

CString CParseTreeView::GetFancyTitle()
{
	CString titleStr = "";
	CString numStr = StripPath(m_strLogPath,0);

	ClipLeft(numStr,3);
	ClipRight(numStr,4);
	int pass = _ttoi((const _TCHAR *)numStr);
	CString cleanNameStr = "";
	//cleanNameStr = theApp.m_pMainFrame->m_wndAnalyzerView.CleanPassNameFromPass(pass);  ddxxx

	long start = m_longStart >= 0 ? m_longStart : 0;
	long end = m_longEnd >= 0 ? m_longEnd : m_longFileLength;

	// SRP 010508
	// if CTreeViewAnalyzer::GetLogFile() sets m_strLogPath to "final.log",
	// pass will evaluate to zero
	if (0 == pass) // SRP 010508
		titleStr.Format(_T("%s: %s (Final %d-%d)"), m_strTextPath,m_strConcept,start,end);
	else
		titleStr.Format(_T("%s: %s (%d %s %d-%d)"), m_strTextPath,m_strConcept,pass,cleanNameStr,start,end);

	return titleStr;
	//CDocument *doc = GetDocument();
	//doc->SetTitle(titleStr);
}

// OBSOLETE.	// OPT.	// 12/04/06 AM.
// SRP 010611
// Extract and Save off Semantic Info
// Parameter 1 (strLine) is for Tree Item Display
// Parameter 2 (strText) is for ToolTip display of Text (text contains original CR's, etc.)
// Parameter 3 (strSem) is for ToolTip Display of Semantic Node Variables, following Text
void CParseTreeView::GetToolTipTextInfo(CString& strLine, CString& strText, CString& strSem,
	CString &lineSem)	// 06/27/02 AM.
{
	strSem.Empty();
//	strText = strLine;	// 06/27/02 AM.
	strText = lineSem;	// Always putting semantics into tooltip. // 06/27/02 AM.
	int nPos1, nPos2;
	int maxLen = strText.GetLength();
	while (-1 != (nPos1 = strText.Find(_T("(\""), 0)) ) {
		if (-1 == (nPos2 = strText.Find(_T(")"), nPos1 + 2)) )
			break;
		int nCount = nPos2 - nPos1 + 1;
		strSem += strText.Mid(nPos1, nCount);
		if (' ' == strText.GetAt(nPos1 - 1)) {
			--nPos1;
			nCount++;
		}
		if (strText.GetLength() > nPos2 + 1 && ' ' == strText.GetAt(nPos2 + 1))
			nCount++;
		strText.Delete(nPos1, nCount);
	}
	if (!strSem.IsEmpty())
		strSem.Replace(_T(")("), _T(")\n("));

	// Truncate Parse Tree Item Display Text.
	int maxChars = PARSE_TREE_LINE_MAX;
	if (strLine.GetLength() > maxChars) {
		strLine = strLine.Left(maxChars);
		strLine += _T("...");
	}

	// REPLACE SLASH N FOR DISPLAY PURPOSES
	strLine.Replace(_T("\n"),_T(" "));
	strLine.Replace(_T("\r"),_T(" "));
	strLine.Replace(_T("\t"),_T(" "));

}

extern bool appBoolTreeSemantics;	// 06/27/02 AM.


// OPT:	Can optimize by reading and parsing file in a single traversal.	// 11/30/06 AM.
void CParseTreeView::FillParseTree(bool appendFlag)
{
	logmsg(_T("ParseTreeView::FillParseTree"));
	m_boolFillingTree = true;

	StartWaitCursor();
	SetRedraw(false);

	//if (m_strConcept == "" || m_longStart >= 0)
	if (m_strConcept == _T(""))
		m_strConcept = _T("_ROOT");

	//SetFancyTitle();

	if (!appendFlag && appState != VT_APP_LOADING) {
		SetRedraw(false);
		GetTreeCtrl().DeleteAllItems();
		SetRedraw(true);
		DeleteItemData();
	}

	if (!FileExists(m_strLogPath)) {
		SetRedraw(true);
		StopWaitCursor();
		return;
	}

	bool firstFound = false;

	CString cStr = NLPReadFile(m_strLogPath);

	GetTreeCtrl().UpdateWindow();

	VT_APP_STATE lastState = appState;
	appState = VT_APP_GENERATING_TREE;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_STOP_PROCESS);

	HTREEITEM hChild = NULL;
	HTREEITEM hParent = NULL;
	HTREEITEM hParents[TREE_PARENT_LIMIT];
	
	// Add to last sibling, if level hasn't changed.	// OPT.	// 12/02/06 AM.
	HTREEITEM hSib = TVI_LAST;	// OPT.	// 12/02/06 AM.

	for (int i=0; i<TREE_PARENT_LIMIT; i++)
		hParents[i] = NULL;
	hParents[0] = m_itemParent;

	int indent = 0;
	int lastIndent = 0;
	CString lineStr;
	CString trimmedLineStr;
	CString conStr;
	CString rangeStr;
	CString semStr;	// eg (node ("var1" "val1") ("var2 val2"))
	CString semVars;	// eg (("var1" "val1") ("var2 val2"))
	CString dataStr;
	// Todo:	Fix flaky logic.  Eg, inside is never turned off, once on.	// 12/02/06 AM.
	bool inside = false;
	bool found = false;
	bool bConceptNode;	// If reduced node ('_' is 1st char).	// 11/30/06 AM.
	bool inRange = false;
	int lastOne = false;
	int rootIndent = -1;	// Default value.	// 12/02/06 AM.
	long startSel;
	long endSel;
	int passNum;
	int lineNum = 0;
	int maxIndent = 0;  // Help speed
		// Looks like maxindent updated if current indent is greater than maxIndent.
		// Also, if current indent is less than lastIndent.
		// Ie, maxindent appears no different than indent.	// 11/30/06 AM.
	bool firedFlag,builtFlag;

	m_boolEntireTree = m_strConcept == _T("_ROOT") ? true : false;

    int iItem = 0;
	int image,selImage;
	int level = 0;
	int lineCount = 0;
	int lineIndex = 0;
	int lineIndex2 = 0;
	int len = cStr.GetLength();
	int i = 0;
    CParseTreeItemInfo *pItemInfo;
    
	while (lineIndex2 >= 0) {

		if (appState != VT_APP_GENERATING_TREE)
			break;

		// OPT: Redundant parse of lines here. Can do a single read and parse of file. // 12/01/06 AM.
		lineNum++;
		lineIndex2 = cStr.Find(_T("\n"), lineIndex);
		if (lineIndex2 == -1)
			lineStr = cStr.Mid(lineIndex);
		else
			lineStr = cStr.Mid(lineIndex, lineIndex2 - lineIndex);
		//PeekHere(10);

		lineIndex = lineIndex2 + 1;
		if (lineIndex2 > len)
			break;

		if (!lineStr || lineStr == _T(""))
			continue;

		// Looks flaky logic. Name could be redundant. // 12/02/06 AM.
		if (!found)		// OPT.	// 12/02/06 AM.
			found = lineStr.Find(m_strConcept) >= 0;

		// MUST SKIP PAST HEADER!!!
		if (!inside && !found)
			continue;
		inside = true;

		// Moved this line up before getting new indent.	// FIX?	// 11/30/06 AM.
		lastIndent = indent;

		// Since parsing every line, removed the redundant "quick parse".	// OPT.	// 11/30/06 AM.

		// PARSE THE LOG LINE
		ParseLogLine(m_strLogText,lineStr,lineCount,startSel,endSel,passNum,lineNum,
			indent,conStr,semStr,
			semVars,	// Var-values with \n for tool tips.	// OPT.	// 12/01/06 AM.
			rangeStr,firedFlag,builtFlag,
			false,	// Not just a quick parse.	// 12/02/06 AM.
			false);

		if (conStr.GetAt(0) == '_'		// 11/30/06 AM.
		 && conStr.GetLength() > 1)	// 08/30/08 AM.
			bConceptNode = true;			// 11/30/06 AM.
		else									// 11/30/06 AM.
			bConceptNode = false;		// 11/30/06 AM.

		// Moved below the full parse.	// OPT.	// 11/30/06 AM.
		if (m_boolConceptsOnly && !bConceptNode)
			continue;

		//PeekHere(10);

		// Moved below parse of line.	// 11/30/06 AM.
		// SET IMAGE TYPE
		image = 2;
		selImage = 3;
		if (bConceptNode) {
			image = 0;
			selImage = 1;
		}

		// Moved below parse of line.	// 11/30/06 AM.
		// CHECK FOR OPENING ONLY WHAT IS NEEDED
		level = indent / 3;
		//if (level > m_intLevel)
			//continue;

		if (indent >= TREE_PARENT_LIMIT)
			continue;

		if (indent > maxIndent)
			maxIndent = indent;

		// CHECK TO SEE IF LINE IS WITHIN START AND END SELECTION
		if (m_longStart >=0) {
			inRange = startSel >= m_longStart;

			// SECOND PART OF CONDITION BLOCKS FOR INCLUDING THE NODE BEING EXPANDED
			if (!inRange)
				continue;

			else if (inRange) {
				if (startSel >= m_longEnd)
					lastOne = 2;
				else
					inside = true;
			}
		}

		// MAKE LINE STRING FOR TREE ITEM
		FormatTreeItemText(lineStr,conStr,startSel,endSel,rangeStr,	// 07/12/08 AM.
			semStr,appBoolTreeSemantics,appBoolTreeOffsets,bConceptNode,appBoolTreeText);

		// ALWAYS PUT SEMANTICS IN THE POPUP!	// 06/27/02 AM.
		// Todo: In order to have tree text for sem (yellow box), we'd need an array to store the
		// variables of the parents, waiting till text is completed.  // 07/12/08 AM.
		CString lineSem;	// 06/27/02 AM.
		if (appBoolTreeSemantics)	// 06/27/02 AM.
			lineSem = lineStr;	// 06/27/02 AM.
		else if (semStr != _T("")) {	// 06/27/02 AM.
			if (appBoolTreeOffsets && !bConceptNode)
				lineSem.Format(_T("%s::(%d,%d,%s)   %s"),(LPCTSTR)conStr,startSel,endSel,(LPCTSTR)semStr,(LPCTSTR)rangeStr);
			else
				lineSem.Format(_T("%s::(%s)   %s"),(LPCTSTR)conStr,(LPCTSTR)semStr,(LPCTSTR)rangeStr);
		}

		if ((rootIndent == -1) && found)	// 12/02/06 AM.
			rootIndent = indent;

		if (indent == lastIndent)	// Continue in same list.	// OPT.	// 12/02/06 AM.
			{
			if (indent == 0)			// OPT.	// 12/02/06 AM.
				hParent = TVI_ROOT;	// OPT.	// 12/02/06 AM.
			}
		else
			{
			// Reset the last sibling added.
			hSib = TVI_LAST;			// OPT.	// 12/02/06 AM.

			// IF INDENT IS LESS THAN BEFORE, THEN CLEAR THE PARENTS
			// Not sure why, unless worried that tree has skips in indent.	// 11/30/06 AM.
			// I guess in a concepts-only tree there are such skips.	// 11/30/06 AM.
			if (indent < lastIndent)
				{
				for (i=maxIndent; i>indent; i--)
					hParents[i] = NULL;
				maxIndent = indent;
				if (!m_boolEntireTree && rootIndent == indent)
					inside = false;
				}

			// IF THERE IS AN INDENT, GET ITS PARENT ITEM
			// Not sure why a loop, unless worried about skips in indent level.	// 11/30/06 AM.
			// I guess in a concepts-only tree there are such skips.	// 11/30/06 AM.
			hParent = NULL;
			if (indent)
				{
				for (i=indent-1; i>0; i--) {
					if (hParents[i] != NULL)
						break;
				}
				hParent = hParents[i];
				}
			}

		// ADD ACTUAL NODE
		if (inside && lastOne != 2) {
			// SRP 010611
//			CString strSem;										// OPT.	// 12/01/06 AM.
//			CString strText;										// OPT.	// 12/01/06 AM.
//			CString strLine = lineStr;							// OPT.	// 12/01/06 AM.
//			GetToolTipTextInfo(strLine, strText, strSem,	// OPT.	// 12/01/06 AM.
//				lineSem);	// 06/27/02 AM.					// OPT.	// 12/01/06 AM.

			// Todo: Copy once from C style strings.		// OPT.	// 12/02/06 AM.
			CString cp_semVars = semVars;						// OPT.	// 12/02/06 AM.

			// Get (initial) text for node.								// 07/12/08 AM.
			int maxChars = PARSE_TREE_LINE_MAX;
			CString cp_lineStr;									// OPT.	// 12/02/06 AM.
			MakeTreeItemText(cp_lineStr,lineStr,rangeStr,			// 07/11/08 AM.
					bConceptNode,appBoolTreeText,hParents,indent);

			CString cp_lineSem;									// OPT.	// 12/02/06 AM.
			if (lineSem.GetLength() > maxChars)
				{
				cp_lineSem = lineSem.Left(maxChars-4);
				cp_lineSem += _T("...");
				}
			else
				cp_lineSem = lineSem;	// Todo: remove copies.	// 12/02/06 AM.

			// INSERT THE NEW PARSE TREE NODE.
			hChild = GetTreeCtrl().InsertItem(
				cp_lineStr,	// strLine,		// OPT.	// 12/01/06 AM.
				image,selImage,hParent,
				hSib								// OPT.	// 12/02/06 AM.
				);
			hSib = hChild;	// Now the last sibling.	// 12/02/06 AM.
			if (!firstFound) {
				m_itemCurrentRoot = hChild;
			}

			firstFound = true;
			pItemInfo = new CParseTreeItemInfo(iItem++,hChild,conStr,
				startSel,endSel,passNum,lineNum,level,
				cp_lineSem,	// strText,		// OPT.	// 12/01/06 AM.
				cp_semVars	// strSem		// OPT.	// 12/01/06 AM.
				);
			if (!m_itemInfoRoot)
				m_itemInfoRoot = pItemInfo;
			if (m_lastItemInfo)
				m_lastItemInfo->SetNext(pItemInfo);
			m_lastItemInfo = pItemInfo;

			// IF LAST LEVEL AND 
 			//if (level == m_intLevel && bConceptNode)
				//GetTreeCtrl().InsertItem("hold",image,selImage,hChild);

		}
		hParents[indent] = hChild;

		// DO LINE COUNT FOR ADJUSTING STRING RETRIEVAL
		if (conStr == _T("\\n"))
			lineCount++;

		if (lastOne)
			break;

		lineIndex = lineIndex2 + 1;
		if (lineIndex2 > len)
			break;
	}

	m_itemCurrentRoot = GetTreeCtrl().GetRootItem();

	if (m_itemCurrentRoot != NULL) {
		GetTreeCtrl().SelectItem(m_itemCurrentRoot);
		m_itemSelected = GetTreeCtrl().GetSelectedItem();

		appState = lastState;
		wnd->EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_STOP_PROCESS,false);

		SetRedraw(true);
	}

	StopWaitCursor();

	m_boolFillingTree = false;
}


/////////////////////
// Modularize formatting of text for a node.	// 07/12/08 AM.
// With pretty text tree flag, will keep revisiting the text of an item.
// Todo: Make this inline.
/////////////////////

inline void CParseTreeView::FormatTreeItemText(
	CString &lineStr,
	CString conStr,
	long startSel,
	long endSel,
	CString rangeStr,
	CString semStr,
	bool bTreeSem,	// If tree semantics
	bool bTreeOff,	// If tree offsets
	bool bConceptNode,	// If nonliteral node ("concept node")
	bool bPrettyText	// If getting text from tree rather than input buffer
	)
{
if (!bPrettyText)	// As before, based on input buffer.
	{
	if (!bTreeSem || semStr == _T(""))	// 06/27/02 AM.
		{
		// if (semStr == "") {	// 06/27/02 AM.
		if (bTreeOff && !bConceptNode)
			lineStr.Format(_T("%s::(%d,%d)   %s"),(LPCTSTR)conStr,startSel,endSel,(LPCTSTR)rangeStr);
		else
			lineStr.Format(_T("%s::   %s"),(LPCTSTR)conStr,(LPCTSTR)rangeStr);
		}
	else
		{
		if (bTreeOff && !bConceptNode)
			lineStr.Format(_T("%s::(%d,%d,%s)   %s"),(LPCTSTR)conStr,startSel,endSel,(LPCTSTR)semStr,(LPCTSTR)rangeStr);
		else
			lineStr.Format(_T("%s::(%s)   %s"),(LPCTSTR)conStr,(LPCTSTR)semStr,(LPCTSTR)rangeStr);
		}
	return;
	}

// New way, based on parse tree text, not input buffer.
if (!bTreeSem || semStr == _T(""))	// 06/27/02 AM.
	{
	// if (semStr == "") {	// 06/27/02 AM.
	if (bTreeOff && !bConceptNode)
		lineStr.Format(_T("%s::(%d,%d)   %s"),(LPCTSTR)conStr,startSel,endSel,(LPCTSTR)rangeStr);
	else if (!bConceptNode)	// If literal.	// 07/12/08 AM.
		lineStr.Format(_T("%s::   %s"),(LPCTSTR)conStr,(LPCTSTR)rangeStr);
	else	// Nonliteral.	// 07/12/08 AM.
		// Text will be fetched from literals under current nonliteral node.
		lineStr.Format(_T("%s::   "),(LPCTSTR)conStr); // Text from literals under here.
	
	}
else
	{
	if (bTreeOff && !bConceptNode)
		lineStr.Format(_T("%s::(%d,%d,%s)   %s"),(LPCTSTR)conStr,startSel,endSel,(LPCTSTR)semStr,(LPCTSTR)rangeStr);
	else if (!bConceptNode)	// If literal.	// 07/12/08 AM.
		lineStr.Format(_T("%s::(%s)   %s"),(LPCTSTR)conStr,(LPCTSTR)semStr,(LPCTSTR)rangeStr);
	else	// Nonliteral.	// 07/12/08 AM.
		// Text will be fetched from literals under current nonliteral node.
		lineStr.Format(_T("%s::(%s)   "),(LPCTSTR)conStr,(LPCTSTR)semStr);
	}

}


/////////////////////
// Modularize creation of text for a node.	// 07/11/08 AM.
// With pretty text tree flag, will keep revisiting the text of an item.
// Todo: Make this inline.
/////////////////////

inline void CParseTreeView::MakeTreeItemText(	// 07/11/08 AM.
	CString &cp_lineStr,	// Return the initial string.
	CString lineStr,
	CString conStr,
	bool bConceptNode,	// If nonliteral node
	bool bPrettyText,		// If displaying text in parse tree (rather than input buffer)
	HTREEITEM hParents[],
	int indent
	)
{
// Must keep strings short enough.							// 12/02/06 AM.
int maxChars = PARSE_TREE_LINE_MAX;
if (lineStr.GetLength() > maxChars)
	{
	cp_lineStr = lineStr.Left(maxChars-4);
	cp_lineStr += _T("...");
	}
else
	cp_lineStr = lineStr;	// Todo: remove copies.	// 12/02/06 AM.

// Bringing back the fix of newlines etc.	// FIX.	// 06/19/07 AM.
cp_lineStr.Replace(_T("\n"),_T(" "));	// FIX.	// 06/19/07 AM.
cp_lineStr.Replace(_T("\r"),_T(" "));	// FIX.	// 06/19/07 AM.
cp_lineStr.Replace(_T("\t"),_T(" "));	// FIX.	// 06/19/07 AM.

// If nonliteral or not getting text from tree, done.
if (bConceptNode || !bPrettyText || !indent || !cp_lineStr)
	return;	// Done.

// If literal and text from tree, fix up a text for the parents up the parse tree.
int i;
for (i=indent-1; i>=0; i--)
	{
	if (hParents[i] != NULL)
		{
		// Fix up ancestor text.
		CString cs = GetTreeCtrl().GetItemText(hParents[i]);
		long len = cs.GetLength();
		if (len >= maxChars) // Ancestors have maxed out.
			return;
		long len1 = conStr.GetLength();
		CString ch = cs.Right(1);
		CString chr = conStr.Left(1);
		long tot = len + len1;
		// Don't put space before sentence punct.  Need a switch here.
		if (chr == _T(",") || chr == _T(".") || chr == _T(";")
		 || chr == _T(":") || chr == _T("!") || chr == _T("?"))
		   ;
		else if (ch != _T(" "))
			{
			cs += _T(" ");
			++tot;
			}
		cs += conStr;
		if (tot > maxChars)
		  {
		  cs = cs.Left(maxChars-3);
		  cs += _T("...");
		  }
		GetTreeCtrl().SetItemText(hParents[i],cs);
		if (tot >= maxChars)
		  return;
		}
	}

}
/////////////////////

void CParseTreeView::DeleteItemData()
{
	if (m_itemInfoRoot) {
		CParseTreeItemInfo *info = m_itemInfoRoot;
		CParseTreeItemInfo *nextInfo;
		int debugCount = 0;
		while (info) {
			nextInfo = info->GetNext();
			delete info;
			info = nextInfo;
			debugCount++;
		}
		m_itemInfoRoot = NULL;
		m_lastItemInfo = NULL;
	}
}

// OPT:	This looks extremely inefficient.	// 08/22/08 AM.
CParseTreeItemInfo *CParseTreeView::GetItemDataFromItem(HTREEITEM treeItem)
{
	CParseTreeItemInfo *info = m_itemInfoRoot;
	if (m_itemInfoRoot) {
		while (info) {
			if (info->GetTreeItem() == treeItem)
				break;
			info = info->GetNext();
		}
	}
	return info;
}

CString CParseTreeView::OffsetDataStr(int startSel, int endSel) 
{
	CString dataStr;
	dataStr.Format(_T("%d:%d"),startSel,endSel);
	return dataStr;
}

void CParseTreeView::OnInitialUpdate() 
{
	m_icon = AfxGetApp()->LoadIcon(IDR_TREE);
	GetParentFrame()->SetIcon(m_icon,true);

	CTreeView::OnInitialUpdate();

//	CToolTipCtrl *tip = GetTreeCtrl().GetToolTips();
//	if (tip)
//		tip->SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
	EnableToolTips(false);

#ifdef UNICODE
// Actually, the line below messes up Hangul in parse tree!	// 01/31/06 AM.
//	SetFont(&appFontFixed);
#endif

// For multi-column yellow box, testing fixed font.	// 09/09/08 AM.
//SetFont(&appFontFixedSmall);	// 09/09/08 AM.

}

bool CParseTreeView::SetData(CString logPathStr, CString textPathStr) 
{
	return SetData(logPathStr,textPathStr,m_strConcept);
}

void CParseTreeView::ChangeData(CString logPathStr, CString textPathStr) 
{
	SetData(logPathStr,textPathStr,m_strConcept,m_longStart,m_longEnd,false);
}

void CParseTreeView::SetConceptOnlyFlag(bool conceptsOnlyFlag) 
{
	m_boolConceptsOnly = conceptsOnlyFlag;
}

bool CParseTreeView::SetData(CString logPathStr, CString textPathStr, CString conceptStr,
							 long startSel, long endSel, bool appendFlag) 
{
	m_strLogPath = logPathStr;
	m_strTextPath = textPathStr;
	m_longStart = startSel;
	m_longEnd = endSel;
	m_intLevel = 1;
//	int askLength = BUFFER_SIZE;	// 01/24/02 AM.

	m_strLogText = _T("");
	if (m_strTextPath != _T("")) {
		m_strLogText = NLPReadInputFile(InputPath(textPathStr));
		m_longFileLength = m_strLogText.GetLength();
	}

	// Minimize annoying re-ask of this question.	// 01/24/02 AM.
	int len = endSel - startSel;	// 01/24/02 AM.
	if (len == 0)	// 01/24/02 AM.
		len = m_longFileLength;	// 01/24/02 AM.
	if (appState != VT_APP_LOADING && len > g_askLength) {	// 01/24/02 AM.
		if (AfxMessageBox(_T("This tree is very large.  Do you want to continue?"),MB_YESNO) != IDYES)
			return false;
		else {	// 01/24/02 AM.
			// Increase the askLength by current + 50%.	// 01/24/02 AM.
			int tmp = (len + len / 2);	// 01/24/02 AM.
			if (tmp > len)	// Overflow check.	// 01/24/02 AM.
				g_askLength = tmp;	// Bigger threshold for asking question.	// 01/24/02 AM.
		}
	}

	FillParseTree(appendFlag);
	m_strRootConcept = m_strConcept;

	CString tit = GetFancyTitle();
	CParseTreeDoc *doc = (CParseTreeDoc *)GetDocument();
	doc->SetTitle(tit);

	return true;
}

BOOL CParseTreeView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	dwStyle |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CParseTreeView::OnParsetreeOpenall() 
{
	ExpandAllSiblings(GetTreeCtrl().GetRootItem(),TVE_EXPAND);
}

void CParseTreeView::ExpandAllSiblings(HTREEITEM item, UINT nCode) 
{
	SetRedraw(false);
	HTREEITEM sibling = m_itemCurrentRoot;
	CString debugStr;

	while (sibling) {
		debugStr = GetTreeCtrl().GetItemText(sibling);
		ExpandAll(sibling,nCode);
		sibling = GetTreeCtrl().GetNextSiblingItem(sibling);
	}
	GetTreeCtrl().SelectSetFirstVisible(GetTreeCtrl().GetRootItem());
	SetRedraw(true);
}

void CParseTreeView::ExpandAll(HTREEITEM item, UINT nCode) 
{
	m_boolProgramExpanding = true;
	TreeCtrlExpandAll(&(GetTreeCtrl()),item,nCode);
	m_boolProgramExpanding = false;
	GetTreeCtrl().SelectSetFirstVisible(item);
}

void CParseTreeView::ExpandOneLevel() 
{
	SetRedraw(false);
	m_boolProgramExpanding = true;
	TreeCtrlExpandOneLevel(&(GetTreeCtrl()), GetTreeCtrl().GetRootItem(), TVE_EXPAND);
	m_boolProgramExpanding = false;
	SetRedraw(true);
}

void CParseTreeView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_itemSelected = pNMTreeView->itemNew.hItem;
	
	*pResult = 0;
}

void CParseTreeView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTreeView::OnLButtonDown(nFlags, point);
	m_itemSelected = GetTreeCtrl().GetSelectedItem();
}

void CParseTreeView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	logmsg(_T("CParseTreeView::OnRbuttonDown"));	// 05/14/08 AM.
	TreeViewSelectItem(this, point); // SRP 010504

	// TAKE CARE OF GRAYING ITEMS
	UINT disables[100];
	for (int i=0; i<100; i++)
		disables[i] = 0;
	int count = 0;

	m_itemSelected = GetTreeCtrl().GetSelectedItem();
	bool enableViewRuleFired = false;
	if (m_itemSelected) {
		CParseTreeItemInfo *pInfo = GetItemDataFromItem(m_itemSelected);
		if (pInfo->GetPass())
			enableViewRuleFired = true;
	};

	if (!m_itemSelected) {
		disables[count++] = ID_PARSETREE_OPENITEMALL;
		disables[count++] = ID_PARSETREE_HIGHLIGHTTEXT;
		disables[count++] = ID_PARSETREE_FIND;
	}

	if (!m_itemSelected || m_itemCurrentRoot == m_itemSelected)
		disables[count++] = ID_PARSETREE_MAKEITEMROOT;

	if (m_boolEntireTree)
		disables[count++] = ID_PARSETREE_DISPLAYENTIRETREE;

	if (!enableViewRuleFired)
		disables[count++] = ID_PARSETREE_VIEWRULEFIRED;

	// DO MENU	
	ClientToScreen(&point);
	PopupContextMenu(this,IDR_PARSETREE_POPUP,point,disables);
}

void CParseTreeView::OnParsetreeCloseall() 
{
	ExpandAllSiblings(GetTreeCtrl().GetChildItem(m_itemCurrentRoot),TVE_COLLAPSE);
}

void CParseTreeView::OnParsetreeOpenitemall() 
{
	m_itemSelected = GetTreeCtrl().GetSelectedItem();

	if (m_itemSelected) {
		ExpandAll(GetTreeCtrl().GetChildItem(m_itemSelected),TVE_EXPAND);
		GetTreeCtrl().SelectSetFirstVisible(m_itemSelected);
		m_boolProgramExpanding = true;
		GetTreeCtrl().Expand(m_itemSelected,TVE_EXPAND);
		m_boolProgramExpanding = true;
	}
}

void CParseTreeView::OnParsetreeMakeitemroot() 
{
	MakeTreeItemRoot();
}

void CParseTreeView::OnParsetreeDisplayentiretree() 
{
	m_strConcept = m_strRootConcept;
	m_longStart = -1;
	m_longEnd = -1;

	FillParseTree();

	ExpandOneLevel();
}

void CParseTreeView::MakeTreeItemRoot() 
{
	m_itemSelected = GetTreeCtrl().GetSelectedItem();

	if (m_itemSelected) {
		CParseTreeItemInfo *pInfo = GetItemDataFromItem(m_itemSelected);
		m_strConcept = pInfo->GetConcept();
		m_longStart = pInfo->GetStart();
		m_longEnd = pInfo->GetEnd();

		FillParseTree();

		ExpandOneLevel();
	}
}

void CParseTreeView::OnParsetreeOpenonelevel() 
{
	ExpandOneLevel();
}

void CParseTreeView::OnParsetreeFind() 
{
	m_itemSelected = GetTreeCtrl().GetSelectedItem();

	if (m_itemSelected) {
		CParseTreeItemInfo *pInfo = GetItemDataFromItem(m_itemSelected);
		m_strConcept = pInfo->GetConcept();
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		wnd->FindWord(m_strConcept);
	}
}

void CParseTreeView::OnParsetreeHighlighttext() 
{
	m_itemSelected = GetTreeCtrl().GetSelectedItem();

	if (m_itemSelected) {
		CString itemStr = GetTreeCtrl().GetItemText(m_itemSelected);
		CParseTreeItemInfo *pInfo = GetItemDataFromItem(m_itemSelected);
		m_strConcept = pInfo->GetConcept();
		long start = pInfo->GetStart();
		long end = pInfo->GetEnd();

		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		wnd->SelectText(m_strTextPath,start,end);
	}
}

void CParseTreeView::OnDestroy() 
{
	StartWaitCursor();

	SaveState();
	DeleteItemData();
	OnDestroySemPopup(0,1);
	CTreeView::OnDestroy();

	StopWaitCursor();
}

void CParseTreeView::SaveState() 
{
	theApp.AddStatus(_T("treeopen"),StateStr());
}

CString CParseTreeView::StateStr() 
{
	int active = 0;
	CString selectedStr = StateString(&(GetTreeCtrl()),GetTreeCtrl().GetRootItem(),STATE_GET_SELECTED);
	CString stateStr = StateString(&(GetTreeCtrl()),GetTreeCtrl().GetRootItem(),STATE_GET_EXPANDED);

 	CString posStr;
	posStr.Format(_T("%s%c%s%c%s%c%d%c%d%c%s%c%s"),
		RelativizePath(m_strLogPath),STATUSSEP,
		m_strTextPath,STATUSSEP,
		m_strConcept,STATUSSEP,
		m_longStart,STATUSSEP,
		m_longEnd,STATUSSEP,
		selectedStr,STATUSSEP,
		stateStr
		);
	return posStr;
}

void CParseTreeView::SetState(CString stateStr, STATE_TYPE state)
{
	//SetRedraw(false);
	//StateFromString(&m_treeCtrl,GetTreeCtrl().GetRootItem(),state,stateStr);
	StateFromString(&(GetTreeCtrl()),GetTreeCtrl().GetRootItem(),state,stateStr);
	//SetRedraw(true);
}

void CParseTreeView::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM item = pNMTreeView->itemNew.hItem;

	/*
	if (item != GetTreeCtrl().GetRootItem()) {
		CParseTreeItemInfo *pInfo = GetItemDataFromItem(item);
		m_itemParent = item;
		m_strConcept = pInfo->GetConcept();
		m_longStart = pInfo->GetStart();
		m_longEnd = pInfo->GetEnd();
		m_intLevel = pInfo->GetLevel() + 1;

		// DELETE PLACE HOLDER ITEM
		HTREEITEM childItem = GetTreeCtrl().GetChildItem(item);
		if (childItem)
			GetTreeCtrl().DeleteItem(childItem);

		FillParseTree(true);
	}
	*/
	
	*pResult = 0;
}

int CParseTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// create the image list for the tree control
	m_ImageList.Create(IDB_PARSE_TREE, 16, 1, RGB(0,255,0));
	GetTreeCtrl().SetImageList(&m_ImageList, TVSIL_NORMAL);

	return 0;
}

void CParseTreeView::OnParsetreeEditdictionaryentry() 
{
	m_itemSelected = GetTreeCtrl().GetSelectedItem();

	if (m_itemSelected) {
		CParseTreeItemInfo *pInfo = GetItemDataFromItem(m_itemSelected);
		CString conceptStr = pInfo->GetConcept();

		_TCHAR buff[1001];
		StringToChar(buff,conceptStr,1000);
		CONCEPT *wordConcept = cg->getWordConcept(buff);

		if (wordConcept) {
			CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
			wnd->OpenAttributeEditor(wordConcept, false);
		}
		else {
			CString msgStr;
			msgStr.Format(_T("No entry for \"%s\""),conceptStr);
			AfxMessageBox(msgStr);
		}
	}
}

void CParseTreeView::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if (appBoolSizeDynamically && !m_boolProgramExpanding)
		SetWindowSize(this,&m_treeCtrl,true);

	*pResult = 0;
}

void CParseTreeView::OnFileNew() 
{
	theApp.OnFileNew();	
}

void CParseTreeView::OnUpdateParsetreeDisplayentiretree(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_boolEntireTree);
}

void CParseTreeView::OnUpdateParsetreeOpenitemall(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_itemSelected != NULL);
	
}

void CParseTreeView::OnUpdateParsetreeMakeitemroot(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_itemSelected && m_itemCurrentRoot != m_itemSelected);
}

void CParseTreeView::OnUpdateParsetreeHighlighttext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_itemSelected != NULL);
}

void CParseTreeView::OnUpdateParsetreeFind(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_itemSelected != NULL);
}

BOOL CParseTreeView::OnToolTipNeedText(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
{
	return true;

#if 0
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;

	TVHITTESTINFO hit;
	ZeroMemory(&hit, sizeof(TVHITTESTINFO));
	GetCursorPos(&hit.pt);
	GetTreeCtrl().ScreenToClient(&hit.pt);
	HTREEITEM hitItem = GetTreeCtrl().HitTest(&hit);
	if (hitItem && (hit.flags & (TVHT_ONITEM | TVHT_ONITEMRIGHT)) ) {
		// Determine if you need to display the tooltip based upon
		// whether the entire line is visible or not.
		CRect cRItem, cRClient, cRUnion;
		GetTreeCtrl().GetItemRect(hitItem, &cRItem, true);
		GetClientRect(&cRClient);
		if (cRUnion.UnionRect(cRClient, cRItem)) {
			if (cRUnion == cRClient)
				return true;
		}

		CString strTipText = _T("Error!");
		CParseTreeItemInfo *pInfo = GetItemDataFromItem(hitItem);
		if (pInfo) {
			strTipText  = pInfo->GetText();
			if (!pInfo->GetSem().IsEmpty()) {
				strTipText += _T("\n");
				if (pInfo->GetText().GetAt(pInfo->GetText().GetLength() - 1) != '\n')
					strTipText += _T("\n");
				strTipText += pInfo->GetSem();
			}
		}
		int nSize = strTipText.GetLength() + 1;
		ClientToScreen(&cRItem);
		cRItem.OffsetRect(CPoint(-1,-1));
#ifndef _UNICODE
		if (pNMHDR->code == TTN_NEEDTEXTA) {
			if (m_lpToolTipText)
				free(m_lpToolTipText);
			m_lpToolTipText = (LPTSTR)calloc(1,nSize);
			lstrcpyn(m_lpToolTipText, strTipText, nSize);
			pTTTA->lpszText = m_lpToolTipText;
		}
		else {
			if (m_lpWToolTipText)
				free(m_lpWToolTipText);
			m_lpWToolTipText = (LPWSTR)calloc(2,nSize);
			_mbstowcsz((LPWSTR)m_lpWToolTipText, strTipText, nSize);
			pTTTW->lpszText = (LPWSTR)m_lpWToolTipText;
		}
#else
		if (pNMHDR->code == TTN_NEEDTEXTA) {
			if (m_lpToolTipText)
				free(m_lpToolTipText);
			m_lpToolTipText = (LPTSTR)calloc(2,nSize);
			_wcstombsz(m_lpToolTipText, strTipText, nSize);
			pTTTA->lpszText = (LPTSTR)m_lpToolTipText;
		}
		else {
			if (m_lpWToolTipText)
				free(m_lpWToolTipText);
			m_lpWToolTipText = (LPWSTR)calloc(2,nSize);
			lstrcpyn(m_lpWToolTipText, strTipText, nSize);
			pTTTW->lpszText = (LPWSTR)m_lpWToolTipText;
		}
#endif
		*pResult = 0;

		// bring the tooltip window above other popup windows
		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, cRItem.left, cRItem.top, 0, 0,
			SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOOWNERZORDER);
		return true;
	}
	return false;
#endif
}

void CParseTreeView::OnTimer(UINT nIDEvent) 
{
	CTreeView::OnTimer(nIDEvent);
	switch (nIDEvent) {
		case ID_TIMER_SEMPOPUP:
			if (m_SemPopupTimerID && appBoolNodeSemantics) {
				KillTimer(m_SemPopupTimerID);
				m_SemPopupTimerID = 0;

				TVHITTESTINFO hit;
				ZeroMemory(&hit, sizeof(TVHITTESTINFO));
				GetCursorPos(&hit.pt);
				GetTreeCtrl().ScreenToClient(&hit.pt);
				HTREEITEM hitItem = GetTreeCtrl().HitTest(&hit);
				if (hit.flags & (TVHT_ONITEM | TVHT_ONITEMRIGHT | TVHT_ONITEMBUTTON)) {
					if (hitItem == m_SemPopupItem) {
						CParseTreeItemInfo *pInfo = GetItemDataFromItem(m_SemPopupItem);
						m_pSemPopup = new CSemPopup(pInfo, this);
						CSemPopup* pSemPopup = (CSemPopup*)m_pSemPopup;
						CRect cR = pSemPopup->m_cRCreate;
						m_pSemPopup->CreateEx(WS_EX_TOOLWINDOW, 
							NULL, // LPCTSTR lpszClassName,
							_T(""), //LPCTSTR lpszWindowName,
							WS_CHILD, // DWORD dwStyle,
							cR, //const RECT& rect,
							GetDesktopWindow(), //CWnd* pParentWnd,
							ID_SEMPOPUP //UINT nID,
							);
						m_pSemPopup->BringWindowToTop();
						m_pSemPopup->ShowWindow(SW_SHOWNORMAL);
						break;
					}
				}
			}
			if (m_SemPopupItem)
				OnDestroySemPopup(0,0);
			break;
		default:
			break;
	}
}

LRESULT CParseTreeView::OnDestroySemPopup(WPARAM wParam, LPARAM lParam)
{
	m_SemPopupItem = NULL;
	if (m_SemPopupTimerID) {
		KillTimer (m_SemPopupTimerID);
		m_SemPopupTimerID = 0;
	}
	if (m_pSemPopup) {
		CRect cR;
		CSemPopup* pSemPopup = (CSemPopup*)m_pSemPopup;
		if (1 == lParam) {
			pSemPopup->GetWindowRect(&cR);
			pSemPopup->ClientToScreen(&cR);
		}
		pSemPopup->DestroyWindow();
		if (1 == lParam) {
			AfxGetMainWnd()->ScreenToClient(&cR);
			AfxGetMainWnd()->RedrawWindow(cR, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
		}
		delete pSemPopup;
		m_pSemPopup = 0;
	}
	return 0L;
}

void CParseTreeView::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	if (HTCLIENT != nHitTest && m_SemPopupItem)
		OnDestroySemPopup(0,0);
}

void CParseTreeView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CTreeView::OnMouseMove(nFlags, point);

	if (GetFocus() != this)
		return;

	TVHITTESTINFO	hit;
	ZeroMemory(&hit, sizeof(TVHITTESTINFO));
	hit.pt = point;

	ClientToScreen(&hit.pt);
	GetTreeCtrl().ScreenToClient(&hit.pt);
	HTREEITEM hitItem = GetTreeCtrl().HitTest(&hit);
	if (hit.flags & (TVHT_ONITEM | TVHT_ONITEMRIGHT | TVHT_ONITEMBUTTON)) {
		if (hitItem == m_SemPopupItem)
			return;
		if (0 == m_SemPopupTimerID || m_SemPopupItem != NULL) {
			if (m_SemPopupItem)
				OnDestroySemPopup(0,0);
			if (0 == m_SemPopupTimerID) {
				m_SemPopupItem = hitItem;
				m_SemPopupTimerID = SetTimer(ID_TIMER_SEMPOPUP, 100, NULL);
			}
		}
	}
	else if (m_SemPopupItem) {
		OnDestroySemPopup(0,0);
	}
}

void CParseTreeView::OnKillFocus(CWnd* pNewWnd) 
{
	CTreeView::OnKillFocus(pNewWnd);
	if (m_SemPopupItem)
		OnDestroySemPopup(0,0);
}

void CParseTreeView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	ViewRuleFired();

	*pResult = 0;
}

void CParseTreeView::ViewRuleFired() 
{
	m_itemSelected = GetTreeCtrl().GetSelectedItem();

	if (m_itemSelected) {
		CParseTreeItemInfo *pInfo = GetItemDataFromItem(m_itemSelected);
		if (!pInfo->GetPass())
			return;

		m_strConcept = pInfo->GetConcept();
		CString dirPathStr = theApp.m_pMainFrame->m_wndAnalyzerView.PassFileFromNumber(pInfo->GetPass());

		if (FileExists(dirPathStr)) {
			//if (!(doc = theApp.IsVisualTextDocumentOpen("VisualText",dirPathStr)))
			// IF PASS FILE, MOVE TO ANALYZER TAB AND SELECT PASS
			if (EndsWith(dirPathStr,_T("pat")))
				theApp.m_pMainFrame->m_wndAnalyzerView.EditRulesFile(dirPathStr); // SRP 010606, for Pass File Number in Doc Title Bar

			CSyntaxEditorDoc* doc = (CSyntaxEditorDoc *)theApp.OpenNewDocument(_T("SyntaxEditor"),dirPathStr);
			CSyntaxEditorView *view = (CSyntaxEditorView *)GetFirstView(doc);
			CXTPSyntaxEditCtrl &ed = view->GetEditCtrl();

			// Calling GetLineCount() must somehow force edit control to take stock of number of lines.
			// On certain machines (e.g. 450 MHz), LineIndex(intval) wrongly returned -1

			int row = pInfo->GetLine();
			ed.EnsureVisibleRow(row);
			ed.Select(row,1,row,1);

			//CVisualTextView *v = (CVisualTextView *)view;
			//v->Scroll(row);
		}
		m_strConcept = m_strRootConcept;		// FIX.	// 01/09/02 AM.
	}
}

void CParseTreeView::OnParsetree() 
{
	ViewRuleFired();	
}

void CParseTreeView::OnUpdateParsetree(CCmdUI* pCmdUI) 
{
	bool enable = false;
	m_itemSelected = GetTreeCtrl().GetSelectedItem();
	if (m_itemSelected) {
		CParseTreeItemInfo *pInfo = GetItemDataFromItem(m_itemSelected);
		if (pInfo->GetPass())
			enable = true;
	};

	pCmdUI->Enable(enable);
}

// Supposedly faster than DeleteAllItems	// OPT	// 12/02/06 AM.
#ifdef _REF_

SetRedraw(FALSE);
//find last first-level item
HTREEITEM i1 = GetRootItem();
HTREEITEM i2 = i1;
while(i2 != NULL)
{
  i1 = i2;
  i2 = GetNextItem(i1, TVGN_NEXT);
}
//remove from end
while( i1 != NULL)
{
  i2 = GetNextItem(i1, TVGN_PREVIOUS);
  DeleteItem(i1);
  i1 = i2;
}
SetRedraw(TRUE);
#endif

