#include "stdafx.h"
#include "Utils.h"
#include "VisualText.h"
#include "kb.h"
#include "MainFrm.h"
#include "TreeViewAnalyzer.h"
#include "TreeViewGrammar.h"
#include "ListCtrlWMenuLog.h"

extern CVisualTextApp theApp;
extern CString appDirectoryStr;

ListCtrlWMenuLog::ListCtrlWMenuLog(void)
{
}

ListCtrlWMenuLog::~ListCtrlWMenuLog(void)
{
}

BEGIN_MESSAGE_MAP(ListCtrlWMenuLog, CXTPListCtrl)
	ON_NOTIFY_REFLECT(NM_RCLICK, &ListCtrlWMenuLog::OnNMRClick)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_LOGMENU_OPEN, &ListCtrlWMenuLog::OnLogmenuOpen)
	ON_COMMAND(ID_LOGMENU_CLEAR, &ListCtrlWMenuLog::OnLogmenuClear)
	ON_COMMAND(ID_ANALYZELOGMENU_CLEAR, &ListCtrlWMenuLog::OnAnalyzelogmenuClear)
END_MESSAGE_MAP()

void ListCtrlWMenuLog::SetMenuID(int menuID) {
	m_menuID = menuID;
}

void ListCtrlWMenuLog::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
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

void ListCtrlWMenuLog::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	FindInAnalyzerTab();

	CXTPListCtrl::OnLButtonDblClk(nFlags, point);
}

void ListCtrlWMenuLog::FindInAnalyzerTab()
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString fileStr;
	CString dirPathStr;
	CString findStr;
	CString fileOriginStr;
	int intVal1, intVal2;

	DEBUG_TYPE type = (DEBUG_TYPE)GetSelectedFile1(fileStr,findStr,fileOriginStr,intVal1,intVal2);
	if (type != DEBUG_PASS_LINE)
		intVal1--;

	if (type == DEBUG_NOOP) {
		return;
	}
	else if (type == DEBUG_PASS_LINE) {
		// SRP 010606, for Pass File Number in Doc Title Bar
		CString fileNameStr = StripPath(fileStr,0);
		ClipRight(fileNameStr,4);
		int pass = wnd->m_wndAnalyzerView.PassFromFile(fileNameStr);
		//int pass = CTreeViewAnalyzer::Get().PassFromFile(fileNameStr);
		wnd->SelectPass(pass,true);
		wnd->m_wndAnalyzerView.SendMessage(WM_COMMAND,ID_SEQUENCEMENU_EDITRULES);

		CDocument *doc = AfxGetApp()->OpenDocumentFile(fileStr);
		CVisualTextView *view = (CVisualTextView *)GetFirstView(doc);
		CRichEditCtrl &ed = view->GetRichEditCtrl();
		int start = ed.LineIndex(--intVal2);
		view->SetFocus();
		ed.SetSel(start,start);
	}
#if 0 // All find stuff is now in FindListCtrl.cpp // SRP 010606
	else if (type == DEBUG_FIND) {
		if (FileHasExtension(fileStr,"log")) {
			if (fileOriginStr != "") {
				dirPathStr = fileOriginStr;
				if (FileHasExtension(fileOriginStr,"txt"))
					dirPathStr += "_log\\" + fileStr;
			}
			else
				dirPathStr = wnd->GetProcessTextFile(fileStr);
		}
		else if (FileHasExtension(fileStr,"nlp") || FileHasExtension(fileStr, "pat"))
			dirPathStr = RulesPath(fileStr,true);
		else if (FileHasExtension(fileStr,"txt"))
			dirPathStr = InputPath(fileStr);
		else if (FileHasExtension(fileStr,"smp"))
			dirPathStr = GramPath(fileStr);
		else
			return;

		CDocument *doc = AfxGetApp()->OpenDocumentFile(dirPathStr);
		CVisualTextView *view = (CVisualTextView *)GetFirstView(doc);

		CRichEditCtrl &ed = view->GetRichEditCtrl();
		
		char buff[1001];
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
		wnd->m_wndAnalyzerView.ViewPassLog(intVal1);
		wnd->SelectTab(WORKSPACE_ANALYZER);
	}
	else if (type == DEBUG_RULE) {
		//wnd->ViewLogFile(InputPath(fileStr),intVal);
		CString fileNameStr = wnd->m_wndAnalyzerView.CleanPassNameFromPass(intVal1);

		// SRP 010606, for Pass File Number in Doc Title Bar
		int pass = intVal1;
		wnd->SelectPass(pass,true);
		wnd->m_wndAnalyzerView.SendMessage(WM_COMMAND,ID_SEQUENCEMENU_EDITRULES);

		wnd->SelectTab(WORKSPACE_GRAMMAR);
		wnd->m_wndAnalyzerView.SetSelection(fileNameStr);
	}
}

int ListCtrlWMenuLog::GetSelectedFile1(CString &fileStr, CString &findStr,
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

void ListCtrlWMenuLog::OnLogmenuOpen()
{
	FindInAnalyzerTab();
}

void ListCtrlWMenuLog::OnLogmenuClear()
{
	DeleteAllItems();
}

void ListCtrlWMenuLog::OnAnalyzelogmenuClear()
{
	DeleteAllItems();
}
