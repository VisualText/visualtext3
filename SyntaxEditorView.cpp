// MDITextEditorView.cpp : implementation of the CSyntaxEditorView class
//

#include "stdafx.h"
#include "Resource.h"

#include "Utils.h"
#include "kb.h"
#include "VisualText.h"
#include "MainFrm.h"
#include "VisualTextView.h"
#include "SyntaxEditorDoc.h"
#include "SyntaxEditorView.h"

extern CVisualTextApp theApp;
extern CString appStrFind;
extern CString appAskText;
extern bool appBoolDocDirty;
extern bool appBoolDocsDirty;
extern bool appBoolAnalyzerLoaded;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef enum SyntaxEditUpdateViewHint
{
	xtpHintWhiteSpace = (xtpEditHintLast + 1),
};

/////////////////////////////////////////////////////////////////////////////
// CSyntaxEditorView

IMPLEMENT_DYNCREATE(CSyntaxEditorView, CViewBase)

BEGIN_MESSAGE_MAP(CSyntaxEditorView, CViewBase)
	//{{AFX_MSG_MAP(CSyntaxEditorView)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_FIND_SILENT, OnEditFindSilent)
	ON_COMMAND(ID_EDIT_TABIFY, OnEditTabify)
	ON_COMMAND(ID_EDIT_UNTABIFY, OnEditUnTabify)
	ON_COMMAND(ID_EDIT_UPPERCASE, OnEditUppercase)
	ON_COMMAND(ID_EDIT_LOWERCASE, OnEditLowercase)
	ON_COMMAND(ID_EDIT_WHITESPACE, OnEditWhiteSpace)
	ON_UPDATE_COMMAND_UI(ID_EDIT_WHITESPACE, OnUpdateEditWhiteSpace)
	ON_COMMAND(ID_EDIT_COLLAPSEALL, OnEditCollapseAll)
	ON_COMMAND(ID_EDIT_EXPANDALL, OnEditExpandAll)
	ON_COMMAND(ID_TOOLS_BOOKMARK, OnToolsBookmark)
	ON_COMMAND(ID_TOOLS_BREAKPOINT, OnToolsBreakpoint)
	ON_COMMAND(ID_TOOLS_NEXT_BOOKMARK, OnToolsNextBookmark)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_NEXT_BOOKMARK, OnUpdateToolsNextBookmark)
	ON_COMMAND(ID_TOOLS_PREV_BOOKMARK, OnToolsPrevBookmark)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_PREV_BOOKMARK, OnUpdateToolsPrevBookmark)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_OPTIONS, OnUpdateToolsOptions)
	ON_COMMAND(ID_FILE_GOTO, OnFileGoTo)
	ON_COMMAND(ID_FILE_PAGE_SETUP, CViewBase::OnFilePageSetup)
	ON_COMMAND(ID_EDIT_READONLY, OnReadOnly)
	ON_UPDATE_COMMAND_UI(ID_EDIT_READONLY, OnUpdateReadOnly)
	ON_COMMAND(ID_TOOLS_RELOADSYNTAX, OnReloadSyntax)
	ON_COMMAND(ID_TOOLS_RELOADSYNTAX2INIMODE, OnReloadSyntax2IniMode)
	ON_COMMAND(ID_TOOLS_RELOADSYNTAXMODE, OnReloadSyntaxMode)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CViewBase::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CViewBase::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CViewBase::OnFilePrintPreview)
	ON_COMMAND(ID_RULEFILE_INSERT_RULES, OnRulefileInsertRules)
	ON_COMMAND(ID_RULEFILE_INSERT_POST, OnRulefileInsertPost)
	ON_COMMAND(ID_RULEFILE_INSERT_XWHITE, OnRulefileInsertXwhite)
	ON_COMMAND(ID_RULEFILE_INSERT_POSTRULE, OnRulefileInsertPostrule)
	ON_COMMAND(ID_RULEFILE_CONVERT_STRINGTOGENERALRULE, OnRulefileConvertStringtogeneralrule)
	ON_COMMAND(ID_RULEFILE_ADD_MATCHES, OnRulefileAddMatches)
	ON_COMMAND(ID_RULEFILE_ADD_EXCEPT, OnRulefileAddExcept)
	ON_COMMAND(ID_RULEFILE_ADD_FAILS, OnRulefileAddFails)
	ON_COMMAND(ID_RULEFILE_ADD_OPTIONAL, OnRulefileAddOptional)
	ON_COMMAND(ID_RULEFILE_ADD_STAR, OnRulefileAddStar)
	ON_COMMAND(ID_RULEFILE_ADDREMOVE_S, OnRulefileAddremoveS)
	ON_COMMAND(ID_RULEFILE_CONVERT_ONELINETOMULTIPLE, OnRulefileConvertOnelinetomultiple)
	ON_COMMAND(ID_RULEFILE_ADDREMOVE_MAX, OnRulefileAddremoveMax)
	ON_COMMAND(ID_RULEFILE_ADDREMOVE_MIN, OnRulefileAddremoveMin)
	ON_COMMAND(ID_RULEFILE_CHANGE_XWILD, OnRulefileChangeXwild)
	ON_COMMAND(ID_RULEFILE_CHANGE_XNUM, OnRulefileChangeXnum)
	ON_COMMAND(ID_RULEFILE_CHANGE_XALPHA, OnRulefileChangeXalpha)
	ON_COMMAND(ID_RULEFILE_ADDREMOVE_TRIG, OnRulefileAddremoveTrig)
	ON_COMMAND(ID_RULEFILE_INSERT_XPUNCT, OnRulefileInsertXpunct)
	ON_COMMAND(ID_RULEFILE_ADDREMOVE_TREE, OnRulefileAddremoveTree)
	ON_COMMAND(ID_RULEFILE_ADDREMOVE_LOOKAHEAD, OnRulefileAddremoveLookahead)
	ON_COMMAND(ID_RULEFILE_INSERT_XALPHA, OnRulefileInsertXalpha)
	ON_COMMAND(ID_RULEFILE_ADDREMOVE_LAYER, OnRulefileAddremoveLayer)
	ON_COMMAND(ID_RULEFILE_ADDREMOVE_ONE, OnRulefileAddremoveOne)
	ON_COMMAND(ID_RULEFILE_CHANGETO_XWHITE, OnRulefileChangetoXwhite)
	ON_COMMAND(ID_RULEFILE_INSERTLINE_XALPHA, OnRulefileInsertlineXalpha)
	ON_COMMAND(ID_RULEFILE_INSERTLINE_XNUM, OnRulefileInsertlineXnum)
	ON_COMMAND(ID_RULEFILE_INSERTLINE_XWHITE, OnRulefileInsertlineXwhite)
	ON_COMMAND(ID_RULEFILE_INSERTLINE_XWILD, OnRulefileInsertlineXwild)
	ON_COMMAND(ID_RULEFILE_ADDREMOVE_PLUS, OnRulefileAddremovePlus)
	ON_COMMAND(ID_RULEFILE_DELETELINE, OnRulefileDeleteline)
	ON_COMMAND(ID_RULEFILE_SAVERUNLOG, OnRulefileSaverunlog)
	ON_COMMAND(ID_RULEFILE_RUN, OnRulefileRun)
	ON_COMMAND(ID_RULEFILE_VIEWLOG, OnRulefileViewlog)
	ON_COMMAND(ID_RULEFILE_FINDINLOG, OnRulefileFindinlog)
	ON_COMMAND(ID_RULEFILE_DUPLICATELINE, OnRulefileDuplicateline)
	ON_COMMAND(ID_RULEFILE_HEADER, OnRulefileHeader)
	ON_COMMAND(ID_RULEFILE_FIND, OnRulefileFind)
	ON_COMMAND(ID_RULEFILE_RULEHELP, OnRulefileRulehelp)
	ON_COMMAND(ID_RULEFILE_CLOSEALL, OnRulefileCloseall)
	ON_COMMAND(ID_RULEFILE_CLOSEREST, OnRulefileCloserest)
	ON_COMMAND(ID_RULEFILE_NLPHELP, OnRulefileNlphelp)
	ON_COMMAND(ID_RULEFILE_SELECTPASS, OnRulefileSelectpass)
	ON_COMMAND(ID_RULEFILE_STRINGTORULE, OnRulefileStringtorule)
	//ON_COMMAND(ID_TEXT_FINDLOCAL, OnRulefileFindlocal)
	//ON_COMMAND(ID_TEXT_FINDAGAIN, OnRulefileFindagain)
	ON_COMMAND(ID_RULEFILE_INSERTLINE_XSTART, OnRulefileInsertlineXstart)
	ON_COMMAND(ID_RULEFILE_COMMENTUNCOMMENT, OnRulefileCommentuncomment)
	ON_COMMAND(ID_RULEFILE_INSERT_DUMPFILE, OnRulefileInsertDumpfile)
	//ON_COMMAND(ID_RULEFILE_FINDLOCAL, OnRulefileFindlocal)
	//ON_COMMAND(ID_RULEFILE_FINDAGAIN, OnRulefileFindagain)
	ON_COMMAND(ID_TOOLS_RULEEDITOPTIONS, OnToolsRuleeditoptions)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_RULEEDITOPTIONS, &CSyntaxEditorView::OnUpdateToolsRuleeditoptions)
	ON_COMMAND(ID_RULEFILE_REFORMATRULE, &CSyntaxEditorView::OnRulefileReformatrule)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_SAVE_ALL, OnUpdateSaveAll)
	ON_COMMAND(ID_RULEFILE_HELPONSELECTION, OnRulefileHelponselection)
	ON_COMMAND(ID_VIEW_OPTIONS, &CSyntaxEditorView::OnViewOptions)

//	ON_WM_SETFOCUS()
ON_WM_SETFOCUS()
ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CSyntaxEditorView::OnUpdateFileNew)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyntaxEditorView construction/destruction

