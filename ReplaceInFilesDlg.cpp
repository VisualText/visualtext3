/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// ReplaceInFilesDlg.cpp : implementation file
//

#include "stdafx.h"
#include <io.h>
#include "Utils.h"
#include "kb.h"
#include "MainFrm.h"
#include "VisualTextDoc.h"
#include "VisualTextView.h"
#include "ReplaceInFilesDlg.h"

#ifdef UNICODE
extern CFont appFontFixed;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CVisualTextApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CReplaceInFilesDlg dialog
#define WMU_ENABLEREPLACEBUTTON (WM_USER+1101)

CReplaceInFilesDlg::CReplaceInFilesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReplaceInFilesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReplaceInFilesDlg)
	m_bMatchCase = FALSE;
	m_bMatchWholeWord = FALSE;
	m_bRuleFiles = FALSE;
	m_bTextFiles = FALSE;
	m_strDirectory = _T("");
	m_strFind = _T("");
	m_strReplace = _T("");
	m_DirectoryRecursive = FALSE;
	m_RulesRecursive = FALSE;
	m_TextRecursive = FALSE;
	m_strFileSpec = _T("");
	//}}AFX_DATA_INIT
}


void CReplaceInFilesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReplaceInFilesDlg)
	DDX_Check(pDX, IDC_MATCH_CASE, m_bMatchCase);
	DDX_Check(pDX, IDC_MATCH_WHOLE_WORD, m_bMatchWholeWord);
	DDX_Check(pDX, IDC_RULE_FILES, m_bRuleFiles);
	DDX_Check(pDX, IDC_TEXT_FILES, m_bTextFiles);
	DDX_Text(pDX, IDE_DIRECTORY, m_strDirectory);
	DDX_Text(pDX, IDE_FIND, m_strFind);
	DDX_Text(pDX, IDE_REPLACE, m_strReplace);
	DDX_Check(pDX, IDC_DIRECTORY_RECURSIVE, m_DirectoryRecursive);
	DDX_Check(pDX, IDC_RULES_RECURSIVE, m_RulesRecursive);
	DDX_Check(pDX, IDC_TEXT_RECURSIVE, m_TextRecursive);
	DDX_CBString(pDX, IDC_FILESPEC, m_strFileSpec);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReplaceInFilesDlg, CDialog)
	//{{AFX_MSG_MAP(CReplaceInFilesDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_EN_CHANGE(IDE_FIND, OnChangeFind)
	ON_BN_CLICKED(IDC_RULE_FILES, OnRuleFiles)
	ON_BN_CLICKED(IDC_TEXT_FILES, OnTextFiles)
	ON_EN_CHANGE(IDE_DIRECTORY, OnChangeDirectory)
	ON_CBN_EDITCHANGE(IDC_FILESPEC, OnEditchangeFilespec)
	ON_CBN_SELENDOK(IDC_FILESPEC, OnSelendokFilespec)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WMU_ENABLEREPLACEBUTTON, OnEnableReplaceButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReplaceInFilesDlg message handlers

