/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// OutputBarList.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "MainFrm.h"
#include "TreeViewAnalyzer.h"
#include "TreeViewGrammar.h"
#include "OutputBarList.h"
#include ".\outputbarlist.h"

extern CVisualTextApp theApp;
extern CString appStrLocation;

// COutputBarList

IMPLEMENT_DYNAMIC(COutputBarList, CListCtrl)
COutputBarList::COutputBarList()
{
}

COutputBarList::~COutputBarList()
{
}


BEGIN_MESSAGE_MAP(COutputBarList, CListCtrl)
//	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	ON_COMMAND(ID_FINDMENU_CLEAR, OnFindmenuClear)
	ON_COMMAND(ID_FINDMENU_OPEN, OnFindmenuOpen)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// COutputBarList message handlers

void COutputBarList::FindInFindTab()
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString fileStr;
	CString dirPathStr;
	CString findStr;
	CString fileOriginStr;
	int intVal,intVal2;

	DEBUG_TYPE type = GetSelectedFile(fileStr,findStr,fileOriginStr,intVal,intVal2);
	intVal--;

	if (type == DEBUG_NOOP) {
		return;
	}
	else if (type == DEBUG_FIND) {
		CString lowerFileStr = fileStr;
		CString lowerAppStrLocation = appStrLocation;
		lowerFileStr.MakeLower();
		lowerAppStrLocation.MakeLower();

		if (lowerFileStr.Find(lowerAppStrLocation) != 0)
			dirPathStr = AppPath(fileStr);
		else
			dirPathStr = fileStr;
		CDocument *doc;
		
		if (FileExists(dirPathStr)) {
			//if (!(doc = theApp.IsVisualTextDocumentOpen("VisualText",dirPathStr)))
			// IF PASS FILE, MOVE TO ANALYZER TAB AND SELECT PASS
			if (EndsWith(fileStr,_T("pat"))) {
				theApp.m_pMainFrame->m_wndAnalyzerView.EditRulesFile(dirPathStr); // SRP 010606, for Pass File Number in Doc Title Bar
			}
			doc = AfxGetApp()->OpenDocumentFile(dirPathStr);
			CVisualTextView *view = (CVisualTextView *)GetFirstView(doc);

			CRichEditCtrl &ed = view->GetRichEditCtrl();
			// Calling GetLineCount() must somehow force edit control to take stock of number of lines.
			// On certain machines (e.g. 450 MHz), LineIndex(intval) wrongly returned -1
			int lineCount = ed.GetLineCount();
			int lineIndex = ed.LineIndex(intVal);
			long start = lineIndex + intVal2;
			long end = start + findStr.GetLength();

			ed.SetSel(start, end);
			RECT rect;
			view->GetClientRect(&rect);
			int numVisibleLines = (rect.bottom - rect.top) / 16;
			int firstVisibleLine = ed.GetFirstVisibleLine();
			int selectionLine = ed.LineFromChar(start);
			int scrollIt = selectionLine - firstVisibleLine - numVisibleLines / 2;

			view->Scroll(scrollIt);
		}
	}
	else if (type == DEBUG_ERROR) {
		theApp.m_pMainFrame->m_wndAnalyzerView.ViewPassLog(intVal);
		wnd->SelectTab(WORKSPACE_ANALYZER);
	}
}

void COutputBarList::FindInAnalyzerTab()
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString fileStr;
	CString dirPathStr;
	CString findStr;
	CString fileOriginStr;
	int intVal1, intVal2;

	DEBUG_TYPE type = GetSelectedFile1(fileStr,findStr,fileOriginStr,intVal1,intVal2);
	if (type != DEBUG_PASS_LINE)
		intVal1--;

	if (type == DEBUG_NOOP) {
		return;
	}
	else if (type == DEBUG_PASS_LINE) {
		// SRP 010606, for Pass File Number in Doc Title Bar
		CString fileNameStr = StripPath(fileStr,0);
		ClipRight(fileNameStr,4);
		int pass = theApp.m_pMainFrame->m_wndAnalyzerView.PassFromFile(fileNameStr);
		wnd->SelectPass(pass,true);
		theApp.m_pMainFrame->m_wndAnalyzerView.SendMessage(WM_COMMAND,ID_SEQUENCEMENU_EDITRULES);

		CDocument *doc = AfxGetApp()->OpenDocumentFile(fileStr);
		CVisualTextView *view = (CVisualTextView *)GetFirstView(doc);
		CRichEditCtrl &ed = view->GetRichEditCtrl();
		int start = ed.LineIndex(--intVal2);
		view->SetFocus();
		ed.SetSel(start,start);
	}
