/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// TextEditView.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "MainFrm.h"
#include "TextEditView.h"

extern CString appURL;

#define ID_TIMER_CURSOR 500

#define VT_EDITOR_MARGIN 20

static _TCHAR *vtEditorBlueWords [] = {
	_T("<-"),
	_T("@@"),
	_T("@RULES"),
	_T("@@RULES"),
	_T("@CODE"),
	_T("@@CODE"),
	_T("@MULTI"),
	_T("@@MULTI"),
	_T("@NODES"),
	_T("@@NODES"),
	_T("@PATH"),
	_T("@@PATH"),
	_T("@PRE"),
	_T("@@PRE"),
	_T("@POST"),
	_T("@@POST"),
	_T("@CHECK"),
	_T("@@CHECK"),
	_T("@RECURSE"),
	_T("@@RECURSE"),
	_T("@SELECT"),
	_T("@@SELECT"),
	_T("@@"),
	_T("xxyyzz")
};

static _TCHAR *vtEditorRedWords [] = {
	_T("_xNIL"),
	_T("_xWILD"),
	_T("_xSTART"),
	_T("_xEND"),
	_T("_xANY"),
	_T("_xALPHA"),
	_T("_xNUM"),
	_T("_xPUNCT"),
	_T("_xBLANK"),
	_T("_xEOF"),
	_T("_xWHITE"),
	_T("_xCAP"),
	_T("_xCAPLET"),	// 07/10/09 AM.
	_T("_xLET"),		// 07/10/09 AM.
	_T("xxyyzz")
};

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

/////////////////////////////////////////////////////////////////////////////
// CTextEditView

IMPLEMENT_DYNCREATE(CTextEditView, CScrollView)

CTextEditView::CTextEditView()
{
}

CTextEditView::~CTextEditView()
{
}


BEGIN_MESSAGE_MAP(CTextEditView, CScrollView)
	//{{AFX_MSG_MAP(CTextEditView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextEditView drawing

void CTextEditView::OnDraw(CDC* pDC)
{
	//CDocument* pDoc = GetDocument();
	// TODO: add draw code here
	DrawText(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CTextEditView diagnostics

#ifdef _DEBUG
void CTextEditView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CTextEditView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTextEditView message handlers

void CTextEditView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();

	m_boolFirstDraw = true;
	m_boolLeftDragging = false;
	m_boolLineDisplayListFilled = false;

	m_strFilePath = appURL;
	m_strLine = _T("");

	m_intCursorX = -1;
	m_intCursorY = -1;
	m_intCursorSelectStartX = -1;
	m_intCursorSelectStartY = -1;
	m_intCursorSelectEndX = -1;
	m_intCursorSelectEndY = -1;
	m_intCursorDragSelectX = -1;
	m_intCursorDragSelectY = -1;

	m_intLineHeight = 0;
	m_intCharWidth = 0;

	m_intTimerTicker = 0;
	
	m_nTimerID = SetTimer(ID_TIMER_CURSOR,100,NULL);
}

void CTextEditView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	if (!m_intCharWidth) {
		m_fontText.CreateFont(16,0,0,0,FW_NORMAL,false,false,0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_MODERN,
#ifdef UNICODE
			_T("Arial Unicode MS")
#else
			_T("Courier New")
#endif
		);
		pDC->SelectObject(&m_fontText);
#ifdef UNICODE
		pDC->GetTextMetricsW(&m_tm);
#else
		pDC->GetTextMetrics(&m_tm);
#endif
		m_intLineHeight = m_tm.tmHeight + m_tm.tmExternalLeading;

		CSize sizeText = pDC->GetTextExtent(_T("W"));
		m_intCharWidth = sizeText.cx;

		ReadFileToStringList(m_stringListLines,m_strFilePath);

		SetMaximumLineLength(pDC);
		CSize docSize(m_intMaxLineWidth,m_stringListLines.GetCount()*m_intLineHeight);
		SetScrollSizes(MM_TEXT,docSize);
	}

	CScrollView::OnPrepareDC(pDC, pInfo);
}

