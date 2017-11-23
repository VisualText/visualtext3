/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "resource.h"
#include "kb.h"
#include "Utils.h"

#include "VisualText.h"
#include "MainFrm.h"
#include "ASCIIDlg.h"
#include "HexEditDlg.h"
#include "TextInfoDlg.h"
#include "FileCharViewerDlg.h"
#include "FTPClient.h"
#include "WorkspaceView.h"
#include "VisualTextDoc.h"
#include "TreeViewAnalyzer.h"
#include "TreeViewGrammar.h"
#include "SyntaxEditorDoc.h"
#include "SyntaxEditorView.h"
#include "ArchiveListDialog.h"
#include "KnowledgeView.h"
#include "ParseTreeDoc.h"
#include "ParseTreeView.h"
#include "ParseTreeView.h"
#include "BrowserView.h"
#include "VisualTextView.h"
#include "VisualTextPreferences.h"
#include "DictionaryDlg.h"
#include "KnowledgeDoc.h"
#include "ReplaceInFilesDlg.h"
#include "FindDlg.h"
#include "ArchiveNameDlg.h"
#include ".\mainfrm.h"

extern BOOL g_bFind2;
extern CString appAskText;
extern bool appBoolAnalyzerLoaded;
extern CVisualTextApp theApp;
extern CString appURL;
extern CStringList appStatus;
extern CString appAskText;
extern CString appNameStr;
extern CString appAnaFilePathStr;
extern CString appDirectoryStr;
extern CString appRecentAnalyzers;
extern CString appStrVersion;
extern CString appStrSelText;
extern CONCEPT *appGUIConcept;	// 06/14/03 AM.
extern bool appBoolTabbedWindows;

// AVIVA Aviva

extern CG *cg;
extern NLP *nlp;
extern RUG *rug;
extern GUI *gui;
extern bool kbDirty;
extern bool appBoolToolbarLabels;
extern bool appBoolAnalyzerLoaded;
extern bool appBoolHighlightText;
extern bool appBoolLogAnalyzer;
extern bool appBoolVerbose;
extern bool appBoolPageMode;
extern bool appBoolShowFileToolbar;
extern bool appBoolShowKBToolbar;
extern bool appBoolShowDebugToolbar;
extern bool appBoolShowTabToolbar;
extern bool appBoolShowBrowserToolbar;
extern bool appBoolCompile;
extern bool appBoolBatchMode;
extern bool appBoolShowTabsWindow;
extern bool appBoolShowDebugWindow;
extern bool appBoolShowFindWindow;
extern bool appBoolDocDirty;
extern bool appBoolDocDirtyLast;
extern bool appBoolDocsDirty;
extern bool appBoolSaveKBFiles;
extern bool appBoolSaveInputFiles;
extern bool appBoolSaveCFiles;
extern bool appBoolTreeSemantics;
extern bool appBoolNodeSemantics;
extern bool appBoolTreeOffsets;
extern bool appBoolTreeText;			// 07/12/08 AM.
extern bool appBoolAnalysisPending;	// 08/01/03 AM.
extern int appIntRuleTabSize;
extern CFont appFontFixed;

extern VT_APP_STATE appState;
extern CString appStrFind;
extern CString appStrArchiveAuthor;
extern CString appStrFocusDocumentType;
extern CString appStrAppsDirectory;
extern CDocument *appDocFocus;
extern CString appRecentAnalyzers;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(XTP_ID_CUSTOMIZE, OnCustomize)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_VIEW_WORKSPACE, OnViewWorkspace)
	//ON_UPDATE_COMMAND_UI(ID_VIEW_WORKSPACE, OnUpdateViewWorkspace)
	//ON_COMMAND(ID_VIEW_OUTPUT, OnViewOutput)
	//ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT, OnUpdateViewOutput)
	//ON_COMMAND(ID_WINDOW_DOCKINGVIEW, OnWindowDockingview)
	//ON_UPDATE_COMMAND_UI(ID_WINDOW_DOCKINGVIEW, OnUpdateWindowDockingview)
	//ON_COMMAND(IDC_TAB_CLASSVIEW, OnTabGrammarView)
	//ON_UPDATE_COMMAND_UI(IDC_TAB_CLASSVIEW, OnUpdateTabGrammarView)
	//ON_COMMAND(IDC_TAB_RESOURCEVIEW, OnTabAnalyzerView)
	//ON_UPDATE_COMMAND_UI(IDC_TAB_RESOURCEVIEW, OnUpdateTabAnalyzerView)
	//ON_COMMAND(IDC_TAB_FILEVIEW, OnTabTextView)
	//ON_UPDATE_COMMAND_UI(IDC_TAB_FILEVIEW, OnUpdateTabTextView)
	ON_COMMAND(ID_SAVE_ALL, OnSaveAll)
	ON_COMMAND(ID_FILE_PREFERENCES, OnFilePreferences)
	ON_COMMAND(ID_HELP_VISUALTEXTHELP, OnHelpVisualTexthelp)
	ON_WM_CLOSE()
	ON_COMMAND(ID_RUN, OnRun)
	ON_COMMAND(ID_FIND_FORWARD, OnFindForward)
	ON_COMMAND(ID_FIND_BACK, OnFindBack)
	ON_WM_DESTROY()
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_FILE_NEWANALYZER, OnFileNewAnalyzer)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW,OnUpdateFileNew)
	ON_COMMAND(ID_TREE_UP, OnTreeUp)
	ON_COMMAND(ID_TREE_DOWN, OnTreeDown)
	ON_COMMAND(ID_GEN_ANALYZER, OnGenAnalyzer)
	ON_COMMAND(ID_SAVE_KB, OnSaveKb)
	ON_UPDATE_COMMAND_UI(ID_SAVE_KB, OnUpdateSaveKb)
	ON_UPDATE_COMMAND_UI(ID_TREE_UP, OnUpdateTreeUp)
	ON_UPDATE_COMMAND_UI(ID_TREE_DOWN, OnUpdateTreeDown)
	ON_UPDATE_COMMAND_UI(ID_GEN_ANALYZER, OnUpdateGenAnalyzer)
	ON_COMMAND(ID_GEN_DIRTY_AND_RUN, OnGenDirtyAndRun)
	ON_COMMAND(ID_FILE_ARCHIVES_ARCHIVEANALYZER, OnFileArchivesArchiveanalyzer)
	ON_COMMAND(ID_FILE_ARCHIVES_LOCAL_ARCHIVE, OnFileArchivesLocalArchive)
	ON_COMMAND(ID_FILE_ARCHIVES_SERVERARCHIVE, OnFileArchivesServerarchive)
	ON_COMMAND(ID_FILE_ARCHIVES_UPLOADANALYZER, OnFileArchivesUploadanalyzer)
	ON_UPDATE_COMMAND_UI(ID_GEN_DIRTY_AND_RUN, OnUpdateGenDirtyAndRun)
	ON_UPDATE_COMMAND_UI(ID_RUN, OnUpdateRun)
	ON_UPDATE_COMMAND_UI(ID_SAVE_ALL, OnUpdateSaveAll)
	ON_COMMAND(ID_RUN_NEXT, OnRunNext)
	ON_COMMAND(ID_ANALYZER_GENERATEALL, OnAnalyzerGenerateall)
	ON_COMMAND(ID_ANALYZER_GENERATEUNLOCKED, OnAnalyzerGenerateunlocked)
	ON_COMMAND(ID_ANALYZER_GENERATEDIRTY, OnAnalyzerGeneratedirty)
	ON_COMMAND(ID_HIGHLIGHTING, OnHighlighting)
	ON_UPDATE_COMMAND_UI(ID_HIGHLIGHTING, OnUpdateHighlighting)
	ON_COMMAND(ID_GENERATELOGS, OnGenerateLogs)
	ON_UPDATE_COMMAND_UI(ID_GENERATELOGS, OnUpdateGenerateLogs)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_UPDATE_COMMAND_UI(ID_DELETE, OnUpdateDelete)
	ON_COMMAND(ID_FIND, OnFind)
	ON_COMMAND(ID_STOP_PROCESS, OnStopProcess)
	ON_UPDATE_COMMAND_UI(ID_STOP_PROCESS, OnUpdateStopProcess)
	ON_COMMAND(ID_VERBOSE_MODE, OnVerboseMode)
	ON_UPDATE_COMMAND_UI(ID_VERBOSE_MODE, OnUpdateVerboseMode)
	ON_COMMAND(ID_PREV_TEXT, OnPrevText)
	ON_COMMAND(ID_NEXT_TEXT, OnNextText)
	ON_COMMAND(ID_PAGE_MODE, OnPageMode)
	ON_UPDATE_COMMAND_UI(ID_PAGE_MODE, OnUpdatePageMode)
	ON_UPDATE_COMMAND_UI(ID_PREV_TEXT, OnUpdatePrevText)
	ON_UPDATE_COMMAND_UI(ID_NEXT_TEXT, OnUpdateNextText)
	ON_COMMAND(ID_TREE, OnTree)
	ON_COMMAND(ID_GO_URL, OnGoURL)
	ON_COMMAND(ID_PROCESSED_TEXT, OnProcessedText)
	ON_COMMAND(ID_RULE_FILE, OnRuleFile)
	ON_COMMAND(ID_OUTPUT, OnOutput)
	ON_UPDATE_COMMAND_UI(ID_OUTPUT, OnUpdateOutput)
	ON_UPDATE_COMMAND_UI(ID_COMBOBOX, OnUpdateTabCombo)
	ON_UPDATE_COMMAND_UI(ID_TREE, OnUpdateTree)
	ON_COMMAND(ID_KNOWLEDGE_BASE, OnKnowledgeBase)
	ON_COMMAND(ID_UP, OnUp)
	ON_UPDATE_COMMAND_UI(ID_UP, OnUpdateUp)
	ON_COMMAND(ID_DOWN, OnDown)
	ON_UPDATE_COMMAND_UI(ID_DOWN, OnUpdateDown)
	ON_COMMAND(ID_TAB_SAVE, OnTabSave)
	ON_UPDATE_COMMAND_UI(ID_TAB_SAVE, OnUpdateTabSave)
	ON_COMMAND(ID_SLOT_FILL, OnSlotFill)
	ON_UPDATE_COMMAND_UI(ID_SLOT_FILL, OnUpdateSlotFill)
	ON_COMMAND(ID_WEBPAGE, OnWebpage)
	ON_UPDATE_COMMAND_UI(ID_WEBPAGE, OnUpdateWebpage)
	ON_COMMAND(ID_TREE_CONCEPT, OnTreeConcept)

	/*
	ON_COMMAND(ID_TOOLS_KBTOOLBAR, OnToolsKbtoolbar)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_KBTOOLBAR, OnUpdateToolsKbtoolbar)
	ON_COMMAND(ID_TOOLS_BROWSERTOOLBAR, OnToolsBrowsertoolbar)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BROWSERTOOLBAR, OnUpdateToolsBrowsertoolbar)
	ON_COMMAND(ID_TOOLS_TABTOOLBAR, OnToolsTabtoolbar)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_TABTOOLBAR, OnUpdateToolsTabtoolbar)
	ON_COMMAND(ID_TOOLS_DEBUGTOOLBAR, OnToolsDebugtoolbar)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_DEBUGTOOLBAR, OnUpdateToolsDebugtoolbar)
	*/

	ON_UPDATE_COMMAND_UI(ID_BROWSER_OPENURL, OnUpdateBrowserOpenurl)
	ON_COMMAND(ID_BROWSER_OPENURL, OnBrowserOpenurl)
	ON_COMMAND(ID_TOOLS_FIND, OnToolsFind)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_FIND, OnUpdateToolsFind)
	ON_COMMAND(ID_COMPILE, OnCompile)
	ON_COMMAND(ID_DICTIONARY, OnDictionary)
	ON_UPDATE_COMMAND_UI(ID_RUN_NEXT, OnUpdateRunNext)

	ON_UPDATE_COMMAND_UI(ID_COMPILE, OnUpdateCompile)
	ON_COMMAND(ID_KB_DICTIONARY, OnKbDictionary)
	ON_COMMAND(ID_TOOLS_ASCIITABLE, OnToolsAsciitable)
	ON_COMMAND(ID_TOOLS_HEXVIEWER, OnToolsHexviewer)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_HEXVIEWER, OnUpdateToolsHexviewer)
	ON_COMMAND(ID_TOOLS_TEXTFILEINFO, OnToolsTextfileinfo)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_TEXTFILEINFO, OnUpdateToolsTextfileinfo)
	ON_COMMAND(ID_TOOLS_CHARACTERVIEWER, OnToolsCharacterviewer)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CHARACTERVIEWER, OnUpdateToolsCharacterviewer)
	ON_COMMAND(ID_FINDMENU_HELP, OnFindmenuHelp)
	ON_UPDATE_COMMAND_UI(ID_DICTIONARY, OnUpdateDictionary)
	ON_UPDATE_COMMAND_UI(ID_KNOWLEDGE_BASE, OnUpdateKnowledgeBase)
	ON_UPDATE_COMMAND_UI(ID_PROCESSED_TEXT, OnUpdateProcessedText)
	ON_UPDATE_COMMAND_UI(ID_RULE_FILE, OnUpdateRuleFile)
	ON_UPDATE_COMMAND_UI(ID_TREE_CONCEPT, OnUpdateTreeConcept)
	ON_UPDATE_COMMAND_UI(ID_KB_DICTIONARY, OnUpdateKbDictionary)
	ON_UPDATE_COMMAND_UI(ID_ANALYZER_GENERATEALL, OnUpdateAnalyzerGenerateall)
	ON_UPDATE_COMMAND_UI(ID_ANALYZER_GENERATEDIRTY, OnUpdateAnalyzerGeneratedirty)
	ON_UPDATE_COMMAND_UI(ID_ANALYZER_GENERATEUNLOCKED, OnUpdateAnalyzerGenerateunlocked)
	ON_UPDATE_COMMAND_UI(ID_FIND, OnUpdateFind)
	ON_UPDATE_COMMAND_UI(ID_FIND_BACK, OnUpdateFindBack)
	ON_UPDATE_COMMAND_UI(ID_FIND_FORWARD, OnUpdateFindForward)
	ON_COMMAND(ID_BATCH_MODE, OnBatchMode)
	ON_UPDATE_COMMAND_UI(ID_BATCH_MODE, OnUpdateBatchMode)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CLOSEALL, OnUpdateWindowCloseall)
	ON_WM_SETCURSOR() // SRP 01-01-29
	ON_WM_ACTIVATE() // SRP 2001-02-01
	ON_COMMAND(ID_DISPLAY_TABSWINDOW, OnDisplayTabswindow)
	ON_COMMAND(ID_DISPLAY_FINDWINDOW, OnDisplayFindwindow)
	ON_COMMAND(ID_DISPLAY_OUTPUTWINDOW, OnDisplayOutputwindow)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_TABSWINDOW, OnUpdateDisplayTabswindow)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_OUTPUTWINDOW, OnUpdateDisplayOutputwindow)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_FINDWINDOW, OnUpdateDisplayFindwindow)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_COMPILE_KB, OnCompileKb)
	ON_COMMAND(ID_KB_COMPILEKB, OnKbCompilekb)
	ON_UPDATE_COMMAND_UI(ID_KB_COMPILEKB, OnUpdateKbCompilekb)
	ON_UPDATE_COMMAND_UI(ID_COMPILE_KB, OnUpdateCompileKb)
	ON_COMMAND(ID_ANALYZER_COMPILEANALYZER, OnAnalyzerCompileanalyzer)
	ON_UPDATE_COMMAND_UI(ID_ANALYZER_COMPILEANALYZER, OnUpdateAnalyzerCompileanalyzer)
	ON_UPDATE_COMMAND_UI(ID_FILE_ARCHIVES_ARCHIVEANALYZER, OnUpdateFileArchivesArchiveanalyzer)
	ON_UPDATE_COMMAND_UI(ID_FILE_ARCHIVES_UPLOADANALYZER, OnUpdateFileArchivesUploadanalyzer)
	ON_UPDATE_COMMAND_UI(ID_FILE_ARCHIVES_LOCAL_ARCHIVE, OnUpdateFileArchivesLocalArchive)
	ON_COMMAND(ID_REPLACE_IN_FILES, OnReplaceInFiles)
	ON_UPDATE_COMMAND_UI(ID_REPLACE_IN_FILES, OnUpdateReplaceInFiles)
	ON_COMMAND(ID_TREE_SEMANTICS, OnTreeSemantics)
	ON_UPDATE_COMMAND_UI(ID_TREE_SEMANTICS, OnUpdateTreeSemantics)
	ON_COMMAND(ID_VIEW_TREESEMANTICSMODE, OnViewTreesemanticsmode)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TREESEMANTICSMODE, OnUpdateViewTreesemanticsmode)
	ON_COMMAND(ID_VIEW_TREEOFFSETSMODE, OnViewTreeOffsetsmode)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TREEOFFSETSMODE, OnUpdateViewTreeOffsetsmode)
	ON_WM_COPYDATA()
	ON_COMMAND(ID_ATTRIBUTEEDITOR, OnAttributeeditor)
	ON_UPDATE_COMMAND_UI(ID_ATTRIBUTEEDITOR, OnUpdateAttributeeditor)
	ON_COMMAND(ID_TOOLS_ADDWORDS, OnToolsAddwords)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ADDWORDS, OnUpdateToolsAddwords)
	ON_COMMAND(ID_TOOLS_KBATTRIBUTES, OnToolsKbattributes)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_KBATTRIBUTES, OnUpdateToolsKbattributes)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OVR, OnUpdateInsert)
	ON_COMMAND(ID_CONTEXT_HELP, OnContextHelp)
	ON_MESSAGE(WMU_BRINGWINDOWTOTOP, OnBringWindowToTop)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_LINECOL, OnUpdateLineCol)
	ON_MESSAGE(WMU_DESTROY_ATTRIBUTEEDITOR, OnDestroyAttributeEditor)
	ON_COMMAND_RANGE(DUMP_MENU_STARTID, DUMP_MENU_ENDID, OnChooseDumpFile)
	ON_MESSAGE(WMU_SETACTIVEVIEW, OnSetActiveView)
	ON_COMMAND_RANGE(ID_TOOLS_DICTIONARY_URL_LOOKUP_BASE, ID_TOOLS_DICTIONARY_URL_LOOKUP_MAX, OnToolsDictionaryURLLookup)

	ON_COMMAND_EX_RANGE(ID_FILE_SAVEALL, ID_FILE_MRU_WORKSPACE, OnDefaultCommandRange)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_SAVEALL, ID_FILE_MRU_WORKSPACE, OnUpdateDefaultCommandRange)
	
	ON_MESSAGE(CPN_XTP_DROPDOWN, OnToolbarDropDown)
	ON_WM_SETFOCUS()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_VIEW_TREETEXTMODE, OnViewTreetextmode)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TREETEXTMODE, OnUpdateViewTreetextmode)
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)

	ON_UPDATE_COMMAND_UI(ID_URL_COMBOBOX, OnUpdateComboURL)
	ON_XTP_EXECUTE(ID_URL_COMBOBOX, OnUrlCombobox)

	ON_UPDATE_COMMAND_UI(ID_RECENT_COMBO, OnUpdateComboRecent)
	ON_XTP_EXECUTE(ID_RECENT_COMBO, OnRecentCombobox)

	//ON_COMMAND(ID_DUMP, OnDump)

	ON_COMMAND(ID_DUMP, OnDump)
	ON_UPDATE_COMMAND_UI(ID_DUMP, OnUpdateDump)
	ON_XTP_EXECUTE(ID_DUMP, OnDumpSelect)
	ON_NOTIFY(XTP_LBN_POPUP, ID_DUMP, OnListBoxControlPopup)

	//ON_UPDATE_COMMAND_UI(ID_DUMP_LIST, OnUpdateDump)
	//ON_NOTIFY(XTP_LBN_POPUP, ID_DUMP_LIST, OnListBoxControlDump)

	ON_XTP_CREATECONTROL()

	ON_COMMAND(ID_THEME_OFFICE2003, OnToolsThemeOffice2003theme)
	ON_UPDATE_COMMAND_UI(ID_THEME_OFFICE2003, OnUpdateToolsThemeOffice2003theme)
	ON_COMMAND(ID_THEME_VISUALSTUDIO2008, OnToolsThemeVisualStudio2008)
	ON_UPDATE_COMMAND_UI(ID_THEME_VISUALSTUDIO2008, OnUpdateToolsThemeVisualStudio2008)
	ON_COMMAND(ID_THEME_VISUALSTUDIO2010, OnToolsThemeVisualStudio2010)
	ON_UPDATE_COMMAND_UI(ID_THEME_VISUALSTUDIO2010, OnUpdateToolsThemeVisualStudio2010)

	ON_UPDATE_COMMAND_UI(ID_FILE_RECENTANALYZERS, &CMainFrame::OnUpdateFileRecentanalyzers)
	ON_WM_INITMENU()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ROWCOL, OnUpdateRowColIndicator)
	ON_COMMAND_RANGE(ID_VIEW_ANALYZERVIEW, ID_VIEW_GRAMMARVIEW, OnShowViewOther)
	END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_ROWCOL,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

