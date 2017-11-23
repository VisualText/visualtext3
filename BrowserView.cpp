/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// BrowserView.cpp : implementation of the CBrowserView class
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "MainFrm.h"
#include "Resource.h"
#include "BrowserDoc.h"
#include "BrowserView.h"

extern CString appURL;
extern CVisualTextApp theApp;
extern CString appAskText;
extern VT_APP_STATE appState;
extern CString appStrFocusDocumentType;
extern CDocument *appDocFocus;	
extern bool appBoolAnalyzerLoaded;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrowserView

IMPLEMENT_DYNCREATE(CBrowserView, CHtmlView)

BEGIN_MESSAGE_MAP(CBrowserView, CHtmlView)
	//{{AFX_MSG_MAP(CBrowserView)
	ON_COMMAND(ID_BACK_URL, OnBackUrl)
	ON_COMMAND(ID_OPTIONS_BACK, OnOptionsBack)
	ON_COMMAND(ID_OPTIONS_FORWARD, OnOptionsForward)
	ON_COMMAND(ID_OPTIONS_STOP, OnOptionsStop)
	ON_COMMAND(ID_URL_STOP, OnUrlStop)
	ON_COMMAND(ID_FORWARD_URL, OnForwardUrl)
	ON_COMMAND(ID_CLEAR_URL, OnClearUrl)
	ON_COMMAND(ID_ADD_URL, OnAddUrl)
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_RUN_HTML, OnRunHtml)
	ON_COMMAND(ID_SPIDER, OnSpider)
	ON_UPDATE_COMMAND_UI(ID_SPIDER, OnUpdateSpider)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_NEW_BROWSER, OnNewBrowser)
	ON_UPDATE_COMMAND_UI(ID_BACK_URL, OnUpdateBackUrl)
	ON_UPDATE_COMMAND_UI(ID_FORWARD_URL, OnUpdateForwardUrl)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CHtmlView::OnFilePrint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowserView construction/destruction

CBrowserView::CBrowserView()
{
	// TODO: add construction code here

}

CBrowserView::~CBrowserView()
{
}

BOOL CBrowserView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CHtmlView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CBrowserView drawing

void CBrowserView::OnDraw(CDC* pDC)
{
	CBrowserDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void CBrowserView::OnInitialUpdate()
{
	m_icon = AfxGetApp()->LoadIcon(IDR_BROWSER);
	GetParentFrame()->SetIcon(m_icon,true);

	CHtmlView::OnInitialUpdate();
	Navigate2(appURL);
}

/////////////////////////////////////////////////////////////////////////////
// CBrowserView printing


/////////////////////////////////////////////////////////////////////////////
// CBrowserView diagnostics

#ifdef _DEBUG
void CBrowserView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CBrowserView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CBrowserDoc* CBrowserView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBrowserDoc)));
	return (CBrowserDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBrowserView message handlers

void CBrowserView::OnTitleChange(LPCTSTR lpszText) 
{
	CString titleStr = GetLocationName();
	CBrowserDoc *doc = GetDocument();
	doc->SetTitle(titleStr);
}

void CBrowserView::OnOptionsBack() 
{
	OnBackUrl();
}

void CBrowserView::OnOptionsForward() 
{
	OnForwardUrl();
}

void CBrowserView::OnOptionsStop() 
{
	appState = VT_APP_STOP_COMMAND;
	OnUrlStop();	
}

void CBrowserView::OnUrlStop() 
{
	Stop();
}

void CBrowserView::OnBackUrl() 
{
	GoBack();
}

void CBrowserView::OnForwardUrl() 
{
	GoForward();	
}

void CBrowserView::OnAddUrl()
{
	FetchURLText();	
}

void CBrowserView::OnClearUrl() 
{
	CXTPControlComboBox* comboBoxURL = (CXTPControlComboBox*)theApp.m_pMainFrame->m_browserToolBar->GetControl(3);

	if (AfxMessageBox(_T("Do you want to clear all the URLs?"), MB_YESNO) == IDYES) {
		comboBoxURL->ResetContent();
	}
}

void CBrowserView::FetchURLText()
{
	CString htmlTextStr;
	StartWaitCursor();

	if (FetchURLString(appURL,htmlTextStr)) {
		if (AskForText(_T("Enter file name (without \".html\"):"))) {
			theApp.m_pMainFrame->m_wndTextView.AddTextFile(appAskText,htmlTextStr,_T("htm"));
		}
	} else {
		AfxMessageBox(_T("Could not retrieve HTML text"));
	}
	StopWaitCursor();
}

void CBrowserView::SaveState() 
{
	theApp.AddStatus(_T("browseropen"),StateStr());
}

CString CBrowserView::StateStr() 
{
	CString pathName = GetLocationURL();

	// VT Browser doesn't recognize the file:/// format, so
	// need to strip that off.	// 11/04/02 AM.
	// if starts with file:/// strip it.
	if (pathName.Find(_T("file:///")) >= 0)	// 11/04/02 AM.
		pathName = pathName.Right(pathName.GetLength()-8);	// 11/04/02 AM.

	// Don't ask me how I found this.  Took one year!
	WINDOWPLACEMENT wp;
	GetParentFrame()->GetWindowPlacement(&wp);

	int scrollPos = 0;

 	CString posStr;
	posStr.Format(_T("%s%c%d%c%d%c%d%c%d%c%d"),
	//posStr.Format(_T("%s%c%d"),
		pathName,STATUSSEP,
		wp.rcNormalPosition.top,STATUSSEP,
		wp.rcNormalPosition.bottom,STATUSSEP,
		wp.rcNormalPosition.left,STATUSSEP,
		wp.rcNormalPosition.right,STATUSSEP,
		scrollPos
	);

	return posStr;
}

void CBrowserView::OnDestroy() 
{
	CMainFrame *wnd = (CMainFrame*)AfxGetMainWnd();
	if (GetDocument() == wnd->m_pDocBrowserDictLookup)
		wnd->m_pDocBrowserDictLookup = 0;
	SaveState();
	CHtmlView::OnDestroy();
	if (theApp.GetNumberOfOpenViews() == 1)
		appDocFocus = NULL; // SRP 010505
}

void CBrowserView::OnNavigateComplete2(LPCTSTR strURL) 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->UpdateUrlPulldown(strURL);

	CHtmlView::OnNavigateComplete2(strURL);
}

