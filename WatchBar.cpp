/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// CWatchBar : implementation file
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "kb.h"
#include "Resource.h"
#include "WatchBar.h"
#include ".\watchbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWatchBar

CWatchBar::CWatchBar()
{
	m_cy = ::GetSystemMetrics(SM_CYHSCROLL);

	LOGFONT lf;
	xtAfxData.font.GetLogFont(&lf);
	_tcscpy(lf.lfFaceName, _T("Courier"));
	m_sheetFont.CreateFontIndirect(&lf);
}

CWatchBar::~CWatchBar()
{
}

IMPLEMENT_DYNAMIC(CWatchBar, CXTDockWindow)

BEGIN_MESSAGE_MAP(CWatchBar, CXTDockWindow)
	//{{AFX_MSG_MAP(CWatchBar)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchBar message handlers

int CWatchBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTDockWindow::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc;
	GetInsideRect(rc);
	rc.DeflateRect(1,1);

	// Create  the flat tab control.
	if (!m_flatTabCtrl.Create(WS_CHILD|WS_VISIBLE|(FTS_XT_DEFAULT & ~FTS_XT_HASHOMEEND)|FTS_XT_HSCROLL,
		rc, this, IDC_FLATTABCTRL))
	{
		TRACE0( "Failed to create flattab control\n" );
		return -1;
	}

	CreateTabListCtrl(m_sheet1, IDC_SHEET1);
	CreateTabListCtrl(m_sheet2, IDC_SHEET2);
	CreateTabListCtrl(m_sheet3, IDC_SHEET3);
	CreateTabListCtrl(m_sheet4, IDC_SHEET4);

	// Insert tabs into the flat tab control.
	m_flatTabCtrl.InsertItem(0, _T("Watch1"), &m_sheet1);
	m_flatTabCtrl.InsertItem(1, _T("Watch2"), &m_sheet2);
	m_flatTabCtrl.InsertItem(2, _T("Watch3"), &m_sheet3);
	m_flatTabCtrl.InsertItem(3, _T("Watch4"), &m_sheet4);

	// Insert text into the list control.
    AddWatchVar(m_sheet1, _T("ERR,hr"), _T("S_OK"));
    AddWatchVar(m_sheet1, _T("m_iTotalItems"), _T("275"));
    AddWatchVar(m_sheet1, _T("rc"), _T("{top=4 bottom=121 left=4 right=396}"));

	// Set the current tab.
    m_flatTabCtrl.SetCurSel(0);
	EnableContextMenus(false);

	return 0;
}

BOOL CWatchBar::CreateTabListCtrl(CWatchBarList& listCtrl, UINT nID)
{
	// Define the default style for the output list controls.
	DWORD dwStyle = WS_CHILD | LVS_REPORT | LVS_NOSORTHEADER | 
		WS_VSCROLL | WS_TABSTOP;

	// Create the list control.
	if (!listCtrl.Create( dwStyle, CRect(0,0,0,0), &m_flatTabCtrl, nID ))
	{
		TRACE0( "Failed to create sheet4.\n" );
		return FALSE;
	}

	listCtrl.InsertColumn(0, _T("Name"),  LVCFMT_LEFT, 125);
	listCtrl.InsertColumn(1, _T("Value"), LVCFMT_LEFT, 355);

	ListView_SetExtendedListViewStyle(listCtrl.m_hWnd, LVS_EX_GRIDLINES);

	listCtrl.SetFont(&m_sheetFont);
	listCtrl.GetDlgItem(0)->SetFont(&xtAfxData.fontBold);

	return TRUE;
}

void CWatchBar::AddWatchVar(CWatchBarList& listCtrl, LPCTSTR pszName, LPCTSTR pszVal)
{
    int iItem = listCtrl.GetItemCount();
    listCtrl.InsertItem(iItem, pszName);
	listCtrl.SetItem(iItem, 1, LVIF_TEXT, pszVal, 0, NULL, NULL, NULL);
}

BOOL CWatchBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

void CWatchBar::OnWindowPosChanged(WINDOWPOS FAR* lpWP) 
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

// 05/14/08 AM.
void CWatchBar::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	logmsg(_T("CwatchBAr::OnRbuttonDown"));	// 05/14/08 AM.
//	CXTDockWindow::OnRButtonDown(nFlags, point);	// 05/14/08 AM.
}