static UINT uHideCmds[] = 
{
    ID_FILE_PRINT, ID_FILE_PRINT_PREVIEW, ID_WINDOW_CASCADE
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	_tsetlocale(LC_ALL, _T(""));
	InitVars();

    // Enable/Disable XP GUI Mode
    //xtAfxData.bXPMode = FALSE;

    // Enable/Disable Menu Shadows
    //xtAfxData.bMenuShadows = FALSE;

	// Enable/Disable Toolbar Visualizations
	//xtAfxData.bToolBarVisualize = FALSE;

	// Enable/Disable Toolbar Accelerator Tips.
	//xtAfxData.bToolBarAccelTips = TRUE;

    CXTPRegistryManager regMgr;
	m_nTabIndex     = regMgr.GetProfileInt(_T("Settings"), _T("m_nTabIndex"),     0);
	m_bDockingView  = regMgr.GetProfileInt(_T("Settings"), _T("m_bDockingView"),  TRUE);
	m_bTextView     = regMgr.GetProfileInt(_T("Settings"), _T("m_bTextView"),     TRUE);
	m_bGrammarView  = regMgr.GetProfileInt(_T("Settings"), _T("m_bGrammarView"),    TRUE);
	m_bAnalyzerView = regMgr.GetProfileInt(_T("Settings"), _T("m_bAnalyzerView"), TRUE);
	m_bTabbedView   = regMgr.GetProfileInt(_T("Settings"), _T("m_bTabbedView"),   FALSE);
}

