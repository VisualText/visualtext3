/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// VisualText.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "string.h"
#include "sys/stat.h"
#include "afxsock.h"
#include "io.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "mDOCMULTI.cpp"
#include "myDocMgr.cpp"

#include "Utils.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "Resource.h"
#include "BrowserDoc.h"
#include "BrowserView.h"
#include "KnowledgeDoc.h"
#include "KnowledgeView.h"
#include "ParseTreeView.h"
#include "ParseTreeDoc.h"
#include "SyntaxEditorDoc.h"
#include "SyntaxEditorView.h"
#include "TextEditView.h"
#include "TextEditDoc.h"
#include "TreeViewOutput.h"
#include "VisualTextPreferences.h"
#include "NewAnalyzerDialog.h"
#include "SMTP.h"
#include "EmailDlg.h"
#include "RegistrationDlg.h"
#include "UserLogFile.h"
#include "TreeViewGrammar.h"
#include "TreeViewAnalyzer.h"
#include "TreeViewText.h"
#include "WorkspaceView.h"
#include "AboutDlg.h"

#include "VisualTextDoc.h"
#include "VisualTextView.h"
#include "VisualText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString appStrVersion;
CString appStrCopyright = _T("VisualText® Copyright © 1999 - 2018 Text Analysis International, Inc.  All rights reserved. NLP++® Copyright © 2000 - 2018 Text Analysis International, Inc.  All rights reserved.Conceptual Grammar™ Copyright © 1999 - 2017 Text Analysis International, Inc.  All rights reserved. Patents Pending");
CString appStrCopyright2 = _T("Contains WordNet 2.0 Copyright © 1997 by Princeton University. All rights reserved. -- THE WORDNET SOFTWARE AND DATABASE IS PROVIDED \"AS IS\" AND PRINCETON UNIVERSITY MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED. BY WAY OF EXAMPLE, BUT NOT LIMITATION, PRINCETON UNIVERSITY MAKES NO REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE, DATABASE OR DOCUMENTATION WILL NOT INFRINGE ANY THIRD PARTY PATENTS, COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS.");

CString appAnaFilePathStr = _T("");
CString appNameStr = _T("");   // e.g. "corporate" This is a single name which is the .ana file without the extension
CString appDirectoryStr = _T("");   // e.g. "c:/apps" This is a single name which is the .ana file without the extension
CString appSelection = _T("");
CString appURL = _T("");
CString appAskText = _T("");
CString appHelpTopic = _T("");
CString appRecentAnalyzers = _T("");
CString appRunFilePathStr = _T("");
CString appLastURLSelected = _T("");

CString appStrSMTPServer = _T("");
CString appStrFromEmail = _T("");

CString appStrArchiveFTP = _T("");
CString appStrArchiveDirectoryPath = _T("");
CString appStrArchiveAuthor = _T("");
CString appStrArchiveUsername = _T("");
CString appStrArchivePassword = _T("");
CString appStrFind = _T("");
CString appStrFocusDocumentType = _T("");
CString appStrSelText = _T("");
CString appStrAppsDirectory = _T("");
CONCEPT *appGUIConcept = NULL;

CDocument *appDocFocus;

CFont appFontFixed;
CFont appFontFixedSmall;	// 09/09/08 AM.
CFont appFontCopyright;

#ifdef UNICODE
CString appStrFontFixed = _T("Courier New");
CString appStrFontFixedSmall = _T("Lucida Console");  // 06/05/2012 Dd
//CString appStrFontFixed = _T("Arial Unicode MS");
//CString appStrFontFixedSmall = _T("Arial Unicode MS");	// 09/09/08 AM.
#else
CString appStrFontFixed = _T("Courier New");
CString appStrFontFixedSmall = _T("Courier New");	// 09/09/08 AM.
//CString appStrFontFixedSmall = _T("Lucida Console");	// 09/09/08 AM.
#endif

int appRunLevel = -1;
CStringList appStatus;
CONCEPT *appPassConcept = NULL;
NLP *nlp = NULL;
VT_APP_STATE appState = VT_APP_NORMAL;

void *appAttrConcept;

BOOL appBoolKBSafeEditMode = true;

BOOL appBoolTipOfDay = true;	// 05/21/08 AM.
bool appBoolAnalyzerLoaded = false;
bool appBoolSaveInputFiles = false;
bool appBoolSaveKBFiles = false;
bool appBoolSaveLogFiles = false;
bool appBoolSaveCFiles = false;

bool appBoolShowFileToolbar = false;
bool appBoolCompile = false;
bool appBoolDocExists = false;

bool appBoolShowTabsWindow = true;
bool appBoolShowDebugWindow = true;
bool appBoolShowFindWindow = true;

bool appBoolTabbedWindows = false;
bool appBoolToolbarLabels = false;
bool appBoolAutosizeWindows = false;
bool appBoolSizeDynamically = false;
bool appBoolOpeningTextFile = false;

bool appBoolDocDirty = false;
bool appBoolDocDirtyLast = false;
bool appBoolDocsDirty = false;

// If a text analysis is pending.	// 08/01/03 AM.
bool appBoolAnalysisPending = false;	// 08/01/03 AM.

int appIntMaxWindowWidth = 500;
int appIntMaxWindowHeight = 300;
int appIntLastWindowX = 0;
int appIntLastWindowY = 0;
int appIntWindowShiftX = 25;
int appIntWindowShiftY = 25;
int appIntOpenWindowCount = 0;
int appIntRuleTabSize = 4;
int appIntFixedFontSize = 12;
int appIntFixedFontSizeSmall = 11;	// 09/09/08 AM.
int appButtonReturn = 0;

extern CG *cg;
extern RUG *rug;
extern bool kbDirty;
extern _TCHAR *VTLOGFILE;	// 07/24/03 AM.

// APPLICATION specific state variables
bool appBoolHighlightText = false;
bool appBoolLogAnalyzer = false;	// DEFAULT LOGS TOGGLE.	// 06/06/03 AM.
bool appBoolVerbose = false;	// DEFAULT VERBOSE MODE.	// 06/06/03 AM.
bool appBoolPageMode = true;
bool appBoolBatchMode = false;
bool appBoolShowKBToolbar = false;
bool appBoolShowDebugToolbar = false;
bool appBoolShowTabToolbar = false;
bool appBoolShowBrowserToolbar = false;
bool appBoolTreeSemantics = false;
bool appBoolNodeSemantics = false;
bool appBoolTreeOffsets = false;
bool appBoolLoadCompiledKB = true;	// DEFAULT TRY TO LOAD COMPILED KB.	// 03/01/07 AM.
bool appBoolTreeText = true;	// 07/12/08 AM.
CString appFindStateStr = _T("");

/////////////////////////////////////////////////////////////////////////////
// CVisualTextApp

BEGIN_MESSAGE_MAP(CVisualTextApp, CWinApp)
	//{{AFX_MSG_MAP(CVisualTextApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HELP_TIPOFTHEDAY, OnTipOfTheDay)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, OnUpdateFileMruFile1)
	ON_COMMAND(ID_FILE_OPENANALYZER, OnFileOpenanalyzer)
	ON_COMMAND(ID_FILE_CLOSEANALYZER, OnFileCloseanalyzer)
	ON_COMMAND(ID_FILE_SAVEANALYZER, OnFileSaveanalyzer)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSEANALYZER, OnUpdateFileCloseanalyzer)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEANALYZER, OnUpdateFileSaveanalyzer)
	ON_COMMAND(ID_WINDOW_CLOSEALL, OnWindowCloseall)
	ON_COMMAND(ID_ANALYZER_GENERATEUNLOCKED, OnAnalyzerGenerateunlocked)
	ON_COMMAND(ID_ANALYZER_GENERATEALL, OnAnalyzerGenerateall)
	ON_COMMAND(ID_ANALYZER_GENERATEDIRTY, OnAnalyzerGeneratedirty)
	ON_COMMAND(ID_FILE_SAVEANALYZERAS, OnFileSaveanalyzeras)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEANALYZERAS, OnUpdateFileSaveanalyzeras)
	ON_COMMAND(ID_TOOLS_EMAIL, OnToolsEmail)
	ON_COMMAND(ID_PAGE_SETUP, OnPageSetup)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN,OnUpdateFileOpen)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND_RANGE(RECENT_ANALYZERS_STARTID, RECENT_ANALYZERS_ENDID, OnChooseAnalyzer)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVisualTextApp construction

CVisualTextApp::CVisualTextApp()
/*
: m_pMainFrame(0)
, m_pDocTemplate(0)
, m_pDocWorkspTemplate(0)
, m_pChildFrame(0)
*/
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CVisualTextApp::~CVisualTextApp()
{
//	theApp.WriteProfileString(_T("PREFERENCES"),_T("RECENTANALYZERS"),appRecentAnalyzers);
//	theApp.WriteProfileString(_T("PREFERENCES"),_T("LASTANALYZER"),appAnaFilePathStr);
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CVisualTextApp object

CVisualTextApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CVisualTextApp initialization

BOOL CVisualTextApp::InitInstance()
{
	CString commandLineStr = GetCommandLine();

	//_CrtDumpMemoryLeaks();

//   logmsg("Starting VisualText.");	// 10/29/02 AM.
//	logmsg("Enter InitInstance:");	// 07/24/03 AM.
	m_pDocManager = (CDocManager *)  new CMyDocManager ;

	appState = VT_APP_LOADING;

	InitAppGlobals(true);
	LoadFonts();

   /////////////////////////////////////////////////
   // INITIALIZE VISUALTEXT RUNTIME SYSTEM                     // 08/29/02 AM.
   /////////////////////////////////////////////////
	size_t len = 0;

#ifdef _UNICODE
	wchar_t vtpath[500]; 
	_wgetenv_s( &len, vtpath, 500, L"VISUALTEXT" ); 
	if (!IsDirectory(vtpath)) {
		vtpath[0] = NULL;
		_wgetenv_s( &len, vtpath, 500, L"TAIDEV" ); 
	}
#else
	_TCHAR *vtpath = NULL;
	_dupenv_s(&vtpath, &len, _T("VISUALTEXT"));
	if (!IsDirectory(vtpath)) {
		vtpath = NULL;
		_dupenv_s(&vtpath, &len, _T("TAIDEV"));
	}
#endif

//	char *logfile = "C:\\TEMP\\VisualText.log";                 // 08/29/02 AM.
	_TCHAR rfbdir[1001];                                          // 08/29/02 AM.
	_stprintf_s(rfbdir,_T("%s\\data\\rfb\\spec"),vtpath);               // 08/29/02 AM.
//	vtrun = new VTRun(         // GLOBAL VARIABLE FOR RUNTIME.  // 08/29/02 AM.
	logmsg(_T("  Before makeVTRun"));	// 07/24/03 AM.
	vtrun = VTRun::makeVTRun(												// 07/19/03 AM.
		VTLOGFILE,              // Verbose/error log file.       // 07/24/03 AM.
		rfbdir,                 // VisualText dir for RFB spec.  // 08/29/02 AM.
		true                    // Build silently.               // 08/29/02 AM.
		);
    logmsg(_T("Starting VisualText."));	// 10/29/02 AM.
	logmsg(_T("  makeVTRun"));	// 07/24/03 AM.
   /////////////////////////////////////////////////

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return false;
	}

	if(!AfxSocketInit()) {
		AfxMessageBox(_T("Could not initialize Windows Sockets!"));
		return false;
	}

	// CG: The following block was added by the Splash Screen component.
	CCommandLineInfo cmdInfo;

	// CALL REGISTRATION WINDOW

//#define _EVAL_COPY
#ifdef _EVAL_COPY
	CRegistrationDlg dlg;
	dlg.DoModal();
	if (appState == VT_APP_STOP_COMMAND)
		return false;
#endif

	CreateSplash();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MSC_VER <= 1200 // MFC 6.0 or earlier
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif // MFC 6.0 or earlier

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
//	SetRegistryKey(_T("Codejock Software Demos"));	// Commented crapola.	// 05/13/08 AM.
	SetRegistryKey(_T("Text Analysis International"));	// Changed to this.	// 05/13/08 AM.

	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	m_pDocTemplate = new CmyMultiDocTemplate(
		IDI_TEXT,
		RUNTIME_CLASS(CVisualTextDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CVisualTextView));
	AddDocTemplate(m_pDocTemplate);

	m_pVisualText = new CmyMultiDocTemplate(
		IDR_VISUALTYPE,
		RUNTIME_CLASS(CVisualTextDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CVisualTextView));
	AddDocTemplate(m_pVisualText);

	// BLOCK: doc template registration
	m_pBrowser = new CmyMultiDocTemplate(
		IDR_BROWSER_MENU,
		RUNTIME_CLASS(CBrowserDoc),		// document class
		RUNTIME_CLASS(CChildFrame),		// custom MDI child frame
		RUNTIME_CLASS(CBrowserView));		// view class
	AddDocTemplate(m_pBrowser);

	// BLOCK: doc template registration
	m_pKnowledge = new CmyMultiDocTemplate(
		IDR_KNOWLEDGE_MENU,
		RUNTIME_CLASS(CKnowledgeDoc),		// document class
		RUNTIME_CLASS(CChildFrame),		// custom MDI child frame
		RUNTIME_CLASS(CKnowledgeView));		// view class
	AddDocTemplate(m_pKnowledge);

	// BLOCK: doc template registration
	//CmyMultiDocTemplate* pNewDocTemplate = new CmyMultiDocTemplate(
	m_pParseTree = new CmyMultiDocTemplate(
		IDR_PARSETREE,
		RUNTIME_CLASS(CParseTreeDoc),		// document class
		RUNTIME_CLASS(CChildFrame),		// custom MDI child frame
		RUNTIME_CLASS(CParseTreeView));		// view class
	AddDocTemplate(m_pParseTree);

	// BLOCK: doc template registration
	//CmyMultiDocTemplate* pNewDocTemplate = new CmyMultiDocTemplate(
	m_pTextEdit = new CmyMultiDocTemplate(
		IDR_TEXT_EDITOR,
		RUNTIME_CLASS(CTextEditDoc),		// document class
		RUNTIME_CLASS(CChildFrame),		// custom MDI child frame
		RUNTIME_CLASS(CTextEditView));		// view class
	AddDocTemplate(m_pTextEdit);

	m_pSyntaxEditor = new CmyMultiDocTemplate(
		IDR_SYNTAXEDIT_MENU,
		RUNTIME_CLASS(CSyntaxEditorDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CSyntaxEditorView));
	AddDocTemplate(m_pSyntaxEditor);

	LoadPreferences();

	// create main MDI Frame window

	m_pMainFrame = new CMainFrame;
	if (!m_pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = m_pMainFrame;

	LoadRecentAnalyzersString();
	bool autoLoad = GetProfileString(_T("PREFERENCES"),_T("AUTOLOAD"),_T("false")) == _T("true") ? 1 : 0;
	bool analyzerExists = false;
	bool launchedByAnaFile = false;

	if (cmdInfo.m_strFileName.Find(_T(".ana")) > 0) {
		autoLoad = true;
		launchedByAnaFile = true;
		appAnaFilePathStr = cmdInfo.m_strFileName;
		analyzerExists = ReadAnalyzerStatusFile(appAnaFilePathStr);
	}
	else if (autoLoad) {
 		appAnaFilePathStr = GetProfileString(_T("PREFERENCES"),_T("LASTANALYZER"),_T(""));
		analyzerExists = ReadAnalyzerStatusFile(appAnaFilePathStr);
	}
	else {
		m_splashDlg.UpdateMessage(_T("\"Auto-load\" analyzer turned off"));
		Sleep(3000);
	}

	if (autoLoad && analyzerExists) {
		SetAppName();
		// SRP 010628
		if (!UTLFindRunningVTInst(appAnaFilePathStr)) {
			if (!OpenAnalyzer()) {
				CloseAnalyzer();
				appState = VT_APP_NORMAL;
			}
			else {
				appState = VT_APP_LOADED;
			}
		}
	}
	else if (appAnaFilePathStr != _T("") && !analyzerExists && autoLoad)
		AfxMessageBox(_T("Couldn't load last analyzer: " + appAnaFilePathStr));

	if (appState == VT_APP_STOP_COMMAND) {
		CloseAnalyzer();
		DestroySplash();
		m_pMainFrame->DestroyWindow();
		appState = VT_APP_NORMAL;
		return false;
	}

	// Parse command line for standard shell commands, DDE, file open
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew) {
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	}

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return false;

	// MAKE FULL SCREEN IF SET
	// Make full screen by default.	// 05/17/08 AM.
	WINDOWPLACEMENT wp;
	m_pMainFrame->GetWindowPlacement(&wp);
	if (GetProfileString(_T("PREFERENCES"),_T("FULLSCREEN"),_T("true")) == _T("true"))
		wp.showCmd = SW_SHOWMAXIMIZED;
	m_pMainFrame->SetWindowPlacement(&wp);

	m_pMainFrame->SetFrameTitle(); //PD 02/14/2001 to avoid ESP error on onupdateframetitle

	if (!autoLoad)
		appState = VT_APP_NORMAL;

	if (launchedByAnaFile) {
		CDocument *doc = IsVisualTextDocumentOpen(_T("VisualText"),cmdInfo.m_strFileName);
		if (doc) {
			CView *view = GetFirstView(doc);
//			if (!view) return false();	// .NET COMPLAINT.	// 06/11/03 AM.
			if (!view) return false;	// 06/11/03 AM.
			view->GetParentFrame()->DestroyWindow();
		}
	}

	logmsg(_T("Leave InitInstance:"));	// 07/24/03 AM.
	
	// The main window has been initialized, so show and update it.
	m_pMainFrame->ShowWindowEx(m_nCmdShow);
	m_pMainFrame->UpdateWindow();

	DestroySplash();

	// This method handles the 'Tip of the Day' component.
	ShowTipAtStartup();

	m_pMainFrame->m_paneManager.ShowPane(IDR_PANE_LOG);
	m_pMainFrame->m_paneManager.ShowPane(IDR_PANE_TEXT);

	m_pMainFrame->m_wndLogPane.m_listCtrl.AutoSizeColumn();
	m_pMainFrame->m_wndAnalyzePane.m_listCtrl.AutoSizeColumn();

	m_pMainFrame->UpdateRecentAnalyzers();

	return TRUE;
}