CSyntaxEditorView::CSyntaxEditorView() : m_dwLastUpdate(0), m_dwParserStart(0)
{
	m_rcMargin.left   = 25;
	m_rcMargin.right  = 25;
	m_rcMargin.top    = 50;
	m_rcMargin.bottom = 50;

	CString sPath = CXTPSyntaxEditCtrl::GetModulePath();

	BOOL bFileMode = TRUE;
	//bFileMode = !bFileMode;

	// Add _tsetlocale call to allow some text functions (like isleadbyte) works correctly
	// A good place for this is CMainFrame implementation (see CMainFrame constructor).
	// EXAMPLE: 
	// _tsetlocale(LC_ALL, _T(""));

	if (bFileMode)
		GetEditCtrl().SetConfigFile(CXTPSyntaxEditCtrl::GetModulePath() + _T("Config\\SyntaxEdit.ini"));
	//Test for remote config
	//GetEditCtrl().SetConfigFile(_T("\\\\Midc\\software\\TempFolderForTesting\\SyntaxEdit.ini"));

	if (!bFileMode)
	{
		#include "_cpp1_schclass.h"
		#include "ColorsDataEntry.h"

		GetEditCtrl().SetSyntaxAndColorScheme(sScheme, sColorTheme);
	}

	appIntRuleTabSize = GetEditCtrl().GetTabSize();
}

void CSyntaxEditorView::OnToolsFind() 
{
	theApp.m_pMainFrame->FindDlg();	
}

void CSyntaxEditorView::OnUpdateToolsFind(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);
}

void CSyntaxEditorView::OnReloadSyntaxMode()
{
	#include "_cpp2_schclass.h"
	#include "ColorsDataEntry1.h"

	if (GetEditCtrl().GetEditBuffer())
		GetEditCtrl().SetSyntaxAndColorScheme(sScheme, sColorTheme);
}

void CSyntaxEditorView::OnReloadSyntax2IniMode()
{
	GetEditCtrl().ReloadSyntaxConfigFileMode();
	CString sFile = CXTPSyntaxEditCtrl::GetModulePath() + _T("EditConfig\\SyntaxEdit.ini");

	CFileDialog dlg(TRUE, _T("ini"), _T("*.ini"));
	if (dlg.DoModal() == IDOK)
		sFile = dlg.GetPathName();
	GetEditCtrl().SetConfigFile(sFile);
}

void CSyntaxEditorView::OnReloadSyntax()
{
	#include "_cpp2_schclass.h"
	#include "ColorsDataEntry1.h"

	if (GetEditCtrl().GetEditBuffer())
		GetEditCtrl().SetSyntaxAndColorScheme(sScheme, sColorTheme, TRUE, TRUE);
}

CSyntaxEditorView::~CSyntaxEditorView()
{
}

BOOL CSyntaxEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CViewBase::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSyntaxEditorView printing

BOOL CSyntaxEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return CViewBase::OnPreparePrinting(pInfo);
}

void CSyntaxEditorView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CViewBase::OnBeginPrinting(pDC, pInfo);
}

void CSyntaxEditorView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CViewBase::OnEndPrinting(pDC, pInfo);
}

void CSyntaxEditorView::OnPrintHeader(CDC* pDC, CPrintInfo* pInfo)
{
	CXTPFontDC fontDC(pDC, GetEditCtrl().GetPaintManager()->GetFont(),
		::GetSysColor(COLOR_WINDOWTEXT));

	CPoint ptFrom;
	ptFrom.x = m_rcMargin.left;
	ptFrom.y = m_rcMargin.top - 5;
	pDC->MoveTo(ptFrom);

	CPoint ptTo;
	ptTo.x = pInfo->m_rectDraw.right - m_rcMargin.right;
	ptTo.y = m_rcMargin.top - 5;
	pDC->LineTo(ptTo);

	CString csPathName = GetDocument()->GetPathName();
	CString csFileName = csPathName.Right(csPathName.GetLength()-(csPathName.ReverseFind('\\')+1));

	if (csFileName.IsEmpty())
    {
		csFileName = GetDocument()->GetTitle();
		csFileName.Remove('*');
    }

	CSize sizeText = pDC->GetTextExtent(csFileName);
	int x = m_rcMargin.left;
	int y = m_rcMargin.top - 10 - sizeText.cy;
	pDC->TextOut(x, y, csFileName);

	CTime tmCurrent = CTime::GetCurrentTime();
	CString csTime = tmCurrent.Format(_T("%m-%d-%Y %I:%M%p"));
	sizeText = pDC->GetTextExtent(csTime);

	pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
	pDC->TextOut((ptTo.x - sizeText.cx), y, csTime);
}

void CSyntaxEditorView::OnPrintFooter(CDC* pDC, CPrintInfo* pInfo)
{
	CXTPFontDC fontDC(pDC, GetEditCtrl().GetPaintManager()->GetFont(),
		::GetSysColor(COLOR_WINDOWTEXT));

	int iBottomPos = (pInfo->m_rectDraw.Height() - m_rcMargin.bottom + 5);

	CPoint ptFrom;
	ptFrom.x = m_rcMargin.left;
	ptFrom.y = iBottomPos;
	pDC->MoveTo(ptFrom);

	CPoint ptTo;
	ptTo.x = pInfo->m_rectDraw.right - m_rcMargin.right;
	ptTo.y = iBottomPos;
	pDC->LineTo(ptTo);

	CString csPageNo;
	csPageNo.Format(_T("Page %d of %d"), pInfo->m_nCurPage, pInfo->GetMaxPage());

	pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
	pDC->TextOut(m_rcMargin.left, iBottomPos + 5, csPageNo);
}

void CSyntaxEditorView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	// Print header in application-customized mode
//	OnPrintHeader(pDC, pInfo);

//	CRect rcDraw_orig = pInfo->m_rectDraw;

//	pInfo->m_rectDraw.top += m_rcMargin.top;
//	pInfo->m_rectDraw.bottom -= m_rcMargin.bottom;

	// Print body
	CViewBase::OnPrint(pDC, pInfo);

//	pInfo->m_rectDraw = rcDraw_orig;
	// Print footer
//	OnPrintFooter(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CSyntaxEditorView diagnostics

#ifdef _DEBUG
void CSyntaxEditorView::AssertValid() const
{
	CViewBase::AssertValid();
}

void CSyntaxEditorView::Dump(CDumpContext& dc) const
{
	CViewBase::Dump(dc);
}

CSyntaxEditorDoc* CSyntaxEditorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSyntaxEditorDoc)));
	return (CSyntaxEditorDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSyntaxEditorView message handlers

BOOL CSyntaxEditorView::OnRowColChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (!CViewBase::OnRowColChanged(pNMHDR, pResult))
		return FALSE;

	XTP_EDIT_NMHDR_ROWCOLCHANGED* pNMHDR_RCC = (XTP_EDIT_NMHDR_ROWCOLCHANGED*)pNMHDR;
	if (!pNMHDR_RCC)
		return FALSE;

	theApp.m_pMainFrame->SetRowCol(pNMHDR_RCC->nRow, pNMHDR_RCC->nCol);

	*pResult = 0;
	return TRUE;
}

BOOL CSyntaxEditorView::OnParseEvent(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (!CViewBase::OnParseEvent(pNMHDR, pResult))
		return FALSE;

	XTP_EDIT_NMHDR_PARSEEVENT* pNMParseEvent = (XTP_EDIT_NMHDR_PARSEEVENT*)pNMHDR;
	if (!pNMParseEvent)
		return FALSE;

	CDocument* pDoc = GetDocument();
	if (!pDoc)
		return FALSE;

	const int cnUpdateTimeOut = 500;
	CString strMsg;
	BOOL bUpdate = TRUE;

	if (pNMParseEvent->code == xtpEditOnTextBlockParsed) 
	{
		DWORD dwTime = ::GetTickCount();
		bUpdate = labs(dwTime - m_dwLastUpdate) >= cnUpdateTimeOut;

		CXTPSyntaxEditLexTextBlock* pTBended = (CXTPSyntaxEditLexTextBlock*)pNMParseEvent->wParam;
		if (pTBended && bUpdate)
		{
			m_dwLastUpdate = dwTime;

			DWORD dwTime1 = GetTickCount();//DEBUG
			double dTime = labs(dwTime1-m_dwParserStart)/1000.0;

			strMsg.Format(_T(" Parsing time(%.1f sec). Line(%d)."), dTime, pTBended->m_PosEndLC.nLine);

//			strMsg.Format(_T(" Parsing time(%.3f sec). Last parsed block: (%d,%d - %d,%d)::%s"), 
//				dTime,
//				pTBended->m_PosStartLC.nLine, pTBended->m_PosStartLC.nCol, 
//				pTBended->m_PosEndLC.nLine, pTBended->m_PosEndLC.nCol, 
//				pTBended->m_ptrLexClass ? pTBended->m_ptrLexClass->GetClassName() 
//				: _T("?<NULL>") );
		}
	}
	
	else if (pNMParseEvent->code == xtpEditOnParserStarted) 
	{		
		strMsg = _T("*** Parser Started");

		m_dwParserStart = GetTickCount();
	}
	
	else if (pNMParseEvent->code == xtpEditOnParserEnded) 
	{	
		if(!pNMParseEvent->wParam || (pNMParseEvent->wParam & xtpEditLPR_RunFinished)) 
		{
			DWORD dwTime1 = GetTickCount();
			double dTime = labs(dwTime1-m_dwParserStart)/1000.0;
			strMsg.Format(_T(" Parsing time(%.3f sec)."), dTime);
			strMsg = "";
		}
	}

	if (bUpdate)	
	{		
		CString strTitle = pDoc->GetTitle();
		int nFIdx = strTitle.Find(_T(" :.: "));
		if(nFIdx >= 0) {
			strTitle = strTitle.Left(nFIdx);

			nFIdx = strTitle.Find(_T("*"));
			if(nFIdx >= 0) {
				strTitle = strTitle.Left(nFIdx);
			}
			strTitle.TrimRight();
		}
		
		if (pDoc->IsModified()) {
			strTitle += _T(" * ");
		}

		if(strMsg.GetLength() > 0) {
			strTitle += _T(" :.: ");
			strTitle += strMsg;		
		}		
        				
		pDoc->SetTitle(strTitle);
		if (strTitle == "") {
			int moose = 0;
		}
	}

	*pResult = 0;
	return TRUE;
}

