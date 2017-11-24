/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__B74C696A_3328_4767_900B_9AEB0552C14C__INCLUDED_)
#define AFX_MAINFRM_H__B74C696A_3328_4767_900B_9AEB0552C14C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DockingPaneManager.h"

#include "TreeViewText.h"
#include "AttributeEditor.h"
#include "SlotDlg.h"
#include "VisualText.h"
#include "VisualTextDoc.h"
#include "VisualTextView.h"
#include "TreeViewAnalyzer.h"
#include "TreeViewGrammar.h"
#include "TreeViewOutput.h"
#include "FindPane.h"
#include "FindPane2.h"
#include "AnalyzePane.h"
#include "LogPane.h"

enum OUTPUT_TAB {
	OUTPUT_TAB_NONE = -1,
	OUTPUT_TAB_ANALYZE,
	OUTPUT_TAB_DEBUG,
	OUTPUT_TAB_FIND1,
	OUTPUT_TAB_FIND2,
};

enum WORKSPACE_TAB {
	WORKSPACE_NONE = -1,
	WORKSPACE_GRAMMAR,
	WORKSPACE_ANALYZER,
	WORKSPACE_TEXT,
};

#define WORKSPACE_TAB_MAX 3
#define NUM_RECENT_ANALYZERS 10
#define RECENT_ANALYZERS_STARTID 800000
#define RECENT_ANALYZERS_ENDID 800010
#define DUMP_MENU_STARTID 800100
#define DUMP_MENU_ENDID 800299

#define cmdidMRUList 0x200
#define MyMRUListGroup 0x1200

class CVisualTextDoc;


class CMainFrame : public CXTPMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

	LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
	CXTPTabClientWnd m_MTIClientWnd;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	//}}AFX_VIRTUAL

	void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

	void CreatePaneWindows();
	void SetTabbedWindows(BOOL value);

// Implementation
public:
	CSlotDlg m_slotDlg;
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CTreeViewAnalyzer		m_wndAnalyzerView;
	CTreeViewGrammar		m_wndGrammarView;
	CTreeViewText			m_wndTextView;
	CDockingPaneManager		m_paneManager;

	CXTPMenuBar*			m_menuBar;
	CXTPMenuBar*			m_menuBarKnowledge;

	CAnalyzePane	m_wndAnalyzePane;
	CLogPane		m_wndLogPane;
	CFindPane		m_wndFindPane;
	CFindPane2		m_wndFindPane2;

public:  // control bar embedded members
	CWnd*			m_pWndFocus; // SRP 2001-02-01 New Protected Member Variable

	CXTPStatusBar		m_wndStatusBar;

	CXTPToolBar*	m_mainToolBar;
	CXTPToolBar*	m_browserToolBar;
	CXTPToolBar*	m_knowledgeToolBar;
	CXTPToolBar*	m_debugToolBar;
	CXTPToolBar*	m_tabToolBar;

// Generated message map functions
protected:
	CXTPWindowPos m_wndPosition;
	BOOL         m_bResourceView;
	BOOL         m_bDockingView;
	BOOL         m_bTabbedView;
	BOOL         m_bGrammarView;
	BOOL         m_bAnalyzerView;
	BOOL         m_bTextView;
	int          m_nTabIndex;

	CString m_strProcessText;
	CString m_strURLText;
	TEXT_TREE_TYPE m_textTreeType;

