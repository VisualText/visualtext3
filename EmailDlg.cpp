/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// EmailDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "MainFrm.h"
#include "EmailDlg.h"

extern CVisualTextApp theApp;
extern CFont appFontFixed;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEmailDlg dialog


CEmailDlg::CEmailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEmailDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEmailDlg)
	m_strBody = _T("");
	m_strSubject = _T("");
	m_strTo = _T("");
	m_strEmailList = _T("");
	m_strStatus = _T("");
	//}}AFX_DATA_INIT
}


void CEmailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEmailDlg)
	DDX_Text(pDX, IDC_BODY, m_strBody);
	DDX_Text(pDX, IDC_SUBJECT, m_strSubject);
	DDX_Text(pDX, IDC_TO, m_strTo);
	DDX_Text(pDX, IDC_EMAIL_LIST, m_strEmailList);
	DDX_Text(pDX, IDC_STATUS, m_strStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEmailDlg, CDialog)
	//{{AFX_MSG_MAP(CEmailDlg)
	ON_BN_CLICKED(IDC_EMAIL_LIST_CHOOSE, OnEmailListChoose)
	ON_BN_CLICKED(IDC_PREVIOUS_EMAIL, OnPreviousEmail)
	ON_BN_CLICKED(IDC_NEXT_EMAIL, OnNextEmail)
	ON_BN_CLICKED(IDC_SEND_CURRENT, OnSendCurrent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEmailDlg message handlers

void CEmailDlg::SetData(CString strTo, CString strSubject, CString strBody)
{
	m_strTo = strTo;
	m_strSubject = strSubject;
	m_strBody = strBody;
}

void CEmailDlg::SetFileData(CString filePathStr)
{
	CStringList lines;
	ReadFileToStringList(lines,filePathStr);

	POSITION pos = lines.GetHeadPosition();
	CString lineStr;
	m_strTo = _T("");
	m_strSubject = _T("");
	m_strBody = _T("");

	int line = 0;

	while (pos && (lineStr = lines.GetNext(pos))) {
		line++;
		if (line < 3 && lineStr.Find(_T("to:"),0) == 0) {
			m_strTo = lineStr;
			ClipLeft(m_strTo,3);
		}
		else if (line < 3 && lineStr.Find(_T("subject:"),0) == 0) {
			m_strSubject = lineStr;
			ClipLeft(m_strSubject,8);
		}
		else
			m_strBody += lineStr + _T("\r\n");
	}
}

void CEmailDlg::OnOK() 
{
	UpdateData(true);

	if (m_strTo != _T("") && m_strSubject != _T("") && m_strBody != _T("")) {
		if (SendEmail())
			CDialog::OnOK();
	}
	else
		AfxMessageBox(_T("All fields must be filled in to send email!"));
}

bool CEmailDlg::SendEmail() 
{
	if (m_strEmailList != _T(""))
		return SendToEmailList();
	else
		return SendOneEmail();
}

bool CEmailDlg::SendToEmailList() 
{
	if (FileExists(m_strEmailList)) {
		CStringList emails;
		ReadFileToStringList(emails,m_strEmailList);

		POSITION pos = emails.GetHeadPosition();
		m_intCurrentEmail = 0;
		int total = emails.GetCount();

		// DETERMINE INSERT POINT FOR ADDING
		while (pos && (m_strTo = emails.GetNext(pos))) {
			CurrentEmail();
			SendOneEmail();
		}
	}
	else {
		AfxMessageBox(_T("Email list file does not exist.  Can't send email"));
		return false;
	}

	return true;
}

bool CEmailDlg::SendOneEmail() 
{
	return theApp.SendOneMessage(m_strTo,m_strSubject,m_strBody);
}

BOOL CEmailDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CEdit *ed = (CEdit *)GetDlgItem(IDC_BODY);
	ed->SetFont(&appFontFixed);

	// SET FOCUS ON ANALYZER NAME
	ed = (CEdit *)GetDlgItem(IDC_TO);
	ed->SetFocus();
	ed->SetSel(0,-1);

	m_intCurrentEmail = 0;
	
	return false;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEmailDlg::OnEmailListChoose() 
{
	UpdateData(true);

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString outputFilePathStr = InputPath(wnd->GetProcessText()) + _T("_log\\*.txt");
	CString filterStr = _T("text files (*.txt)|*.txt|All file (*.*)|*.*||");
	CFileDialog fileDlg(true,_T("txt"),outputFilePathStr,NULL,filterStr);

	if (fileDlg.DoModal() == IDOK) {

		m_strEmailList = fileDlg.GetPathName();
		ReadFileToStringList(m_strListEmails,m_strEmailList);
		m_intEmailListCount = m_strListEmails.GetCount();
		m_intCurrentEmail = 0;
		CurrentEmail();
	}
}

void CEmailDlg::OnPreviousEmail() 
{
	if (m_intCurrentEmail > 0) {
		UpdateData(true);
		m_intCurrentEmail--;
		CurrentEmail();
	}
}

void CEmailDlg::OnNextEmail() 
{
	if (m_intCurrentEmail < m_intEmailListCount-1) {
		UpdateData(true);
		m_intCurrentEmail++;
		CurrentEmail();
	}
}

void CEmailDlg::CurrentEmail()
{
	CString valueStr = NthStringListItem(m_strListEmails,m_intCurrentEmail);
	NthPathSegment(valueStr,m_strTo,'\t',0);
	NthPathSegment(valueStr,m_strSubject,'\t',1);
	NthPathSegment(valueStr,m_strBody,'\t',2);
	m_strBody.Replace(_T("<P>"),_T("\r\n\r\n"));
	m_strBody.Replace(_T("<BR>"),_T("\r\n"));
	m_strStatus.Format(_T("(%d of %d)"),m_intCurrentEmail+1,m_intEmailListCount);
	UpdateData(false);
}

void CEmailDlg::OnSendCurrent() 
{
	UpdateData(true);
	StartWaitCursor();
	SendOneEmail();
	StopWaitCursor();
}
