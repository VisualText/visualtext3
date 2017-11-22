/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// VisualText.h : main header file for the GUI_VISUALSTUDIO6 application
//

#if !defined(AFX_GUI_VISUALSTUDIO6_H__E3FFF485_E22F_4F65_B3CF_F588F1B4D365__INCLUDED_)
#define AFX_GUI_VISUALSTUDIO6_H__E3FFF485_E22F_4F65_B3CF_F588F1B4D365__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "SplashDlg.h"
#include "BrowserDoc.h"
#include "mDOCMULTI.h"

#define COLOR_MASK	RGB(0x00,0xFF,0x00)

class CMainFrame;

const TCHAR TAB_GRAM[] = _T( "Gram" );
const TCHAR TAB_ANA[]   = _T( "Ana" );
const TCHAR TAB_TEXT[]  = _T( "Text" );

#define STATUSSEP '\t'
#define LARGE_TEXT_SIZE 1000000

enum ANAL_PASS_TYPE {
	ANAL_PASS_NONE = 0,
	ANAL_PASS_PAT,
	ANAL_PASS_SYSTEM,
	ANAL_PASS_STUB,
	ANAL_PASS_END,
	ANAL_PASS_REC,
	ANAL_PASS_AUTO,
	ANAL_PASS_USER,
	ANAL_PASS_GROUP,
	ANAL_PASS_MAIN,
	ANAL_PASS_FOLDER,
};

enum GRAM_NODE_TYPE {
	GRAM_NODE_NONE = 0,
	GRAM_NODE_STUB,
	GRAM_NODE_FOLDER,
	GRAM_NODE_SAMPLE,
	GRAM_NODE_LABEL,
	GRAM_NODE_RULE,
};

enum GRAM_GEN_TYPE {
	GRAM_GEN_ALL = 0,
	GRAM_GEN_UNLOCKED,
	GRAM_GEN_DIRTY,
};

enum DOC_TYPE {
	DOC_NONE = 0, // SRP 010510
	DOC_FILE,
	DOC_SEQUENCE,
	DOC_RULE,
	DOC_REC,
	DOC_KNOWLEDGE,
	DOC_HTML,
	DOC_LOG,
	DOC_SYS_LOG,
	DOC_SAMPLE,
	DOC_TREE,
	DOC_DICTIONARY,
};

enum DOC_ACTION {
	DOC_ACTION_NOOP = 0,
	DOC_ACTION_AUTOSIZE,
	DOC_ACTION_UPDATE,
	DOC_ACTION_UPDATE_FONT,
	DOC_ACTION_UPDATE_AUTO_GENERATED,
	DOC_ACTION_SAVE,
	DOC_ACTION_SAVE_CONFIRM,
	DOC_ACTION_SAVE_STATE,
	DOC_ACTION_DESTROY_ALL,
	DOC_ACTION_DESTROY_ALL_BUT,
	DOC_ACTION_MINIMIZE,
	DOC_ACTION_UNHIGHLIGHT_ALL,
};

enum VT_APP_STATE {
	VT_APP_NORMAL = 0,
	VT_APP_LOADING,
	VT_APP_LOADED,
	VT_APP_ANALYZING,
	VT_APP_DIR_ANALYZING,
	VT_APP_GENERATING,
	VT_APP_SPIDERING,
	VT_APP_STOP_COMMAND,
	VT_APP_WORD_NETTING,
	VT_APP_GENERATING_TREE,
	VT_APP_EXITING,
};

/////////////////////////////////////////////////////////////////////////////
// CVisualTextApp:
// See VisualText.cpp for the implementation of this class
//

class CVisualTextApp : public CWinApp
{
public:
	CVisualTextApp();
	~CVisualTextApp();

	CmyMultiDocTemplate *m_pOutputDocTemplate;
	CmyMultiDocTemplate *m_pGrammarDocTemplate;
	CmyMultiDocTemplate *m_pAnalyzerDocTemplate;
	CmyMultiDocTemplate *m_pTextDocTemplate;

