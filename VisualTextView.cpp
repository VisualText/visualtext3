/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// VisualTextView.cpp : implementation of the CVisualTextView class
//

//#define _NOT_USED 0

#include "stdafx.h"
#include "Utils.h"
#include "kb.h"
#include "MainFrm.h"
#include "AttributeEditor.h"
#include "CntrItem.h"
#include "TreeViewGrammar.h"
#include "MarkSampleDlg.h"
#include "HexEditDlg.h"
#include "FileCharViewerDlg.h"
#include "TextInfoDlg.h"
#include "EmailDlg.h"
#include "FindDlg.h"
#include "NLPStrs.h"
#include "UserLogFile.h"
#include "KnowledgeView.h"
#include "PrefLookupPage.h"
#include "TreeViewAnalyzer.h"
#include "ASCIIDlg.h"
#include "SyntaxEditorDoc.h"
#include "SyntaxEditorView.h"
#include "VisualTextView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString appAskText;
extern CString appStrFocusDocumentType;
extern CString appStrSelText;
extern CString appStrFind;
extern CVisualTextApp theApp;
extern CG *cg;
extern VT_APP_STATE appState;
extern CFont appFontFixed;
extern CDocument *appDocFocus;

extern int appIntOpenWindowCount;
extern int appIntLastWindowX;
extern int appIntLastWindowY;
extern int appIntWindowShiftX;
extern int appIntWindowShiftY;
extern int appIntOpenWindowCount;
extern int appIntMaxWindowWidth;
extern int appIntMaxWindowHeight;
extern int appIntRuleTabSize;

extern bool appBoolAutosizeWindows;
extern bool appBoolDocExists;
extern bool appBoolOpeningTextFile;
extern bool appBoolDocDirty;
extern bool appBoolDocsDirty;
extern bool appBoolTabbedWindows;
extern int appButtonReturn;

/////////////////////////////////////////////////////////////////////////////
// CVisualTextView

IMPLEMENT_DYNCREATE(CVisualTextView, CRichEditView)

AFX_STATIC const UINT _VTVMsgFindReplace = ::RegisterWindowMessage(FINDMSGSTRING);
BEGIN_MESSAGE_MAP(CVisualTextView, CRichEditView)
	//{{AFX_MSG_MAP(CVisualTextView)
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_FILE_NEWANALYZER, OnFileNewAnalyzer)
	ON_COMMAND(ID_FILE_NEW,OnFileNew)
	ON_COMMAND(ID_RULEFILE_HIGHLIGHTMATCHES, OnRulefileHighlightmatches)
	ON_COMMAND(ID_TEXTVIEW_SELECTANDRUN, OnTextviewSelectandrun)
	ON_COMMAND(ID_TEXT_DISPLAYTREE, OnTextDisplaytree)
	ON_COMMAND(ID_TEXT_WORDWRAPON, OnWordWrapToggle)
	ON_COMMAND(ID_TEXT_VIEWOUTPUT, OnTextViewoutput)
	ON_COMMAND(ID_TEXT_VIEWENTIRETREE, OnTextViewentiretree)
	ON_COMMAND(ID_SAMPLEMENU_GENERATESAMPLES, OnSamplemenuGeneratesamples)
	ON_COMMAND(ID_SAMPLEMENU_SORT, OnSamplemenuSort)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_TEXT_SORTTEXT, OnTextSorttext)
	ON_COMMAND(ID_TEXT_TOOLS_HEXVIEWER, OnTextToolsHexviewer)
	ON_COMMAND(ID_TEXT_TOOLS_ASCIITABLE, OnTextToolsAsciitable)
	ON_COMMAND(ID_TEXT_TOOLS_TEXTFILEINFO, OnTextToolsTextfileinfo)
	ON_COMMAND(ID_TEXT_TOOLS_DICTIONARYLOOKUP, OnTextToolsDictionaryLookup)
	ON_COMMAND(ID_TEXT_TOOLS_DICTIONIZE, OnTextToolsDictionize)
	ON_COMMAND(ID_TEXT_TOOLS_MASK, OnTextToolsMask)
	ON_COMMAND(ID_TOOLS_CHARACTERVIEWER, OnToolsCharacterviewer)
	ON_COMMAND(ID_TEXT_EMAIL, OnTextEmail)
	ON_COMMAND(ID_TEXT_WRAPTEXT, OnTextWraptext)
	ON_WM_CHAR() // SRP 2001-02-01
	ON_COMMAND(ID_EDIT_CUT, OnEditCut) // SRP 2001-02-01
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste) // SRP 2001-02-01
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear) // SRP 2001-02-01
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo) // SRP 2001-02-01
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo) // SRP 2001-04-19
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_SAVE_ALL, OnSaveAll)
	ON_COMMAND(ID_LOGFILE_FINDLOCAL, OnLogfileFindlocal)
	ON_COMMAND(ID_LOGFILE_FINDAGAIN, OnLogfileFindagain)
	ON_COMMAND(ID_RULEFILE_INSERT_XNUM, OnRulefileInsertXnum)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo) // SRP 2001-04-19
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo) // SRP 2001-04-19
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_RULEFILE_HELPONSELECTION, OnRulefileHelponselection)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_COMMAND(ID_TEXT_DICTIONARY_KBATTRIBUTES, OnTextDictionaryKbattributes)
	ON_COMMAND(ID_TEXT_DICTIONARY_ADDWORDS, OnTextDictionaryAddwords)
	ON_COMMAND(ID_TEXT_TOOLS_JUMBLE, OnTextToolsJumble)
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_FONTS, OnFonts)
	ON_COMMAND(ID_TEXT_GENERATERULE, OnTextGeneraterule)
	ON_COMMAND(ID_LOGFILE_VIEWRULEFILE, OnLogfileViewrulefile)
	ON_COMMAND(ID_FIND_IN_FILES, OnFindinfiles)
	ON_COMMAND(ID_LOGFILE_FINDINRULEFILE, OnLogfileFindinrulefile)

	//}}AFX_MSG_MAP
	// Standard printing commands
//	ON_REGISTERED_MESSAGE(_VTVMsgFindReplace, OnFindReplaceCmd)
	ON_COMMAND(ID_FILE_PRINT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CRichEditView::OnFilePrintPreview)
	ON_COMMAND_RANGE(SAMPLE_ID_START, SAMPLE_ID_END, OnSampleViewAdd)
	ON_COMMAND_RANGE(TEXTVIEW_ID_START, TEXTVIEW_ID_END, OnSampleAdd)
	ON_COMMAND_RANGE(ID_DICTIONARY_URL_LOOKUP_BASE, ID_DICTIONARY_URL_LOOKUP_MAX, OnDictionaryURLLookup)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CVisualTextView::OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_SAVE_ALL, &CVisualTextView::OnUpdateSaveAll)
END_MESSAGE_MAP()

	/*
	
#if _NOT_USED
	ON_COMMAND(ID_RULEFILE_MAKEPHRASES, OnRulefileMakephrases)
	ON_COMMAND(ID_TEXT_ADDEXAMPLE, OnTextAddexample)
	ON_COMMAND(ID_TEXT_ADDEXAMPLE, OnTextAddexample)
	ON_COMMAND(ID_RULEFILE_SORTWORDS, OnRulefileSortwords)
#endif
	ON_COMMAND(ID_SAMPLEMENU_SORT, OnSamplemenuSort)
	ON_COMMAND(ID_SAMPLEMENU_GENERATESAMPLES, OnSamplemenuGeneratesamples)
	ON_COMMAND_RANGE(SAMPLE_ID_START, SAMPLE_ID_END, OnSampleViewAdd)
	ON_COMMAND_RANGE(TEXTVIEW_ID_START, TEXTVIEW_ID_END, OnSampleAdd)
	*/

/////////////////////////////////////////////////////////////////////////////
// CVisualTextView construction/destruction

CVisualTextView::CVisualTextView()
{
	InitVars();
}

CVisualTextView::~CVisualTextView()
{
	m_strURLs.RemoveAll();
	RemoveUndoList (); // SRP 010419
}

void CVisualTextView::InitVars()
{
	m_chComment = 1;
	m_bCaseSensitive = false;
	m_bChangeCase = true;
	m_boolFirstTime = true;
	m_boolIsSampleFile = false;
	m_docType = DOC_FILE;

	SetStringQuotes(_T("\""));

	SetZoneColor(RGB(0,0,255), true);
	SetAttributeColor(RGB(171,130,255), true);
	SetConstantColor(RGB(255,0,0), true);
	SetCommentColor(RGB(0,128,0), false);
	SetNLPColor(RGB(148,0,211), true);
	SetNodeColor(RGB(255,0,0), true);
	SetStringColor(RGB(255,0,255), false);

	m_bInForcedChange = false;
	m_changeType = ctUndo;
	m_crOldSel.cpMin = m_crOldSel.cpMax = 0;
	m_bCursorAtEndOfDocument = false;

	m_bCutOp = false;
	m_UndoList.RemoveAll(); // SRP 010419
	m_IndexUndo = 0; // SRP 010419
	m_bInsertMode = true;

	SetMargins(theApp.m_rectPrintMargins);
}

BOOL CVisualTextView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

    //cs.style |= ( WS_CHILD | ES_MULTILINE | ES_WANTRETURN | ES_NOHIDESEL );
	
	appIntOpenWindowCount++;

	return CRichEditView::PreCreateWindow(cs);
}

void CVisualTextView::OnInitialUpdate()
{
	Initialize();

	SetCaseSensitive(false);
	SetSLComment(_T('#'));

	CVisualTextDoc *doc = (CVisualTextDoc *)GetDocument();
	CString pathStr = doc->GetPathName();
	m_concept = theApp.m_pMainFrame->m_wndTextView.ConceptFromTextPath(pathStr,true);
	m_boolIsSampleFile = FileHasSamples(pathStr);
	m_boolIsAutoGenerated = cg->attrWithval(m_concept,_T("comment"),_T("RUG file!"));

	m_docType = doc->m_DocType;
	CalculateIsAnalyzed();

	//AddKeywords(szKeywords);
	//AddConstants(szConstants);
	//AddLargers(szLargers);

	FormatAll();
	GetRichEditCtrl().SetModify(false);

	SetIconType();
}

bool CVisualTextView::CalculateIsAnalyzed()
{
	CVisualTextDoc *doc = GetDocument();
	CString pathStr = doc->GetPathName() + _T("_log");
	m_boolIsAnalyzed = IsDirectory(pathStr) ? true : false;;

	return m_boolIsAnalyzed;
}

bool CVisualTextView::IsSampleFile()
{
	return m_boolIsSampleFile;
}

bool CVisualTextView::IsAutoGenerated()
{
	return m_boolIsAutoGenerated;
}

void CVisualTextView::SetIconType()
{	
	CVisualTextDoc *doc = (CVisualTextDoc *)GetDocument();
	unsigned short icon;

	switch (doc->m_DocType) {
	case DOC_FILE:
	case DOC_HTML:
		icon = IDR_TEXT;
		break;
	case DOC_RULE:
	case DOC_REC:
		icon = IDR_RULE;
		break;
	case DOC_KNOWLEDGE:
		icon = IDR_KNOWLEDGE;
		break;
	case DOC_SAMPLE:
		icon = IDR_SAMPLE;
		break;
	case DOC_LOG:
		icon = IDR_LOG;
		break;
	default:
		icon = IDR_VISUALTYPE;
	}

	m_icon = AfxGetApp()->LoadIcon(icon);
	GetParentFrame()->SetIcon(m_icon,true);
}

void CVisualTextView::Initialize() 
{
	CRichEditCtrl &edit = GetRichEditCtrl();

	edit.SetFont(&appFontFixed);
	//edit.SetDefaultCharFormat(cfDefault);

	edit.SetEventMask(ENM_CHANGE | ENM_SELCHANGE | ENM_PROTECTED);
//	edit.SetOptions(ECOOP_XOR,ECO_AUTOWORDSELECTION);

	CDC *pDC = GetParentFrame()->GetDC();	

	pDC->SelectObject(&appFontFixed);
#ifdef UNICODE
	pDC->GetTextMetricsW(&m_tm);
	m_intCharWidth = m_tm.tmAveCharWidth;
#else
	pDC->GetTextMetrics(&m_tm);
	m_intCharWidth = m_tm.tmMaxCharWidth;
#endif
	m_intLineHeight = m_tm.tmHeight + m_tm.tmExternalLeading;

	LOGFONT lf;
	appFontFixed.GetObject(sizeof(lf), &lf);
	m_strFontFaceName = lf.lfFaceName;
	m_FontHeight = -MulDiv(lf.lfHeight, 72, pDC->GetDeviceCaps(LOGPIXELSY)) * 20; // twips

	GetParentFrame()->ReleaseDC(pDC);

	// OBSCURE CODE: TURNS OFF AUTOMATIC WRAPPING FOR RICH TEXT EDIT
	edit.SetTargetDevice(NULL, 1);
	m_wordWrap = false;

	CVisualTextDoc *doc = GetDocument();
	doc->Update();

	edit.HideSelection(TRUE, FALSE);
	edit.SetSel(0, -1); // Full selection
	edit.SetSel(0, 0);
	edit.HideSelection(FALSE, FALSE);
}

void CVisualTextView::SetSLComment(TCHAR chComment, TCHAR chComment2)
{
	m_chComment = chComment;
	m_chComment2 = chComment2;
}

void CVisualTextView::SetCaseSensitive(BOOL bSensitive)
{
	m_bCaseSensitive = bSensitive;
}

void CVisualTextView::SetChangeCase(BOOL bChange)
{
	m_bChangeCase = bChange;
}

void CVisualTextView::SetStringQuotes(LPCTSTR lpszStrQ)
{
	m_strStringQuotes = lpszStrQ;
}

void CVisualTextView::SetSLComment(LPCTSTR lpszComment)
{
	m_strComment = lpszComment;
}

void CVisualTextView::SetZoneColor(COLORREF clr, BOOL bBold)
{
	m_icZone.clrColor = clr;
	m_icZone.bBold = bBold;
}

void CVisualTextView::SetAttributeColor(COLORREF clr, BOOL bBold)
{
	m_icAttribute.clrColor = clr;
	m_icAttribute.bBold = bBold;
}

void CVisualTextView::SetConstantColor(COLORREF clr, BOOL bBold)
{
	m_icConstant.clrColor = clr;
	m_icConstant.bBold = bBold;
}

void CVisualTextView::SetCommentColor(COLORREF clr, BOOL bBold)
{
	m_icComment.clrColor = clr;
	m_icComment.bBold = bBold;
}