void CTextEditView::DrawText(CDC* pDC)
{
	if (/*!m_boolFirstDraw || */ m_strFilePath == _T(""))
		return;

	DrawLines(pDC);

	m_boolFirstDraw = false;
}

void CTextEditView::DrawLines(CDC* pDC, int startRow)
{
	if (m_stringListLines.GetCount() == 0)
		return;

	POSITION pos = m_stringListLines.GetHeadPosition();
	CString lineStr = _T("");
	int linePos = 0;

	while (pos && (lineStr = m_stringListLines.GetNext(pos))) {
		if (linePos >= startRow) {
			PrintLine(pDC,lineStr,linePos);
			if (!m_boolLineDisplayListFilled)
				m_stringListDisplayLines.AddTail(m_strDisplayLine);
		}
		linePos += m_intLineHeight;
	}

	InvertText();

	m_boolLineDisplayListFilled = true;
}

void CTextEditView::SetMaximumLineLength(CDC* pDC)
{
	POSITION pos = m_stringListLines.GetHeadPosition();
	CString lineStr = _T("");
	int linePos = 0;
	m_intMaxLineWidth = 0;
	CSize sizeText;

	pDC->SelectObject(&m_fontText);

	while (pos && (lineStr = m_stringListLines.GetNext(pos))) {
		sizeText = pDC->GetTextExtent(lineStr);
		if (sizeText.cx > m_intMaxLineWidth)
			m_intMaxLineWidth = sizeText.cx;
	}
}

void CTextEditView::PrintBreakArea(CDC* pDC, int linePos) 
{
	CBrush brush(RGB(230,230,230));
	pDC->FillRect(CRect(0,linePos,VT_EDITOR_MARGIN-2,linePos+m_intLineHeight+1),&brush);
}

void CTextEditView::PrintSelectedLines() 
{
	CClientDC dc(this);
	CPoint scrollPoint;
	scrollPoint = GetScrollPosition();
	int y = m_intCursorSelectStartY-scrollPoint.y;
	int lineNumber = m_intCursorSelectStartY/m_intLineHeight;
	int numLines = 1+abs(m_intCursorSelectStartY-m_intCursorSelectEndY)/m_intLineHeight;
	CString lineStr;

	for (int i=0; i< numLines; i++) {
		PrintLine(&dc,NthStringListItem(m_stringListLines,lineNumber++),y);
		y += m_intLineHeight;
	}

	CString msgStr;
	msgStr.Format(_T("Prt Sel Lines: %d"),numLines);
	TraceIt(msgStr);
}

void CTextEditView::PrintLine(int cursorY) 
{
	CClientDC dc(this);
	CPoint scrollPoint;
	scrollPoint = GetScrollPosition();
	int y = cursorY-scrollPoint.y;
	int lineNumber = cursorY / m_intLineHeight;
	CString lineStr = NthStringListItem(m_stringListLines,lineNumber);

	PrintLine(&dc,lineStr,y);
}

void CTextEditView::PrintLine() 
{
	CClientDC dc(this);
	CPoint scrollPoint;
	scrollPoint = GetScrollPosition();
	int y = m_intCursorY-scrollPoint.y;
	int lineNumber = m_intLineNumber;

	PrintLine(&dc,NthStringListItem(m_stringListLines,lineNumber),y);
}