void CVisualTextApp::UpdateSyntaxAllViews(CView* pSender/*=NULL*/)
{
	for (POSITION posDoc = m_pSyntaxEditor->GetFirstDocPosition(); posDoc;)
	{
		CXTPSyntaxEditDoc* pDoc = DYNAMIC_DOWNCAST(
			CXTPSyntaxEditDoc, m_pSyntaxEditor->GetNextDoc(posDoc));
		
		if (pDoc)
			pDoc->UpdateAllViews(pSender, xtpEditHintInitView);
	}
}

// App command to run the dialog
void CVisualTextApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CVisualTextApp message handlers

void CVisualTextApp::FindTipsFile(LPCTSTR lpszTipsFile)
{
	// if the tips.txt file does not exist, create one...
	if (_taccess(lpszTipsFile, 0) == -1)
	{
		TRY
		{
			CStdioFile file(lpszTipsFile,
				CFile::modeCreate | CFile::modeWrite | CFile::typeText);

			// After 10 years, some pertinent tips.	// 05/17/08 AM.
			file.WriteString(_T("Tip #1: Technical support is available at (949) 376-8507\n"));
			file.WriteString(_T("What's new? See Help menu > VisualText Help > Getting Started > New in Current Version.\n"));
			file.WriteString(_T("Learn VisualText quickly with the Tutorials in the Help > VisualText Help menu\n"));
			file.WriteString(_T("Sample analyzers are available under the Apps tab of http://www.textanalysis.com\n"));
			file.WriteString(_T("Customize your VisualText settings with the File > Preferences menu.\n"));
			file.WriteString(_T("Make a c:\\apps or d:\\apps folder for your analyzers and point to it in File > Preferences.\n"));
			file.WriteString(_T("Use the Start Button > Control Panels System to edit your APPS environment variable.\n"));
			file.WriteString(_T("Check/edit the TAI and VISUALTEXT environment variables as well.\n"));

			file.WriteString(_T("To update an analyzer, see Help > VisualText Help > Getting Started > Compiled and Standalone Analyzers.\n"));
			file.WriteString(_T("To compile an analyzer, see Help > VisualText Help > VisualText Basics > Compiled and Standalone Analyzers.\n"));
			file.WriteString(_T("See a sample driver program for your analyzer in the PROG C++ project of every analyzer.\n"));
			file.WriteString(_T("Does your analyzer run slowly?  Try turning off the Analyzer > Generate Logs button. Compiling may make the analyzer 2-3 times faster also.\n"));
			file.WriteString(_T("The View > Page Mode toggle enables you to browse and update all your inputs and outputs seamlessly.\n"));
			file.WriteString(_T("Double-click on a parse tree node to get to the rule that created it.\n"));
			file.WriteString(_T("Double-click on a syntax error to get to the line of code that is responsible for it.\n"));			
			file.Close();
		}
		CATCH( CFileException, e )
		{
#ifdef _DEBUG
			afxDump << _T("File could not be opened ") << e->m_cause << _T("\n");
#endif
		}
		END_CATCH
	}
}

void CVisualTextApp::ShowTipAtStartup()
{
	// This method handles the 'Tip of the Day' component.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if (cmdInfo.m_bShowSplash && appBoolTipOfDay)
		OnTipOfTheDay();
}

void CVisualTextApp::OnTipOfTheDay() 
{
	// our tip of the day messages.
	TCHAR szProgramName[ _MAX_PATH ];
	::GetModuleFileName( NULL, szProgramName, _MAX_PATH );

	CSplitPath sp( szProgramName );

	CString strTipsFile;
	strTipsFile.Format(_T("%s%stips.txt"), sp.GetDrive(), sp.GetDir());

	FindTipsFile(strTipsFile);

	CXTPTipOfTheDay dlg(strTipsFile);
	dlg.m_bStartup = appBoolTipOfDay;
    dlg.SetDefaultTitle(_T("     VisualText tip..."));	// Title for tips popup. // 05/17/08 AM.
	dlg.DoModal();
	appBoolTipOfDay = dlg.m_bStartup;
	theApp.WriteProfileString(_T("PREFERENCES"),_T("TIPOFDAY"),BoolStr(appBoolTipOfDay));
}

void CVisualTextApp::OnFileNew() 
{
	m_pDocTemplate->OpenDocumentFile(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CSplitPath

CSplitPath::CSplitPath(LPCTSTR lpszPathBuffer/*=NULL*/)
{
	if (lpszPathBuffer != NULL)
    {
        SplitPath(lpszPathBuffer);
    }
}

CSplitPath::~CSplitPath()
{
	// TODO: add destruction code here.

}

/////////////////////////////////////////////////////////////////////////////

void CSplitPath::SplitPath(LPCTSTR lpszPathBuffer)
{
    ::_tsplitpath_s(lpszPathBuffer, m_szDrive, m_szDir, m_szFName, m_szExt);
}

bool CVisualTextApp::SendOneMessage(CString emailStr, CString subjectStr, CString bodyStr)
{
	if (appStrSMTPServer != _T("") && appStrFromEmail != _T("")) {

		CSMTP smtp(appStrSMTPServer); // You can set server/port later, too.
		CMailMessage msg;
		msg.m_sFrom = appStrFromEmail;
		msg.m_sSubject = subjectStr;
		msg.m_sBody = bodyStr;
		msg.AddMultipleRecipients(emailStr);

		if (!smtp.Connect()) {
			AfxMessageBox(smtp.GetLastError());
			return false;
		}

		if (!smtp.SendMessage(&msg)) {
			AfxMessageBox(smtp.GetLastError());
			return false;
		}

		if(!smtp.Disconnect()) {
			AfxMessageBox(smtp.GetLastError());
			return false;
		}
	}
	else {
		AfxMessageBox(_T("You need to supply the SMTP Server and From Email/nin the Visual Text Preferences."));
		CVisualTextPreferences pref;
		pref.DoModal();
		return false;
	}
	CString msgStr;
	msgStr.Format(_T("Email sent to %s about %s"),emailStr,subjectStr);
	AfxMessageBox(msgStr);
	return true;
}

CView* CVisualTextApp::GetKBEditor(void)
{
	CmyMultiDocTemplate *pTemplate;
	POSITION pos = GetFirstDocTemplatePosition();
	while (pos != NULL) {
		pTemplate = (CmyMultiDocTemplate*) GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CmyMultiDocTemplate)));
		POSITION docPos = pTemplate->CmyMultiDocTemplate::GetFirstDocPosition();
		while (docPos != NULL) {
			CDocument *pDoc = pTemplate->CmyMultiDocTemplate::GetNextDoc(docPos);
			if (pDoc) {
				if (pDoc->IsKindOf(RUNTIME_CLASS(CKnowledgeDoc)) ) {
					return GetFirstView(pDoc);
				}
			}
		}
	}
	return NULL;
}

CDocument *CVisualTextApp::IsVisualTextDocumentOpen(CString docTypeStr, CString filePathStr)
{
	CString strDocName;
	CmyMultiDocTemplate *pTemplate;
	POSITION pos = GetFirstDocTemplatePosition();
	CDocument *doc = NULL;
	POSITION docPos;
	CString docPathStr;

	while (pos != NULL) {
		pTemplate = (CmyMultiDocTemplate*) GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CmyMultiDocTemplate)));
		pTemplate->GetDocString(strDocName,CDocTemplate::docName);
		if (strDocName == docTypeStr) {
			docPos = pTemplate->GetFirstDocPosition();

			while (docPos != NULL) {
				doc = pTemplate->CmyMultiDocTemplate::GetNextDoc(docPos);
				if (doc) {
					docPathStr = doc->GetPathName();
					if (docPathStr.CompareNoCase(filePathStr) == 0)
						return doc;
				}
			}	
		}
	}
	return NULL;
}

bool CVisualTextApp::CreateNewAnalyzer(bool loadTemplate, bool askToCloseCurrentAnalyzer,
									   bool saveAs) 
{
	if (!askToCloseCurrentAnalyzer || !appBoolAnalyzerLoaded || saveAs) {

		CString oldAnaPathStr = appAnaFilePathStr;
		CString oldStrName = appNameStr;
		CString oldStrLocation = appDirectoryStr;

		CNewAnalyzerDialog dlg;
		dlg.EnableTemplates(loadTemplate);
		CString oldAppPath = AppPath();
		CString oldAnalyzerName = appNameStr;

		if (dlg.DoModal() == IDOK) {

			StartWaitCursor();

			// CLOSE CURRENT ANALYZER GRACEFULLY
			CString templatePathStr = appAskText;
			CString newAnaPathStr = appAnaFilePathStr;
			CString newStrName = appNameStr;
			CString newStrLocation = appDirectoryStr;
			appAnaFilePathStr = oldAnaPathStr;
			appNameStr = oldStrName;
			appDirectoryStr = oldStrLocation;
			CloseAnalyzer();
			// RESTORE APP VARIABLES TO USER CHOSEN ANALYZER
			appAnaFilePathStr = newAnaPathStr;
			appNameStr = newStrName;
			appDirectoryStr = newStrLocation;

			// CREATE NEW DIRECTORY

			if (saveAs) {
				templatePathStr = oldAppPath;
			}

			if (templatePathStr != _T("")) {
				CStringList extStrs;
				extStrs.AddTail(_T("zip"));
				CopyDirectory(templatePathStr,AppPath(),extStrs);
			}
			else if (templatePathStr == _T("") && !loadTemplate)
				MakeDirectory(AppPath());

			// RENAME THE ANALYZER PROJECT FILE
			// Not the sequence file.  // 01/10/02 AM.
			// Only copy analyzer project file if saving as, not if creating new.
			if (loadTemplate || saveAs) {
				if (saveAs) {	// FIX.	// 01/10/02 AM.
//					CString origSeq = AnalyzerFile(templatePathStr);	// 01/10/02 AM.
//					if (saveAs)	// 01/10/02 AM.
					CString origSeq = AnalyzerFile(oldAnalyzerName);
					CString newSeq = AnalyzerFile();
					int errorCode = _trename((const _TCHAR *)origSeq, (const _TCHAR *)newSeq);
					if (errorCode) {
						CString msgStr = _T("");
						if (errno == EACCES)
							msgStr.Format(_T("File \"%s\" already exists"),newSeq);
						else if (errno == ENOENT)
							msgStr.Format(_T("File \"%s\" not found"),origSeq);
						else if (errno == EINVAL)		// 01/08/02 AM.
							msgStr.Format(_T("Invalid name: \"%s\""),origSeq);	// 01/08/02 AM.
						else	// 01/08/02 AM.
							msgStr.Format(_T("Error copying project file: \"%s\""),origSeq);	// 01/08/02 AM.
						if (msgStr != _T(""))
							AfxMessageBox(msgStr);
					}
				}

				if (saveAs) {	// 01/10/02 AM.
					CString appStr = appNameStr;
					CString dirStr = appDirectoryStr;
					/*
					char* chDir = appDirectoryStr.GetBuffer(100);
					cg->setAppdir(chDir);
					appDirectoryStr.ReleaseBuffer();
					*/
					CloseAnalyzer();
					appNameStr = appStr;
					appDirectoryStr = dirStr;
				}

				OpenAnalyzer(true);
				m_pMainFrame->m_wndAnalyzerView.SetAnalyzerFileDirty(true);	// Force a save of everything.	// 06/24/03 AM.
				SaveKBx(false);			// 10/12/03 AM.
				SaveAnalyzer(false); // SRP 010505
			}
			StopWaitCursor();
			return true;
		}
	}
	else
		AfxMessageBox(_T("Can't create a new Analyzer without\nclosing the current analyzer."));

	return false;
}

bool CVisualTextApp::OpenAnalyzer(bool newFlag)
{
	logmsg(_T("Enter OpenAnalyzer:"));	// 07/24/03 AM.
	VT_APP_STATE lastState = appState;
	appState = VT_APP_LOADING;

	StartWaitCursor();

	theApp.AddRecentAnalyzer(appAnaFilePathStr);
	theApp.WriteProfileString(_T("PREFERENCES"),_T("RECENTANALYZERS"),appRecentAnalyzers); // SRP 010505
	theApp.WriteProfileString(_T("PREFERENCES"),_T("LASTANALYZER"),appAnaFilePathStr);

	bool splashCreated = false;

	if (!m_splashDlg) {
		CreateSplash();
		m_splashDlg.CenterWindow();
		m_splashDlg.ShowWindow(SW_SHOW);
		splashCreated = true;
		m_pMainFrame->ClearOutput();
	}

	RestoreGlobals(appStatus);

	// LOAD KB - CHECK TO LOAD COMPILED VERSION
	CString msgStr;

	if (appBoolLoadCompiledKB)  // Dd xxx
		msgStr.Format(_T("Loading compiled \"%s\" KB"),appNameStr);
	else
		msgStr.Format(_T("Loading \"%s\" KB"),appNameStr);
	m_splashDlg.UpdateMessage(msgStr);
	m_splashDlg.UpdateWindow();

	if (!LoadKB(appBoolLoadCompiledKB)) {
		if (splashCreated)
			DestroySplash();
		CloseAnalyzer();
		StopWaitCursor(); // SRP 01-01-26
		appState = VT_APP_NORMAL;
		return false;
	}
	LoadAppGUIConcept();

	if (appState == VT_APP_STOP_COMMAND) {
		StopWaitCursor(); // SRP 01-01-26
		return false;
	}

	m_pMainFrame->m_paneManager.ShowPane(IDR_PANE_GRAMMAR);
	m_pMainFrame->m_wndGrammarView.Load();

	if (appState == VT_APP_STOP_COMMAND) {
		StopWaitCursor(); // SRP 01-01-26
		return false;
	}

	msgStr.Format(_T("Loading \"%s\" Analyzer"),appNameStr);

	m_splashDlg.UpdateMessage(msgStr);
	m_splashDlg.UpdateWindow();

	m_pMainFrame->m_paneManager.ShowPane(IDR_PANE_ANALYZER);

	m_pMainFrame->m_wndAnalyzerView.Load();

	m_pMainFrame->m_paneManager.ShowPane(IDR_PANE_ANALYZE);
	m_pMainFrame->m_paneManager.ShowPane(IDR_PANE_LOG);
	m_pMainFrame->m_paneManager.ShowPane(IDR_PANE_FIND);
	m_pMainFrame->m_paneManager.ShowPane(IDR_PANE_FIND2);

	m_pMainFrame->m_wndLogPane.m_listCtrl.AutoSizeColumn();

    cg->setDirty(false);	// After loading kb, reset dirty.	// 07/11/03 AM.
	kbDirty = false; // 07/11/03 AM.

	if (appState == VT_APP_STOP_COMMAND) {
		CloseAnalyzer();
		appState = VT_APP_NORMAL;
		StopWaitCursor(); // SRP 01-01-26
		return false;
	}

	m_splashDlg.UpdateMessage(_T("Finalizing"));
	m_splashDlg.UpdateWindow();

	m_pMainFrame->m_paneManager.ShowPane(IDR_PANE_TEXT);

	m_pMainFrame->m_wndTextView.Load();
	cg->setDirty(false);	// After changing kb, reset dirty.	// 07/11/03 AM.
	kbDirty = false; // 07/11/03 AM.

	//m_pMainFrame->m_wndTextView.SetRedraw(false);	// Trying to eliminate flicker in text tab.	// 03/16/07 AM.  ddxxx
	RestoreViews(appStatus);
	//m_pMainFrame->m_wndTextView.SetRedraw(true);	// Trying to eliminate flicker in text tab.	// 03/16/07 AM.  ddxxx
	appStatus.RemoveAll();
	if (splashCreated)
		DestroySplash();

	StopWaitCursor();

	if (lastState != VT_APP_NORMAL)
		appState = lastState;
	else
		appState = VT_APP_LOADED;

	appBoolAnalyzerLoaded = true;

	DisplayAnalyzerLoadStats(true);

	m_pMainFrame->SetFrameTitle(); //PD 02/14/2001 from onupdateframetitle
	
	m_pMainFrame->m_paneManager.RedrawPanes();  // 2011/10/05 Dd

	logmsg(_T("Leave OpenAnalyzer:"));	// 07/24/03 AM.

	return true;
}