void CVisualTextView::SetNLPColor(COLORREF clr, BOOL bBold)
{
	m_icNLP.clrColor = clr;
	m_icNLP.bBold = bBold;
}

void CVisualTextView::SetNodeColor(COLORREF clr, BOOL bBold)
{
	m_icNode.clrColor = clr;
	m_icNode.bBold = bBold;
}

void CVisualTextView::SetStringColor(COLORREF clr, BOOL bBold)
{
	m_icString.clrColor = clr;
	m_icString.bBold = bBold;
}

/////////////////////////////////////////////////////////////////////////////
// CVisualTextView diagnostics

#ifdef _DEBUG
void CVisualTextView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CVisualTextView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CVisualTextDoc* CVisualTextView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVisualTextDoc)));
	return (CVisualTextDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVisualTextView message handlers

int CVisualTextView::IsZone(LPCTSTR lpszSymbol)
{
	return IsInWordList(zoneWords,lpszSymbol,false);
}

int CVisualTextView::IsNodeWord(LPCTSTR lpszSymbol)
{
	return IsInWordList(nodeWords,lpszSymbol);
}

int CVisualTextView::IsNLPWord(LPCTSTR lpszSymbol)
{
	return IsInWordList(nlpWords,lpszSymbol);
}

int CVisualTextView::IsConstant(LPCTSTR lpszSymbol)
{
	return IsInWordList(constantWords,lpszSymbol);
}

int CVisualTextView::IsAttribute(LPCTSTR lpszSymbol)
{
	return IsInWordList(attributeWords,lpszSymbol);
}

BOOL CVisualTextView::IsStringQuote(TCHAR ch)
{
	return (m_strStringQuotes.Find(ch) >= 0);
}



BOOL CVisualTextView::SetFormatRangeFont(int nStart, int nEnd)
{
	if (nStart >= nEnd)
		return false;

	CRichEditCtrl &edit = GetRichEditCtrl();
	//edit.SetSel(nStart, nEnd);  SE de Hilster
	edit.SetSel(nStart, nEnd);

	CHARFORMAT cfm;
	cfm.cbSize = sizeof(cfm);
    edit.GetSelectionCharFormat(cfm);
	
	if ((cfm.dwMask & CFM_FACE)  && cfm.szFaceName == m_strFontFaceName && 
		(cfm.dwMask & CFM_SIZE) && cfm.yHeight == m_FontHeight)
		return false;

	_tcscpy_s(cfm.szFaceName, m_strFontFaceName);
	cfm.yHeight = m_FontHeight;
	cfm.dwMask = CFM_FACE | CFM_SIZE;
	edit.SetSelectionCharFormat(cfm);
	return true;
}

void CVisualTextView::SetFormatRange(int nStart, int nEnd, BOOL bBold, COLORREF clr)
{
	if (nStart >= nEnd)
		return;

	CRichEditCtrl &edit = GetRichEditCtrl();
	//edit.SetSel(nStart, nEnd); SE de Hilster
	edit.SetSel(nStart, nEnd);

	DWORD dwEffects = bBold?CFE_BOLD:0;

	CHARFORMAT cfm;
	cfm.cbSize = sizeof(cfm);
    edit.GetSelectionCharFormat(cfm);
	
	if ((cfm.dwMask & CFM_COLOR)  && cfm.crTextColor == clr && 
		(cfm.dwMask & CFM_BOLD) && (cfm.dwEffects & CFE_BOLD) == dwEffects)
		return;

	cfm.dwEffects = dwEffects;
	cfm.crTextColor = clr;
	cfm.dwMask = CFM_BOLD | CFM_COLOR;

	edit.SetSelectionCharFormat(cfm);
}

void CVisualTextView::ChangeCase(int nStart, int nEnd, LPCTSTR lpsz)
{
	ASSERT((nEnd - nStart) == (int)_tcsclen(lpsz));

	if (!m_bCaseSensitive && m_bChangeCase) {
		CRichEditCtrl &edit = GetRichEditCtrl();
		edit.SetSel(nStart, nEnd);
		edit.ReplaceSel(lpsz);
	}
}

bool CVisualTextView::IsInWordList(_TCHAR *wordList[], CString wordStr, BOOL bCaseSensitive)
{
	return IsInList(wordList,wordStr,_T("XXYYZZ"),bCaseSensitive);
}

CString CVisualTextView::EditGetText(void)
{
	CRichEditCtrl &edit = GetRichEditCtrl();
	ASSERT(::IsWindow(edit.m_hWnd));
	int bufSize = (edit.GetTextLength() + 1) * 2;
	LPTSTR lpsz = (_TCHAR*)_alloca(bufSize);
	lpsz[0] = NULL;
	GETTEXTEX gte;
	gte.cb = bufSize;
	gte.codepage = 1200;
	gte.flags = 0;
	gte.lpDefaultChar = NULL;
	gte.lpUsedDefChar = NULL;
	edit.SendMessage(EM_GETTEXTEX , (WPARAM)&gte, (LPARAM)lpsz);
	return CString(lpsz);
}

CString CVisualTextView::EditGetSelText(void)
{
	CRichEditCtrl &edit = GetRichEditCtrl();
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
	return edit.GetSelText();
#endif
}

void CVisualTextView::FormatTextRange(int nStart, int nEnd)
{
	CVisualTextDoc *doc = GetDocument();
	if (appBoolOpeningTextFile || nStart >= nEnd || doc->m_DocType != DOC_RULE)
		return;

	m_bInForcedChange = true;

	CRichEditCtrl &edit = GetRichEditCtrl();
	CHARRANGE crOldSel;
	edit.GetSel(crOldSel);

	StartWaitCursor();
	
	LockWindowUpdate();

	edit.HideSelection(true, false);

	TCHAR *pBuffer = NULL;
	try {
		edit.SetSel(nStart, nEnd);
//		pBuffer = new TCHAR[nEnd - nStart + 1];  // 05/02/20 SRP
//		long nLen = edit.GetSelText(pBuffer);  // 05/02/20 SRP
		CString strBuffer = EditGetSelText();  // 05/02/21 SRP
		long nLen = nEnd - nStart;  // 05/02/20 SRP
		long nLenEnd = nLen + 1;  // 05/02/20 SRP
		pBuffer = new TCHAR[nLenEnd];  // 05/02/20 SRP
		_tcscpy_s(pBuffer, nLenEnd, strBuffer);  // 05/02/20 SRP

		ASSERT(nLen <= nEnd - nStart);

		pBuffer[nLen] = 0;

		TCHAR *pStart, *pPtr;
		pStart = pPtr = pBuffer;

		TCHAR* pSymbolStart = NULL;
		SymbolColor ic;

		// Adding semicolon for things like "return;".	// 07/12/02 AM.
		CString keyStops = _T(" \n\r\t[]()<>-+*=,!;");	// 07/12/02 AM.
		bool inside = false;
		int nPos = 0;
		TCHAR ch;
		TCHAR chLast = 0;

		while (*pPtr != 0) {
			ch = *pPtr;

			if (ch == m_chComment && (m_chComment2 == 0 || pPtr[1] == m_chComment2)) {
				pSymbolStart = pPtr;
				do {
					ch = *(++pPtr);
				} while (ch != 0 && ch != '\r' && ch != '\n');
				ic = m_icComment;

			} else if (ch == '\\' && chLast == '\\') {	// 03/13/03 AM.
				++pPtr;	// 03/13/03 AM.
			} else if (IsStringQuote(ch)) { // Process strings
				pSymbolStart = pPtr;
				TCHAR chDelimiter = ch;
				do {
					chLast = ch; // SRP 010705 Fix to include backslashed quotes
					ch = *(++pPtr);
					if (ch == '\\' && chLast == '\\')	// 03/13/03 AM.
					  {
					  ch = *(++pPtr);	// 03/13/03 AM.
					  chLast = 0;		// 03/13/03 AM.
					  }
				} while (ch != 0 && (ch != chDelimiter || chLast == '\\') && ch != '\r' && ch != '\n');
				if (ch == chDelimiter) pPtr++;
				ic = m_icString;

			} else if (ch == '[' && chLast != '\\') { // Process numbers
				inside = true;
				pPtr++;

			} else if (ch == ']' && chLast != '\\') { // Process numbers
				inside = false;
				pPtr++;

			} else if (keyStops.Find(ch) < 0) { // Process keywords
				pSymbolStart = pPtr;
				do {
					ch = *(++pPtr);
				} while (keyStops.Find(ch) < 0);
				// while (_istalnum(ch) || ch == '_');
				*pPtr = 0;

				if (inside) {
					nPos = IsAttribute(pSymbolStart);
					ic = m_icAttribute;
				}
				else if ((nPos = IsZone(pSymbolStart))) {
					ic = m_icZone;
				}
				else if ((nPos = IsNLPWord(pSymbolStart))) {
					ic = m_icNLP;
				}
				else if ((nPos = IsNodeWord(pSymbolStart))) {
					ic = m_icNode;
				}

				if (nPos > 0) {
					if (_tcsicmp(m_strComment, pSymbolStart) == 0) {
						*pPtr = ch;
						*pSymbolStart = m_chComment;
						if (pSymbolStart[1] != 0 && m_chComment2 != 0)
							pSymbolStart[1] = m_chComment2;
						pPtr = pSymbolStart;
						pSymbolStart = NULL;
						continue;
					}
				} else {
					nPos = IsConstant(pSymbolStart);
					//if (nPos >= 0) 
					if (nPos > 0) {
						ic = m_icConstant;
					} else {
						pSymbolStart = NULL;
					}
				}
				*pPtr = ch;
			} else {
				pPtr++;
			}

			if (pSymbolStart != NULL) {
				ASSERT(pSymbolStart < pPtr);
				SetFormatRange(nStart + pStart - pBuffer, nStart + pSymbolStart - pBuffer, false, RGB(0,0,0));
				SetFormatRange(nStart + pSymbolStart - pBuffer, nStart + pPtr - pBuffer, ic.bBold, ic.clrColor);
				pStart = pPtr;
				pSymbolStart = 0;
			} else if (*pPtr == 0)
				SetFormatRange(nStart + pStart - pBuffer, nStart + pPtr - pBuffer, false, RGB(0,0,0));
			chLast = ch;
		}

	} catch(...){}

	delete [] pBuffer;

	edit.SetSel(crOldSel);

	edit.HideSelection(false, false);
	UnlockWindowUpdate();
	StopWaitCursor();

	m_bInForcedChange = false;
}

void CVisualTextView::ColorSelections(CString processTextStr, CStringList &selList)
{
	LockWindowUpdate();

	CRichEditCtrl &edit = GetRichEditCtrl();
	CHARRANGE selRange;
	edit.GetSel(selRange);
	int visLine = edit.GetFirstVisibleLine();

	SetFormatRange(0,edit.GetTextLength(),false,RGB(0,0,0));

	long start = 0;
	long end = 0;
	int i = 0;
	POSITION pos = selList.GetHeadPosition();
	CString selStr = _T("");
	CString colorStr = _T("");
	COLORREF colRef;

	while (pos && (selStr = selList.GetNext(pos))) {
		NthPathSegmentLong(selStr,start,',',0);
		NthPathSegmentLong(selStr,end,',',1);
		NthPathSegment(selStr,colorStr,',',2);
		if (colorStr == _T("red"))
			colRef = RGB(255,0,0);
		else if (colorStr == _T("green"))
//			colRef = RGB(0,150,0);
			colRef = RGB(0,180,0);
		else if (colorStr == _T("dark green"))
//			colRef = RGB(0,80,0);
			colRef = RGB(0,140,0);
		else if (colorStr == _T("purple"))
			colRef = RGB(150,0,150);
		else if (colorStr == _T("dark blue"))
//			colRef = RGB(0,0,150);
			colRef = RGB(0,100,200);
		else if (colorStr == _T("blue"))	// CLARIFY CODE.	// 12/10/09 AM.
//			colRef = RGB(0,0,255);
			colRef = RGB(0,150,250);
		else	// default.	// CLARIFY CODE.	// 12/10/09 AM.
			colRef = RGB(0,0,0);	// 12/10/09 AM.
		//AdjustStartEnd(start,end,processTextStr);
		end++;

		SetFormatRange(start,end,true,colRef);
		i++;
	}

	edit.SetSel(0,0);
	edit.LineScroll(visLine);
	edit.SetSel(selRange);
	edit.SetModify(false);

	UnlockWindowUpdate();
}

void CVisualTextView::FormatTextLines(int nLineStart, int nLineEnd)
{
	CRichEditCtrl &edit = GetRichEditCtrl();
	long nStart = edit.LineIndex(edit.LineFromChar(nLineStart));
	long nEnd = edit.LineIndex(edit.LineFromChar(nLineEnd));
	nEnd += edit.LineLength(nLineEnd);

	FormatTextRange(nStart, nEnd);
}

void CVisualTextView::FormatAll()
{
#ifdef _UNICODE
	FormatTextRange(0,GetTextLength());
#endif
}

void CVisualTextView::ClearFormat()
{
	LockWindowUpdate();
	CRichEditCtrl &edit = GetRichEditCtrl();
	int visLine = edit.GetFirstVisibleLine();

	SetFormatRange(0,GetTextLength(),false,RGB(0,0,0));

	CHARRANGE crCurSel;
	crCurSel.cpMin = 0; 
	crCurSel.cpMax = 0; 
	edit.SetSel(crCurSel);
	edit.LineScroll(visLine);
	edit.SetModify(false);

	UnlockWindowUpdate();
}

void CVisualTextView::WhenChange() 
{
	if (m_bInForcedChange)
		return;

	long selStart,selEnd;
	CRichEditCtrl &edit = GetRichEditCtrl();
	edit.GetSel(selStart,selEnd);

	long lineStart,lineEnd;
	lineStart = edit.LineIndex(-1);
	int lineLength = edit.LineLength(-1);
	lineEnd = lineStart + lineLength;
	FormatTextRange(lineStart,lineEnd);

	/*
	CHARRANGE crCurSel; 
	CRichEditCtrl &edit = GetRichEditCtrl();
	edit.GetSel(crCurSel);

	if (m_changeType == ctMove && crCurSel.cpMin == crCurSel.cpMax) {
		// cut was canceled, so this is paste operation
		m_changeType = ctPaste;
	}

	switch(m_changeType) {
	case ctReplSel:// old=(x,y) -> cur=(x+len,x+len)
	case ctPaste:  // old=(x,y) -> cur=(x+len,x+len)
		FormatTextLines(m_crOldSel.cpMin, crCurSel.cpMax);
		break;
	case ctDelete: // old=(x,y) -> cur=(x,x)
	case ctBack:   // old=(x,y) -> cur=(x,x), newline del => old=(x,x+1) -> cur=(x-1,x-1)
	case ctCut:    // old=(x,y) -> cur=(x,x)
		FormatTextLines(crCurSel.cpMin, crCurSel.cpMax);
		break;
	case ctUndo:   // old=(?,?) -> cur=(x,y)
		FormatTextLines(crCurSel.cpMin, crCurSel.cpMax);
		break;
	case ctMove:   // old=(x,x+len) -> cur=(y-len,y) | cur=(y,y+len)
		FormatTextLines(crCurSel.cpMin, crCurSel.cpMax);
		if (crCurSel.cpMin > m_crOldSel.cpMin) // move after
			FormatTextLines(m_crOldSel.cpMin, m_crOldSel.cpMin);
		else // move before
			FormatTextLines(m_crOldSel.cpMax, m_crOldSel.cpMax);
		break;
	default:
		FormatAll();
		break;
	}

	//undo action does not call OnProtected, so make it default
	m_changeType = ctUndo;
	*/
}

void CVisualTextView::SerializePublic(CArchive& ar)
{
	Serialize(ar);
}

void CVisualTextView::OnSaveAll() 
{
	theApp.DoAllViews(NULL,_T("VisualText"),_T(".pat"),DOC_ACTION_SAVE);
	theApp.DoAllViews(NULL,_T("VisualText"),_T(".txt"),DOC_ACTION_SAVE);
	theApp.DoAllViews(NULL,_T("VisualText"),_T(".htm"),DOC_ACTION_SAVE);  // 07/15/06 AM.
	theApp.DoAllViews(NULL,_T("VisualText"),_T(".html"),DOC_ACTION_SAVE); // 07/15/06 AM.
	theApp.DoAllViews(NULL,_T("VisualText"),_T(".xml"),DOC_ACTION_SAVE);  // 07/15/06 AM.
	theApp.DoAllViews(NULL,_T("VisualText"),_T(".pos"),DOC_ACTION_SAVE);  // 07/15/06 AM.
	appBoolDocDirty = theApp.DirtyDocs(appDocFocus);
	appBoolDocsDirty = theApp.DirtyDocs();
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->OnUpdateFrameTitle(false);
}

void CVisualTextView::FindLocal() 
{
	if (!GetSelTextTrim())
		return;

	if (appStrFind != _T("")) {
		FindForward();  // TO MOVE TO NEXT WORD
	}
}

#if _NOT_USED
/**********
void CVisualTextView::OnRulefileMakephrases() 
{
	if (AskForText("Enter node name:")) {
		CRichEditCtrl &edit = GetRichEditCtrl();
		long start;
		long end;

		edit.GetSel(start,end);
		CString contentStr;
		GetWindowText(contentStr);

		CString selStr = contentStr.Mid(start,end-start+1);
		CStringList wordStrings;
		StringToPhraseList(wordStrings,appAskText,selStr);
		CString words = StringListToLines(wordStrings);
		edit.ReplaceSel(words);
		edit.SetSel(start,start+words.GetLength());
	}	
}

void CVisualTextView::OnTextAddexample() 
{
	TextAddExample();
}

void CVisualTextView::OnRulefileSortwords() 
{
	SortSelection();
}

***********/
#endif

void CVisualTextView::OnDestroy() 
{
	CRichEditView::OnDestroy();
	CVisualTextDoc *doc = GetDocument();
	/*
	COleClientItem* pActiveItem = doc->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL && pActiveItem->GetActiveView() == this) {
		pActiveItem->Deactivate();
		ASSERT(doc->GetInPlaceActiveItem(this) == NULL);
	}
	*/

	appIntOpenWindowCount--;

	if (!appBoolTabbedWindows)
		SaveState();

	if (theApp.GetNumberOfOpenViews() == 1)
		appDocFocus = NULL; // SRP 010505
}

void CVisualTextView::SaveState() 
{
	theApp.AddStatus(_T("docopen"),StateStr());
}

CString CVisualTextView::StateStr() 
{
	CVisualTextDoc *doc = GetDocument();
	CString pathName = doc->GetPathName();
	CRichEditCtrl &edit = GetRichEditCtrl();

	// Don't ask me how I found this.  Took one year!
	WINDOWPLACEMENT wp;
	GetParentFrame()->GetWindowPlacement(&wp);

	int active = 0;
	int scrollPos = edit.GetFirstVisibleLine();
	long start;
	long end;
	edit.GetSel(start,end);

 	CString posStr;
//	posStr.Format(_T("%s%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d"),
	posStr.Format(_T("%s%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d"),	// FIX DUE TO PURIFY // 08/02/07 AM.
//	posStr.Format(_T("%s%c%d%c%d%c%d%c%d"),	// FIX DUE TO PURIFY // 08/02/07 AM.
		RelativizePath(pathName),STATUSSEP,
		wp.rcNormalPosition.top,STATUSSEP,
		wp.rcNormalPosition.bottom,STATUSSEP,
		wp.rcNormalPosition.left,STATUSSEP,
		wp.rcNormalPosition.right,STATUSSEP,
		scrollPos,STATUSSEP,
		active,STATUSSEP,
		start,STATUSSEP,
		end
		);

	return posStr;
}

int CVisualTextView::IndentReturn(int lineOffset)
{
	CRichEditCtrl &edit = GetRichEditCtrl();

	CString lineStr;
	int line = edit.LineFromChar(-1);
	int index = edit.LineIndex(line+lineOffset);
	int indentSize = -1;
	int lineLength;

	if (index >= 0) {
		_TCHAR lineBuff[1001];
		lineLength = edit.GetLine(line,lineBuff,1000);
		lineBuff[lineLength] = '\0';
		CString lineStr = lineBuff;

		LPCTSTR x = lineStr;	// FIX?	// 05/21/07 AM.
		_TCHAR *ptr = (_TCHAR *) x;	// FIX?	// 05/21/07 AM.


//		_TCHAR *ptr = (_TCHAR *) &lineStr;	// OPT. // 11/30/06 AM.
		indentSize = IndentSize(ptr,appIntRuleTabSize);
//		indentSize = IndentSize(lineStr,appIntRuleTabSize);	// The old one.	// 05/21/07 AM.
	}

	return indentSize;
}

void CVisualTextView::OnFindBack() 
{
	if (!GetSelTextTrim(true,false))
		return;

	CRichEditCtrl &edit = GetRichEditCtrl();
	CHARRANGE range;
	edit.GetSel(range);
	long numLines = edit.GetLineCount() - 1;
	//long end = edit.LineIndex(numLines);
	long end = edit.GetTextLength() - 1;
	long len = appStrFind.GetLength();
	long found = -1;

#ifdef UNICODE
	FINDTEXTEXW *textFind = new FINDTEXTEXW;
#else
	FINDTEXTEX *textFind = new FINDTEXTEX;
#endif

	_TCHAR buff[1001];
	_tcscpy_s(buff,(const _TCHAR *)appStrFind);
	buff[appStrFind.GetLength()] = '\0';
	textFind->lpstrText = buff;
	textFind->chrg.cpMin = range.cpMin - 1 - (len * 2);
	textFind->chrg.cpMax = range.cpMin - 1;
	textFind->chrgText = range;

	while (found < 0 && textFind->chrg.cpMin - len > 0L) {
		found = edit.FindText(NULL,textFind);
		textFind->chrg.cpMin -= len;
		textFind->chrg.cpMax -= len;
	}

	if (found < 0) {
		textFind->chrg.cpMin = end - (len * 2);
		textFind->chrg.cpMax = end;
		textFind->chrgText = range;
		while (found < 0 && textFind->chrg.cpMin > 0L) {
			found = edit.FindText(NULL,textFind);
			textFind->chrg.cpMin -= len;
			textFind->chrg.cpMax -= len;
		}
	}

	if (found < 0) {
		CString msgStr;
		msgStr.Format(_T("\"%s\" not found!"),appStrFind);
		AfxMessageBox(msgStr);
		edit.SetSel(range);
	}
	else
		edit.SetSel(textFind->chrgText);

	delete textFind;
}

void CVisualTextView::OnFindForward() 
{
	if (!GetSelTextTrim(true,false))
		return;

	CRichEditCtrl &edit = GetRichEditCtrl();
	CHARRANGE range;
	edit.GetSel(range);
	// SRP 010507
	// The code below for "end" returned the BEGINNING of the last line, not its END.
	// Use -1 to search to the end of the text.
	long numLines = edit.GetLineCount() - 1;
	long end = edit.LineIndex(numLines);

#ifdef UNICODE
			FINDTEXTEXW *textFind = new FINDTEXTEXW;
#else
			FINDTEXTEX *textFind = new FINDTEXTEX;
#endif

	_TCHAR buff[1001];
	StringToChar(buff,appStrFind,1000);
	textFind->lpstrText = buff;
	textFind->chrg.cpMin = range.cpMax + 1;
	textFind->chrg.cpMax = -1;
	textFind->chrgText = range;
	long found = -1;

	found = edit.FindText(FR_DOWN,textFind);

	if (found < 0) {
		textFind->chrg.cpMin = 0L;
		textFind->chrg.cpMax = /* end */ -1; // SRP 010507
		if (edit.FindText(FR_DOWN,textFind) < 0) {
			CString msgStr;
			msgStr.Format(_T("\"%s\" not found!"),appStrFind);
			AfxMessageBox(msgStr);
		}
	}

	edit.SetSel(textFind->chrgText);
	delete textFind;
}

bool CVisualTextView::GetSelTextTrim()
{
	long start;
	long end;
	return GetSelTextTrim(start,end);
}

bool CVisualTextView::GetSelTextTrim(bool trimFlag, bool askFlag)
{
	long start;
	long end;
	return GetSelTextTrim(start,end,trimFlag,askFlag);
}

bool CVisualTextView::GetSelTextTrim(long &start, long &end, bool trimFlag, bool askFlag)
{
	CRichEditCtrl &edit = GetRichEditCtrl();
	CString selStr = EditGetSelText();  // 05/02/21 SRP

	edit.GetSel(start,end);
	end--;  // Adjust for Rich Edit

	//if (adjust) {
		//AdjustStartEnd(start,end);
		//appAskText.Replace("\r\n","\n");
	//}

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

bool CVisualTextView::GetCursorLine(CString &lineStr, int nth) 
{
	CDocument *doc = GetDocument();
	CDocument *logDoc = theApp.ViewLogFile(doc);

	CRichEditCtrl &edit = GetRichEditCtrl();
	_TCHAR lineBuff[1001];
	int lineLength = edit.GetLine(edit.LineFromChar(-1),lineBuff,1000);
	lineBuff[lineLength] = '\0';

	lineStr = lineBuff;
	if (nth >=0) {
		CString newLineStr;
		NthPathSegment(lineStr,newLineStr,' ',nth);
		lineStr = newLineStr;
	}
	if (lineStr == _T(""))
		return false;
	return true;
}

void CVisualTextView::SetAsModified()
{
	CRichEditCtrl &edit = GetRichEditCtrl();
	edit.SetModify();
	CVisualTextDoc *doc = (CVisualTextDoc *)GetDocument();
	doc->SetTitle(doc->GetTitle());
	ProcessFocus();
}

void CVisualTextView::Scroll(int lines)
{
	CRichEditCtrl &edit = GetRichEditCtrl();
	if (edit) {
		edit.LineScroll(lines);
	}
}

void CVisualTextView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	logmsg(_T("CVisualTextView::OnRbuttonDown"));	// 05/14/08 AM.
	ClientToScreen(&point);
	CVisualTextDoc *doc = GetDocument();
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CONCEPT *sampleConcept;
	WORKSPACE_TAB activeTab = wnd->GetActiveTab();

	switch (doc->m_DocType) {
	case DOC_SAMPLE:
		sampleConcept = wnd->GrammarSampleConcept(doc->GetPathName());
		theApp.m_pMainFrame->m_wndGrammarView.SampleViewPopup(this,point,sampleConcept);
		break;
	case DOC_SYS_LOG:
		break;
	case DOC_LOG:
		LogPopup(point);
		break;
	default:
		if (activeTab != WORKSPACE_GRAMMAR)
			TextViewPopup(point);
		else
			theApp.m_pMainFrame->m_wndGrammarView.SamplePopup(this,point);
		break;
	}
}

void CVisualTextView::TextViewPopup(CPoint point)
{
	// TAKE CARE OF GRAYING ITEMS
	UINT disables[100];
	for (int i=0; i<100; i++)
		disables[i] = 0;
	int count = 0;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	CRichEditCtrl &ed = GetRichEditCtrl();
	long start,end;
	ed.GetSel(start,end);
	bool selectionMadeFlag = start == end ? false : true;

	if (appStrFind == _T(""))
		disables[count++] = ID_TEXT_FINDAGAIN;
	if (!GetDocument()->IsModified())
		disables[count++] = ID_FILE_SAVE;
	if (!selectionMadeFlag) {
		disables[count++] = ID_TEXT_DICTIONARY_ADDWORDS;
		disables[count++] = ID_TEXT_DICTIONARY_KBATTRIBUTES;
	}
	if (wnd->SelectedTextFolderEmpty())
		disables[count++] = ID_TEXTVIEW_SELECTANDRUN;

	CalculateIsAnalyzed();
	if (! m_boolIsAnalyzed) {
		disables[count++] = ID_TEXT_GENERATERULE;
		disables[count++] = ID_TEXT_VIEWENTIRETREE;
		disables[count++] = ID_TEXT_VIEWOUTPUT;
		disables[count++] = ID_TEXT_DISPLAYTREE;
	}

	CMenu menu, menuLookup;
	menu.LoadMenu(IDR_TEXTVIEW);
	CMenu *context = menu.GetSubMenu(0);
	int nPos;
	if (FindMenuPosFromString(context, _T("&Dictionary"), nPos)) {
		CMenu* pMenuDict = context->GetSubMenu(nPos);
		CString strLookup = _T("&Lookup");
		if (FindMenuPosFromString(pMenuDict, strLookup, nPos)) {
			CMenu menuLookup;
			CStringArray strNames;
			CPrefLookupPage::GetDictionaryLookupMenu(menuLookup, ID_DICTIONARY_URL_LOOKUP_BASE, strNames, m_strURLs);
			pMenuDict->DeleteMenu(nPos,MF_BYPOSITION);
			pMenuDict->InsertMenu(nPos,MF_ENABLED | MF_BYPOSITION | MF_POPUP | MF_STRING,
				(UINT)menuLookup.GetSafeHmenu(),strLookup);
			menuLookup.Detach();
		}
	}

	context->CheckMenuItem(ID_TEXT_WORDWRAPON, m_wordWrap ? MF_CHECKED : MF_UNCHECKED);

	PopupContextMenu(this,context,point,disables);
}

void CVisualTextView::LogPopup(CPoint point)
{
	// TAKE CARE OF GRAYING ITEMS
	UINT disables[100];
	for (int i=0; i<100; i++)
		disables[i] = 0;
	int count = 0;

	if (appStrFind == _T(""))
		disables[count++] = ID_LOGFILE_FINDAGAIN;

	CString titleStr = GetDocument()->GetTitle();
	if (titleStr.Find(_T("final.log")) >= 0) {
		disables[count++] = ID_LOGFILE_FINDINRULEFILE;
		disables[count++] = ID_LOGFILE_VIEWRULEFILE;
	}

	// DO MENU	
	PopupContextMenu(this,IDR_LOGFILE,point,disables);	
}

void CVisualTextView::FindForward() 
{
	OnFindForward();
}

void CVisualTextView::FindBack() 
{
	OnFindBack();
}

void CVisualTextView::LogFilePathStr(CString &logPathStr, CString &textPathStr)
{
	CVisualTextDoc *doc = GetDocument();
	textPathStr = StripPathReverse(RelativizePath(doc->GetPathName()),0,true);
	CString pathStr = doc->GetPathName() + _T("_log");
	int pass = theApp.m_pMainFrame->m_wndAnalyzerView.SelectedPass();
	logPathStr = OutAnaPath(pathStr,pass);
	if (!FileExists(logPathStr))
		logPathStr = pathStr + _T("\\final.log");
}


void CVisualTextView::TextAddExample(UINT menuID) 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString msgStr = _T("Do you want to add to the ");
	msgStr += theApp.m_pMainFrame->m_wndGrammarView.GetSelection() + _T(" now?");

	if (wnd->GetActiveTab() != WORKSPACE_GRAMMAR) {
		wnd->SelectTab(WORKSPACE_GRAMMAR);
		if (AfxMessageBox(msgStr, MB_YESNO) != IDYES)
			return;
	}

	long start,end;

	if (GetSelTextTrim(start,end)) {
		CVisualTextDoc *doc = GetDocument();
		CString pathStr = doc->GetPathName();
		CONCEPT *sampleConcept = wnd->GrammarSampleConcept(pathStr);
		theApp.m_pMainFrame->m_wndGrammarView.AddConceptSample(appStrFind,pathStr,start,end,menuID,sampleConcept);	
	}
}

