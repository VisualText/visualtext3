/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// SlotDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Utils.h"
#include "kb.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSlotDlg dialog

CSlotDlg::CSlotDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSlotDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSlotDlg)
	//}}AFX_DATA_INIT
}

void CSlotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSlotDlg)
	DDX_Control(pDX, IDC_OUTPUT_LIST, m_listCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSlotDlg, CDialog)
	//{{AFX_MSG_MAP(CSlotDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlotDlg message handlers

void CSlotDlg::FillList()
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	CString outputPathStr = wnd->ProcessedFileStr(_T("output.txt"));

	if (FileExists(outputPathStr)) {
		CStringList lines;
		ReadFileToStringList(lines,outputPathStr);
		POSITION pos = lines.GetHeadPosition();
		CString lineStr;
		CString fieldStr;
		CString valueStr;
		int colon;
		int i = 0;

		while (pos && (lineStr = lines.GetNext(pos))) {
			if (lineStr == _T(""))
				continue;

			colon = lineStr.Find(_T(":"),0);
			if (colon >= 0) {
				fieldStr = lineStr.Mid(0,colon);
				TrimString(fieldStr);

				valueStr = lineStr.Mid(colon+1);
				TrimString(valueStr);

				m_listCtrl.InsertItem(i,fieldStr);
				m_listCtrl.SetItemText(i,1,valueStr);
				i++;
			}
		}
	}
}

BOOL CSlotDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	m_listCtrl.InsertColumn(0,_T("Field"),LVCFMT_LEFT,125);
	m_listCtrl.InsertColumn(1,_T("Value"),LVCFMT_LEFT,200);
	
	FillList();
	
	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

void CSlotDlg::OnOK() 
{
	CDialog::OnOK();
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
}

void CSlotDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
}