void CSyntaxEditorView::OnContextMenu(CWnd* /*pWnd*/, CPoint point) 
{
	if (point.x == -1 && point.y == -1)
	{
		// keystroke invocation
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);
		
		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	/*
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_RULEFILE));
	
	CMenu* pPopup = menu.GetSubMenu(1);
	ASSERT(pPopup != NULL);

	CWnd* pWndPopupOwner = this;
	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	((CXTPMDIFrameWnd*)AfxGetMainWnd())->GetCommandBars()->
		TrackPopupMenuEx(pPopup, TPM_RIGHTBUTTON, point.x, point.y);
	*/

	// TAKE CARE OF GRAYING ITEMS
	UINT disables[100];
	for (int i=0; i<100; i++)
		disables[i] = 0;
	int count = 0;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	SetCursorLineType();

	if (appStrFind == _T(""))
		disables[count++] = ID_RULEFILE_FINDAGAIN;
	if (!GetDocument()->IsModified())
		disables[count++] = ID_RULEFILE_SAVE;
	if (wnd->SelectedTextFolderEmpty())
		disables[count++] = ID_RULEFILE_RUN;

	CSyntaxEditorDoc *doc = GetDocument();
	if (!doc->IsCurrentPassFile()) {
		disables[count++] = ID_RULEFILE_SELECTPASS;
	}

	// CalculateIsAnalyzed();  11/17/2013 David took out
	if (!m_boolIsAnalyzed) {
		disables[count++] = ID_RULEFILE_HIGHLIGHTMATCHES;
		disables[count++] = ID_RULEFILE_VIEWLOG;
		disables[count++] = ID_RULEFILE_FINDINLOG;
	}

	if (m_cursorLineType != CURSOR_LINE_RULE_UNFORMATTED) {
		disables[count++] = ID_RULEFILE_CONVERT_ONELINETOMULTIPLE;

	
		//if (m_cursorLineType != CURSOR_LINE_NORMAL) {
		//	disables[count++] = ID_RULEFILE_STRINGTORULE;
		//	disables[count++] = ID_RULEFILE_CONVERT_STRINGTOGENERALRULE;
		//}
	}
	
	if (m_cursorLineType != CURSOR_LINE_BLANK &&
		m_cursorLineType != CURSOR_LINE_NORMAL) {

		// ALLOW WHEN ON THE LAST LINE OF A FORMATTED RULE TO
		// INSERT LINES
		if (m_cursorLineType != CURSOR_LINE_RULE_UNFORMATTED) {
			disables[count++] = ID_RULEFILE_INSERT_XALPHA;
			disables[count++] = ID_RULEFILE_INSERT_XNUM;
			disables[count++] = ID_RULEFILE_INSERT_XPUNCT;
			disables[count++] = ID_RULEFILE_INSERT_XWHITE;
		}

		disables[count++] = ID_RULEFILE_INSERT_DUMPFILE;
		disables[count++] = ID_RULEFILE_HEADER;
		disables[count++] = ID_RULEFILE_INSERT_POSTRULE;
		disables[count++] = ID_RULEFILE_INSERT_RULES;
		disables[count++] = ID_RULEFILE_INSERT_POST;
		disables[count++] = ID_RULEFILE_STRINGTORULE;
		disables[count++] = ID_RULEFILE_CONVERT_STRINGTOGENERALRULE;
	}

	if (m_cursorLineType != CURSOR_LINE_RULE_FORMATTED) {

		if (m_cursorLineType != CURSOR_LINE_RULE_FORMATTED_FIRST &&
			m_cursorLineType != CURSOR_LINE_RULE_FORMATTED_LAST)
			disables[count++] = ID_RULEFILE_REFORMATRULE;

		disables[count++] = ID_RULEFILE_ADD_EXCEPT;
		disables[count++] = ID_RULEFILE_ADD_FAILS;
		disables[count++] = ID_RULEFILE_ADDREMOVE_LAYER;
		disables[count++] = ID_RULEFILE_ADDREMOVE_LOOKAHEAD;
		disables[count++] = ID_RULEFILE_ADD_MATCHES;
		disables[count++] = ID_RULEFILE_ADDREMOVE_MAX;
		disables[count++] = ID_RULEFILE_ADDREMOVE_MIN;
		disables[count++] = ID_RULEFILE_ADDREMOVE_ONE;
		disables[count++] = ID_RULEFILE_ADDREMOVE_PLUS;
		disables[count++] = ID_RULEFILE_ADD_OPTIONAL;
		disables[count++] = ID_RULEFILE_ADD_STAR;
		disables[count++] = ID_RULEFILE_ADDREMOVE_S;
		disables[count++] = ID_RULEFILE_ADDREMOVE_TREE;
		disables[count++] = ID_RULEFILE_ADDREMOVE_TRIG;

		// ALLOW WHEN ON THE FIRST LINE OF A FORMATTED RULE TO
		// INSERT LINES
		if (m_cursorLineType != CURSOR_LINE_RULE_FORMATTED_FIRST) {
			disables[count++] = ID_RULEFILE_INSERTLINE_XALPHA;
			disables[count++] = ID_RULEFILE_INSERTLINE_XNUM;
			disables[count++] = ID_RULEFILE_INSERTLINE_XSTART;
			disables[count++] = ID_RULEFILE_INSERTLINE_XWHITE;
			disables[count++] = ID_RULEFILE_INSERTLINE_XWILD;
		}

		disables[count++] = ID_RULEFILE_CHANGE_XALPHA;
		disables[count++] = ID_RULEFILE_CHANGE_XNUM;
		disables[count++] = ID_RULEFILE_CHANGETO_XWHITE;
		disables[count++] = ID_RULEFILE_CHANGE_XWILD;
	}

	// DO MENU	
	PopupContextMenu(this,IDR_RULEFILE,point,disables);	
}

void CSyntaxEditorView::SetCursorLineType()
{
	m_cursorLineType = GetCursorLineType();
}
/*
bool CSyntaxEditorView::CalculateIsAnalyzed()
{
	int pass = LogFilePassNumberFromTitle();
	if (pass > 0) {
		CString logPathFileStr;
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		wnd->GetLogPath(pass,logPathFileStr,true);
		CString dirPathStr = StripPath(logPathFileStr,0,true);
		m_boolIsAnalyzed = IsDirectory(dirPathStr) ? true : false;
	}
	else
		m_boolIsAnalyzed = false;
	return m_boolIsAnalyzed;
}
*/

void CSyntaxEditorView::InsertLine(CString str, int cursorPos) 
{
	if (!SelectionInRule())
		return;

	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	CXTPSyntaxEditSelection sel = edit.GetSelection();
	XTP_EDIT_LINECOL start = sel.GetStart_disp();

	edit.InsertRow(start.nLine, str + _T("\n"), true);
	ReformatRule(false);
}

void CSyntaxEditorView::OnRulefileInsertPostrule() 
{
//	OnRulefileInsertPost();
//	OnRulefileInsertRules();
	// Generate one Undo/Redo
	InsertString(_T("@POST\n    single();\n@RULES\n\n_xNIL <-\n    _xALPHA [s]\t### (1)\n    @@\n"));
}

/*
bool CSyntaxEditorView::GetWorkingText2(CString &lineStr, long &lineIndex, long &colIndex, long &lineIndex2,
									    long &colIndex2, long &selStart, long &selEnd, CString alertMsgStr)
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	CXTPSyntaxEditSelection &sel = edit.GetSelection();
	edit.GetSelectionText(lineStr);
	colIndex = sel.GetStart_disp().nCol;
	colIndex2 = sel.GetENd_disp().nCol;
	//edit.GetSel(selStart,selEnd);

	int startLineNum = edit.LineFromChar(selStart);
	int endLineNum = edit.LineFromChar(selEnd);

	if (startLineNum != endLineNum && alertMsgStr != _T("")) {
		CString msgStr;
		msgStr.Format(_T("When %s, simply click on the line. Please choose one of the following:"),alertMsgStr);
		AskUser(msgStr,IDR_WAVE_RULEJOCKEY,2,1,_T("Unselect All"),_T("Cancel"),_T(""));
		if (appButtonReturn == 1) {
			edit.SetSel(selStart,selStart);
		}
		return false;
	}

	_TCHAR lineBuff[10001];
	lineIndex = edit.LineIndex(startLineNum);
	int lineLength = edit.GetLine(startLineNum,lineBuff,10000);
	lineBuff[lineLength] = '\0';
	lineStr = lineBuff;
	return true;
}
*/

void CSyntaxEditorView::OnRulefileConvertOnelinetomultiple() 
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	CXTPSyntaxEditSelection &sel = edit.GetSelection();
	CString lineStr = "";
	edit.GetSelectionText(lineStr);

	/*
	CString lineStr;
	long lineIndex,selStart,selEnd;
	CString msgStr = _T("converting one rule line to multiples");
	if (!GetWorkingText(lineStr,lineIndex,selStart,selEnd,msgStr))
		return;
		*/

	CStringList tokens;
	StringToWordList(tokens,lineStr);
	CString tokenStr;
	CString ruleStr = _T("");
	POSITION pos = tokens.GetHeadPosition();
	bool rewriteFound = false;
	bool inBracket = false;

	while (pos && (tokenStr = tokens.GetNext(pos))) {

		// TAKE CARE OF REWRITE ARROW FORMATTING
		if (!rewriteFound) {
			ruleStr += tokenStr;

			if (tokenStr == _T("<-"))
				rewriteFound = true;
			else
				ruleStr += _T(" ");
			continue;
		}

		if (tokenStr.GetAt(0) == '[')
			inBracket = true;

		// PUT CORRECT SPACER
		if (inBracket)
			ruleStr += _T(" ");
		else
			ruleStr += _T("\n    ");

		ruleStr += tokenStr;

		if (inBracket && tokenStr.GetAt(tokenStr.GetLength()-1) == ']')
			inBracket = false;
	}

	//edit.Select(lineIndex,0,lineIndex,lineIndex+lineStr.GetLength()-1);

	edit.ReplaceSel(ruleStr);

	m_strPost = ruleStr;
	m_cr2Post.cpMin -= ruleStr.GetLength();

	//edit.SetSel(selStart,selEnd);
	edit.Select(sel.GetStart_str().nLine, sel.GetStart_str().nCol, sel.GetEnd_str().nLine, sel.GetEnd_str().nCol);
}