void CVisualTextView::OnFileNewAnalyzer() 
{
	theApp.CreateNewAnalyzer();
}

void CVisualTextView::OnFileNew() 
{
	theApp.OnFileNew();
}

void CVisualTextView::OnSampleAdd(UINT idIn) 
{
	int adjust = 3391;  // THIS IS THE LOWORD OF SAMPLE_ID_START
	                    // Makes it 1-numbered for since UINT can't be -1
	int menuIndex = idIn - adjust;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	if (menuIndex <= 2) {
		bool backward = menuIndex == 2 ? true : false;
		wnd->OpenNextText(backward);
	}
	else if (menuIndex == 3)  { // Display Tree
		OnTextDisplaytree();
	}
	//else if (menuIndex == 4)  { // Guess!
		//DoForm();
	//}
	else if (menuIndex == 4)  { // Display Output File
		wnd->OpenTextOutput();
	}
	else
		TextAddExample(menuIndex-5);
}

void CVisualTextView::OnSampleViewAdd(UINT idIn) 
{
	int adjust = 23391;  // THIS IS THE LOWORD OF SAMPLE_ID_START
	                    // Makes it 1-numbered for since UINT can't be -1
	int menuIndex = idIn - adjust;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	//if (menuIndex == 1)  { // Guess!
		//DoForm();
	//}
	/*else*/
	if (menuIndex == 1) {
		OnSamplemenuGeneratesamples();
	}
	else if (menuIndex == 2)  {
		OnSamplemenuSort();
	}
	else
		TextAddExample(menuIndex-3);
}