void CTextEditView::PrintLine(CDC* pDC, CString lineStr, int linePos, bool display) 
{
	m_strDisplayLine = _T("");

	// CLEAR LINE BEFORE PRINTING
	if (display)
		pDC->FillRect(CRect(0,linePos,m_intMaxLineWidth,linePos+m_intLineHeight),WHITE_BRUSH);

	if (lineStr == _T("")) {
		if (display)
			PrintBreakArea(pDC,linePos);
		return;
	}

	CString str = _T("");
	_TCHAR lastChar = '\0';
	_TCHAR nextChar = '\0';
	_TCHAR c;
	int horizPos = VT_EDITOR_MARGIN;
	int len = lineStr.GetLength();
	int i = 0;
	bool inString = false;
	CSize sizeText;


	while (i < len) {

		c = lineStr.GetAt(i++);
		if (i < len)
			nextChar = lineStr.GetAt(i);
		else
			nextChar = '\0';

		if (c == '\\') {
			;
		}

		else if (inString) {
			str += c;
			if (c == '"') {
				StringOut(pDC,linePos,horizPos,str,display);
				inString = false;
				str = _T("");
			}
		}

		else if (c == '"') {
			StringOut(pDC,linePos,horizPos,str,display);
			str = c;
			inString = true;
		}

		else if (lastChar == '\\') {
			str += c;
		}

		else if (c == '\t') {
			StringOut(pDC,linePos,horizPos,str,display);
			StringOut(pDC,linePos,horizPos,_T("      "),display);
			str = _T("");
		}

		else if (c == '#') {
			StringOut(pDC,linePos,horizPos,str,display);
			str = lineStr.Mid(i-1);
			StringOut(pDC,linePos,horizPos,str,display);

			break;
		}

		else if (c == ' ') {
			str += c;
			if (nextChar != ' ') {
				StringOut(pDC,linePos,horizPos,str,display);
				str = _T("");
			}
		}

		else if (nextChar == ' ' || nextChar == '\0') {
			str += c;
			StringOut(pDC,linePos,horizPos,str,display);
			str = _T("");
		}

		else {
			str += c;
		}

		lastChar = c;
	}

	if (horizPos > m_intMaxLineWidth)
		m_intMaxLineWidth = horizPos;
	
	if (display)
		PrintBreakArea(pDC,linePos);
}

void CTextEditView::StringOut(CDC* pDC, int linePos, int &horizPos, CString str, bool display) 
{
	if (display && str != _T("")) {
		bool boldIt = false;

		if (str.GetAt(0) == '"')
			pDC->SetTextColor(RGB(255,0,255));

		else if (str.GetAt(0) == '#')
			pDC->SetTextColor(RGB(0,128,0));

		else if (IsWordColor(str,vtEditorBlueWords)) {
			boldIt = true;
			pDC->SetTextColor(RGB(0,0,255));
		}
		else if (IsWordColor(str,vtEditorRedWords))
			pDC->SetTextColor(RGB(255,0,0));

		else
			pDC->SetTextColor(RGB(0,0,0));

		pDC->SelectObject(&m_fontText);
		CSize sizeText = pDC->GetTextExtent(str);
		int startX = horizPos;
		int endX = horizPos+sizeText.cx;
		CString subStr;
		CString msgStr;

		pDC->TextOut(horizPos,linePos,str);
		pDC->SetTextColor(RGB(0,0,0));
		pDC->SetBkColor(RGB(255,255,255));

		//TraceIt(str,0,horizPos,linePos);

		horizPos += sizeText.cx;
	}

	m_strDisplayLine += str;
}

bool CTextEditView::IsWordColor(CString wordStr, _TCHAR *list[]) 
{
	int i = 0;
	CString wStr;

	while ((wStr = list[i++]) != _T("xxyyzz")) {
		if (wStr == wordStr)
			return true;
	}
	return false;
}