	CmyMultiDocTemplate *m_pVisualText;
	CmyMultiDocTemplate *m_pBrowser;
	CmyMultiDocTemplate *m_pKnowledge;
	CmyMultiDocTemplate *m_pParseTree;
	CmyMultiDocTemplate *m_pTextEdit;
	CmyMultiDocTemplate *m_pSyntaxEditor;

	void UpdateSyntaxAllViews(CView* pSender=NULL);

	CRect m_rectPrintMargins; // twips
	int m_MinMarginX; // twips
	int m_MinMarginY; // twips

	CDocument *OpenNewDocument(const CString &strTarget, CString filePath, bool showFlag=true);
	CDocument *GetDocument(CString docType);
	CView *GetView(CString docType);
	void DoAllViews(CView *activeView, CString docType, CString extStr=_T(""), DOC_ACTION docAction=DOC_ACTION_NOOP);
	bool AnalyzerUsingFile(CString filePathStr);
	void SetAnalyzerRuleFileModified(CString pathStr);
	void RunAnalyzer();
	CDocument *ViewLogFile(CDocument *doc);
	bool SaveAnalyzerStatusFile();
	bool ReadAnalyzerStatusFile(CString in_fileName);
	void AddStatus(CString nameStr, CString valueStr);
	void AddStatus(CString nameStr, int valInt);
//	void DumpAppGUIConcept(void);	// 06/26/03 AM.
	void LoadAppGUIConcept(void);
	bool OpenAnalyzer(bool newFlag=false);
	void CloseAllViews(bool temporary=false);
	void CloseAnalyzer();
	void AddRecentAnalyzer(CString newAnalyzerStr=_T(""));
	void RemoveRecentAnalyzer(CString oldAnalyzerStr);
	void SaveAnalyzer(bool askConfirm=false);
	void SaveKBx(bool askConfirm=false);	// 10/12/03 AM.
	bool CreateNewAnalyzer(bool loadTemplate=true, bool askToCloseCurrentAnalyzer=false,
		bool saveAs=false);
	CBrowserDoc *IsBrowserDocumentOpen(CString filePathStr);
	CDocument *IsVisualTextDocumentOpen(CString docTypeStr, CString filePathStr);
	void SaveAllStates();
	void UpdateDocFromPath(CString docPathStr);
	void CreateSplash();
	void DestroySplash();
	void UpdateAnalyzerWindows(bool forceFlag=false, bool changeBrowser=true, bool updatingCurrent=false);
	CString GetDocumentTypeString(CDocument *docToCompare);
	CDocument *IsDocumentTypeOpen(CString docTypeStr);
	void UpdateParseTrees(int newPass);
	bool RunDictionaryLookup(CString filePathStr, LPCTSTR lpURL=NULL, LPCTSTR lpNameID=NULL);
	void Dictionize(CString filePathInStr);
	void MaskText(CString fileNameStr);
	void JumbleText(CString fileNameStr);
	CString GetAppStatusValue(CString keyStr);
	bool SendOneMessage(CString emailStr, CString subjectStr, CString bodyStr);
	UINT EmailThreadProc(LPVOID pParam);
	void SetWindowSize(CView *view, int width, int length, int fileLength, bool dontMove=false);
	void SavePageModeState();
	void RestoreGlobals(CStringList &stateList);
	void RestoreViews(CStringList &stateList, bool createIfDoesNotExist=false);
	void RestoreState(CStringList &stateList, bool createIfDoesNotExist=false);
	bool DoHourGlass(void);
	VT_APP_STATE GetAppState() const;
	bool DirtyDocs(CDocument *docIn=NULL);
	int GetNumberOfOpenViews(void); // SRP 010505
	CView* GetKBEditor(void);
	CString GetAppsDir(void); // de Hhilster 120412
	void SetPrinterMinMargins(void); // SRP 010506
	bool UpdateRuleFileTitles();
	int RecentFileListSize(void); // SRP 010511
	void DisplayAnalyzerLoadStats(bool);	// 05/26/01 AM.
	CView* GetFirstVisualTextView(void);
	void AddWordToKBSystemDictionary(LPCTSTR lpWord);
	void FindKBAttribute(LPCTSTR lpStr);
	CString TemplateFromExtension(LPCTSTR lpszFileName);

public:
	CDocument* OpenDocumentFile(const LPCTSTR lpszFileName, const CString &templateName, const bool showFlag) ; 
	CDocument* OpenDocumentFile(const LPCTSTR lpszFileName, const CString &templateName) ; 
	CDocument* OpenDocumentFile(const LPCTSTR lpszFileName, const bool showFlag);

private:
	CSplashDlg m_splashDlg;