CDocument *CVisualTextApp::OpenNewDocument(const CString &strTarget, CString filePath, bool showFlag)
{ 
	CString strDocName;
	CmyMultiDocTemplate *pTemplate;
	POSITION pos = GetFirstDocTemplatePosition();
	//CDocument *doc;
	CVisualTextDoc *doc; 
	CString docTitleStr;
	CString docPathStr;

	_TCHAR fileChar[1001];
	StringToChar(fileChar,strTarget,1000);

	/*
 	if (!filePath.IsEmpty() && filePath.Find(_T("http://")) != 0 ) { // SRP 010307
		CString strMsg;
		if (!FileExists(filePath)) {
			strMsg.Format(_T("The file:\n\n%s\n\ndoes not exist."), filePath);
			AfxMessageBox(strMsg);
			return NULL;
		}
		struct _stati64 buf;
		__int64 result = _tstati64(filePath, &buf);
		__int64 fileSize = buf.st_size;
		// RAISING THE LIMIT HERE.	// 06/29/03 AM.
//		if (fileSize >= 0x1000000) {  // (16777216) 16MB - 24 bytes 	// 06/29/03 AM.
		if (fileSize >= 0x4000000) {  // 64 MB	// 06/29/03 AM.
			strMsg.Format(_T("The file:\n\n%s\n\nis too large to open."), filePath);
			AfxMessageBox(strMsg);
			return NULL;
		}
		else if (fileSize >= 0x400000) { (4194304) 4MB - 22 bytes
			strMsg.Format(_T("The file:\n\n%s\n\nis very large and will take a long time to open. Continue?"), filePath);
			if (IDNO == AfxMessageBox(strMsg, MB_YESNO))
				return NULL;
		}
	} // SRP 010307
	*/

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	while (pos != NULL) {
		pTemplate = (CmyMultiDocTemplate*) GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CmyMultiDocTemplate)));
		pTemplate->GetDocString(strDocName,CDocTemplate::docName);
 
		if (strDocName == strTarget) {
			POSITION docPos = pTemplate->CmyMultiDocTemplate::GetFirstDocPosition();

			// If doc exists, bring to front
			while (docPos != NULL) {
				doc = (CVisualTextDoc *)pTemplate->CmyMultiDocTemplate::GetNextDoc(docPos);
				if (doc) {

					docTitleStr = doc->GetTitle();
					docPathStr = doc->GetPathName();
					//if (docTitleStr.CompareNoCase(filePath) == 0 ||

					if (docTitleStr.CompareNoCase(filePath) == 0 ||
						docPathStr.CompareNoCase(filePath) == 0) {
						// BRING TO FRONT
						CView *view = GetFirstView(doc);
						if (view){
							view->GetParentFrame()->BringWindowToTop();
							appBoolDocExists = true;

							//read only management
							if( _taccess(filePath,0) != -1 && _taccess( filePath, 2 ) == -1 ) {
								//where we have a read only file we need to keep track of that
								if (strTarget == _T("VisualText")) {
									CVisualTextDoc* vtDoc = (CVisualTextDoc*)doc;
	//								vtDoc->SetReadOnly(true);
								}
							}	
							
							return doc;
						}
					}
				}
			}
			appBoolDocExists = false;

			if (strDocName == _T("Knowledge")) {
				doc = (CVisualTextDoc *)pTemplate->CmyMultiDocTemplate::OpenDocumentFile(NULL,showFlag, IDI_KB);
			}
			else if (strDocName == _T("ParseTree")) {
				doc = (CVisualTextDoc *)pTemplate->CmyMultiDocTemplate::OpenDocumentFile(NULL,showFlag, IDI_TREE);
			}
			else if (strDocName == _T("SyntaxEditor")) {
				doc = (CVisualTextDoc *)pTemplate->CmyMultiDocTemplate::OpenDocumentFile((LPCTSTR)filePath,showFlag, IDI_PAT);
			}
			else if (strDocName == _T("Browser")) {
				appURL = filePath;
				doc = (CVisualTextDoc *)pTemplate->CmyMultiDocTemplate::OpenDocumentFile(NULL,showFlag, IDI_GLOBE);
			}
			else if (strDocName == _T("TextEditor")) {
				doc = (CVisualTextDoc *)pTemplate->CmyMultiDocTemplate::OpenDocumentFile(NULL,showFlag);
			}
			else  {
				int iconID = IDI_TEXT;
				if (EndsWith(filePath, ".pat"))
					iconID = IDI_PAT;
				doc = (CVisualTextDoc *)pTemplate->CmyMultiDocTemplate::OpenDocumentFile((LPCTSTR)filePath,showFlag, iconID);
				if (doc) {
					//We can't actually safely open a file when we haven't got
					//an analyzer open. PD 02/26/2001
					CVisualTextView *view = (CVisualTextView *)GetFirstView(doc);
					if (appState != VT_APP_NORMAL) {
						if (showFlag) {
							if (!appBoolTabbedWindows && appBoolAutosizeWindows) {
								view->AdjustDocSizeToText(filePath);
							}
						}
						else {
							view->GetParentFrame()->SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);
						}
					}
					if (filePath == InputPath(wnd->GetProcessText())) {
						wnd->HighlightText();
					}
				}
				else
					return NULL;
			}

			//read only management
			if( _taccess(filePath,0) != -1 && _taccess( filePath, 2 ) == -1 ) {
				//where we have a read only file we need to keep track of that
				if (strTarget == _T("VisualText") && doc) {
					CVisualTextDoc* vtDoc = (CVisualTextDoc*)doc;
					LPCTSTR docTitle;
					CString newTitle;
					docTitle = vtDoc->GetTitle();
					newTitle = (CString)docTitle + (CString)_T(" (read only)");
					vtDoc->SetTitle(newTitle);
//					vtDoc->SetReadOnly(true);
				}
			}

			return doc;
		}
	}
	return NULL;
}
int CVisualTextApp::GetNumberOfOpenViews(void)
{
	int nViews = 0;
	CmyMultiDocTemplate *pTemplate;
	POSITION pos = GetFirstDocTemplatePosition();
	while (pos != NULL) {
		pTemplate = (CmyMultiDocTemplate*) GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CmyMultiDocTemplate)));
		POSITION docPos = pTemplate->CmyMultiDocTemplate::GetFirstDocPosition();
		while (docPos != NULL) {
			CDocument *pDoc = pTemplate->CmyMultiDocTemplate::GetNextDoc(docPos);
			if (pDoc) {
				if (pDoc->IsKindOf(RUNTIME_CLASS(CVisualTextDoc)) ||
				 pDoc->IsKindOf(RUNTIME_CLASS(CBrowserDoc)) ||
				 pDoc->IsKindOf(RUNTIME_CLASS(CKnowledgeDoc)) ) {
					if (GetFirstView(pDoc))
						nViews++;
				}
			}
		}
	}
	return nViews;
}

void CVisualTextApp::AddRecentAnalyzer(CString newAnalyzerStr)
{
	if (appRecentAnalyzers == _T("")) {
		appRecentAnalyzers = newAnalyzerStr;
		return;
	}

	CString newAnalyzerLowerStr = newAnalyzerStr;
	newAnalyzerLowerStr.MakeLower();

	//PD 02/19/2001
	//don't record unloaded archives with the zip suffix
	int pos=0;
	if (!newAnalyzerLowerStr.IsEmpty() &&
		(pos = newAnalyzerLowerStr.Find(_T(".zip"))) &&
		(pos >= 0)
	 ) {
		newAnalyzerLowerStr.SetAt(pos,'\0');
	}

	CStringList analyzerList;
	TokensBySeparator(analyzerList,appRecentAnalyzers,STATUSSEP);
	CStringList uniqueList,uniqueLowerList;
	CString analyzerLowerStr;
	CString analyzerStr;
	int count = analyzerList.GetCount();

	//analyzerList.AddHead(newAnalyzerLowerStr);
	//UniqueStringList(analyzerList);

	// BUILD A UNIQUE LIST
	int i = 0;

	for (i=0; i<NUM_RECENT_ANALYZERS && i<count; i++) {
		analyzerStr = NthStringListItem(analyzerList,i);
		analyzerLowerStr = analyzerStr;
		analyzerLowerStr.MakeLower();

		if (analyzerStr != _T("") && analyzerStr.GetLength() > 2 &&
			!uniqueLowerList.Find(analyzerLowerStr) &&
			analyzerLowerStr != newAnalyzerLowerStr) {

			uniqueList.AddTail(analyzerStr);
			uniqueLowerList.AddTail(analyzerLowerStr);
		}

		if (uniqueList.GetCount() > NUM_RECENT_ANALYZERS) {
			uniqueList.RemoveHead();
			uniqueLowerList.AddTail(analyzerLowerStr);
		}
	}

	// CLIP THE NUMBER TO ONE LESS THEN THE MAX NUMBER TO ADD THE newAnalyerStr
	if (newAnalyzerStr != _T("") && uniqueList.GetCount() >= NUM_RECENT_ANALYZERS)
		uniqueList.RemoveTail();
		//uniqueList.RemoveHead();

	// REBUILD THE appRecentAnalyzers GLOBAL VARIABLE
	appRecentAnalyzers = _T("");
	count = uniqueList.GetCount();
	for (i=0; i<count; i++) {
		analyzerStr = NthStringListItem(uniqueList,i);
		analyzerLowerStr = NthStringListItem(uniqueLowerList,i);
		if (appRecentAnalyzers == _T(""))
			appRecentAnalyzers = analyzerStr;
		else if (analyzerLowerStr != newAnalyzerLowerStr) //PD 02/19/2001
			appRecentAnalyzers += STATUSSEP + analyzerStr;
	}

	// ADD NEW ANALYZER TO THE LIST TOP OF THE LIST
	if (newAnalyzerStr != _T("")) {
		if (appRecentAnalyzers == _T(""))
			appRecentAnalyzers = newAnalyzerStr;//PD 02/19/2001
		else if (appRecentAnalyzers.MakeLower() != newAnalyzerLowerStr)//PD 02/19/2001
			appRecentAnalyzers = newAnalyzerStr + STATUSSEP + appRecentAnalyzers;//PD 02/19/2001
	}
}

void CVisualTextApp::RemoveRecentAnalyzer(CString oldAnalyzerStr)
{
	if (appRecentAnalyzers == _T("")) {
		appRecentAnalyzers = oldAnalyzerStr;
		return;
	}

	CStringList analyzerList;
	TokensBySeparator(analyzerList,appRecentAnalyzers,STATUSSEP);
	CStringList newList;
	CString analyzerStr;
	int pos = -1;
	int i = 0;

	for (i=0; i<NUM_RECENT_ANALYZERS; i++) {
		analyzerStr = NthStringListItem(analyzerList,i);
		if (analyzerStr != _T("") && analyzerStr != oldAnalyzerStr && !newList.Find(analyzerStr))
			newList.AddTail(analyzerStr);
	}

	appRecentAnalyzers = _T("");
	int count = newList.GetCount();
	for (i=0; i<count; i++) {
		analyzerStr = NthStringListItem(newList,i);
		if (appRecentAnalyzers == _T(""))
			appRecentAnalyzers = analyzerStr;
		else
			appRecentAnalyzers += STATUSSEP + analyzerStr;
	}
	theApp.WriteProfileString(_T("PREFERENCES"),_T("RECENTANALYZERS"),appRecentAnalyzers); // SRP 010505
}

bool CVisualTextApp::ReadAnalyzerStatusFile(CString filePathStr)
{
	appStatus.RemoveAll();
	if (!ReadStatusFile(filePathStr))
		return false;

	appDirectoryStr = StripPath(filePathStr,1,true);
	CString rightStr = StripPath(filePathStr,1);
	NthPathSegment(rightStr,appNameStr,'\\',0);

	return true;
}

CString CVisualTextApp::GetAppStatusValue(CString keyStr)
{
	POSITION pos = appStatus.GetHeadPosition();
	CString statusValue;
	CString key;
	CString segOneStr;

	while (pos && (statusValue = appStatus.GetNext(pos))) {
		NthPathSegment(statusValue,key,STATUSSEP,0);
		NthPathSegment(statusValue,segOneStr,STATUSSEP,1);
		if (key == keyStr)
			return segOneStr;
	}

	return _T("");
}

void CVisualTextApp::SavePageModeState()
{
	CString filePathStr;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	CString strDocName;
	CmyMultiDocTemplate *pTemplate;
	POSITION pos = GetFirstDocTemplatePosition();
	POSITION docPos;
	CString stateStr;
	bool appendFlag = false;

	while (pos != NULL) {
		pTemplate = (CmyMultiDocTemplate*) GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CmyMultiDocTemplate)));
		pTemplate->GetDocString(strDocName,CDocTemplate::docName);

		if (strDocName == _T("VisualText")) {
			CVisualTextDoc *doc = NULL;
			CVisualTextView *view = NULL;
			docPos = pTemplate->CmyMultiDocTemplate::GetFirstDocPosition();

			while (docPos != NULL) {
				doc = (CVisualTextDoc *)pTemplate->GetNextDoc(docPos);
				if (!doc)
					continue;
				view = (CVisualTextView *)GetFirstView(doc);
				if (!view)
					continue;
				stateStr = _T("docopen");
				stateStr += STATUSSEP + view->StateStr();
				m_pMainFrame->m_wndTextView.SetPageModeState(stateStr,appendFlag);
				appendFlag = true;
			}
		}
		else if (strDocName == _T("SyntaxEditor")) {
			CSyntaxEditorDoc *doc = NULL;
			CSyntaxEditorView *view = NULL;
			docPos = pTemplate->CmyMultiDocTemplate::GetFirstDocPosition();

			while (docPos != NULL) {
				doc = (CSyntaxEditorDoc *)pTemplate->GetNextDoc(docPos);
				if (!doc)
					continue;
				view = (CSyntaxEditorView *)GetFirstView(doc);
				if (!view)
					continue;
				stateStr = _T("ruleopen");
				stateStr += STATUSSEP + view->StateStr();
				m_pMainFrame->m_wndTextView.SetPageModeState(stateStr,appendFlag);
				appendFlag = true;
			}
		}
		else if (strDocName == _T("Browser")) {
			CBrowserDoc *doc = NULL;
			CBrowserView *view = NULL;
			docPos = pTemplate->GetFirstDocPosition();

			while (docPos != NULL) {
				doc = (CBrowserDoc *)pTemplate->GetNextDoc(docPos);
				if (!doc)
					continue;
				view = (CBrowserView *)GetFirstView(doc);
				if (!view)
					continue;
				stateStr = _T("browseropen");
				stateStr += STATUSSEP + view->StateStr();
				m_pMainFrame->m_wndTextView.SetPageModeState(stateStr,appendFlag);
				appendFlag = true;
			}
		}
		else if (strDocName == _T("ParseTree")) {
			CParseTreeDoc *doc = NULL;
			CParseTreeView *view = NULL;
			docPos = pTemplate->GetFirstDocPosition();

			while (docPos != NULL) {
				doc = (CParseTreeDoc *)pTemplate->GetNextDoc(docPos);
				if (!doc)
					continue;
				view = (CParseTreeView *)GetFirstView(doc);
				if (!view)
					continue;
				stateStr = _T("treeopen");
				stateStr += STATUSSEP + view->StateStr();
				m_pMainFrame->m_wndTextView.SetPageModeState(stateStr,appendFlag);
				appendFlag = true;
			}
		}
	}

	// CLEAR IF NO WINDOWS
	if (!appendFlag)
		m_pMainFrame->m_wndTextView.SetPageModeState(_T(""),false);
}


void CVisualTextApp::CloseAnalyzer()
{
	logmsg(_T("Enter CloseAnalyzer:"));	// 07/24/03 AM.
	VT_APP_STATE lastState = appState;

	//PD 02/20/2001
	//If we hit cancel button during a load, we don't
	//want to keep any changes that may have happened
	//as a result of the load
	if (VT_APP_STOP_COMMAND == appState)
		appState = VT_APP_EXITING;

	StartWaitCursor(); // SRP 01-01-30
	CMainFrame *mainFrame = (CMainFrame *)AfxGetMainWnd();
	bool analyzerJustStarted = !appBoolAnalyzerLoaded &&
		(m_pMainFrame->m_wndAnalyzerView.IsAnalyzerFileDirty() || kbDirty);
	
	if ((!analyzerJustStarted && appBoolAnalyzerLoaded && mainFrame->AnalyzerCreated()) ||
		(analyzerJustStarted && mainFrame->AnalyzerCreated())) {

//		DumpAppGUIConcept();	// 06/24/03 AM.
		SaveAnalyzerStatusFile(); // All info should now be saved automatically

		CloseAllViews(); // OnDestroy() for each view saves off window state info!

        AfxGetThreadState()->m_pRoutingFrame = NULL;

		//PD 02/20/2001
		// Prompt the user to save any changes,
		// EXCEPT when the user has hit the cancel button during a load,
		if (appState != VT_APP_STOP_COMMAND) {
			bool loadCompiledKB = appBoolLoadCompiledKB;
			if (loadCompiledKB && kbDirty) {
				CString msgStr = _T("The KB has changed since it was last compiled.  Do you want to compile the KB before exiting?");
			// Default for compiling kb is NO.	// 09/02/08 AM.
				if (AfxMessageBox(msgStr,MB_YESNO|MB_DEFBUTTON2) == IDYES) {
					KBCompile();
				}
			}
			SaveKBx(true);			// 10/12/03 AM.
			SaveAnalyzer(true);
			theApp.WriteProfileString(_T("PREFERENCES"),_T("RECENTANALYZERS"),appRecentAnalyzers);
			theApp.WriteProfileString(_T("PREFERENCES"),_T("LASTANALYZER"),AnalyzerFile());
		}
		kbDirty = false;
	
		m_pMainFrame->m_wndGrammarView.Clear();
		m_pMainFrame->m_wndAnalyzerView.Clear();
		m_pMainFrame->m_wndTextView.Clear();
		mainFrame->ClearOutput();
		mainFrame->SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);

//		if (cg)
//			delete cg;
		logmsg(_T("  Before deleteCG"));	// 07/24/03 AM.
		vtrun->deleteCG(cg);	// 07/18/03 AM.
		cg = NULL;
		logmsg(_T("  After deleteCG"));	// 07/24/03 AM.
		if (nlp)	// 07/18/03 AM.
			{
			logmsg(_T("  Before nlp zero CG"));	// 07/24/03 AM.
			nlp->setCG(0);	// 07/18/03 AM.
			logmsg(_T("  After nlp zero CG"));	// 07/24/03 AM.
			}
		//mainFrame->ClearTextCombo();  ddxxx

		InitAppGlobals();
		mainFrame->SetFrameTitle(); // goes AFTER InitAppGlobals()
	}
	if (appState == VT_APP_EXITING)
		appState = VT_APP_NORMAL;
	else
		appState = lastState;

	mainFrame->UpdateWindow();
	StopWaitCursor(); // SRP 01-01-30
	logmsg(_T("Leave CloseAnalyzer:"));	// 07/24/03 AM.
}

void CVisualTextApp::AddStatus(CString nameStr, CString valueStr)
{
	CString statusStr = _T("");
	statusStr.Format(_T("%s%c%s"),nameStr,STATUSSEP,valueStr);
	POSITION pos = appStatus.Find(statusStr);
	if (pos == NULL)
		appStatus.AddTail(statusStr);
}

void CVisualTextApp::AddStatus(CString nameStr, int valInt)
{
	CString valStr;
	valStr.Format(_T("%d"),valInt);
	AddStatus(nameStr,valStr);
}

