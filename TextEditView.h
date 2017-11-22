/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_TEXTEDITVIEW_H__EA788130_E59D_11D3_A5C2_00105A99059F__INCLUDED_)
#define AFX_TEXTEDITVIEW_H__EA788130_E59D_11D3_A5C2_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextEditView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTextEditView view

class CTextEditView : public CScrollView
{
protected:
	CTextEditView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTextEditView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextEditView)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTextEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CTextEditView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_strFilePath;
	CString m_strLine;
	CString m_strDisplayLine;
	CStringList m_stringListLines;
	CStringList m_stringListDisplayLines;
	bool m_boolFirstDraw;
	bool m_boolLeftDragging;
	bool m_boolLineDisplayListFilled;
	CFont m_fontText;
	TEXTMETRIC m_tm;
	int m_intLineHeight;
	int m_intMaxLineWidth;
	int m_intCharWidth;
	int m_intCursorX;
	int m_intCursorY;
	int m_intCursorSelectStartX;
	int m_intCursorSelectStartY;
	int m_intCursorSelectEndX;
	int m_intCursorSelectEndY;
	int m_intCursorDragSelectX;
	int m_intCursorDragSelectY;
	int m_intColumnNumber;
	int m_intLineNumber;
	int m_intTimerTicker;
	UINT m_nTimerID;
	_TCHAR m_keyLast;

	void DrawText(CDC* pDC);
	void DrawLines(CDC* pDC, int startRow=0);
	void SetMaximumLineLength(CDC* pDC);
	void PrintBreakArea(CDC* pDC, int linePos);
	void PrintSelectedLines();
	void PrintLine();
	void PrintLine(int cursorY);
	void PrintLine(CDC* pDC, CString lineStr, int linePos, bool display=true);
	void StringOut(CDC* pDC, int linePos, int &horizPos, CString str, bool display=true);
	bool IsWordColor(CString wordStr, _TCHAR *list[]);
	void DrawCursor();
	void InsertLine();
	void InsertCharacter(UINT nChar);
	void DeleteCharacter();
	void TraceIt(CString captionStr, UINT nChar=0, int x=-1, int y=-1);
	void CalculateCursorPoint(CPoint point, int &x, int &y, bool setIt=true);
	void InvertText();
	void InvertText(int lastCursorX, int lastCursorY, int cursorX, int cursorY);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnViewOptions();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTEDITVIEW_H__EA788130_E59D_11D3_A5C2_00105A99059F__INCLUDED_)