void CTextEditView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	logmsg(_T("CTextEditView::OnLbuttonDown"));	// 05/14/08 AM.
	CClientDC dc(this);
	int newX, newY;

	// IF SHIFT DOWN, THEN SELECT FROM LAST CURSOR
	bool shifted = GetKeyState(VK_SHIFT) < 0;
	if (shifted) {
		CalculateCursorPoint(point,newX,newY,true);
		m_intCursorSelectStartX = m_intCursorDragSelectX = m_intCursorX;
		m_intCursorSelectStartY = m_intCursorDragSelectY = m_intCursorY;
		m_intCursorSelectEndX = m_intCursorX = newX;
		m_intCursorSelectEndY = m_intCursorY = newY;
		InvertText();
		TraceIt(_T("shifted left mouse button"));
		return;
	}

	// PRINT OUT OVER CURSOR LINE OR SELECTED LINES
	if (m_intCursorX != -1 && m_intCursorY != -1)
		PrintSelectedLines();

	CalculateCursorPoint(point,newX,newY,true);

	// MUST FOLLOW maxX CALCULATION
	m_intColumnNumber = (newX - VT_EDITOR_MARGIN) / m_intCharWidth;

	// IF NEW CURSOR POSITION, THEN DRAWCURSOR
	if (newX != m_intCursorX || newY != m_intCursorY) {
		m_intCursorX = newX;
		m_intCursorY = newY;
	}

	m_intCursorSelectStartX = m_intCursorSelectEndX = m_intCursorDragSelectX = m_intCursorX;
	m_intCursorSelectStartY = m_intCursorSelectEndY = m_intCursorDragSelectY = m_intCursorY;

	TraceIt(_T("LeftButtonDown"),0,point.x,point.y);

	m_boolLeftDragging = true;

	CScrollView::OnLButtonDown(nFlags, point);
}

void CTextEditView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_boolLeftDragging = false;

	int startLineNumber = m_intCursorSelectStartY / m_intLineHeight;
	int endLineNumber = m_intCursorSelectEndY / m_intLineHeight;

	// CHECK IF SELECTION IS BACKWARDS
	if (startLineNumber > endLineNumber ||
		m_intCursorSelectStartX > m_intCursorSelectEndX) {

		SwitchInt(m_intCursorSelectStartX,m_intCursorSelectEndX);
		SwitchInt(m_intCursorSelectStartY,m_intCursorSelectEndY);
		TraceIt(_T("Left Button Up: Switched!"));
	}
	
	CScrollView::OnLButtonUp(nFlags, point);
}

void CTextEditView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_boolLeftDragging) {
		int x,y;
		CalculateCursorPoint(point,x,y,false);

		if (x != m_intCursorDragSelectX || y != m_intCursorDragSelectY) {

			CClientDC dc(this);

			bool moreThanOneLine = m_intCursorSelectStartY != m_intCursorSelectEndY;
			bool movingDown = y > m_intCursorDragSelectY;
			bool movingUp = y < m_intCursorDragSelectY;
			bool movingLeft = x < m_intCursorDragSelectX;
			bool movingRight = x > m_intCursorDragSelectX;

			bool gettingSmaller =
				(
				(movingUp && y > m_intCursorSelectStartY) ||
				(movingDown && y < m_intCursorSelectEndY) ||
				(movingLeft && x > 0) ||
				(movingRight && x < m_intMaxLineWidth)
				);
			
			if (movingUp && m_intCursorDragSelectY > m_intCursorSelectStartY)
				PrintLine(m_intCursorDragSelectY);

			if (movingDown && m_intCursorDragSelectY < y) {
				TraceIt(_T("moving down print"));
				PrintLine(m_intCursorDragSelectY-m_intLineHeight);
			}

			//if (gettingSmaller)
				//raceIt("getting smaller");

			/*
			if (movingUp) {
				TraceIt("movingUp");

				if (m_intCursorDragSelectY > m_intCursorSelectStartY)
					PrintLine(m_intCursorDragSelectY);
				m_intCursorSelectEndX = x;
				m_intCursorSelectEndY = y;
			}

			else if (movingDown) {
				TraceIt("movingDown");

				if (m_intCursorDragSelectY == m_intCursorSelectEndY) {
					PrintLine(m_intCursorSelectEndY);
					PrintLine(m_intCursorSelectEndY-m_intLineHeight);
					m_intCursorSelectEndY = y;
				}
				else
					m_intCursorSelectStartY = y;

				m_intCursorSelectStartX = x;
			}

			else if (movingLeft) {
				TraceIt("movingLeft");

				if (gettingSmaller && y == m_intCursorSelectEndY) {
					PrintLine(m_intCursorSelectEndY);
					m_intCursorSelectEndX = x;
					TraceIt("movingLeft printing");
				}
				else {
					TraceIt("movingLeft normal");
					m_intCursorSelectStartX = x;
				}
			}

			else if (movingRight) {
				TraceIt("movingRight");

				if (gettingSmaller && y == m_intCursorSelectStartY) {
					PrintLine(m_intCursorSelectStartY);
					m_intCursorSelectStartX = x;
					TraceIt("movingRight printing");
				}
				else {
					TraceIt("movingRight normal");
					m_intCursorSelectEndX = x;
				}
			}
			*/

			m_intCursorDragSelectX = m_intCursorSelectEndX = m_intCursorX = x;
			m_intCursorDragSelectY = m_intCursorSelectEndY = m_intCursorY = y;

			InvertText();
		}
	}

	CScrollView::OnMouseMove(nFlags, point);
}

