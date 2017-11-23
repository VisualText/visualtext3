/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// WatchBarList.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "WatchBarList.h"
#include ".\watchbarlist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWatchBarList

CWatchBarList::CWatchBarList()
{
}

CWatchBarList::~CWatchBarList()
{
}


BEGIN_MESSAGE_MAP(CWatchBarList, CListCtrl)
	//{{AFX_MSG_MAP(CWatchBarList)
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchBarList message handlers

void CWatchBarList::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CListCtrl::OnWindowPosChanged(lpwndpos);
	
    ShowScrollBar(SB_HORZ, FALSE);
    ModifyStyle(WS_HSCROLL, 0, SWP_DRAWFRAME);
}

void CWatchBarList::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	logmsg(_T("CWatchBArList::OnRbuttonDown"));	// 05/14/08 AM.
//	CListCtrl::OnRButtonDown(nFlags, point);
}
