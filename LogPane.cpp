// LogPane.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "Utils.h"
#include "LogPane.h"

// CLogPane

IMPLEMENT_DYNCREATE(CLogPane, CWnd)

CLogPane::CLogPane()
{
}

CLogPane::~CLogPane()
{
}

BEGIN_MESSAGE_MAP(CLogPane, CWnd)
	//{{AFX_MSG_MAP(CLogPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_SETFOCUS()
	ON_NOTIFY(NM_RCLICK, NM_RCLICK, OnRButtonDown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogPane message handlers

static int arColWidths[] = {
	200,
	600
};

static CString arColLabels[] = {
	_T("Operation"),
	_T("Description"),
};

int CLogPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	if (m_listCtrl.GetSafeHwnd() == 0)
	{
		// Define the default style for the output list controls.
		//DWORD dwStyle = LVS_REPORT | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS |
		//	WS_CHILD | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE | LVS_EX_FULLROWSELECT;
		DWORD dwStyle = LVS_REPORT | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS |
			WS_CHILD | WS_VSCROLL | WS_TABSTOP|WS_VISIBLE;

		// Create the list control.
		if (!m_listCtrl.Create( dwStyle, CRect(0,0,0,0), this, 0xff ))
		{
			TRACE0( "Failed to create list control.\n" );
			return FALSE;
		}

		// initialize the list control.

		// Insert the columns.
		m_listCtrl.BuildColumns(_countof(arColWidths), arColWidths, arColLabels);

		// Subclass the flat header control.
		m_listCtrl.SubclassHeader();

		// lock the first two colums from sizing operations.
		CXTPHeaderCtrl* pHeaderCtrl = m_listCtrl.XTPGetHeaderCtrl( );
		if ( pHeaderCtrl != NULL )
		{
			//pHeaderCtrl->FreezeColumn(0);

			// enable autosizing for columns.
			pHeaderCtrl->EnableAutoSize();
		}

		ListView_SetExtendedListViewStyle(
			m_listCtrl.m_hWnd, LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
		m_listCtrl.AutoSaveColumns(_T("LogListColumns"));

		m_listCtrl.SetMenuID(IDR_LOG);
	}

	return 0;
}

void CLogPane::OnRButtonDown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint point, ptScreen;
	::GetCursorPos(&ptScreen);
	
	point = ptScreen;
	m_listCtrl.ScreenToClient(&point);

	logmsg(_T("CLogPane::OnRbuttonDown"));	// 05/14/08 AM.

	PopupContextMenu(this,IDR_FIND,point);
}

void CLogPane::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_listCtrl.GetSafeHwnd())
	{
		m_listCtrl.MoveWindow(0, 0, cx, cy);
	}
}

void CLogPane::OnSetFocus(CWnd* /*pOldWnd*/)
{
	m_listCtrl.SetFocus();
}