#if 0 // All find stuff is now in FindListCtrl.cpp // SRP 010606
	else if (type == DEBUG_FIND) {
		if (EndsWith(fileStr,_T("log"))) {
			if (fileOriginStr != _T("")) {
				dirPathStr = fileOriginStr;
				if (EndsWith(fileOriginStr,_T(".txt")))
					dirPathStr += _T("_log\\") + fileStr;
			}
			else
				dirPathStr = wnd->GetProcessTextFile(fileStr);
		}
		else if (EndsWith(fileStr,_T(".pat")))
			dirPathStr = RulesPath(fileStr,true);
		else if (EndsWith(fileStr,_T(".txt")))
			dirPathStr = InputPath(fileStr);
		else if (EndsWith(fileStr,_T(".smp")))
			dirPathStr = GramPath(fileStr);
		else
			return;

		CDocument *doc = AfxGetApp()->OpenDocumentFile(dirPathStr);
		CVisualTextView *view = (CVisualTextView *)GetFirstView(doc);

		CRichEditCtrl &ed = view->GetRichEditCtrl();
		
		_TCHAR buff[1001];
		int count = ed.GetLineCount();  // THIS JUST INITIALIZES THE ED!  :-(
		long start = ed.LineIndex(intVal1);
		long len = ed.GetLine(intVal1,buff,1000);
		buff[len] = '\0';
		CString buffStr = buff;
		start += buffStr.Find(findStr);
		long end = start + findStr.GetLength();
		view->SetFocus();
		ed.SetSel(start, end);
	}
#endif
	else if (type == DEBUG_ERROR) {
		//wnd->ViewLogFile(InputPath(fileStr),intVal);
		theApp.m_pMainFrame->m_wndAnalyzerView.ViewPassLog(intVal1);
		wnd->SelectTab(WORKSPACE_ANALYZER);
	}
	else if (type == DEBUG_RULE) {
		//wnd->ViewLogFile(InputPath(fileStr),intVal);
		CString fileNameStr = theApp.m_pMainFrame->m_wndAnalyzerView.CleanPassNameFromPass(intVal1);

		// SRP 010606, for Pass File Number in Doc Title Bar
		int pass = intVal1;
		wnd->SelectPass(pass,true);
		theApp.m_pMainFrame->m_wndAnalyzerView.SendMessage(WM_COMMAND,ID_SEQUENCEMENU_EDITRULES);

		wnd->SelectTab(WORKSPACE_GRAMMAR);
		theApp.m_pMainFrame->m_wndGrammarView.SetSelection(fileNameStr);
	}
}

void COutputBarList::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	switch (pNMHDR->idFrom) {
		case IDC_SHEET1:
		case IDC_SHEET2:			// FIX.	// 02/19/05 DD.
			FindInAnalyzerTab();
			break;
		case IDC_SHEET3:
		case IDC_SHEET4:
			FindInFindTab();
			break;
	}

	// TODO: Add your control notification handler code here
	*pResult = 0;
}

