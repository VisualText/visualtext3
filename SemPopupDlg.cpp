/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// SemPopupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "ParseTreeItemInfo.h"
#include "MainFrm.h"
#include "SemPopupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSemPopupDlg dialog


CSemPopupDlg::CSemPopupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSemPopupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSemPopupDlg)
	m_strText = _T("");
	//}}AFX_DATA_INIT
}


void CSemPopupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSemPopupDlg)
	DDX_Text(pDX, IDE_TEXT, m_strText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSemPopupDlg, CDialog)
	//{{AFX_MSG_MAP(CSemPopupDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSemPopupDlg message handlers

BOOL CSemPopupDlg::OnInitDialog() 
{
	m_cR.top -= GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYDLGFRAME);
	m_cR.bottom += GetSystemMetrics(SM_CXHSCROLL) + GetSystemMetrics(SM_CYDLGFRAME);
	m_cR.left -= GetSystemMetrics(SM_CXDLGFRAME);
	m_cR.right += GetSystemMetrics(SM_CXVSCROLL) + MulDiv(2,GetSystemMetrics(SM_CXDLGFRAME),1);
	MoveWindow(m_cR, true);

	CRect cRDlg;
	GetClientRect(&cRDlg);
	CEdit* pEdit = (CEdit*)GetDlgItem(IDE_TEXT);
	pEdit->SetWindowPos(NULL, 0, 0, cRDlg.Width(), cRDlg.Height(), SWP_NOZORDER);
	m_strText = m_pInfo->GetText();
	if (!m_pInfo->GetSem().IsEmpty()) {
		m_strText += _T("\n");
		if (m_pInfo->GetText().GetAt(m_pInfo->GetText().GetLength() - 1) != '\n')
			m_strText += _T("\n");
		m_strText += m_pInfo->GetSem();
	}
	m_strText.Replace(_T("\n"), _T("\r\n"));
	pEdit->SetFont(&m_Font, true);

	CString strTitle;
	strTitle.Format(_T("Parse Tree Item # %d Information"), m_pInfo->GetItem() + 1);
	SetWindowText(strTitle);

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSemPopupDlg::OnSize(UINT nType, int cx, int cy) 
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDE_TEXT);
	if (pEdit)
		pEdit->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE);
	CDialog::OnSize(nType, cx, cy);
}
