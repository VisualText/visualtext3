/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// OutputBar.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "kb.h"
#include "Utils.h"
#include "MainFrm.h"
#include "TreeViewAnalyzer.h"
#include "OutputBar.h"
#include ".\outputbar.h"

extern CVisualTextApp theApp;
extern CString appStrLocation;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputBar::COutputBar()
{
	m_cy = ::GetSystemMetrics(SM_CYHSCROLL);

	LOGFONT lf;
	xtAfxData.font.GetLogFont(&lf);
	_tcscpy(lf.lfFaceName, _T("Courier"));
	m_sheetFont.CreateFontIndirect(&lf);
}

COutputBar::~COutputBar()
{
	// TODO: add destruction code here.
}

IMPLEMENT_DYNAMIC(COutputBar, CXTDockWindow)

BEGIN_MESSAGE_MAP(COutputBar, CXTDockWindow)
	//{{AFX_MSG_MAP(COutputBar)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputBar message handlers

int COutputBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTDockWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc;
	GetInsideRect(rc);
	rc.DeflateRect(1,1);
	
    // Create  the flat tab control.
	if (!m_flatTabCtrl.Create(WS_CHILD|WS_VISIBLE|FTS_XT_DEFAULT|FTS_XT_HSCROLL,
		rc, this, IDC_FLATTABCTRL))
	{
		TRACE0( "Failed to create flattab control\n" );
		return -1;
	}

	CreateTabOutputWnd(m_sheet1, _T("Operation"), _T("Description"), IDC_SHEET1);
	CreateTabOutputWnd(m_sheet2, _T("Operation"), _T("Description"), IDC_SHEET2);
	CreateTabOutputWnd(m_sheet3, _T("File"), _T("Line"), IDC_SHEET3);
	CreateTabOutputWnd(m_sheet4, _T("File"), _T("Line"), IDC_SHEET4);

    // Insert tabs into the flat tab control.
	m_flatTabCtrl.InsertItem(0, _T("Analyze"),         &m_sheet1);
	m_flatTabCtrl.InsertItem(1, _T("Log"),           &m_sheet2);
	m_flatTabCtrl.InsertItem(2, _T("Find in Files 1"), &m_sheet3);
	m_flatTabCtrl.InsertItem(3, _T("Find in Files 2"), &m_sheet4);

	// Set the current tab.
    m_flatTabCtrl.SetCurSel(0);

	return 0;
}

BOOL COutputBar::CreateTabOutputWnd(COutputBarList& listCtrl, CString column1Str, CString column2Str, UINT nID)
{
	// Define the default style for the output list controls.
	DWORD dwStyle = WS_CHILD | LVS_REPORT | LVS_NOSORTHEADER | 
		WS_VSCROLL | WS_TABSTOP;

	// Create the list control.
	if (!listCtrl.Create( dwStyle, CRect(0,0,0,0), &m_flatTabCtrl, nID ))
	{
		TRACE0( "Failed to create sheet.\n" );
		return FALSE;
	}

	listCtrl.InsertColumn(0, column1Str,  LVCFMT_LEFT, 300);
	listCtrl.InsertColumn(1, column2Str, LVCFMT_LEFT, 500);

	ListView_SetExtendedListViewStyle(listCtrl.m_hWnd, LVS_EX_TWOCLICKACTIVATE |
		LVS_EX_FULLROWSELECT | LVS_EX_UNDERLINECOLD | LVS_EX_UNDERLINEHOT);

	listCtrl.SetFont(&m_sheetFont);
	listCtrl.GetDlgItem(0)->SetFont(&xtAfxData.fontBold);

	return TRUE;
}

BOOL COutputBar::CreateTabOutputWnd(COutputBarEdit& wndOutput, UINT nID)
{
	// Create the sheet1 list box.
	if (!wndOutput.Create(WS_CHILD, CRect(0,0,0,0), &m_flatTabCtrl, nID))
	{
		TRACE1( "Failed to create sheet id=%d.\n", nID );
		return FALSE;
	}

	wndOutput.SetFont(&m_sheetFont);

	return TRUE;
}

BOOL COutputBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	if(IDC_FLATTABCTRL == (UINT)wParam)
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;

		switch(pNMHDR->code)
		{
		case TCN_SELCHANGING:
			break;
		case TCN_SELCHANGE:
			break;
		}
	}
	
	return CXTDockWindow::OnNotify(wParam, lParam, pResult);
}

void COutputBar::OnWindowPosChanged(WINDOWPOS FAR* lpWP) 
{
	CXTDockWindow::OnWindowPosChanged(lpWP);
	
	CRect rc;
	GetInsideRect(rc);
	rc.DeflateRect(1,1);

	// Resize the flat tab control...
	if (m_flatTabCtrl.GetSafeHwnd()) {
		m_flatTabCtrl.MoveWindow(rc);
	}
}