CMainFrame::~CMainFrame()
{
	if (m_pFullScreenLayout)
		delete m_pFullScreenLayout;

    CXTPRegistryManager regMgr;
	regMgr.WriteProfileInt(_T("Settings"), _T("m_nTabIndex"),     m_nTabIndex );
	regMgr.WriteProfileInt(_T("Settings"), _T("m_bDockingView"),  m_bDockingView );
	regMgr.WriteProfileInt(_T("Settings"), _T("m_bTextView"),     m_bTextView    );
	regMgr.WriteProfileInt(_T("Settings"), _T("m_bGrammarView"),    m_bGrammarView   );
	regMgr.WriteProfileInt(_T("Settings"), _T("m_bAnalyzerView"), m_bAnalyzerView);
	regMgr.WriteProfileInt(_T("Settings"), _T("m_bTabbedView"),   m_bTabbedView  );
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Initialize the command bars
	if (!InitCommandBars())
        return -1;

    // Get a pointer to the command bars object.
    CXTPCommandBars* pCommandBars = GetCommandBars();
    if(pCommandBars == NULL)
    {
        TRACE0("Failed to create command bars object.\n");
        return -1;      // fail to create
    }

	appBoolNodeSemantics = true;

	// Add the menu bar

	/*
	m_menuBar = pCommandBars->SetMenu(_T("Menu Bar"), IDR_MAINFRAME);
	if (m_menuBar == NULL)
	{
		TRACE0("Failed to create menu bar.\n");
		return -1;      // fail to create
	}
	*/

	pCommandBars->GetImageManager()->SetIcons(IDR_BROWSER_TOOLBAR);
	pCommandBars->GetImageManager()->SetIcons(IDR_DEBUG_TOOLBAR);
	pCommandBars->GetImageManager()->SetIcons(IDR_KNOWLEDGE_TOOLBAR);
	pCommandBars->GetImageManager()->SetIcons(IDR_MAINFRAME_TOOLBAR);
	//pCommandBars->GetImageManager()->SetIcons(IDR_TAB_TOOLBAR);

	pCommandBars->GetCommandBarsOptions()->ShowKeyboardCues(xtpKeyboardCuesShowWindowsDefault);
	pCommandBars->GetToolTipContext()->SetStyle(xtpToolTipOffice);
	pCommandBars->GetShortcutManager()->SetAccelerators(IDR_MAINFRAME);

    // Set Office 2003 Theme
    //CXTPPaintManager::SetTheme(xtpThemeVisualStudio2008);

	CreatePaneWindows();
  
    // Initialize the docking pane manager and set the
    // initial them for the docking panes.  Do this only after all
    // control bars objects have been created and docked.
	VERIFY(m_paneManager.InstallDockingPanes(this));
	m_paneManager.SetTheme(xtpPaneThemeOffice2003);
	m_paneManager.UseSplitterTracker(FALSE);
	m_paneManager.SetShowContentsWhileDragging(TRUE);
	m_paneManager.SetAlphaDockingContext(TRUE);

	// Create docking panes.
	CXTPDockingPane* pwndPaneAnalzyer = m_paneManager.CreatePane(
		IDR_PANE_ANALYZER, CRect(0, 0,200, 120), xtpPaneDockLeft);
	CXTPDockingPane* pwndPaneGrammar = m_paneManager.CreatePane(
		IDR_PANE_GRAMMAR, CRect(0, 0,200, 120), xtpPaneDockLeft);
	CXTPDockingPane* pwndPaneText = m_paneManager.CreatePane(
		IDR_PANE_TEXT, CRect(0, 0,200, 120), xtpPaneDockLeft);

	m_paneManager.AttachPane(pwndPaneAnalzyer, pwndPaneText);
	m_paneManager.AttachPane(pwndPaneGrammar, pwndPaneText);

	int nIDs1[] = {IDR_PANE_GRAMMAR, IDR_PANE_ANALYZER, IDR_PANE_TEXT};
	m_paneManager.SetIcons(IDB_IL_TAB, nIDs1, 3, RGB(0, 255, 0));

	CXTPDockingPane* pwndPaneAnalyze = m_paneManager.CreatePane(
		IDR_PANE_ANALYZE, CRect(0, 0,200, 200), xtpPaneDockBottom);
	CXTPDockingPane* pwndPaneLog = m_paneManager.CreatePane(
		IDR_PANE_LOG, CRect(0, 0,200, 200), xtpPaneDockBottom);
	CXTPDockingPane* pwndPaneFind = m_paneManager.CreatePane(
		IDR_PANE_FIND, CRect(0, 0,200, 200), xtpPaneDockBottom);
	CXTPDockingPane* pwndPaneFind2 = m_paneManager.CreatePane(
		IDR_PANE_FIND2, CRect(0, 0,200, 200), xtpPaneDockBottom);

	m_paneManager.AttachPane(pwndPaneLog, pwndPaneAnalyze);
	m_paneManager.AttachPane(pwndPaneFind, pwndPaneLog);
	m_paneManager.AttachPane(pwndPaneFind2, pwndPaneFind);

	// initialize the toolbars.

	//////////////////////////////////
	// MAIN TOOLBAR
	//////////////////////////////////

	m_mainToolBar = (CXTPToolBar*)pCommandBars->Add(_T("Main"), xtpBarTop);
	if (!m_mainToolBar ||
		!m_mainToolBar->LoadToolBar(IDR_MAINFRAME_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	if (appBoolToolbarLabels)
		m_mainToolBar->GetControls()->Add(xtpControlLabel, 0, 0, 0)->SetCaption(_T("Main:"));

	//////////////////////////////////
	// BROWSER TOOLBAR
	//////////////////////////////////

	m_browserToolBar = (CXTPToolBar*)pCommandBars->Add(_T("Browser"), xtpBarTop);
	m_browserToolBar->GetControls()->Add(xtpControlLabel, 0)->SetCaption(_T("Browser:"));
	if (!m_browserToolBar ||
		!m_browserToolBar->LoadToolBar(IDR_BROWSER_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	if (appBoolToolbarLabels)
		m_browserToolBar->GetControls()->Add(xtpControlLabel, 0, 0, 0)->SetCaption(_T("Browser:"));
	DockRightOf(m_browserToolBar, m_mainToolBar);

	//////////////////////////////////
	// TAB TOOLBAR
	//////////////////////////////////
	/*
	CXTPToolBar* pTabToolBarButtons = (CXTPToolBar*)pCommandBars->Add(_T("Tabs"), xtpBarTop);
	pTabToolBarButtons->GetControls()->Add(xtpControlLabel, 0)->SetCaption(_T("Tabs:"));
	if (!pTabToolBarButtons ||
		!pTabToolBarButtons->LoadToolBar(IDR_TAB_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	pTabToolBarButtons->GetControls()->Add(xtpControlLabel, 0, 0, 0)->SetCaption(_T("Tab:"));
	*/

	//////////////////////////////////
	// DEBUG TOOLBAR
	//////////////////////////////////

	m_debugToolBar = (CXTPToolBar*)pCommandBars->Add(_T("Debug"), xtpBarTop);
	if (!m_debugToolBar ||
		!m_debugToolBar->LoadToolBar(IDR_DEBUG_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	if (appBoolToolbarLabels)
		m_debugToolBar->GetControls()->Add(xtpControlLabel, 0, 0, 0)->SetCaption(_T("Debug:"));
	//DockRightOf(m_debugToolBar, pTabToolBarButtons);

	//////////////////////////////////
	// KNOWLEDGE TOOLBAR
	//////////////////////////////////

	m_knowledgeToolBar = (CXTPToolBar*)pCommandBars->Add(_T("Knowledge"), xtpBarTop);
	if (!m_knowledgeToolBar ||
		!m_knowledgeToolBar->LoadToolBar(IDR_KNOWLEDGE_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	if (appBoolToolbarLabels)
		m_knowledgeToolBar->GetControls()->Add(xtpControlLabel, 0, 0, 0)->SetCaption(_T("Knowledge:"));
	DockRightOf(m_knowledgeToolBar, m_debugToolBar);

	// Load the previous state for docking panes.
	CXTPDockingPaneLayout layoutNormal(&m_paneManager);
	if (layoutNormal.Load(_T("NormalLayout")))
	{
		m_paneManager.SetLayout(&layoutNormal);
	}

	m_paneManager.SetAlphaDockingContext(TRUE);
	m_paneManager.SetShowDockingContextStickers(TRUE);
	m_paneManager.SetShowContentsWhileDragging(TRUE);
	m_paneManager.SetDefaultPaneOptions(xtpPaneHasMenuButton);
	m_paneManager.SetThemedFloatingFrames(FALSE);

	// Load the previous state for toolbars and menus.
    LoadCommandBars(_T("CommandBars"));

	SetTabbedWindows(appBoolTabbedWindows);

	// Initialize row, col indicator.
	m_wndStatusBar.SetPaneInfo(m_wndStatusBar.CommandToIndex(
		ID_INDICATOR_ROWCOL), ID_INDICATOR_ROWCOL, SBPS_NORMAL, 75);

	SetRowCol(0,0);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnClose() 
{
	switch (appState) {
		case VT_APP_ANALYZING:
		case VT_APP_DIR_ANALYZING:
		{
			AddDebugLine(_T("User interruption"), _T("Abort analysis"));
			m_wndAnalyzerView.EndAnalyzeThread();
			break;
		}
	}

	m_bTabbedView = ( theApp.m_pChildFrame != NULL );
	m_wndPosition.SaveWindowPos(this);

	appState = VT_APP_EXITING;

	bool analyzerJustStarted = !appBoolAnalyzerLoaded && kbDirty;
	
	//fix for crash on exit bug PD 02/14/2001
	AfxGetThreadState()->m_pRoutingFrame = NULL;

	if (!analyzerJustStarted && appBoolAnalyzerLoaded && AnalyzerCreated())
		theApp.CloseAnalyzer();
	else if (analyzerJustStarted && AnalyzerCreated())
		theApp.CloseAnalyzer();

	CXTPDockingPaneLayout layoutNormal(&m_paneManager);
	m_paneManager.GetLayout(&layoutNormal);
	layoutNormal.Save(_T("NormalLayout"));

	SaveCommandBars(_T("CommandBars"));

	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	static HWND hFocus = 0;

	if (nState == WA_INACTIVE)
	{
		hFocus = m_paneManager.GetActivePane() != NULL ? ::GetFocus() : NULL;
	}
	
	CMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	if (nState != WA_INACTIVE && hFocus)
	{
		::SetFocus(hFocus);		
		hFocus = NULL;		
	}
}

BOOL CMainFrame::ShowWindowEx(int nCmdShow)
{
	ASSERT_VALID(this);

	// Restore frame window size and position.
	m_wndPosition.LoadWindowPos(this);
	nCmdShow = m_wndPosition.showCmd;

	if ( !m_bDockingView && m_bTabbedView )
	{
		theApp.m_pDocWorkspTemplate->OpenDocumentFile(NULL);
	}

	return ShowWindow(nCmdShow);
}

void CMainFrame::ExchangeLayout(CXTPPropExchange* pPX, BOOL bSerializeControls /*= TRUE*/)
{
	XTP_COMMANDBARS_PROPEXCHANGE_PARAM param; param.bSerializeControls = bSerializeControls; 

	CXTPPropExchangeSection pxCommandBars(pPX->GetSection(_T("CommandBars")));
	GetCommandBars()->DoPropExchange(&pxCommandBars, &param);

	CXTPPropExchangeSection secShortcuts(pPX->GetSection(_T("Shortcuts")));
	GetCommandBars()->GetShortcutManager()->DoPropExchange(&secShortcuts);

	CXTPPropExchangeSection secOptions(pPX->GetSection(_T("Options")));	
	GetCommandBars()->GetCommandBarsOptions()->DoPropExchange(&secOptions, TRUE);

	CXTPPropExchangeSection pxDockingPane(pPX->GetSection(_T("DockingPane")));	
	m_paneManager.DoPropExchange(&pxDockingPane);
}

LRESULT CMainFrame::OnToolbarDropDown(WPARAM wParam, LPARAM lParam)
{
	/*
	NMTOOLBAR* pNMTB = ( NMTOOLBAR* )wParam;

	if ( pNMTB->iItem == ID_WIZBAR_ACTIONS )
	{
		// construct a pointer to the calling toolbar.
		CXTPToolBar* pToolBar = DYNAMIC_DOWNCAST( CXTPToolBar, CWnd::FromHandle( pNMTB->hdr.hwndFrom ) );
		ASSERT_VALID( pToolBar );

		CRect rect = (CRect*)lParam;
		
		// load and display popup menu
		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_POPUP_WIZARD));
		
		CMenu* pPopup = menu.GetSubMenu(0);

		::SetMenuDefaultItem(pPopup->GetSafeHmenu(),
			ID_WIZBAR_GOTOFUNCTIONDECLARATION, FALSE);

		pToolBar->TrackMenuPopup(pPopup, rect);
		pToolBar->Invalidate();

		return 1; // handled.
	}
	*/

	return 0;
}

BOOL CMainFrame::OnDefaultCommandRange(UINT nID)
{
	UNREFERENCED_PARAMETER(nID);
	return TRUE;
}

void CMainFrame::OnUpdateDefaultCommandRange(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void CMainFrame::OnEditUndo() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnEditRedo() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnViewWorkspace() 
{
	/*
	if ( m_bDockingView )
	{
		ShowControlBar( &m_wndWorkspaceBar,
			!m_wndWorkspaceBar.IsWindowVisible( ), FALSE );
	}
	else
	{
		if ( theApp.m_pChildFrame != NULL )
		{
			m_bTabbedView = FALSE;
			theApp.m_pChildFrame->SendMessage(WM_CLOSE);
		}
		else
		{
			m_bTabbedView = TRUE;
			theApp.m_pDocWorkspTemplate->OpenDocumentFile(NULL);
		}
	}
	*/
}
/*
void CMainFrame::OnUpdateViewWorkspace(CCmdUI* pCmdUI) 
{
	if ( m_bDockingView )
	{
		pCmdUI->SetCheck( m_wndWorkspaceBar.IsWindowVisible( ) );
	}
	else
	{
		pCmdUI->SetCheck( theApp.m_pChildFrame != NULL );
	}
}

void CMainFrame::OnViewOutput() 
{
	ShowControlBar( &m_wndOutputBar,
		!m_wndOutputBar.IsVisible(), FALSE);
}

void CMainFrame::OnUpdateViewOutput(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_wndOutputBar.IsVisible( ) );
}

void CMainFrame::OnWindowDockingview() 
{
	m_bDockingView = !m_bDockingView;
	
	if (!m_bDockingView)
	{
		ShowControlBar(&m_wndWorkspaceBar, FALSE, FALSE);

		// Get the last selected index and open tabbed view.
		m_nTabIndex = m_wndWorkspaceBar.GetTabCtrl().GetCurSel();
		theApp.m_pDocWorkspTemplate->OpenDocumentFile(NULL);
	}
	else
	{
		theApp.m_pChildFrame->SendMessage(WM_CLOSE);
		
		// Show the tab control bar and update the tab display.
		ShowControlBar(&m_wndWorkspaceBar, TRUE, FALSE);
		m_wndWorkspaceBar.RefreshTabDisplay();
	}
}

void CMainFrame::OnUpdateWindowDockingview(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bDockingView);
}

void CMainFrame::HideShowTabView(BOOL& bTabView, CWnd* pTree, LPCTSTR lpszText, int iIndex, int iIconIndex)
{
	logmsg(_T("Enter HideShowTabView"));
	bTabView = !bTabView;
	if ( m_bDockingView )
	{
		if ( bTabView )
		{
			m_wndWorkspaceBar.AddControl( lpszText, pTree, iIndex, iIconIndex );
			m_nTabIndex = iIndex;
		}
		else
		{
			m_wndWorkspaceBar.DeleteView( pTree, FALSE );
		}
	}
	else
	{
		if ( theApp.m_pChildFrame != NULL )
		{
			CWorkspaceView* pView = (CWorkspaceView*)theApp.m_pChildFrame->GetActiveView();
			ASSERT_KINDOF(CWorkspaceView, pView);
			
			if ( pView )
			{
				if ( bTabView )
				{
					pView->AddControl( lpszText, pTree, iIndex, iIconIndex );
					m_nTabIndex = iIndex;
				}
				else
				{
					pView->DeleteView( pTree, FALSE );
				}
			}
		}
	}
}

void CMainFrame::OnTabGrammarView() 
{
	HideShowTabView(m_bGrammarView, &CTreeViewGrammar::Get( ), TAB_GRAM, 0, 0);
}

void CMainFrame::OnUpdateTabGrammarView(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bGrammarView);
	pCmdUI->Enable(m_bTextView || m_bAnalyzerView);
}

void CMainFrame::OnTabAnalyzerView() 
{
	HideShowTabView(m_bAnalyzerView, &CTreeViewAnalyzer::Get( ), TAB_ANA, 1, 1);
}

void CMainFrame::OnUpdateTabAnalyzerView(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bAnalyzerView);
	pCmdUI->Enable(m_bTextView || m_bGrammarView);
}

void CMainFrame::OnTabTextView() 
{
	int iIndex = 0;
	if ( m_bDockingView )
	{
		iIndex = m_wndWorkspaceBar.GetTabCtrl().GetItemCount();
	}
	else
	{
		CWorkspaceView* pView = (CWorkspaceView*)theApp.m_pChildFrame->GetActiveView();
		ASSERT_KINDOF(CWorkspaceView, pView);
		
		if ( pView )
		{
			iIndex = pView->GetTabCtrl().GetItemCount();
		}
	}

	HideShowTabView(m_bTextView, &CTreeViewText::Get( ), TAB_TEXT, iIndex, 2);
}
*/

int CMainFrame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{
	if (lpCreateControl->bToolBar)
	{
		CXTPToolBar* pToolBar = DYNAMIC_DOWNCAST(CXTPToolBar, lpCreateControl->pCommandBar);
		if (!pToolBar)
			return FALSE;

		if (lpCreateControl->nID == ID_URL_COMBOBOX && pToolBar->GetBarID() == IDR_BROWSER_TOOLBAR)
		{
			CXTPControlComboBox* pComboBoxURL = (CXTPControlComboBox*)CXTPControlComboBox::CreateObject();
			pComboBoxURL->SetWidth(350);
			pComboBoxURL->SetDropDownWidth(350);
			pComboBoxURL->SetDropDownListStyle();
			pComboBoxURL->SetFlags(xtpFlagManualUpdate);
			lpCreateControl->pControl = pComboBoxURL;
			return TRUE;
		}

		if (lpCreateControl->nID == ID_DUMP && pToolBar->GetBarID() == IDR_DEBUG_TOOLBAR)
		{
			CXTPControlPopup* pButtonDump = CXTPControlPopup::CreateControlPopup(xtpControlSplitButtonPopup);

			CXTPPopupToolBar* pDumpBar = CXTPPopupToolBar::CreatePopupToolBar(GetCommandBars());
			pDumpBar->EnableCustomization(FALSE);
			pDumpBar->SetBorders(CRect(2, 2, 2, 2));
			pDumpBar->DisableShadow();

			CXTPControlListBox* pControlListBox = (CXTPControlListBox*)pDumpBar->GetControls()->Add(new CXTPControlListBox(), ID_DUMP);
			//CXTPControlListBox* pControlListBox = (CXTPControlListBox*)pDumpBar->GetControls()->Add(m_listBoxDump, ID_DUMP);
			pControlListBox->SetWidth(140);

			pButtonDump->SetCommandBar(pDumpBar);
			pDumpBar->InternalRelease();

			lpCreateControl->pControl = pButtonDump;
			return TRUE;
		}
	}

	return FALSE;
}

void CMainFrame::OnUpdateTabTextView(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTextView);
	pCmdUI->Enable(m_bAnalyzerView || m_bGrammarView);
}

void CMainFrame::AddAnalyzerLine(CString firstColumn, CString secondColumn, bool showPaneFlag) 
{
	int last = m_wndAnalyzePane.m_listCtrl.GetItemCount();
	m_wndAnalyzePane.m_listCtrl.InsertItem(last, NULL, 0);
	m_wndAnalyzePane.m_listCtrl.SetItemText(last,0,firstColumn);
	m_wndAnalyzePane.m_listCtrl.SetItemText(last,1,secondColumn);
	if (showPaneFlag) m_paneManager.ShowPane(IDR_PANE_ANALYZE);
}

void CMainFrame::AddDebugLine(CString firstColumn, CString secondColumn, bool showPaneFlag) 
{
	int last = m_wndLogPane.m_listCtrl.GetItemCount();
	m_wndLogPane.m_listCtrl.InsertItem(last, NULL, 0);
	m_wndLogPane.m_listCtrl.SetItemText(last,0,firstColumn);
	m_wndLogPane.m_listCtrl.SetItemText(last,1,secondColumn);
	m_wndLogPane.RedrawWindow();
	if (showPaneFlag) m_paneManager.ShowPane(IDR_PANE_LOG);
	m_wndLogPane.m_listCtrl.EnsureVisible(last, false);
}

void CMainFrame::AddFindLine(CString firstColumn, CString secondColumn, bool showPaneFlag) 
{
	int last = m_wndFindPane.m_listCtrl.GetItemCount();
	m_wndFindPane.m_listCtrl.InsertItem(last, NULL, 0);
	m_wndFindPane.m_listCtrl.SetItemText(last,0,firstColumn);
	m_wndFindPane.m_listCtrl.SetItemText(last,1,secondColumn);
	if (showPaneFlag) m_paneManager.ShowPane(IDR_PANE_FIND);
	m_wndFindPane.m_listCtrl.EnsureVisible(last, false);
}

void CMainFrame::AddFindLine2(CString firstColumn, CString secondColumn, bool showPaneFlag) 
{
	int last = m_wndFindPane2.m_listCtrl.GetItemCount();
	m_wndFindPane2.m_listCtrl.InsertItem(last, NULL, 0);
	m_wndFindPane2.m_listCtrl.SetItemText(last,0,firstColumn);
	m_wndFindPane2.m_listCtrl.SetItemText(last,1,secondColumn);
	if (showPaneFlag) m_paneManager.ShowPane(IDR_PANE_FIND2);
	m_wndFindPane2.m_listCtrl.EnsureVisible(last, false);
}

void CMainFrame::AddFindLineHead(CString firstColumn, CString secondColumn, bool showPaneFlag) 
{
	m_wndFindPane.m_listCtrl.InsertItem(0, NULL, 0);
	m_wndFindPane.m_listCtrl.SetItemText(0,0,firstColumn);
	m_wndFindPane.m_listCtrl.SetItemText(0,1,secondColumn);
	if (showPaneFlag) m_paneManager.ShowPane(IDR_PANE_FIND);
}

void CMainFrame::AddFindLineHead2(CString firstColumn, CString secondColumn, bool showPaneFlag) 
{
	m_wndFindPane2.m_listCtrl.InsertItem(0, NULL, 0);
	m_wndFindPane2.m_listCtrl.SetItemText(0,0,firstColumn);
	m_wndFindPane2.m_listCtrl.SetItemText(0,1,secondColumn);
	if (showPaneFlag) m_paneManager.ShowPane(IDR_PANE_FIND2);
}

void CMainFrame::ClearOutput()
{
	SetRedraw(false);
	m_wndAnalyzePane.m_listCtrl.DeleteAllItems();
	SetRedraw(true);
}

void CMainFrame::ClearFind()
{
	SetRedraw(false);
	m_wndFindPane.m_listCtrl.DeleteAllItems();
	SetRedraw(true);
}

void CMainFrame::ClearFind2()
{
	SetRedraw(false);
	m_wndFindPane2.m_listCtrl.DeleteAllItems();
	SetRedraw(true);
}

CString CMainFrame::GetProcessText()
{
	return m_strProcessText;
}

CString CMainFrame::GetURLText()
{
	return m_strURLText;
}

CString CMainFrame::GetProcessTextFile(CString subFilePathStr)
{
	if (m_strProcessText == _T(""))
		return _T("");

	CString returnStr = InputPath(m_strProcessText) + _T("_log");
	if (subFilePathStr != _T("")) returnStr += _T("\\") + subFilePathStr;
	return returnStr;
}

void CMainFrame::AddComboText(CString textStr, bool select) 
{
	if (textStr.Find(_T("_log\\")) > 0) {
		textStr = StripPath(textStr,0,true);
		ClipRight(textStr,4);
	}

	// SET PROCESSED TEXT VARIABLE ONLY IF THE FILE EXISTS!
	CTreeViewText *view = &(theApp.m_pMainFrame->m_wndTextView);
	if (select && view->SelectTextFromPath(textStr))
		m_strProcessText = textStr;
	else if (!select)
		m_strProcessText = textStr;
	else
		m_strProcessText = _T("");

	m_textTreeType = view->GetTextTreeType();
}

void CMainFrame::Run()
{
	if (m_strProcessText != _T("")) {
		ClearOutput();
		RunAnalyzer(m_strProcessText);
	}
	else {
		CString msgStr = _T("No directory or file chosen to analyze.");
		AfxMessageBox(msgStr);
	}
}

void CMainFrame::ServerArchive(CString archiveTypeStr)
{
	ClearOutput();

	StartWaitCursor();
	GetServerArchiveDirectoryString(archiveTypeStr);
	StopWaitCursor();

	if (appAskText != _T("")) {
		CArchiveListDialog archiveDlg;
		archiveDlg.SetArchiveType(archiveTypeStr);
		archiveDlg.SetDestinationType(ARCHIVE_DEST_SERVER);
		archiveDlg.DoModal();
	}
}

void CMainFrame::LocalArchive(CString archiveTypeStr)
{
	ClearOutput();

	appAskText = _T("");
	CArchiveListDialog archiveDlg;
	archiveDlg.SetArchiveType(archiveTypeStr);
	archiveDlg.SetDestinationType(ARCHIVE_DEST_LOCAL);
	archiveDlg.DoModal();
}

bool CMainFrame::AnalyzerCreated() 
{
	if (!appBoolAnalyzerLoaded) {
		CString msgStr = _T("No analyzer project has been created.\n\nDo you want to create one?");
		if (AfxMessageBox(msgStr, MB_YESNO) == IDYES)
			theApp.CreateNewAnalyzer();
		else
			return false;
	}
	return true;
}

CONCEPT *CMainFrame::GrammarConceptSelected(bool safeFlag)
{
	if (safeFlag)
		return m_wndGrammarView.SafeConceptSelected();
	return m_wndGrammarView.ConceptSelected();
}

WORKSPACE_TAB CMainFrame::GetActiveTab()
{
	if (m_wndAnalyzerView.GetFocus()) return WORKSPACE_ANALYZER;
	if (m_wndGrammarView.GetFocus()) return WORKSPACE_GRAMMAR;

	return WORKSPACE_TEXT;
}

void CMainFrame::EnableToolBarButton(int nIDToolbar, int nIDItem, int value)
{
	/*
	if (nIDToolbar == IDR_BROWSER_TOOLBAR)
		m_browserToolBar->GetToolBarCtrl().EnableButton(nIDItem,value);
	else if (nIDToolbar == IDR_KNOWLEDGE_TOOLBAR)
		m_knowledgeToolBar->GetToolBarCtrl().EnableButton(nIDItem,value);
	else if (nIDToolbar == IDR_DEBUG_TOOLBAR)
		m_debugToolBar->GetToolBarCtrl().EnableButton(nIDItem,value);
	else if (nIDToolbar == IDR_TAB_TOOLBAR)
		m_tabToolBar->GetToolBarCtrl().EnableButton(nIDItem,value);
		*/
}

void CMainFrame::SelectTab(int tabID)
{
	if (m_paneManager.IsPaneSelected(tabID))
		return;
}

bool CMainFrame::TextHasAnaLogFiles(CString fullPathStr)
{
	if (fullPathStr == _T(""))
		fullPathStr = InputPath(m_strProcessText);
	return m_wndTextView.HasAnaLogFiles(fullPathStr);
}

void CMainFrame::ReIconKB(CONCEPT *concept) 
{
	if (concept) {
		CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
		if (pView)
			pView->ReIcon(concept);
	}
}

bool CMainFrame::IsSelectedTextOpen() 
{
	return theApp.IsVisualTextDocumentOpen(_T("VisualText"),InputPath(m_strProcessText)) ? true : false;
}

void CMainFrame::HighlightMatches(int pass) 
{
	m_wndAnalyzerView.HighlightMatches(pass);
}

bool CMainFrame::GetLogPath(CString &logPathStr, bool dontAsk)
{
	return m_wndAnalyzerView.GetLogFile(logPathStr,dontAsk);
}

bool CMainFrame::GetLogPath(int pass, CString &logPathStr, bool dontAsk)
{
	return m_wndAnalyzerView.GetLogFile(pass,logPathStr,
		false,	// Seems missing FORCE arg.	// 11/10/03 AM.
		dontAsk);
}

void CMainFrame::OpenParseTree(CString conceptStr, CString logPathStr, CString textPathStr) 
{
	if (logPathStr != _T("")) {
		if (textPathStr == _T(""))
			textPathStr = GetProcessText();

		if (FileExists(InputPath(textPathStr))) {
			StartWaitCursor();
			CParseTreeDoc *doc = (CParseTreeDoc *)theApp.OpenNewDocument(_T("ParseTree"),logPathStr);
			if (doc) {
				CParseTreeView *view = (CParseTreeView *)GetFirstView(doc);
				if (view->SetData(logPathStr,textPathStr,conceptStr))
					view->ExpandOneLevel();
				else
					view->DestroyWindow();
			}
			StopWaitCursor();
		}
	}
}

void CMainFrame::OpenParseTree(CString conceptStr, CString logPathStr, CStringList &selList) 
{
	long start;
	long end;

	POSITION pos = selList.GetHeadPosition();
	CString selStr = _T("");
	CParseTreeDoc *doc = (CParseTreeDoc *)theApp.OpenNewDocument(_T("ParseTree"),logPathStr);
	if (!doc) return;
	
	CParseTreeView *view = (CParseTreeView *)GetFirstView(doc);

	while (pos && (selStr = selList.GetNext(pos))) {
		NthPathSegmentLong(selStr, start,',',0);
		NthPathSegmentLong(selStr, end,',',1);
		view->SetData(logPathStr,m_strProcessText,conceptStr,start,end,true);
	}

	if (selList.GetCount())
		view->ExpandOneLevel();
}

void CMainFrame::OpenParseTree(long start, long end) 
{
	CString logPathStr;
	GetLogPath(logPathStr);

	if (m_strProcessText != _T("") && logPathStr != _T("") &&
		start >= 0 && end >= 0  && start <= end) {
		CString conceptStr = GetGrammarConceptName();

		CParseTreeDoc *doc = (CParseTreeDoc *)theApp.OpenNewDocument(_T("ParseTree"),logPathStr);
		if (doc) {
			CParseTreeView *view = (CParseTreeView *)GetFirstView(doc);
			if (view->SetData(logPathStr,m_strProcessText,conceptStr,start,end))
				view->ExpandOneLevel();
			else
				view->GetParentFrame()->DestroyWindow();
		}
	}
}

void CMainFrame::OpenParseTree(bool conceptsOnly) 
{
	CString logPathStr;
	GetLogPath(logPathStr);

	if (m_strProcessText != _T("") && logPathStr != _T("")) {
		CString conceptStr = GetGrammarConceptName();

		CParseTreeDoc *doc = (CParseTreeDoc *)theApp.OpenNewDocument(_T("ParseTree"),logPathStr);
		if (doc) {
			CParseTreeView *view = (CParseTreeView *)GetFirstView(doc);
			view->SetConceptOnlyFlag(conceptsOnly);
			if (view->SetData(logPathStr,m_strProcessText,conceptStr,-1,-1))
				view->ExpandOneLevel();
			else
				view->GetParentFrame()->DestroyWindow();
		}
	}
}

void CMainFrame::FindWord(CString selStr, CVisualTextDoc *doc, bool noCase,
						  CString dirPathStr) 
{
	if (selStr == _T(""))
		selStr = appStrFind;
	else
		appStrFind = selStr;

	if (appStrFind != _T("")) {
		StartWaitCursor();
		EnsureFindWindowVisible();

		if (g_bFind2)
			ClearFind2();
		else
			ClearFind();

		int total = 0;

		total += FindWordInFile(InputPath(m_strProcessText),_T(""),appStrFind,noCase);
		total += FindWordInGrammar(appStrFind,noCase);
		total += FindWordInRuleFiles(appStrFind,_T(""),noCase);
		total += FindWordInOutputFiles(appStrFind,_T(".txt"),noCase);
		total += FindWordInOutputFiles(appStrFind,_T(".log"),noCase);
		//total += FindWordInTextFiles(appStrFind,noCase);

		CString msgStr;
		msgStr.Format(_T("Found %d:"),total);
		AddFindLineHead(msgStr,appStrFind);

		StopWaitCursor();
	}
	else {
		CString msgStr = _T("You must enter or select a word for this option");
		AfxMessageBox(msgStr);
	}
}

int CMainFrame::FindWordInFile(CString dirPathStr, CString fileName, CString selStr,
							   bool noCase)
{
	int lineNum = 0;
	int count = 0;
	int inLineIndex = -1;	// FIX. So it will look from zeroth char of each line. // 03/20/06 AM.
	CString lineStr;
	CString fileStr;
	CString fullPathStr = dirPathStr;
	if (fileName != _T(""))
		fullPathStr += _T("\\") + fileName;
	bool multi = false;	// Flag if finding multiple patterns per line.           // 03/20/06 AM.

	if (g_bFind2)
		m_paneManager.ShowPane(IDR_PANE_FIND2);
	else
		m_paneManager.ShowPane(IDR_PANE_FIND);

	while ((lineNum = FileContains(fullPathStr,selStr,lineStr,lineNum,inLineIndex,noCase,multi))) {
		lineStr.TrimLeft(_T("\r\n\t "));
		lineStr.TrimRight(_T("\r\n\t "));
		lineStr.Replace(_T("\t"),_T("   "));
		fileStr.Format(_T("File: %s [%d %d]:"),RelativizePath(fullPathStr),lineNum, inLineIndex);

		// ADD PASS NUMBER FOR ANALYZER PASS
		if (EndsWith(fullPathStr,_T("pat"))) {
			CString str = fileName;
			ClipRight(str,4);
			int pass = m_wndAnalyzerView.PassFromFile(str);
			CString newLineStr;
			str.Format(_T("[pass %d] %s"),++pass,lineStr);
			lineStr = str;
		}

		if (g_bFind2)
			AddFindLine2(fileStr,lineStr);
		else
			AddFindLine(fileStr,lineStr);
		count++;
		PeekHere();
	}

	if (g_bFind2)
		m_wndFindPane2.m_listCtrl.AutoSizeColumn();
	else
		m_wndFindPane.m_listCtrl.AutoSizeColumn();

	return count;
}

void CMainFrame::OpenBrowserDictLookup(LPCTSTR lpURL, LPCTSTR lpWord)
{
	CString strWord = lpWord;
	strWord.TrimLeft();
	strWord.TrimRight();
	if (strWord.IsEmpty()) {
		AddDebugLine(_T("FAIL: Dictionary Lookup"),_T("Word is blank"),true);
		return;
	}
	int pos = strWord.FindOneOf(_T("\r\n\t "));
	if (pos != -1)
		strWord = strWord.Left(pos);
	CBrowserView *view = NULL;
	CString strURL;
	strURL.Format(_T("%s%s"), lpURL, strWord);
	if (!m_pDocBrowserDictLookup) {
		m_pDocBrowserDictLookup = (CBrowserDoc*)theApp.OpenNewDocument(_T("Browser"),strURL);
		if (m_pDocBrowserDictLookup)
			view = (CBrowserView*)GetFirstView(m_pDocBrowserDictLookup);
  	}
	else {
		view = (CBrowserView *)GetFirstView(m_pDocBrowserDictLookup);
		if (view)
			view->Navigate(strURL);
	}
	if (view)
		SendMessage(WMU_SETACTIVEVIEW, 0, (LPARAM)(LPVOID)view);
}

void CMainFrame::UpdateUrlPulldown(CString urlStr) 
{
	CXTPControlComboBox* comboBoxURL = (CXTPControlComboBox*)m_browserToolBar->GetControl(3);

	if (urlStr.Find(_T("about:blank"),0) >= 0) {
		int count = comboBoxURL->GetCount();
		int foundIndex = comboBoxURL->FindStringExact(-1,urlStr);
		comboBoxURL->DeleteItem(foundIndex);

	} else if (urlStr != _T("") && urlStr.Find(_T("dnserror"),0) < 0) {
		int count = comboBoxURL->GetCount();
		int foundIndex = comboBoxURL->FindStringExact(-1,urlStr);

		if (!count || foundIndex == CB_ERR)
			comboBoxURL->AddString(urlStr);
		comboBoxURL->SetEditText(urlStr);
		//m_comboBoxURL->SetWindowText(urlStr);
		appURL = urlStr;
	}
}

void CMainFrame::BrowserNavigate(CString urlStr, bool newWindow, CBrowserDoc *docIn) 
{
	if (!IsInternetConnected()) {
		AfxMessageBox(_T("No internet connection detected."));
		return;
	}

	CXTPControlComboBox* comboBoxURL = (CXTPControlComboBox*)m_browserToolBar->GetControl(3);

	appURL = urlStr;
	if (urlStr == _T(""))
		appURL = comboBoxURL->GetEditText();

	if (appURL != _T("")) {
		CBrowserDoc *doc = docIn;
		if (!doc)
			doc = (CBrowserDoc *)theApp.IsBrowserDocumentOpen(appURL);

		CMDIChildWnd *child = MDIGetActive();
		if (!newWindow && child) {
			CBrowserDoc *docActive = (CBrowserDoc *)child->GetActiveDocument();
			CString docTypeStr = theApp.GetDocumentTypeString(docActive);

			if (docTypeStr == _T("Browser")) {
				CBrowserView *view = (CBrowserView *)GetFirstView(docActive);
				view->Navigate(appURL);
				return;
			}
		}

		if (!newWindow && doc) {
			CBrowserView *view = (CBrowserView *)GetFirstView(doc);
			view->GetParentFrame()->BringWindowToTop();
		}
		else
			theApp.OpenNewDocument(_T("Browser"),appURL);
	}
}

void CMainFrame::RunAnalyzer(CString textPathStr)
{
	CString msgStr = _T("");
	msgStr.Format(_T("\"%s\" is a directory.\nDo you want to run the analyzer on the entire folder?"),
		textPathStr);

	if (IsDirectory(InputPath(textPathStr)) && AfxMessageBox(msgStr, MB_YESNO) == IDNO)
		return;

	OnSaveAll();

	m_wndAnalyzerView.RunAnalyzer(InputPath(textPathStr), 0);
}

void CMainFrame::OnSaveAll() 
{
	theApp.DoAllViews(NULL,_T("SyntaxEditor"),_T(".pat"),DOC_ACTION_SAVE);
	theApp.DoAllViews(NULL,_T("VisualText"),_T(".txt"),DOC_ACTION_SAVE);
	theApp.DoAllViews(NULL,_T("VisualText"),_T(".htm"),DOC_ACTION_SAVE);  // 07/15/06 AM.
	theApp.DoAllViews(NULL,_T("VisualText"),_T(".html"),DOC_ACTION_SAVE); // 07/15/06 AM.
	theApp.DoAllViews(NULL,_T("VisualText"),_T(".xml"),DOC_ACTION_SAVE);  // 07/15/06 AM.
	theApp.DoAllViews(NULL,_T("VisualText"),_T(".pos"),DOC_ACTION_SAVE);  // 07/15/06 AM.
	appBoolDocDirty = theApp.DirtyDocs(appDocFocus);
	appBoolDocsDirty = theApp.DirtyDocs();
	OnUpdateFrameTitle(false);
}

void CMainFrame::SetProcessText(CString processText)
{
	m_strProcessText = processText;
	//CTreeViewText *view = (CTreeViewText *)m_wndWorkspaceBar.GetView(WORKSPACE_TEXT);
	//m_textTreeType = view->GetTextTreeType();
}

void CMainFrame::SetAnalyzingToolbarButtons(bool value) 
{
	EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_GEN_ANALYZER,value);
	EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_STOP_PROCESS,!value);
	EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_GEN_DIRTY_AND_RUN,value);
	EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_RUN,value);
	EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_RUN_NEXT,value);
	EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_GENERATELOGS,value);
	EnableToolBarButton(IDR_KNOWLEDGE_TOOLBAR,ID_VERBOSE_MODE,value);
}

void CMainFrame::SetSpideringToolbarButtons(bool value) 
{
	EnableToolBarButton(IDR_BROWSER_TOOLBAR,ID_ADD_URL,value);
	EnableToolBarButton(IDR_BROWSER_TOOLBAR,ID_NEW_BROWSER,value);
	EnableToolBarButton(IDR_BROWSER_TOOLBAR,ID_RUN_HTML,value);
	EnableToolBarButton(IDR_BROWSER_TOOLBAR,ID_SPIDER,value);
	EnableToolBarButton(IDR_BROWSER_TOOLBAR,ID_URL_STOP,!value);
	EnableToolBarButton(IDR_BROWSER_TOOLBAR,ID_BACK_URL,value);
	EnableToolBarButton(IDR_BROWSER_TOOLBAR,ID_FORWARD_URL,value);
	EnableToolBarButton(IDR_BROWSER_TOOLBAR,ID_CLEAR_URL,value);
}

CAttributeEditor *CMainFrame::GetAttributeDlg()
{
	return (CAttributeEditor *)m_pAttributeEditor;
}

int CMainFrame::FindWordInOutputFiles(CString selStr, CString extStr, bool noCase)
{
	CString logPathStr;
	GetLogPath(logPathStr,true);
	if (logPathStr != _T("")) {
		logPathStr = StripPath(logPathStr,0,true);
		return FindWordInDirectoryFiles(selStr,logPathStr,extStr,noCase);
	}
	return false;
}

// Now will use the analyzer sequence in the KB to order based on pass files.	// 06/14/03 AM.
// For the current analyzer only.	// 06/14/03 AM.
// Note this looks in sequence only, NOT at all files in current spec folder.	// 06/14/03 AM.
int CMainFrame::FindWordInRuleFiles(CString selStr, CString dirPathStr, bool noCase) 
{
	int total = 0;
	if (selStr != _T("")) {
		CString dirFindPathStr;
		if (dirPathStr == _T(""))
			dirFindPathStr = RulesPath();
		total += FindWordInSequenceFiles(selStr,dirFindPathStr,_T("pat"),noCase); // SRP 010705
	}

	return total;
}

// This version is for other analyzers (and also for current analyzer).	// 06/14/03 AM.
// Does NOT use the analyzer sequence in the KB. Looks at all files in spec.	// 06/14/03 AM.
// Todo: An idea here is to parse the sequence file for other analyzers.	// 06/14/03 AM.
int CMainFrame::FindWordInOtherRuleFiles(CString selStr, CString dirPathStr, bool noCase) 
{
	int total = 0;
	if (selStr != _T("")) {
		CString dirFindPathStr;
		if (dirPathStr == _T(""))
			dirFindPathStr = RulesPath();
		else {
			if (dirPathStr.Find(_T(",")) > 0) {
				CStringList directories;
				StringToStringList(directories,dirPathStr,_T(","));
				POSITION pos = directories.GetHeadPosition();
				CString dirStr = _T("");
				while (pos && (dirStr = directories.GetNext(pos))) {
					dirFindPathStr = appStrAppsDirectory;
					dirFindPathStr += _T("\\") + dirStr + _T("\\spec");
					total += FindWordInDirectoryFiles(selStr,dirFindPathStr,_T("pat"),noCase); // SRP 010705
				}
				return total;
			}
			else {
			// SRP 010417 Use APPS environment variable
				dirFindPathStr = appStrAppsDirectory;
				dirFindPathStr += _T("\\") + dirPathStr + _T("\\spec");
			}
		}
		total += FindWordInDirectoryFiles(selStr,dirFindPathStr,_T("pat"),noCase); // SRP 010705
	}

	return total;
}

int CMainFrame::FindWordInDirectoryFiles(CString selStr, CString dirPathStr,
									     CString extStr, bool noCase) 
{
	CFileFind finder;
	CString pathStr = dirPathStr + _T("\\*.*");
	BOOL bWorking = finder.FindFile(pathStr);
	
	int count = 0;
	CString fileNameStr;
	CString subDirPathStr;
	bool found = false;

	while (bWorking) {
		found = true;
		bWorking = finder.FindNextFile();
		fileNameStr = finder.GetFileName();
		if (fileNameStr.Find(_T("."),0) == 0)
			continue;
		if (finder.IsDirectory()) {
			subDirPathStr = dirPathStr+_T("\\")+fileNameStr;
			count += FindWordInDirectoryFiles(selStr,subDirPathStr,extStr,noCase);
		}
		if (fileNameStr.Find(_T(".ana")) != 0 &&
			(extStr == _T("") || EndsWith(fileNameStr,extStr)))
			count += FindWordInFile(dirPathStr,finder.GetFileName(),selStr,noCase);
	}
	finder.Close();

	return count;
}

// Works off the analyzer sequence in the KB. Gives passes in order.	// 06/14/03 AM.
int CMainFrame::FindWordInSequenceFiles(CString selStr, CString dirPathStr,
									     CString extStr, bool noCase) 
{
	CFileFind finder;
	CString pathStr = dirPathStr + _T("\\*.*");
	CONCEPT *spec = cg->findConcept(appGUIConcept, _T("spec"));
	CONCEPT *pass = cg->Down(spec);
	ANAL_PASS_TYPE typ;

	int count = 0;
	CString fileNameStr;
	CString subDirPathStr;

	while (pass) {
		typ = CTreeViewAnalyzer::ConceptType(pass);
		if (typ != ANAL_PASS_PAT && typ != ANAL_PASS_REC) // If not pat file type, skip.
			{
			pass = cg->Next(pass);
			continue;
			}
		fileNameStr = ConceptName(pass) + _T(".pat");

		// If file not found, warning and skip.

		count += FindWordInFile(dirPathStr,fileNameStr,selStr,noCase);
		pass = cg->Next(pass);
	}

	return count;
}

CString CMainFrame::GetGrammarConceptName() 
{
	CONCEPT *grammarConcept = GrammarConceptSelected();
	CString conceptStr = ConceptName(grammarConcept);
	return conceptStr;
}

int CMainFrame::FindWordInGrammar(CString selStr, bool noCase)
{
	int total = 0;

	CStringList conceptList;
	CONCEPT *gram = cg->findConcept(cg->findRoot(),_T("gram"));
	CString pathStr;
	FindConcept(conceptList,cg->Down(gram),selStr,pathStr,noCase);

	if (conceptList.GetCount() > 0) {
		POSITION pos = conceptList.GetHeadPosition();
		while (pos && (pathStr = conceptList.GetNext(pos))) {
			AddFindLine(_T("Concept: gram"), pathStr);
			total++;
		}
	}

	return total;
}

void CMainFrame::EnsureFindWindowVisible(void)
{
	/*
	if (!m_wndWatchBar.IsWindowVisible()) {
		SendMessage(WM_COMMAND, ID_DISPLAY_FINDWINDOW);
		// SRP 010410 for Painting Problems
		m_wndWatchBar.GetParent()->RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);
		m_wndWatchBar.RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);
		if (m_wndOutputBar.IsWindowVisible()) {
			m_wndOutputBar.GetParent()->RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);
			m_wndOutputBar.RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);
		}
		if (m_wndWorkspaceBar.IsWindowVisible()) {
			m_wndWorkspaceBar.GetParent()->RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);
			m_wndWorkspaceBar.RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);
		}
		
	}
	*/
}

void CMainFrame::SelectText(CDocument *doc, bool selectTab)
{
	m_wndTextView.SelectText(doc);
	if (selectTab) SelectTab(WORKSPACE_TEXT);
}

void CMainFrame::SelectText(CString relFilePathStr, long start, long end) 
{
	CDocument *doc = AfxGetApp()->OpenDocumentFile(InputPath(relFilePathStr));
	CVisualTextView *view = (CVisualTextView *)GetFirstView(doc);

	if (start >= 0 && end > 0) {
		CRichEditCtrl &ed = view->GetRichEditCtrl();
		//view->AdjustStartEnd(start,end,relFilePathStr);
		ed.SetSel(start,end+1);
	}
}

void CMainFrame::AddErrorToDebug() 
{
	CStringList list;
	ReadFileToStringList(list,GetProcessTextFile(_T("err.log")));
	if (!list.GetCount())
		return;

	CString viewStr;
	CString nameStr;
	nameStr = _T("Log file: ") + GetProcessText() + _T(":");
	POSITION pos = list.GetHeadPosition();

	while (pos && (viewStr = list.GetNext(pos))) {
		if (viewStr.Find(_T("[Pass")) != 0 || !appBoolLogAnalyzer)
			AddDebugLine(nameStr,viewStr,true);
	}
}

void CMainFrame::HighlightMatchesSelections(CStringList &selList, CVisualTextDoc *doc) 
{
	CString processTextStr = GetProcessText();
	CString docPathStr = InputPath(processTextStr);
	if (!doc) {
		CVisualTextApp *ThisApp = (CVisualTextApp *)AfxGetApp() ;
		if (FileExists(docPathStr)) //PD 02/14/2001
			doc = (CVisualTextDoc *)ThisApp->CVisualTextApp::OpenDocumentFile(docPathStr);
	}

	if (doc) {
		CVisualTextView *view = (CVisualTextView *)GetFirstView(doc);

		if (selList.GetCount())
			view->ColorSelections(processTextStr,selList);
		else {
			view->ClearFormat();
		}
	}
}

void CMainFrame::HighlightText(CVisualTextDoc *doc) 
{
	int pass = -1;

	if (appBoolHighlightText &&
		IsSelectedTextOpen() &&
		appState != VT_APP_EXITING &&
		appState != VT_APP_SPIDERING ) {

		switch (GetActiveTab()) {

		case WORKSPACE_GRAMMAR:
			pass = m_wndGrammarView.PassNumber();
			break;

		case WORKSPACE_ANALYZER:
		case WORKSPACE_TEXT:
			pass = m_wndAnalyzerView.SelectedPass();
			break;
		}
	}

	if (pass >= 0)
		HighlightMatches(pass);
	else
		theApp.DoAllViews(NULL,_T("VisualText"),_T(""),DOC_ACTION_UNHIGHLIGHT_ALL);
}

void CMainFrame::SaveAnalyzerFile(bool askUserFlag)
{
	CTreeViewAnalyzer *view = &(theApp.m_pMainFrame->m_wndAnalyzerView);
	view->Save(askUserFlag);
	AddDebugLine(_T("Saved Analyzer File"),_T(""));
}

void CMainFrame::OnFilePreferences() 
{
	CVisualTextPreferences pref;
	pref.DoModal();
}

void CMainFrame::OnChooseAnalyzer(UINT idIn) 
{
	//ChooseAnalyzer(idIn);
}

void CMainFrame::ChooseAnalyzer(UINT idIn) 
{
	logmsg(_T("Enter ChooseAnalyzer:"));	// 07/24/03 AM.
	bool closePrevious = !appBoolAnalyzerLoaded ? 0 : 1;

	int adjust = 13568;  // THIS IS THE LOWORD OF SAMPLE_ID_START
	                    // Makes it 1-numbered for since UINT can't be -1

	int analyzerMenuNum = idIn - adjust;
	CStringList analyzerList;
	TokensBySeparator(analyzerList,appRecentAnalyzers,STATUSSEP);
	CString newAppAnaPathStr = NthStringListItem(analyzerList,analyzerMenuNum);

	CString holdAnaPathStr = appAnaFilePathStr;
	CString holdStrName = appNameStr;
	CString holdStrLocation = appDirectoryStr;

	appAnaFilePathStr = newAppAnaPathStr;
	SetAppName();
	appDirectoryStr = StripPath(appAnaFilePathStr,1,true);
	CString analyzerFile = AnalyzerFile();
	bool loaded = false;

	// SRP 010628
	if (UTLFindRunningVTInst(analyzerFile)) {
		appAnaFilePathStr = holdAnaPathStr;
		appNameStr = holdStrName;
		appDirectoryStr = holdStrLocation;
		return;
	}

	if (FileExists(analyzerFile)) {
		if (closePrevious) {
			appAnaFilePathStr = holdAnaPathStr;
			appNameStr = holdStrName;
			appDirectoryStr = holdStrLocation;
			theApp.CloseAnalyzer();

			//RESTORE FOR READING CHOSEN ANALYZER
			appAnaFilePathStr = newAppAnaPathStr;
			appNameStr = StripPath(appAnaFilePathStr,0);
			appDirectoryStr = StripPath(appAnaFilePathStr,0,true);
		}
		theApp.ReadAnalyzerStatusFile(analyzerFile);
		loaded = theApp.OpenAnalyzer();
	}

	if (!loaded) {
		AfxMessageBox(_T("Couldn't load analyzer!"));
		theApp.RemoveRecentAnalyzer(newAppAnaPathStr);
		appAnaFilePathStr = holdAnaPathStr;
		appNameStr = holdStrName;
		appDirectoryStr = holdStrLocation;
	}

	m_paneManager.RedrawPanes();
	this->UpdateWindow();

	logmsg(_T("Leave ChooseAnalyzer:"));	// 07/24/03 AM.
}

void CMainFrame::GenAnalyzer(GRAM_GEN_TYPE type, bool askConfirm) 
{
	CTreeViewGrammar *view = &(theApp.m_pMainFrame->m_wndGrammarView);
	view->GenerateAnalyzer(type,askConfirm);
	theApp.DoAllViews(NULL,_T("VisualText"),_T("pat"),DOC_ACTION_UPDATE_AUTO_GENERATED);
}

BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    #define lpNM   ((LPNMHDR)lParam)
    #define lpnmTB ((LPNMTOOLBAR)lParam)

    BOOL bRet = false;
	CWnd *pWnd;

    switch(lpNM->code){

        case TBN_DROPDOWN:

			CMenu menu;
			menu.CreatePopupMenu();
			CTreeViewText *pView = &(theApp.m_pMainFrame->m_wndTextView);

			CStringList dumps;
			pView->ProcessedFiles(dumps,_T(".txt"));

			POSITION pos = dumps.GetHeadPosition();
			CString dumpFileStr = _T("");
			CString fileNameStr;
			UINT menuID = DUMP_MENU_STARTID;

			while (pos && (dumpFileStr = dumps.GetNext(pos)) && menuID <= DUMP_MENU_ENDID) {
				fileNameStr = StripPath(dumpFileStr,0);
				ClipRight(fileNameStr,4);
				menu.AppendMenu(MF_STRING,menuID++,fileNameStr);
			}

			CRect rc;
			pWnd = m_debugToolBar;
			//m_debugToolBar.GetToolBarCtrl().GetRect(ID_DUMP,&rc);
			//m_debugToolBar.ClientToScreen(&rc);
			
			menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
				rc.left, rc.bottom, this, &rc);

 			break;
	}
	return CMDIFrameWnd::OnNotify(wParam, lParam, pResult);
}


void CMainFrame::OnKnowledgeBase() 
{
	CView *pView = theApp.GetKBEditor();
	if (!pView) {
		CDocument *doc = theApp.OpenNewDocument(_T("Knowledge"),_T(""));
		if (doc)
			pView = GetFirstView(doc);
	}
	if (pView) {
		SendMessage(WMU_SETACTIVEVIEW, 0, (LPARAM)(LPVOID)pView);
		pView->GetParentFrame()->BringWindowToTop();
	}
}

void CMainFrame::OnUp() 
{
	CMDIChildWnd *child = MDIGetActive();

	if (child) {
		CKnowledgeDoc *docActive = (CKnowledgeDoc *)child->GetActiveDocument();
		CString docTypeStr = theApp.GetDocumentTypeString(docActive);
		if (docTypeStr == _T("Knowledge") && docActive) {
			CKnowledgeView *view = (CKnowledgeView *)GetFirstView(docActive);
			view->MoveSelUp();
		}
	}
}

void CMainFrame::OnUpdateUp(CCmdUI* pCmdUI) 
{
	if (appBoolAnalyzerLoaded) {
		bool active = false;
		CMDIChildWnd *child = MDIGetActive();

		if (child) {
			CKnowledgeDoc *docActive = (CKnowledgeDoc *)child->GetActiveDocument();
			CString docTypeStr = theApp.GetDocumentTypeString(docActive);
			if (docTypeStr == _T("Knowledge") && docActive) {
				CKnowledgeView *view = (CKnowledgeView *)GetFirstView(docActive);
				active = view->CanMoveUp();
			}
		}
		
		pCmdUI->Enable(active);
	}
	else
		pCmdUI->Enable(false);
}

void CMainFrame::OnDown() 
{
	CMDIChildWnd *child = MDIGetActive();

	if (child) {
		CKnowledgeDoc *docActive = (CKnowledgeDoc *)child->GetActiveDocument();
		CString docTypeStr = theApp.GetDocumentTypeString(docActive);
		if (docTypeStr == _T("Knowledge") && docActive) {
			CKnowledgeView *view = (CKnowledgeView *)GetFirstView(docActive);
			view->MoveSelDown();
		}
	}
}

void CMainFrame::OnUpdateDown(CCmdUI* pCmdUI) 
{
	if (appBoolAnalyzerLoaded) {
		bool active = false;
		CMDIChildWnd *child = MDIGetActive();

		if (child) {
			CKnowledgeDoc *docActive = (CKnowledgeDoc *)child->GetActiveDocument();
			CString docTypeStr = theApp.GetDocumentTypeString(docActive);
			if (docTypeStr == _T("Knowledge") && docActive) {
				CKnowledgeView *view = (CKnowledgeView *)GetFirstView(docActive);
				active = view->CanMoveDown();
			}
		}
		
		pCmdUI->Enable(active);
	}
	else
		pCmdUI->Enable(false);
}

void CMainFrame::OnTabSave() 
{
	switch (GetActiveTab()) {

	case WORKSPACE_GRAMMAR:
		SaveKB(true);
		break;

	case WORKSPACE_ANALYZER:
		SaveAnalyzerFile();
		break;

	case WORKSPACE_TEXT:
		break;
	}	
}

void CMainFrame::OnUpdateTabSave(CCmdUI* pCmdUI) 
{
	if (appBoolAnalyzerLoaded) {
		WORKSPACE_TAB tab = (WORKSPACE_TAB) GetActiveTab();
		CTreeViewAnalyzer *viewAnalyzer = &(theApp.m_pMainFrame->m_wndAnalyzerView);

		pCmdUI->Enable(appBoolAnalyzerLoaded &&
					   (tab == WORKSPACE_GRAMMAR && kbDirty) ||
					   (tab == WORKSPACE_ANALYZER && viewAnalyzer->IsAnalyzerFileDirty()));
	}
	else
		pCmdUI->Enable(false);
}

void CMainFrame::OnSlotFill() 
{
	if (!m_slotDlg) {
		m_slotDlg;
		m_slotDlg.Create(IDD_SLOT_DLG, this);
		m_slotDlg.ShowWindow(true);
	}
}

void CMainFrame::OnUpdateSlotFill(CCmdUI* pCmdUI) 
{
	CTreeViewText *view = &(theApp.m_pMainFrame->m_wndTextView);
	pCmdUI->Enable(view->OpenProcessedFiles("output.txt",false));
	pCmdUI->Enable(false);
}

CString CMainFrame::ProcessedFileStr(CString extStr, CString processPathStr) 
{
	CTreeViewText *view = &(theApp.m_pMainFrame->m_wndTextView);
	return view->ProcessedFileStr(extStr,processPathStr);
}

void CMainFrame::OnWebpage() 
{
	if (m_textTreeType == TEXT_TREE_HTML) {
		CString filePathStr = InputPath(m_strProcessText);
		CString urlStr = GetUrlFromUrlFile(filePathStr);
		if (urlStr == _T(""))
			theApp.OpenNewDocument(_T("Browser"),filePathStr);
		else
			BrowserNavigate(urlStr);
	}
}

void CMainFrame::OnUpdateWebpage(CCmdUI* pCmdUI) 
{
	if (appBoolAnalyzerLoaded) {
		pCmdUI->Enable(m_textTreeType == TEXT_TREE_HTML);
	}
	else
		pCmdUI->Enable(false);
}

void CMainFrame::OnTreeConcept() 
{
	OpenParseTree(true);	
}
/*
void CMainFrame::OnToolsKbtoolbar() 
{
	appBoolShowKBToolbar = !m_knowledgeToolBar->IsWindowVisible(); // SRP 010410
	//ShowControlBar(&m_knowledgeToolBar,appBoolShowKBToolbar,false);
	//m_knowledgeToolBar.CCJToolBar::ShowWindow(appBoolShowKBToolbar);
}

void CMainFrame::OnUpdateToolsKbtoolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_knowledgeToolBar->IsWindowVisible()); // SRP 010410
}

void CMainFrame::OnToolsBrowsertoolbar() 
{
	//appBoolShowBrowserToolbar = !m_browserToolBar.IsWindowVisible(); // SRP 010410
	//ShowControlBar(&m_browserToolBar,appBoolShowBrowserToolbar,false);
}

void CMainFrame::OnUpdateToolsBrowsertoolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_browserToolBar->IsWindowVisible()); // SRP 010410
}

void CMainFrame::OnToolsTabtoolbar() 
{
	//appBoolShowTabToolbar = !m_tabToolBar.IsWindowVisible(); // SRP 010410
	//ShowControlBar(&m_tabToolBar,appBoolShowTabToolbar,false);
}

void CMainFrame::OnUpdateToolsTabtoolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_tabToolBar->IsWindowVisible()); // SRP 010410
}
*/

void CMainFrame::OnUpdateBrowserOpenurl(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);
}