void CBrowserView::OnSetFocus(CWnd* pOldWnd) 
{
	CHtmlView::OnSetFocus(pOldWnd);
	
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->UpdateUrlPulldown(GetLocationURL());

	appStrFocusDocumentType = _T("VisualText");
	appDocFocus = GetDocument();	
}

void CBrowserView::OnNewBrowser() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->BrowserNavigate(_T(""),true);
}

void CBrowserView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CHtmlView::OnBeforeNavigate2(lpszURL, nFlags, lpszTargetFrameName, baPostedData, lpszHeaders, pbCancel);
}

void CBrowserView::OnRunHtml() 
{
	CString htmlTextStr = _T("");
	if (FetchURLString(appURL,htmlTextStr)) {
		if (AskForText(_T("Enter file name (without \".html\"):"))) {
			CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
			wnd->ClearOutput();

			CString htmlPathStr = theApp.m_pMainFrame->m_wndTextView.AddTextFile(appAskText,htmlTextStr,_T("htm"));
			CString strippedPathStr = StripPathReverse(RelativizePath(htmlPathStr),0,true);
			theApp.m_pMainFrame->m_wndTextView.SelectTextFromPath(strippedPathStr);
			wnd->RunAnalyzer(strippedPathStr);

			CString outputPathStr = theApp.m_pMainFrame->m_wndTextView.ProcessedFileStr(_T("output.txt"));
			if (FileExists(outputPathStr))
				theApp.OpenDocumentFile(outputPathStr);
		}
	}
}

