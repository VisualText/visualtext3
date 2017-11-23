/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// NewAnalyzerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "VisualText.h"
#include "NewAnalyzerDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString appNameStr;
extern CString appDirectoryStr;
extern CString appAnaFilePathStr;
extern CString appStrAppsDirectory;
extern CString appAskText;

/////////////////////////////////////////////////////////////////////////////
// CNewAnalyzerDialog dialog


CNewAnalyzerDialog::CNewAnalyzerDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNewAnalyzerDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewAnalyzerDialog)
	m_strAnalyzerName = _T("");
	m_strAnalyzerLocation = _T("");
	//}}AFX_DATA_INIT
}


void CNewAnalyzerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewAnalyzerDialog)
	DDX_Control(pDX, IDC_TEMPLATE_LIST, m_listCtrlType);
	DDX_Text(pDX, IDC_ANALYZER_NAME, m_strAnalyzerName);
	DDX_Text(pDX, IDC_LOCATION, m_strAnalyzerLocation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewAnalyzerDialog, CDialog)
	//{{AFX_MSG_MAP(CNewAnalyzerDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewAnalyzerDialog message handlers

void CNewAnalyzerDialog::OnOK() 
{
	// TODO: Add extra validation here

	UpdateData(true);

	CString errorStr = _T("");
	CString msgStr;

	// GET SELECTED TEMPLATE - IF NOT SELECTED, STORE "" IN appAskText
	POSITION pos = m_listCtrlType.GetFirstSelectedItemPosition();	
	if (pos != NULL) {
		int nItem = m_listCtrlType.GetNextSelectedItem(pos);
		CString selectedTemplateStr = m_listCtrlType.GetItemText(nItem,0);
		appAskText = TemplateDirectory(selectedTemplateStr);
	}
	else {
		//appAskText = AppsDirectory("Bare"); // SRP 010319
		appAskText = _T("");
	}

	if (m_strAnalyzerName == _T(""))
		errorStr += _T("The analyzer needs a name.\n");

	if (m_strAnalyzerLocation == _T(""))
		errorStr += _T("The analyzer needs a location.\n");

	//if (m_strAnalyzerName != "" && m_strAnalyzerLocation != "" && FileExists(appAnaFilePathStr))
		//errorStr += "That analyzer already exists!\n";
	
	if (errorStr == _T("")) {
		CString dirPathStr = m_strAnalyzerLocation + _T("\\") + m_strAnalyzerName;

		if (m_strAnalyzerName == appNameStr) {
			msgStr = _T("Error(s):\n\nBecause you have used the same name as the current analyzer, you must close the current analyzer in order to overwrite it.");
			AfxMessageBox(msgStr);
			return;
		}

		if (IsDirectory(dirPathStr)) {
			msgStr.Format(_T("Analyzer already exists: \"%s\"\nDo you want to overwrite it?"),
				dirPathStr);
			if(AfxMessageBox(msgStr,MB_YESNOCANCEL) != IDYES)
				return;
			if (!RecursiveRemove(dirPathStr,true))
				return;
		}

		appNameStr = m_strAnalyzerName;
		appDirectoryStr = m_strAnalyzerLocation;
		appAnaFilePathStr = AnalyzerFile();
		
		CDialog::OnOK();
	}
	else {
		msgStr.Format(_T("Error(s):\n\n%s"),errorStr);
		AfxMessageBox(msgStr);
	}
}

BOOL CNewAnalyzerDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// SET FOCUS ON ANALYZER NAME
	CEdit *ed = (CEdit *)GetDlgItem(IDC_ANALYZER_NAME);
	ed->SetFocus();

	m_listCtrlType.InsertColumn(0,_T("Type"),LVCFMT_LEFT,150);

	if (m_boolEnableTemplates)
		LoadAnalyzerTypes();
	else
		GetDlgItem(IDC_TEMPLATE_LIST)->EnableWindow(m_boolEnableTemplates);

	m_strAnalyzerLocation = appStrAppsDirectory;

	UpdateData(false);

	return false; // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

void CNewAnalyzerDialog::LoadAnalyzerTypes()
{
	CFileFind finder;
	CString pathStr = TemplateDirectory(_T("*.*"));
	BOOL bWorking = finder.FindFile(pathStr);
	CString fileNameStr;
	int count = 0;
	int first = 0;

	while (bWorking) {
		bWorking = finder.FindNextFile();
		fileNameStr = finder.GetFileName();

		// SELECT BARE AS DEFAULT
		if (fileNameStr == _T("Bare"))
			first = count;

		if (fileNameStr.GetAt(0) != '.' && finder.IsDirectory())
			m_listCtrlType.InsertItem(count++,fileNameStr);
	}
	// SRP 010319
	// Since List Control sorts the items,
	// need to search for "Bare" to highlight as default template
	int nCount = m_listCtrlType.GetItemCount();
	for (int nItem = 0; nItem < nCount; nItem++) {
		CString strText = m_listCtrlType.GetItemText(nItem, 0);
		if (strText == _T("Bare")) {
			first = nItem;
			break;
		}
	}

	m_listCtrlType.SetItemState(0,LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	
}


void CNewAnalyzerDialog::EnableTemplates(bool enable)
{
	m_boolEnableTemplates = enable;
}