void CVisualTextApp::UpdateAnalyzerWindows(bool forceFlag, bool changeBrowser,
										   bool updatingCurrent)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	CString filePathStr = wnd->GetProcessText();
	if (IsDirectory(InputPath(filePathStr)))
		return;

	CString strDocName;
	CmyMultiDocTemplate *pTemplate;
	POSITION pos = GetFirstDocTemplatePosition();
	CVisualTextDoc *doc = NULL;
	POSITION docPos;
	CString docPathStr;
	CString docRelPathStr;
	CString dirStr;
	CString dirFileStr;
	CString docFileStr;
	CString fileStr;
	CString fileUrlStr;
	CString textStr;
	int scrollPos = 0;

	CBrowserDoc *browserDoc = NULL;
	CBrowserDoc *firstBrowserDoc = NULL;
	CString browserDocPathStr = _T("");
	CString localFilePath = _T("");
	bool browserLocal = false;
	CString processText = wnd->GetProcessText();
	CString processTextDir = StripPath(processText,0,true);

	CVisualTextDoc *docOpen;

	while (pos != NULL) {
		pTemplate = (CmyMultiDocTemplate*) GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CmyMultiDocTemplate)));
		pTemplate->GetDocString(strDocName,CDocTemplate::docName);

		if (strDocName == _T("VisualText")) {
			docPos = pTemplate->CmyMultiDocTemplate::GetFirstDocPosition();
			int fileLength;
			CString msgStr;

			while (docPos != NULL) {

				doc = (CVisualTextDoc *)pTemplate->GetNextDoc(docPos);
				if (!doc)
					continue;
				if (doc->m_DocType != DOC_LOG && doc->m_DocType != DOC_FILE && doc->m_DocType != DOC_HTML)
					continue;
				CVisualTextView *visualTextView = (CVisualTextView*)GetFirstView(doc);
				if (!visualTextView)
					continue;

				docPathStr = doc->GetPathName();

				if (doc->IsModified()) {
					CString msgStr;
					msgStr.Format(_T("The open document: %s\nhas been modified!"), docPathStr);
					if (appBoolPageMode)
						AskUser(msgStr,IDR_WAVE_LOSTBOY,3,1,_T("&Save File and Continue"),_T("&Continue (and Lose Edits)"),_T("Turn &Off Page Mode"));
					else
						AskUser(msgStr,IDR_WAVE_LOSTBOY,2,1,_T("&Save File and Continue"),_T("&Continue (and Lose Edits)"), _T(""));
					switch (appButtonReturn) {
						case 1:
							doc->OnSaveDocument(docPathStr);
							visualTextView->GetRichEditCtrl().SetModify(false);
							break;
						case 3:
							appBoolPageMode = false;
							return;
						case 2:
						default:
							break;
					}
				}

				docRelPathStr = RelativizePath(docPathStr);
				ClipLeft(docRelPathStr,6);

				// DON'T UPDATE THE TEXT FILE YOU A PROCESSING UNLESS FLAG IS SET
				if (updatingCurrent && docRelPathStr.CompareNoCase(processText) == 0)
					continue;

				if (docRelPathStr.Find(_T("_log")) > 0)
					dirStr = StripPath(docRelPathStr,1,true);
				else
					dirStr = StripPath(docRelPathStr,0,true);
				dirFileStr = StripPath(docRelPathStr,0,true);
				docFileStr = StripPath(docRelPathStr,0);
				fileStr = StripPath(filePathStr,0);


				if ((forceFlag || docRelPathStr.CompareNoCase(filePathStr)) &&
					dirStr.CompareNoCase(processTextDir) == 0) {

					if (docRelPathStr.Find(_T("_log\\")) >= 0)
						docPathStr = StripPath(dirFileStr,0,true)+
							_T("\\")+fileStr+_T("_log\\")+docFileStr;
					else
						docPathStr = dirFileStr+_T("\\")+fileStr;

					// IF ALREADY OPEN, USE THAT ONE
					docPathStr = InputPath(docPathStr);
					docOpen = (CVisualTextDoc *)IsVisualTextDocumentOpen(_T("VisualText"),docPathStr);
					if (docOpen) {
						doc = docOpen;
						visualTextView = (CVisualTextView*)GetFirstView(doc);
						if (!visualTextView)
							continue;
					}

					// CHECK FOR VERY LONG FILES
					fileLength = GetFileLength(docPathStr) / 1000;
					if (fileLength > LARGE_TEXT_SIZE) {
						msgStr.Format(_T("The file \"%s\" is very large (%d k).  Opening will take a lot of time.\n\nDo you wish to proceed?"),fileStr,fileLength);
						if (AfxMessageBox(msgStr, MB_YESNO) != IDYES)
							continue;
					}

					// FILL WINDOW WITH NEW TEXT
					doc->SetPathName(docPathStr,false);
					textStr = NLPReadInputFile(docPathStr);	// 08/20/08 AM.
					visualTextView->SetWindowText(textStr);
					visualTextView->UpdateWindow();

					visualTextView->Scroll(scrollPos);
					if (doc->m_DocType == DOC_RULE)
						visualTextView->FormatAll();
			
					visualTextView->GetRichEditCtrl().SetModify(false);
				}
			}
		}

		else if (strDocName == _T("ParseTree")) {
			docPos = pTemplate->GetFirstDocPosition();
			while (docPos != NULL) {
				CParseTreeDoc *parseTreeDoc = (CParseTreeDoc *)pTemplate->GetNextDoc(docPos);
				if (!parseTreeDoc)
					continue;
				CParseTreeView *parseTreeView = (CParseTreeView *)GetFirstView(parseTreeDoc);
				if (!parseTreeView)
					continue;
				CString logPathStr = parseTreeView->GetLogPathStr();
				CString logStr = StripPath(logPathStr,0);
				CString newLogPathStr = InputPath(filePathStr) + _T("_log\\") + logStr;
				if (!appBoolAnalysisPending &&	// 08/01/03 AM.
					(forceFlag || logPathStr.CompareNoCase(newLogPathStr)) ) {
					parseTreeView->ChangeData(newLogPathStr,filePathStr);
					//parseTreeView->SetData(newLogPathStr,filePathStr);
					parseTreeView->ExpandOneLevel();
				}
			}
		}
		else if (changeBrowser && !browserDoc && strDocName == _T("Browser")) {
			CString docPathStr;
			docPos = pTemplate->GetFirstDocPosition();
			while (docPos != NULL) {
				CBrowserDoc *bDoc = (CBrowserDoc *)pTemplate->GetNextDoc(docPos);
				if (!bDoc)
					continue;
				docPathStr = bDoc->GetUrl();
				fileUrlStr = GetUrlFromUrlFile(InputPath(filePathStr));

				if (docPathStr.Find(_T("file")) == 0) {
					browserLocal = true;
					localFilePath = filePathStr;
					continue;
				}
				if (!firstBrowserDoc) {
					firstBrowserDoc = bDoc;
					browserDocPathStr = docPathStr;
				}
				if (docPathStr.CompareNoCase(fileUrlStr) != 0) {
					browserDocPathStr = fileUrlStr;
					browserDoc = bDoc;
				}
			}
		}

		else if (strDocName == _T("Knowledge")) {
			CString docPathStr;
			docPos = pTemplate->GetFirstDocPosition();
			while (docPos != NULL) {
				CKnowledgeDoc *kDoc = (CKnowledgeDoc *)pTemplate->GetNextDoc(docPos);
				if (!kDoc)
					continue;
				CKnowledgeView *kView = (CKnowledgeView *)GetFirstView(kDoc);
				if (!kView)
					continue;
				kView->Restore();
			}	

		}
	}

	if (browserLocal) {
		appLastURLSelected = localFilePath;
		wnd->BrowserNavigate(InputPath(localFilePath));
	}
	else if (browserDoc) {
		appLastURLSelected = browserDocPathStr;
		wnd->BrowserNavigate(browserDocPathStr,false,browserDoc);
	}
	else if (firstBrowserDoc) {
		appLastURLSelected = browserDocPathStr;
		wnd->BrowserNavigate(browserDocPathStr,false,firstBrowserDoc);
	}
	
	if (appBoolHighlightText)
		wnd->HighlightText();
}

void CVisualTextApp::UpdateParseTrees(int newPass)
{
	CString filePathStr;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	if (filePathStr == _T("")) {
		filePathStr = wnd->GetProcessText();
		if (IsDirectory(InputPath(filePathStr)))
			return; 
	}
	CString strDocName;
	CmyMultiDocTemplate *pTemplate;
	POSITION pos = GetFirstDocTemplatePosition();
	CVisualTextDoc *doc = NULL;
	POSITION docPos;

	while (pos != NULL) {
		pTemplate = (CmyMultiDocTemplate*) GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CmyMultiDocTemplate)));
		pTemplate->GetDocString(strDocName,CDocTemplate::docName);

		if (strDocName == _T("ParseTree")) {
			docPos = pTemplate->CmyMultiDocTemplate::GetFirstDocPosition();
			CParseTreeDoc *parseTreeDoc;
			CParseTreeView *parseTreeView;

			while (docPos != NULL) {
				parseTreeDoc = (CParseTreeDoc *)pTemplate->GetNextDoc(docPos);
				if (!parseTreeDoc)
					continue;
				parseTreeView = (CParseTreeView *)GetFirstView(parseTreeDoc);
				if (!parseTreeView)
					continue;
//				CString newLogPathStr = InputPath(filePathStr) + "_log" + OutAnaPath("",newPass);
				CString newLogPathStr;	// 07/02/02 AM.
				if (appBoolLogAnalyzer)	// If logs toggle is on.	// 07/02/02 AM.
					newLogPathStr = InputPath(filePathStr) + _T("_log") + OutAnaPath(_T(""),newPass);	// 07/02/02 AM.
				else
					newLogPathStr = InputPath(filePathStr) + _T("_log") + _T("\\final.log");	// 07/02/02 AM.
				parseTreeView->ChangeData(newLogPathStr,filePathStr);
				parseTreeView->ExpandOneLevel();
			}
		}
	}
}

CView *CVisualTextApp::GetView(CString docType)
{
	CDocument *doc;
	CView *docView = NULL;

	if ((doc = GetDocument(docType))) {
		docView = GetFirstView(doc);
	}
	return docView;
}

void CVisualTextApp::DoAllViews(CView *activeView, CString docType, CString extStr,
								DOC_ACTION docAction)
{
	CMainFrame *mainFrame = (CMainFrame*)AfxGetMainWnd();
	CString strDocName;
	CDocTemplate *pTemplate;
	POSITION pos = GetFirstDocTemplatePosition();
	CVisualTextDoc *doc = NULL;
	CString docTitleStr;
	CString docPathStr;
	CString textStr;

	while (pos != NULL) {
		pTemplate = (CDocTemplate*) GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CDocTemplate)));
		pTemplate->GetDocString(strDocName,CDocTemplate::docName);

		CString msgStr;
		CString titleStr;
		int scrollPos;

		if (strDocName == docType ||
			docAction == DOC_ACTION_DESTROY_ALL_BUT ||
			docAction == DOC_ACTION_SAVE_STATE
			) {
			POSITION docPos = pTemplate->GetFirstDocPosition();
			while (docPos != NULL) {
				doc = (CVisualTextDoc *)pTemplate->GetNextDoc(docPos);
				if (!doc)
					continue;
				docTitleStr = doc->GetTitle();
				docPathStr = doc->GetPathName();
				if (extStr == _T("") || EndsWith(docTitleStr,extStr) ||
					EndsWith(docPathStr,extStr) ||
					docAction == DOC_ACTION_DESTROY_ALL_BUT ||
					docAction == DOC_ACTION_UNHIGHLIGHT_ALL
					) {
					CView* view = GetFirstView(doc);
					CVisualTextView *visualTextView = (CVisualTextView*)view;
					if (visualTextView) {
						switch (docAction) {
						case DOC_ACTION_AUTOSIZE:
							visualTextView->AdjustDocSizeToText(docPathStr);
							break;
						case DOC_ACTION_UPDATE_FONT:
							LoadFonts();
							visualTextView->GetRichEditCtrl().SetFont(&appFontFixed);
						case DOC_ACTION_UPDATE:
							if (strDocName == _T("ParseTree")) {
								CParseTreeView *parseTreeView = (CParseTreeView *)view;
								if (!parseTreeView)
									continue;
								CString stateStr = parseTreeView->StateStr();
								parseTreeView->FillParseTree();
								CString expandStr,selectStr;
								NthPathSegment(stateStr,selectStr,STATUSSEP,9);
								NthPathSegment(stateStr,expandStr,STATUSSEP,10);
								parseTreeView->SetState(expandStr,STATE_EXPAND_COLLAPSE);
								parseTreeView->SetState(selectStr,STATE_SELECT);
							}
							else {
								scrollPos = visualTextView->GetRichEditCtrl().GetFirstVisibleLine();
								textStr = NLPReadFile(docPathStr);
								visualTextView->SetWindowText(textStr);
								visualTextView->UpdateWindow();
								if (doc->m_DocType == DOC_RULE)
									visualTextView->FormatAll();
								visualTextView->Scroll(scrollPos);
								visualTextView->GetRichEditCtrl().SetModify(false);
							}
							break;
						case DOC_ACTION_UPDATE_AUTO_GENERATED:
							if (visualTextView->IsAutoGenerated()) {
								scrollPos = visualTextView->GetRichEditCtrl().GetFirstVisibleLine();
								textStr = NLPReadFile(docPathStr);
								visualTextView->SetWindowText(textStr);
								visualTextView->UpdateWindow();
								if (doc->m_DocType == DOC_RULE)
									visualTextView->FormatAll();
								visualTextView->Scroll(scrollPos);
								visualTextView->GetRichEditCtrl().SetModify(false);
							}
							break;
						case DOC_ACTION_SAVE:
						case DOC_ACTION_SAVE_CONFIRM:
							titleStr = docTitleStr;
							ClipRight(titleStr,2);
							msgStr.Format(_T("Save changes to \"%s\""),titleStr);
							if (doc->IsModified() &&
								(docAction != DOC_ACTION_SAVE_CONFIRM ||
								 AfxMessageBox(msgStr,MB_YESNO) == IDYES)) {
								doc->OnSaveDocument(doc->GetPathName());
							}
							break;
						case DOC_ACTION_SAVE_STATE:
							if (strDocName == _T("Browser"))
								((CBrowserView*)view)->SaveState();
							else if (strDocName == _T("Knowledge"))
								((CKnowledgeView*)view)->SaveState();
							else if (strDocName == _T("ParseTree"))
								((CParseTreeView*)view)->SaveState();
							else if (strDocName == _T("SyntaxEditor"))
								((CSyntaxEditorView*)view)->SaveState();
							else if (strDocName == _T( "VisualText"))
								((CVisualTextView*)view)->SaveState();
							break;
						case DOC_ACTION_MINIMIZE:
							view->GetParentFrame()->ShowWindow(SW_MINIMIZE);
							break;
						case DOC_ACTION_DESTROY_ALL_BUT:
							if (view == activeView)
								break;
						case DOC_ACTION_DESTROY_ALL:
							view->GetParentFrame()->DestroyWindow();
							break;
						case DOC_ACTION_UNHIGHLIGHT_ALL:
							// ONLY CLEAR FORMAT IN TEXTS IN INPUT
							if (docPathStr.Find(InputPath()) == 0)
								visualTextView->ClearFormat();
							break;
						}
					}
				}
			}
		}
	}

	int zzz = 0;
}

void CVisualTextApp::SaveAllStates() 
{
	DoAllViews(NULL,_T("SyntaxEditor"),_T(".pat"),DOC_ACTION_SAVE_STATE);
	DoAllViews(NULL,_T("VisualText"),_T(""),DOC_ACTION_SAVE_STATE);
	DoAllViews(NULL,_T("Browser"),_T(""),DOC_ACTION_SAVE_STATE);
	DoAllViews(NULL,_T("Knowledge"),_T(""),DOC_ACTION_SAVE_STATE);
	DoAllViews(NULL,_T("ParseTree"),_T(""),DOC_ACTION_SAVE_STATE);
	DoAllViews(NULL,_T("SyntaxEditor"),_T(""),DOC_ACTION_SAVE_STATE);
}

void CVisualTextApp::UpdateDocFromPath(CString docPathStr)
{
	CVisualTextDoc *doc = (CVisualTextDoc *)IsVisualTextDocumentOpen(_T("VisualText"),docPathStr);
	if (doc)
		doc->Update();
}

bool CVisualTextApp::AnalyzerUsingFile(CString filePathStr)
{
	CString rulesPath = RulesPath();
	CString filePath = StripPath(filePathStr,0,true);
	return rulesPath == filePath;
}

void CVisualTextApp::SetAnalyzerRuleFileModified(CString pathStr)
{
	m_pMainFrame->m_wndAnalyzerView.SetPassDirty(pathStr);
}

void CVisualTextApp::RunAnalyzer()
{
	CMainFrame *mainFrame = (CMainFrame *)AfxGetMainWnd();
	mainFrame->Run();
}

CDocument *CVisualTextApp::ViewLogFile(CDocument *doc)
{
	if (!doc)
		return NULL;
	return m_pMainFrame->m_wndAnalyzerView.GetLogFromDocument(doc);
}

CDocument *CVisualTextApp::IsDocumentTypeOpen(CString docTypeStr)
{
	CString strDocName;
	CmyMultiDocTemplate *pTemplate;
	POSITION pos = GetFirstDocTemplatePosition();
	CDocument *doc = NULL;
	CString docPathStr;

	while (pos != NULL) {
		pTemplate = (CmyMultiDocTemplate*) GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CmyMultiDocTemplate)));
		pTemplate->GetDocString(strDocName,CDocTemplate::docName);
		if (strDocName == docTypeStr) {
			doc = pTemplate->CmyMultiDocTemplate::GetNextDoc(pos);
			return doc;
		}
	}
	return NULL;
}

CBrowserDoc *CVisualTextApp::IsBrowserDocumentOpen(CString filePathStr)
{
	CString strDocName;
	CmyMultiDocTemplate *pTemplate;
	POSITION pos = GetFirstDocTemplatePosition();
	CBrowserDoc *doc = NULL;
	POSITION docPos;
	CString docPathStr;

	while (pos != NULL) {
		pTemplate = (CmyMultiDocTemplate*) GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CmyMultiDocTemplate)));
		pTemplate->GetDocString(strDocName,CDocTemplate::docName);
		if (strDocName == _T("Browser")) {
			docPos = pTemplate->GetFirstDocPosition();

			while (docPos != NULL) {
				doc = (CBrowserDoc *)pTemplate->CmyMultiDocTemplate::GetNextDoc(docPos);
				if (doc) {
					docPathStr = doc->GetUrl();
					if (docPathStr.CompareNoCase(filePathStr) == 0)
						return doc;
				}
			}	
		}
	}
	return NULL;
}