void CBrowserView::OnSpider() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	VT_APP_STATE prevAppState = appState;

	CString processedFile = wnd->GetProcessText();

	CString filePathStr = InputPath(processedFile);
	CString fileRelPathStr = StripPathReverse(RelativizePath(filePathStr),0,true);
	CString homePageURLStr = GetUrlFromUrlFile(filePathStr);
	CString htmlTextStr = _T("");
	int level = 1;
	int depth;
	bool trying = true;

	while (trying) {
		trying = AskForText(_T("Enter a depth number from 1 to 3:"),_T("1"));
		if (IsDigit(appAskText)) {
			depth = _ttoi((const _TCHAR *)appAskText);
			if (depth >= 1 && depth <= 3)
				break;
		}
	}
	if (!trying)
		return;

	if (FetchURLString(homePageURLStr,htmlTextStr)) {
		appState = VT_APP_SPIDERING;
		wnd->SetSpideringToolbarButtons(false);

		wnd->ClearOutput();
		//wnd->ClearWatch();  depricated  de Hilster 12/04/29

		if (!RecursiveRemove(DumpDirectory(filePathStr)))
			return;

		wnd->SetProcessText(fileRelPathStr);
		wnd->RunAnalyzer(fileRelPathStr);

		CString urlsStr = theApp.m_pMainFrame->m_wndTextView.ProcessedFileStr(_T("urls.txt"));

		if (FileExists(urlsStr)) {

			appState = VT_APP_SPIDERING;
			CStringList rawUrls,urls,visitedUrls,levelUrls;
			ReadFileToStringList(rawUrls,urlsStr);
			MergeUrlsWithAnchors(urls,rawUrls,homePageURLStr);
			visitedUrls.AddHead(homePageURLStr);

			CString urlStr;
			CString urlPathStr;
			CString spideredPathStr;
			CString strippedPathStr;
			CString msgStr;
			int count = 0;
			bool levelDelete = true;
			CString parentPathStr = StripPath(homePageURLStr,0,true,'/');

			POSITION pos = urls.GetHeadPosition();
			int urlCount = urls.GetCount();

			while (pos && depth && (urlStr = urls.GetNext(pos))) {

				PeekHere(10);

				count++;

				if (appState == VT_APP_STOP_COMMAND)
					break;

				//if (level == 1 && !MergeUrlAndAnchor(parentPathStr,urlListStr,urlStr))
					//continue;

				// CHECK TO SEE IF IT WAS ALREADY SPIDERED
				if (visitedUrls.GetCount() == 0 || !visitedUrls.Find(urlStr))
					visitedUrls.AddHead(urlStr);
				else
					continue;

				// IF FOUND, PROCESS AND MERGE
				if (FetchURLString(urlStr,htmlTextStr)) {

					urlPathStr = _T("spiderTempFile");

					spideredPathStr = theApp.m_pMainFrame->m_wndTextView.AddTextFile(urlPathStr,htmlTextStr,_T("htm"),false);
					strippedPathStr = StripPathReverse(RelativizePath(spideredPathStr),0,true);

					if (theApp.IsDocumentTypeOpen(_T("Browser")))
						theApp.UpdateAnalyzerWindows(true,true);

					wnd->ClearOutput();
					msgStr.Format(_T("Spidering level %d (%d of %d):"),level,count,urlCount);
					wnd->AddDebugLine(msgStr,urlStr);
					wnd->AddFindLine(msgStr,urlStr);
					wnd->SetProcessText(strippedPathStr);
					wnd->RunAnalyzer(strippedPathStr);
					appState = VT_APP_SPIDERING;

					MergeDumpFiles(parentPathStr,filePathStr,spideredPathStr,levelUrls,depth);
					if (!RecursiveRemove(spideredPathStr))
						return;
					if (!RecursiveRemove(DumpDirectory(spideredPathStr)))
						return;
				}

				if (!pos) {
					depth--;
					level++;
					if (depth) {
						visitedUrls.RemoveAll();
						levelDelete = false;
						SortStringList(urls);
						RemoveSuccessiveEquals(urls);
						pos = urls.GetHeadPosition();
						urlCount = urls.GetCount();
						count = 0;
					}
				}
			}

			urls.RemoveAll();
			visitedUrls.RemoveAll();
			if (levelDelete)
				levelUrls.RemoveAll();
		}

		wnd->AddFindLine(_T("Spidering Status:"),_T("Done"),true);

		appState = prevAppState;
		wnd->SetSpideringToolbarButtons(true);
	}

	theApp.m_pMainFrame->m_wndTextView.SelectTextFromPath(processedFile);
	theApp.m_pMainFrame->m_wndTextView.SelectTextFromPath(_T(""));
}

void CBrowserView::MergeDumpFiles(CString urlPathStr, CString homePagePathStr,
								  CString spideredPathStr, CStringList &levelUrls, int depth) 
{
	CString homeDirPathStr = DumpDirectory(homePagePathStr);
	CString spideredDirPathStr = DumpDirectory(spideredPathStr);
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CStringList dumpFiles;
	theApp.m_pMainFrame->m_wndTextView.ProcessedFiles(dumpFiles,_T(".txt"),spideredDirPathStr);

	POSITION pos = dumpFiles.GetHeadPosition();
	CString dumpFilePathStr;
	CString dumpFileStr;
	CString homePageDumpPathStr;
	CString spideredDumpPathStr;
	CString spideredContentStr;
	CStringList rawUrlList,newUrlList;

	while (pos && (dumpFilePathStr = dumpFiles.GetNext(pos))) {
		dumpFileStr = StripPath(dumpFilePathStr,0);
		homePageDumpPathStr = theApp.m_pMainFrame->m_wndTextView.ProcessedFileStr(dumpFileStr,homeDirPathStr);
		spideredDumpPathStr = theApp.m_pMainFrame->m_wndTextView.ProcessedFileStr(dumpFileStr,spideredDirPathStr);

		if (depth > 1 && dumpFilePathStr.Find(_T("\\urls.txt")) > 0) {
			ReadFileToStringList(rawUrlList,dumpFilePathStr);
			MergeUrlsWithAnchors(newUrlList,rawUrlList,urlPathStr);
			MergeStringLists(levelUrls,newUrlList);
			newUrlList.RemoveAll();
			rawUrlList.RemoveAll();
		}

		if (FileExists(homePageDumpPathStr)) {
			spideredContentStr = NLPReadFile(spideredDumpPathStr);
			AppendToFile(homePageDumpPathStr,spideredContentStr);
		}
		else
			MoveFile(spideredDumpPathStr,homePageDumpPathStr);
	}
}