void CSyntaxEditorView::AddAttribute(CString newElementStr, CString valueStr)
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	CXTPSyntaxEditSelection &sel = edit.GetSelection();

	int startLine = sel.GetStart_disp().nLine;
	int index = startLine;
	int cursor = index;
	CString lineStr = edit.GetLineText(startLine);

	/*
	int line = edit.LineFromChar(-1);
	int index = edit.LineIndex(line);
	_TCHAR lineBuff[1001];
	int lineLength = edit.GetLine(line,lineBuff,1000);
	lineBuff[lineLength] = '\0';
	lineStr = lineBuff;
	*/

	CStringList tokens;
	int bracketStatus = 0;	// 05/24/07 AM.
	TokenizeLine(tokens,lineStr,bracketStatus);

	CString newLineStr = _T("");
	CString tokenStr;

	POSITION pos = tokens.GetHeadPosition();
	bool hasBracket = false;
	bool inBracket = false;
	bool hasSuggested = false;
	bool elementExists = false;
	int elementCount = 0;
	CString elementEqualStr = newElementStr;
	elementEqualStr += _T("=");

	// DETERMINE INSERT POINT FOR ADDING
	while (pos && (tokenStr = tokens.GetNext(pos))) {
		if (tokenStr ==_T("<-"))
			hasSuggested = true;

		if (tokenStr == _T("]"))
			inBracket = false;

		if (inBracket)
			elementCount++;

		if (tokenStr == _T("[")) {
			hasBracket = true;
			inBracket = true;
		}

		if (tokenStr == newElementStr || tokenStr.Find(elementEqualStr,0) == 0)
			elementExists = true;
	}

	// IF NO SUGGESTED ARROW, ASSUME ONE SEGMENT OF RULE AND
	// ADD INDENT
	if (!hasSuggested)
		newLineStr += _T("    ");

	bool firstTime = true;
	bool alreadyAdded = false;
	bool elementFound = false;
	inBracket = false;
	pos = tokens.GetHeadPosition();
	CString lastTokenStr = _T("");
	int numTokens = tokens.GetCount();

	while (pos && (tokenStr = tokens.GetNext(pos))) {
		elementFound = tokenStr == newElementStr || tokenStr.Find(elementEqualStr,0) == 0;

		// FOR CASES WITHOUT BRACKETS SO NEED TO ADD THEM
		if (!hasBracket) {
			if ((!firstTime || numTokens == 1) && !alreadyAdded) {
				if (numTokens == 1)
					newLineStr += tokenStr;
				newLineStr += _T(" [");
				newLineStr += newElementStr;
				cursor += newLineStr.GetLength();
				if (valueStr != _T("")) {
					newLineStr += _T("=") + valueStr;
					cursor += 2;
				}
				newLineStr += _T("]");
				alreadyAdded = true;
			}
			if (!firstTime)
				newLineStr += _T(" ");
			if (numTokens > 1)
				newLineStr += tokenStr;
			firstTime = false;
			continue;
		}

		// IF ONE ELEMENT AND EXISTS, THEN SKIP UNTIL PAST BRACKETS
		if ((tokenStr == _T("[") || inBracket || tokenStr == _T("]")) &&
			elementExists && elementCount == 1) {

			inBracket = tokenStr == _T("]") ? false : true;
			cursor = index + newLineStr.GetLength();
			continue;
		}

		// IF ELEMENT EXISTS, THEN REMOVE BY SKIPPING ADDITION
		if (inBracket && elementFound)
			continue;

		if (!firstTime && lastTokenStr != _T("[") &&
			!(tokenStr == _T("]") && elementExists))
			newLineStr += _T(" ");

		if (tokenStr == _T("["))
			inBracket = true;

		if (!elementExists && inBracket && tokenStr == _T("]")) {
			newLineStr += newElementStr;
			cursor += newLineStr.GetLength();
			if (valueStr != _T("")) {
				newLineStr += _T("=") + valueStr;
				cursor += 2;
			}
			inBracket = false;
		}

		newLineStr += tokenStr;

		firstTime = false;

		lastTokenStr = tokenStr;
	}

	ReplaceSelection(newLineStr,true);
	ReformatRule(false);
}

void CSyntaxEditorView::ReplaceSelection(CString newLineStr, bool addSlashN) {
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	CXTPSyntaxEditSelection &sel = edit.GetSelection();

	int startLine = sel.GetStart_disp().nLine;
	int col1 = sel.GetStart_disp().nCol;
	int col2 = sel.GetEnd_disp().nCol;
	edit.Select(startLine, 1, startLine + 1, 1);

	if (addSlashN == true)
		edit.ReplaceSel(newLineStr + "\n");
	else
		edit.ReplaceSel(newLineStr);

	edit.Select(startLine, col1, startLine, col2);
}

CString CSyntaxEditorView::StandardizeLine(CString lineInStr,
	int &elementNum,	// Track rule elements.	// 05/24/07 AM.
	int &bracketStatus,	// Track multi-line brackets.	// 05/24/07 AM.
	int ruleLineNum)
{
	CStringList tokens;
	TokenizeLine(tokens,lineInStr,bracketStatus);

	POSITION pos = tokens.GetHeadPosition();
	bool hasBracket = false;
	bool hasSuggested = false;
	bool hasComment = false;
	bool hasCommentNumber = false;
	bool elementExists = false;
	bool justPassedComment = false;

	int afterBracketPos = 0;
	int commentPos = 0;
	int commentNumberPos = 0;
	int elementCount = 0;

	CString lastTokenStr = _T("");
	CString newLineStr = _T("");
	CString tokenStr;

	// RULE ELEMENT TYPES AND POSITIONS
	while (pos && (tokenStr = tokens.GetNext(pos))) {

		if (lastTokenStr != _T("\\") && tokenStr.GetAt(0) != '\\') {

			if (tokenStr ==_T("<-"))
				hasSuggested = true;

			// COMMENT STUFF HERE
			if (tokenStr.Find(_T("#"),0) == 0) {
				hasComment = true;
				justPassedComment = true;
				commentPos = elementCount;
			}

			if (justPassedComment &&
				tokenStr.GetAt(0) == '(' && tokenStr.GetAt(tokenStr.GetLength() - 1) == ')') {
				//(tokenStr.GetAt(2) == ')' || tokenStr.GetAt(3) == ')')) {

				hasCommentNumber = true;
				commentNumberPos = elementCount;
				justPassedComment = false;
			}

			if (tokenStr == _T("]"))
				afterBracketPos = elementCount + 1;
			else if (tokenStr == _T("["))
				hasBracket = true;
		}

		lastTokenStr = tokenStr;
		elementCount++;
	}

	// If end of multi-line bracket, pretend start bracket is here.	// 05/24/07 AM.
	if (bracketStatus == 3)	// 05/24/07 AM.
		hasBracket = true;	// 05/24/07 AM.
	else if (bracketStatus)	// 05/24/07 AM.
		hasBracket = false;	// Else, pretend no bracket yet.	// 05/24/07 AM.

	// IF NO SUGGESTED ARROW, ASSUME ONE SEGMENT OF RULE AND
	// ADD INDENT
	if (!hasSuggested)
		{
		if (bracketStatus > 1)	// Multi-line bracket, after 1st line.	// 05/24/07 AM.
			newLineStr += _T("        ");	// 05/24/07 AM.
		else	// 05/24/07 AM.
			newLineStr += _T("    ");
		}
	pos = tokens.GetHeadPosition();
	int numTokens = tokens.GetCount();
	bool firstTime = true;
	bool spacer = false;
	lastTokenStr = _T("");
	elementCount = 0;
	CString numStr;

	while (pos && (tokenStr = tokens.GetNext(pos))) {

		spacer = (lastTokenStr == _T("[") || tokenStr == _T("]")) ? false : true;

		if (ruleLineNum && spacer && hasComment && commentPos == elementCount)
			newLineStr += _T("\t");
		else if (spacer && !firstTime)
			newLineStr += _T(" ");

		// PLACE COMMENT NUMBER
		if (tokenStr != _T("@@") && ruleLineNum &&
			(!bracketStatus	// Not inside multi-line bracket.	// 05/24/07 AM.
			  || bracketStatus == 3) // Ended multi-line.	// 05/24/07 AM.
			&&
			((hasComment && commentPos + 1 == elementCount) ||
			 (!hasComment && (!pos || !hasBracket || afterBracketPos == elementCount)))) {

			if (!hasComment && tokenStr != _T("@@")) {
				newLineStr += tokenStr;
				newLineStr += _T("\t### ");
			}

			newLineStr += _T("(");
			numStr.Format(_T("%d"),++elementNum);	// 05/24/07 AM.
			newLineStr += numStr;
			newLineStr += _T(")");

			if (hasComment && !hasCommentNumber) {
				newLineStr += _T(" ");
				newLineStr += tokenStr;
			}
		}
		// SKIP COMMENT IF HAS ONE
		//else if (hasCommentNumber && ruleLineNum && commentPos + 1 == elementCount)
			//continue;
		else
			newLineStr += tokenStr;

		lastTokenStr = tokenStr;

		firstTime = false;
		elementCount++;
	}

	return newLineStr;
}

void CSyntaxEditorView::ChangeElement(CString newElementStr)
{
	if (!SelectionInRule())
		return;

	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	CXTPSyntaxEditSelection &sel = edit.GetSelection();

	int startLine = sel.GetStart_str().nLine;
	int index = startLine;
	int cursor = index;
	CString lineStr = edit.GetLineText(index);

	CStringList tokens;
	int bracketStatus = 0;	// 05/24/07 AM.
	TokenizeLine(tokens,lineStr,bracketStatus);

	POSITION pos = tokens.GetHeadPosition();
	bool suggestedFound = false;
	CString newLineStr = _T("");
	CString tokenStr;

	// DETERMINE INSERT POINT
	while (pos && (tokenStr = tokens.GetNext(pos))) {
		if (tokenStr ==_T("<-")) {
			suggestedFound = true;
			break;
		}
	}
	if (!suggestedFound)
		newLineStr += _T("    ");

	pos = tokens.GetHeadPosition();
	bool firstTime = true;
	CString lastTokenStr = _T("");

	while (pos && (tokenStr = tokens.GetNext(pos))) {
		if (firstTime) {
			newLineStr += newElementStr;
			cursor = index + newLineStr.GetLength();
		}
		else {
			if (!firstTime && lastTokenStr != _T("[") && tokenStr != _T("]"))
				newLineStr += _T(" ");
			newLineStr += tokenStr;
		}
		firstTime = false;
		lastTokenStr = tokenStr;
	}

	ReplaceSelection(newLineStr, true);
	ReformatRule(false);
}