BOOL CReplaceInFilesDlg::OnInitDialog() 
{
	CVisualTextView* pView = (CVisualTextView*)theApp.GetFirstVisualTextView();
	if (pView) {
		CString strText = pView->GetRichEditCtrl().GetSelText();
		if (!strText.IsEmpty())
			m_strFind = strText;
	}
	CDialog::OnInitDialog();
	EnableReplaceButton();

#ifdef UNICODE
	SetFont(&appFontFixed);
	GetDlgItem(IDE_FIND)->SetFont(&appFontFixed);
	GetDlgItem(IDE_REPLACE)->SetFont(&appFontFixed);
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CReplaceInFilesDlg::OnBrowse() 
{
	CString strDir;
	if (UTLSelectFolder(m_hWnd, _T("Choose directory to perform global replace-in-files operation."), strDir, true)) {
		m_strDirectory = strDir;
		UpdateData(false);
		EnableReplaceButton();
	}
}

void CReplaceInFilesDlg::OnOK() 
{
	UpdateData(true);
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	//wnd->ClearWatch();  deprecated  de Hilster 12/04/29
	int nTotal = 0;
	if (m_bTextFiles)
		nTotal += DoReplaceInFiles(InputPath(), _T("*.txt"), m_TextRecursive);
	if (m_bRuleFiles)
		nTotal += DoReplaceInFiles(RulesPath(), _T("*.pat"), m_RulesRecursive);
	if (!m_strDirectory.IsEmpty() && _taccess(m_strDirectory, 0) == 0 && IsDirectory(m_strDirectory))
		nTotal += DoReplaceInFiles(m_strDirectory, m_strFileSpec, m_DirectoryRecursive);
	CString strCol1;
	strCol1.Format(_T("Search (%s) Replace %d"), m_strFind, nTotal);
	wnd->AddFindLineHead(strCol1, m_strReplace);
	CDialog::OnOK();
}

void CReplaceInFilesDlg::GetLineText(LPCTSTR lpStart, int offset, int& line, CString& strLineText)
{
	int lastLine = 0;
	line = 1;
	for (int n = 0; n < offset && lpStart[n]; n++) {
		if ('\n' == lpStart[n]) {
			line++;
			lastLine = n + 1;
		}
	}
	strLineText = lpStart + lastLine;
	int pos = strLineText.Find('\n');
	if (pos != -1)
		strLineText = strLineText.Left(pos - 1);
}

int CReplaceInFilesDlg::SearchReplaceInFile(LPCTSTR strFilePath)
{
	CString strMsg;
	bool firstFlag = true;
	CVisualTextDoc* docOpen = (CVisualTextDoc*)theApp.IsVisualTextDocumentOpen(CString(_T("VisualText")), CString(strFilePath));
	CVisualTextView* docView = NULL;
	CHARRANGE cr = {0,0};
	int diff = m_strReplace.GetLength() - m_strFind.GetLength();
	int crOffset = 0;
	if (docOpen) {
		docView = (CVisualTextView*)GetFirstView(docOpen);
		docView->GetRichEditCtrl().GetSel(cr);
		if (docOpen->IsModified()) {
			strMsg.Format(
				_T("The open document\n\n\"%s\"\n\nhas been edited, but not saved!")
				_T("\n\nWould you like to save and continue with the Replace operation?"),
				strFilePath);
			if (IDNO == AfxMessageBox(strMsg, MB_YESNO))
				return 0;
			docOpen->OnSaveDocument(docOpen->GetPathName());
		}
	}
	CFile obFile;
	int nTotal = 0;
	if (obFile.Open(strFilePath, CFile::modeReadWrite, NULL)) {
		DWORD fileLen = (DWORD)obFile.GetLength();
		HGLOBAL h;
		LPTSTR lpStart;
		if (h = GlobalAlloc (GHND, fileLen + 1)) {
			if (lpStart = (LPTSTR)GlobalLock (h)) {
				if (obFile.Read (lpStart, fileLen) == fileLen) {
					CString strText = lpStart;
					CString strFindText = m_strFind;
					if (!m_bMatchCase) {
						strText.MakeLower();
						strFindText.MakeLower();
					}
					int lenFind = strFindText.GetLength();
					CDWordArray offsets;
					CDWordArray lengths;
					CByteArray replaces;
					int startPos = 0;
					int matchPos = -1;
					while (-1 != (matchPos = strText.Find(strFindText, startPos)) ) {
						BOOL bMatch = true;
						if (m_bMatchWholeWord) {
							_TCHAR chFront = 0 == matchPos ? ' ' : strText.GetAt(matchPos - 1);
							_TCHAR chEnd = strText.GetAt(matchPos + lenFind);
							if ( _istalnum(chFront) || _istalnum(chEnd) )
								bMatch = false;
						}
						if (bMatch) {
							offsets.Add(startPos);
							lengths.Add(matchPos - startPos);
							replaces.Add(1);
						}
						startPos = matchPos + lenFind;
						if (cr.cpMin > startPos)
							crOffset += diff;
					}
					if (startPos < (int)fileLen) {
						offsets.Add(startPos);
						lengths.Add(fileLen - startPos);
						replaces.Add(0);
					}
					if (replaces.GetAt(0)) {
						CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
						CString strOut1, strOut2, strLineText;
						int line;

						int numOffsets = offsets.GetSize();
						obFile.SeekToBegin();
						obFile.SetLength(0);
						for (int n = 0; n < numOffsets; n++) {

							obFile.Write(lpStart + offsets.GetAt(n), lengths.GetAt(n));
							if (replaces.GetAt(n)) {
								GetLineText(lpStart, offsets.GetAt(n) + lengths.GetAt(n), line, strOut2);
								strOut1.Format(_T("File: %s [%d]:"), strFilePath, line);
								if (firstFlag) {
									wnd->ClearFind();
									firstFlag = false;
								}
								wnd->AddFindLine(strOut1,strOut2);
								nTotal++;

								obFile.Write(m_strReplace, m_strReplace.GetLength());
							}
						}
					}
					offsets.RemoveAll();
					lengths.RemoveAll();
					replaces.RemoveAll();
				}
				GlobalUnlock (h);
			}
			GlobalFree (h);
		}
		obFile.Close();
	}
	if (nTotal > 0 && docOpen) {
		CRichEditCtrl &edit = docView->GetRichEditCtrl();
		int scrollPos = edit.GetFirstVisibleLine();
		CString textStr = NLPReadFile(strFilePath);
		docView->SetWindowText(textStr);
		docView->UpdateWindow();
		if (docOpen->m_DocType == DOC_RULE)
			docView->FormatAll();
		docView->Scroll(scrollPos);
		cr.cpMin += crOffset;
		cr.cpMax += crOffset;
		edit.SetSel(cr);
		edit.SetModify(false);
		docView->RemoveUndoList();
	}
	return nTotal;
}

int CReplaceInFilesDlg::DoReplaceInFiles(LPCTSTR strFolder, LPCTSTR strPattern, BOOL bRecurse)
{
	BOOL bFound;
	CFileFind finder;
	CString strPath, strFilePath;
	int nTotal = 0;

	strPath = strFolder + CString(_T("\\")) + strPattern;
	bFound = finder.FindFile(strPath);
	while (bFound) {
		bFound = finder.FindNextFile();
		if (finder.IsDirectory())
			continue;
		if (!finder.IsReadOnly())
			nTotal += SearchReplaceInFile(finder.GetFilePath());
	}
	finder.Close();

	if (bRecurse) {
		strPath = strFolder + CString(_T("\\*.*"));
		bFound = finder.FindFile(strPath);
		while (bFound) {
			bFound = finder.FindNextFile();
			if (finder.IsDirectory() && !finder.IsDots())
				nTotal += DoReplaceInFiles(finder.GetFilePath(), strPattern, bRecurse);
		}
		finder.Close();
	}
	return nTotal;
}

void CReplaceInFilesDlg::EnableReplaceButton(void)
{
	UpdateData(true);
	BOOL bEnable = false;
	if (!m_strFind.IsEmpty()) {
		bEnable =
			IsDlgButtonChecked(IDC_RULE_FILES) ||
			IsDlgButtonChecked(IDC_TEXT_FILES) ||
			(!m_strDirectory.IsEmpty() && _taccess(m_strDirectory, 0) == 0 && IsDirectory(m_strDirectory) && !m_strFileSpec.IsEmpty());
	}
	GetDlgItem(IDOK)->EnableWindow(bEnable);
}

void CReplaceInFilesDlg::OnChangeFind() 
{
	EnableReplaceButton();
}

void CReplaceInFilesDlg::OnRuleFiles() 
{
	EnableReplaceButton();
}

void CReplaceInFilesDlg::OnTextFiles() 
{
	EnableReplaceButton();
}

void CReplaceInFilesDlg::OnChangeDirectory() 
{
	EnableReplaceButton();
}

void CReplaceInFilesDlg::OnEditchangeFilespec() 
{
	EnableReplaceButton();
}

void CReplaceInFilesDlg::OnSelendokFilespec() 
{
	PostMessage(WMU_ENABLEREPLACEBUTTON);
}

LRESULT CReplaceInFilesDlg::OnEnableReplaceButton(WPARAM wParam, LPARAM lParam)
{
	EnableReplaceButton();
	return 0L;
}

