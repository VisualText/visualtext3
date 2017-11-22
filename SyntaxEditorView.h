// MDITextEditorView.h : interface of the CSyntaxEditorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDITEXTEDITORVIEW_H__8755594F_0001_4908_9353_5187AEEF9610__INCLUDED_)
#define AFX_MDITEXTEDITORVIEW_H__8755594F_0001_4908_9353_5187AEEF9610__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define CViewBase	CView
#define CViewBase	CXTPSyntaxEditView

class CSyntaxEditorView : public CViewBase
{
protected: // create from serialization only
	CSyntaxEditorView();
	DECLARE_DYNCREATE(CSyntaxEditorView)

// Attributes
public:
	CSyntaxEditorDoc* GetDocument();
	CString StateStr();
	void SaveState();

// Operations
public:

//private:
	CURSOR_LINE_TYPE m_cursorLineType;
	bool m_boolIsAnalyzed;
	int appIntRuleTabSize;
	CHARRANGE m_crPre, m_crPost, m_cr2Pre, m_cr2Post; // Working Variables
	CString m_strPre, m_strPost; // Working Variables

	void SetCursorLineType();
	bool CalculateIsAnalyzed();
	bool ReformatRule(BOOL bLink=false);
	CURSOR_LINE_TYPE GetCursorLineType();
	bool SelectionInRule(bool noAlert=false);
	void GetRuleLines(CStringList &ruleLines, long &start, long &end, bool noAlert=false);
	void AddAttribute(CString newElementStr, CString valueStr=_T(""));
	void AddRuleElement(CString newRuleElementStr, bool parens=false);
	void ElementLocation(CString lineStr, CString elementStr, int &cursor, int &insertPoint,
										 int &bracketStart, int &bracketEnd,
										 int &elementStart, int &elementEnd);
	void ChangeElement(CString newElementStr);
	void InsertString(CString str);
	void InsertLine(CString str, int cursorPos=0);
	void TokenizeLine(CStringList &tokens, CString lineStr, int &bracketStatus);
	bool GetWorkingText(CString &lineStr,long &lineIndex,long &selStart,long &selEnd, CString alertMsgStr);
	bool GetWorkingText2(CString &lineStr, long &lineIndex, long &colIndex, long &lineIndex2, long &colIndex2, long &selStart,long &selEnd, CString alertMsgStr);
	CString CSyntaxEditorView::StandardizeLine(CString lineInStr,
		int &elementNum,	// Track rule elements.	// 05/24/07 AM.
		int &bracketStatus,	// Track multi-line brackets.	// 05/24/07 AM.
		int ruleLineNum);
	void CSyntaxEditorView::StringToTokens(CStringList &tokens, CString textStr);
	bool CSyntaxEditorView::GetSelTextTrim(long &start, long &end, bool trimFlag, bool askFlag);
	void FindLocal();
	CString EditGetSelText(void);
	void ReplaceSelection(CString newLineStr, bool addSlashN);
	void AppendToText(CString textStr, int blankLines=0);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSyntaxEditorView)
public:
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