// NOTE: Adding some multi-line status.  Variable=bracketStatus	// 05/24/07 AM.
// 0 = no bracket or [] single line bracket.
// 1 = start bracket seen on line.
// 2 = inside bracket, but no start or end on this line.
// 3 = end bracket seen in multi-line bracket.
// NOTE: Still assuming well-behaved rules. Todo is parse comprehensively.
//		Should probably find comments and grab them verbatim.
//		This function should compute the line length as it's gathering the line.
//		(We care only about the end-bracket and out-of-bracket lengths, to set element nums.)

void CSyntaxEditorView::TokenizeLine(CStringList &tokens, CString lineStr,
	int &bracketStatus)	// 05/24/07 AM. 
{
	tokens.RemoveAll();
	CString wordStr = _T("");
	_TCHAR c;
	CString cStr;
	int length = lineStr.GetLength();
	bool inBracket = false;
	bool inParen = false;
	bool backSlashed = false;
	bool bracketStart = false;	// 05/24/07 AM.
	bool bracketEnd = false;	// 05/24/07 AM.

	// Reset bracket status.	// 05/24/07 AM.
	if (bracketStatus == 3)	// 05/24/07 AM.
		bracketStatus = 0;	// 05/24/07 AM.

	for (int i=0; i<length; i++) {
		c = lineStr.GetAt(i);
		cStr = c;

		// Fixing handling of backslash.	// 05/24/07 AM.
		switch (c)	// 05/24/07 AM.
			{
			case '[':	// 05/24/07 AM.
				if (backSlashed)
					{
					wordStr += _T("\\");	// FIX.	// 05/24/07 AM.
					wordStr += c;	// 05/24/07 AM.
					backSlashed = false;
					}
				else	// FIX.	// 05/24/07 AM.
					{
					wordStr = _T("");
					tokens.AddTail(cStr);
					bracketStart = true;	// This line started the bracket.	// 05/24/07 AM.
					}
				break;
			case '(':	// 05/24/07 AM.
				if (backSlashed) {
					wordStr += _T("\\");	// FIX.	// 05/24/07 AM.
					backSlashed = false;
				}
				else
					inParen = true;
				wordStr += c;	// 05/24/07 AM.				
				break;
			case ')':	// FIX.	// 05/24/07 AM.
				if (backSlashed)	// FIX.	// 05/24/07 AM.
					{
					wordStr += _T("\\");	// FIX.	// 05/24/07 AM.
					wordStr += c;	// 05/24/07 AM.
					backSlashed = false;	// FIX.	// 05/24/07 AM.
					}
				else
					{
					wordStr += c;	// 05/24/07 AM.
					tokens.AddTail(wordStr);
					wordStr = _T("");
					inParen = false;
					}
				break;
			case '\\':	// 05/24/07 AM.
				if (backSlashed)	// FIX.	// 05/24/07 AM.
					{
					wordStr = _T("\\");	// FIX.	// 05/24/07 AM.
					wordStr += c;	// 05/24/07 AM.
					backSlashed = false;	// FIX.	// 05/24/07 AM.
					}
				else
					backSlashed = true;
				break;
			case ']':	// 05/24/07 AM>
				if (backSlashed)
					{
					wordStr += _T("\\");	// FIX.	// 05/24/07 AM.
					wordStr += c;	// 05/24/07 AM.
					backSlashed = false;
					}
				else
					{
					if (wordStr != _T(""))
						{
						tokens.AddTail(wordStr);
						wordStr = _T("");
						}
					tokens.AddTail(cStr);
					wordStr = _T("");
					bracketEnd = true;	// This line ended the bracket.	// 05/24/07 AM.
					}
				break;
			default:	// 05/24/07 AM.
				// Any backslashed character.
				if (backSlashed)	// FIX.	// 05/24/07 AM.
					{
					wordStr += _T("\\");	// FIX.	// 05/24/07 AM.
					wordStr += c;	// 05/24/07 AM.
					backSlashed = false;	// FIX.	// 05/24/07 AM.
					}
				else if (!inParen && _istspace(c))
					{
					if (wordStr != _T(""))
						{
						tokens.AddTail(wordStr);
						wordStr = _T("");
						}
					}
				else
					wordStr += c;	// 05/24/07 AM.
				break;
			}	// end switch


		}	// end for loop

	if (wordStr != _T(""))
		tokens.AddTail(wordStr);
	if (bracketStart && bracketEnd)	// 05/24/07 AM.
		bracketStatus = 0;	// 05/24/07 AM.
	else if (bracketStart)	// 05/24/07 AM.
		bracketStatus = 1;	// 05/24/07 AM.
	else if (bracketEnd)		// 05/24/07 AM.
		{
		if (bracketStatus)	// Multi-line bracket.	// 05/24/07 AM.
			bracketStatus = 3;	// End of multi-line.	// 05/24/07 AM.
		// Else error, there was no start bracket.
		}
	else if (bracketStatus)	// Inside multi-line bracket.	// 05/24/07 AM.
		bracketStatus = 2;	// No brackets on this line.	// 05/24/07 AM.
}

bool CSyntaxEditorView::SelectionInRule(bool noAlert)
{
	bool inRule = true;
	long startRow,endRow;
	CStringList ruleLines;
	GetRuleLines(ruleLines,startRow,endRow,noAlert);
	if (ruleLines.GetCount() < 3)
		inRule = false;
	return inRule;
}

/*
bool CSyntaxEditorView::GetWorkingText(CString &lineStr, long &lineIndex,
									 long &selStart, long &selEnd, CString alertMsgStr)
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	edit.GetSel(selStart,selEnd);

	int startLineNum = edit.LineFromChar(selStart);
	int endLineNum = edit.LineFromChar(selEnd);

	if (startLineNum != endLineNum && alertMsgStr != _T("")) {
		CString msgStr;
		msgStr.Format(_T("When %s, simply click on the line. Please choose one of the following:"),alertMsgStr);
		AskUser(msgStr,IDR_WAVE_RULEJOCKEY,2,1,_T("Unselect All"),_T("Cancel"),_T(""));
		if (appButtonReturn == 1) {
			edit.SetSel(selStart,selStart);
		}
		return false;
	}

	_TCHAR lineBuff[10001];
	lineIndex = edit.LineIndex(startLineNum);
	int lineLength = edit.GetLine(startLineNum,lineBuff,10000);
	lineBuff[lineLength] = '\0';
	lineStr = lineBuff;
	return true;
}
*/

CURSOR_LINE_TYPE CSyntaxEditorView::GetCursorLineType()
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	CXTPSyntaxEditSelection &sel = edit.GetSelection();

	int line = sel.GetStart_str().nLine;
	int startLine = line;
	int cursorLine = line;
	CString lineStr = edit.GetLineText(startLine);

	bool found = false;
	bool firstTime = true;
	bool suggested,ruleEnding;
	bool abort = false;

	bool atEndAlready = false;
	bool ruleOneLine = false;
	int lineLength;
	int numLines = 0;
	CURSOR_LINE_TYPE type;

	// SEARCH UP FROM CURSOR
	while (line >= 0) {
		lineStr = edit.GetLineText(line);
		lineLength = lineStr.GetLength();
		
		suggested = lineStr.Find(_T("<-"),0) >= 0;
		ruleEnding = lineStr.Find(_T("@@")) > 0;

		if (lineStr == _T("\n") && numLines == 0)
			return CURSOR_LINE_BLANK;

		if (suggested && ruleEnding && line == cursorLine) {
			return CURSOR_LINE_RULE_UNFORMATTED;
		}

		if (lineStr.Find(_T("@"),0) == 0) {
			type = CURSOR_LINE_NORMAL;
			break;
		}

		numLines++;

		if (lineStr.Find(_T("@@")) >= 0 && firstTime) {
			atEndAlready = true;
		}
		else if (suggested && (!ruleEnding || firstTime)) {
			found = true;
			break;
		}
		line--;
		firstTime = false;
	}

	int firstLine = line;

	if (!found)
		return CURSOR_LINE_NORMAL;

	// SEARCH DOWN FROM CURSOR

	line = startLine;
	int index = 0;

	while (!abort && !atEndAlready && line >= 0 && index >= 0) {
		lineStr = edit.GetLineText(line);
		lineLength = lineStr.GetLength();

		if (lineStr.Find(_T("@"),0) == 0) {
			break;
		}

		line++;
		numLines++;
		
		if (lineStr.Find(_T("@@"),0) >= 0) {
			found = true;
			break;
		}
	}

	if (!abort && numLines >= 3 && found) {
		type = CURSOR_LINE_RULE_FORMATTED;
		if (cursorLine == --line)
			type = CURSOR_LINE_RULE_FORMATTED_LAST;
		else if (cursorLine == firstLine)
			type = CURSOR_LINE_RULE_FORMATTED_FIRST;
	}

	return type;
}

void CSyntaxEditorView::GetRuleLines(CStringList &ruleLines, long &startRow, long &endRow, bool noAlert)
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	CXTPSyntaxEditSelection sel = edit.GetSelection();
	int lineStart = sel.GetStart_str().nLine;
	startRow = lineStart;

	int index;
	bool found = false;
	bool firstTime = true;
	bool suggested,ruleEnding;
	bool abort = false;

	ruleLines.RemoveAll();
	bool atEndAlready = false;
	bool ruleOneLine = false;
	int line = startRow;
	CString lineStr;

	// SEARCH UP FROM CURSOR
	while (line >= 0) {
		lineStr = edit.GetLineText(line); 

		suggested = lineStr.Find(_T("<-"),0) >= 0;
		ruleEnding = lineStr.Find(_T("@@")) > 0;

		if (suggested && ruleEnding) {
			abort = true;
			ruleOneLine = true;
			break;
		}

		if (lineStr.Find(_T("@"),0) == 0) {
			abort = true;
			break;
		}

		if (lineStr.Find(_T("@@")) >= 0 && firstTime) {
			atEndAlready = true;
			break;
		}
		else if (suggested && (!ruleEnding || firstTime)) {
			found = true;
			break;
		}
		firstTime = false;
		line--;
		ruleLines.AddHead(lineStr);
	}
	startRow = line;

	// SEARCH DOWN FROM CURSOR

	//line = edit.LineFromChar(-1) + 1;
	line = lineStart + 1;
	index = 0;

	if (!found)
		ruleLines.RemoveAll();

	while (!abort && !atEndAlready && line >= 0 && index >= 0) {
		lineStr = edit.GetLineText(line); 
		ruleLines.AddTail(lineStr);

		if (lineStr.Find(_T("@"),0) == 0) {
			abort = true;
			break;
		}

		if (lineStr.Find(_T("@@"),0) >= 0) {
			found = true;
			endRow = line;
			break;
		}
		line++;
	}

	if (abort && !noAlert) {
		ruleLines.RemoveAll();
		CString msgStr;
		if (ruleOneLine) {
			msgStr = _T("This function only works for formatted rules that are not all on one line.\n\nTo perform rule editing, right click on \"convert > One Line to Mutiple\".");
			AskUser(msgStr,IDR_WAVE_RULEJOCKEY,1,1,_T("Ok"),_T(""),_T(""));
		}
		else {
			msgStr = _T("No rule was detected where you clicked.\n\nTo perform rule editing, click somewhere within a rule.");
			AskUser(msgStr,IDR_WAVE_RULEJOCKEY,1,1,_T("Ok"),_T(""),_T(""));
		}
	}

	//end--;
}