void CVisualTextView::DoForm() 
{
	long start,end;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	GetSelTextTrim(start,end);
	CMarkSampleDlg dlg;
	CVisualTextDoc *doc = GetDocument();
	CString pathStr = doc->GetPathName();
	CONCEPT *sampleConcept = wnd->GrammarSampleConcept(pathStr);
	dlg.SetInfo(appStrFind,pathStr,sampleConcept,start,end);
	dlg.DoModal();
}

void CVisualTextView::OnRulefileHighlightmatches() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString logPathStr;
	int pass = LogFilePassNumberFromTitle();

	if (wnd->GetLogPath(pass,logPathStr))
		wnd->HighlightMatches(logPathStr,_T("")); // SRP 010418
}

int CVisualTextView::LogFilePassNumberFromTitle(bool logFileFlag)
{
	CStringList parseList;
	CVisualTextDoc *doc = GetDocument();
	CString titleStr = doc->GetTitle();
	int pass = -1;

	if (logFileFlag && ParseLine(parseList,titleStr,_T("ana%d.log ("))) {
		pass = _ttoi((const _TCHAR *)NthStringListItem(parseList,0));
	}
	else if (!logFileFlag && ParseLine(parseList,titleStr,_T("(pass %d)"))) {
		pass = _ttoi((const _TCHAR *)NthStringListItem(parseList,0));
	}
	return pass;
}

void CVisualTextView::SelectText() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CVisualTextDoc *doc = GetDocument();
	wnd->SelectText(doc,false);
}


void CVisualTextView::OnLogfileViewrulefile() 
{
	AfxGetApp()->OpenDocumentFile(RuleFileFromLog());
}

void CVisualTextView::OnFindinfiles() 
{
	long start,end;
	if (!GetSelTextTrim(start,end,false,false))
		return;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->FindWord();
}

void CVisualTextView::OnLogfileFindinrulefile() 
{
	// Does this make sense?  Don't know.  de Hilster 1/28/14
	/*
	CString ruleFilePathStr = RuleFileFromLog();
	CDocument *doc = AfxGetApp()->OpenDocumentFile(ruleFilePathStr);

	if (doc) {
		CSyntaxEditorView *view = (CSyntaxEditorView *)GetFirstView(doc);
		CXTPSyntaxEditCtrl &edit = view->GetEditCtrl();
		edit.Find(appStrFind, false, false, false);
		//view->FindText(appStrFind);
		//view->Scroll(5);
	}
	*/
}

CString CVisualTextView::RuleFileFromLog()
{
	int pass = LogFilePassNumberFromTitle(true);
	CString fileNameStr = theApp.m_pMainFrame->m_wndAnalyzerView.CleanPassNameFromPass(pass);
	return RulesPath(fileNameStr);
}

void CVisualTextView::OnTextGeneraterule() 
{
	long start,end;

	if (GetSelTextTrim(start,end,false,false)) {
		//SelectText(); xxx Dd
		if (appStrFind != _T("")) {
			CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
			if (theApp.m_pMainFrame->m_wndTextView.OpenProcessedFiles(_T("log"),false)) {
				CString ruleStr = theApp.m_pMainFrame->m_wndAnalyzerView.LogFileRule(wnd->GetProcessText(),start,end);
				PasteTextToClipboard(ruleStr);
				theApp.m_pMainFrame->m_wndAnalyzerView.AppendToRuleFile(ruleStr);
			}
			else
				AfxMessageBox(_T("You cannot hand-auto-generate a rule without analyzer log files."));
		}
		else
			AfxMessageBox(_T("You must choose text for hand-auto-generating a rule."));
	}
}

void CVisualTextView::OnTextviewSelectandrun() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->ClearOutput();
	SelectText();
	theApp.RunAnalyzer();	
}

void CVisualTextView::OnTextDisplaytree() 
{
	long start, end;

	if (GetSelTextTrim(start, end, false)) {
		SelectText();
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		wnd->OpenParseTree(start, end);
	}
}

void CVisualTextView::OnWordWrapToggle()
{
	CRichEditCtrl &edit = GetRichEditCtrl();

	CMenu menu;
	menu.LoadMenu(IDR_TEXTVIEW);
	CMenu *context = menu.GetSubMenu(0);

	if (m_wordWrap) {
		edit.SetTargetDevice(NULL, 1);
		m_wordWrap = false;
	}
	else {
		edit.SetTargetDevice(NULL, 0);
		m_wordWrap = true;
	}
}

void CVisualTextView::OnTextViewoutput() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->OpenTextOutput();	
}

void CVisualTextView::OnTextViewentiretree() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString logPathStr,textPathStr;
	LogFilePathStr(logPathStr,textPathStr);
	wnd->OpenParseTree(_T(""),logPathStr,textPathStr);
}

void CVisualTextView::OnSamplemenuGeneratesamples() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CVisualTextDoc *doc = GetDocument();
	CString pathStr = doc->GetPathName();
	doc->OnSaveDocument(pathStr);
	wnd->RegenerateSampleFile(pathStr);	
}

void CVisualTextView::OnSamplemenuSort() 
{
	SortSelection();
}

void CVisualTextView::SortSelection() 
{
	StartWaitCursor();

	CRichEditCtrl &edit = GetRichEditCtrl();
	long start;
	long end;

	edit.GetSel(start,end);
	GetUndoParmsSelText(NULL, m_crPre, true);

	// IF NOTHING SELECTED, SORT ALL
	CString selStr = _T("");
	if (start == end) {
		start = 0;
		end = edit.GetTextLength();
		edit.SetSel(start,end);
	}

	selStr = EditGetSelText();  // 05/02/21 SRP

	CStringList wordStrings;
	StringToWordList(wordStrings,selStr);
	SortStringList(wordStrings);
	RemoveSuccessiveEquals(wordStrings);
	CString words = StringListToLines(wordStrings);

	GetUndoParmsSelText(&m_strPre, m_cr2Pre);

	edit.ReplaceSel(words);
	SetAsModified();

	m_strPost = words;
	GetUndoParmsSelText(NULL, m_cr2Post);
	m_cr2Post.cpMin -= words.GetLength();

	GetUndoParmsSelText(NULL, m_crPost);
	AddToUndoList (new CUndoItem (ReverseReplaceTextAtNewLocOp, ReplaceTextAtNewLocOp, m_strPre, m_strPost, m_crPre, m_crPost, m_cr2Pre, m_cr2Post));

	StopWaitCursor();
}

void CVisualTextView::AdjustStartEnd(long &start, long &end, CString textPathStr)
{
	CRichEditCtrl &edit = GetRichEditCtrl();
	int numExtraChars;

	if (textPathStr != _T("")) {
		AdjustFileStartEnd(start,end,textPathStr);
	}
	else {
		numExtraChars = edit.LineFromChar(start);
		start -= numExtraChars;
		end -= numExtraChars;
	}
}

void CVisualTextView::StringToTokens(CStringList &tokens, CString textStr) 
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

void CVisualTextView::OnMouseMove(UINT nFlags, CPoint point) 
{
	/*
	if (GetFocus() == this)
		DragLabel(point);
	else
		m_boolFirstTime = true;
	*/

	CRichEditView::OnMouseMove(nFlags, point);
}

void CVisualTextView::DragLabel(CPoint point) 
{
	if (m_boolFirstTime)
		GetWindowBitmap();
	else
		SetWindowBitmap();

	CClientDC dc(this);
	int x = point.x;
	int y = point.y - 25;
	CString dragStr = GetDocument()->GetTitle();
	CSize size = dc.GetTextExtent(dragStr);

	// DRAW TEXT
	dc.TextOut(x,y,dragStr);
	CRect textRect(x,y,x+size.cx,y+size.cy);
	dc.DrawEdge(textRect,BDR_RAISEDOUTER | EDGE_BUMP,BF_RECT);

	m_boolFirstTime = false;
}

void CVisualTextView::GetWindowBitmap() 
{
	return;
	CClientDC dc(this);
	UpdateWindow();
	
	m_dragDC.DeleteDC();
	m_dragDC.CreateCompatibleDC(&dc); 
	GetClientRect(m_dragRect);
	CBitmap m_bitmap;
	m_bitmap.CreateCompatibleBitmap(&dc,m_dragRect.Width(),m_dragRect.Height());
	m_dragBitmap = m_dragDC.SelectObject(&m_bitmap);
	m_dragDC.BitBlt(m_dragRect.left,m_dragRect.top,m_dragRect.Width(),m_dragRect.Height(),
		&dc, 0, 0, SRCCOPY);
}

void CVisualTextView::SetWindowBitmap() 
{
	return;
	CClientDC dc(this);
	
	dc.BitBlt(m_dragRect.left,m_dragRect.top,m_dragRect.Width(),m_dragRect.Height(),
		&m_dragDC, 0, 0, SRCCOPY);
}

void CVisualTextView::OnTextSorttext() 
{
	SortSelection();
}

void CVisualTextView::InsertString(CString str) 
{
	GetUndoParmsSelText(&m_strPre, m_crPre, true);

	CRichEditCtrl &edit = GetRichEditCtrl();
	long start;
	long end;

	edit.GetSel(start,end);
	edit.ReplaceSel(str);

	FormatTextRange(start,start+str.GetLength());
	SetAsModified();

	m_strPost = str;
	GetUndoParmsSelText(NULL, m_crPost);
	AddToUndoList (new CUndoItem (ReverseInsertStringOp, InsertStringOp, m_strPre, m_strPost, m_crPre, m_crPost));
}