CString CVisualTextApp::GetDocumentTypeString(CDocument *docToCompare)
{
	CDocument *doc = NULL;
	POSITION docPos;
	POSITION curTemplatePos = GetFirstDocTemplatePosition();

	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);

		docPos = curTemplate->GetFirstDocPosition();

		while (docPos != NULL) {
			doc = curTemplate->GetNextDoc(docPos);
			if (doc == docToCompare)
				return str;
		}
	}
	return _T("");
}

bool CVisualTextApp::DirtyDocs(CDocument *docIn)
{
	CDocument *doc = NULL;
	POSITION docPos;
	POSITION curTemplatePos = GetFirstDocTemplatePosition();

	while (curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if (str == _T("SyntaxEditor") || str == _T("VisualText"))
		{
			docPos = curTemplate->GetFirstDocPosition();
			while (docPos != NULL) {
				doc = curTemplate->GetNextDoc(docPos);
				if (!docIn && doc->IsModified())
					return true;
				else if (doc == docIn && doc->IsModified())
					return true;
			}
		}
	}

	return false;
}

bool CVisualTextApp::UpdateRuleFileTitles()
{
	CString strDocName;
	CmyMultiDocTemplate *pTemplate;
	POSITION pos = GetFirstDocTemplatePosition();
	CVisualTextDoc *doc = NULL;
	POSITION docPos;
	CString docPathStr;
	bool updatedFlag = false;

	while (pos != NULL) {
		pTemplate = (CmyMultiDocTemplate*) GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CmyMultiDocTemplate)));
		pTemplate->GetDocString(strDocName,CDocTemplate::docName);
		if (strDocName == _T("VisualText")) {
			docPos = pTemplate->GetFirstDocPosition();

			while (docPos != NULL) {
				doc = (CVisualTextDoc *)pTemplate->CmyMultiDocTemplate::GetNextDoc(docPos);
				if (doc && doc->m_DocType == DOC_RULE) {
					doc->SetTitle(_T(""));
					updatedFlag = true;
				}
			}	
		}
	}
	return updatedFlag;
}

#define WINBUFF_SIZE 1024

bool CVisualTextApp::SaveAnalyzerStatusFile()
{
	if (appBoolAnalyzerLoaded) {
		appStatus.RemoveAll();
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

		// ADD COMBO TEXT FOR MOST RECENT TEXT FILES
		/*  ddxxx
		int count = wnd->m_comboBoxRecent->GetCount();
		CString str;

		// Trying to limit the amount of text tree expansion.	// 05/20/08 AM.
		if (count > 1)	// 05/20/08 AM.
			count = 1;	// 05/20/08 AM.

		for (int i=0; i<count; i++) {
			wnd->m_comboBoxRecent->GetLBText(i,str);
			theApp.AddStatus(_T("addcombotext"),str);
		}
		*/

		AddStatus(_T("attributeeditor"),wnd->GetAttributeDlg() ? _T("true") : _T("false"));

		wnd->AddChildWindowStates();

		AddStatus(_T("processfile"),wnd->GetProcessText());

		if (wnd->GetActiveTab() != WORKSPACE_NONE) {
			AddStatus(_T("activetab"),wnd->GetActiveTab());
			AddStatus(_T("grammarsel"),m_pMainFrame->m_wndGrammarView.GetSelection());
			AddStatus(_T("grammarstate"),m_pMainFrame->m_wndGrammarView.GetState());
			AddStatus(_T("analyzersel"),m_pMainFrame->m_wndAnalyzerView.GetSelection());
			AddStatus(_T("analyzerstate"),m_pMainFrame->m_wndAnalyzerView.GetState());
			AddStatus(_T("textsel"),m_pMainFrame->m_wndTextView.GetSelection());
			AddStatus(_T("textstate"),m_pMainFrame->m_wndTextView.GetState());

			// ADD STATES FOR PAGE MODE
			/*
			CString stateStr = wnd->GetTextPageModeState();
			CStringList states;
			StringToStringListString(states,stateStr,"<<SEP>>");
			POSITION pos = states->GetHeadPosition();
			CString valueStr;
			while (pos && (stateStr = states->GetNext(pos))) {
				if (stateStr != "")
					AddStatus("textpagemodestate",stateStr);
			}
			delete states;
			*/
		}
		AddStatus(_T("highlighttext"),BoolStr(appBoolHighlightText));
		AddStatus(_T("loganalyzer"),BoolStr(appBoolLogAnalyzer));
		AddStatus(_T("verbosemode"),BoolStr(appBoolVerbose));
		AddStatus(_T("pagemode"),BoolStr(appBoolPageMode));
		AddStatus(_T("batchmode"),BoolStr(appBoolBatchMode));
		AddStatus(_T("kbtoolbar"),BoolStr(appBoolShowKBToolbar));
		AddStatus(_T("debugtoolbar"),BoolStr(appBoolShowDebugToolbar));
		AddStatus(_T("tabtoolbar"),BoolStr(appBoolShowTabToolbar));
		AddStatus(_T("browsertoolbar"),BoolStr(appBoolShowBrowserToolbar));
		AddStatus(_T("treesemantics"),BoolStr(appBoolTreeSemantics));
		AddStatus(_T("treeoffsets"),BoolStr(appBoolTreeOffsets));
		AddStatus(_T("loadcompiledkb"),BoolStr(appBoolLoadCompiledKB));
		AddStatus(_T("findstate"),appFindStateStr);

		// DO REVERSE SO THAT FLAGS ARE SET BEFORE STORING WINDOW STATES
		POSITION pos = appStatus.GetTailPosition();
		CString contentStr = _T("");
		CString statusValue;

		while (pos && (statusValue = appStatus.GetPrev(pos))) {
			contentStr += statusValue;
			contentStr += _T("\n");
		}
		
		CString fileStr = AnalyzerFile();
		appStatus.RemoveAll();
		bool bSuccess = NLPWriteFile(fileStr,contentStr);
		wnd->AddDebugLine(_T("Saving Status File"),bSuccess?_T("Success"):_T("Failed"));
		return bSuccess;
	}
	return false;
}

void CVisualTextApp::RestoreGlobals(CStringList &stateList)
{
	appBoolHighlightText = false;
	appBoolLogAnalyzer = false;	// DEFAULT LOGS TOGGLE.	// 06/06/03 AM.
	appBoolVerbose = false;	// DEFAULT VERBOSE MODE.	// 06/06/03 AM.
	appBoolPageMode = true;
	appBoolBatchMode = false;
	appBoolShowKBToolbar = false;
	appBoolShowDebugToolbar = false;
	appBoolShowTabToolbar = false;
	appBoolShowBrowserToolbar = false;
	appBoolTreeSemantics = false;
	appBoolTreeOffsets = false;
	appBoolLoadCompiledKB = true;	// DEFAULT TRY TO LOAD COMPILED KB.	// 03/01/07 AM.
	appFindStateStr = _T("");

	POSITION pos = stateList.GetHeadPosition();
	CString statusValue;
	CString docPath;
	CRect rect;
	CRect tabrect;
	CString key;
	CString segOneStr;
	CString workStr;
	int segOneInt;
	int segDone;
	bool boolVal;

	while (pos && (statusValue = stateList.GetNext(pos))) {
		segDone = NthPathSegment(statusValue,key,STATUSSEP,0);
		NthPathSegment(statusValue,segOneStr,STATUSSEP,1);
		boolVal = (bool)BoolValueVT(segOneStr);

		if(IsDigit(segOneStr))
			NthPathSegmentInt(statusValue,segOneInt,STATUSSEP,1);

		if (key == _T("highlighttext")) {
			appBoolHighlightText = boolVal;
		}
		else if (key == _T("loganalyzer")) {
			appBoolLogAnalyzer = boolVal;
		}
		else if (key == _T("verbosemode")) {
			appBoolVerbose = boolVal;
		}
		else if (key == _T("pagemode")) {
			appBoolPageMode = boolVal;
		}
		else if (key == _T("batchmode")) {
			appBoolBatchMode = boolVal;
		}
		else if (key == _T("kbtoolbar")) {
			appBoolShowKBToolbar = boolVal;
		}
		else if (key == _T("debugtoolbar")) {
			appBoolShowDebugToolbar = boolVal;
		}
		else if (key == _T("tabtoolbar")) {
			appBoolShowTabToolbar = boolVal;
		}
		else if (key == _T("browsertoolbar")) {
			appBoolShowBrowserToolbar = boolVal;
		}
		else if (key == _T("treesemantics")) {
			appBoolTreeSemantics = boolVal;
		}
		else if (key == _T("treeoffsets")) {
			appBoolTreeOffsets = boolVal;
		}
		else if (key == _T("loadcompiledkb")) {
			appBoolLoadCompiledKB = boolVal;
		}
		else if (key == _T("findstate")) {
			appFindStateStr = segOneStr;
		}
	}
}

void CVisualTextApp::RestoreViews(CStringList &stateList, bool createIfDoesNotExist)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	CList<CVisualTextView *,CVisualTextView *> views;
	POSITION pos = stateList.GetHeadPosition();
	CVisualTextDoc *doc;
	CVisualTextView *view;
	CString statusValue;
	CString docPath;
	CRect rect;
	CRect tabrect;
	int vertPos;
	int active;
	long start;
	long end;
	CString key;
	CString segOneStr;
	CString workStr;
	int tabSelect = 0;
	int segOneInt = 0;
	int segDone = 0;
	int count = 0;
	bool boolVal;
	bool fileExists;
	bool firstTextModeState = true;

	// Only adding a limited number of combotext items, till I can figure out
	// how to keep the tree from expanding for each one of them.	// 05/21/08 AM.
	int countcombos = 0;	// 05/21/08 AM.
	int maxcombos = 1;	// 05/21/08 AM.
	int seg = 0;

	while (pos && (statusValue = stateList.GetNext(pos))) {
		segDone = NthPathSegment(statusValue,key,STATUSSEP,0);
		NthPathSegment(statusValue,segOneStr,STATUSSEP,1);
		boolVal = (bool)BoolValueVT(segOneStr);

		seg = 0;

		if(IsDigit(segOneStr))
			NthPathSegmentInt(statusValue,segOneInt,STATUSSEP,1);

		if (key == _T("docopen")) {
			NthPathSegment(statusValue,docPath,STATUSSEP,1);

			// CHECK TO SEE IF THE ANALYZER FOLDER WAS MOVED AND CORRECT PATH
			CString movePathStr = docPath;
			if (MovedPathStr(movePathStr))
				docPath = movePathStr;

			fileExists = FileExists(docPath);
			if (!createIfDoesNotExist && !fileExists)
				continue;

			if (createIfDoesNotExist && !fileExists)
				NLPWriteFile(docPath,_T(" "));

			doc = (CVisualTextDoc *)OpenDocumentFile(docPath,false);
			if (!doc)
				continue;

			NthPathSegmentInt(statusValue,vertPos,STATUSSEP,6);
			NthPathSegmentInt(statusValue,active,STATUSSEP,7);
			NthPathSegmentLong(statusValue,start,STATUSSEP,8);
			NthPathSegmentLong(statusValue,end,STATUSSEP,9);

			view = (CVisualTextView *)GetFirstView(doc);
			if (!view)
				continue;
			CRichEditCtrl &edit = view->GetRichEditCtrl();
			//edit.GetLineCount();

			if (!appBoolTabbedWindows) {
				NthPathSegmentLong(statusValue,rect.top,STATUSSEP,2);
				NthPathSegmentLong(statusValue,rect.bottom,STATUSSEP,3);
				NthPathSegmentLong(statusValue,rect.left,STATUSSEP,4);
				NthPathSegmentLong(statusValue,rect.right,STATUSSEP,5);
				view->GetParentFrame()->MoveWindow(rect,true);
			}

			edit.LineScroll(vertPos);
			edit.SetSel(start,end);

			view->GetParentFrame()->ShowWindow(SW_SHOW);
			views.AddTail((CVisualTextView *)view);
		}
		else if (key == _T("ruleopen")) {
			NthPathSegment(statusValue,docPath,STATUSSEP,1);

			// CHECK TO SEE IF THE ANALYZER FOLDER WAS MOVED AND CORRECT PATH
			CString movePathStr = docPath;
			if (MovedPathStr(movePathStr))
				docPath = movePathStr;

			fileExists = FileExists(docPath);
			if (!createIfDoesNotExist && !fileExists)
				continue;

			if (createIfDoesNotExist && !fileExists)
				NLPWriteFile(docPath,_T(" "));

			CSyntaxEditorDoc *doc = (CSyntaxEditorDoc *)OpenNewDocument(_T("SyntaxEditor"),docPath,false);
			if (!doc)
				continue;

			int startline = 0;
			int startcol = 0;
			int endline = 0;
			int endcol = 0;

			NthPathSegmentInt(statusValue,vertPos,STATUSSEP,6);
			NthPathSegmentInt(statusValue,active,STATUSSEP,7);
			NthPathSegmentInt(statusValue,startline,STATUSSEP,8);
			NthPathSegmentInt(statusValue,startcol,STATUSSEP,9);
			NthPathSegmentInt(statusValue,endline,STATUSSEP,10);
			NthPathSegmentInt(statusValue,endcol,STATUSSEP,11);

			//POSITION docViewPos = doc->GetFirstViewPosition();
			//return doc->GetNextView(docViewPos);

			CSyntaxEditorView *view = (CSyntaxEditorView *)GetFirstView(doc);

			if (!view)
				continue;
			CXTPSyntaxEditCtrl &edit = view->GetEditCtrl();

			if (!appBoolTabbedWindows) {
				NthPathSegmentLong(statusValue,rect.top,STATUSSEP,2);
				NthPathSegmentLong(statusValue,rect.bottom,STATUSSEP,3);
				NthPathSegmentLong(statusValue,rect.left,STATUSSEP,4);
				NthPathSegmentLong(statusValue,rect.right,STATUSSEP,5);
				view->GetParentFrame()->MoveWindow(rect,true);
			}

			edit.Scroll(0, vertPos - 1);
			edit.Select(startline, startcol, endline, endcol);

			view->GetParentFrame()->ShowWindow(SW_SHOW);
			views.AddTail((CVisualTextView *)view);
		}
		else if (key == _T("treeopen")) {
			CString logPathStr;
			CString textPathStr;
			CString conceptStr;
			long longStart;
			long longEnd;
			NthPathSegment(statusValue,logPathStr,STATUSSEP,1);
			NthPathSegment(statusValue,textPathStr,STATUSSEP,2);
			NthPathSegment(statusValue,conceptStr,STATUSSEP,3);
			CString newStrPath = logPathStr;
			if (MovedPathStr(newStrPath))
				logPathStr = newStrPath;
			if (!FileExists(logPathStr))
				continue;
			NthPathSegmentLong(statusValue,longStart,STATUSSEP,4);
			NthPathSegmentLong(statusValue,longEnd,STATUSSEP,5);

			CString selectedStr;
			CString stateStr;
			NthPathSegment(statusValue,selectedStr,STATUSSEP,6);
			NthPathSegment(statusValue,stateStr,STATUSSEP,7);

			CParseTreeDoc *doc = (CParseTreeDoc *)OpenNewDocument(_T("ParseTree"),logPathStr,false);
			if (!doc)
				continue;
			CParseTreeView *view = (CParseTreeView *)GetFirstView(doc);
			if (!view)
				continue;
			view->SetData(logPathStr,textPathStr,conceptStr,longStart,longEnd);
			view->SetState(stateStr,STATE_EXPAND_COLLAPSE);
			view->SetState(selectedStr,STATE_SELECT);

			if (!appBoolTabbedWindows) {
				NthPathSegmentLong(statusValue,rect.top,STATUSSEP,6);
				NthPathSegmentLong(statusValue,rect.bottom,STATUSSEP,7);
				NthPathSegmentLong(statusValue,rect.left,STATUSSEP,8);
				NthPathSegmentLong(statusValue,rect.right,STATUSSEP,9);
				view->GetParentFrame()->MoveWindow(rect,true);
			}

			view->GetParentFrame()->ShowWindow(SW_SHOW);
			views.AddTail((CVisualTextView *)view);
		}
		else if (key == _T("kbopen")) {
			CString conceptPathStr;
			bool boolOneRootNode;

			NthPathSegment(statusValue,conceptPathStr,STATUSSEP,1);
			NthPathSegmentBool(statusValue,boolOneRootNode,STATUSSEP,2);

			CString selectedStr;
			CString stateStr;
			NthPathSegment(statusValue,selectedStr,STATUSSEP,7);
			NthPathSegment(statusValue,stateStr,STATUSSEP,8);

			// SRP 2001-02-01 Set showFlag to "true" so that OnInitialUpdate() will called.
			// Fixes Title Bar Error
			CKnowledgeDoc *doc = (CKnowledgeDoc *)OpenNewDocument(_T("Knowledge"),_T(""),true);
			if (!doc)
				continue;
			CKnowledgeView *view = (CKnowledgeView *)GetFirstView(doc);
			if (!view)
				continue;
			view->SetData(conceptPathStr,boolOneRootNode);
			view->SetState(stateStr,STATE_EXPAND_COLLAPSE);
			view->SetState(selectedStr,STATE_SELECT);

			if (!appBoolTabbedWindows) {
				NthPathSegmentLong(statusValue,rect.top,STATUSSEP,3);
				NthPathSegmentLong(statusValue,rect.bottom,STATUSSEP,4);
				NthPathSegmentLong(statusValue,rect.left,STATUSSEP,5);
				NthPathSegmentLong(statusValue,rect.right,STATUSSEP,6);
				view->GetParentFrame()->MoveWindow(rect,true);
			}

			view->GetParentFrame()->ShowWindow(SW_SHOW);
			views.AddTail((CVisualTextView *)view);
		}
		else if (key == _T("browseropen")) {
			NthPathSegment(statusValue,docPath,STATUSSEP,1);
			CString newStrPath = docPath;
			if (MovedPathStr(newStrPath))
				docPath = newStrPath;

			CBrowserDoc *doc = (CBrowserDoc *)OpenNewDocument(_T("Browser"),docPath,false);
			if (!doc)
				continue;

			NthPathSegmentInt(statusValue,vertPos,STATUSSEP,6);
			NthPathSegmentInt(statusValue,active,STATUSSEP,7);
			NthPathSegmentLong(statusValue,start,STATUSSEP,8);
			NthPathSegmentLong(statusValue,end,STATUSSEP,9);

			CBrowserView *view = (CBrowserView *)GetFirstView(doc);
			if (!view)
				continue;
			view->Navigate2(docPath);

			if (!appBoolTabbedWindows) {
				NthPathSegmentLong(statusValue,rect.top,STATUSSEP,2);
				NthPathSegmentLong(statusValue,rect.bottom,STATUSSEP,3);
				NthPathSegmentLong(statusValue,rect.left,STATUSSEP,4);
				NthPathSegmentLong(statusValue,rect.right,STATUSSEP,5);
				view->GetParentFrame()->MoveWindow(rect,true);
			}

			view->GetParentFrame()->ShowWindow(SW_SHOW);
			views.AddTail((CVisualTextView *)view);
		}
		else if (key == _T("activetab")) {
			if (segOneInt >= WORKSPACE_TAB_MAX)
				segOneInt = WORKSPACE_TAB_MAX - 1;
			wnd->SelectTab(segOneInt);
		}
		else if (key == _T("grammarsel")) {
			m_pMainFrame->m_wndGrammarView.SetSelection(segOneStr);
		}
		else if (key == _T("analyzersel")) {
			m_pMainFrame->m_wndAnalyzerView.SetSelection(segOneStr);
		}
		else if (key == _T("textsel")) {
			m_pMainFrame->m_wndTextView.SetSelection(segOneStr);
		}
		else if (key == _T("textstate")) {
			m_pMainFrame->m_wndTextView.SetState(segOneStr);
		}
		else if (key == _T("grammarstate")) {
			m_pMainFrame->m_wndGrammarView.SetState(segOneStr);
		}
		else if (key == _T("analyzerstate")) {
			m_pMainFrame->m_wndAnalyzerView.SetState(segOneStr);
		}

		else if (key == _T("doctabsel")) {
			tabSelect = segOneInt;
		}
		else if (key == _T("textpagemodestate")) {
			workStr = StripPathReverse(statusValue,0,true,STATUSSEP);
			m_pMainFrame->m_wndTextView.RestorePageModeState(workStr,firstTextModeState);
			firstTextModeState = false;
		}
		else if (key == _T("attributeeditor") && boolVal) {
			wnd->SendMessage(WM_COMMAND,ID_ATTRIBUTEEDITOR);
		}
	}

	if (appBoolHighlightText)
		wnd->HighlightText();

	if (!appBoolTabbedWindows) {
		pos = views.GetHeadPosition();
		while (pos && (view = views.GetNext(pos)))
			view->GetParentFrame()->BringWindowToTop();
	}

	if (appState == VT_APP_LOADING && tabSelect >= 0 && views.GetCount() > 0) {
		m_pMainFrame->m_MTIClientWnd.Refresh();  // Must be here to update docs added to the tabs manager
		CXTPTabManagerItem* pItem = m_pMainFrame->m_MTIClientWnd.GetItem(tabSelect);
		int c = m_pMainFrame->m_MTIClientWnd.GetItemCount();
		if (pItem != NULL) {
			pItem->Select();
		}
	}
	
	//if (appBoolTabbedWindows) {
	//	m_pMainFrame->m_MTIClientWnd.LoadState();
	//}
}