void CSyntaxEditorView::InsertString(CString str) 
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	edit.ReplaceSel(str);
	ReformatRule(false);
}

void CSyntaxEditorView::OnRulefileInsertRules() 
{
	InsertString(_T("@RULES\n\n_xNIL <-\n    _xALPHA [s]\t### (1)\n    @@\n"));
}

void CSyntaxEditorView::OnRulefileInsertPost() 
{
	InsertString(_T("@POST\n    single();\n"));	
}

void CSyntaxEditorView::OnRulefileInsertXwhite() 
{
	InsertString(_T("_xWHITE"));	
}
void CSyntaxEditorView::OnRulefileChangeXwild() 
{
	ChangeElement(_T("_xWILD"));
}

void CSyntaxEditorView::OnRulefileChangeXnum() 
{
	ChangeElement(_T("_xNUM"));
}

void CSyntaxEditorView::OnRulefileChangeXalpha() 
{
	ChangeElement(_T("_xALPHA"));
}

void CSyntaxEditorView::OnRulefileAddremoveTrig() 
{
	AddAttribute(_T("trig"));
}

void CSyntaxEditorView::OnRulefileAddMatches() 
{
	AddAttribute(_T("matches"),_T("()"));
}

void CSyntaxEditorView::OnRulefileAddExcept() 
{
	AddAttribute(_T("except"),_T("()"));
}

void CSyntaxEditorView::OnRulefileAddFails() 
{
	AddAttribute(_T("fails"),_T("()"));
}

void CSyntaxEditorView::OnRulefileAddOptional() 
{
	AddAttribute(_T("optional"));
}

void CSyntaxEditorView::OnRulefileAddStar() 
{
	AddAttribute(_T("star"));
}

void CSyntaxEditorView::OnRulefileAddremoveS() 
{
	AddAttribute(_T("s"));
}

void CSyntaxEditorView::OnRulefileAddremoveMax() 
{
	AddAttribute(_T("max"),_T("0"));
}

void CSyntaxEditorView::OnRulefileAddremoveMin() 
{
	AddAttribute(_T("min"),_T("0"));
}

void CSyntaxEditorView::OnRulefileInsertXpunct() 
{
	InsertString(_T("_xPUNCT"));
}

void CSyntaxEditorView::OnRulefileAddremoveTree() 
{
	AddAttribute(_T("tree"));
}

void CSyntaxEditorView::OnRulefileAddremoveLookahead() 
{
	AddAttribute(_T("lookahead"));
}

void CSyntaxEditorView::OnRulefileInsertXalpha() 
{
	InsertString(_T("_xALPHA"));
}

void CSyntaxEditorView::OnRulefileAddremoveLayer() 
{
	AddAttribute(_T("layer"),_T("()"));
}

void CSyntaxEditorView::OnRulefileAddremoveOne() 
{
	AddAttribute(_T("one"));
}

void CSyntaxEditorView::OnRulefileChangetoXwhite() 
{
	ChangeElement(_T("_xWHITE"));
}

void CSyntaxEditorView::OnRulefileInsertlineXalpha() 
{
	InsertLine(_T("_xALPHA [s]"),1);
}

void CSyntaxEditorView::OnRulefileInsertlineXnum() 
{
	InsertLine(_T("_xNUM [s]"),1);
}

void CSyntaxEditorView::OnRulefileInsertlineXwhite() 
{
	InsertLine(_T("_xWHITE [s star]"),1);
}

void CSyntaxEditorView::OnRulefileInsertlineXwild() 
{
	InsertLine(_T("_xWILD [group=\"_unnamed\"]"),19);
}

void CSyntaxEditorView::OnRulefileAddremovePlus() 
{
	AddAttribute(_T("plus"));
}

void CSyntaxEditorView::OnRulefileDeleteline() 
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	if (edit) {
		ReplaceSelection("",false);
		ReformatRule(false);
	}
}

void CSyntaxEditorView::OnRulefileDuplicateline() 
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	CXTPSyntaxEditSelection sel = edit.GetSelection();
	if (edit) {
		XTP_EDIT_LINECOL start = sel.GetStart_disp();
		CString lineStr = edit.GetLineText(start.nLine);
		edit.InsertRow(start.nLine+1,lineStr + "\n",true);
		ReformatRule(false);
	}
}

void CSyntaxEditorView::OnRulefileCloserest() 
{
	CVisualTextDoc *doc = (CVisualTextDoc *)GetDocument();
	CString filePathStr = doc->GetPathName();
	theApp.DoAllViews(this,_T("VisualText"),_T(""),DOC_ACTION_DESTROY_ALL_BUT);
}

void CSyntaxEditorView::OnRulefileNlphelp() 
{
//	HelpTopic("NLP++");
	HelpTopic(_T("NLP_PP_Stuff/About_NLP++"));	// Trying this.	// 05/31/03 AM.
}

void CSyntaxEditorView::OnRulefileSelectpass() 
{
	CVisualTextDoc *doc = (CVisualTextDoc *)GetDocument();
	CString titleStr = doc->GetTitle();
	CStringList parseList;
	ParseLine(parseList,titleStr,_T("%w (pass %d)"));
	int pass = _ttoi((const _TCHAR *)NthStringListItem(parseList,1));

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->SelectPass(--pass,true);
}


void CSyntaxEditorView::StringToTokens(CStringList &tokens, CString textStr) 
{
	tokens.RemoveAll();

	int i = 0;
	int len = textStr.GetLength();
	_TCHAR c;
	_TCHAR lastC = ' ';
	CString gatherStr = _T("");
	CString newStr;
	CString conceptStr;
	CString specialChar;

	for (i=0; i<=len; i++) {
		if (i < len)
			c = textStr.GetAt(i);
		else
			c = '\0';

		if (alphabetic(c) && (gatherStr == _T("") || !_istdigit(lastC))) {
			gatherStr += c;
		}
		else if (_istdigit(c) && (gatherStr == _T("") || !alphabetic(lastC))) {
			gatherStr += c;
		}
		else {
			if (gatherStr != _T(""))
				tokens.AddTail(gatherStr);
			if (_istpunct(c) || c == ' ') {
				specialChar.Format(_T("\\%c"),c);
				tokens.AddTail(specialChar);
			}
			gatherStr = _T("");
			if (_istdigit(c) || alphabetic(c)) {
				gatherStr = c;
			}
		}
		lastC = c;
	}
}

void CSyntaxEditorView::OnRulefileStringtorule() 
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	CXTPSyntaxEditSelection sel = edit.GetSelection();
	int lineStart = sel.GetStart_str().nLine;

	CString lineStr = edit.GetLineText(lineStart);
	/*
	long lineIndex,selStart,selEnd;
	CString msgStr = _T("converting a string to a rule");
	if (!GetWorkingText(lineStr,lineIndex,selStart,selEnd,msgStr))
		return;
		*/

	CStringList tokens;
	StringToTokens(tokens,lineStr);
	CString tokenStr;
	CString ruleStr = _T("");

	TrimString(lineStr);
	ruleStr += _T("##############################\n");
	ruleStr += _T("# SAMPLE: ") + lineStr + _T("\n");
	ruleStr += _T("##############################\n\n");

	POSITION pos = tokens.GetHeadPosition();
	if (tokens.GetCount())
		ruleStr += _T("_xNIL <- ");
	bool firstTime = true;
	CString numStr;
	int tokenNumber = 0;

	while (pos && (tokenStr = tokens.GetNext(pos))) {
		ruleStr += _T("\n   ");

		if (tokenStr == _T("\\ "))
			ruleStr += _T(" _xWHITE [s star]");
		else {
			ruleStr += _T(" ");
			ruleStr += tokenStr + _T(" [s]");
		}

		numStr.Format(_T(" (%d) "),++tokenNumber);
		ruleStr += _T("\t###") + numStr;

		firstTime = false;
	}

	if (!firstTime)
		ruleStr += _T("\n    @@");

	//GetUndoParmsSelText(NULL, m_crPre, true);

	edit.Select(lineStart, sel.GetStart_str().nCol, sel.GetEnd_str().nLine, sel.GetEnd_str().nCol);
	//edit.SetSel(lineIndex,lineIndex+lineStr.GetLength());

	//GetUndoParmsSelText(&m_strPre, m_cr2Pre);
	
	edit.ReplaceSel(ruleStr);
	SetDirty();

	m_strPost = ruleStr;
	//GetUndoParmsSelText(NULL, m_cr2Post);
	m_cr2Post.cpMin -= ruleStr.GetLength();

	//GetUndoParmsSelText(NULL, m_crPost);
	//AddToUndoList (new CUndoItem (ReverseReplaceTextAtNewLocOp, ReplaceTextAtNewLocOp, m_strPre, m_strPost, m_crPre, m_crPost, m_cr2Pre, m_cr2Post));
}