bool CVisualTextView::GetWorkingText(CString &lineStr, long &lineIndex,
									 long &selStart, long &selEnd, CString alertMsgStr)
{
	CRichEditCtrl &edit = GetRichEditCtrl();
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

void CVisualTextView::OnTextToolsHexviewer() 
{
	CVisualTextDoc *doc = (CVisualTextDoc *)GetDocument();
	CHexEditDlg dlg;
	dlg.SetFileData(doc->GetPathName());

	StartWaitCursor();
	dlg.DoModal();
	StopWaitCursor();
}

void CVisualTextView::OnTextToolsAsciitable() 
{
	CASCIIDlg dlg;
	dlg.DoModal();	
}

void CVisualTextView::OnTextToolsTextfileinfo() 
{
	StartWaitCursor();

	CTextInfoDlg dlg;
	CVisualTextDoc *doc = (CVisualTextDoc *)GetDocument();
	dlg.SetData(doc->GetPathName());
	dlg.DoModal();

	StopWaitCursor();
}

void CVisualTextView::OnTextToolsDictionaryLookup() 
{
    CVisualTextDoc *doc = (CVisualTextDoc *)GetDocument();
    CString strPathRelative = RelativizePath(doc->GetPathName());
	CString strFileName = StripPath(strPathRelative,0,true);
	ClipRight(strFileName,4);
	theApp.RunDictionaryLookup(strPathRelative, NULL, strFileName);
	theApp.m_pMainFrame->m_wndTextView.Refresh();
}

void CVisualTextView::OnTextToolsDictionize() 
{
	CVisualTextDoc *doc = (CVisualTextDoc *)GetDocument();
	theApp.Dictionize(doc->GetPathName());
	theApp.m_pMainFrame->m_wndTextView.Refresh();
}

void CVisualTextView::OnTextToolsMask() 
{
	CVisualTextDoc *doc = (CVisualTextDoc *)GetDocument();
	theApp.MaskText(doc->GetPathName());	
}

void CVisualTextView::OnToolsCharacterviewer() 
{
	CVisualTextDoc *doc = (CVisualTextDoc *)GetDocument();
	CFileCharViewerDlg dlg;

	dlg.SetData(doc->GetPathName());

	StartWaitCursor();
	dlg.DoModal();
	StopWaitCursor();
}

void CVisualTextView::AdjustDocSizeToText(CString fullFilePathStr)
{
	//if (!appBoolDocExists) {

		appIntLastWindowX = (appIntOpenWindowCount - 1) * appIntWindowShiftX;
		appIntLastWindowY = (appIntOpenWindowCount - 1) * appIntWindowShiftY;

		long width,length,fileLength;
		long w,l;
		long tw,tl;
		int minw = 150;
		int minl = 100;

		NLPFileTextWidthLength(fullFilePathStr,width,length,fileLength);
		w = width;
		l = length;

		w += 2; // PAD LONGEST LINE WITH ONE CHARACTER BEFORE AND AFTER

		// FETCH TEXT DIMENSIONS WITHOUT FRAME
		WindowSizeFromTextSize(w,l,tw,tl);
		theApp.SetWindowSize(this,tw,tl,fileLength,true);
	//}
}

void CVisualTextView::WindowSizeFromTextSize(long width, long length,
											 long &wOut, long &lOut) 
{
	wOut = width * m_intCharWidth;
	lOut = length * m_intLineHeight;
}

void CVisualTextView::OnTextEmail() 
{
	CDocument *doc = GetDocument();
	CEmailDlg dlg;
	dlg.SetFileData(doc->GetPathName());
	dlg.DoModal();
}

bool CVisualTextView::ApplyTabToSelection(bool shiftFlag)
{
	CRichEditCtrl &edit = GetRichEditCtrl();
	CString textSelStr = EditGetSelText();  // 05/02/21 SRP
	if (textSelStr == _T(""))
		return false;

	int indentSize = IndentReturn(-1);

	CStringList lines;
	StringToStringList(lines,textSelStr,_T("\n"));

	if (lines.GetCount() < 2)
		return false;

	CVisualTextDoc *doc = GetDocument();
	GetUndoParmsSelText(&m_strPre, m_crPre, true);

	POSITION pos = lines.GetHeadPosition();
	CString newStr = _T("");
	CString lineStr;
	int endAdjust = 0;

	while (pos && (lineStr = lines.GetNext(pos))) {
		if (newStr != _T(""))
			newStr += _T("\n");
		if (shiftFlag) {
			if (lineStr.GetAt(0) == '\t') {
				ClipLeft(lineStr,1);
				endAdjust--;
			}
			else {
				ClipLeft(lineStr,indentSize);
				endAdjust++;
			}
		}
		else
			newStr += _T("\t");
		newStr += lineStr;
	}
	newStr += _T("\n");

	long selStart,selEnd;
	edit.GetSel(selStart,selEnd);
	edit.ReplaceSel(newStr);
//	selEnd += (newStr.GetLength() - textSelStr.GetLength());
	selEnd += newStr.GetLength() - textSelStr.GetLength() + 1; // SRP 010620 for consisency with Undo/Redo
	FormatTextRange(selStart,selEnd);
	edit.SetSel(selStart,selEnd);

	m_strPost = newStr;
	GetUndoParmsSelText(NULL, m_crPost);
	AddToUndoList (new CUndoItem (ReversePasteOp, PasteOp, m_strPre, m_strPost, m_crPre, m_crPost));

	return true;
}

void CVisualTextView::OnTextWraptext() 
{
	CRichEditCtrl &edit = GetRichEditCtrl();
	long selStart,selEnd;
	edit.GetSel(selStart,selEnd);

	GetUndoParmsSelText(NULL, m_crPre, true);

	// IF NOTHING SELECTED, ASSUME THEY WANT TO WRAP ALL
	if (selStart == selEnd)
		edit.SetSel(0,edit.GetTextLength() - 1);

	GetUndoParmsSelText(&m_strPre, m_cr2Pre);

	CString textSelStr = EditGetSelText();  // 05/02/21 SRP

	bool trying = true;
	int maxCol = 50;

	while (trying) {
		trying = AskForText(_T("Enter max column width (10 or greater):"),_T("50"));
		if (IsDigit(appAskText)) {
			maxCol = _ttoi((const _TCHAR *)appAskText);
			if (maxCol >= 10)
				break;
		}
	}
	if (!trying)
		return;

	CString newStr = StringWrap(textSelStr,maxCol);
	edit.ReplaceSel(newStr);
	m_strPost = newStr;
	GetUndoParmsSelText(NULL, m_cr2Post);
	m_cr2Post.cpMin -= newStr.GetLength();

	edit.SetSel(selStart,selEnd);
	SetAsModified();

	GetUndoParmsSelText(NULL, m_crPost);
	AddToUndoList (new CUndoItem (ReverseReplaceTextAtNewLocOp, ReplaceTextAtNewLocOp, m_strPre, m_strPost, m_crPre, m_crPost, m_cr2Pre, m_cr2Post));
}

// SRP 2001-02-01
void CVisualTextView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	BOOL bSetModified = true;
	switch (nChar) {
		case VK_BACK:
			if (0 == m_crBefore.cpMin && 0 == m_crBefore.cpMax)
				bSetModified = false;
			break;

		default:
			GetUndoParmsSelText(&m_strPre, m_crPre, true);
			break;
	}

	CRichEditView::OnChar(nChar, nRepCnt, nFlags);

	WhenChange();

	if (bSetModified) {
		BOOL bCharOK = false;
		SetAsModified();
		CUndoItem* pItem = NULL;
		switch (nChar) {
			case VK_BACK:
				GetUndoParmsSelText(&m_strPost, m_crPost);
				pItem = new CUndoItem (ReverseBackspaceOp, BackspaceOp, m_strPre, m_crPost, m_crPre);
				break;
			case VK_TAB:
				bCharOK = true;
				// fall thru
			default:
				if (_istprint(nChar) || bCharOK) {
					GetUndoParmsSelText(&m_strPost, m_crPost);
					if (m_bInsertMode || m_crPre.cpMin != m_crPre.cpMax || m_bCursorAtEndOfDocument) {
						AdjustUndoParms(&m_strPre, m_crPre);
						pItem = new CUndoItem (ReverseInsertCharOp, InsertCharOp, m_strPre, nChar, m_crPost, m_crPre);
					}
					else {
						pItem = new CUndoItem (ReverseReplaceCharOp, ReplaceCharOp, m_strPre.GetAt(0), nChar, m_crPre, m_crPre);
					}
				}
				break;
		}
		if (pItem)
			AddToUndoList (pItem);
	}
}

void CVisualTextView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	// SRP 010420 Ctrl+F4: Standard Child Window Closing Accelerator Key
	// Added here rather than the accelerator table because accelerator was then working
	// on all windows, including CMainFrame
	// Focus must be in the editview!
	if (VK_F4 == nChar && ISKEYPRESSED(VK_CONTROL)) {
		PostMessage(WM_COMMAND, ID_FILE_CLOSE);
	}

	CRichEditCtrl &edit = GetRichEditCtrl();
	long textLength = edit.GetTextLength() - (edit.GetLineCount() - 1);
	CHARRANGE cr;
	edit.GetSel(cr);
	m_bCursorAtEndOfDocument = !(textLength != cr.cpMin || textLength != cr.cpMax);

	BOOL bDoDelete = !m_bCursorAtEndOfDocument;

	switch (nChar) {
		case VK_INSERT:
			m_bInsertMode = !m_bInsertMode;
			break;
		case VK_DELETE:
			if (bDoDelete)
				GetUndoParmsSelText(&m_strPre, m_crPre, true, VK_DELETE);
			break;
	}

	CRichEditView::OnKeyDown(nChar, nRepCnt, nFlags);

	switch (nChar) {
		case VK_DELETE:
			if (bDoDelete) {
				SetAsModified();
				GetUndoParmsSelText(&m_strPost, m_crPost);
				AddToUndoList (new CUndoItem (ReverseDeleteOp, DeleteOp, m_strPre, m_crPost, m_crPre));
			}
			break;
	}
}

// SRP 010307 Restored WM_KEYDOWN messages to PreTranslateMessage().
// Had moved this code to OnKeyDown(), but it turns out the CR is lost
// by the time it reaches OnKeyDown() in normal operation with this particular MFC class
BOOL CVisualTextView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (WM_KEYDOWN == pMsg->message) {
		CVisualTextDoc *doc = GetDocument();
		CRichEditCtrl &edit = GetRichEditCtrl();
		switch (pMsg->wParam) {

			case VK_BACK:
				GetUndoParmsSelText(&m_strPre, m_crPre, true, VK_BACK);
				edit.GetSel(m_crBefore);
				break;

			case VK_RETURN:
			{
				//if (m_docType == DOC_LOG || m_boolIsSampleFile)
				if (m_docType == DOC_LOG)
					return false;

				bool returnPlease = false;
				//CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
				//CString charStr;
				int indentSize = IndentReturn(-1);
				GetUndoParmsSelText(&m_strPre, m_crPre, true);
				if (indentSize) {
					CString str = _T("\n") + IndentStr(indentSize,appIntRuleTabSize);
					edit.ReplaceSel(str);
					m_strPost = str;
					//wnd->AddDebugLine("PreTrans:","Indenting");
					returnPlease = true;
				}
				else {
					edit.ReplaceSel(_T("\n"));
					m_strPost = _T("\n");
					//wnd->AddDebugLine("PreTrans:","Plain return");
					returnPlease = true;
				}
				//charStr.Format("%d",pMsg->message);
				//wnd->AddDebugLine("PreTrans message:",charStr);
				//charStr.Format("%d",pMsg->wParam);
				//wnd->AddDebugLine("PreTrans param:",charStr);
				if (returnPlease) {
					SetAsModified();
					GetUndoParmsSelText(NULL, m_crPost);
					AddToUndoList (new CUndoItem (ReversePasteOp, PasteOp, m_strPre, m_strPost, m_crPre, m_crPost));
					return true;
				}
				break;
			}

			case VK_TAB:
			{
				if (m_docType == DOC_LOG || m_boolIsSampleFile)
					return false;

				bool bShift = ISKEYPRESSED(VK_SHIFT) ? true : false;
				if (ApplyTabToSelection(bShift)) {
					SetAsModified();
					return true;
				}
				break;
			}

			default:
				break;
		}
	}
	
	return CRichEditView::PreTranslateMessage(pMsg);
}

// SRP 2001-02-01
// Same as OnEditCut key - only happens if there is a selection
void CVisualTextView::OnEditClear() 
{
	if (GetFocus() != this) {
		GetFocus()->SendMessage(WM_CLEAR);
		return;
	}

	GetUndoParmsSelText(&m_strPre, m_crPre, true, VK_DELETE);

	m_bCutOp = true;
	CRichEditView::OnEditClear();
	m_bCutOp = false;

	SetAsModified();
	GetUndoParmsSelText(&m_strPost, m_crPost);
	AddToUndoList (new CUndoItem (ReverseDeleteOp, DeleteOp, m_strPre, m_crPost, m_crPre));
}

// SRP 2001-02-01
void CVisualTextView::OnEditCut() 
{
	if (GetFocus() != this) {
		GetFocus()->SendMessage(WM_CUT);
		return;
	}

	GetUndoParmsSelText(&m_strPre, m_crPre, true, VK_DELETE);

	m_bCutOp = true;
	CRichEditView::OnEditCut();
	m_bCutOp = false;

	SetAsModified();
	GetUndoParmsSelText(&m_strPost, m_crPost);
	AddToUndoList (new CUndoItem (ReverseDeleteOp, DeleteOp, m_strPre, m_crPost, m_crPre));
}

void CVisualTextView::OnEditCopy() 
{
	if (GetFocus() != this) {
		GetFocus()->SendMessage(WM_COPY);
		return;
	}
	m_bCutOp = true;
	CRichEditView::OnEditCopy();
	m_bCutOp = false;
}

// SRP 2001-02-01
void CVisualTextView::OnEditPaste() 
{
	if (GetFocus() != this) {
		GetFocus()->SendMessage(WM_PASTE);
		return;
	}

	GetUndoParmsSelText(&m_strPre, m_crPre, true);
	GetClipboardText(m_strPost);

	LockWindowUpdate();
	CRichEditView::OnEditPaste();

	SetAsModified();
	GetUndoParmsSelText(NULL, m_crPost);
	int finalPosCalc = m_crPre.cpMin + m_strPost.GetLength();
	// If the edit control has added extra white space...
	if (m_crPost.cpMin > finalPosCalc) {
		CRichEditCtrl &edit = GetRichEditCtrl();
		edit.SetSel(finalPosCalc, m_crPost.cpMax);
		CString strTextExtra = EditGetSelText();  // 05/02/21 SRP
		if (!strTextExtra.IsEmpty())
			m_strPost += strTextExtra;
		edit.SetSel(m_crPost);
	}
	BOOL bRedraw = SetFormatRangeFont(m_crPre.cpMin, m_crPost.cpMax);
	FormatTextLines(m_crPre.cpMin, m_crPost.cpMax);
	AddToUndoList (new CUndoItem (ReversePasteOp, PasteOp, m_strPre, m_strPost, m_crPre, m_crPost));
	// KLUDGE
	// If you paste in something with a large type font, SetFormatRangeFont will adjust the font face
	// and the font height, but the line spacing will not be shrunk.  Undo/Redo does this...
	if (bRedraw) {
		OnEditUndo();
		OnEditRedo();
	}
	UnlockWindowUpdate();
}