protected:
	virtual BOOL OnRowColChanged(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnParseEvent(NMHDR* pNMHDR, LRESULT* pResult);
	void OnPrintHeader(CDC* pDC, CPrintInfo* pInfo);
	void OnPrintFooter(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CSyntaxEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSyntaxEditorView)
	afx_msg void OnToolsBookmark();
	afx_msg void OnToolsBreakpoint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditFindSilent();
	afx_msg void OnToolsNextBookmark();
	afx_msg void OnUpdateToolsNextBookmark(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolsOptions(CCmdUI* pCmdUI);
	afx_msg void OnToolsPrevBookmark();
	afx_msg void OnUpdateToolsPrevBookmark(CCmdUI* pCmdUI);
	afx_msg void OnEditTabify();
	afx_msg void OnEditUnTabify();
	afx_msg void OnEditUppercase();
	afx_msg void OnEditLowercase();
	afx_msg void OnEditWhiteSpace();
	afx_msg void OnEditCollapseAll();
	afx_msg void OnEditExpandAll();
	afx_msg void OnUpdateEditWhiteSpace(CCmdUI* pCmdUI);
	afx_msg void OnToolsOptions();
	afx_msg void OnFileGoTo();
	afx_msg void OnReadOnly();
	afx_msg void OnUpdateReadOnly(CCmdUI* pCmdUI);
	afx_msg void OnReloadSyntax();
	afx_msg void OnReloadSyntax2IniMode();
	afx_msg void OnReloadSyntaxMode();
	afx_msg void OnRulefileInsertRules();
	afx_msg void OnRulefileInsertPost();
	afx_msg void OnRulefileInsertXwhite();
	afx_msg void OnRulefileInsertPostrule();
	afx_msg void OnRulefileConvertStringtogeneralrule();
	afx_msg void OnRulefileAddMatches();
	afx_msg void OnRulefileAddExcept();
	afx_msg void OnRulefileAddFails();
	afx_msg void OnRulefileAddOptional();
	afx_msg void OnRulefileAddStar();
	afx_msg void OnRulefileAddremoveS();
	afx_msg void OnRulefileConvertOnelinetomultiple();
	afx_msg void OnRulefileAddremoveMax();
	afx_msg void OnRulefileAddremoveMin();
	afx_msg void OnRulefileChangeXwild();
	afx_msg void OnRulefileChangeXnum();
	afx_msg void OnRulefileChangeXalpha();
	afx_msg void OnRulefileAddremoveTrig();
	afx_msg void OnRulefileInsertXpunct();
	afx_msg void OnRulefileAddremoveTree();
	afx_msg void OnRulefileAddremoveLookahead();
	afx_msg void OnRulefileInsertXalpha();
	afx_msg void OnRulefileAddremoveLayer();
	afx_msg void OnRulefileAddremoveOne();
	afx_msg void OnRulefileChangetoXwhite();
	afx_msg void OnRulefileInsertlineXalpha();
	afx_msg void OnRulefileInsertlineXnum();
	afx_msg void OnRulefileInsertlineXwhite();
	afx_msg void OnRulefileInsertlineXwild();
	afx_msg void OnRulefileAddremovePlus();
	afx_msg void OnRulefileDeleteline();
	afx_msg void OnRulefileCloserest();
	afx_msg void OnRulefileNlphelp();
	afx_msg void OnRulefileSelectpass();
	afx_msg void OnRulefileStringtorule();
	afx_msg void OnRulefileInsertlineXstart();
	afx_msg void OnRulefileCommentuncomment();
	afx_msg void OnRulefileFind();
	afx_msg void OnRulefileRulehelp();
	afx_msg void OnRulefileCloseall();
	afx_msg void OnToolsFind();
	afx_msg void OnUpdateToolsFind(CCmdUI* pCmdUI) ; 
	//afx_msg void OnRulefileFindlocal();
	//afx_msg void OnRulefileFindagain();
	afx_msg void OnRulefileSaverunlog();
	afx_msg void OnRulefileRun();
	afx_msg void OnRulefileViewlog();
	afx_msg void OnRulefileFindinlog();
	afx_msg void OnRulefileDuplicateline();
	afx_msg void OnRulefileHeader();
	afx_msg void OnRulefileInsertDumpfile();
	afx_msg void OnRulefileHelponselection();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	RECT  m_rcMargin;
	DWORD m_dwLastUpdate;
	DWORD m_dwParserStart;
	CXTPSyntaxEditGoToLineDlg m_dlgGoTo;
public:
	afx_msg void OnToolsRuleeditoptions();
	afx_msg void OnUpdateToolsRuleeditoptions(CCmdUI *pCmdUI);
	afx_msg void OnRulefileReformatrule();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
public:
	afx_msg void OnUpdateFileNew(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSaveAll(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	//afx_msg void OnSaveAll();
	afx_msg void OnViewOptions();
};

#ifndef _DEBUG  // debug version in MDITextEditorView.cpp
inline CSyntaxEditorDoc* CSyntaxEditorView::GetDocument()
   { return (CSyntaxEditorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDITEXTEDITORVIEW_H__8755594F_0001_4908_9353_5187AEEF9610__INCLUDED_)