void CMainFrame::OnBrowserOpenurl() 
{
	if (AskForText(_T("Enter URL:"))) {
		BrowserNavigate(appAskText);
	}	
}

void CMainFrame::OnToolsFind() 
{
	OnFind();	
}

void CMainFrame::OnUpdateToolsFind(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);
}

void CMainFrame::OnEditTest() 
{
	theApp.OpenNewDocument(_T("TextEditor"),_T("d:\\apps\\Spider\\spec\\Anchors.pat"));	
}

void CMainFrame::OnCompile() 
{
	appBoolCompile ^= 1;
}

void CMainFrame::OnToolsDebugtoolbar() 
{
	appBoolShowDebugToolbar = !m_debugToolBar->IsWindowVisible(); // SRP 010410
	//ShowControlBar(&m_debugToolBar,appBoolShowDebugToolbar,false);
}

void CMainFrame::OnUpdateToolsDebugtoolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_debugToolBar->IsWindowVisible()); // SRP 010410
}

void CMainFrame::OnDictionary() 
{
	CWnd* mainWnd = AfxGetMainWnd() ;
	CDictionaryDlg dlg(mainWnd);
	dlg.DoModal();
}

void CMainFrame::OnUpdateCompile(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(appBoolCompile);	
}

void CMainFrame::OnKbDictionary() 
{
	OnDictionary();
}