void CSyntaxEditorView::OnRulefileConvertStringtogeneralrule() 
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	CXTPSyntaxEditSelection sel = edit.GetSelection();
	int lineStart = sel.GetStart_str().nLine;

	CString lineStr = edit.GetLineText(lineStart);

	/*
	CString lineStr;
	long lineIndex,selStart,selEnd;
	CString msgStr = _T("converting a string to a general rule");
	if (!GetWorkingText(lineStr,lineIndex,selStart,selEnd,msgStr))
		return;
	*/

	CStringList tokens;
	StringToTokens(tokens,lineStr);
	CString tokenStr;
	CString ruleStr = _T("");

	TrimString(lineStr);
	ruleStr += _T("##############################\n");
	ruleStr += _T("# SAMPLE: ") + lineStr + _T("\n");
	ruleStr += _T("##############################\n\n");

	POSITION pos = tokens.GetHeadPosition();
	if (tokens.GetCount())
		ruleStr += _T("_xNIL <- ");
	bool firstTime = true;
	CString numStr;
	int tokenNumber = 0;

	while (pos && (tokenStr = tokens.GetNext(pos))) {
		ruleStr += _T("\n   ");

		if (tokenStr == _T("\\ "))
			ruleStr += _T(" _xWHITE [s star]");
		else if (IsDigit(tokenStr))
			ruleStr += _T(" _xNUM [s]");
		else if (IsAlpha(tokenStr))
			ruleStr += _T(" _xALPHA [s]");
		else {
			ruleStr += _T(" ");
			ruleStr += tokenStr + _T(" [s]");
		}

		numStr.Format(_T(" (%d) "),++tokenNumber);
		ruleStr += _T("\t###") + numStr;
		if (tokenStr != _T("\\ "))
			ruleStr += tokenStr;

		firstTime = false;
	}

	if (!firstTime)
		ruleStr += _T("\n    @@");

	ReplaceSelection(ruleStr, true);
}

void CSyntaxEditorView::OnEditFindSilent() 
{
	if (!GetFindReplaceDlg())
		return;

	CString csSelText;
	GetEditCtrl().GetSelectionText(csSelText);

	if (csSelText.IsEmpty()) 
	{
		CPoint pt(GetCaretPos());
		pt.y += 2;

		GetEditCtrl().SelectWord(pt);
		GetEditCtrl().GetSelectionText(csSelText);
	}

	csSelText = csSelText.SpanExcluding(_T("\r\n"));
	csSelText.Replace(_T("\t"), _T("    "));

	if (csSelText.IsEmpty()) 
	{
		csSelText = _T(" ");
	}
	else
	{
		GetFindReplaceDlg()->m_csFindText = csSelText;
	}
    
	BOOL bShiftKey  = (::GetKeyState(VK_SHIFT) & KF_UP) != 0;

	if (GetEditCtrl().Find(csSelText,
		GetFindReplaceDlg()->m_bMatchWholeWord,
		GetFindReplaceDlg()->m_bMatchCase, !bShiftKey))
	{
		SetDirty();		
	}
}

void CSyntaxEditorView::OnRulefileRulehelp() 
{
//	HelpTopic("Rule_Files");
	HelpTopic(_T("NLP_PP_Stuff/About_Pass_Files"));	// 05/31/03 AM.
}

void CSyntaxEditorView::OnRulefileFind() 
{
	long start,end;
	if (!GetSelTextTrim(start,end,false,false))
		return;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->FindWord();
}

void CSyntaxEditorView::OnRulefileCloseall() 
{
	theApp.DoAllViews(this,_T("VisualText"),_T("pat"),DOC_ACTION_DESTROY_ALL_BUT);
}

void CSyntaxEditorView::OnRulefileSaverunlog() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
/*
	OnFileSave();
	wnd->ClearOutput();
	theApp.RunAnalyzer();
	CDocument *doc = GetDocument();
	CDocument *logDoc = theApp.ViewLogFile(doc);

	if (GetCursorLine(appAskText,0)) {
		CVisualTextView *view = (CVisualTextView *)GetFirstView(logDoc);
		view->FindText(appAskText);
	}
	*/
}

void CSyntaxEditorView::OnRulefileRun() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->Run();
}

void CSyntaxEditorView::OnRulefileViewlog() 
{
	CString titleStr = GetDocument()->GetTitle();
	int pass = theApp.m_pMainFrame->m_wndAnalyzerView.PassFromFile(titleStr);
	theApp.m_pMainFrame->m_wndAnalyzerView.ViewPassLog(pass,false);
}

void CSyntaxEditorView::OnRulefileHeader() 
{
	if (AskForText(_T("Header:"))) {
		CString selStr;
		CString lineCharStr = _T("=");
		int maxLength = 60;
		int dashLength = (maxLength - appAskText.GetLength()) / 2;
		CString lineStr = RepeatStr(lineCharStr,dashLength);
		selStr += _T("\n#") + lineStr + _T(" ");
		selStr += appAskText;
		selStr += _T(" ") + lineStr;
		if (dashLength % appAskText.GetLength())
			selStr += lineCharStr;
		selStr += _T("\n");
		InsertString(selStr);
	}	
}

void CSyntaxEditorView::OnRulefileFindinlog() 
{
	/*
	if (GetSelTextTrim()) {
		CDocument *doc = theApp.m_pMainFrame->m_wndAnalyzerView.ViewPassLog();
		if (doc) {
			CVisualTextView *view = (CVisualTextView *)GetFirstView(doc);
			view->FindForward();
			//view->FindText(appStrFind);
		}
	}
	*/
}

void CSyntaxEditorView::OnRulefileInsertlineXstart() 
{
	InsertLine(_T("_xSTART [s]"),1);
}

void CSyntaxEditorView::OnRulefileCommentuncomment() 
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	CXTPSyntaxEditSelection sel = edit.GetSelection();

	int i = 0;
	int nStart = sel.GetStart_disp().nLine;
	int nEnd = sel.GetEnd_disp().nLine;

	int finalStart = nStart;
	int finalStartCol = sel.GetStart_disp().nCol;

	if (nStart > nEnd) {
		int z = nStart;
		nStart= nEnd;
		nEnd = z;
	}

	CString lineStr;
	CString newLineStr;
	CString newStr;
	bool thisLineCommented = false;
	bool firstLine = true;
	bool addComments = false;

	for (i=nStart; i<=nEnd; i++) {
		lineStr = edit.GetLineText(i);
		newLineStr = lineStr;

		thisLineCommented = lineStr.GetAt(0) == '#';
		if (firstLine)
			addComments = !thisLineCommented;

		// ADD COMMENTS
		if (addComments && !thisLineCommented) {
			newLineStr = _T("#") + lineStr;
		}
		// REMOVE COMMENTS
		else if (!addComments && thisLineCommented) {
			ClipLeft(newLineStr,1);
		}
		firstLine = false;
		newStr += newLineStr + _T("\n");
	}

	edit.Select(nStart, 1, nEnd + 1, 1);
	edit.ReplaceSel(newStr);
	edit.Select(finalStart, finalStartCol, finalStart, finalStartCol);
}

void CSyntaxEditorView::OnRulefileInsertDumpfile() 
{
	CString lineStr;
	CString textFileNameStr;
	NthPathSegment(GetDocument()->GetTitle(),textFileNameStr,' ',0);
	lineStr.Format(_T("\"%s.txt\" << phrasetext() << \"\\n\";\n"),textFileNameStr);
	InsertString(lineStr);	
}

CString CSyntaxEditorView::EditGetSelText(void)
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();

#ifdef UNICODE
	ASSERT(::IsWindow(edit.m_hWnd));
	CHARRANGE cr;
	cr.cpMin = cr.cpMax = 0;
	edit.SendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);
	int bufSize = (cr.cpMax - cr.cpMin + 1) * 2;
	LPTSTR lpsz = (_TCHAR*)_alloca(bufSize);
	lpsz[0] = NULL;
	GETTEXTEX gte;
	gte.cb = bufSize;
	gte.codepage = 1200;
	gte.flags = GT_SELECTION;
	gte.lpDefaultChar = NULL;
	gte.lpUsedDefChar = NULL;
	edit.SendMessage(EM_GETTEXTEX , (WPARAM)&gte, (LPARAM)lpsz);
	return CString(lpsz);
#else
	return "";
	//return edit.GetSelText();  xxx Dd
#endif
}

bool CSyntaxEditorView::GetSelTextTrim(long &start, long &end, bool trimFlag, bool askFlag)
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	CXTPSyntaxEditSelection sel = edit.GetSelection();

	CString selStr = "";
	edit.GetSelectionText(selStr); 

	// TRIM
	if (trimFlag && selStr != _T("")) {
		int len = selStr.GetLength();
		selStr.TrimRight(_T(" \n\t\r"));
		if (selStr.GetLength() < len)
			end -= len - selStr.GetLength();
		len = selStr.GetLength();
		selStr.TrimLeft(_T(" \n\t\r"));
		if (selStr.GetLength() < len)
			start += len - selStr.GetLength();
	}

	// IF EMPTY, ASK FOR
	if (askFlag && selStr == _T("")) {
		bool returnValue = AskForText(_T("Enter word or phrase:"),appStrFind);
		if (returnValue)
			appStrFind = appAskText;
		return returnValue;
	}
	if (selStr != _T(""))
		appStrFind = selStr;

	return true;
}

void CSyntaxEditorView::OnEditTabify() 
{
	GetEditCtrl().TabifySelection();
}

void CSyntaxEditorView::OnEditUnTabify() 
{
	GetEditCtrl().UnTabifySelection();
}

void CSyntaxEditorView::OnEditUppercase() 
{
	GetEditCtrl().UppercaseSelection();
}

void CSyntaxEditorView::OnEditLowercase() 
{
	GetEditCtrl().LowercaseSelection();
}

void CSyntaxEditorView::OnEditWhiteSpace() 
{
	// toggle whitespace.
	GetEditCtrl().EnableWhiteSpace(
		!GetEditCtrl().IsEnabledWhiteSpace());

	CDocument* pDoc = GetDocument();
	if (pDoc)
	{
		pDoc->UpdateAllViews(this, xtpHintWhiteSpace,
			GetEditCtrl().IsEnabledWhiteSpace() ? (CObject*)1 : NULL);
	}
}

void CSyntaxEditorView::OnUpdateEditWhiteSpace(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetEditCtrl().IsEnabledWhiteSpace());
}

void CSyntaxEditorView::OnEditCollapseAll() 
{
	GetEditCtrl().CollapseAll();
}

void CSyntaxEditorView::OnEditExpandAll() 
{
	GetEditCtrl().ExpandAll();
}

void CSyntaxEditorView::OnToolsBookmark() 
{
	int iRow = GetEditCtrl().GetCurRow();
	GetEditCtrl().AddRemoveBookmark(iRow);
	UpdateAllViews();
}

void CSyntaxEditorView::OnToolsBreakpoint() 
{
	int iRow = GetEditCtrl().GetCurRow();
	GetEditCtrl().AddRemoveBreakPoint(iRow);
	UpdateAllViews();
}

