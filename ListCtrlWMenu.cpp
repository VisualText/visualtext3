#include "stdafx.h"
#include "Utils.h"
#include "VisualText.h"
#include "kb.h"
#include "MainFrm.h"
#include "TreeViewAnalyzer.h"
#include "TreeViewGrammar.h"
#include "SyntaxEditorDoc.h"
#include "SyntaxEditorView.h"
#include "ListCtrlWMenu.h"

extern CVisualTextApp theApp;
extern CString appDirectoryStr;

ListCtrlWMenu::ListCtrlWMenu(void)
{
}

ListCtrlWMenu::~ListCtrlWMenu(void)
{
}
BEGIN_MESSAGE_MAP(ListCtrlWMenu, CXTPListCtrl)
	ON_NOTIFY_REFLECT(NM_RCLICK, &ListCtrlWMenu::OnNMRClick)
	ON_COMMAND(ID_FINDMENU_CLEAR, &ListCtrlWMenu::OnFindmenuClear)
	ON_COMMAND(ID_FINDMENU_OPEN, &ListCtrlWMenu::OnFindmenuOpen)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void ListCtrlWMenu::SetMenuID(int menuID) {
	m_menuID = menuID;
}

void ListCtrlWMenu::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	CPoint point, ptScreen;
	::GetCursorPos(&ptScreen);
	
	point = ptScreen;
	//ScreenToClient(&point);

	PopupContextMenu(this,m_menuID,point);

	*pResult = 0;
}

void ListCtrlWMenu::OnFindmenuClear()
{
	DeleteAllItems();
}

void ListCtrlWMenu::OnFindmenuOpen()
{
	FindInFindTab();
}

void ListCtrlWMenu::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	FindInFindTab();

	CXTPListCtrl::OnLButtonDblClk(nFlags, point);
}

void ListCtrlWMenu::FindInFindTab()
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString fileStr;
	CString dirPathStr;
	CString findStr;
	CString fileOriginStr;
	int intVal,intVal2;

	DEBUG_TYPE type = (DEBUG_TYPE)GetSelectedFile(fileStr,findStr,fileOriginStr,intVal,intVal2);

	if (type == DEBUG_NOOP) {
		return;
	}
	else if (type == DEBUG_FIND) {
		CString lowerFileStr = fileStr;
		CString lowerAppStrLocation = appDirectoryStr;
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

			if (EndsWith(fileStr,_T("nlp")) || EndsWith(fileStr, _T("pat"))) {
				doc = theApp.m_pMainFrame->m_wndAnalyzerView.EditRulesFile(dirPathStr); // SRP 010606, for Pass File Number in Doc Title Bar
				CSyntaxEditorView *view = (CSyntaxEditorView *)GetFirstView(doc);
				CXTPSyntaxEditCtrl &edit = view->GetEditCtrl();
				CString lineStr = edit.GetLineText(intVal);
				intVal2 += CharCount(lineStr, '\t') * edit.GetTabSize();
				edit.Select(intVal,intVal2,intVal,intVal2 + findStr.GetLength());
				edit.EnsureVisibleRow(intVal);

			} else {
				intVal--;
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
	}
	else if (type == DEBUG_ERROR) {
		theApp.m_pMainFrame->m_wndAnalyzerView.ViewPassLog(intVal);
		wnd->SelectTab(WORKSPACE_ANALYZER);
	}
}

int ListCtrlWMenu::GetSelectedFile(CString &fileStr, CString &findStr,
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