// SRP 2001-02-01
// SRP 2001-04-19
void CVisualTextView::OnEditUndo()
{
	if (GetFocus() != this) {
		GetFocus()->SendMessage(WM_UNDO);
		return;
	}

	CString *pStr;
	CHARRANGE sel;
	_TCHAR ch;

	SetAsModified();
	--m_IndexUndo;
	CUndoItem *pItem = (CUndoItem *)m_UndoList.GetAt(m_IndexUndo);
	CRichEditCtrl &edit = GetRichEditCtrl();
	LockWindowUpdate();
	switch (pItem->m_opUndo) {
		case ReverseDragOp:
			sel = pItem->m_PUndo.sel;
			sel.cpMax = sel.cpMin;
			edit.  SetSel(sel);
			pStr = pItem->m_PUndo.pStr;
			edit.ReplaceSel(*pStr);
			sel = pItem->m_PUndo.sel;
			edit.SetSel(sel);
			FormatTextLines(sel.cpMin, sel.cpMax);
			break;

		case ReverseDropOp:
			sel = pItem->m_PUndo.sel;
			pStr = pItem->m_PUndo.pStr;
			sel.cpMax += pStr->GetLength();
			edit.SetSel(sel);
			edit.ReplaceSel(_T(""));
			FormatTextLines(sel.cpMin, sel.cpMin);
			break;

		case ReverseReplaceTextAtNewLocOp:
			sel = pItem->m_PRedo.sel2;
			edit.SetSel(sel);
			pStr = pItem->m_PUndo.pStr;
			edit.ReplaceSel(*pStr);
			sel.cpMax += pStr->GetLength();
			FormatTextLines(sel.cpMin, sel.cpMax);

			sel = pItem->m_PUndo.sel;
			edit.SetSel(sel);
			if (pItem->m_PUndo.bLink)
				PostMessage(WM_COMMAND, ID_EDIT_UNDO);
			break;

		case ReverseInsertStringOp:
			sel = pItem->m_PRedo.sel;
			if (sel.cpMin == sel.cpMax) {
				pStr = pItem->m_PRedo.pStr;
				sel.cpMin = sel.cpMax - pStr->GetLength();
			}
			edit.SetSel(sel);

			pStr = pItem->m_PUndo.pStr;
			edit.ReplaceSel(*pStr);

			sel = pItem->m_PUndo.sel;
			edit.SetSel(sel);
			FormatTextLines(sel.cpMin, sel.cpMax);
			break;

		case ReversePasteOp:
			sel = pItem->m_PRedo.sel;
			if (sel.cpMin == sel.cpMax) {
				pStr = pItem->m_PRedo.pStr;
				sel.cpMin = sel.cpMax - pStr->GetLength();
			}
			edit.SetSel(sel);

			pStr = pItem->m_PUndo.pStr;
			edit.ReplaceSel(*pStr);

			sel = pItem->m_PUndo.sel;
			edit.SetSel(sel);
			FormatTextLines(sel.cpMin, sel.cpMax);
			break;

		case ReverseInsertCharOp:
			pStr = pItem->m_PUndo.pStr;
			sel = pItem->m_PUndo.sel;
			--sel.cpMin;
			edit.SetSel(sel);
			edit.ReplaceSel(*pStr);
			edit.GetSel(sel);
			sel.cpMin -= pStr->GetLength();
			edit.SetSel(sel);
			FormatTextLines(sel.cpMin, sel.cpMax);
			break;

		case ReverseReplaceCharOp:
			ch = pItem->m_PUndo.ch;
			sel = pItem->m_PUndo.sel;
			sel.cpMax++;
			edit.SetSel(sel);
			edit.ReplaceSel(CString(ch));
			FormatTextLines(sel.cpMin, sel.cpMax);
			--sel.cpMax;
			edit.SetSel(sel);
			break;

		case ReverseDeleteOp:
			pStr = pItem->m_PUndo.pStr;
			sel = pItem->m_PUndo.sel;
			edit.SetSel(sel);
			edit.ReplaceSel(*pStr);
			edit.SetSel(pItem->m_PRedo.sel);
			sel.cpMax += pStr->GetLength();
			FormatTextLines(sel.cpMin, sel.cpMax);
			break;

		case ReverseBackspaceOp:
			pStr = pItem->m_PUndo.pStr;
			sel = pItem->m_PUndo.sel;
			edit.SetSel(sel);
			edit.ReplaceSel(*pStr);
			edit.SetSel(pItem->m_PRedo.sel);
			FormatTextLines(sel.cpMin, sel.cpMax);
			break;

	}
	UnlockWindowUpdate();
}

// SRP 2001-04-19
void CVisualTextView::OnEditRedo()
{
	CString *pStr;
	_TCHAR ch;
	CHARRANGE sel;

	SetAsModified();
	CUndoItem *pItem = (CUndoItem *)m_UndoList.GetAt(m_IndexUndo);
	m_IndexUndo++;
	CRichEditCtrl &edit = GetRichEditCtrl();
	LockWindowUpdate();
	switch (pItem->m_opRedo) {
		case DragOp:
			sel = pItem->m_PUndo.sel;
			edit.SetSel(sel);
			edit.ReplaceSel(_T(""));
			FormatTextLines(sel.cpMin, sel.cpMin);
			break;

		case DropOp:
			sel = pItem->m_PRedo.sel;
			edit.SetSel(sel);
			pStr = pItem->m_PUndo.pStr;
			edit.ReplaceSel(*pStr);
			sel.cpMax += pStr->GetLength();
			edit.SetSel(sel);
			FormatTextLines(sel.cpMin, sel.cpMax);
			break;

		case ReplaceTextAtNewLocOp:
			sel = pItem->m_PUndo.sel2;
			edit.SetSel(sel);
			pStr = pItem->m_PRedo.pStr;
			edit.ReplaceSel(*pStr);
			sel.cpMax += pStr->GetLength();
			FormatTextLines(sel.cpMin, sel.cpMax);

			sel = pItem->m_PRedo.sel;
			edit.SetSel(sel);
			if (pItem->m_PRedo.bLink)
				PostMessage(WM_COMMAND, ID_EDIT_REDO);
			break;

		case InsertStringOp:
			sel = pItem->m_PUndo.sel;
			edit.SetSel(sel);

			pStr = pItem->m_PRedo.pStr;
			edit.ReplaceSel(*pStr);

			sel = pItem->m_PRedo.sel;
			edit.SetSel(sel);
			sel.cpMin -= pStr->GetLength();
			FormatTextLines(sel.cpMin, sel.cpMax);
			break;

		case PasteOp:
			sel = pItem->m_PUndo.sel;
			edit.SetSel(sel);

			pStr = pItem->m_PRedo.pStr;
			edit.ReplaceSel(*pStr);

			sel = pItem->m_PRedo.sel;
			edit.SetSel(sel);
			sel.cpMin -= pStr->GetLength();
			FormatTextLines(sel.cpMin, sel.cpMax);
			break;

		case InsertCharOp:
			ch = pItem->m_PRedo.ch;
			sel = pItem->m_PRedo.sel;
			edit.SetSel(sel);
			if (sel.cpMin != sel.cpMax)
				edit.ReplaceSel(_T(""));
			edit.ReplaceSel(CString(ch));
			FormatTextLines(sel.cpMin, sel.cpMin);
			break;

		case ReplaceCharOp:
			ch = pItem->m_PRedo.ch;
			sel = pItem->m_PRedo.sel;
			sel.cpMax++;
			edit.SetSel(sel);
			edit.ReplaceSel(CString(ch));
			FormatTextLines(sel.cpMin, sel.cpMin);
			break;

		case DeleteOp:
			sel = pItem->m_PRedo.sel;
			if (sel.cpMin == sel.cpMax)
				sel.cpMax++;
			edit.SetSel(sel);
			edit.ReplaceSel(_T(""));
			FormatTextLines(sel.cpMin, sel.cpMin);
			break;

		case BackspaceOp:
			sel = pItem->m_PRedo.sel;
			if (sel.cpMin == sel.cpMax)
				--sel.cpMin;
			edit.SetSel(sel);
			edit.ReplaceSel(_T(""));
			FormatTextLines(sel.cpMin, sel.cpMin);
			break;

	}
	UnlockWindowUpdate();
}

void CVisualTextView::OnUpdateEditClear(CCmdUI* pCmdUI)
{
	OnUpdateEditCopy(pCmdUI);
}

void CVisualTextView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	CEdit* pEdit= (CEdit*)GetFocus();
	BOOL bEnable = true;
	if ((CEdit*)this == pEdit) {
		int nStartChar, nEndChar;
		pEdit->GetSel(nStartChar, nEndChar);
		bEnable = nStartChar != nEndChar;
	}
	pCmdUI->Enable (bEnable);
}

void CVisualTextView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	OnUpdateEditCopy(pCmdUI);
}

void CVisualTextView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	CEdit* pEdit= (CEdit*)GetFocus();
	BOOL bEnable = true;
	if ((CEdit*)this == pEdit)
#ifdef UNICODE
		bEnable = ::IsClipboardFormatAvailable(CF_UNICODETEXT);
#else
		bEnable = ::IsClipboardFormatAvailable(CF_TEXT);
#endif
	pCmdUI->Enable(bEnable);
}

// SRP 2001-04-19
void CVisualTextView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	CEdit* pEdit= (CEdit*)GetFocus();
	BOOL bEnable = true;
	if ((CEdit*)this == pEdit)
		bEnable = m_IndexUndo > 0;
	pCmdUI->Enable (bEnable);
}

// SRP 2001-04-19
void CVisualTextView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	BOOL bEnable = m_UndoList.GetSize() && m_IndexUndo < m_UndoList.GetSize();
	pCmdUI->Enable (bEnable);
}

void CVisualTextView::OnSetFocus(CWnd* pOldWnd) 
{
	CRichEditView::OnSetFocus(pOldWnd);
	ProcessFocus();
}

void CVisualTextView::ProcessFocus() 
{
	if (appState == VT_APP_LOADING) {
		CRichEditCtrl &edit = GetRichEditCtrl();
		appStrSelText = EditGetSelText();  // 05/02/21 SRP
	}

	appStrFocusDocumentType = _T("VisualText");
	appDocFocus = GetDocument();
	appBoolDocDirty = theApp.DirtyDocs(GetDocument());
	appBoolDocsDirty = theApp.DirtyDocs();
}

void CVisualTextView::OnLogfileFindlocal() 
{
	FindLocal();
}

void CVisualTextView::OnLogfileFindagain() 
{
	FindForward();
}

void CVisualTextView::OnRulefileInsertXnum() 
{
	InsertString(_T("_xNUM"));
}

// SRP 010419
void CVisualTextView::AddToUndoList (CUndoItem *pItemAdd)
{
	CUndoItem *pItem = NULL;
	int i, max;
	max = m_UndoList.GetSize();
	if (max && m_IndexUndo < max) {
		for (i = max - 1; i >= m_IndexUndo; --i) {
			pItem = (CUndoItem *)m_UndoList.GetAt(i);
			if (pItem) {
				delete pItem;
				m_UndoList.RemoveAt(i);
			}
		}
	}
	m_UndoList.Add (pItemAdd);

	if (m_UndoList.GetSize() > MAX_UNDO) {
		if (pItem = (CUndoItem *)m_UndoList.GetAt(0)) {
			delete pItem;
			m_UndoList.RemoveAt(0);
		}
	}

	m_IndexUndo = m_UndoList.GetSize();
}

// SRP 010419
void CVisualTextView::RemoveUndoList (void)
{
	int i, max = m_UndoList.GetSize();
	for (i = 0; i < max; i++) {
		CUndoItem *pItem = (CUndoItem *)m_UndoList.GetAt(i);
		if (pItem) delete pItem;
	}
	m_UndoList.RemoveAll();
	m_IndexUndo = 0;
}

void CVisualTextView::AdjustUndoParms(CString* pStr, CHARRANGE &cr)
{
	// In special cases where a selection include the very end of a line, and therefore a CR,
	// the edit control doesn't delete the CR when doing a replace
	if (pStr) {
		int len = pStr->GetLength();
		if (len > 0) {
			switch (pStr->GetAt(len - 1)) {
				case 0xa:
				case 0xd:
					pStr->SetAt(len-1,0);
					--cr.cpMax;
					break;
			}
		}
	}
}

void CVisualTextView::GetUndoParmsSelText(CString* pStr, CHARRANGE &cr, BOOL bPreAction, _TCHAR nChar)
{
	CRichEditCtrl &edit = GetRichEditCtrl();
	edit.GetSel(cr);
	BOOL bGetReplaceChar = bPreAction && !m_bInsertMode && cr.cpMin == cr.cpMax && !m_bCursorAtEndOfDocument;
	BOOL bDeleteKey = bPreAction && nChar == VK_DELETE && cr.cpMin == cr.cpMax;
	BOOL bBackspaceKey = bPreAction && nChar == VK_BACK && cr.cpMin == cr.cpMax;
	if (bBackspaceKey) {
		--cr.cpMin;
		edit.SetSel(cr);
	}
	else if (bDeleteKey) {
		cr.cpMax++;
		edit.SetSel(cr);
	}
	else if (bGetReplaceChar) {
		cr.cpMax++;
		edit.SetSel(cr);
	}

	if (pStr)
		*pStr = EditGetSelText();  // 05/02/21 SRP

	if (bBackspaceKey) {
		cr.cpMin++;
		edit.SetSel(cr);
	}
	else if (bDeleteKey) {
		--cr.cpMax;
		edit.SetSel(cr);
	}
	else if (bGetReplaceChar) {
		// The following code compensates for the following behavior of the RichEditCtrl:
		// When in OverStrike mode and the cursor is at the end of a line, rather than replace
		// the LF/CR, the edit control lengthens the current line.
		// Therefore Undo must make sure not to add back a CR.
		// NOTE: str.Empty() will not do because str.GetAt(0) will crash.
		if (pStr && pStr->GetLength() > 0) { // SRP 011114
			switch (pStr->GetAt(0)) {
				case 0xa:
				case 0xd:
					pStr->SetAt(0,0);
					break;
			}
			--cr.cpMax; // SRP 011114: Moved within scope of IF statement
		}
		edit.SetSel(cr);
	}
#if 0 // REMOVED because we are now handling double-clicking ourselves...
	// This code to RESET the selection is necessary in case the selection was made by double-clicking.
	// Selecting by double-clicking also selects intraword spacing following the selected word.
	// When a character is then typed to replace the selection, the edit control adds back an intraword
	// space.  There seems to be no switch available to stop this behavior.
	// However, if there is any further change to the selection, either via the keyboard or programmatically
	// (by the use of the SetSel() function), then this behavior of adding back an intraword space
	// does not happen - that is, the entire selection is cut as expected.
	// This normal behavior is crucial for implementation of the undo/redo code.
	if (bPreAction && cr.cpMin != cr.cpMax)
		edit.SetSel(cr);
#endif
}