void CTextEditView::InvertText() 
{
	int startLineNumber = m_intCursorSelectStartY / m_intLineHeight;
	int endLineNumber = m_intCursorSelectEndY / m_intLineHeight;

	int startX = m_intCursorSelectStartX;
	int endX = m_intCursorSelectEndX;
	int startY = m_intCursorSelectStartY;
	int endY = m_intCursorSelectEndY;

	// CHECK IF SELECTION IS BACKWARDS
	if (startLineNumber > endLineNumber ||
		m_intCursorSelectStartX > m_intCursorSelectEndX) {

		SwitchInt(startX,endX);
		SwitchInt(startY,endY);
		//SwitchInt(m_intCursorSelectStartX,m_intCursorSelectEndX);
		//SwitchInt(m_intCursorSelectStartY,m_intCursorSelectEndY);
	}

	InvertText(startX,startY,endX,endY);
}

void CTextEditView::InvertText(int startX, int startY, int endX, int endY) 
{
	CString lineStr;
	int start,end;
	CString subStr;
	CPoint scrollPoint;
	scrollPoint = GetScrollPosition();
	CClientDC dc(this);

	int startLineNumber = startY / m_intLineHeight;
	int endLineNumber = endY / m_intLineHeight;
	int x = startX;
	int y = startY;

	for (int l = startLineNumber; l <= endLineNumber; l++) {
		lineStr = NthStringListItem(m_stringListDisplayLines,l);
		start = (startX-VT_EDITOR_MARGIN)/m_intCharWidth;
		end = (endX-VT_EDITOR_MARGIN)/m_intCharWidth;

		// IF THERE ARE MORE THAN ONE LINE TO BE INVERTED, THEN SPECIAL TREATMENT
		if (startLineNumber != endLineNumber) {
			if (l == startLineNumber) {
				end = lineStr.GetLength();
			}
			else if (l != startLineNumber && l != endLineNumber) {
				start = 0;
				end = lineStr.GetLength();
				x = VT_EDITOR_MARGIN;
			}
			else if (l == endLineNumber) {
				x = VT_EDITOR_MARGIN;
				start = 0;
			}
		}

		subStr = lineStr.Mid(start,end-start);

		dc.SelectObject(&m_fontText);
		dc.SetTextColor(RGB(255,255,255));
		dc.SetBkColor(RGB(0,0,0));
		dc.TextOut(x-scrollPoint.x,y-scrollPoint.y,subStr);

		CString msgStr;
		msgStr.Format(_T("Invert Text: \"%s\" (%d - %d)"), subStr,start,end);
		TraceIt(msgStr);

		y += m_intLineHeight;
	}
}

