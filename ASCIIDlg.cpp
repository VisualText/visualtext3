/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// ASCIIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "ASCIIDlg.h"
#include ".\asciidlg.h"

extern CFont appFontFixed;
extern CString appStrFontFixed;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CASCIIDlg dialog


CASCIIDlg::CASCIIDlg(CWnd* pParent /*=NULL*/)
	: CXTPResizeDialog(CASCIIDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CASCIIDlg)
	m_asciiStr = _T("");
	//}}AFX_DATA_INIT
}


void CASCIIDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTPResizeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CASCIIDlg)
	DDX_Text(pDX, IDC_ACSII_DISPLAY, m_asciiStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CASCIIDlg, CXTPResizeDialog)
	//{{AFX_MSG_MAP(CASCIIDlg)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CASCIIDlg message handlers

BOOL CASCIIDlg::OnInitDialog() 
{
	CXTPResizeDialog::OnInitDialog();

	CEdit *ed = (CEdit *)GetDlgItem(IDC_ACSII_DISPLAY);
	ed->SetFont(&appFontFixed);

	SetResize(IDC_ACSII_DISPLAY, XTP_ANCHOR_TOPLEFT, XTP_ANCHOR_BOTTOMRIGHT);
	SetResize(IDOK, XTP_ANCHOR_BOTTOMCENTER, XTP_ANCHOR_BOTTOMCENTER);
	
	LoadPlacement(_T("CASCIIDlg"));

	FillAsciiString();

	CString strWindowTitle;
	strWindowTitle.Format(_T("ASCII Table - Font : %s"), appStrFontFixed);
	SetWindowText(strWindowTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CASCIIDlg::FillAsciiString()
{
	m_asciiStr = _T("");
	CString chStr;
	int numCols = 4;
	int perCol = 256 / numCols;
	int j,k;

	setlocale(LC_ALL, "");
 
	for (int i=0; i<perCol; i++) {
		for (j=0; j<numCols; j++) {
			k = i+j*perCol;
			CString c((char)k); 
			if (k == 0 || k == 9)
#ifdef UNICODE
				chStr.Format(_T("%3d %2x %C   "),k,k,1);
#else
				chStr.Format(_T("%3d %2x %c   "),k,k,1);
#endif
			else
#ifdef UNICODE
				chStr.Format(_T("%3d %2x %C   "),k,k,k);
#else
				chStr.Format(_T("%3d %2x %c   "),k,k,k);
#endif
			m_asciiStr += chStr;
		}
		m_asciiStr += _T("\r\n");
	}

	UpdateData(false);
}

void CASCIIDlg::OnDestroy()
{
	CXTPResizeDialog::OnDestroy();

	SavePlacement(_T("CASCIIDlg"));
}