void CMainFrame::OnToolsAsciitable() 
{
	CASCIIDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnToolsHexviewer() 
{
	CFileDialog fileDlg(true);

	if (fileDlg.DoModal() == IDOK) {
		CString fileName = fileDlg.GetPathName();
		CHexEditDlg dlg;
		dlg.SetFileData(fileName);
		dlg.DoModal();
	}
}

void CMainFrame::OnUpdateToolsHexviewer(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CMainFrame::OnToolsTextfileinfo() 
{
	CFileDialog fileDlg(true);

	if (fileDlg.DoModal() == IDOK) {
		CString fileName = fileDlg.GetPathName();
		CTextInfoDlg dlg;
		dlg.SetData(fileName);
		dlg.DoModal();
	}
}

void CMainFrame::OnUpdateToolsTextfileinfo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CMainFrame::OnToolsCharacterviewer() 
{
	CFileDialog fileDlg(true);

	if (fileDlg.DoModal() == IDOK) {
		CString fileName = fileDlg.GetPathName();
		CFileCharViewerDlg dlg;
		dlg.SetData(fileName);
		dlg.DoModal();
	}
}

void CMainFrame::OnUpdateToolsCharacterviewer(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CMainFrame::OnFindmenuHelp() 
{
	HelpTopic(_T("Find"));			// 04/27/01 AM.	
}

void CMainFrame::OnUpdateDictionary(CCmdUI* pCmdUI) 
{
	//pCmdUI->Enable(appBoolAnalyzerLoaded);
	pCmdUI->Enable(false);
}

void CMainFrame::OnUpdateKnowledgeBase(CCmdUI* pCmdUI) 
{
//	CVisualTextApp *ThisApp = (CVisualTextApp *)AfxGetApp() ;
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CMainFrame::OnUpdateProcessedText(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_strProcessText != _T(""));
}

void CMainFrame::OnUpdateRuleFile(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_strProcessText != _T(""));	
}

void CMainFrame::OnUpdateTreeConcept(CCmdUI* pCmdUI) 
{
	if (appBoolAnalyzerLoaded) {
		CTreeViewText *view = &(theApp.m_pMainFrame->m_wndTextView);
		pCmdUI->Enable(view->OpenProcessedFiles(_T("final.log"),false) || view->OpenProcessedFiles(_T("ana001.log"),false) );
	}
	else
		pCmdUI->Enable(false);
}

void CMainFrame::OnUpdateKbDictionary(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(false);
	//pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CMainFrame::OnUpdateAnalyzerGenerateall(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CMainFrame::OnUpdateAnalyzerGeneratedirty(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CMainFrame::OnUpdateAnalyzerGenerateunlocked(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CMainFrame::OnUpdateFind(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CMainFrame::OnUpdateFindBack(CCmdUI* pCmdUI) 
{
	CString selStr = appStrFind;
	if (selStr == _T(""))
		selStr = appStrSelText;
	pCmdUI->Enable(appBoolAnalyzerLoaded && selStr != _T("") && appStrFocusDocumentType == _T("VisualText"));	
}

void CMainFrame::OnUpdateFindForward(CCmdUI* pCmdUI) 
{
	CString selStr = appStrFind;
	if (selStr == _T(""))
		selStr = appStrSelText;
	pCmdUI->Enable(appBoolAnalyzerLoaded && selStr != _T("") && appStrFocusDocumentType == _T("VisualText"));	
}

void CMainFrame::OnBatchMode() 
{
	if (!appBoolBatchMode && appBoolHighlightText && !appBoolBatchMode) {
		if (AfxMessageBox(_T("Turn on batch mode? If you do so, no new log files will be generated. Highlighting will no longer be kept up to date"),MB_YESNO)==IDYES) {
			appBoolBatchMode ^= 1;
			appBoolHighlightText ^= 1;
		}
	}
	else appBoolBatchMode ^= 1;	
}

void CMainFrame::OnUpdateBatchMode(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(appBoolBatchMode);	
}

void CMainFrame::OnUpdateWindowCloseall(CCmdUI* pCmdUI) 
{
	if (appBoolAnalyzerLoaded)
		pCmdUI->Enable(GetDocumentCount());
	else
		pCmdUI->Enable(false);
}

// SRP 01-01-29
BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	if (theApp.DoHourGlass())
		return true;
	return CMDIFrameWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CMainFrame::OnDisplayTabswindow() 
{
	logmsg(_T("  OnDisplayTabsWindow"));

	OnBarCheck(ID_VIEW_WORKSPACE);

	// Show the tab control bar and update the tab display.          // 05/13/08 AM.
	//appBoolShowTabsWindow = !m_wndWorkspaceBar.IsWindowVisible();    // 05/13/08 AM.
	//ShowControlBar(&m_wndWorkspaceBar,appBoolShowTabsWindow,FALSE);  // 05/13/08 AM.
//	m_wndWorkspaceBar.RefreshTabDisplay();	// In case of need.       // 05/13/08 AM.

	if (appBoolShowTabsWindow)	// 05/13/08 AM.
		logmsg(_T("  appBoolShowTabsWindow=1"));	// 05/13/08 AM.
	else
		logmsg(_T("  appBoolShowTabsWindow=0"));	// 05/13/08 AM.

}

void CMainFrame::OnDisplayFindwindow() 
{
	OnBarCheck(IDC_BAR_OUTPUT);

}

void CMainFrame::OnDisplayOutputwindow() 
{
	// COMMENTED OUT.	// 05/13/08 AM.
//	OnBarCheck(IDC_BAR_WATCH);
}

void CMainFrame::OnUpdateDisplayTabswindow(CCmdUI* pCmdUI) 
{
	logmsg(_T("  OnUpdateDisplayTabsWindow"));

	//pCmdUI->SetCheck(m_wndWorkspaceBar.IsWindowVisible()); // SRP 010410

}

void CMainFrame::OnUpdateDisplayOutputwindow(CCmdUI* pCmdUI) 
{
	// COMMENTING OUT. Pretty much an obsolete button.	// 05/13/08 AM.
//	pCmdUI->SetCheck(m_wndOutputBar.IsWindowVisible()); // SRP 010410
//	pCmdUI->SetCheck(FALSE);	// Tried this.	// 05/13/08 AM.
	pCmdUI->Enable(FALSE);	// Trying this.	// 05/13/08 AM.
}

void CMainFrame::OnUpdateDisplayFindwindow(CCmdUI* pCmdUI) 
{
// COMMENTED OUT.	// 05/13/08 AM>
//	pCmdUI->SetCheck(m_wndWatchBar.IsWindowVisible()); // SRP 010410


	// "Find Window" is being use for the whole tabbed stuff at the bottom. // 05/13/08 AM.
	//pCmdUI->SetCheck(m_wndOutputBar.IsWindowVisible()); // 05/13/08 AM.
}

void CMainFrame::OnFileNew() 
{
	theApp.OnFileNew();
}

void CMainFrame::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	if (appBoolAnalyzerLoaded &&
		appState != VT_APP_LOADING &&
		appState != VT_APP_ANALYZING &&
		appState != VT_APP_DIR_ANALYZING &&
		appState != VT_APP_GENERATING) {

		pCmdUI->Enable(appBoolDocDirty);
		return;
	}
	pCmdUI->Enable(false);
}

void CMainFrame::OnCompileKb() 
{
	KBCompile();
}

void CMainFrame::OnKbCompilekb() 
{
	KBCompile();
}

void CMainFrame::OnUpdateKbCompilekb(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CMainFrame::OnUpdateCompileKb(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

void CMainFrame::OnAnalyzerCompileanalyzer() 
{
	appBoolCompile ^= 1;
}

void CMainFrame::OnUpdateAnalyzerCompileanalyzer(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(appBoolCompile);		
}

void CMainFrame::OnUpdateFileArchivesArchiveanalyzer(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);		
}

void CMainFrame::OnUpdateFileArchivesUploadanalyzer(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);		
}

void CMainFrame::OnUpdateFileArchivesLocalArchive(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);		
}

void CMainFrame::OnReplaceInFiles() 
{
	CReplaceInFilesDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnUpdateReplaceInFiles(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);
}

void CMainFrame::OnTreeSemantics() 
{
	appBoolNodeSemantics ^= 1;
	//appBoolTreeSemantics ^= 1;
	//theApp.DoAllViews(NULL,_T("ParseTree"),_T(""),DOC_ACTION_UPDATE);
}

void CMainFrame::OnUpdateTreeSemantics(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(appBoolNodeSemantics);
	//pCmdUI->SetCheck(appBoolTreeSemantics);
}

void CMainFrame::OnViewTreesemanticsmode() 
{
	OnTreeSemantics();
}

void CMainFrame::OnUpdateViewTreesemanticsmode(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(appBoolTreeSemantics);
}

void CMainFrame::OnViewTreeOffsetsmode() 
{
	appBoolTreeOffsets ^= 1;
	theApp.DoAllViews(NULL,_T("ParseTree"),_T(""),DOC_ACTION_UPDATE);
}

void CMainFrame::OnUpdateViewTreeOffsetsmode(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(appBoolTreeOffsets);
}

void CMainFrame::OnContextHelp()
{
	/*
	// don't enter twice, and don't enter if initialization fails
	if (m_bHelpMode == HELP_ACTIVE || !CanEnterHelpMode())
		return;

	// don't enter help mode with pending WM_EXITHELPMODE message
	MSG msg;
	if (PeekMessage(&msg, m_hWnd, WM_EXITHELPMODE, WM_EXITHELPMODE,
		PM_REMOVE|PM_NOYIELD))
	{
		return;
	}

	BOOL bHelpMode = m_bHelpMode;
	ASSERT(m_bHelpMode == HELP_INACTIVE || m_bHelpMode == HELP_ENTERING);
	m_bHelpMode = HELP_ACTIVE;

#if 0
#ifndef _AFX_NO_OLE_SUPPORT
	// allow any in-place active servers to go into help mode
	if (bHelpMode != HELP_ENTERING && m_pNotifyHook != NULL &&
		!m_pNotifyHook->OnContextHelp(TRUE))
	{
		TRACE0("Error: an in-place server failed to enter context help mode.\n");
		m_pNotifyHook->OnContextHelp(FALSE);    // undo partial help mode
		m_bHelpMode = HELP_INACTIVE;
		return;
	}
#endif
#endif

	if (bHelpMode == HELP_INACTIVE)
	{
		// need to delay help startup until later
		PostMessage(WM_COMMAND, ID_CONTEXT_HELP);
		m_bHelpMode = HELP_ENTERING;
		return;
	}

	ASSERT(m_bHelpMode == HELP_ACTIVE);

	// display special help mode message on status bar
	UINT nMsgSave = (UINT)SendMessage(WM_SETMESSAGESTRING,
		(WPARAM)AFX_IDS_HELPMODEMESSAGE);
	if (nMsgSave == 0)
		nMsgSave = AFX_IDS_IDLEMESSAGE;

	DWORD   dwContext = 0;
	POINT   point;

	GetCursorPos(&point);
	SetHelpCapture(point, NULL);
	LONG lIdleCount = 0;
	CWinApp* pApp = AfxGetApp();

	while (m_bHelpMode)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!ProcessHelpMsg(msg, &dwContext))
				break;
			ASSERT(dwContext == 0);
		}
		else if (!pApp->OnIdle(lIdleCount++))
		{
			lIdleCount = 0;
			WaitMessage();
		}
	}

	m_bHelpMode = HELP_INACTIVE;
	ReleaseCapture();

	// make sure the cursor is set appropriately
	SetCapture();
	ReleaseCapture();

	// restore original status bar text
	SendMessage(WM_SETMESSAGESTRING, (WPARAM)nMsgSave);

#if 0
#ifndef _AFX_NO_OLE_SUPPORT
	// tell in-place servers to exit Shift+F1 help mode
	if (m_pNotifyHook != NULL)
		m_pNotifyHook->OnContextHelp(FALSE);
#endif
#endif

	if (dwContext != 0)
	{
		if (dwContext == -1) {
			HelpTopic();
		}
		else {
			HelpTopic(_T(""), dwContext);
		}
	}
	PostMessage(WM_KICKIDLE);    // trigger idle update
	*/
}

void CMainFrame::OnUpdateInsert(CCmdUI* pCmdUI)
{
	CVisualTextView *view = GetActiveVisualTextView();
	pCmdUI->Enable(view ? !view->m_bInsertMode: false);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.lpszClass = _T("XTPMainFrame");
	CXTPDrawHelpers::RegisterWndClass(AfxGetInstanceHandle(), cs.lpszClass, 
		CS_DBLCLKS, AfxGetApp()->LoadIcon(IDR_MAINFRAME));

	return TRUE;
}

BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	COPYDATASTRUCT cds = *pCopyDataStruct;
	DWORD msgType = cds.dwData;
	switch (msgType) {
		case MSG_GETLOADEDANALYZER:
			return !appAnaFilePathStr.CompareNoCase((LPTSTR)cds.lpData);
		default:
			break;
	}
	return CMDIFrameWnd::OnCopyData(pWnd, pCopyDataStruct);
}