public:

	CString m_strSlogTextPath;
	CDocument* m_pDocBrowserDictLookup;
	CAttributeEditor* m_pAttributeEditor;

	// Overrode CWnd implementation to restore saved window position.
	void AddComboText(CString textStr, bool select=false);
	void RecentAnalyzerMenu();
	void OpenAttributeEditor(CONCEPT* concept, BOOL bCalledFromKB);
	CVisualTextView *GetActiveVisualTextView();
	BOOL ShowWindowEx(int nCmdShow);
	void EnableToolBarButton(int nIDToolbar, int nIDItem, int value=true);
	void UpdateToolbar();
	void SetMenuID(UINT menuID);
	void SetChosenText(CString newStr);
	CString GetProcessText();
	CString GetURLText();
	void SetProcessText(CString processText);
	void AddAnalyzerLine(CString firstColumn, CString secondColumn, bool showPaneFlag=false);
	void AddDebugLine(CString firstColumn, CString secondColumn, bool showPaneFlag=false);
	void AddDebugLineHead(CString firstColumn, CString secondColumn, bool showPaneFlag=false);
	void AddErrorToDebug();
	CString GetProcessTextFile(CString subFilePathStr=_T(""));
	void SortDebug(int column);
	void ClearOutput();
	void ClearFind();
	void ClearFind2();
	void LoadOutput();
	void Run();
	bool InitComboBox();
	bool GetLogPath(CString &logPathStr, bool dontAsk=false);
	bool GetLogPath(int pass, CString &logPathStr, bool dontAsk=false);
	void UpdateUrlPulldown(CString urlStr);
	void SelectTab(int tabNumber);
	void SelectPass(int pass, bool selectTab=false);
	WORKSPACE_TAB GetActiveTab();
	void SelectText(CDocument *doc, bool selectTab=false);
	void AddConcept(CString exampleStr, CString textPath, long start, long end,
		UINT menuID=0, CONCEPT *concept=NULL);
	void SetOutputSel(CString selStr);
	void ViewLogTree(CString logFilePathStr);
	void AddFindLine(CString firstColumn, CString secondColumn, bool showPaneFlag=false);
	void AddFindLine2(CString firstColumn, CString secondColumn, bool showPaneFlag=false);
	void AddFindLineHead(CString firstColumn, CString secondColumn, bool showPaneFlag=false);
	void AddFindLineHead2(CString firstColumn, CString secondColumn, bool showPaneFlag=false);
	void GrammarSamplePopup(CWnd *wnd, CPoint point);
	void GrammarSampleViewPopup(CWnd *wnd, CPoint point, CONCEPT *concept);
	void SaveAnalyzerFile(bool askUserFlag=false);
	CString GetTextPageModeState();
	void ServerArchive(CString archiveTypeStr);
	void LocalArchive(CString archiveTypeStr);
	void UpdateRecentAnalyzers();
	void UpdateRecentFilesPopup(CMenu* pPopupMenu);
	void UpdateDictionaryLookupURLSPopup(CMenu* pPopupMenu);
	bool AnalyzerCreated();
	CString WindowTitle(CString titleStr=_T(""));
	CONCEPT *GrammarConceptSelected(bool safeFlag=false);
	CONCEPT *GrammarGetConceptFromMenuID(UINT menuID);
	void AddOutputItems(CString stateStr);
	void GenAnalyzer(GRAM_GEN_TYPE type=GRAM_GEN_ALL, bool askConfirm=true);
	void OpenTextOutput();
	void HighlightMatches(int pass);
	void HighlightMatches(CString logPathStr, CString conceptStr);
	void HighlightMatches(CONCEPT *concept);
	void HighlightMatchesSelections(CStringList &selList, CVisualTextDoc *doc=NULL);
	void OpenNextText(bool backward=false);
	void OpenParseTree(CString logPathStr, CString conceptStr, CStringList &selList);
	void OpenParseTree(CString logPathStr, CString conceptStr, CString textPathStr=_T(""));
	void OpenParseTree(long start, long end);
	void OpenParseTree(bool conceptsOnly=false);
	int SelectPassFromConcept(CONCEPT *concept);
	void SelectGrammarConcept(CString conceptStr);
	CONCEPT *GrammarConceptFromName(CString conceptStr);
	CString GetGrammarConceptName();
	CONCEPT *GrammarSampleConcept(CString fullPathStr);
	CONCEPT *TextViewConcept(CString fullPathStr);
	bool TextHasAnaLogFiles(CString fullPathStr=_T(""));
	void EnsureFindWindowVisible(void);
	void FindWord(CString selStr=_T(""), CVisualTextDoc *doc=NULL, bool noCase=false, CString dirPathStr=_T(""));
	int FindWordInGrammar(CString selStr, bool noCase=false);
	int FindWordInOutputFiles(CString selStr, CString extStr, bool noCase);
	int FindWordInTextFiles(CString selStr, bool noCase);
	int FindWordInRuleFiles(CString selStr, CString dirPathStr, bool noCase=false);
	int FindWordInOtherRuleFiles(CString selStr, CString dirPathStr, bool noCase=false); // 06/14/03 AM.
	bool RenameAnalyzerPass(CONCEPT *concept, CString newNameStr);
	void RetypeAnalyzerPass(CONCEPT *concept, ANAL_PASS_TYPE type);
	void SelectText(CString relFilePathStr, long start=-1, long end=-1);
	void ReloadTextItem(CString fullPathStr);
	void SetTextAsAnalyzed(CString fullPathStr);
	bool IsSelectedTextOpen();
	void BrowserNavigate(CString urlStr=_T(""), bool newWindow=false, CBrowserDoc *docIn=NULL);
	int PassFromFile(CString fileNameStr);
	CString ProcessedFileStr(CString extStr=_T(""), CString processPathStr=_T(""));
	void RunAnalyzer(CString textPathStr=_T(""));
	void ProcessedFiles(CStringList &files, CString extStr=_T(""), CString processPathStr=_T(""));
	void RegenerateSampleFile(CString fullPathStr);
	void StopProcess();
	void SetAnalyzingToolbarButtons(bool value=true);
	void SetSpideringToolbarButtons(bool value=true);
	void ChooseAnalyzer(UINT idIn);
	void HighlightText(CVisualTextDoc *doc=NULL);
	CString LogFileRule(long start, long end);
	void AppendToRuleFile(CString ruleStr);
	void CMainFrame::SetFrameTitle(CString addString=_T(""));
	bool SelectedTextFolderEmpty();
	CONCEPT *ConceptFromPassName(CString passNameStr);
	void SetPassActive(CONCEPT *concept, bool activeFlag);
	void OpenBrowserDictLookup(LPCTSTR lpURL, LPCTSTR lpWord);
	void ReIconKB(CONCEPT *concept);
	int FindWordInFile(CString dirPathStr, CString fileName, CString selStr, bool noCase=true);
	CAttributeEditor *GetAttributeDlg();
	int FindWordInDirectoryFiles(CString selStr, CString dirPathStr=_T(""), CString extStr=_T(""), bool noCase=true);
	int FindWordInSequenceFiles(CString selStr, CString dirPathStr=_T(""), CString extStr=_T(""), bool noCase=true);
	void AddBitmapToMenuItem(UINT menuID, UINT bitmapID);
	void AddChildWindowStates();
	void AutosizeAll();
	void SaveGUITheme();

	void OnLoadMenuIndirect();
	UINT AddMenuItem(LPVOID MenuTemplate, int TemplateBytes, WCHAR* MenuString, WORD MenuID, BOOL IsPopup, BOOL LastItem);

	void SetRowCol(int nRow, int nCol);
	void FindDlg();