void CTextEditView::CalculateCursorPoint(CPoint point, int &x, int &y, bool setIt) 
{
	CClientDC dc(this);

	// ADJUST FOR SCROLLING
	CPoint scrollPoint;
	scrollPoint = GetScrollPosition();
	point.x += scrollPoint.x;
	point.y += scrollPoint.y;

	x = (point.x / m_intCharWidth) * m_intCharWidth + (m_intCharWidth / 2);
	y = (point.y / m_intLineHeight) * m_intLineHeight;

	// CALCULATE NECESSARY VARIABLES RELATED TO CLICKING
	int lineNumber = y / m_intLineHeight;
	CString lineStr = NthStringListItem(m_stringListLines,lineNumber);

	if (setIt) {
		m_intLineNumber = lineNumber;
		m_strLine = lineStr;
	}

	// CALCULATE IF CLICKED BEYOND THE STRING LINE
	CString oldDisplayLineStr = m_strDisplayLine;

	PrintLine(&dc,lineStr,y,false);
	dc.SelectObject(&m_fontText);
	CSize sizeText = dc.GetTextExtent(m_strDisplayLine);
	int maxX = sizeText.cx + VT_EDITOR_MARGIN;
	if (m_strLine == _T(""))
		x = VT_EDITOR_MARGIN;
	else if (m_strLine != _T("") && x > maxX)
		x = maxX;

	if (!setIt)
		m_strDisplayLine = oldDisplayLineStr;
}

void CTextEditView::DrawCursor() 
{
	if (m_intCursorX == -1 || m_intCursorY == -1)
		return;

	CClientDC dc(this);

	// ADJUST FOR SCROLLING
	CPoint scrollPoint;
	scrollPoint = GetScrollPosition();
	int x = m_intCursorX - scrollPoint.x;
	int y = m_intCursorY - scrollPoint.y;

	dc.InvertRect(CRect(x-1,y,x+1,y+m_intLineHeight));
}



void CTextEditView::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent) {
		case ID_TIMER_CURSOR:
		if (!m_boolLeftDragging && m_intTimerTicker++ == 3) {
			DrawCursor();
			m_intTimerTicker = 0;
		}
		break;
	}

	CScrollView::OnTimer(nIDEvent);
}

void CTextEditView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CClientDC dc(this);
	CString cStr = 
		(CString)(_TCHAR)	// 01/12/03 AM.
		nChar;
	CString singleCharStr = _T("_= ");
	bool shifted = GetKeyState(VK_SHIFT) < 0;

	if (nChar == VK_BACK) {
		DeleteCharacter();
	}
	else if (nChar == '\r') {
		InsertLine();
	}
	else if ('A' <= nChar && nChar <= 'Z') {
		if (!shifted)
			nChar = to_lower(nChar);
		InsertCharacter(nChar);
	}
	else if (nChar != VK_SHIFT) {
		if (shifted) {
			switch (nChar) {
			case '`':  nChar = '~'; break;
			case '1':  nChar = '!'; break;
			case '2':  nChar = '@'; break;
			case '3':  nChar = '#'; break;
			case '4':  nChar = '$'; break;
			case '5':  nChar = '%'; break;
			case '6':  nChar = '^'; break;
			case '7':  nChar = '&'; break;
			case '8':  nChar = '*'; break;
			case '9':  nChar = '('; break;
			case '0':  nChar = ')'; break;
			case 186:  nChar = ':'; break;
			case 187:  nChar = '+'; break;
			case 188:  nChar = '<'; break;
			case 189:  nChar = '_'; break;
			case 190:  nChar = '>'; break;
			case 191:  nChar = '?'; break;
			case 192:  nChar = '~'; break;
			case 219:  nChar = '{'; break;
			case 220:  nChar = '|'; break;
			case 221:  nChar = '}'; break;
			case 222:  nChar = '"'; break;
			}
		}
		else {
			switch (nChar) {
			case 186:  nChar = ';'; break;
			case 187:  nChar = '='; break;
			case 188:  nChar = ','; break;
			case 189:  nChar = '-'; break;
			case 190:  nChar = '.'; break;
			case 191:  nChar = '/'; break;
			case 192:  nChar = '`'; break;
			case 219:  nChar = '['; break;
			case 220:  nChar = '\\|'; break;
			case 221:  nChar = ']'; break;
			case 222:  nChar = '\''; break;
			}
		}
		InsertCharacter(nChar);
	}

	TraceIt(_T("OnKeyDown"),nChar);

	m_keyLast = nChar;
}