CString CBrowserView::DumpDirectory(CString filePathStr) 
{
	CString dirPathStr = filePathStr;
	dirPathStr += _T("_log\\");
	return dirPathStr;
}

void CBrowserView::MergeUrlsWithAnchors(CStringList &newList, CStringList &anchorList, CString urlPathStr) 
{
	POSITION pos = anchorList.GetHeadPosition();
	CString anchorStr;
	CString newUrlStr;

	while (pos && (anchorStr = anchorList.GetNext(pos))) {
		if (MergeUrlAndAnchor(urlPathStr,anchorStr,newUrlStr)) {
			if (!newList.Find(newUrlStr,0))
				newList.AddTail(newUrlStr);
		}
	}
}

bool CBrowserView::MergeUrlAndAnchor(CString urlStr, CString anchorStr, CString &newUrlStr)
{
	newUrlStr = _T("");

	if (anchorStr.GetLength() <= 2)
		return false;
	else if (anchorStr.Find(_T("http://"),0) == 0) {
		newUrlStr = anchorStr;
		return true;
	}

	_TCHAR firstChar = anchorStr.GetAt(0);
	_TCHAR secondChar = anchorStr.GetAt(1);
	CString workingPathStr;
	CString workingUrlStr;

	if (firstChar == '#')
		return false;

	// IF THE ANCHOR STARTS WITH A "..", DO UPWARD MOVEMENT
	else if (firstChar == '.' && secondChar == '.') {
		workingPathStr = StripPath(urlStr,1,true,'/');
		workingUrlStr = anchorStr;
		ClipLeft(workingUrlStr,2);
		newUrlStr = workingPathStr + workingUrlStr;
	}

	// IF THE ANCHOR STARTS WITH A "./", DO UPWARD MOVEMENT
	else if (firstChar == '.' && secondChar == '/') {
		workingPathStr = StripPath(urlStr,0,true,'/');
		workingUrlStr = anchorStr;
		ClipLeft(workingUrlStr,1);
		newUrlStr = workingPathStr + workingUrlStr;
	}

	// IF THE ANCHOR STARTS WITH A '/', THEN IT GETS GLOMMED ONTO THE
	// URL HOME PAGE ROOT
	else if (firstChar == '/') {
		int i = urlStr.Find(_T("/"),8);
		workingPathStr = urlStr.Mid(0,i);
		newUrlStr = workingPathStr + anchorStr;
	}

	// IF THE ANCHOR IS A WEBPAGE (ENDS IN HTM OR HTML) THEN STRIP AND GLOM
	else if (EndsWith(urlStr,_T(".html")) || EndsWith(urlStr,_T(".htm"))) {
		workingPathStr = StripPath(urlStr,0,true,'/');
		newUrlStr = workingPathStr + _T("/") + anchorStr;
	}

	// IF THE ANCHOR DOES NOT START WITH http://, THEN IT GETS GLOMMED ONTO THE
	// CURRENT PAGE PATH
	else if (urlStr.GetAt(urlStr.GetLength()-1) == '/')
		newUrlStr = urlStr + anchorStr;

	else
		newUrlStr = urlStr + _T("/") + anchorStr;
	
	return true;
}

void CBrowserView::OnUpdateSpider(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(appBoolAnalyzerLoaded && appState != VT_APP_SPIDERING && appState != VT_APP_ANALYZING);	
}

void CBrowserView::OnFileNew() 
{
	OnNewBrowser();
}

void CBrowserView::OnUpdateBackUrl(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here	
}

void CBrowserView::OnUpdateForwardUrl(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
}

void CBrowserView::OnFileSaveAs() 
{
	CString htmlTextStr;
	StartWaitCursor();

	if (FetchURLString(appURL,htmlTextStr)) {
		CString htmlStr = StripPath(appURL,0,false,'/');
		CString filterStr = _T("Html files (*.htm;*.html)|*.htm|Text files (*.txt)|*.txt|All files (*.*)|*.*||");
		CFileDialog fileDlg(false,_T("html"),htmlStr,OFN_OVERWRITEPROMPT,filterStr);

		if (fileDlg.DoModal() == IDOK)
			theApp.m_pMainFrame->m_wndTextView.AddTextFile(appAskText,htmlTextStr,_T("html"));
	}
	StopWaitCursor();
}

