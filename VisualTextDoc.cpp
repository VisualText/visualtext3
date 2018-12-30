/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// VisualTextDoc.cpp : implementation of the CVisualTextDoc class
//

#include "stdafx.h"
#include "Utils.h"
#include "kb.h"
#include "MainFrm.h"
#include "TreeViewAnalyzer.h"
#include "io.h"

#include "VisualTextDoc.h"
#include "CntrItem.h"

extern bool appBoolOpeningTextFile;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CVisualTextApp theApp;
extern CString appDirectoryStr;
extern CG *cg;

/////////////////////////////////////////////////////////////////////////////
// CVisualTextDoc

IMPLEMENT_DYNCREATE(CVisualTextDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CVisualTextDoc, CRichEditDoc)
	//{{AFX_MSG_MAP(CVisualTextDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, CRichEditDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, CRichEditDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, CRichEditDoc::OnUpdateObjectVerbMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVisualTextDoc construction/destruction

CVisualTextDoc::CVisualTextDoc()
{
	m_bPromptSaveModified = true ; //SRP 2001-02-01
	m_bReadOnly = false;
	m_concept = NULL;
	m_DocType = DOC_NONE; // SRP 010510
}

CVisualTextDoc::~CVisualTextDoc()
{
}

BOOL CVisualTextDoc::OnNewDocument()
{
	if (!CRichEditDoc::OnNewDocument())
		return false;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return true;
}

//PD 02/14/2001
void CVisualTextDoc::SetPathName( LPCTSTR lpszPathName, BOOL bAddToMRU) {
	CRichEditDoc::SetPathName(lpszPathName,bAddToMRU) ;
}

 
CRichEditCntrItem* CVisualTextDoc::CreateClientItem(REOBJECT* preo) const
{
	// cast away constness of this
	return new CVisualTextCntrItem(preo, (CVisualTextDoc*) this);
}


/////////////////////////////////////////////////////////////////////////////
// CVisualTextDoc serialization

void CVisualTextDoc::Serialize(CArchive& ar)
{
	m_DocType = DocumentTypeFromFile(ar.m_strFileName);

	CRichEditDoc::m_bRTF = false;

	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		return;
		// TODO: add loading code here
	}

	// Calling the base class CDocBase enables serialization
	//  of the container document's COleClientItem objects.
	// TODO: set CDocBase::m_bRTF = false if you are serializing as text
	CRichEditDoc::Serialize(ar);
}


/////////////////////////////////////////////////////////////////////////////
// CVisualTextDoc diagnostics

#ifdef _DEBUG
void CVisualTextDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CVisualTextDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVisualTextDoc commands


//input parameters as const, input objects as const & PD 02/14/2001
DOC_TYPE CVisualTextDoc::DocumentTypeFromFile(const CString &path)
{
	m_DocType = DOC_FILE;

	if (!appBoolOpeningTextFile) {
		if (path.Find(InputPath(),0) == 0) {
			m_DocType = DOC_FILE;
		}
		else if (IsPathSampleFile(path)) {
			m_DocType = DOC_SAMPLE;
		}
		else if (EndsWith(path,_T("htm")) || EndsWith(path,_T("html"))) {
			m_DocType = DOC_HTML;
		}
		else if (EndsWith(path,_T("seq"))) {
			m_DocType = DOC_SEQUENCE;
		}
		else if (EndsWith(path,_T("pat")) || EndsWith(path, _T("nlp"))) {
			m_DocType = DOC_RULE;
		}
		else if (EndsWith(path,_T("rec"))) {
			m_DocType = DOC_REC;
		}
		else if (path.Find(InputPath()) == 0) {
			m_DocType = DOC_FILE;
		}
	}

	if (EndsWith(path,_T(".log"))) {
		m_DocType = DOC_SYS_LOG;
		// LOG FILES MUST BE SPECIFIED EXPLICITLY HERE.	// 06/29/03 AM.
		if (EndsWith(path,_T("final.log"))
		 || EndsWith(path,_T("err.log"))	// 06/29/03 AM.
		 || EndsWith(path,_T("dbg.log"))	// 06/29/03 AM.
		 || EndsWith(path,_T("init.log"))	// 06/29/03 AM.
		 || path.Find(_T("\\ana"),0) > 0 || path.Find(_T("ana"),0) == 0)
			m_DocType = DOC_LOG;
	}

	return m_DocType;
}