LRESULT CMainFrame::OnBringWindowToTop(WPARAM wParam, LPARAM lParam)
{
	::BringWindowToTop((HWND)wParam);
	return 0L;
}

void CMainFrame::OnUpdateLineCol(CCmdUI* pCmdUI)
{
	static CWnd* pViewLast = 0;
	static CHARRANGE crLast = {-1,-1};
	CString cText = _T("");
	CVisualTextView *pView = GetActiveVisualTextView();
	if (pView) {
		CRichEditCtrl &edit = pView->GetRichEditCtrl();
		CHARRANGE cr;
		edit.GetSel(cr);
		if (pViewLast != (CWnd*)pView) {
			crLast.cpMin = -1;
			crLast.cpMax = -1;
			pViewLast = (CWnd*)pView;
		}
		if (cr.cpMin == crLast.cpMin && cr.cpMax == crLast.cpMax)
			return;
		int pos = 0;
		if (cr.cpMin == cr.cpMax) {
			pos = cr.cpMin;
		}
		else {
			// Situation 1: selection continuing change downward
			if (crLast.cpMin == cr.cpMin)
				pos = cr.cpMax;
			// Situation 2: selection continuing change upward
			else if (crLast.cpMax == cr.cpMax)
				pos = cr.cpMin;
			// Situation 3: selection direction reversed: downward to upward
			else if (crLast.cpMin == cr.cpMax)
				pos = cr.cpMin;
			// Situation 4: selection direction reversed: upward to downward
			else if (crLast.cpMax == cr.cpMin)
				pos = cr.cpMax;
			// Situation 5: file is opened with a selection (e.g. dbl-clk find results)
			else
				pos = cr.cpMin;
		}
		int line = edit.LineFromChar(pos);
		int lineBeg = edit.LineIndex(line);
		int lineEnd = edit.LineIndex(line + 1);
#if 0
		// The tab code screws up Backwards Selection in RichEditCtrl because of the SetSel() call
		int col = pos - lineBeg;

		// Tab Calculation
		pView->LockWindowUpdate();
		edit.SetSel(lineBeg, lineEnd-1);
		CString str = edit.GetSelText();
		edit.SetSel(cr);
		pView->UnlockWindowUpdate();
		if (-1 != str.Find('\t')) {
			int tabCol = -1;
			int len = str.GetLength();
			for (int n = 0; n < len && n < col; n++) {
				tabCol++;
				if ('\t' == str.GetAt(n))
					tabCol = ((tabCol / appIntRuleTabSize) * appIntRuleTabSize) + appIntRuleTabSize - 1;
			}
			col = tabCol + 1;
		}

		cText.Format(_T("Ln %d Col %d"), line + 1, col + 1);
#endif
		cText.Format(_T("Ln %d"), line + 1);
		crLast = cr;
	}
	else if (crLast.cpMin != -1) {
		crLast.cpMin = -1;
		crLast.cpMax = -1;
	}
	pCmdUI->SetText (cText);
}

long CMainFrame::OnDestroyAttributeEditor (WPARAM uParam, LPARAM lParam)
{
	if (m_pAttributeEditor) {
		CAttributeEditor* editor = (CAttributeEditor*)m_pAttributeEditor;
		editor->Cleanup();
		editor->DestroyWindow();
		delete editor;
		m_pAttributeEditor = NULL;
	}
	return 0L;
}

void CMainFrame::OnAttributeeditor() 
{
	if (!m_pAttributeEditor) {
		m_pAttributeEditor = new CAttributeEditor();
		CAttributeEditor* editor = (CAttributeEditor*)m_pAttributeEditor;
		editor->Create(IDD_ATTRIBUTE_EDITOR);
	}	
}

void CMainFrame::OpenAttributeEditor(CONCEPT* concept, BOOL bCalledFromKB) 
{
	OnAttributeeditor();
	m_pAttributeEditor->ShowWindow(SW_SHOW);
	m_pAttributeEditor->SetCGConcept(concept);
	m_pAttributeEditor->InitListWithData();
	if (!bCalledFromKB) {
		CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
		if (pView)
			pView->SelectFromConcept(concept);
	}
}


void CMainFrame::OnSetTextAsAnalyzed(AFX_PMSG index) 
{
	CString fullPathStr;
	m_wndTextView.SetTextAsAnalyzed(fullPathStr);
}

void CMainFrame::OnUpdateAttributeeditor(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((BOOL)!m_pAttributeEditor && appBoolAnalyzerLoaded);
}

LONG CMainFrame::OnSetActiveView(WPARAM wParam, LPARAM lParam)
{
	CView* pView = (CView *)lParam;
	HMENU hMenu = ::GetMenu(m_hWnd);
	if (hMenu) {
		HMENU hMenuWindows = GetWindowMenuPopup(hMenu);
		if (hMenuWindows) {
			UINT nItems = GetMenuItemCount(hMenuWindows);
			CString cViewTitle = pView->GetDocument()->GetTitle();
			LPTSTR lp;
			_TCHAR strTitle[128];
			for (UINT i = 0; i < nItems; i++) {
				GetMenuString(hMenuWindows, i, (LPTSTR)strTitle, sizeof(strTitle), MF_BYPOSITION);
				WORD wID = GetMenuItemID (hMenuWindows, i);
				if (wID >= AFX_IDM_FIRST_MDICHILD && (lp = _tcsstr(strTitle, cViewTitle)) && !_tcscmp(lp, cViewTitle)) {
					SendMessage(WM_COMMAND, wID);
					break;
				}
			}
		}
	}
	return 0L;
}

void CMainFrame::OnToolsAddwords() 
{
	if (AskForText(_T("Enter a word to add to the KB System Dictionary:")))
		theApp.AddWordToKBSystemDictionary(appAskText);
}

void CMainFrame::OnUpdateToolsAddwords(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((BOOL)appBoolAnalyzerLoaded);
}

void CMainFrame::OnToolsKbattributes() 
{
	if (AskForText(_T("Enter a word to find it and its attributes in the KB System Dictionary:")))
		theApp.FindKBAttribute(appAskText);
}

void CMainFrame::OnUpdateToolsKbattributes(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((BOOL)appBoolAnalyzerLoaded);
}

void CMainFrame::OnToolsDictionaryURLLookup(UINT uID)
{
	UINT idx = uID - ID_TOOLS_DICTIONARY_URL_LOOKUP_BASE;
	if (AskForText(_T("Enter a word for Dictionary Lookup:")))
		OpenBrowserDictLookup(m_strURLs[idx], appAskText);
}

void CMainFrame::OnUpdateToolsDictionaryURLLookup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((BOOL)appBoolAnalyzerLoaded);
}

void CMainFrame::OnChooseDumpFile(UINT idIn) 
{
	int adjust = 13668;  // THIS IS THE LOWORD OF DUMP_ID_START
	                    // Makes it 1-numbered for since UINT can't be -1

	int dumpMenuNum = idIn - adjust;

	CStringList dumps;
	m_wndTextView.ProcessedFiles(dumps,_T(".txt"));

	POSITION pos = dumps.GetHeadPosition();
	CString fullPathStr = _T("");
	int i = 0;

	while (pos && (fullPathStr = dumps.GetNext(pos))) {
		if (i++ == dumpMenuNum) {
			theApp.OpenNewDocument(_T("VisualText"),fullPathStr);
			break;
		}
	}
}

void CMainFrame::OnUpdateRunNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded &&
		m_wndTextView.HasNextText() &&
		// I want option to process next text even if current hasn't been analyzed.	// 05/21/08 AM.
//		m_strProcessText != _T("") &&		// 05/21/08 AM.
		appState != VT_APP_ANALYZING &&
		appState != VT_APP_DIR_ANALYZING &&
		appState != VT_APP_WORD_NETTING &&
		appState != VT_APP_GENERATING);		
}

void CMainFrame::OnUpdateSaveAll(CCmdUI* pCmdUI) 
{
	if (appBoolAnalyzerLoaded &&
		appState != VT_APP_ANALYZING &&
		appState != VT_APP_DIR_ANALYZING &&
		appState != VT_APP_GENERATING) {
		
		pCmdUI->Enable(theApp.DirtyDocs());
		return;
	}
	pCmdUI->Enable(false);
}

void CMainFrame::OnUpdateOutput(CCmdUI* pCmdUI) 
{
	if (appBoolAnalyzerLoaded)
		pCmdUI->Enable(m_wndTextView.OpenProcessedFiles(_T("output.txt"),false));
	else
		pCmdUI->Enable(false);
}


void CMainFrame::OnDumpSelect(NMHDR* pNMHDR, LRESULT* pResult)
{
	CXTPControlListBox* pControlListBox = DYNAMIC_DOWNCAST(CXTPControlListBox, ((NMXTPCONTROL*)pNMHDR)->pControl);
	if (pControlListBox)
	{
		int dumpMenuNum = pControlListBox->GetListCtrl()->GetCurSel();

		CStringList dumps;
		m_wndTextView.ProcessedFiles(dumps,_T(".txt"));

		POSITION pos = dumps.GetHeadPosition();
		CString dumpFileStr = _T("");
		CString fileNameStr;
		UINT menuID = DUMP_MENU_STARTID;
		int i = 0;

		while (pos && (dumpFileStr = dumps.GetAt(pos)) && menuID <= DUMP_MENU_ENDID) {
			if (!EndsWith(dumpFileStr, _T("\\output.txt")) && i++ == dumpMenuNum) {
				theApp.OpenNewDocument(_T("VisualText"),dumpFileStr);
				break;
			}
			dumps.GetNext(pos);
		}

		*pResult = 1; // Handled;
	}
}

void CMainFrame::OnListBoxControlPopup(NMHDR* pNMHDR, LRESULT* pRes)
{
	ASSERT(pNMHDR != NULL);
	ASSERT(pRes != NULL);
	CXTPControlListBox* pControlListBox = DYNAMIC_DOWNCAST(CXTPControlListBox, ((NMXTPCONTROL*)pNMHDR)->pControl);
	if (pControlListBox)
	{
		CListBox* pListBox = pControlListBox->GetListCtrl();
		pListBox->ResetContent();

		CStringList dumps;
		m_wndTextView.ProcessedFiles(dumps,_T(".txt"));

		POSITION pos = dumps.GetHeadPosition();
		CString dumpFileStr = _T("");
		CString fileNameStr;
		UINT menuID = DUMP_MENU_STARTID;

		while (pos && (dumpFileStr = dumps.GetNext(pos)) && menuID <= DUMP_MENU_ENDID) {
			fileNameStr = StripPath(dumpFileStr,0);
			ClipRight(fileNameStr,4);
			if (fileNameStr != "output")
				pListBox->AddString(fileNameStr);
		}
		*pRes = 1;
	}
}

void CMainFrame::OnDump() 
{
	m_wndTextView.OpenProcessedFiles();	
}

void CMainFrame::OnUpdateDump(CCmdUI* pCmdUI) 
{
	if (appBoolAnalyzerLoaded) {
		bool hasDem = m_wndTextView.OpenProcessedFiles(_T(".txt"),false,true);
		pCmdUI->Enable(hasDem);

	} else
		pCmdUI->Enable(false);
}

void CMainFrame::OnUpdateTabCombo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);
}

void CMainFrame::OnUpdateTree(CCmdUI* pCmdUI) 
{
	if (appBoolAnalyzerLoaded)
		pCmdUI->Enable(m_wndTextView.OpenProcessedFiles(_T("final.log"),false) || 
					   m_wndTextView.OpenProcessedFiles(_T("ana001.log"),false) );
	else
		pCmdUI->Enable(false);
}

void CMainFrame::OnOutput() 
{
	if (!m_wndTextView.OpenOutputFile())
		AfxMessageBox(_T("No output file for selected text."));
}

CVisualTextView *CMainFrame::GetActiveVisualTextView()
{
	CMDIChildWnd *child = MDIGetActive();
	if (child) {
		CVisualTextDoc *doc = (CVisualTextDoc *)child->GetActiveDocument();
		if (doc && doc->IsKindOf(RUNTIME_CLASS(CVisualTextDoc)) ) // SRP 010715
			return (CVisualTextView *)GetFirstView(doc);
	}
	return NULL;
}

void CMainFrame::OnHelpVisualTexthelp() 
{
	HelpTopic();
}

void CMainFrame::CreatePaneWindows()
{
	if (!m_wndFindPane.GetSafeHwnd())
	{
		m_wndFindPane.Create(_T("STATIC"), NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CXTPEmptyRect(), this, 0);
	}

	if (!m_wndFindPane2.GetSafeHwnd())
	{
		m_wndFindPane2.Create(_T("STATIC"), NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CXTPEmptyRect(), this, 0);
	}
	if (!m_wndLogPane.GetSafeHwnd())
	{
		m_wndLogPane.Create(_T("STATIC"), NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CXTPEmptyRect(), this, 0);
	}
	if (!m_wndAnalyzePane.GetSafeHwnd())
	{
		m_wndAnalyzePane.Create(_T("STATIC"), NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CXTPEmptyRect(), this, 0);
	}
}

LRESULT CMainFrame::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_DPN_SHOWWINDOW)
	{
		CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;

		if (!pPane->IsValid())
		{
			switch (pPane->GetID())
			{
				case IDR_PANE_TEXT:
					pPane->Attach(m_wndTextView.CreatePane(this));
					break;
				case IDR_PANE_ANALYZER:
					pPane->Attach(m_wndAnalyzerView.CreatePane(this));
					break;
				case IDR_PANE_GRAMMAR:
					pPane->Attach(m_wndGrammarView.CreatePane(this));
					break;
				case IDR_PANE_ANALYZE:
					pPane->Attach(&m_wndAnalyzePane);
					break;
				case IDR_PANE_LOG:
					pPane->Attach(&m_wndLogPane);
					break;
				case IDR_PANE_FIND:
					pPane->Attach(&m_wndFindPane);
					break;
				case IDR_PANE_FIND2:
					pPane->Attach(&m_wndFindPane2);
					break;
			}
		}
		return 1;
	}
	return 0;
}

void CMainFrame::OnCustomize()
{
	// Get a pointer to the command bars object.
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if(pCommandBars != NULL)
	{
		// Instanciate the customize dialog object.
		CXTPCustomizeSheet dlg(pCommandBars);

		// Add the options page to the customize dialog.
		CXTPCustomizeOptionsPage pageOptions(&dlg);
		dlg.AddPage(&pageOptions);

		// Add the commands page to the customize dialog.
		CXTPCustomizeCommandsPage* pCommands = dlg.GetCommandsPage();
		pCommands->AddCategories(IDR_RULEFILE);

		// Use the command bar manager to initialize the
		// customize dialog.
		pCommands->InsertAllCommandsCategory();
		pCommands->InsertBuiltInMenus(IDR_RULEFILE);
		pCommands->InsertNewMenuCategory();

		// Dispaly the dialog.
		dlg.DoModal();
	}
}

