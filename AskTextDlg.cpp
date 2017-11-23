/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// AskTextDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "AskTextDlg.h"

#ifdef UNICODE
	extern CFont appFontFixed;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString appAskText;

/////////////////////////////////////////////////////////////////////////////
// CAskTextDlg dialog


CAskTextDlg::CAskTextDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAskTextDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAskTextDlg)
	m_strMsg = _T("");
	m_strUserText = _T("");
	//}}AFX_DATA_INIT
}


void CAskTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAskTextDlg)
	DDX_Text(pDX, IDC_MSG, m_strMsg);
	DDX_Text(pDX, IDC_USER_TEXT, m_strUserText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAskTextDlg, CDialog)
	//{{AFX_MSG_MAP(CAskTextDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAskTextDlg message handlers

BOOL CAskTextDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CEdit *ed = (CEdit *)GetDlgItem(IDC_USER_TEXT);
	ed->SetFocus();
	if (appAskText != _T(""))
		ed->SetSel(0,appAskText.GetLength());

#ifdef UNICODE
	ed->SetFont(&appFontFixed);
#endif
	
	m_strMsg = appAskText;
	UpdateData(false);

	return false;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

void CAskTextDlg::OnOK() 
{
	UpdateData(true);
	appAskText = m_strUserText;

	CDialog::OnOK();
}

void CAskTextDlg::OnCancel() 
{
	appAskText = _T("");
	
	CDialog::OnCancel();
}