void CVisualTextDoc::SetTitle(LPCTSTR lpszTitle) 
{
	//we don't change read only files
	if (m_bReadOnly)
		return;

	CString pathNameStr = GetPathName();
	CString inputPathStr = RulesPath();

	CString titleStr = lpszTitle;

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	if (!pathNameStr.IsEmpty() && pathNameStr.Find(AppPath()) < 0)
		titleStr = pathNameStr;

	// IF TYPE NOT SET, THEN SET IT!
	if (m_DocType <= DOC_NONE  || m_DocType > DOC_DICTIONARY) // SRP 010510
		DocumentTypeFromFile(pathNameStr);

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

		if (m_DocType == DOC_SAMPLE) {
			CONCEPT *sampleConcept = wnd->GrammarSampleConcept(GetPathName());
			CString conceptStr = ConceptName(sampleConcept);
			CString titStr = titleStr;
			if (titStr.CompareNoCase(conceptStr))
				titleStr += _T(" (") + conceptStr + _T(")");
		}
		else {
			titleStr = StripPath(RelativizePath(pathNameStr),0)
					   + _T(" (")
					   +  StripPathReverse(StripPath(RelativizePath(pathNameStr),0,true),0,true) 
					   + _T(")");
			titleStr.Replace(_T("_log"),_T(""));
		}
	}

	bool updated = false;
	if (lastChar && lastChar != '*' && IsModified()) {
		titleStr += _T(" *");
		updated = true;
	}
	else if (lastChar && !IsModified()) {
		if (lastChar == '*')
			ClipRight(titleStr,2);
		updated = true;
	}

	if (titleStr != _T(""))
		CRichEditDoc::SetTitle(titleStr);
	else
		CRichEditDoc::SetTitle(lpszTitle);

	if (updated)
		wnd->OnUpdateFrameTitle(false);
}

LPCTSTR CVisualTextDoc::GetTitle() const {
	return CRichEditDoc::GetTitle();
}

BOOL CVisualTextDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
 	//Check read only status
	if (_taccess(lpszPathName,0)!= -1 && _taccess(lpszPathName,2) == -1) {
		CString msgStr;
		msgStr.Format(_T("%s is read only."),lpszPathName);
		AfxMessageBox(msgStr);
		return false;
	}
	CVisualTextView *view = (CVisualTextView *)GetFirstView(this);
	if (view->IsSampleFile()) {
		CString msgStr;
		msgStr.Format(_T("The file \"%s\" has samples in the gram tab.\nEditing a file can invalidate existing and future samples.\nDo you still want to save it?"),lpszPathName);
		if (AfxMessageBox(msgStr,MB_YESNO) != IDYES)
			return false;
	}
	theApp.SetAnalyzerRuleFileModified(lpszPathName);


#ifdef UNICODE
	CString contentStr = view->EditGetText();
	int returnVal = NLPWriteFile(lpszPathName, contentStr);
	SetModifiedFlag(0);
#else
	int returnVal = CRichEditDoc::OnSaveDocument(lpszPathName);
#endif
	SetTitle(GetTitle());
	return returnVal;
}


void CVisualTextDoc::Update() 
{
	CVisualTextView *docView = (CVisualTextView *)GetFirstView(this);

	//PD 02/22/2001
	// crash protection
	if (!docView)
		return;

	int scrollPos = docView->GetRichEditCtrl().GetFirstVisibleLine();
	//CString textStr = NLPReadFile(GetPathName(),true);
	const CString pathNameStr = GetPathName();
	CString textStr = NLPReadInputFile(pathNameStr);	// 08/20/08 AM.
	docView->SetWindowText(textStr);
	docView->UpdateWindow();
	docView->Scroll(scrollPos);
	if (m_DocType == DOC_RULE)
		docView->FormatAll();
	docView->GetRichEditCtrl().SetModify(false);
}

void CVisualTextDoc::ChangeType(DOC_TYPE docType) 
{
	m_DocType = docType;
	CString titleStr = StripPath(GetPathName(),0);
	ClipRight(titleStr,4);  // TRIM OFF .TXT
	SetTitle(titleStr);
	CVisualTextView *docView = (CVisualTextView *)GetFirstView(this);

	//PD 02/22/2001
	// crash protection
	if (!docView)
		return;

	docView->SetIconType();
}

// SRP 2001-02-01
void CVisualTextDoc::OnCloseDocument() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_bPromptSaveModified && GetView() && IsModified() && !m_bReadOnly)
		SaveModified();
	
	CRichEditDoc::OnCloseDocument();
}
BOOL CVisualTextDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	BOOL bClose = CRichEditDoc::CanCloseFrame(pFrame);
	if (bClose)
		m_bPromptSaveModified = false;
	return bClose;
}

void CVisualTextDoc::SetReadOnly(bool bSetting) {
	m_bReadOnly = bSetting;
}

bool CVisualTextDoc::GetReadOnly() const {
	return  m_bReadOnly;
}

void CVisualTextDoc::UpdateDocviewContent()
{
	CVisualTextView *docView = (CVisualTextView *)GetFirstView(this);
	long scrollPos = docView->GetRichEditCtrl().GetFirstVisibleLine();

	CString textStr = NLPReadFile(GetPathName());
	docView->SetWindowText(textStr);
	docView->UpdateWindow();

	docView->Scroll(scrollPos);
	if (m_DocType == DOC_RULE)
		docView->FormatAll();

	docView->GetRichEditCtrl().SetModify(false);
}