void CSyntaxEditorView::OnToolsNextBookmark() 
{
	GetEditCtrl().NextBookmark();
}

void CSyntaxEditorView::OnUpdateToolsOptions(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetEditCtrl().IsConfigFileMode());
}

void CSyntaxEditorView::OnUpdateToolsNextBookmark(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetEditCtrl().HasBookmarks());
}

void CSyntaxEditorView::OnToolsPrevBookmark() 
{
	GetEditCtrl().PrevBookmark();	
}

void CSyntaxEditorView::OnUpdateToolsPrevBookmark(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetEditCtrl().HasBookmarks());
}

//void CSyntaxEditorView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
//{
//	CViewBase::OnUpdate(pSender, lHint, pHint);
//	
//	if (pSender == this) 
//	{
//		return;
//	}
//
//	switch (lHint)
//	{
//	case xtpHintWhiteSpace:
//		break;
//	}
//	
//	appIntRuleTabSize = GetEditCtrl().GetTabSize();
//}

void CSyntaxEditorView::OnFileGoTo() 
{
	m_dlgGoTo.ShowDialog(&GetEditCtrl(),
		FALSE/*select line*/, TRUE/*hide after search*/);
}

void CSyntaxEditorView::OnReadOnly()
{
	GetEditCtrl().SetReadOnly(!GetEditCtrl().IsReadOnly());
}

void CSyntaxEditorView::OnUpdateReadOnly(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetEditCtrl().IsReadOnly() ? 1 : 0);
}

void CSyntaxEditorView::OnInitialUpdate()
{
	CXTPSyntaxEditView::OnInitialUpdate();

//#include "_cpp1_schclass.h"
//#include "ColorsDataEntry.h"
//	GetEditCtrl().SetConfigFile(_T(""), FALSE);
//
//	GetEditCtrl().m_bUseMonitor = FALSE;
//
//	if (GetEditCtrl().GetEditBuffer())
//	{
//		GetEditCtrl().GetEditBuffer()->SetFileExt(_T(".cpp"));
//	}
//	GetEditCtrl().SetSyntaxAndColorScheme(sScheme, sColorTheme);
//	OnUpdate(NULL, xtpEditHintInitView, NULL);
}

void CSyntaxEditorView::SaveState() 
{
	theApp.AddStatus(_T("ruleopen"),StateStr());
}

CString CSyntaxEditorView::StateStr() 
{
	CSyntaxEditorDoc *doc = GetDocument();
	CString pathName = doc->GetPathName();
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();

	// Don't ask me how I found this.  Took one year!
	WINDOWPLACEMENT wp;
	GetParentFrame()->GetWindowPlacement(&wp);

	int active = 0;
	int scrollPos = edit.GetScrollPos(SB_VERT);
	CXTPSyntaxEditSelection sel = edit.GetSelection();
	XTP_EDIT_LINECOL start = sel.GetStart_disp();
	XTP_EDIT_LINECOL end = sel.GetEnd_disp();

 	CString posStr;
	posStr.Format(_T("%s%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d"),	// FIX DUE TO PURIFY // 08/02/07 AM.
		RelativizePath(pathName),STATUSSEP,
		wp.rcNormalPosition.top,STATUSSEP,
		wp.rcNormalPosition.bottom,STATUSSEP,
		wp.rcNormalPosition.left,STATUSSEP,
		wp.rcNormalPosition.right,STATUSSEP,
		scrollPos,STATUSSEP,
		active,STATUSSEP,
		start.nLine,STATUSSEP,
		start.nCol,STATUSSEP,
		end.nLine,STATUSSEP,
		end.nCol
		);

	return posStr;
}

void CSyntaxEditorView::OnToolsRuleeditoptions()
{
	// Create dialog object
	CXTPSyntaxEditPropertiesDlg dlg(this);

	// Instantiate dialog
	if (dlg.DoModal() == IDOK)
	{
		// Copy settings to remaining views.
		theApp.UpdateSyntaxAllViews(this);
	}
	
	//GetEditCtrl().EnableEditAccelerators(FALSE);
}

void CSyntaxEditorView::OnUpdateToolsRuleeditoptions(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetEditCtrl().IsConfigFileMode());
}

void CSyntaxEditorView::OnRulefileReformatrule()
{
	ReformatRule(true);
}

bool CSyntaxEditorView::ReformatRule(BOOL bLink)
{
	SetCursorLineType();
	if (m_cursorLineType != CURSOR_LINE_RULE_FORMATTED
		&& m_cursorLineType != CURSOR_LINE_RULE_FORMATTED_FIRST
		&& m_cursorLineType != CURSOR_LINE_RULE_FORMATTED_LAST)
		return false;

	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	CXTPSyntaxEditSelection &sel = edit.GetSelection();
	int s = sel.GetStart_disp().nLine;
	int sCol = sel.GetStart_disp().nCol;
	int e = sel.GetEnd_disp().nLine;
	int eCol = sel.GetEnd_disp().nCol;

	long start,end;
	CStringList ruleLines;
	GetRuleLines(ruleLines,start,end);

	if (ruleLines.GetCount() < 3)
		return false;

	POSITION pos = ruleLines.GetHeadPosition();
	CString newLinesStr;
	CString newLineStr;
	CString lineStr;
	CStringList newLines;
	int lineNum = 1;
	int elementNum = 0;	// Track rule element number.	// 05/24/07 AM.
	int bracketStatus = 0;	// Track multi-line brackets.	// 05/24/07 AM.

	// DETERMINE INSERT POINT
	int furthestTab = 0;
	int tab;
	while (pos && (lineStr = ruleLines.GetNext(pos))) {
		newLineStr = StandardizeLine(lineStr,elementNum,bracketStatus,lineNum++);
		if (bracketStatus == 1 || bracketStatus == 2)	// Multi-line brackets. // 05/24/07 AM>
			tab = 1+newLineStr.GetLength();	// 05/24/07 AM.
		else	// 05/24/07 AM.
			tab = newLineStr.ReverseFind('\t');
		if (tab > 60)	// 05/24/07 AM.
			tab = 60;	// 05/24/07 AM.
		if (tab > appIntRuleTabSize && tab > furthestTab)
			furthestTab = tab;
		newLines.AddTail(newLineStr);
	}

	// TAKE CARE OF LINING UP TABS
	pos = newLines.GetHeadPosition();
	lineNum = 0;
	int i,tabCount,adjustedTab,furthestAdjusted;
	CString tabStr;
	furthestAdjusted = (furthestTab / appIntRuleTabSize) * appIntRuleTabSize + appIntRuleTabSize;
	newLinesStr = "";

	while (pos && (newLineStr = newLines.GetNext(pos))) {
		if (lineNum++)
			newLinesStr += _T("\n");
		tab = newLineStr.ReverseFind('\t');
		if (tab > appIntRuleTabSize && tab < furthestTab) {
			tabStr = _T("");
			adjustedTab = (tab / appIntRuleTabSize) * appIntRuleTabSize + appIntRuleTabSize;
			tabCount = (furthestAdjusted - adjustedTab) / appIntRuleTabSize;
			for (i=0; i<tabCount; i++)
				tabStr += _T("\t");
			if (tabStr != _T(""))
				newLineStr.Insert(tab,tabStr);
		}
		newLinesStr += newLineStr;
	}

	int col1 = sel.GetStart_disp().nCol;
	int col2 = sel.GetEnd_disp().nCol;

	lineStr = edit.GetLineText(end);
	CString tabSpace;
	for (int j=0; j<edit.GetTabSize(); j++) {
		tabSpace += _T(" ");
	}
	lineStr.Replace(_T("\t"),tabSpace);
	edit.Select(start + 1, 1, end, lineStr.GetLength() + 1);

	edit.ReplaceSel(newLinesStr);
	edit.Select(s, sCol, e, eCol);

	return true;
}

void CSyntaxEditorView::OnSetFocus(CWnd* pOldWnd)
{
	CXTPSyntaxEditView::OnSetFocus(pOldWnd);

	appBoolDocDirty = GetDocument()->IsModified() ? true : false;;
}

void CSyntaxEditorView::OnUpdateFileNew(CCmdUI *pCmdUI)
{
	appBoolDocDirty = GetDocument()->IsModified() ? true : false;;
}

void CSyntaxEditorView::OnUpdateSaveAll(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.DirtyDocs());
}

void CSyntaxEditorView::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->IsModified());
}

// NEED TO TEST
void CSyntaxEditorView::AppendToText(CString textStr, int blankLines) 
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	edit.SelectAll();
	CXTPSyntaxEditSelection sel = edit.GetSelection();

	int nLine = sel.GetEnd_disp().nLine;
	int nCol = sel.GetEnd_disp().nCol;
	edit.Unselect();

	edit.InsertString(_T("\n\n") + textStr, nLine, nCol, 0);
}

void CSyntaxEditorView::OnRulefileHelponselection() 
{
	CXTPSyntaxEditCtrl &edit = GetEditCtrl();
	CXTPSyntaxEditSelection &sel = edit.GetSelection();

	int nLine = sel.GetStart_str().nLine;
	int nCol = sel.GetStart_str().nCol;
	CString lineStr = edit.GetLineText(nLine);

	int length = lineStr.GetLength(); 
	int start = 0;
	int end = length;
	_TCHAR c;
	CString cStr;

	// Forward
	for (int i=nCol; i<length; i++) {
		c = lineStr.GetAt(i);
		if (!alphabetic(c)) {
			end = i;
			break;
		}
	}

	// backward
	if (nCol > 0) nCol--;
	for (int i=nCol; i>=0; i--) {
		c = lineStr.GetAt(i);
		if (!alphabetic(c)) {
			start = i + 1;
			break;
		}
	}
	CString helpStr;
	helpStr = lineStr.Mid(start,end-start);

	HelpTopic(helpStr);	
}

void CSyntaxEditorView::OnViewOptions()
{
	// Create dialog object
	CXTPSyntaxEditPropertiesDlg dlg(this);

	// Instantiate dialog
	if (dlg.DoModal() == IDOK)
	{
		// Copy settings to remaining views.
		//theApp.UpdateAllViews(this);
		theApp.DoAllViews(NULL, _T("SyntaxEditor"), _T(""), DOC_ACTION_UPDATE);
	}

	GetEditCtrl().EnableEditAccelerators(FALSE);
}