void CMainFrame::OnRuleFile() 
{
	m_wndAnalyzerView.OpenSelectedRulesFile();
}

void CMainFrame::OnProcessedText() 
{
	CVisualTextDoc *textDoc =
		(CVisualTextDoc *)theApp.OpenNewDocument(_T("VisualText"),InputPath(m_strProcessText));	
}

void CMainFrame::OnTree() 
{
	OpenParseTree();	
}

void CMainFrame::OnUpdateNextText(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_strProcessText != _T("") && appBoolPageMode);
}

void CMainFrame::OnUpdatePrevText(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_strProcessText != _T("") && appBoolPageMode);
}

void CMainFrame::OnUpdatePageMode(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(appBoolPageMode);	
}

void CMainFrame::OnPageMode() 
{
	appBoolPageMode ^= 1;
	if (!appBoolPageMode)
		theApp.SavePageModeState();
}

void CMainFrame::OnNextText() 
{
	m_wndTextView.NextText();
}

void CMainFrame::OnPrevText() 
{
	m_wndTextView.PrevText();
}

void CMainFrame::OnUpdateVerboseMode(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(appBoolVerbose);	
}

void CMainFrame::OnVerboseMode() 
{
	appBoolVerbose ^= 1;	
}

void CMainFrame::OnUpdateStopProcess(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appState == VT_APP_ANALYZING ||
		appState == VT_APP_DIR_ANALYZING || 
//		appState == VT_APP_GENERATING ||
		appState == VT_APP_WORD_NETTING);
}

void CMainFrame::OnStopProcess() 
{
	StopProcess();
}

void CMainFrame::OnDelete() 
{
	switch (GetActiveTab()) {

	case WORKSPACE_GRAMMAR:
		m_wndGrammarView.DeleteSelectedAndAnalyzerPass();
		break;

	case WORKSPACE_ANALYZER:
		m_wndAnalyzerView.DeleteSelected();
		break;

	case WORKSPACE_TEXT:
		m_wndTextView.DeleteSelected();
		break;
	}
}

void CMainFrame::OnUpdateDelete(CCmdUI* pCmdUI) 
{
	if (appBoolAnalyzerLoaded) {
		WORKSPACE_TAB tab = (WORKSPACE_TAB)GetActiveTab();
		bool displayFlag = true;

		if (tab == WORKSPACE_ANALYZER) {
			CTreeViewAnalyzer *view = &(theApp.m_pMainFrame->m_wndAnalyzerView);
			displayFlag = view->CanDelete();
		}

		if (displayFlag && tab == WORKSPACE_TEXT) {
			CTreeViewText *view = &(theApp.m_pMainFrame->m_wndTextView);
			displayFlag = view->TreeItemCount() ? true : false;
		}

		if (displayFlag && tab == WORKSPACE_GRAMMAR) {
			CTreeViewGrammar *view = &(theApp.m_pMainFrame->m_wndGrammarView);
			displayFlag = view->TreeItemCount() ? true : false;
		}

		pCmdUI->Enable(displayFlag);
	}
	else
		pCmdUI->Enable(false);
}

void CMainFrame::FindDlg() 
{
	CString findStr = _T("");

	CVisualTextView *view = GetActiveVisualTextView();
	if (view) {
		CRichEditCtrl &edit = view->GetRichEditCtrl();
		//findStr = StreamOut(SF_TEXT | SF_UNICODE | SFF_SELECTION, es);
		//findStr = edit.GetSelText();  // Older stuff, for reference. AM.
		findStr = view->EditGetSelText();  // 05/02/21 SRP
	}

	// Get Active Document
	CMDIChildWnd *child = MDIGetActive();
	CVisualTextDoc *doc = NULL;
	if (child)
		doc = (CVisualTextDoc *)child->GetActiveDocument();

	//FindTextWord(findStr,doc,true);

	if (findStr == _T(""))
		findStr = appStrFind;
	appStrFind = findStr;

	CFindDlg dlg;
	dlg.SetData(appStrFind,doc);
	dlg.DoModal();
}

void CMainFrame::OnFind() 
{
	FindDlg();
}

void CMainFrame::OnUpdateGenerateLogs(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(appBoolLogAnalyzer);	
}

void CMainFrame::OnGenerateLogs() 
{
   if (appBoolBatchMode && !appBoolLogAnalyzer) {
		if (AfxMessageBox(_T("Log files require that batch mode be turned off. Continue?"),MB_YESNO)==IDYES) {
			appBoolBatchMode ^=1;
		}
		else return;
	}
	if ( appBoolLogAnalyzer && appBoolHighlightText ) {
		if (AfxMessageBox(_T("Turning off log files disables highlighting. Continue?"),MB_YESNO)==IDYES) {
			OnHighlighting();
		}
		else return;
	}
	appBoolLogAnalyzer ^= 1;
}

void CMainFrame::OnUpdateHighlighting(CCmdUI* pCmdUI) 
{
	if (appBoolLogAnalyzer)
		pCmdUI->SetCheck(appBoolHighlightText);
	else {
		appBoolHighlightText ^= 0;
		pCmdUI->SetCheck(appBoolHighlightText);
	}
}

void CMainFrame::OnHighlighting() 
{
	appBoolHighlightText ^= 1;

	if (appBoolHighlightText) {
		if (appBoolLogAnalyzer && !appBoolBatchMode)
			HighlightText();
		else if (!appBoolLogAnalyzer && appBoolBatchMode) {
			if (AfxMessageBox(_T("Highlighting mode requires that log generation be turned on on and batch mode be turned off. Continue?"),MB_YESNO)==IDYES) {
				appBoolBatchMode ^=1;
				appBoolLogAnalyzer ^= 1;
				HighlightText();
			}
			else return;
		}
		else if (appBoolBatchMode) {
			if (AfxMessageBox(_T("Highlighting mode requires that batch mode be turned off. Continue?"),MB_YESNO)==IDYES) {
				appBoolBatchMode ^= 1;
				HighlightText();
			}
			else return;
		}
		else {
			if (!appBoolLogAnalyzer && AfxMessageBox(_T("Highlighting mode requires that log generation be turned on. Continue?"),MB_YESNO)==IDYES) {
				appBoolLogAnalyzer ^= 1;
				HighlightText();
			}
			else return;
		}
		/*
		if (appBoolHighlightText && appBoolLogAnalyzer && !appBoolBatchMode)
			HighlightText();
			*/
	}
	else
		theApp.DoAllViews(NULL,_T("VisualText"),_T(""),DOC_ACTION_UNHIGHLIGHT_ALL);
}

int CMainFrame::FindWordInTextFiles(CString selStr, bool noCase)
{
	int total = 0;
	total += FindWordInDirectoryFiles(selStr,InputPath(),_T(".txt"),noCase);
	total += FindWordInDirectoryFiles(selStr,InputPath(),_T(".htm"),noCase);
	return total;
}

void CMainFrame::StopProcess() 
{
	if (appState == VT_APP_LOADING ||
		appState == VT_APP_WORD_NETTING ||
		appState == VT_APP_SPIDERING) {

		AddDebugLine(_T("User interruption"), _T("Abort processing"));
#ifdef GEODUCK_
		nlp->gui->SetMessage(GUI_MESSAGE_ABORT);
#endif
		appState = VT_APP_NORMAL;
		m_knowledgeToolBar->UpdateWindow();
		SetAnalyzingToolbarButtons(true);
	}
	else if (appState == VT_APP_DIR_ANALYZING ||
		appState == VT_APP_ANALYZING) {
			m_wndAnalyzerView.m_fSuspendAnalyzeThread = true;
//		appState = VT_APP_STOP_COMMAND;
		AddDebugLine(_T("User interruption"), _T("Abort analysis"));
	}
	else if (appState == VT_APP_GENERATING_TREE) {
		appState = VT_APP_STOP_COMMAND;
		AddDebugLine(_T("User interruption"), _T("Stop tree generation"));
	}
}

void CMainFrame::OnAnalyzerGeneratedirty() 
{
	GenAnalyzer(GRAM_GEN_DIRTY,false);	
}

void CMainFrame::OnAnalyzerGenerateunlocked() 
{
	GenAnalyzer(GRAM_GEN_UNLOCKED,false);	
}

void CMainFrame::OnAnalyzerGenerateall() 
{
	GenAnalyzer(GRAM_GEN_ALL,false);	
}

void CMainFrame::RunNext(bool closeOtherWindows) 
{
	appBoolAnalysisPending = true;	// 08/01/03 AM.

	m_strSlogTextPath = m_wndTextView.NextSlog();
	if (m_strSlogTextPath == _T("")) {
		AfxMessageBox(_T("No more text to process!"));
		return;
	}

	AddComboText(m_strSlogTextPath);
	ClearOutput();

	RunAnalyzer(m_strSlogTextPath);
	theApp.UpdateAnalyzerWindows();
	appBoolAnalysisPending = false;	// 08/01/03 AM.
}

void CMainFrame::OnRunNext() 
{
	if (m_strSlogTextPath != _T("") && IsDirectory(InputPath(m_strSlogTextPath)))
		return;
	RunNext();
}

void CMainFrame::OnUpdateRun(CCmdUI* pCmdUI) 
{
	bool status = appBoolAnalyzerLoaded &&
		m_strProcessText != _T("") &&
		!SelectedTextFolderEmpty() &&
		m_textTreeType != TEXT_TREE_LOG &&
		appState != VT_APP_ANALYZING &&
		appState != VT_APP_DIR_ANALYZING &&
		appState != VT_APP_WORD_NETTING &&
		appState != VT_APP_GENERATING ? true : false;

	pCmdUI->Enable(status);		
}

bool CMainFrame::SelectedTextFolderEmpty()
{
	return m_wndTextView.IsSelectedEmptyFolder();
}

void CMainFrame::OnUpdateGenDirtyAndRun(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_strProcessText != _T("") &&
		appState != VT_APP_ANALYZING &&
		appState != VT_APP_DIR_ANALYZING &&
		appState != VT_APP_WORD_NETTING &&
		appState != VT_APP_GENERATING);		
}

void CMainFrame::OnFileArchivesUploadanalyzer() 
{
	OnFileArchivesArchiveanalyzer();
	if (appAskText != _T("")) {
		CString remoteFileStr = StripPath(appAskText,0);
		UploadArchive(_T("Analyzer"),remoteFileStr,appAskText);
	}
}

void CMainFrame::OnFileArchivesServerarchive() 
{
	ServerArchive(_T("Analyzer"));
}

void CMainFrame::OnFileArchivesLocalArchive() 
{
	LocalArchive(_T("Analyzer"));
}

void CMainFrame::OnFileArchivesArchiveanalyzer() 
{
	ClearOutput();

	CArchiveNameDlg archiveNameDlg;
	CString archiveNameStr = ArchiveAutoGenName();
	CString zipFileStr = AppPath(archiveNameStr) + _T(".zip");
	appAskText = zipFileStr;

	archiveNameDlg.DoModal();
	if (appAskText == _T(""))
		return;

	// Save the Analyzer before archiving
//	theApp.DumpAppGUIConcept();	// 06/26/03 AM.
	theApp.SaveAnalyzerStatusFile();
	theApp.SaveKBx(false);			// 10/12/03 AM.
	theApp.SaveAnalyzer(false);

	zipFileStr = appAskText;
	zipFileStr.Replace(_T(" "), _T("_"));
	appAskText = zipFileStr;

	//zipFile zf = zipOpen(zipFileStr,false);
	TCHAR buff[1001];
	StringToChar(buff,zipFileStr,1000);
	HZIP zf = CreateZip(buff, 0, ZIP_FILENAME);

	ZipFiles(zf,AppPath(),_T("*.ana"),zipFileStr,_T(""));
	ZipFiles(zf,AppPath(),_T("*.kb"),zipFileStr,_T("")); // gui hierarchy

	if (appBoolSaveKBFiles)
		ZipFiles(zf,KBPath(_T("user")),_T("*.kb"),zipFileStr,KBPathRel(_T("user")));

	ZipFiles(zf,RulesPath(),_T("*.*"),zipFileStr,RulesPathRel(),true);

	if (appBoolSaveInputFiles)
		ZipFiles(zf,InputPath(),_T("*.*"),zipFileStr,InputPathRel(),true);

	if (appBoolSaveCFiles) {
		ZipFiles(zf,AppPath(),_T("*.cpp"),zipFileStr,_T(""));
		ZipFiles(zf,AppPath(),_T("*.h"),zipFileStr,_T(""));
		ZipFiles(zf,AppPath(),_T("*.dsp"),zipFileStr,_T(""));
		ZipFiles(zf,AppPath(),_T("*.dsw"),zipFileStr,_T(""));
		ZipFiles(zf,AppPath(),_T("*.def"),zipFileStr,_T(""));
		ZipFiles(zf,AppPath(),_T("*.txt"),zipFileStr,_T(""));

		ZipFiles(zf,AppPath(_T("user")),_T("*.cpp"),zipFileStr,_T("user"));
		ZipFiles(zf,AppPath(_T("user")),_T("*.h"),zipFileStr,_T("user"));
		ZipFiles(zf,AppPath(_T("user")),_T("*.dsp"),zipFileStr,_T("user"));
		ZipFiles(zf,AppPath(_T("user")),_T("*.dsw"),zipFileStr,_T("user"));
		ZipFiles(zf,AppPath(_T("user")),_T("*.def"),zipFileStr,_T("user"));
		ZipFiles(zf,AppPath(_T("user")),_T("*.txt"),zipFileStr,_T("user"));

		ZipFiles(zf,KBPath(),_T("*.cpp"),zipFileStr,KBPathRel());
		ZipFiles(zf,KBPath(),_T("*.h"),zipFileStr,KBPathRel());
		ZipFiles(zf,KBPath(),_T("*.dsp"),zipFileStr,KBPathRel());
		ZipFiles(zf,KBPath(),_T("*.dsw"),zipFileStr,KBPathRel());
		ZipFiles(zf,KBPath(),_T("*.def"),zipFileStr,KBPathRel());
		ZipFiles(zf,KBPath(),_T("*.txt"),zipFileStr,KBPathRel());
	}

	CloseZip(zf);
}

void CMainFrame::OnGenDirtyAndRun() 
{
	OnSaveAll();
	m_wndTextView.SetToReslog();
	ClearOutput();
	GenAnalyzer(GRAM_GEN_DIRTY,false);
	RunAnalyzer(m_strSlogTextPath);
	theApp.UpdateAnalyzerWindows(false,true,true);
}

void CMainFrame::OnUpdateGenAnalyzer(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && m_wndGrammarView.TreeItemCount() &&
		appState != VT_APP_ANALYZING &&
		appState != VT_APP_DIR_ANALYZING &&
		appState != VT_APP_GENERATING);		
}

void CMainFrame::OnUpdateTreeDown(CCmdUI* pCmdUI) 
{
	if (appBoolAnalyzerLoaded) {
		WORKSPACE_TAB tab = (WORKSPACE_TAB) GetActiveTab();
		pCmdUI->Enable((tab == WORKSPACE_GRAMMAR && m_wndGrammarView.CanMoveSelectedDown()) ||
			(tab == WORKSPACE_TEXT && m_wndTextView.CanMoveSelectedDown()) ||
			(tab == WORKSPACE_ANALYZER && m_wndAnalyzerView.CanMoveSelectedDown()));	
	}
	else
		pCmdUI->Enable(false);
}

void CMainFrame::OnUpdateTreeUp(CCmdUI* pCmdUI) 
{
	if (appBoolAnalyzerLoaded) {
		WORKSPACE_TAB tab = (WORKSPACE_TAB) GetActiveTab();
		pCmdUI->Enable((tab == WORKSPACE_GRAMMAR && m_wndAnalyzerView.CanMoveSelectedUp()) ||
			(tab == WORKSPACE_TEXT && m_wndTextView.CanMoveSelectedUp()) ||
			(tab == WORKSPACE_ANALYZER && m_wndAnalyzerView.CanMoveSelectedUp()));
	}
	else
		pCmdUI->Enable(false);
}

void CMainFrame::OnUpdateSaveKb(CCmdUI* pCmdUI) 
{
//	pCmdUI->Enable(appBoolAnalyzerLoaded && kbDirty);
	pCmdUI->Enable(appBoolAnalyzerLoaded);
}

void CMainFrame::OnSaveKb() 
{
	if (!AutoSaveKB())
		SaveKB();
	SaveAnalyzerFile();
}

void CMainFrame::OnGenAnalyzer() 
{
	GenAnalyzer(GRAM_GEN_DIRTY,false);
}

void CMainFrame::OnTreeDown() 
{
	switch (GetActiveTab()) {

	case WORKSPACE_GRAMMAR:
		m_wndGrammarView.MoveSelDown();
		break;

	case WORKSPACE_ANALYZER:
		m_wndAnalyzerView.MoveSelDown();
		break;

	case WORKSPACE_TEXT:
		m_wndTextView.MoveSelDown();
		break;
	}
}

void CMainFrame::OnTreeUp() 
{
	switch (GetActiveTab()) {

	case WORKSPACE_GRAMMAR:
		m_wndGrammarView.MoveSelUp();
		break;

	case WORKSPACE_ANALYZER:
		m_wndAnalyzerView.MoveSelUp();
		break;

	case WORKSPACE_TEXT:
		m_wndTextView.MoveSelUp();
		break;
	}
}

void CMainFrame::OnUpdateFileNew(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);
}

void CMainFrame::OnFileNewAnalyzer() 
{
	theApp.CreateNewAnalyzer();
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CMDIFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	UpdateRecentAnalyzers();
	//UpdateRecentFilesPopup(pPopupMenu);
	UpdateDictionaryLookupURLSPopup(pPopupMenu);
}

void CMainFrame::OnDestroy() 
{
	CMDIFrameWnd::OnDestroy();
}

void CMainFrame::OnUpdateComboRecent(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);		
}

void CMainFrame::OnRecentCombobox(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/*  ddxxx move original code here
	int nIndex = m_comboBoxURL->GetCurSel();
	if (nIndex < 0) return;
	CString selectedText;
	m_comboBoxURL->GetLBText(nIndex, selectedText);

	if (selectedText != _T("")) {
		m_comboBoxURL->SetEditText(selectedText);
		BrowserNavigate();
		return;
	}
	*/
}

void CMainFrame::OnUpdateComboURL(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);		
}

void CMainFrame::OnUrlCombobox(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CXTPControlComboBox* comboBoxURL = (CXTPControlComboBox*)m_browserToolBar->GetControl(3);
	CString selectedText = comboBoxURL->GetEditText();

	if (selectedText == _T("about:blank")) {
		int count = comboBoxURL->GetCount();
		int foundIndex = comboBoxURL->FindStringExact(-1,_T("about:blank"));
		comboBoxURL->DeleteItem(foundIndex);
		AfxMessageBox(_T("Blank page"));

	} else if (selectedText != _T("")) {
		selectedText = NormalizeURL(selectedText);
		comboBoxURL->SetEditText(selectedText);
		BrowserNavigate();
		return;
	}
}