void CVisualTextApp::RestoreState(CStringList &stateList, bool createIfDoesNotExist)
{
	RestoreGlobals(stateList);
	RestoreViews(stateList, createIfDoesNotExist);

}
// SRP 01-01-29
bool CVisualTextApp::DoHourGlass(void)
{
	if (m_nWaitCursorCount > 0) {
		RestoreWaitCursor();
		return true;
	}
	return false;
}

VT_APP_STATE CVisualTextApp::GetAppState() const {
	return appState;
}

void CVisualTextApp::DisplayAnalyzerLoadStats(
	bool first	// 05/26/01 AM.
	)
{
	// ADD make_ana.txt LINES TO DEBUG INITIAL OUTPUT
	CString outputPathStr = LogsPath(_T("make_ana.log"));	// 05/19/01 AM.
	CStringList makeAnaLines;
	ReadFileToStringList(makeAnaLines,outputPathStr);
	if (makeAnaLines.GetCount() > 15000) {	// 05/04/01 AM.
//	if (makeAnaLines.GetCount() > 1000) {		// Annoying	// 05/04/01 AM.
		CString msgStr;
		msgStr.Format(_T("There are %d lines in the \"make_ana.log\" file.\nDo you want to display them all in the log window?"),
			makeAnaLines.GetCount());
		if (AfxMessageBox(msgStr,MB_YESNO) != IDYES) {
			return;
		}
	}

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	POSITION pos = makeAnaLines.GetHeadPosition();
	CString lineStr;

	while (pos && (lineStr = makeAnaLines.GetNext(pos))) {
		lineStr.Replace(_T("\t"),_T(" "));
		lineStr.Replace(_T("\r"),_T(" "));
		if (first)	// 05/26/01 AM.
			wnd->AddDebugLine(_T("Analyzer Load:"),lineStr,true);	// 05/26/01 AM.
		else	// 05/26/01 AM.
			wnd->AddDebugLine(_T("Analyzer Update:"),lineStr,true);	// 05/26/01 AM.
	}
}

void CVisualTextApp::MaskText(CString fileNameStr) 
{
	StartWaitCursor();
	CString fullPathStr;
	//if we have a full path with c: or the like don't use inputpath
	//PD 02/27/2001
	if (fileNameStr.GetAt(1) != ':')
		fullPathStr = InputPath(fileNameStr);
	else fullPathStr = fileNameStr;
	CString textStr = NLPReadFile(fullPathStr);
	CString newTextStr = _T("");
	int len = textStr.GetLength();
	TCHAR *pBuffer = NULL;

	try {
		pBuffer = new TCHAR[len + 1];
		StringToChar(pBuffer,textStr,len+1);

		TCHAR *pStart, *pPtr;
		pStart = pPtr = pBuffer;

		//TCHAR ch;
		_TUCHAR ch ; //so that isupper will work on all ansi chars

		while (*pPtr != 0) {
			ch = *pPtr;
			if (alphabetic(ch)) {
				if (is_upper(ch))
					ch = 'A';
				else
					ch ='a';
			}
			if (ch != '\r')
				newTextStr += (_TCHAR)ch;
			*pPtr++;
		}
		//Output the mask to a different file
		//PD 02/27/2001
		CString newPathStr = fullPathStr + _T(".msk");
		m_pMainFrame->m_wndTextView.AddNewText(newPathStr,newTextStr);

		/*
		NLPWriteFile(newPathStr,newTextStr);
		CVisualTextDoc *doc =
			(CVisualTextDoc *)theApp.OpenDocumentFile(newPathStr,true);
		if (doc) {
			CVisualTextView *view = (CVisualTextView *)GetFirstView(doc);
			if (!view)
				return;
			view->GetParentFrame()->BringWindowToTop();
		}
		*/

		//delete pBuffer;

	} catch(...){}

	StopWaitCursor();
}

void CVisualTextApp::Dictionize(CString filePathInStr) 
{
	if (!cg)
		return;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	CString msgStr;
	msgStr.Format(_T("Dictionize expects its input to be an alphabetical list of words or phrases.\nDo you want to break the file \"%s\" into dictionary pieces?"),
		filePathInStr);
	if (AfxMessageBox(msgStr,MB_YESNO) != IDYES)
		return;

	if (filePathInStr == _T(""))
		return;
	FILE *fp;
	errno_t err;
	if (!(err = _tfopen_s(&fp,(const _TCHAR *)filePathInStr, _T("r"))) != 0 ) {
#ifdef _DEBUG
		afxDump << _T("File could not be opened ") << filePathInStr << _T("\n");
#endif
		return;
	}

	_TCHAR line[1001];
	
	CString lineStr;
	CString dirPathStr;
	CString filePathStr;
	CString fileNameStr;
	CString nextFileNameStr;
	CString contentStr;
	CString leftStr;

	// FIND DICTIONARY
	CONCEPT *sys = cg->findConcept(cg->findRoot(),_T("sys"));
	CONCEPT *dict = cg->findConcept(sys,_T("dict"));
	CONCEPT *a1 = cg->findConcept(dict,_T("a"));
	CONCEPT *folder = cg->findConcept(a1,_T("a"));
	CONCEPT *nextFolder = NULL;
	CONCEPT *file = NULL;
	CONCEPT *nextFile = NULL;

	bool firstFile = true;
	bool getNextFolder = false;
	bool keepOn = true;
	bool backup = false;
	bool directoryEmpty = true;

	while (keepOn) {

		if (!backup && !_fgetts(line,1000,fp))
			break;
		backup = false;

		lineStr = line;
		if (EndsWith(lineStr,_T("\n")))
			ClipRight(lineStr,1);
		if (lineStr == _T("") || lineStr.GetAt(0) < 'a' || 'z' < lineStr.GetAt(0))
			continue;

		if (!file) {
			if (!firstFile)
				folder = cg->Next(folder);
			if (directoryEmpty && IsDirectory(dirPathStr))
				RecursiveRemove(dirPathStr);
			dirPathStr = InputPath() + _T("\\") + ConceptName(folder);
			MakeDirectory(dirPathStr);
			directoryEmpty = true;
			wnd->AddDebugLine(_T("Dictionizing"),dirPathStr,true);
			nextFolder = cg->Next(folder);
			getNextFolder = true;
			file = cg->Down(folder);
			firstFile = true;
		}

		if (getNextFolder) {
			if (!firstFile)
				file = cg->Next(file);
			if (!file) {
				backup = true;
				continue;
			}
			fileNameStr = ConceptName(file);
			if (!(nextFile = cg->Next(file)) && nextFolder) {
				nextFile = cg->Down(nextFolder);
			}
			nextFileNameStr = ConceptName(nextFile);
			//contentStr = "";
			firstFile = false;
		}

		leftStr = lineStr.Left(nextFileNameStr.GetLength());

		//We expect the input to be alphabetically arranged. If
		//it is not, we will have to quit.
		if (!getNextFolder && 
			leftStr.GetLength() == fileNameStr.GetLength() && 
			leftStr.CompareNoCase(fileNameStr) < 0
		) {
			AfxMessageBox(_T("The input to the dictionize function must be in alphabetical order, one entry to a line. Aborting dictionize call."));
			break;
		}

		// IF IT IS LESS THAN THE NEXT FOLDER ADD TO CURRENT FILE STRING
		if (leftStr.CompareNoCase(nextFileNameStr) < 0) {
			if (contentStr != _T(""))
				contentStr += _T("\n");
			contentStr += lineStr;
			getNextFolder = false;
		}

		// IF EQUAL WRITE OUT CURRENT FILE
		else if (leftStr.CompareNoCase(nextFileNameStr) >= 0) {
			if (contentStr != _T("")) {
				filePathStr = dirPathStr + _T("\\") + fileNameStr + _T(".txt");
				NLPWriteFile(filePathStr,contentStr);
				contentStr = _T("");
				directoryEmpty = false;
			}
			backup = true;
			getNextFolder = true;
		}

		if (!folder)
			break;

		PeekHere(5);
	}

	if (leftStr.CompareNoCase(nextFileNameStr) < 0 && contentStr != _T("")) {
		filePathStr = dirPathStr + _T("\\") + fileNameStr + _T(".txt");
		NLPWriteFile(filePathStr,contentStr);
		contentStr = _T("");
	}

	fclose(fp);
}

CString GetUniqueDirectoryName(LPCTSTR lpFolder, LPCTSTR lpNameID)
{
	CFileFind finder;
	CString strParentPattern, strDirFind, strDirName;
	strDirFind.Format(_T("%s%s"), lpFolder, lpNameID);
	strParentPattern.Format(_T("%s\\*.*"), StripPath(CString(lpFolder),0,true));
	BOOL bFound = finder.FindFile(strParentPattern);
	int num = -1;
	while (bFound) {
		bFound = finder.FindNextFile();
		if (!finder.IsDirectory())
			continue;
		if (finder.IsDots())
			continue;
		CString strFolderName = finder.GetFilePath();
		// if exact match, set num to 0
		if (!strFolderName.CompareNoCase(strDirFind)) {
			if (0 > num)
				num = 0;
			continue;
		}
		// if first part matches, check to see that only digits comprise the remaining letters
		if (!_memicmp(strFolderName, strDirFind, strDirFind.GetLength()) ) {
			int pos = strDirFind.GetLength();
			int len = strFolderName.GetLength();
			BOOL bFound = true;
			for (int n = pos; n < len; n++) {
				if (_istdigit(strFolderName.GetAt(n)) )
					continue;
				bFound = false;
				break;
			}
			if (bFound) {
				int curNum = _ttoi(strFolderName.Mid(pos));
				if (curNum > num)
					num = curNum;
				continue;
			}
		}
	}
	finder.Close();
	if (-1 == num) return strDirFind;
	strDirName.Format(_T("%s%02.2u"),strDirFind, num + 1);
	return strDirName;
}

bool CVisualTextApp::RunDictionaryLookup(CString filePathStr, LPCTSTR lpURL, LPCTSTR lpNameID)
{
	VT_APP_STATE lastState = appState;
	CString fullFilePathStr = InputPath(filePathStr);
	CStringList words;
	ReadFileToStringList(words,fullFilePathStr);
	bool userAbort = false;

	if (filePathStr != _T("") && words.GetCount()) {
		//CStringList words;
		//ReadFileToStringList(words,fullFilePathStr);

		POSITION pos = words.GetHeadPosition();
		int numWords = words.GetCount();
		CString wordStr;
		CString urlStr;
		CString htmlTextStr;
		CString msgStr;
		CString writeFilePathStr;
		//CString DictionaryLookupNetURL = "http://www.cogsci.princeton.edu/cgi-bin/webwn/?stage=1&word=";
		//CString DictionaryLookupNetURL = "http://www.m-w.com/cgi-bin/dictionary?book=Dictionary&va=";
		//CString DictionaryLookupNetURL = "http://www.dictionary.com/cgi-bin/dict.pl?term=";
		CString DictionaryLookupNetURL;
		if (lpURL) {
			DictionaryLookupNetURL = lpURL;
		}
		else {
			CStringArray strNames;
			CStringArray strURLs;
			CPrefLookupPage::GetDictionaryLookupList(strNames, strURLs);
			DictionaryLookupNetURL = strURLs[0];
			strNames.RemoveAll();
			strURLs.RemoveAll();
		}
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

		int retrieved = 1;

		CString dirPathStr;
		CString strNameID;
		dirPathStr = RemoveExtension(fullFilePathStr);
		if (lpNameID) {
			strNameID = _T("_");
			strNameID += lpNameID;
		}
		else {
			strNameID = _T("_wn");
		}
		dirPathStr = GetUniqueDirectoryName(dirPathStr, strNameID);
		MakeDirectory(dirPathStr);
		appState = VT_APP_WORD_NETTING;
		wnd->SetAnalyzingToolbarButtons(false);

		int error=0;
		int currentNum = 1;
		while (pos && (wordStr = words.GetNext(pos))) {
			if (appState == VT_APP_NORMAL
				|| appState == VT_APP_LOADED) {//PD 02/19/2001
				userAbort = true;
				break;
			}
			if (wordStr == _T("") || wordStr.Find(' ')!=-1 ) {
				if (!error)
					wnd->AddDebugLine(_T("Dictionary Lookup: Adding Text ERROR"),_T("Blank line or multiple words"),true);
					//AfxMessageBox("Blank line or multiple words to a line. The WordNet tool expects a list of words as input, one word to a line.") ;
				error=1;
				continue;
			}

//			msgStr.Format("(%d of %d) %s",retrieved,numWords,wordStr);
			msgStr.Format(_T("(%d of %d) %s"),currentNum++,numWords,wordStr);
			urlStr.Format(_T("%s%s"),DictionaryLookupNetURL,wordStr);
			wnd->AddDebugLine(_T("Dictionary Lookup: Adding Text"),msgStr,true);
			PeekHere(10);

			if (FetchURLString(urlStr,htmlTextStr)) {
				PeekHere(10);
				//AddTextFile(wordStr,htmlTextStr,"htm");
				writeFilePathStr = dirPathStr + _T("\\") + wordStr + _T(".htm");
				NLPWriteFile(writeFilePathStr,htmlTextStr);
				retrieved++;
			}
			else
				wnd->AddDebugLine(_T("Dictionary Lookup: Failed for"),wordStr,true);
			PeekHere(10);
		}
		PeekHere(10);
		if (!userAbort) {
			wordStr.Format(_T("%d of %d retrieved"),--retrieved,numWords);
			wnd->AddDebugLine(_T("Done with Dictionary Lookup"),wordStr,true);
		}

		wnd->SetAnalyzingToolbarButtons(false);
		appState = lastState;
	}

	return !userAbort;
}

void CVisualTextApp::JumbleText(CString fileNameStr) 
{
	StartWaitCursor();

	CString fullPathStr;
	//if we have a full path with c: or the like don't use inputpath
	//PD 02/27/2001
	if (fileNameStr.GetAt(1) != ':')
		fullPathStr = InputPath(fileNameStr);
	else fullPathStr = fileNameStr;
	CString textStr = NLPReadFile(fullPathStr);
	CString newTextStr = _T("");
	int len = textStr.GetLength();
	TCHAR *pBuffer = NULL;

	try {
		pBuffer = new TCHAR[len + 1];
		StringToChar(pBuffer,textStr,len+1);

		TCHAR *pStart, *pPtr;
		pStart = pPtr = pBuffer;

		//TCHAR ch;
		_TUCHAR ch ; //so that isupper will work on all ansi chars
		srand((unsigned)time(NULL));
		int randInt;
		CString vowels = _T("aeiou");

		while (*pPtr != 0) {
			ch = *pPtr;
			randInt = rand();
			if (vowels.Find(ch) >= 0)
				ch = vowels.GetAt(randInt % 5);
			else if ('a' <= ch && ch <= 'z')
				ch = randInt % 26 + 'a';
			else if ('A' <= ch && ch <= 'Z')
				ch = randInt % 26 + 'A';
			else if (_istdigit(ch))
				ch = randInt % 10 + '0';
			else
				ch = *pPtr;
			if (ch != '\r')
				newTextStr += (_TCHAR)ch;
			*pPtr++;
		}
		//Output the mask to a different file
		//PD 02/27/2001
		CString newPathStr = fullPathStr + _T(".jmb");
		m_pMainFrame->m_wndTextView.AddNewText(newPathStr,newTextStr);

		/*
		NLPWriteFile(newPathStr,newTextStr);
		CVisualTextDoc *doc =
			(CVisualTextDoc *)theApp.OpenDocumentFile(newPathStr,true);
		if (doc) {
			CVisualTextView *view = (CVisualTextView *)GetFirstView(doc);
			if (!view)
				return;
			view->GetParentFrame()->BringWindowToTop();
		}
		*/

		//delete pBuffer;

	} catch(...){}

	StopWaitCursor();
}