	bool ReadStatusFile(CString filePathStr);
	void OnRulesFile();
	int AddRuleMenuItems();
	void InitAnalyzer();
	void InitAppGlobals(bool firstTime=false);
	void LoadRecentAnalyzersString();
	void LoadPreferences();
	void LoadFonts();
	void LoadCompiledKB();
//	void DisplayAnalyzerLoadStats();		// 05/19/01 AM.
	bool MovedPathStr(CString &pathStr);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisualTextApp)
	public:
	virtual BOOL InitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	virtual void OnFileOpen() ;
	virtual BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate) ;
	virtual int ExitInstance();
	virtual BOOL GetPrinterDeviceDefaults(PRINTDLG* pPrintDlg);
	//}}AFX_VIRTUAL

	CMainFrame*			m_pMainFrame;
	CmyMultiDocTemplate*	m_pDocTemplate;
	CmyMultiDocTemplate*	m_pDocWorkspTemplate;
	CFrameWnd*			m_pChildFrame;

// Implementation
	//{{AFX_MSG(CVisualTextApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnUpdateFileMruFile1(CCmdUI* pCmdUI);
	afx_msg void OnFileOpenanalyzer();
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI) ;
	afx_msg void OnFileCloseanalyzer();
	afx_msg void OnFileSaveanalyzer();
	afx_msg void OnChooseAnalyzer(UINT idIn);
	afx_msg void OnUpdateFileCloseanalyzer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveanalyzer(CCmdUI* pCmdUI);
	afx_msg void OnWindowCloseall();
	afx_msg void OnAnalyzerGenerateunlocked();
	afx_msg void OnAnalyzerGenerateall();
	afx_msg void OnAnalyzerGeneratedirty();
	afx_msg void OnFileSaveanalyzeras();
	afx_msg void OnUpdateFileSaveanalyzeras(CCmdUI* pCmdUI);
	afx_msg void OnToolsEmail();
	afx_msg void OnPageSetup();
	afx_msg void OnEditPaste();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditClear();
	afx_msg void OnEditUndo();
	//}}AFX_MSG

	afx_msg void OnTipOfTheDay();

	DECLARE_MESSAGE_MAP()

	void ShowTipAtStartup();
	void FindTipsFile(LPCTSTR lpszTipsFile);
};

extern CVisualTextApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSplitPath class

class CSplitPath
{
// Construction / destruction
public:

	// Constructs a CSplitPath object.
	CSplitPath(LPCTSTR lpszPathBuffer=NULL);

	// Destroys a CSplitPath object, handles cleanup and de-allocation.
	virtual ~CSplitPath();

// Member variables
protected:
   TCHAR m_szDrive[_MAX_DRIVE];
   TCHAR m_szDir[_MAX_DIR];
   TCHAR m_szFName[_MAX_FNAME];
   TCHAR m_szExt[_MAX_EXT];

// Member functions
public:
    void SplitPath(LPCTSTR lpszPathBuffer);
    CString GetDrive() const;
    CString GetDir() const;
    CString GetFName() const;
    CString GetExt() const;
	CString GetFullPath() const;
	CString GetFullName() const;
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE CString CSplitPath::GetDrive() const {
    return CString(m_szDrive);
}
AFX_INLINE CString CSplitPath::GetDir() const {
    return CString(m_szDir);
}
AFX_INLINE CString CSplitPath::GetFName() const {
    return CString(m_szFName);
}
AFX_INLINE CString CSplitPath::GetExt() const {
    return CString(m_szExt);
}
AFX_INLINE CString CSplitPath::GetFullPath() const {
	return GetDrive() + GetDir();
}
AFX_INLINE CString CSplitPath::GetFullName() const {
	return GetFName() + GetExt();
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GUI_VISUALSTUDIO6_H__E3FFF485_E22F_4F65_B3CF_F588F1B4D365__INCLUDED_)
