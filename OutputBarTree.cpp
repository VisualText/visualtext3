/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// OutputBarTree.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "OutputBarTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBarTree

COutputBarTree::COutputBarTree()
{
}

COutputBarTree::~COutputBarTree()
{
}

BEGIN_MESSAGE_MAP(COutputBarTree, CTreeCtrl)
	//{{AFX_MSG_MAP(COutputBarTree)
	ON_WM_WINDOWPOSCHANGED()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputBarTree message handlers

void COutputBarTree::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	// for the tree control, we'll call ShowScrollBar(SB_HORZ, FALSE) both
	// before and after CTreeCtrl::OnWindowPosChanged() - this seems to 
	// reduce flicker much more
	ShowScrollBar(SB_HORZ, FALSE);
	CTreeCtrl::OnWindowPosChanged(lpwndpos);
	ShowScrollBar(SB_HORZ, FALSE);
	ModifyStyle(WS_HSCROLL, 0, SWP_DRAWFRAME);
}

void COutputBarTree::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	// since the newly expanded / collapsed item could affect the horz.
	// scroll bar, we'll force a resync here
	//((CXTPFlatTabCtrl *)GetParent())->SyncScrollBar();

	*pResult = 0;
}

void COutputBarTree::PreSubclassWindow() 
{
	CTreeCtrl::PreSubclassWindow();
	CommonInit();
}

int COutputBarTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CommonInit();
	
	return 0;
}

void COutputBarTree::CommonInit()
{
	// make tooltips "top-most" so they don't get obscured by the flat tab
	// when floating in a dock bar
	CToolTipCtrl *pTTCtrl = 
#if _MSC_VER >= 1200  // VC6
		GetToolTips();
#else
		(CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TVM_GETTOOLTIPS, 0, 0L));
#endif

	if (pTTCtrl->GetSafeHwnd())
	{
		pTTCtrl->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, 
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}
}