void CVisualTextApp::SetWindowSize(CView *view, int width, int length, int fileLength,
								   bool dontMove)
{
	if (!view)
		return;

	//don't resize if window is maximized PD 03/05/2001
	if (view->GetParentFrame()->IsZoomed())
		return;

	int minw = 150;
	int minl = 100;

		// ADD MORE FOR SCROLLBARS
	if (length >= appIntMaxWindowHeight)
		width += 30;
	if (width == appIntMaxWindowWidth)
		length += 30;

	width += 16;  // ADD 2 SKINNY FRAME PIECES
	length += 16 + 30;  // ADD 2 SKINNY FRAME PIECES AND ONE BIG TITLEBAR PIECE

	if (width > appIntMaxWindowWidth)
		width = appIntMaxWindowWidth;
	if (length > appIntMaxWindowHeight)
		length = appIntMaxWindowHeight;

	if ( appBoolAutosizeWindows &&
		(width < appIntMaxWindowWidth || length < appIntMaxWindowHeight)) {

		if (width < minw)
			width = minw;
		if (length < minl)
			length = minl;
		if (fileLength == 0) {
			width = appIntMaxWindowWidth;
			length = appIntMaxWindowHeight;
		}
		unsigned int flags = SWP_SHOWWINDOW;
		if (dontMove)
			flags = SWP_NOMOVE;
		view->GetParentFrame()->SetWindowPos(NULL,appIntLastWindowX,appIntLastWindowY,
			width,length,flags);
	}
	else {
		unsigned int flags = 0;
		if (dontMove)
			flags = SWP_NOMOVE;
		view->GetParentFrame()->SetWindowPos(NULL,appIntLastWindowX,appIntLastWindowY,
		   appIntMaxWindowWidth,appIntMaxWindowHeight,flags);
	}
}

void CVisualTextApp::SaveAnalyzer(bool askConfirm)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	// COMPLETE DECOUPLING OF SAVING ANALYZER AND KB.	// 10/12/03 AM.
//	BOOL bKBSaved = AutoSaveKB(askConfirm);	// 10/12/03 AM.

	// Want to re-establish separation of analyzer and kb.	// 06/24/03 AM.

	// Ask about KB separately from analyzer definition.	// 06/24/03 AM.
	// COMPLETE DECOUPLING OF SAVING ANALYZER AND KB.	// 10/12/03 AM.
//	if (!bKBSaved && kbDirty)	// 10/12/03 AM.
//		SaveKB(askConfirm);	// 10/12/03 AM.

//	if (kbDirty || wnd->AnalyzerDirty()) {	// 06/24/03 AM.
	if (m_pMainFrame->m_wndAnalyzerView.IsAnalyzerFileDirty()) {	// 06/24/03 AM.
		CString msgStr = _T("The analyzer has been modified. Do you want to save it?");
		// SRP 010505 Reinstated "askConfirm" for automatic saving of New Analyzer after Opening
		if (askConfirm && AfxMessageBox(msgStr,MB_YESNO) == IDNO)
			return;

	// Gettting rid of it altogether.	// 06/26/03 AM.
//		DumpAppGUIConcept();	// This should be part of saving analyzer. // 06/24/03 AM.

		wnd->SaveAnalyzerFile();
//		if (!bKBSaved)	// 06/24/03 AM.
//			SaveKB();	// 06/24/03 AM.
		SaveAllModified();
	}
}

// COMPLETE DECOUPLING OF SAVING ANALYZER AND KB.	// 10/12/03 AM.
void CVisualTextApp::SaveKBx(bool askConfirm)
{
	BOOL bKBSaved = AutoSaveKB(askConfirm);	// 10/12/03 AM.

	if (!bKBSaved && kbDirty)	// 10/12/03 AM.
		SaveKB(askConfirm);	// 10/12/03 AM.
}

//Added so we don't have to call OnUpdateFrameTitle explicitly
//so as to avoid Purify crash PD 02/14/2001
void CMainFrame::SetFrameTitle(CString addString) 
{
	CString csAppName;

	if (appState != VT_APP_EXITING) {
		if (appBoolAnalyzerLoaded) {
			if (addString.IsEmpty())
				csAppName.Format(_T("VisualText - %s"), appNameStr);
			else
				csAppName.Format(_T("VisualText - %s : %s"), appNameStr, addString);
		}
		else
			csAppName = _T("VisualText - (no analyzer currently loaded)");

		SetWindowText(csAppName);
	}
}

void CMainFrame::UpdateDictionaryLookupURLSPopup(CMenu* pPopupMenu)
{
	CMenu *context = pPopupMenu;
	if (!pPopupMenu) {
		pPopupMenu = GetMenu();
		int pos;
		if (!FindMenuPosFromString(pPopupMenu, _T("&Tools"), pos))
			return;
		context = pPopupMenu->GetSubMenu(pos);
	}
	int nPos;
	if (FindMenuPosFromString(context, _T("&Dictionary"), nPos)) {
		CMenu* pMenuDict = context->GetSubMenu(nPos);
		CString strLookup = _T("&Lookup");
		if (FindMenuPosFromString(pMenuDict, strLookup, nPos)) {
			CMenu menuLookup;
			CStringArray strNames;
			CPrefLookupPage::GetDictionaryLookupMenu(menuLookup, ID_TOOLS_DICTIONARY_URL_LOOKUP_BASE, strNames, m_strURLs);
			pMenuDict->DeleteMenu(nPos,MF_BYPOSITION);
			pMenuDict->InsertMenu(nPos,MF_ENABLED | MF_BYPOSITION | MF_POPUP | MF_STRING,
				(UINT)menuLookup.GetSafeHmenu(),strLookup);
			menuLookup.Detach();
		}
	}
}

void CVisualTextApp::LoadRecentAnalyzersString() 
{
	CMainFrame *mainFrame = (CMainFrame *)AfxGetMainWnd();
	appRecentAnalyzers = GetProfileString(_T("PREFERENCES"),_T("RECENTANALYZERS"),_T(""));
	if (appRecentAnalyzers == _T("")) return;

	CStringList recentAnalyzers;
	SeparateByChar(appRecentAnalyzers,STATUSSEP,recentAnalyzers);
	UniqueStringList(recentAnalyzers);

	POSITION pos = recentAnalyzers.GetHeadPosition();
	CString analyzer,newAppRecentAnalyzers;
	while (pos && (analyzer = recentAnalyzers.GetNext(pos))) {
		if (FileExists(analyzer)) {
			if (newAppRecentAnalyzers != "")
				newAppRecentAnalyzers += STATUSSEP;
			newAppRecentAnalyzers += analyzer;
		}
		else {
			mainFrame->AddDebugLine("Recent Analyzer:","couldn't find",true);
		}
	}

	appRecentAnalyzers = newAppRecentAnalyzers;
}

// Simple stupid.	// 06/26/03 AM.
void CVisualTextApp::LoadAppGUIConcept(void)
{
	bool save = cg->getDirty();	// 07/11/03 AM.
	CONCEPT *root = cg->findRoot();
	CONCEPT *sys = cg->findConcept(root,_T("sys"));
	appGUIConcept = cg->findConcept(sys, _T("gui"));
	if (appGUIConcept)
		cg->rmConcept(appGUIConcept);
	appGUIConcept = cg->makeConcept(sys, _T("gui"));
	cg->setDirty(save);	// Kb status unaffected.	// 07/11/03 AM.
}

void CVisualTextApp::CreateSplash()
{
	if (!m_splashDlg) {
		m_splashDlg.Create(IDD_SPLASH_DLG, AfxGetMainWnd());
	}
	m_splashDlg.ShowWindow(SW_SHOW);
	m_splashDlg.UpdateWindow();
	m_splashDlg.Init();
}

void CVisualTextApp::DestroySplash()
{
	m_splashDlg.DestroyWindow();
	delete m_splashDlg;
}

bool CVisualTextApp::ReadStatusFile(CString filePathStr)
{
	if (filePathStr == _T("")) return false;
	if (!FileExists(filePathStr)) return false;
	CString cStr = NLPReadFile(filePathStr);
	CString lineStr;
	int len = cStr.GetLength();
	int lineIndex = 0;
	int lineIndex2 = 0;
	int lineNum = 0;

	appStatus.RemoveAll();

	while (lineIndex2 >= 0) {
		lineNum++;
		lineIndex2 = cStr.Find(_T("\n"), lineIndex);

		if (lineIndex2 == -1)
			lineStr = cStr.Mid(lineIndex);
		else
			lineStr = cStr.Mid(lineIndex, lineIndex2 - lineIndex);

		appStatus.AddTail(lineStr);

		lineIndex = lineIndex2 + 1;
		if (lineIndex2 > len)
			break;
	}

	return true;
}

void CVisualTextApp::InitAppGlobals(bool firstTime)
{
	appAnaFilePathStr = _T("");
	appNameStr = _T("");
	appDirectoryStr = _T("");
	appSelection = _T("");
	appURL = _T("");
	appAskText = _T("");
	appHelpTopic = _T("");
	appBoolAnalyzerLoaded = false;

	if (firstTime) {
		appRecentAnalyzers = _T("");
	}

	// SRP 010424
	CString strFileVersionInResourceFile = _T("x.x.x.x");
	UTLGetFileVersion(strFileVersionInResourceFile);
	appStrVersion.Format(_T("Version %s"), strFileVersionInResourceFile);
}

void CVisualTextApp::CloseAllViews(bool temporary)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	if (appBoolTabbedWindows) wnd->m_MTIClientWnd.SaveState();
	DoAllViews(NULL,_T("VisualText"),_T(""),DOC_ACTION_SAVE_CONFIRM);
	DoAllViews(NULL,_T("VisualText"),_T(""),DOC_ACTION_DESTROY_ALL);
	DoAllViews(NULL,_T("Browser"),_T(""),DOC_ACTION_DESTROY_ALL);
	DoAllViews(NULL,_T("Knowledge"),_T(""),DOC_ACTION_DESTROY_ALL);
	DoAllViews(NULL,_T("ParseTree"),_T(""),DOC_ACTION_DESTROY_ALL);
	DoAllViews(NULL,_T("SyntaxEditor"),_T(""),DOC_ACTION_DESTROY_ALL);
	if (!temporary) wnd->SendMessage(WMU_DESTROY_ATTRIBUTEEDITOR);
	wnd->UpdateWindow();
}

bool CVisualTextApp::MovedPathStr(CString &pathStr)
{
	bool isFileURL = pathStr.Find(_T("file:///")) == 0;

	if (isFileURL) {
		ClipLeft(pathStr,8);
		pathStr.Replace(_T("/"),_T("\\"));
	}

	CString newPathStr = pathStr;
	newPathStr = AppPath(pathStr);

	// CHECK FOR OLDER VERSION PATHS
	if (!FileExists(newPathStr)) {
		CString relPathStr = RelativizePath(pathStr);
		newPathStr = AppPath(relPathStr);

		if (!FileExists(newPathStr)) {
			int pos;
			if ((pos = pathStr.Find(_T("\\input\\"))) > 0 ||
				(pos = pathStr.Find(_T("\\spec\\"))) > 0) {
				newPathStr = AppPath(pathStr.Mid(pos+1,pathStr.GetLength()-pos));
			}
			if (!FileExists(newPathStr))
				return false;
		}
	}

	if (isFileURL) {
		newPathStr.Replace(_T("\\"),_T("/"));
		newPathStr = _T("file:///") + newPathStr;
	}
	pathStr = newPathStr;
	return true;
}

CString CVisualTextApp::TemplateFromExtension(LPCTSTR lpszFileName)
{
	CString templateName = _T("VisualText");
	if (EndsWith(lpszFileName, ".pat")) templateName = _T("SyntaxEditor");
	return templateName;
}

CDocument* CVisualTextApp::OpenDocumentFile(LPCTSTR lpszFileName, bool showFlag) 
{
	_TCHAR path[1000];
	_tcscpy_s(path,lpszFileName);

	CDocument *doc = (CDocument *)CVisualTextApp::OpenNewDocument(TemplateFromExtension(lpszFileName),path,showFlag);
	return doc;
}

CDocument* CVisualTextApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	_TCHAR path[1000];
	_tcscpy_s(path,lpszFileName);

	CDocument *doc = (CDocument *)CVisualTextApp::OpenNewDocument(TemplateFromExtension(lpszFileName),path);
	return doc;
}

void CVisualTextApp::LoadFonts()
{
	appFontFixed.Detach();
//	appFontFixedSmall.Detach();	// 09/09/08 AM.
	appFontCopyright.Detach();
	appFontFixed.CreateFont(-appIntFixedFontSize, 0,0,0,0,0,0,0,0,0,0,0,0, appStrFontFixed);
	appFontFixedSmall.CreateFont(-appIntFixedFontSizeSmall, 0,0,0,0,0,0,0,0,0,0,0,0, appStrFontFixedSmall); //09/09/08 AM.
#ifdef UNICODE
	appFontCopyright.CreateFont(-10, 0,0,0,0,0,0,0,0,0,0,0,0, _T("Arial Unicode MS"));
#else
	appFontCopyright.CreateFont(-10, 0,0,0,0,0,0,0,0,0,0,0,0, _T("Arial"));
#endif
}

CDocument *CVisualTextApp::GetDocument(CString docType)
{
	CString strDocName;
	CmyMultiDocTemplate *pSelectedTemplate;
	POSITION pos = GetFirstDocTemplatePosition();
	CDocument *doc = NULL;
	POSITION docPos;

	while (pos != NULL) {
		pSelectedTemplate = (CmyMultiDocTemplate*) GetNextDocTemplate(pos);
		ASSERT(pSelectedTemplate != NULL);
		ASSERT(pSelectedTemplate->IsKindOf(RUNTIME_CLASS(CmyMultiDocTemplate)));
		pSelectedTemplate->GetDocString(strDocName,CDocTemplate::docName);
		if (strDocName == docType) {
			docPos = pSelectedTemplate->GetFirstDocPosition();
			// ASSUME ONLY ONE NOW!
			if (docPos)
				doc = (CBrowserDoc *)pSelectedTemplate->CmyMultiDocTemplate::GetNextDoc(docPos);
			break;
		}
	}
	return doc;
}
	
void CVisualTextApp::LoadPreferences()
{
	appBoolKBSafeEditMode = BoolValueVT(GetProfileString(_T("PREFERENCES"),_T("KBSAFEEDITMODE"),_T("false")));	// 05/21/08 AM.
	bULFflag = BoolValueVT(GetProfileString(_T("PREFERENCES"),_T("USERLOGFILE"),_T("false")));
	appBoolTipOfDay = BoolValueVT(theApp.GetProfileString(_T("PREFERENCES"),_T("TIPOFDAY"),_T("true")));	// 05/21/08 AM.
	appBoolTabbedWindows = BoolValueVT(theApp.GetProfileString(_T("PREFERENCES"),_T("TABBEDWINDOWS"),_T("false")));
	appBoolToolbarLabels = BoolValueVT(theApp.GetProfileString(_T("PREFERENCES"),_T("TOOLBARLABELS"),_T("false")));
	appBoolAutosizeWindows = BoolValueVT(theApp.GetProfileString(_T("PREFERENCES"),_T("AUTOSIZEWINDOWS"),_T("false")));
	appBoolSizeDynamically = BoolValueVT(theApp.GetProfileString(_T("PREFERENCES"),_T("SIZEDYNAMICALLY"),_T("false")));
	appIntMaxWindowWidth = StringToInteger(theApp.GetProfileString(_T("PREFERENCES"),_T("MAXWINDOWWIDTH"),_T("500")));
	appIntMaxWindowHeight = StringToInteger(theApp.GetProfileString(_T("PREFERENCES"),_T("MAXWINDOWHEIGHT"),_T("300")));
	appIntRuleTabSize = StringToInteger(theApp.GetProfileString(_T("PREFERENCES"),_T("RULETABSIZE"),_T("4")));
	appIntFixedFontSize = StringToInteger(theApp.GetProfileString(_T("PREFERENCES"),_T("FIXEDFONTSIZE"),_T("12")));

	appStrSMTPServer = GetProfileString(_T("PREFERENCES"),_T("SMTPSERVER"),_T(""));
	appStrFromEmail = GetProfileString(_T("PREFERENCES"),_T("FROMEMAIL"),_T(""));

	appStrArchiveAuthor = theApp.GetProfileString(_T("PREFERENCES"),_T("ARCHIVEAUTHOR"),_T("Your Name Here"));
	appStrArchiveFTP = theApp.GetProfileString(_T("PREFERENCES"),_T("ARCHIVEFTP"),_T(""));
	appStrArchiveDirectoryPath = theApp.GetProfileString(_T("PREFERENCES"),_T("ARCHIVEDIRECTORY"),_T(""));
	appStrArchiveUsername = theApp.GetProfileString(_T("PREFERENCES"),_T("ARCHIVEUSERNAME"),_T(""));
	appStrArchivePassword = theApp.GetProfileString(_T("PREFERENCES"),_T("ARCHIVEPASSWORD"),_T(""));

	GetAppsDir();
	
	CString strPrintMargins = theApp.GetProfileString(_T("PREFERENCES"),_T("PRINTMARGINS"),_T("1800 1440 1800 1440"));
	UTLReadRect (m_rectPrintMargins, strPrintMargins);
	SetPrinterMinMargins();

	int t = theApp.GetProfileInt(_T("PREFERENCES"),_T("GUITHEME"),xtpThemeVisualStudio2008);
	CXTPPaintManager::SetTheme((XTPPaintTheme)t);
}

