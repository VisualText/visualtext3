// VisualTextView.cpp : implementation of the CVisualTextView class
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "NLPStrs.h"
#include "VisualText.h"
#include "BrowserDoc.h"
#include "VisualTextDoc.h"
#include "VisualTextView.h"

extern bool appBoolOpeningTextFile;
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
extern int appButtonReturn;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CVisualTextView, CRichEditView)

BEGIN_MESSAGE_MAP(CVisualTextView, CRichEditView)
	//{{AFX_MSG_MAP(CVisualTextView)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_HELP_CODEJOCKSOFTWAREONTHEWEB_CODEJOCKSOFTWAREHOMEPAGE, OnHelpCodejocksoftwareonthewebCodejocksoftwarehomepage)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CRichEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVisualTextView construction/destruction

CVisualTextView::CVisualTextView()
{
	// TODO: add construction code here

}

CVisualTextView::~CVisualTextView()
{
}

BOOL CVisualTextView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if (!CRichEditView::PreCreateWindow(cs))
		return FALSE;

	cs.style &= ~(ES_AUTOHSCROLL|WS_HSCROLL);	// Enable word-wrapping
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CVisualTextView drawing

void CVisualTextView::OnDraw(CDC* pDC)
{
	CVisualTextDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CVisualTextView printing

BOOL CVisualTextView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Output message if there is something wrong ...
//	MessageBox("Could not load ...", "Title ...");
	
	// default CEditView preparation
	return CRichEditView::OnPreparePrinting(pInfo);
}

void CVisualTextView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CEditView begin printing.
	CRichEditView::OnBeginPrinting(pDC, pInfo);
}

void CVisualTextView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CEditView end printing
	CRichEditView::OnEndPrinting(pDC, pInfo);
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

void CVisualTextView::CreateFont()
{
	// get the logfont used by Xtreme Toolkit
	XT_NONCLIENTMETRICS ncm;

	// set the font name to 'Courier New'.
	_tcscpy(ncm.lfMessageFont.lfFaceName, _T("Courier New"));

	// create the font and set the edit controls
	// font to the one we just created.
	m_font.CreateFontIndirect(&ncm.lfMessageFont);
	SetFont(&m_font);
}

int CVisualTextView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CRichEditView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CreateFont();
	CXTColorRef::RefreshColors();
    
	return 0;
}

void CVisualTextView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// keystroke invocation
	if (point.x == -1 && point.y == -1)
	{
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);
		
		point = rect.TopLeft();
		point.Offset(5, 5);
	}
	
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP));
	
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	// append a menu item.
	pPopup->AppendMenu(MF_BYCOMMAND, ID_FILE_NEW, _T("&New"));

	// insert a separator menu item.
	pPopup->InsertMenu(pPopup->GetMenuItemCount()-1, MF_SEPARATOR|MF_BYPOSITION);
	
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y, GetTopLevelFrame());
}

void CVisualTextView::OnHelpCodejocksoftwareonthewebCodejocksoftwarehomepage() 
{
	// TODO: Add your command handler code here
	CXTHyperLink link;
	link.GotoURL( _T( "http://www.codejock.com" ) );
}

void CVisualTextView::Scroll(int lines)
{
	CRichEditCtrl &edit = GetRichEditCtrl();
	if (edit) {
		edit.LineScroll(lines);
	}
}

void CVisualTextView::FormatAll()
{
	FormatTextRange(0,GetTextLength());
}

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