void CTextEditView::InsertLine() 
{
	CString newLineStr;
	int len = m_strDisplayLine.GetLength();

	if (m_intColumnNumber >= len) {
		newLineStr = _T("");
	}
	else {
		newLineStr += m_strDisplayLine.Mid(m_intColumnNumber+1);
	}
	m_strDisplayLine = newLineStr;

	InsertAfterNthStringListItem(m_stringListLines,m_intLineNumber,newLineStr);
	CClientDC dc(this);
	DrawLines(&dc,m_intLineNumber++);
}

void CTextEditView::InsertCharacter(UINT nChar) 
{
	CString newLineStr;
	int len = m_strDisplayLine.GetLength();

	if (m_intColumnNumber >= len) {
		newLineStr = m_strDisplayLine;
		newLineStr += 
		(CString)(_TCHAR)	// 01/12/03 AM.
			nChar;
	}
	else {
		newLineStr = m_strDisplayLine.Mid(0,m_intColumnNumber);
		newLineStr += 
		(CString)(_TCHAR)	// 01/12/03 AM.
			nChar;
		newLineStr += m_strDisplayLine.Mid(m_intColumnNumber);
	}
	m_strDisplayLine = newLineStr;

	PrintLine();
	ReplaceNthStringListItem(m_stringListLines,m_intLineNumber,newLineStr);

	m_intCursorX += m_intCharWidth;
	m_intColumnNumber++;
}

void CTextEditView::DeleteCharacter() 
{
	CString newLineStr;
	int len = m_strDisplayLine.GetLength();

	if (m_intColumnNumber >= len) {
		newLineStr = m_strDisplayLine;
		ClipRight(newLineStr,1);
	}
	else {
		newLineStr = m_strDisplayLine.Mid(0,m_intColumnNumber-1);
		newLineStr += m_strDisplayLine.Mid(m_intColumnNumber);
	}

	m_strDisplayLine = newLineStr;

	PrintLine();
	ReplaceNthStringListItem(m_stringListLines,m_intLineNumber,m_strDisplayLine);

	m_intCursorX -= m_intCharWidth;
	m_intColumnNumber--;
}

void CTextEditView::OnDestroy() 
{
	if (m_nTimerID) {
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}
	CScrollView::OnDestroy();
}

void CTextEditView::TraceIt(CString captionStr, UINT nChar, int x, int y) 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString charStr;
	CString finalCharStr;

	if (nChar != 0) {
		charStr.Format(_T("%d   (%c) "),nChar,nChar);
		finalCharStr += charStr;
	}

	if (x >= 0 && y >= 0) {
		charStr.Format(_T("(x= %d, y= %d)  "),x,y);
		finalCharStr += charStr;
	}

	charStr.Format(_T("CS:(%d, %d) SS:(%d, %d) ES:(%d - %d), DRAG:(%d - %d), line= %d, col= %d)"),
		m_intCursorX,m_intCursorY,
		m_intCursorSelectStartX,m_intCursorSelectStartY,
		m_intCursorSelectEndX,m_intCursorSelectEndY,
		m_intCursorDragSelectX,m_intCursorDragSelectY,
		m_intLineNumber,m_intColumnNumber);

	finalCharStr += charStr;

	wnd->AddFindLine(captionStr,finalCharStr);
}

BOOL CTextEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	cs.style |= ES_NOHIDESEL;

	return CScrollView::PreCreateWindow(cs);
}