CString CVisualTextApp::GetAppsDir() {
	CString appPathDefault = "";
	size_t len = 0;
	#ifdef _UNICODE
		wchar_t vtpath[500]; 
		_wgetenv_s( &len, vtpath, 500, L"APPS" ); 
		appPathDefault = vtpath;

		if (vtpath == NULL || !IsDirectory(appPathDefault)) {
			_wgetenv_s( &len, vtpath, 500, L"VISUALTEXT" ); 
			appPathDefault = vtpath;
			appPathDefault = appPathDefault + _T("\\apps");
		}
	#else
		_TCHAR *vtpath = NULL;
		_dupenv_s(&vtpath, &len, _T("APPS"));
		appPathDefault = vtpath;

		if (vtpath == NULL || !IsDirectory(appPathDefault)) {
			_dupenv_s(&vtpath, &len, _T("VISUALTEXT"));
			appPathDefault = vtpath;
			appPathDefault = appPathDefault + _T("\\apps");
		}
	#endif

	appStrAppsDirectory = theApp.GetProfileString(_T("PREFERENCES"),_T("APPSDIRECTORY"),appPathDefault);

	return appStrAppsDirectory;
}

void CVisualTextApp::SetPrinterMinMargins(void)
{
	m_MinMarginX = 0;
	m_MinMarginY = 0;
	CPrintDialog dlg(true);
	dlg.m_pd.hDevNames = NULL;
	dlg.m_pd.hDevMode = NULL;
	GetPrinterDeviceDefaults(&dlg.m_pd);
	HDC hDC = dlg.CreatePrinterDC();
	if (hDC) {
		int xOffset = GetDeviceCaps(hDC, PHYSICALOFFSETX);
		int yOffset = GetDeviceCaps(hDC, PHYSICALOFFSETY);
		int dpiHorz = GetDeviceCaps(hDC, LOGPIXELSX);
		int dpiVert = GetDeviceCaps(hDC, LOGPIXELSY);
		m_MinMarginX = MulDiv(xOffset, 1440, dpiHorz);
		m_MinMarginY = MulDiv(yOffset, 1440, dpiVert);
		DeleteDC(hDC);
	}
}

// SRP 010517
// The following OnEditXXX() functions need to be placed in the CVisualTextApp message map in order for
// accelerator keys (Ctrl+V, Ctrl+X, etc.) to be able to work in the toolbar comboboxes.
// Additional modifications were also made to VisualTextView.cpp, as well as to OnUpdateEditXXX()
// Modifications may need to be made to HexEditCtrl.cpp/h if the Dialog is ever invoked as modeless.
void CVisualTextApp::OnEditPaste()
{
	if (CWnd::GetFocus())
		CWnd::GetFocus()->SendMessage(WM_PASTE);
}

void CVisualTextApp::OnEditCopy()
{
	if (CWnd::GetFocus())
		CWnd::GetFocus()->SendMessage(WM_COPY);
}

void CVisualTextApp::OnEditCut()
{
	if (CWnd::GetFocus())
		CWnd::GetFocus()->SendMessage(WM_CUT);
}

void CVisualTextApp::OnEditClear()
{
	if (CWnd::GetFocus())
		CWnd::GetFocus()->SendMessage(WM_CLEAR);
}

void CVisualTextApp::OnEditUndo()
{
	if (CWnd::GetFocus())
		CWnd::GetFocus()->SendMessage(WM_UNDO);
}

void CVisualTextApp::OnToolsEmail() 
{
	CEmailDlg dlg;
	dlg.DoModal();
}

void CVisualTextApp::OnChooseAnalyzer(UINT idIn) 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->ChooseAnalyzer(idIn);
}

void CVisualTextApp::OnAnalyzerGenerateunlocked() 
{
	CMainFrame *mainFrame = (CMainFrame *)AfxGetMainWnd();
	mainFrame->GenAnalyzer(GRAM_GEN_UNLOCKED,false);
}

void CVisualTextApp::OnAnalyzerGenerateall() 
{
	CMainFrame *mainFrame = (CMainFrame *)AfxGetMainWnd();
	mainFrame->GenAnalyzer(GRAM_GEN_ALL,false);
}

void CVisualTextApp::OnAnalyzerGeneratedirty() 
{
	CMainFrame *mainFrame = (CMainFrame *)AfxGetMainWnd();
	mainFrame->GenAnalyzer(GRAM_GEN_DIRTY,false);
}


//trying to isolate some file open, file print set up leaks
//PD 02/14/2001 -- we may end up reverting to the direct MFC call
void CVisualTextApp::OnFileOpen()
{
	ASSERT(m_pDocManager != NULL);
	m_pDocManager->OnFileOpen();
}

void CVisualTextApp::OnUpdateFileOpen(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded);	
}

// prompt for file name - used for open and save as
BOOL CVisualTextApp::DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags,
	BOOL bOpenFileDialog, CDocTemplate* pTemplate)
		// if pTemplate==NULL => all document templates
{
	ASSERT(m_pDocManager != NULL);
	return m_pDocManager->DoPromptFileName(fileName, nIDSTitle, lFlags,
		bOpenFileDialog, pTemplate);
}

void CVisualTextApp::OnFileOpenanalyzer() 
{
	// SRP 010417 Use APPS environment variable
	logmsg(_T("Enter OnFileOpenAnalyzer:"));	// 07/24/03 AM.
	CString fileDir = appStrAppsDirectory;
	fileDir += _T("\\*.ana");
	CFileDialog fileDlg(true,_T("ana"),fileDir,OFN_HIDEREADONLY,_T("Analyzer Files (*.ana)|*.ana||"));

	if (fileDlg.DoModal() == IDOK) {
		AfxGetMainWnd()->RedrawWindow(NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE); // SRP 01-01-30
		// SRP 010628
		if (UTLFindRunningVTInst(fileDlg.GetPathName()))
			return;
		CloseAnalyzer();
		appAnaFilePathStr = fileDlg.GetPathName();
		ReadAnalyzerStatusFile(appAnaFilePathStr);
		SetAppName();

		OpenAnalyzer();
	}
	logmsg(_T("Leave OnFileOpenAnalyzer:"));	// 07/24/03 AM.
}

void CVisualTextApp::OnFileCloseanalyzer() 
{
	CloseAnalyzer();	
}

void CVisualTextApp::OnFileSaveanalyzer() 
{
	SaveAnalyzer(true);
	SaveKBx(true);			// 10/12/03 AM.
}

void CVisualTextApp::OnUpdateFileCloseanalyzer(CCmdUI* pCmdUI) 
{
	switch (appState) {
		case VT_APP_ANALYZING:
		case VT_APP_DIR_ANALYZING:
			pCmdUI->Enable(false);	
			break;
		default:
			pCmdUI->Enable(appBoolAnalyzerLoaded);	
			break;
	}
}

void CVisualTextApp::OnUpdateFileSaveanalyzer(CCmdUI* pCmdUI) 
{
	switch (appState) {
		case VT_APP_ANALYZING:
		case VT_APP_DIR_ANALYZING:
			pCmdUI->Enable(false);	
			break;
		default:
//			pCmdUI->Enable(appBoolAnalyzerLoaded && kbDirty);	// 06/26/03 AM.
			pCmdUI->Enable(appBoolAnalyzerLoaded && m_pMainFrame->m_wndAnalyzerView.IsAnalyzerFileDirty());	// 06/26/03 AM.
			break;
	}
}

void CVisualTextApp::OnWindowCloseall() 
{
	DoAllViews(NULL,_T("VisualText"),_T(""),DOC_ACTION_SAVE_CONFIRM);
	DoAllViews(NULL,_T("VisualText"),_T(""),DOC_ACTION_DESTROY_ALL);
	DoAllViews(NULL,_T("Browser"),_T(""),DOC_ACTION_DESTROY_ALL);
	DoAllViews(NULL,_T("Knowledge"),_T(""),DOC_ACTION_DESTROY_ALL);
	DoAllViews(NULL,_T("ParseTree"),_T(""),DOC_ACTION_DESTROY_ALL);
	DoAllViews(NULL,_T("SyntaxEditor"),_T(""),DOC_ACTION_DESTROY_ALL);
}

inline int UTLroundleast(int n)
{
	int mod = n%10;
	n -= mod;
	if (mod >= 5)
		n += 10;
	else if (mod <= -5)
		n -= 10;
	return n;
}

void UTLRoundRect(LPRECT r1)
{
	r1->left = UTLroundleast(r1->left);
	r1->right = UTLroundleast(r1->right);
	r1->top = UTLroundleast(r1->top);
	r1->bottom = UTLroundleast(r1->bottom);
}

void UTLMulDivRect(LPRECT r1, LPRECT r2, int num, int div)
{
	r1->left = MulDiv(r2->left, num, div);
	r1->top = MulDiv(r2->top, num, div);
	r1->right = MulDiv(r2->right, num, div);
	r1->bottom = MulDiv(r2->bottom, num, div);
}

void UTLGlobalUnlock (HGLOBAL h)
{
	if (h == 0) return;
	int nLocks = (GlobalFlags (h) & GMEM_LOCKCOUNT);
	for (int i = 0; i < nLocks; i++)
		GlobalUnlock (h);
}

void CVisualTextApp::OnPageSetup()
{
	CPageSetupDialog dlg;
	PAGESETUPDLG& psd = dlg.m_psd;
	psd.Flags |= PSD_MARGINS | PSD_INTHOUSANDTHSOFINCHES | PSD_DEFAULTMINMARGINS;
	int nUnitsPerInch = 1000;
	UTLMulDivRect(&psd.rtMargin, theApp.m_rectPrintMargins, nUnitsPerInch, 1440);
	UTLRoundRect(&psd.rtMargin);

	// get the current device from the app
	PRINTDLG pd;
	pd.hDevNames = NULL;
	pd.hDevMode = NULL;
	GetPrinterDeviceDefaults(&pd);
	psd.hDevNames = pd.hDevNames;
	psd.hDevMode = pd.hDevMode;
	
	if (dlg.DoModal() == IDOK)
	{
		CRect cRMin, cRMinMargins;
		dlg.GetMargins(NULL, &cRMin);
		UTLRoundRect(&cRMin);
		UTLMulDivRect(cRMinMargins, &cRMin, 1440, nUnitsPerInch);
		m_MinMarginX = cRMinMargins.right;
		m_MinMarginY = cRMinMargins.bottom;

		UTLRoundRect(&psd.rtMargin);
		UTLMulDivRect(theApp.m_rectPrintMargins, &psd.rtMargin, 1440, nUnitsPerInch);
		theApp.SelectPrinter(psd.hDevNames, psd.hDevMode);
		CString strPrintMargins;
		strPrintMargins.Format(_T("%u %u %u %u"),
			theApp.m_rectPrintMargins.left,
			theApp.m_rectPrintMargins.top,
			theApp.m_rectPrintMargins.right,
			theApp.m_rectPrintMargins.bottom);
		WriteProfileString(_T("PREFERENCES"),_T("PRINTMARGINS"),strPrintMargins);

		CVisualTextView* pView = (CVisualTextView*)GetFirstVisualTextView();
		if (pView)
			pView->SetMargins(m_rectPrintMargins);
	}
}

void CVisualTextApp::OnFileSaveanalyzeras() 
{
	theApp.CreateNewAnalyzer(false,true,true);//PD 02/19/2001
//	theApp.CreateNewAnalyzer(false,false,true);
}

void CVisualTextApp::OnUpdateFileSaveanalyzeras(CCmdUI* pCmdUI) 
{
	switch (appState) {
		case VT_APP_ANALYZING:
		case VT_APP_DIR_ANALYZING:
			pCmdUI->Enable(false);	
			break;
		default:
//			pCmdUI->Enable(appBoolAnalyzerLoaded && kbDirty);	
			pCmdUI->Enable(appBoolAnalyzerLoaded);	
			break;
	}
}

void CVisualTextApp::OnUpdateFileMruFile1(CCmdUI* pCmdUI) 
{
	if (!appBoolAnalyzerLoaded)//PD 02/19/2001
		pCmdUI->Enable(false);	//PD 02/19/2001
	else if (pCmdUI->m_nIndex == 0)
		OnUpdateRecentFileMenu(pCmdUI);
}

// SRP 010508 Subclassed this virtual function to avoid a Purify complaint
// Complaint is benign
BOOL CVisualTextApp::GetPrinterDeviceDefaults(PRINTDLG* pPrintDlg)
{
	UpdatePrinterSelection(m_hDevNames == NULL); //force default if no current
	if (m_hDevNames == NULL)
		return FALSE;               // no printer defaults

	pPrintDlg->hDevNames = m_hDevNames;
	pPrintDlg->hDevMode = m_hDevMode;

//	::GlobalUnlock(m_hDevNames);
//	::GlobalUnlock(m_hDevMode);
	UTLGlobalUnlock(m_hDevNames);
	UTLGlobalUnlock(m_hDevMode);
	
	return TRUE;
}

int CVisualTextApp::ExitInstance() 
{
	logmsg(_T("Enter ExitInstance"));
	delete m_pOutputDocTemplate;
	delete m_pGrammarDocTemplate;
	delete m_pAnalyzerDocTemplate;
	delete m_pTextDocTemplate;

   // Remove VisualText Runtime System.			                // 08/29/02 AM.
	if (vtrun)
	   {
		if (cg)	// 07/24/03 AM.
			{
			logmsg(_T("  Before deleteCG"));
			vtrun->deleteCG(cg);	// 07/18/03 AM.
			cg = 0;	// 07/18/03 AM.
			logmsg(_T("  After deleteCG"));
			}
		if (nlp)	// 07/18/03 AM.
			{
			logmsg(_T("  Before Nlp zero CG"));
			nlp->setCG(0);	// So it won't be deleted below.	// 07/18/03 AM.
			logmsg(_T("  After Nlp zero CG"));
			logmsg(_T("  Before deleteNLP"));	// 07/24/03 AM.
			vtrun->deleteNLP(nlp);	// 07/24/03 AM.
			nlp = 0;	// 07/24/03 AM.
			logmsg(_T("  After deleteNLP"));	// 07/24/03 AM.
			}
		logmsg(_T("  Before deleteVTRun"));
		VTRun::deleteVTRun(vtrun);	// 07/19/03 AM.
      vtrun = 0;                                                // 08/29/02 AM.
		logmsg(_T("  After deleteVTRun"));
      }
	else	// 07/24/03 AM.
		logmsg(_T("  EMPTY VTRUN"));	// 07/24/03 AM.

	logmsg(_T("Leave ExitInstance"));
	return CWinApp::ExitInstance();
}
	
CView* CVisualTextApp::GetFirstVisualTextView(void)
{
	CmyMultiDocTemplate *pTemplate;
	POSITION pos = GetFirstDocTemplatePosition();
	while (pos != NULL) {
		pTemplate = (CmyMultiDocTemplate*) GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CmyMultiDocTemplate)));
		POSITION docPos = pTemplate->CmyMultiDocTemplate::GetFirstDocPosition();
		while (docPos != NULL) {
			CVisualTextDoc *pDoc = (CVisualTextDoc *)pTemplate->CmyMultiDocTemplate::GetNextDoc(docPos);
			if (pDoc) {
				if (pDoc->IsKindOf(RUNTIME_CLASS(CVisualTextDoc)) )
					return GetFirstView(pDoc);
			}
		}
	}
	return NULL;
}

void CVisualTextApp::FindKBAttribute(LPCTSTR lpStr)
{
	CString selStr = lpStr;
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	if (selStr.IsEmpty()) {
		wnd->AddDebugLine(_T("FAIL: Search for word in dictionary:"),_T("Word is blank"),true);
		return;
	}
	CONCEPT *wordConcept = cg->getWordConcept((LPTSTR)(LPCTSTR)selStr);
#if 0
	if (NULL == wordConcept) {
		selStr.MakeLower();
		wordConcept = cg->getWordConcept((LPTSTR)(LPCTSTR)selStr);
	}
#endif
	if (wordConcept) {
		wnd->OpenAttributeEditor(wordConcept, false);
	}
	else {
		wnd->AddDebugLine(_T("Find KB Attribute: Not in dictionary:"),selStr,true);
	}
}

void CVisualTextApp::AddWordToKBSystemDictionary(LPCTSTR lpWord)
{
	CString strWord = lpWord;
	strWord.TrimLeft();
	strWord.TrimRight();
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	if (strWord.IsEmpty()) {
		wnd->AddDebugLine(_T("FAIL: Add word to dictionary:"),_T("Word is blank"),true);
		return;
	}
	int pos = strWord.FindOneOf(_T("\r\n\t "));
	if (pos != -1) {
		CString strAdd;
		strAdd.Format(_T("Normally, only single words are added.")
			_T("\nSelect YES to add just the first word (%s)")
			_T("\nSelect NO to Cancel"), strWord.Left(pos));
		if (IDNO == AfxMessageBox(strAdd, MB_YESNO))
			return;
		strWord = strWord.Left(pos);
	}
	CONCEPT *concept = cg->getWordConcept((LPTSTR)(LPCTSTR)strWord);
#if 0
	if (NULL == concept) {
		CString strLower = strWord;
		strLower.MakeLower();
		if (concept = cg->getWordConcept((LPTSTR)(LPCTSTR)strLower))
			strWord = strLower;
	}
#endif
	if (concept) {
		CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
		if (pView)
			pView->SelectFromConcept(concept);
		wnd->AddDebugLine(_T("Word is already in dictionary:"),strWord,true);
		return;
	}

	if (concept = cg->addWord((LPTSTR)(LPCTSTR)strWord) ) {
		wnd->AddDebugLine(_T("Added Word to dictionary:"),strWord,true);
		CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
		if (pView) {
			pView->Restore();
			pView->SelectFromConcept(concept);
		}
	}
	else {
		wnd->AddDebugLine(_T("Unable to add word to dictionary:"),strWord,true);
	}
}

int CVisualTextApp::RecentFileListSize(void)
{
	if (m_pRecentFileList == NULL)
		return 0;
	int n = 0;
	for (n = 0; n < m_pRecentFileList->GetSize(); n++) {
		if (m_pRecentFileList->m_arrNames[n].IsEmpty())
			break;
	}
	return n;
}