void CVisualTextView::FormatTextRange(int nStart, int nEnd)
{
	CVisualTextDoc *doc = GetDocument();
	if (appBoolOpeningTextFile || nStart >= nEnd || doc->m_DocType != DOC_RULE)
		return;

	m_bInForcedChange = true;

	CRichEditCtrl &edit = GetRichEditCtrl();
	CHARRANGE crOldSel;
	edit.GetSel(crOldSel);

	theApp.BeginWaitCursor();
	
	LockWindowUpdate();

	edit.HideSelection(true, false);

	TCHAR *pBuffer = NULL;
	try {
		edit.SetSel(nStart, nEnd);
		pBuffer = new TCHAR[nEnd - nStart + 1];
		long nLen = edit.GetSelText(pBuffer);

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
	theApp.EndWaitCursor();

	m_bInForcedChange = false;
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
			colRef = RGB(0,150,0);
		else if (colorStr == _T("dark green"))
			colRef = RGB(0,80,0);
		else if (colorStr == _T("purple"))
			colRef = RGB(150,0,150);
		else if (colorStr == _T("dark blue"))
			colRef = RGB(0,0,150);
		else
			colRef = RGB(0,0,255);
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

// NEED TO TEST
void CVisualTextView::AppendToText(CString textStr, int blankLines) 
{
	GetUndoParmsSelText(NULL, m_crPre, true);

	CRichEditCtrl &edit = GetRichEditCtrl();
	long start,end;
	long fileLength = edit.GetTextLength();
	edit.SetSel(fileLength,fileLength);
	m_cr2Pre.cpMin = m_cr2Pre.cpMax = fileLength;

	CString replaceStr = _T("");
	for (int i=0; i<blankLines; i++)
		replaceStr += _T("\n");
	replaceStr += textStr;

	edit.ReplaceSel(replaceStr);
	start = fileLength + blankLines;
	end = start + textStr.GetLength();
	FormatTextRange(start,end);

	m_strPost = replaceStr;
	m_cr2Post.cpMin = fileLength;
	m_cr2Post.cpMax = fileLength + replaceStr.GetLength();

	edit.SetSel(start,end);
	GetUndoParmsSelText(NULL, m_crPost);

	SetAsModified();

	AddToUndoList (new CUndoItem (ReverseReplaceTextAtNewLocOp, ReplaceTextAtNewLocOp, _T(""), m_strPost, m_crPre, m_crPost, m_cr2Pre, m_cr2Post));
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
		*pStr = edit.GetSelText();

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

void CVisualTextView::SetAsModified()
{
	CRichEditCtrl &edit = GetRichEditCtrl();
	edit.SetModify();
	CVisualTextDoc *doc = (CVisualTextDoc *)GetDocument();
	doc->SetTitle(doc->GetTitle());
	ProcessFocus();
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

void CVisualTextView::AdjustDocSizeToText(CString fullFilePathStr)
{
	if (!appBoolDocExists) {

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
		theApp.SetWindowSize(this,tw,tl,fileLength);
	}
}

void CVisualTextView::WindowSizeFromTextSize(long width, long length,
											 long &wOut, long &lOut) 
{
	wOut = width * m_intCharWidth;
	lOut = length * m_intLineHeight;
}

void CVisualTextView::ProcessFocus() 
{
	if (appState == VT_APP_LOADING) {
		CRichEditCtrl &edit = GetRichEditCtrl();
		appStrSelText = edit.GetSelText();
	}

	appStrFocusDocumentType = _T("VisualText");
	appDocFocus = GetDocument();
	appBoolDocDirty = theApp.DirtyDocs(GetDocument());
	appBoolDocsDirty = theApp.DirtyDocs();
}

void CVisualTextView::SetFormatRange(int nStart, int nEnd, BOOL bBold, COLORREF clr)
{
	if (nStart >= nEnd)
		return;

	CRichEditCtrl &edit = GetRichEditCtrl();
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
	ASSERT((nEnd - nStart) == (int)_tcslen(lpsz));

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

bool CVisualTextView::IsAutoGenerated()
{
	return m_boolIsAutoGenerated;
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
	posStr.Format(_T("%s%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d"),
		RelativizePath(pathName),STATUSSEP,
		wp.rcNormalPosition.top,STATUSSEP,
		wp.rcNormalPosition.bottom,STATUSSEP,
		wp.rcNormalPosition.left,STATUSSEP,
		wp.rcNormalPosition.right,STATUSSEP,
		scrollPos,STATUSSEP,
		active,STATUSSEP,
		start,STATUSSEP,
		end);

	return posStr;
}

void CVisualTextView::FindForward() 
{
	OnFindForward();
}

void CVisualTextView::FindBack() 
{
	OnFindBack();
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
//	long numLines = edit.GetLineCount() - 1;
//	long end = edit.LineIndex(numLines);

	FINDTEXTEX *textFind = new FINDTEXTEX;
	_TCHAR buff[1001];
	_tcscpy(buff,(const _TCHAR *)appStrFind);
	buff[appStrFind.GetLength()] = '\0';
	textFind->lpstrText = buff;
	textFind->chrg.cpMin = range.cpMax + 1;
	textFind->chrg.cpMax = /* end */ -1; // SRP 010507
	textFind->chrgText = range;

	if (edit.FindText(NULL,textFind) < 0) {
		textFind->chrg.cpMin = 0L;
		textFind->chrg.cpMax = /* end */ -1; // SRP 010507
		if (edit.FindText(NULL,textFind) < 0) {
			CString msgStr;
			msgStr.Format(_T("\"%s\" not found!"),appStrFind);
			AfxMessageBox(msgStr);
		}
	}

	edit.SetSel(textFind->chrgText);
	delete textFind;
}

void CVisualTextView::OnFindBack() 
{
	if (!GetSelTextTrim(true,false))
		return;

	CRichEditCtrl &edit = GetRichEditCtrl();
	CHARRANGE range;
	edit.GetSel(range);
	long numLines = edit.GetLineCount() - 1;
	long end = edit.LineIndex(numLines);
	long len = appStrFind.GetLength();
	long found = -1;

	FINDTEXTEX *textFind = new FINDTEXTEX;
	_TCHAR buff[1001];
	_tcscpy(buff,(const _TCHAR *)appStrFind);
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
	CString selStr = edit.GetSelText();

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
