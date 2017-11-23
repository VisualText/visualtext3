/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// FindDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Utils.h"
#include "kb.h"
#include "MainFrm.h"
#include "FindDlg.h"

BOOL g_bFind2 = false;

extern CString appNameStr;
extern CString appFindStateStr;
extern CString appStrAppsDirectory;
extern CVisualTextApp theApp;
extern CString appStrFind;
extern CDocument *appDocFocus;
extern CFont appFontFixed;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindDlg dialog


CFindDlg::CFindDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindDlg)
	m_strFind = _T("");
	m_boolMatchCase = false;
	m_strRuleDir = _T("");
	m_boolLogFiles = false;
	m_boolRuleFiles = false;
	m_boolConcept = false;
	m_boolTextFiles = false;
	m_boolAppend = false;
	m_boolOutputFiles = false;
	//}}AFX_DATA_INIT
}


void CFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindDlg)
	DDX_Control(pDX, IDC_OTHER_RULE_FILES, m_listCtrlRuleDir);
	DDX_Text(pDX, IDC_FIND_STR, m_strFind);
	DDX_Check(pDX, IDC_MATCH_CASE, m_boolMatchCase);
	DDX_Check(pDX, IDC_LOG_FILES, m_boolLogFiles);
	DDX_Check(pDX, IDC_RULE_FILES, m_boolRuleFiles);
	DDX_Check(pDX, IDC_CONCEPTS, m_boolConcept);
	DDX_Check(pDX, IDC_TEXT_FILES, m_boolTextFiles);
	DDX_Check(pDX, IDC_APPEND, m_boolAppend);
	DDX_Check(pDX, IDC_OUTPUT_FILES, m_boolOutputFiles);
	DDX_Check(pDX, IDC_FIND2, m_boolFind2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindDlg, CDialog)
	//{{AFX_MSG_MAP(CFindDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CFindDlg::OnOK)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindDlg message handlers

void CFindDlg::SetData(CString findStr, CVisualTextDoc *doc)
{
	m_doc = doc;
	m_strFind = findStr;
}

void CFindDlg::OnOK() 
{
	UpdateData(true);

	g_bFind2 = m_boolFind2;

	appStrFind = m_strFind;

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();

	if (g_bFind2)
		wnd->ClearFind2();
	else
		wnd->ClearFind();

	bool noCase = m_boolMatchCase ? false : true;
	int total = 0;

	//TrimString(m_strFind);

	if (m_strFind != _T("")) {
		StartWaitCursor();

		if (!m_boolAppend) {
			wnd->ClearFind();
		}

		if (m_boolTextFiles)
			total += wnd->FindWordInTextFiles(m_strFind,noCase);

		if (m_boolConcept)
			total += wnd->FindWordInGrammar(m_strFind,noCase);

		if (m_boolOutputFiles)
			total += wnd->FindWordInOutputFiles(m_strFind,_T(".txt"),noCase);

		if (m_boolLogFiles)
			total += wnd->FindWordInOutputFiles(m_strFind,_T(".log"),noCase);

		if (m_boolRuleFiles)
			total += wnd->FindWordInRuleFiles(m_strFind,_T(""),noCase);

		CString otherRuleFilesStr = GetOtherRulesStr();

		if (otherRuleFilesStr != _T(""))
			total += wnd->FindWordInOtherRuleFiles(m_strFind,otherRuleFilesStr,noCase);

		StopWaitCursor();
	}

	CString msgStr;
	msgStr.Format(_T("Found %d:"),total);

	if (g_bFind2)
		wnd->AddFindLineHead2(msgStr, m_strFind, true);
	else
		wnd->AddFindLineHead(msgStr, m_strFind, true);

	appFindStateStr = GetStateStr();
	
	CDialog::OnOK();
}

CString CFindDlg::GetStateStr() 
{
	CString stateStr;

	stateStr.Format(_T("%d:%d:%d:%d:%d:%d:%d:%d"),m_boolMatchCase,m_boolAppend,m_boolTextFiles,
		m_boolConcept,m_boolOutputFiles,m_boolLogFiles,m_boolRuleFiles,m_boolFind2);

	return stateStr;
}

void CFindDlg::StateFromStr() 
{
	CString stateStr;
	int intValue;

	NthPathSegmentInt(appFindStateStr,intValue,':',0);
	m_boolMatchCase = intValue;

	NthPathSegmentInt(appFindStateStr,intValue,':',1);
	m_boolAppend = intValue;

	NthPathSegmentInt(appFindStateStr,intValue,':',2);
	m_boolTextFiles = intValue;

	NthPathSegmentInt(appFindStateStr,intValue,':',3);
	m_boolConcept = intValue;

	NthPathSegmentInt(appFindStateStr,intValue,':',4);
	m_boolOutputFiles = intValue;

	NthPathSegmentInt(appFindStateStr,intValue,':',5);
	m_boolLogFiles = intValue;

	NthPathSegmentInt(appFindStateStr,intValue,':',6);
	m_boolRuleFiles = intValue;

	NthPathSegmentInt(appFindStateStr,intValue,':',7);
	m_boolFind2 = intValue;
}

CString CFindDlg::GetOtherRulesStr() 
{
	CString dirStr = _T("");
	POSITION pos = m_listCtrlRuleDir.GetFirstSelectedItemPosition();

	if (pos == NULL)
		return _T("");

	CString itemStr;
	int nItem;

	while ((nItem = m_listCtrlRuleDir.GetNextSelectedItem(pos)) >= 0) {
		itemStr = m_listCtrlRuleDir.GetItemText(nItem,0);
		if (dirStr != _T(""))
			dirStr += _T(",");
		dirStr += itemStr;
	}
	return dirStr;

}

BOOL CFindDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_strFind = appStrFind;
	m_boolFind2 = false;

	CEdit *edit = (CEdit *)GetDlgItem(IDC_FIND_STR);
	edit->SetFont(&appFontFixed);

	// IF NO STRING ALREADY IN FINDSTR, GET SELECTION IF THERE IS ONE
// Commenting this out till someone can fix it.	// 09/06/02 AM.
#ifdef _CRASH_020906_
	if (appDocFocus && appStrFind == _T("")) {
		// SRP 010505
		// AppDocFocus could be BrowserView or KnowledgeView
		if (appDocFocus->IsKindOf(RUNTIME_CLASS(CVisualTextDoc)) ) {
			CVisualTextView *view = (CVisualTextView *)GetFirstView(appDocFocus);
			if (view) {
				CString selStr;
				if (view->GetSelTextTrim(true,false))
					m_strFind = appStrFind;
			}
		}
	}
#endif

	FillDirectoryCombo();
	StateFromStr();

	UpdateData(false);
	
	CEdit *ed = (CEdit *)GetDlgItem(IDC_FIND_STR);
	ed->SetFocus();
	ed->SetSel(0,m_strFind.GetLength());

	return false;  // return true unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return false
}

void CFindDlg::FillDirectoryCombo() 
{
	CFileFind finder;
	// SRP 010417 Use APPS environment variable
	CString rulesPathStr = appStrAppsDirectory;
	BOOL bWorking = finder.FindFile(rulesPathStr+_T("\\*.*"));
	CString fileName;

	int i = 0;
	while (bWorking) {
		bWorking = finder.FindNextFile();
		fileName = finder.GetFileName();
		if (fileName.Find(_T("."),0) == 0)
			continue;
		if (finder.IsDirectory())
			m_listCtrlRuleDir.InsertItem(i++,fileName);
	}

	finder.Close();
}
