// MDITextEditorDoc.cpp : implementation of the CSyntaxEditorDoc class
//

#include "stdafx.h"

#include "kb.h"
#include "Utils.h"
#include "VisualText.h"
#include "MainFrm.h"
#include "SyntaxEditorDoc.h"
#include "Resource.h"

extern CVisualTextApp theApp;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE 
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSyntaxEditorDoc

IMPLEMENT_DYNCREATE(CSyntaxEditorDoc, CDocBase)

BEGIN_MESSAGE_MAP(CSyntaxEditorDoc, CDocBase)
	//{{AFX_MSG_MAP(CSyntaxEditorDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyntaxEditorDoc construction/destruction

CSyntaxEditorDoc::CSyntaxEditorDoc()
{
	// TODO: add one-time construction code here

}

CSyntaxEditorDoc::~CSyntaxEditorDoc()
{
}

BOOL CSyntaxEditorDoc::OnNewDocument()
{
	if (!CDocBase::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	m_concept = NULL;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSyntaxEditorDoc serialization

void CSyntaxEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}

	CDocBase::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CSyntaxEditorDoc diagnostics

#ifdef _DEBUG
void CSyntaxEditorDoc::AssertValid() const
{
	CDocBase::AssertValid();
}

void CSyntaxEditorDoc::Dump(CDumpContext& dc) const
{
	CDocBase::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSyntaxEditorDoc commands

BOOL CSyntaxEditorDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocBase::OnOpenDocument(lpszPathName))
		return FALSE;
	
	// TODO: Add your specialized creation code here
	
	return TRUE;
}

BOOL CSyntaxEditorDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	theApp.SetAnalyzerRuleFileModified(lpszPathName);

	return CDocBase::OnSaveDocument(lpszPathName);
}

void CSyntaxEditorDoc::OnCloseDocument() 
{
	// TODO: Add your specialized code here and/or call the base class
	theApp.m_pMainFrame->SetRowCol(0,0);
	
	CDocBase::OnCloseDocument();
}

void CSyntaxEditorDoc::OnChangedViewList() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDocBase::OnChangedViewList();
}

bool CSyntaxEditorDoc::IsCurrentPassFile() const {
	return  m_boolCurrentAnalyzerPassFile;
}

//PD 02/14/2001
void CSyntaxEditorDoc::SetPathName( LPCTSTR lpszPathName, BOOL bAddToMRU) {
	CDocBase::SetPathName(lpszPathName,bAddToMRU) ;
}

void CSyntaxEditorDoc::SetTitle(LPCTSTR lpszTitle) 
{
	//we don't change read only files
	//if (m_bReadOnly)
	//	return;

	CString pathNameStr = GetPathName();
	CString inputPathStr = RulesPath();
	m_boolCurrentAnalyzerPassFile = pathNameStr.Find(inputPathStr,0) == 0 ? true : false;

	CString titleStr = lpszTitle;
	if (EndsWith(titleStr,_T(".pat")))
		ClipRight(titleStr,4);

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	if (1 == 1)
	//if (!m_concept && m_boolCurrentAnalyzerPassFile)
		m_concept = theApp.m_pMainFrame->m_wndAnalyzerView.ConceptFromName(titleStr);
	if (m_concept)
		titleStr = ConceptName(m_concept);

	if (!pathNameStr.IsEmpty() && pathNameStr.Find(AppPath()) < 0)
		titleStr = pathNameStr;

	_TCHAR lastChar;
	int len = titleStr.GetLength();

	if (len > 0)
		lastChar = titleStr.GetAt(len-1);
	else lastChar = '\0';	
	_TCHAR thirdFromLastChar = '\0';
	if (len >= 3)
		thirdFromLastChar = titleStr.GetAt(len-3);
	else thirdFromLastChar = '\0';

	CString workStr;

	if (lastChar && lastChar != '*' && lastChar != ')' &&
		(len < 3 || (thirdFromLastChar && thirdFromLastChar != ')'))) {

		if (titleStr.CompareNoCase(MAINSTR) && m_boolCurrentAnalyzerPassFile) {
			int pass = KBPassNumber(m_concept);
			if (pass > 0) {
				workStr.Format(_T(" (pass %d)"),pass);
				titleStr += workStr;
			}
		}
	}

	bool updated = false;
	if (lastChar && lastChar != '*' && IsModified()) {
		//titleStr += _T(" *");
		updated = true;
	}
	else if (lastChar && !IsModified()) {
		if (lastChar == '*')
			ClipRight(titleStr,2);
		updated = true;
	}

	if (titleStr != _T(""))
		CDocBase::SetTitle(titleStr);
	else
		CDocBase::SetTitle(lpszTitle);

	if (updated)
		wnd->OnUpdateFrameTitle(false);
}