protected:
	CStringArray		m_strURLs;

	void HideShowTabView(BOOL& bTabView, CWnd* pTree, LPCTSTR lpszText, int iIndex, int iIconIndex);

	//{{AFX_MSG(CMainFrame)
	afx_msg void OnShowViewOther(UINT nID);
	afx_msg void OnUpdateRowColIndicator(CCmdUI* pCmdUI);
	afx_msg void OnFilePreferences();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnViewWorkspace();
	afx_msg void OnUpdateViewWorkspace(CCmdUI* pCmdUI);
	afx_msg void OnViewOutput();
	afx_msg void OnUpdateViewOutput(CCmdUI* pCmdUI);
	afx_msg void OnWindowDockingview();
	afx_msg void OnUpdateWindowDockingview(CCmdUI* pCmdUI);
	afx_msg void OnTabGrammarView();
	afx_msg void OnUpdateTabGrammarView(CCmdUI* pCmdUI);
	afx_msg void OnTabAnalyzerView();
	afx_msg void OnUpdateTabAnalyzerView(CCmdUI* pCmdUI);
	afx_msg void OnTabTextView();
	afx_msg void OnUpdateTabTextView(CCmdUI* pCmdUI);

	afx_msg BOOL OnDefaultCommandRange(UINT nID);
	afx_msg void OnUpdateDefaultCommandRange(CCmdUI* pCmdUI);
	afx_msg LRESULT OnToolbarDropDown(WPARAM wParam, LPARAM lParam);
	afx_msg void OnHelpVisualTexthelp();
	afx_msg void OnClose();
	afx_msg void OnSaveAll();
	afx_msg void OnRun();
	afx_msg void OnFindBack();
	afx_msg void OnFindForward();
	afx_msg void OnDestroy();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnFileNewAnalyzer();
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI) ; 
	afx_msg void OnTreeUp();
	afx_msg void OnTreeDown();
	afx_msg void OnGenAnalyzer();
	afx_msg void OnSaveKb();
	afx_msg void OnUpdateSaveKb(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTreeUp(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTreeDown(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGenAnalyzer(CCmdUI* pCmdUI);
	afx_msg void OnGenDirtyAndRun();
	afx_msg void OnFileArchivesArchiveanalyzer();
	afx_msg void OnFileArchivesLocalArchive();
	afx_msg void OnFileArchivesServerarchive();
	afx_msg void OnFileArchivesUploadanalyzer();
	afx_msg void OnUpdateGenDirtyAndRun(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRun(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveAll(CCmdUI* pCmdUI);
	afx_msg void OnRunNext();
	afx_msg void OnAnalyzerGenerateall();
	afx_msg void OnAnalyzerGenerateunlocked();
	afx_msg void OnAnalyzerGeneratedirty();
	afx_msg void OnHighlighting();
	afx_msg void OnUpdateHighlighting(CCmdUI* pCmdUI);
	afx_msg void OnGenerateLogs();
	afx_msg void OnUpdateGenerateLogs(CCmdUI* pCmdUI);
	afx_msg void OnDelete();
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnFind();
	afx_msg void OnStopProcess();
	afx_msg void OnUpdateStopProcess(CCmdUI* pCmdUI);
	afx_msg void OnVerboseMode();
	afx_msg void OnUpdateVerboseMode(CCmdUI* pCmdUI);
	afx_msg void OnPrevText();
	afx_msg void OnNextText();
	afx_msg void OnPageMode();
	afx_msg void OnUpdatePageMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrevText(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNextText(CCmdUI* pCmdUI);
	afx_msg void OnTree();
	afx_msg void OnGoURL();
	afx_msg void OnProcessedText();
	afx_msg void OnRuleFile();
	afx_msg void OnOutput();
	afx_msg void OnUpdateOutput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDump(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTabCombo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTree(CCmdUI* pCmdUI);
	afx_msg void OnKnowledgeBase();
	afx_msg void OnUp();
	afx_msg void OnUpdateUp(CCmdUI* pCmdUI);
	afx_msg void OnDown();
	afx_msg void OnUpdateDown(CCmdUI* pCmdUI);
	afx_msg void OnTabSave();
	afx_msg void OnUpdateTabSave(CCmdUI* pCmdUI);
	afx_msg void OnSlotFill();
	afx_msg void OnUpdateSlotFill(CCmdUI* pCmdUI);
	afx_msg void OnWebpage();
	afx_msg void OnUpdateWebpage(CCmdUI* pCmdUI);
	afx_msg void OnTreeConcept();

	/*
	afx_msg void OnToolsKbtoolbar();
	afx_msg void OnUpdateToolsKbtoolbar(CCmdUI* pCmdUI);
	afx_msg void OnToolsBrowsertoolbar();
	afx_msg void OnUpdateToolsBrowsertoolbar(CCmdUI* pCmdUI);
	afx_msg void OnToolsTabtoolbar();
	afx_msg void OnUpdateToolsTabtoolbar(CCmdUI* pCmdUI);
	*/

	afx_msg void OnUpdateBrowserOpenurl(CCmdUI* pCmdUI);
	afx_msg void OnBrowserOpenurl();
	afx_msg void OnToolsFind();
	afx_msg void OnUpdateToolsFind(CCmdUI* pCmdUI) ; 
	afx_msg void OnCompile();
	afx_msg void OnToolsDebugtoolbar();
	afx_msg void OnUpdateToolsDebugtoolbar(CCmdUI* pCmdUI);
	afx_msg void OnDictionary();
	afx_msg void OnUpdateRunNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCompile(CCmdUI* pCmdUI);
	afx_msg void OnKbDictionary();
	afx_msg void OnToolsAsciitable();
	afx_msg void OnToolsHexviewer();
	afx_msg void OnUpdateToolsHexviewer(CCmdUI* pCmdUI) ;
	afx_msg void OnToolsTextfileinfo();
	afx_msg void OnUpdateToolsTextfileinfo(CCmdUI* pCmdUI) ; 
	afx_msg void OnToolsCharacterviewer();
	afx_msg void OnUpdateToolsCharacterviewer(CCmdUI* pCmdUI); 
	afx_msg void OnFindmenuHelp();
	afx_msg void OnUpdateDictionary(CCmdUI* pCmdUI);
	afx_msg void OnUpdateKnowledgeBase(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProcessedText(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRuleFile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTreeConcept(CCmdUI* pCmdUI);
	afx_msg void OnUpdateKbDictionary(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAnalyzerGenerateall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAnalyzerGeneratedirty(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAnalyzerGenerateunlocked(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFind(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFindBack(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFindForward(CCmdUI* pCmdUI);
	afx_msg void OnBatchMode();
	afx_msg void OnUpdateBatchMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWindowCloseall(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);  // SRP 01-01-29
	afx_msg void OnDisplayTabswindow();
	afx_msg void OnDisplayFindwindow();
	afx_msg void OnDisplayOutputwindow();
	afx_msg void OnUpdateDisplayTabswindow(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayOutputwindow(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayFindwindow(CCmdUI* pCmdUI);
	afx_msg void OnFileNew();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnCompileKb();
	afx_msg void OnKbCompilekb();
	afx_msg void OnUpdateKbCompilekb(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCompileKb(CCmdUI* pCmdUI);
	afx_msg void OnAnalyzerCompileanalyzer();
	afx_msg void OnUpdateAnalyzerCompileanalyzer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileArchivesArchiveanalyzer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileArchivesUploadanalyzer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileArchivesLocalArchive(CCmdUI* pCmdUI);
	afx_msg void OnReplaceInFiles();
	afx_msg void OnUpdateReplaceInFiles(CCmdUI* pCmdUI);
	afx_msg void OnTreeSemantics();
	afx_msg void OnUpdateTreeSemantics(CCmdUI* pCmdUI);
	afx_msg void OnViewTreesemanticsmode();
	afx_msg void OnUpdateViewTreesemanticsmode(CCmdUI* pCmdUI);
	afx_msg void OnViewTreeOffsetsmode();
	afx_msg void OnUpdateViewTreeOffsetsmode(CCmdUI* pCmdUI);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnAttributeeditor();
	afx_msg void OnSetTextAsAnalyzed(AFX_PMSG index);
	afx_msg void OnUpdateAttributeeditor(CCmdUI* pCmdUI);
	afx_msg void OnToolsAddwords();
	afx_msg void OnUpdateToolsAddwords(CCmdUI* pCmdUI);
	afx_msg void OnToolsKbattributes();
	afx_msg void OnUpdateToolsKbattributes(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnToolsDictionaryURLLookup(UINT uID);
	afx_msg void OnUpdateToolsDictionaryURLLookup(CCmdUI* pCmdUI);
	afx_msg void OnChooseDumpFile(UINT idIn);
	afx_msg void OnEditTest();
	afx_msg void OnContextHelp();   // for Shift+F1 help
	afx_msg void OnUpdateInsert(CCmdUI* pCmdUI);
	afx_msg LRESULT OnBringWindowToTop(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateLineCol(CCmdUI* pCmdUI);
	afx_msg long OnDestroyAttributeEditor(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnSetActiveView(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChooseAnalyzer(UINT idIn);
	//afx_msg LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCustomize();

	afx_msg void OnDump();
	afx_msg void OnDumpSelect(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnUrlCombobox(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateComboURL(CCmdUI* pCmdUI);
	afx_msg void OnRecentCombobox(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateComboRecent(CCmdUI* pCmdUI);
	afx_msg void OnListBoxControlPopup(NMHDR* pNMHDR, LRESULT* pRes);
	afx_msg void OnEditUndo(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnUrlCombobox();

	afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);

	DECLARE_MESSAGE_MAP()

	void ExchangeLayout(CXTPPropExchange* pPX, BOOL bSerializeControls = TRUE);

	BOOL m_bFullScreen;
	CRect m_rcMainFrame;
	CXTPPropExchangeXMLNode* m_pFullScreenLayout;

private:
	int m_intRecentAnalyzersPos;
	bool m_boolBlockOutputTabChange;

	void InitVars();
	void RunNext(bool closeOtherWindows=true);
	CMenu* _GetDynaPopupMenu1();

public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnViewTreetextmode();
	afx_msg void OnUpdateViewTreetextmode(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolsThemeOffice2003theme(CCmdUI* pCmdUI);
	afx_msg void OnToolsThemeOffice2003theme();
	afx_msg void OnUpdateToolsThemeVisualStudio2008(CCmdUI* pCmdUI);
	afx_msg void OnToolsThemeVisualStudio2008();
	afx_msg void OnUpdateToolsThemeVisualStudio2010(CCmdUI* pCmdUI);
	afx_msg void OnToolsThemeVisualStudio2010();
	afx_msg void OnUpdateFileRecentanalyzers(CCmdUI *pCmdUI);
	afx_msg void OnViewAnalyzerview();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__B74C696A_3328_4767_900B_9AEB0552C14C__INCLUDED_)