///////////////////////////////////////////////////////////////////
// CVisualTextView printing
/////////////////////////////////////////////////////////////////////////////

void CVisualTextView::OnFilePrint() 
{
	CRichEditView::OnFilePrint();
}

void CVisualTextView::OnFilePrintPreview() 
{
	CRichEditView::OnFilePrintPreview();
}

void CVisualTextView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	ASSERT(pInfo != NULL);  // overriding OnPaint -- never get this.

	pDC->SetMapMode(MM_TEXT);

	if (pInfo->m_nCurPage > (UINT)m_aPageStart.GetSize() &&
		!PaginateTo(pDC, pInfo))
	{
		// can't paginate to that page, thus cannot print it.
		pInfo->m_bContinuePrinting = FALSE;
	}
	ASSERT_VALID(this);
}

BOOL CVisualTextView::PaginateTo(CDC* pDC, CPrintInfo* pInfo)
	// attempts pagination to pInfo->m_nCurPage, TRUE == success
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	// SRP 010508
	if (pInfo->m_nCurPage > pInfo->GetMaxPage())
		return false;

	CRect rectSave = pInfo->m_rectDraw;
	UINT nPageSave = pInfo->m_nCurPage;
	ASSERT(nPageSave > 1);
	ASSERT(nPageSave >= (UINT)m_aPageStart.GetSize());
	VERIFY(pDC->SaveDC() != 0);
	pDC->IntersectClipRect(0, 0, 0, 0);
	pInfo->m_nCurPage = m_aPageStart.GetSize();
	while (pInfo->m_nCurPage < nPageSave)
	{
		ASSERT(pInfo->m_nCurPage == (UINT)m_aPageStart.GetSize());
		OnPrepareDC(pDC, pInfo);
		ASSERT(pInfo->m_bContinuePrinting);
		pInfo->m_rectDraw.SetRect(0, 0,
			pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
		pDC->DPtoLP(&pInfo->m_rectDraw);
		OnPrint(pDC, pInfo);
		if (pInfo->m_nCurPage == (UINT)m_aPageStart.GetSize())
			break;
		++pInfo->m_nCurPage;
	}
	BOOL bResult = pInfo->m_nCurPage == nPageSave;
	pDC->RestoreDC(-1);
	pInfo->m_nCurPage = nPageSave;
	pInfo->m_rectDraw = rectSave;
	ASSERT_VALID(this);
	return bResult;
}

void CVisualTextView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	ASSERT(pInfo != NULL);
	ASSERT(pInfo->m_bContinuePrinting);

	UINT nPage = pInfo->m_nCurPage;
	ASSERT(nPage <= (UINT)m_aPageStart.GetSize());
	long nIndex = (long) m_aPageStart[nPage-1];

	// print as much as possible in the current page.
	nIndex = PrintPage(pDC, nIndex, 0xFFFFFFFF);

	// SRP 010502
	// The following adjustment is ONLY necessary because text is being stripped of '\r' during initial load.
	// Native format of RichEditCtrl uses "\r\n".
	CRichEditCtrl &edit = GetRichEditCtrl();
	CHARRANGE crSave;
	edit.GetSel(crSave);
	edit.SetSel(0,-1);
	CString str = EditGetSelText();  // 05/02/21 SRP
	edit.SetSel(crSave);
	int len = str.GetLength();
	int nCR = 0;
	for (int n = 0; n < len; n++) {
		if ('\r' == str.GetAt(n))
			nCR++;
	}
	int nLineCount = GetRichEditCtrl().GetLineCount();
	int nTextLength = GetTextLength();
	int nMaxChar = nTextLength - (nLineCount - nCR) + 2;
	if (nIndex >= nMaxChar)
	{
		TRACE0("End of Document\n");
		pInfo->SetMaxPage(nPage);
	}

	// update pagination information for page just printed
	if (nPage == (UINT)m_aPageStart.GetSize())
	{
		if (nIndex < nMaxChar)
			m_aPageStart.Add(nIndex);
	}
	else
	{
		ASSERT(nPage+1 <= (UINT)m_aPageStart.GetSize());
		ASSERT(nIndex == (long)m_aPageStart[nPage+1-1]);
	}
	//	CRichEditView::OnPrint(pDC, pInfo);
}

long CVisualTextView::PrintPage(CDC* pDC, long nIndexStart, long nIndexStop)
	// worker function for laying out text in a rectangle.
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	FORMATRANGE fr;

	// offset by printing offset
	pDC->SetViewportOrg(-pDC->GetDeviceCaps(PHYSICALOFFSETX),
		-pDC->GetDeviceCaps(PHYSICALOFFSETY));
	// adjust DC because richedit doesn't do things like MFC
	if (::GetDeviceCaps(pDC->m_hDC, TECHNOLOGY) != DT_METAFILE && pDC->m_hAttribDC != NULL)
	{
		::ScaleWindowExtEx(pDC->m_hDC,
			::GetDeviceCaps(pDC->m_hDC, LOGPIXELSX),
			::GetDeviceCaps(pDC->m_hAttribDC, LOGPIXELSX),
			::GetDeviceCaps(pDC->m_hDC, LOGPIXELSY),
			::GetDeviceCaps(pDC->m_hAttribDC, LOGPIXELSY), NULL);
	}

	fr.hdcTarget = pDC->m_hAttribDC;
	fr.hdc = pDC->m_hDC;
	fr.rcPage = GetPageRect();
	fr.rc = GetPrintRect();
#define EIGHTH_INCH 180
#define INCH 1440
	// SRP 010506 Adjustment based on AM's and SP's machines.
	fr.rc.bottom -= INCH + (2 * EIGHTH_INCH) + theApp.m_MinMarginY;
	fr.rc.right += EIGHTH_INCH + theApp.m_MinMarginX;

	fr.chrg.cpMin = nIndexStart;
	fr.chrg.cpMax = nIndexStop;
	long lRes = GetRichEditCtrl().FormatRange(&fr,TRUE);
	return lRes;
}

BOOL CVisualTextView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	return DoPreparePrinting(pInfo);
}

//////////////////////////////////////////////////
// Replace
//////////////////////////////////////////////////
BOOL CVisualTextView::SameAsSelected(LPCTSTR lpszCompare, BOOL bCase, BOOL /*bWord*/)
{
	// check length first
	size_t nLen = lstrlen(lpszCompare);
	long lStartChar, lEndChar;
	GetRichEditCtrl().GetSel(lStartChar, lEndChar);
	if (nLen != (size_t)(lEndChar - lStartChar))
		return FALSE;

	// length is the same, check contents
	CString strSelect = EditGetSelText();
	return (bCase && lstrcmp(lpszCompare, strSelect) == 0) ||
		(!bCase && lstrcmpi(lpszCompare, strSelect) == 0);
}

void CVisualTextView::OnReplaceSel(CFindReplaceDialog *pDialog)
{
	ASSERT_VALID(this);

	LPCTSTR lpszFind= pDialog->GetFindString();
	BOOL bNext = pDialog->SearchDown();
	BOOL bCase = pDialog->MatchCase();
	BOOL bWord = pDialog->MatchWholeWord();
	LPCTSTR lpszReplace = pDialog->GetReplaceString();

	if (!SameAsSelected(lpszFind, bCase, bWord))
	{
		if (!FindText(lpszFind,bCase,bWord,bNext))
			TextNotFound(lpszFind);
		else
			AdjustDialogPosition(pDialog);
		return;
	}

	GetUndoParmsSelText(&m_strPre, m_crPre, true);
	GetRichEditCtrl().ReplaceSel(lpszReplace);
	m_strPost = lpszReplace;
	GetUndoParmsSelText(NULL, m_crPost);
	AddToUndoList (new CUndoItem (ReversePasteOp, PasteOp, m_strPre, m_strPost, m_crPre, m_crPost));
	SetAsModified();
	if (!FindText(lpszFind,bCase,bWord,bNext))
		TextNotFound(lpszFind);
	else
		AdjustDialogPosition(pDialog);
	ASSERT_VALID(this);
}

void CVisualTextView::OnReplaceAll(CFindReplaceDialog *pDialog)
{
	ASSERT_VALID(this);

	LPCTSTR lpszFind= pDialog->GetFindString();
	BOOL bNext = pDialog->SearchDown();
	BOOL bCase = pDialog->MatchCase();
	BOOL bWord = pDialog->MatchWholeWord();
	LPCTSTR lpszReplace = pDialog->GetReplaceString();

	CWaitCursor wait;
	// no selection or different than what looking for
	if (!SameAsSelected(lpszFind, bCase, bWord))
	{
		if (!FindText(lpszFind,bCase,bWord))
		{
			TextNotFound(lpszFind);
			return;
		}
	}

	GetRichEditCtrl().HideSelection(TRUE, FALSE);
	do
	{
		GetUndoParmsSelText(&m_strPre, m_crPre, true);
		GetRichEditCtrl().ReplaceSel(lpszReplace);
		m_strPost = lpszReplace;
		GetUndoParmsSelText(NULL, m_crPost);
		AddToUndoList (new CUndoItem (ReversePasteOp, PasteOp, m_strPre, m_strPost, m_crPre, m_crPost));
		SetAsModified();
	} while (FindTextSimple(lpszFind,bCase,bWord));
	TextNotFound(lpszFind);
	GetRichEditCtrl().HideSelection(FALSE, FALSE);

	ASSERT_VALID(this);
}

LRESULT CVisualTextView::OnFindReplaceCmd(WPARAM, LPARAM lParam)
{
	ASSERT_VALID(this);
	CFindReplaceDialog* pDialog = CFindReplaceDialog::GetNotifier(lParam);
	ASSERT(pDialog != NULL);

	if (pDialog->IsTerminating())
		;
	else if (pDialog->FindNext())
	{
		OnFindNext(pDialog->GetFindString(), pDialog->SearchDown(),
			pDialog->MatchCase(), pDialog->MatchWholeWord());
	}
	else if (pDialog->ReplaceCurrent())
	{
		//ASSERT(!pEditState->bFindOnly);
		OnReplaceSel(pDialog);
	}
	else if (pDialog->ReplaceAll())
	{
		//ASSERT(!pEditState->bFindOnly);
		OnReplaceAll(pDialog);
	}
	ASSERT_VALID(this);
	return 0;
}

void CVisualTextView::OnRulefileHelponselection() 
{
	CString selStr = _T("");
	CRichEditCtrl &edit = GetRichEditCtrl();
	CHARRANGE cr;
	edit.GetSel(cr);
	if (cr.cpMin == cr.cpMax) {
		CHARRANGE crSave = cr;
		int selStart, selEnd;
		if (GetSelFromCursor(selStart, selEnd)) {
			LockWindowUpdate();
			edit.SetSel(selStart, selEnd);
			selStr = EditGetSelText();  // 05/02/21 SRP
			edit.SetSel(crSave);
			UnlockWindowUpdate();
		}
	}
	if (selStr.IsEmpty()) {
		if (!GetSelTextTrim(true,true))
			return;
		else
			selStr = appStrFind;
	}
	HelpTopic(selStr);	
}

BOOL VTVWordChar(_TCHAR ch)
{
	switch (ch) {
		case '_':
		case '@':
			return true;
		default:
			return _istalnum(ch);
	}
}

BOOL CVisualTextView::GetSelFromCursor(int& selStart, int& selEnd)
{
	CRichEditCtrl &edit = GetRichEditCtrl();
	long nLineStart, nLineEnd;
	edit.GetSel(nLineStart, nLineEnd);
	if (nLineStart != nLineEnd)
		return false;

	LockWindowUpdate();

	long nStart = edit.LineIndex(edit.LineFromChar(nLineStart));
	long nEnd = nStart + edit.LineLength(nLineStart);
	CHARRANGE crSave;
	edit.GetSel(crSave);
	edit.SetSel(nStart, nEnd);
	CString strText = EditGetSelText();  // 05/02/21 SRP
	edit.SetSel(crSave);

	UnlockWindowUpdate();

	int startParse = nLineStart - nStart;
	int len = strText.GetLength();
	if (startParse < len && !VTVWordChar(strText.GetAt(startParse)) ) {
		if (startParse - 1 >= 0 && VTVWordChar(strText.GetAt(startParse - 1)) ) {
			--startParse;
		}
		else {
			startParse = -1;
		}
	}
	if (nLineStart >= nEnd || -1 == startParse) {
		selStart = nLineStart;
		selEnd = nLineEnd;
		return true;
	}
	int s = 0;
	int e = 0;

	for (s = startParse; s > 0; --s) {
		if (!VTVWordChar(strText.GetAt(s - 1)) )
			break;
	}
	for (e = startParse; e < len - 1; e++) {
		if (!VTVWordChar(strText.GetAt(e + 1)) )
			break;
	}
	e++;
	selStart = s + nStart;
	selEnd = e + nStart;
	return true;
}

void CVisualTextView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CRichEditCtrl &edit = GetRichEditCtrl();
	long nLineStart, nLineEnd;
	edit.GetSel(nLineStart, nLineEnd);
	if (nLineStart == nLineEnd) {
		int selStart, selEnd;
		GetSelFromCursor(selStart, selEnd);
		edit.SetSel(selStart, selEnd);
	}
	//	CRichEditView::OnLButtonDblClk(nFlags, point);
}

///////////////////////////////////////
// All of this subclassing provides a way to undo/redo drag/drop cut-and-paste

BEGIN_INTERFACE_MAP(CVisualTextView, CCtrlView)
	// we use IID_IUnknown because richedit doesn't define an IID
	INTERFACE_PART(CVisualTextView, IID_IUnknown, RichEditOleCallback)
END_INTERFACE_MAP()

STDMETHODIMP_(ULONG) CVisualTextView::XRichEditOleCallback::AddRef()
{
	METHOD_PROLOGUE_EX_(CVisualTextView, RichEditOleCallback)
	return (ULONG)pThis->InternalAddRef();
}

