// FindPane2.cpp : implementation file
//

#include "stdafx.h"
#include "VisualText.h"
#include "FindPane2.h"


// CFindPane2

IMPLEMENT_DYNCREATE(CFindPane2, CWnd)

CFindPane2::CFindPane2()
{

}

CFindPane2::~CFindPane2()
{
}

BEGIN_MESSAGE_MAP(CFindPane2, CWnd)
	//{{AFX_MSG_MAP(CFindPane2)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFindPane message handlers

static int arColWidths[] = {
	200,
	600
};

static CString arColLabels[] = {
	_T("File"),
	_T("Line"),
};

int CFindPane2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (m_listCtrl.GetSafeHwnd() == 0)
	{
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

		m_listCtrl.SetMenuID(IDR_FIND);
	}

	return 0;
}

void CFindPane2::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_listCtrl.GetSafeHwnd())
	{
		m_listCtrl.MoveWindow(0, 0, cx, cy);
	}
}

void CFindPane2::OnSetFocus(CWnd* /*pOldWnd*/)
{
	m_listCtrl.SetFocus();
}


