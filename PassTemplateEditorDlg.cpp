// PassTemplateEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VisualText.h"
#include "PassTemplateEditorDlg.h"
#include "Utils.h"
#include ".\passtemplateeditordlg.h"

extern CString appStrPassTemplateFilePath;
extern CFont appFontFixed;

// CPassTemplateEditorDlg dialog

IMPLEMENT_DYNAMIC(CPassTemplateEditorDlg, CDialog)
CPassTemplateEditorDlg::CPassTemplateEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPassTemplateEditorDlg::IDD, pParent)
	, m_strTemplate(_T(""))
	, m_strPathDir(_T(""))
{
}

CPassTemplateEditorDlg::~CPassTemplateEditorDlg()
{
}

void CPassTemplateEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEMPLATE, m_strTemplate);
	DDX_Text(pDX, IDC_FILE_PATH, m_strPathDir);
}


BEGIN_MESSAGE_MAP(CPassTemplateEditorDlg, CDialog)
	ON_BN_CLICKED(IDC_DATETIME_BUTTTON, OnBnClickedDatetimeButtton)
	ON_BN_CLICKED(IDC_AUTHOR_BUTTON, OnBnClickedAuthorButton)
	ON_BN_CLICKED(IDC_PASSNAME_BUTTON, OnBnClickedPassnameButton)
	ON_BN_CLICKED(IDC_FILE_PATH_DIR, OnBnClickedFilePathDir)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_YEAR_BUTTON, OnBnClickedYearButton)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CPassTemplateEditorDlg message handlers

void CPassTemplateEditorDlg::OnBnClickedDatetimeButtton()
{
	InsertTag(_T("date"));
}

void CPassTemplateEditorDlg::OnBnClickedAuthorButton()
{
	InsertTag(_T("author"));
}

void CPassTemplateEditorDlg::OnBnClickedPassnameButton()
{
	InsertTag(_T("passname"));
}

void CPassTemplateEditorDlg::OnBnClickedYearButton()
{
	InsertTag(_T("year"));
}

void CPassTemplateEditorDlg::OnBnClickedFilePathDir()
{
}

void CPassTemplateEditorDlg::OnBnClickedOk()
{
	Close();
	OnOK();
}

BOOL CPassTemplateEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CEdit *ed = (CEdit *)GetDlgItem(IDC_TEMPLATE);
	ed->SetFont(&appFontFixed);

	CString dirPathStr = _tgetenv(_T("VISUALTEXT"));
	m_strPathDir = theApp.GetProfileString(_T("PREFERENCES"),_T("PASSTEMPLATEDIRECTORY"),dirPathStr);
	if (m_strPathDir == dirPathStr)
		m_strPathDir += PASS_TEMPLATE_FILENAME;
	appStrPassTemplateFilePath = m_strPathDir;

	m_strTemplate = NLPReadFile(m_strPathDir);
	if (m_strTemplate.IsEmpty())
		m_strTemplate = DefaultFileContent();
	else
		m_strTemplate.Replace(_T("\n"),_T("\r\n"));

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPassTemplateEditorDlg::OnClose()
{
	Close();
	CDialog::OnClose();
}

CString CPassTemplateEditorDlg::DefaultFileContent(void)
{
	CString contentStr;
	contentStr.Empty();

	contentStr = _T("\
###############################################\r\n\
# FILE: <passname>\r\n\
# SUBJ:\r\n\
# AUTH: <author>\r\n\
# CREATED: <created>\r\n\
# Copyright: <year>\r\n\
###############################################\r\n\
\r\n\
@NODES _ROOT\r\n\
\r\n\
@RULES\r\n\
_xNIL <-\r\n\
	_xEND	# 1\r\n\
	@@");

	return contentStr;
}

void CPassTemplateEditorDlg::Close(void)
{
	UpdateData(true);

	NLPWriteFile(m_strPathDir,m_strTemplate);
	appStrPassTemplateFilePath = m_strPathDir;
	theApp.WriteProfileString(_T("PREFERENCES"),_T("PASSTEMPLATEDIRECTORY"),m_strPathDir);
}

void CPassTemplateEditorDlg::InsertTag(CString tagStr)
{
	CEdit *ed = (CEdit *)GetDlgItem(IDC_TEMPLATE);
	CString str;
	str.Format(_T("<%s>"),tagStr);
	ed->ReplaceSel(str,true);
	ed->SetFocus();
}