DEBUG_TYPE COutputBarList::GetSelectedFile(CString &fileStr, CString &findStr,
									   CString &fileOriginStr, int &intVal, int &intVal2)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	_TCHAR buff[1001];
	int line = GetSelectionMark();
	GetItemText(line,0,buff,1000);
	CString nameStr = buff;
	GetItemText(line,1,buff,1000);
	CString viewStr = buff;
	findStr = _T("");
	fileOriginStr = _T("");

	DEBUG_TYPE type = DEBUG_NOOP;
	intVal = -1;
	CStringList parseList;

	if ((ParseLine(parseList,nameStr,_T("File: %a[%d %d]:")))) {
		fileStr = NthStringListItem(parseList,0);
		fileStr.TrimRight(_T(" "));
		intVal = _ttoi((const _TCHAR *)NthStringListItem(parseList,1));
		intVal2 = _ttoi((const _TCHAR *)NthStringListItem(parseList,2));

		// Get search word
		GetItemText(0,1,buff,1000);
		findStr = buff;
		// Get search origin file
		GetItemText(1,1,buff,1000);
		fileOriginStr = buff;

		type = DEBUG_FIND;
	}
	else if ((ParseLine(parseList,nameStr,_T("Concept: %w")))) {
		TokensBySeparator(parseList,viewStr, '/');
		wnd->SelectTab(WORKSPACE_GRAMMAR);
		theApp.m_pMainFrame->m_wndGrammarView.SelectFromStringList(parseList);
	}
	else if (nameStr.Find(_T("Spidering level")) == 0) {
		wnd->BrowserNavigate(viewStr);
	}

	return type;
}

DEBUG_TYPE COutputBarList::GetSelectedFile1(CString &fileStr, CString &findStr,
									    CString &fileOriginStr, int &intVal1, int &intVal2)
{
	_TCHAR buff[1001];
	int line = GetSelectionMark();
	GetItemText(line,0,buff,1000);
	CString nameStr = buff;
	GetItemText(line,1,buff,1000);
	CString viewStr = buff;
	findStr = _T("");
	fileOriginStr = _T("");

	DEBUG_TYPE type = DEBUG_NOOP;
	intVal1 = -1;
	intVal2 = -1;
	CStringList parseList;

	if ((ParseLine(parseList,viewStr,_T("%d %d [")))) {
		intVal1 = _ttoi((const _TCHAR *)NthStringListItem(parseList,0));
		intVal2 = _ttoi((const _TCHAR *)NthStringListItem(parseList,1));
		if (intVal1 > 0) { // SRP 010607 // if intVal == 0, the currently selected pass in the Ana Tab is opened.
			fileStr = theApp.m_pMainFrame->m_wndAnalyzerView.PassFileFromNumber(intVal1);
			if (FileExists(fileStr)) // SRP 010607 // Prevent trying to open system passes (like tokenize, that have no associated file)
				type = DEBUG_PASS_LINE;
		}
	}
	else if ((ParseLine(parseList,nameStr,_T("Log file: %w:")))) {
		fileStr = NthStringListItem(parseList,0);
		if ((ParseLine(parseList,viewStr,_T("[Pass %d")))) {
			intVal1 = _ttoi((const _TCHAR *)NthStringListItem(parseList,0));
			type = DEBUG_ERROR;
		}
	}
	else if (nameStr.CompareNoCase(_T("Processing pass:")) == 0) {
		if ((ParseLine(parseList,viewStr,_T("%d %w %w")))) {
			intVal1 = _ttoi((const _TCHAR *)NthStringListItem(parseList,0));
			type = DEBUG_ERROR;
		}
	}
	else if (nameStr.CompareNoCase(_T("Generating rules:")) == 0) {
		if ((ParseLine(parseList,viewStr,_T("%d %w %w")))) {
			intVal1 = _ttoi((const _TCHAR *)NthStringListItem(parseList,0));
			type = DEBUG_RULE;
		}
	}
	return type;
}


void COutputBarList::OnContextMenu(CWnd* pWnd, CPoint point)
{
if (GetItemCount() == 0)
   return;

UINT disables[100];

for (int i=0; i<100; i++)
   disables[i] = 0;

CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
//int tab = wnd->m_wndOutputBar.m_flatTabCtrl.GetCurSel();

// 02/19/05 DD.
PopupContextMenu(this,IDR_FIND,point,disables); 
}


void COutputBarList::OnFindmenuClear() 
{
	StartWaitCursor();
	SetRedraw(false);
	DeleteAllItems();
	SetRedraw(true);
	StopWaitCursor();
}

void COutputBarList::OnFindmenuOpen() 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	int tab = 0;
	//tab = wnd->m_wndOutputBar.m_flatTabCtrl.GetCurSel();

	if (tab == 0)
		FindInAnalyzerTab();
	else if (tab >= 2 && tab <=3)
		FindInFindTab();
}