STDMETHODIMP_(ULONG) CVisualTextView::XRichEditOleCallback::Release()
{
	METHOD_PROLOGUE_EX_(CVisualTextView, RichEditOleCallback)
	return (ULONG)pThis->InternalRelease();
}

STDMETHODIMP CVisualTextView::XRichEditOleCallback::QueryInterface(
	REFIID iid, LPVOID* ppvObj)
{
	METHOD_PROLOGUE_EX_(CVisualTextView, RichEditOleCallback)
	return (HRESULT)pThis->InternalQueryInterface(&iid, ppvObj);
}

STDMETHODIMP CVisualTextView::XRichEditOleCallback::GetNewStorage(LPSTORAGE* ppstg)
{
	METHOD_PROLOGUE_EX_(CVisualTextView, RichEditOleCallback)

	// Create a flat storage and steal it from the client item
	// the client item is only used for creating the storage
	COleClientItem item;
	item.GetItemStorageFlat();
	*ppstg = item.m_lpStorage;
	HRESULT hRes = E_OUTOFMEMORY;
	if (item.m_lpStorage != NULL)
	{
		item.m_lpStorage = NULL;
		hRes = S_OK;
	}
	//pThis->GetDocument()->InvalidateObjectCache();
	return hRes;
}

STDMETHODIMP CVisualTextView::XRichEditOleCallback::GetInPlaceContext(
	LPOLEINPLACEFRAME* lplpFrame, LPOLEINPLACEUIWINDOW* lplpDoc,
	LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	METHOD_PROLOGUE_EX(CVisualTextView, RichEditOleCallback)
	return pThis->GetWindowContext(lplpFrame, lplpDoc, lpFrameInfo);
}

STDMETHODIMP CVisualTextView::XRichEditOleCallback::ShowContainerUI(BOOL fShow)
{
	METHOD_PROLOGUE_EX(CVisualTextView, RichEditOleCallback)
	return pThis->ShowContainerUI(fShow);
}

STDMETHODIMP CVisualTextView::XRichEditOleCallback::QueryInsertObject(
	LPCLSID /*lpclsid*/, LPSTORAGE /*pstg*/, LONG /*cp*/)
{
	METHOD_PROLOGUE_EX(CVisualTextView, RichEditOleCallback)
	//pThis->GetDocument()->InvalidateObjectCache();
	return S_OK;
}

STDMETHODIMP CVisualTextView::XRichEditOleCallback::DeleteObject(LPOLEOBJECT /*lpoleobj*/)
{
	METHOD_PROLOGUE_EX_(CVisualTextView, RichEditOleCallback)
	//pThis->GetDocument()->InvalidateObjectCache();
	return S_OK;
}

STDMETHODIMP CVisualTextView::XRichEditOleCallback::QueryAcceptData(
	LPDATAOBJECT pDataObject, CLIPFORMAT* lpcfFormat, DWORD reco,
	BOOL fReally, HGLOBAL hMetaPict)
{
	METHOD_PROLOGUE_EX(CVisualTextView, RichEditOleCallback)

	pThis->GetUndoParmsSelText(&pThis->m_strPost, pThis->m_crPost, true);
	BOOL bDoUndo = reco == RECO_DROP && pThis->m_strPost.IsEmpty();
	if (bDoUndo) {
		CString strDrop = _T("");
		STGMEDIUM stg;
		FORMATETC formatEtc;
#ifdef UNICODE
		formatEtc.cfFormat = CF_UNICODETEXT;
#else
		formatEtc.cfFormat = CF_TEXT;
#endif
		formatEtc.ptd = NULL;
		formatEtc.dwAspect = DVASPECT_CONTENT;
		formatEtc.lindex = -1;
		formatEtc.tymed = TYMED_HGLOBAL;
		HRESULT hRes = pDataObject->GetData(&formatEtc, &stg);
		if (SUCCEEDED(hRes)) {
			HGLOBAL hData = stg.hGlobal;
			if (hData) {
				LPTSTR lp = (LPTSTR)GlobalLock(hData);
				if (lp) {
					strDrop = lp;
					GlobalUnlock(hData);
				}
			}
//	ULF_Out("QAD-DROP");
//	ULF_Out("strDrop=|%s| m_crPost.cpMin=%d m_crPost.cpMax=%d", strDrop, pThis->m_crPost.cpMin, pThis->m_crPost.cpMax);
//			pThis->SetFormatRangeFont(pThis->m_crPost.cpMin, pThis->m_crPost.cpMin + strDrop.GetLength());
//			pThis->FormatTextLines(pThis->m_crPost.cpMin, pThis->m_crPost.cpMin + strDrop.GetLength());
			pThis->AddToUndoList (new CUndoItem (ReverseDropOp, DropOp, strDrop, pThis->m_crPost, pThis->m_crPost));
			pThis->SetAsModified();
		}
	}

	return pThis->QueryAcceptData(pDataObject, lpcfFormat, reco,
		fReally, hMetaPict);
}

STDMETHODIMP CVisualTextView::XRichEditOleCallback::ContextSensitiveHelp(BOOL /*fEnterMode*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CVisualTextView::XRichEditOleCallback::GetClipboardData(
	CHARRANGE* lpchrg, DWORD reco, LPDATAOBJECT* lplpdataobj)
{
	METHOD_PROLOGUE_EX(CVisualTextView, RichEditOleCallback)
	LPDATAOBJECT lpOrigDataObject = NULL;

	// get richedit's data object
	if (FAILED(pThis->m_lpRichEditOle->GetClipboardData(lpchrg, reco,
		&lpOrigDataObject)))
	{
		return E_NOTIMPL;
	}

	// allow changes
	HRESULT hRes = pThis->GetClipboardData(lpchrg, reco, lpOrigDataObject,
		lplpdataobj);

	// if changed then free original object
	if (SUCCEEDED(hRes))
	{
		if (lpOrigDataObject!=NULL)
			lpOrigDataObject->Release();
	}
	else
	{
		// use richedit's data object
		*lplpdataobj = lpOrigDataObject;
		hRes = S_OK;
	}

	if (!pThis->m_bCutOp) {
		CString strDrag = _T("");
		STGMEDIUM stg;
		FORMATETC formatEtc;
#ifdef UNICODE
		formatEtc.cfFormat = CF_UNICODETEXT;
#else
		formatEtc.cfFormat = CF_TEXT;
#endif
		formatEtc.ptd = NULL;
		formatEtc.dwAspect = DVASPECT_CONTENT;
		formatEtc.lindex = -1;
		formatEtc.tymed = TYMED_HGLOBAL;
		HRESULT hRes = (*lplpdataobj)->GetData(&formatEtc, &stg);
		if (SUCCEEDED(hRes)) {
			HGLOBAL hData = stg.hGlobal;
			if (hData) {
				LPTSTR lp = (LPTSTR)GlobalLock(hData);
				if (lp) {
					strDrag = lp;
					GlobalUnlock(hData);
				}
			}
		}
//		pThis->GetUndoParmsSelText(&pThis->m_strPre, pThis->m_crPre, true, VK_DELETE);
//		pThis->AddToUndoList (new CUndoItem (ReverseDragOp, DragOp, pThis->m_strPre, pThis->m_crPre, pThis->m_crPre));
		pThis->SetAsModified();
		pThis->AddToUndoList (new CUndoItem (ReverseDragOp, DragOp, strDrag, *lpchrg, *lpchrg));
//ULF_Out("strCut=|%s| lpchrg(min=%d,max=%d)", strDrag, lpchrg->cpMin, lpchrg->cpMax);
	}

	return hRes;
}

STDMETHODIMP CVisualTextView::XRichEditOleCallback::GetDragDropEffect(
	BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	if (!fDrag) // allowable dest effects
	{
		DWORD dwEffect;
		// check for force link
		if ((grfKeyState & (MK_CONTROL|MK_SHIFT)) == (MK_CONTROL|MK_SHIFT))
			dwEffect = DROPEFFECT_LINK;
		// check for force copy
		else if ((grfKeyState & MK_CONTROL) == MK_CONTROL)
			dwEffect = DROPEFFECT_COPY;
		// check for force move
		else if ((grfKeyState & MK_ALT) == MK_ALT)
			dwEffect = DROPEFFECT_MOVE;
		// default -- recommended action is move
		else
			dwEffect = DROPEFFECT_MOVE;
		if (dwEffect & *pdwEffect) // make sure allowed type
			*pdwEffect = dwEffect;
	}
	return S_OK;
}

STDMETHODIMP CVisualTextView::XRichEditOleCallback::GetContextMenu(
	WORD seltype, LPOLEOBJECT lpoleobj, CHARRANGE* lpchrg,
	HMENU* lphmenu)
{
	METHOD_PROLOGUE_EX(CVisualTextView, RichEditOleCallback)
	HMENU hMenu = pThis->GetContextMenu(seltype, lpoleobj, lpchrg);
	if (hMenu == NULL)
		return E_NOTIMPL;
	*lphmenu = hMenu;
	return S_OK;
}

int CVisualTextView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCtrlView::OnCreate(lpCreateStruct) != 0)
		return -1;
	GetRichEditCtrl().LimitText(lMaxSize);
	GetRichEditCtrl().SetEventMask(ENM_SELCHANGE | ENM_CHANGE | ENM_SCROLL);
	VERIFY(GetRichEditCtrl().SetOLECallback(&m_xRichEditOleCallback));
	m_lpRichEditOle = GetRichEditCtrl().GetIRichEditOle();
	DragAcceptFiles();
	GetRichEditCtrl().SetOptions(ECOOP_OR, ECO_AUTOWORDSELECTION);
	WrapChanged();
	ASSERT(m_lpRichEditOle != NULL);
	return 0;
}

void CVisualTextView::OnTextDictionaryKbattributes() 
{
	if (!GetSelTextTrim())
		return;
	theApp.FindKBAttribute(appStrFind);
}

void CVisualTextView::OnTextDictionaryAddwords() 
{
	if (!GetSelTextTrim())
		return;
	theApp.AddWordToKBSystemDictionary(appStrFind);
}

void CVisualTextView::OnDictionaryURLLookup(UINT uID)
{
	UINT idx = uID - ID_DICTIONARY_URL_LOOKUP_BASE;
	if (!GetSelTextTrim())
		return;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->OpenBrowserDictLookup(m_strURLs[idx],appStrFind);
}

void CVisualTextView::OnTextToolsJumble() 
{
	CVisualTextDoc *doc = (CVisualTextDoc *)GetDocument();
	theApp.JumbleText(doc->GetPathName());	
}

void CVisualTextView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CRichEditCtrl &edit = GetRichEditCtrl();
	appStrSelText = EditGetSelText();  // 05/02/21 SRP
	
	CRichEditView::OnLButtonUp(nFlags, point);
}

void CVisualTextView::OnFonts() 
{
#ifdef old1_
	CFontDialog dlg;
	if (IDOK == dlg.DoModal()) {
		m_font.DeleteObject();
		m_font.CreateFontIndirect(dlg.m_cf.lpLogFont);
		GetRichEditCtrl().SetFont(&m_font);

	// !!! To fix NT redraw problem.	// 02/03/03 AM (from SRP).
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
#endif

#ifdef old2_
	CFontDialog dlg;
	if (IDOK == dlg.DoModal()) {
		m_font.DeleteObject();
		m_font.CreateFontIndirect(dlg.m_cf.lpLogFont);
		GetRichEditCtrl().SetFont(&m_font);

		GetRichEditCtrl().RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
#endif

#ifdef old3_
	CFontDialog dlg;
	if (IDOK == dlg.DoModal()) {
		m_font.DeleteObject();
		m_font.CreateFontIndirect(dlg.m_cf.lpLogFont);
		GetRichEditCtrl().SetFont(&m_font);

		CHARRANGE crOldSel;
		GetRichEditCtrl().GetSel(crOldSel);
		GetRichEditCtrl().SetSel(crOldSel);
		GetRichEditCtrl().SetModify(true);

		// Try without, then with the following line...
//		GetEditCtrl().RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
#endif
#ifdef old4_
	CFontDialog dlg;
	if (IDOK == dlg.DoModal()) {
		 m_font.DeleteObject();
		 m_font.CreateFontIndirect(dlg.m_cf.lpLogFont);
		 GetRichEditCtrl().SetFont(&m_font);

		 LockWindowUpdate();

		 CHARRANGE crOldSel;
		 GetRichEditCtrl().GetSel(crOldSel);
		 GetRichEditCtrl().SetSel(0, -1); // Full selection
		 GetRichEditCtrl().SetSel(crOldSel);
		 GetRichEditCtrl().SetModify(true);

		 UnlockWindowUpdate();

		 // Try without, then with the following line...
		 GetRichEditCtrl().RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
#endif

   LOGFONT lf;
    CFontDialog dlg;
    dlg.m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
    dlg.m_cf.lpLogFont = &lf;
    if (m_font.m_hObject) {
        m_font.GetLogFont(&lf);
    }
    else {
        appFontFixed.GetObject(sizeof(lf), &lf);
    }
    if (IDOK == dlg.DoModal()) {
        m_font.DeleteObject();
        m_font.CreateFontIndirect(dlg.m_cf.lpLogFont);

        CClientDC dc(this);
        m_strFontFaceName = dlg.m_cf.lpLogFont->lfFaceName;
        m_FontHeight = -MulDiv(dlg.m_cf.lpLogFont->lfHeight,72, dc.GetDeviceCaps(LOGPIXELSY)) * 20; // twips

        SetAsModified();

        LockWindowUpdate();

        CRichEditCtrl &edit = GetRichEditCtrl();
        edit.SetFont(&m_font);

        // Save current selection
        CHARRANGE crOldSel;
        edit.GetSel(crOldSel);

        edit.SetSel(0,edit.GetTextLength());

        CHARFORMAT cfm;
        cfm.cbSize = sizeof(cfm);
        edit.GetSelectionCharFormat(cfm);

        _tcscpy_s(cfm.szFaceName, m_strFontFaceName);
        cfm.dwMask = CFM_FACE | CFM_SIZE;
        cfm.yHeight = m_FontHeight;
        edit.SetSelectionCharFormat(cfm);

        FormatTextLines(0,edit.GetTextLength());

        // Restore selection
        edit.SetSel(crOldSel);

        UnlockWindowUpdate();
    }
}

void CVisualTextView::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->IsModified());
}

void CVisualTextView::OnUpdateSaveAll(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.DirtyDocs());
}