void CMainFrame::OnFindForward() 
{
	CVisualTextView *view = (CVisualTextView *)GetFirstView(appDocFocus);
	if (view)
		view->FindForward();
}

void CMainFrame::OnFindBack() 
{
	CVisualTextView *view = (CVisualTextView *)GetFirstView(appDocFocus);
	if (view)
		view->FindBack();
}

void CMainFrame::OnRun() 
{
	Run();
}

void CMainFrame::UpdateRecentFilesPopup(CMenu* pPopupMenu)
{
	int pos;
	int id = 0;
	if (!pPopupMenu) {
		CMenu* mainMenu = GetMenu();
		CMenu* foundMenu;
		if (!FindMenuPos(mainMenu,ID_FILE_NEW,foundMenu,pos))
			return;
		pPopupMenu = mainMenu->GetSubMenu(pos);
		id = ID_FILE_NEW;
	}
	else {
		id = pPopupMenu->GetMenuItemID(0);
	}

	if (id == ID_FILE_NEW) {
		CString menuStr = _T("R&ecent Files");
		if (FindMenuPosFromString(pPopupMenu,menuStr,pos)) {
			CMenu* pSubMenu = pPopupMenu->GetSubMenu(pos);
			VERIFY(pPopupMenu->ModifyMenu(pos,
				(UINT)(appBoolAnalyzerLoaded && theApp.RecentFileListSize() ? MF_ENABLED : MF_GRAYED) |
				MF_BYPOSITION | MF_POPUP | MF_STRING,
				(UINT)pSubMenu->GetSafeHmenu(),menuStr));
		}
	}
}

void CMainFrame::InitVars()
{
	m_strProcessText = _T("");
	m_strSlogTextPath = _T("");
	CString	m_strURLText = _T("");
	appBoolHighlightText = false;
	m_intRecentAnalyzersPos = -1;
	m_pWndFocus = NULL; // SRP 2001-02-01
	m_pAttributeEditor = NULL;
	m_pDocBrowserDictLookup = NULL;
	m_boolBlockOutputTabChange = false;
	m_bFullScreen = FALSE;
	m_pFullScreenLayout = NULL;

	m_rcMainFrame.SetRectEmpty();
}

void CMainFrame::SelectPass(int pass, bool selectTab)
{
	m_wndAnalyzerView.SelectPass(pass);
	m_wndAnalyzerView.SetFocus();
	if (selectTab)
		SelectTab(WORKSPACE_ANALYZER);
}

CONCEPT *CMainFrame::GrammarSampleConcept(CString fullPathStr) 
{
	CString relPathStr = RelativizePath(fullPathStr);
	CONCEPT *gram = cg->findConcept(cg->findRoot(),_T("gram"));
	CONCEPT *sampleConcept = m_wndGrammarView.ConceptWithAttribute(gram,_T("sampfile"),relPathStr);
	return sampleConcept;
}

void CMainFrame::OpenTextOutput()
{
	// FETCH WINDOWS
	CString textFullPathStr = InputPath(m_strProcessText);
	CString outputFullPathStr = textFullPathStr + _T("_log\\output.txt");

	//OPEN WINDOWS
	CVisualTextView *docView = NULL;
	CVisualTextDoc *textOut = NULL;

	if (FileExists(outputFullPathStr)) {
		textOut = (CVisualTextDoc *)theApp.OpenNewDocument(_T("VisualText"),outputFullPathStr);
		docView = (CVisualTextView *)GetFirstView(textOut);
		if (docView->GetParentFrame()->IsIconic())
			docView->GetParentFrame()->ShowWindow(SW_RESTORE);
		else
			docView->GetParentFrame()->ShowWindow(SW_SHOW);
	}
	else
		AfxMessageBox(_T("No ouput file for this text"));
}

void CMainFrame::OpenNextText(bool backward) 
{
	m_strSlogTextPath = m_wndTextView.NextSlog(backward);
	if (m_strSlogTextPath == _T("")) {
		AfxMessageBox(_T("No more text to process!"));
		return;
	}
	AddComboText(m_strSlogTextPath);

	theApp.UpdateAnalyzerWindows();

	m_wndTextView.SelectSlog();
}

void CMainFrame::HighlightMatches(CString logPathStr, CString conceptStr) 
{
	CStringList selList;
	m_wndAnalyzerView.HighlightRuleMatches(selList,logPathStr,conceptStr);
	HighlightMatchesSelections(selList);
}

void CMainFrame::RegenerateSampleFile(CString fullPathStr) 
{
	m_wndGrammarView.RegenerateSampleFile(GrammarSampleConcept(fullPathStr),RelativizePath(fullPathStr));
}

void CMainFrame::OnGoURL() 
{
	BrowserNavigate();	
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CMDIChildWnd *child = MDIGetActive();

	if (child) {
		CString titleStr;
		child->GetWindowText(titleStr);
		SetFrameTitle(titleStr);
	}
}

void CMainFrame::AddChildWindowStates()
{
	BOOL maxim = FALSE;
	CMDIChildWnd *child = MDIGetActive(&maxim);
	CMDIChildWnd *firstChild = child;
	CString strDocName,textStr,tabStr,selStr;
	CVisualTextDoc *doc = NULL;
	CVisualTextView *view = NULL;
	CXTPTabManagerItem* pItem = NULL;
	int sel = 0;

	int c = m_MTIClientWnd.GetItemCount();

	if (c > 0) {
		pItem = m_MTIClientWnd.GetItem(0);
		tabStr = pItem->GetCaption();
		sel = pItem->GetTabManager()->GetSelectedItem()->GetIndex();
		selStr.Format(_T("%d"),sel);
		theApp.AddStatus(_T("doctabsel"),selStr);

		for (int nIndex = 0; nIndex < c; nIndex++) {
			pItem = m_MTIClientWnd.GetItem(nIndex);
			tabStr = pItem->GetCaption();

			if (nIndex == 0) {
				sel = pItem->GetTabManager()->GetSelectedItem()->GetIndex();
			}

			child = MDIGetActive(&maxim);
			firstChild = child;

			while (child) {
				doc = (CVisualTextDoc *)child->GetActiveDocument();
				view = (CVisualTextView *)GetFirstView(doc);
				strDocName = theApp.GetDocumentTypeString(doc);

				child->GetWindowText(textStr);

				if (textStr == tabStr) {
					if (strDocName == _T("Browser"))
						((CBrowserView*)view)->SaveState();
					else if (strDocName == _T("Knowledge"))
						((CKnowledgeView*)view)->SaveState();
					else if (strDocName == _T("ParseTree"))
						((CParseTreeView*)view)->SaveState();
					else if (strDocName == _T("SyntaxEditor"))
						((CSyntaxEditorView*)view)->SaveState();
					else if (strDocName == _T("VisualText"))
						((CVisualTextView*)view)->SaveState();
					break;
				}

				MDINext();
				child = MDIGetActive(&maxim);
				if (child == firstChild)
					child = NULL;
			}
		}

	} else {
		child = MDIGetActive(&maxim);
		firstChild = child;

		while (child) {
			doc = (CVisualTextDoc *)child->GetActiveDocument();
			view = (CVisualTextView *)GetFirstView(doc);
			strDocName = theApp.GetDocumentTypeString(doc);

			child->GetWindowText(textStr);

			if (strDocName == _T("Browser"))
				((CBrowserView*)view)->SaveState();
			else if (strDocName == _T("Knowledge"))
				((CKnowledgeView*)view)->SaveState();
			else if (strDocName == _T("ParseTree"))
				((CParseTreeView*)view)->SaveState();
			else if (strDocName == _T("SyntaxEditor"))
				((CSyntaxEditorView*)view)->SaveState();
			else if (strDocName == _T("VisualText"))
				((CVisualTextView*)view)->SaveState();

			MDINext();
			child = MDIGetActive(&maxim);
			if (child == firstChild)
				child = NULL;
		}
	}
}

void CMainFrame::AutosizeAll() {
	BOOL maxim = FALSE;
	CMDIChildWnd *child = MDIGetActive(&maxim);
	CMDIChildWnd *firstChild = child;
	CString strDocName,textStr,tabStr,selStr;
	CVisualTextDoc *doc = NULL;
	CVisualTextView *view = NULL;
	CXTPTabManagerItem* pItem = NULL;

	child = MDIGetActive(&maxim);
	firstChild = child;

	while (child) {
		doc = (CVisualTextDoc *)child->GetActiveDocument();
		view = (CVisualTextView *)GetFirstView(doc);

		view->AdjustDocSizeToText(doc->GetPathName());

		MDINext();
		child = MDIGetActive(&maxim);
		if (child == firstChild)
			child = NULL;
	}
}

void CMainFrame::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	logmsg(_T("MainFrm::OnRbuttonDown"));	// 05/14/08 AM.

//	CMDIFrameWnd::OnRButtonDown(nFlags, point);
}

void CMainFrame::OnViewTreetextmode()
{
// 07/12/08 AM.
	appBoolTreeText ^= 1;
	theApp.DoAllViews(NULL,_T("ParseTree"),_T(""),DOC_ACTION_UPDATE);
}

void CMainFrame::OnUpdateViewTreetextmode(CCmdUI *pCmdUI)
{
// 07/12/08 AM.
	pCmdUI->SetCheck(appBoolTreeText);
}

void CMainFrame::OnToolsThemeOffice2003theme()
{
	CXTPPaintManager::SetTheme(xtpThemeOffice2003);
	XTP_COMMANDBARS_ICONSINFO* pIconsInfo = XTPPaintManager()->GetIconsInfo();
	pIconsInfo->bUseDisabledIcons = TRUE;

	GetCommandBars()->RedrawCommandBars();
	SaveGUITheme();
}

void CMainFrame::OnUpdateToolsThemeOffice2003theme(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(XTPPaintManager()->GetCurrentTheme() == xtpThemeOffice2003);
}

void CMainFrame::OnToolsThemeVisualStudio2008()
{
	CXTPPaintManager::SetTheme(xtpThemeVisualStudio2008);
	XTP_COMMANDBARS_ICONSINFO* pIconsInfo = XTPPaintManager()->GetIconsInfo();
	pIconsInfo->bUseDisabledIcons = TRUE;

	GetCommandBars()->RedrawCommandBars();
	SaveGUITheme();
}

void CMainFrame::OnUpdateToolsThemeVisualStudio2008(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(XTPPaintManager()->GetCurrentTheme() == xtpThemeVisualStudio2008);
}

void CMainFrame::OnToolsThemeVisualStudio2010()
{
	CXTPPaintManager::SetTheme(xtpThemeVisualStudio2010);
	XTP_COMMANDBARS_ICONSINFO* pIconsInfo = XTPPaintManager()->GetIconsInfo();
	pIconsInfo->bUseDisabledIcons = TRUE;

	GetCommandBars()->RedrawCommandBars();
	SaveGUITheme();
}

void CMainFrame::OnUpdateToolsThemeVisualStudio2010(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(XTPPaintManager()->GetCurrentTheme() == xtpThemeVisualStudio2010);
}

void CMainFrame::SaveGUITheme() {
	int themeInt = (int)XTPPaintManager()->GetCurrentTheme();
	theApp.WriteProfileInt(_T("PREFERENCES"),_T("GUITHEME"),themeInt);
}

void CMainFrame::OnLoadMenuIndirect() {

	return;
	// For simplicity, allocate 500 bytes from stack. May use 
	// GlobalAlloc() to allocate memory bytes from heap.
	BYTE milist[500];
	memset(milist, 0, 500);
	int bytes_left = sizeof(milist);

	// Fill up the MENUITEMTEMPLATEHEADER structure.
	MENUITEMTEMPLATEHEADER* mheader = (MENUITEMTEMPLATEHEADER*) milist;
	mheader->versionNumber = 0;
	mheader->offset = 0;

	int bytes_used = sizeof(MENUITEMTEMPLATEHEADER);
	bytes_left -= bytes_used;

	// Add the following menu items to menu bar:
	// File     Edit
	//   Exit     Copy
	//            Paste
	bytes_used += AddMenuItem(milist + bytes_used, bytes_left, L"&File", 0, 
	  TRUE, FALSE);
	bytes_left -= bytes_used;
	bytes_used += AddMenuItem(milist + bytes_used, bytes_left, L"E&xit", 
	  ID_APP_EXIT, FALSE, TRUE);
	bytes_left -= bytes_used;
	bytes_used += AddMenuItem(milist + bytes_used, bytes_left, L"&Edit", 0, 
	  TRUE, TRUE);
	bytes_left -= bytes_used;
	bytes_used += AddMenuItem(milist + bytes_used, bytes_left, L"&Copy", 
	  ID_EDIT_COPY, FALSE, FALSE);
	bytes_left -= bytes_used;
	bytes_used += AddMenuItem(milist + bytes_used, bytes_left, L"&Paste", 
	  ID_EDIT_PASTE, FALSE, TRUE);
	bytes_left -= bytes_used;

	CMenu m_IndiMenu;
	m_IndiMenu.LoadMenu(IDR_MAINFRAME);
	ASSERT(m_IndiMenu);

	// Load resource from a menu template in memory.
	ASSERT(m_IndiMenu.LoadMenuIndirect(milist));

	// Remove and destroy old menu
	SetMenu(NULL);
	::DestroyMenu(m_hMenuDefault);

	// Add new menu.
	SetMenu(&m_IndiMenu);

	// Assign default menu
	m_hMenuDefault = m_IndiMenu.m_hMenu;
}

UINT CMainFrame::AddMenuItem(LPVOID MenuTemplate, int TemplateBytes, WCHAR* MenuString, 
   WORD MenuID, BOOL IsPopup, BOOL LastItem)
{
   MENUITEMTEMPLATE* mitem = (MENUITEMTEMPLATE*) MenuTemplate;

   UINT  bytes_used = 0;
   if (IsPopup)         // for popup menu
   {      
      if (LastItem)
         mitem->mtOption = MF_POPUP | MF_END;
      else
         mitem->mtOption = MF_POPUP;
      bytes_used += sizeof (mitem->mtOption);  

      mitem = (MENUITEMTEMPLATE*) ((BYTE*) MenuTemplate + bytes_used);
      // a popup doesn't have mtID!!!

      TemplateBytes -= bytes_used;
      wcscpy_s((WCHAR*) mitem, TemplateBytes / sizeof(WCHAR), MenuString);
      bytes_used += (UINT)(sizeof (WCHAR) * (wcslen(MenuString) + 1)); // include '\0'   
   }
   else      // for command item
   {
      mitem->mtOption = LastItem ? MF_END : 0;
      mitem->mtID = MenuID; 
      TemplateBytes -= bytes_used;
      wcscpy_s(mitem->mtString, TemplateBytes / sizeof(WCHAR), MenuString);  
      bytes_used += (UINT)(sizeof (mitem->mtOption ) + sizeof (mitem->mtID) + 
         sizeof (WCHAR) * (wcslen(MenuString) + 1));   // include '\0'
   }

   return bytes_used;
}

void CMainFrame::RecentAnalyzerMenu() {
	CPoint point, ptScreen;
	::GetCursorPos(&ptScreen);
	
	point = ptScreen;

	CMenu menu;
	menu.CreatePopupMenu();

	CStringList analyzerList;
	TokensBySeparator(analyzerList,appRecentAnalyzers,STATUSSEP);
	CStringList newList;
	CString analyzerStr;

	int i = 0;

	UINT disables[100];
	for (int i=0; i<100; i++)
		disables[i] = 0;
	int count = 0;

	for (i=0; i<NUM_RECENT_ANALYZERS; i++) {
		analyzerStr = NthStringListItem(analyzerList,i);
		if (analyzerStr != _T(""))
			menu.InsertMenu(i + 1, MF_BYPOSITION, RECENT_ANALYZERS_STARTID + 1, analyzerStr);
	}

	PopupContextMenu(this,&menu,point,disables);	
}

void CMainFrame::UpdateRecentAnalyzers()
{
	// Locate the color submenu
	CMenu* pTopMenu = AfxGetMainWnd()->GetMenu();
	CMenu *pFileMenu = pTopMenu->GetSubMenu(0);

	int pos = FindMenuItem(pFileMenu, _T("Recent Anal&yzers"));
	CMenu *pRecentAnalyzerMenu = pFileMenu->GetSubMenu(pos);

	if (pRecentAnalyzerMenu == NULL) return;

	CStringList analyzerList;
	TokensBySeparator(analyzerList,appRecentAnalyzers,STATUSSEP);
	CString analyzerStr;

	int i = 0;
	int count = pRecentAnalyzerMenu->GetMenuItemCount();
	for (i = count - 1; i >= 0; i--)
		pRecentAnalyzerMenu->DeleteMenu(i, MF_BYPOSITION);

	for (i=0; i<NUM_RECENT_ANALYZERS; i++) {
		analyzerStr = NthStringListItem(analyzerList,i);
		if (analyzerStr != _T("")) {
			pRecentAnalyzerMenu->AppendMenu(MF_STRING, RECENT_ANALYZERS_STARTID + i, analyzerStr);
			if (analyzerStr == appAnaFilePathStr)
				pRecentAnalyzerMenu->EnableMenuItem(i, MF_BYPOSITION | MF_GRAYED | MF_DISABLED);
		} else
			break;
	}

	CString menuStr;
	//for (i = pRecentAnalyzerMenu->GetMenuItemCount()-1; i > 0; i--) {
	//}
	pRecentAnalyzerMenu->GetMenuString(0, menuStr, MF_BYPOSITION);
	if (menuStr == _T("Recent Analyzer")) {
		pRecentAnalyzerMenu->DeleteMenu(0, MF_BYPOSITION);
	}
}

void CMainFrame::OnUpdateFileRecentanalyzers(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(true);
}

void CMainFrame::SetTabbedWindows(BOOL value)
{
	if (value) {
		VERIFY(m_MTIClientWnd.Attach(this, TRUE));
		m_MTIClientWnd.EnableToolTips();
		m_MTIClientWnd.GetPaintManager()->m_bShowIcons = TRUE;
		m_MTIClientWnd.SetNewTabPosition(xtpWorkspaceNewTabLeftMost);
		m_MTIClientWnd.SetFlags(xtpWorkspaceHideClose | xtpWorkspaceHideArrowsAlways | xtpWorkspaceShowActiveFiles | xtpWorkspaceShowCloseTab);
	}
	else if (m_MTIClientWnd.IsAttached()) {
		m_MTIClientWnd.Detach();
	}
}

void CMainFrame::SetRowCol(int nRow, int nCol)
{
	CString csRowCol;
	csRowCol.Format(_T("Ln %d, Col %d"), nRow, nCol);

	// xxx de Hilster
	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(
		ID_INDICATOR_ROWCOL), csRowCol);
}

void CMainFrame::OnUpdateRowColIndicator(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnShowViewOther(UINT nID)
{
	m_paneManager.ShowPane(nID);
}
