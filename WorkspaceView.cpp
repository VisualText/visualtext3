/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// WorkspaceView.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "MainFrm.h"
#include "TreeViewGrammar.h"
#include "TreeViewAnalyzer.h"
#include "TreeViewText.h"
#include "WorkspaceBarFileTree.h"
#include "WorkspaceView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceView

IMPLEMENT_DYNCREATE(CWorkspaceView, CXTPTabView)

BEGIN_MESSAGE_MAP(CWorkspaceView, CXTPTabView)
	//{{AFX_MSG_MAP(CWorkspaceView)
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	//ON_MESSAGE_VOID(XTWM_INITIAL_UPDATE, OnTabViewUpdate)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceView construction/destruction

CWorkspaceView::CWorkspaceView()
{
	// TODO: add construction code here
	m_iHitTest = -1;
}

CWorkspaceView::~CWorkspaceView()
{
}

BOOL CWorkspaceView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CXTPTabView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceView diagnostics

#ifdef _DEBUG
void CWorkspaceView::AssertValid() const
{
	CXTPTabView::AssertValid();
}

void CWorkspaceView::Dump(CDumpContext& dc) const
{
	CXTPTabView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceView message handlers

int CWorkspaceView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTPTabView::OnCreate(lpCreateStruct) == -1)
		return -1;

    // create the tree control.
	if (!CTreeViewGrammar::Get().Create( this ))
    {
        TRACE0( "Unable to create tree control.\n" );
        return -1;
    }
	
    if (!CTreeViewAnalyzer::Get( ).Create( this ))
    {
        TRACE0( "Unable to create tree control.\n" );
        return -1;
    }
	
    if (!CTreeViewText::Get( ).Create( this ))
    {
        TRACE0( "Unable to create tree control.\n" );
        return -1;
    }
	
	// add the tree control to the tab docking bar.
	AddControl( TAB_GRAM, &CTreeViewGrammar::Get( ) );
	AddControl( TAB_ANA, &CTreeViewAnalyzer::Get( ) );
	AddControl( TAB_TEXT, &CTreeViewText::Get( ) );

	// Create the image list used with the tab control bar.
	if (!m_imageList.Create(IDB_IL_TAB, 16, 1, COLOR_MASK))
	{
		TRACE0("Failed to create image list.\n");
		return -1;
	}

	// Associate the image list with the tab control bar.
	SetTabImageList(&m_imageList);
	SetActiveView(0);

	// Set tabs to bottom
	ModifyTabStyle(NULL, TCS_BOTTOM);

	// hook 
	theApp.m_pChildFrame = GetParentFrame();
	ASSERT_VALID(theApp.m_pChildFrame);

	/*
	if ( theApp.m_pMainFrame && ::IsWindow( theApp.m_pMainFrame->m_wndWorkspaceBar.m_hWnd ) )
	{
		m_hookFrame.Install( &theApp.m_pMainFrame->m_wndWorkspaceBar.GetTabCtrl( ),
			theApp.m_pChildFrame, this );
	}
	*/

	// Recalc tab layout after view has finished initializing.
	//PostMessage( XTWM_INITIAL_UPDATE );

    if (!theApp.m_pMainFrame->m_bTextView)
    {
		DeleteView(&CTreeViewText::Get( ), FALSE);
    }

	if (!theApp.m_pMainFrame->m_bGrammarView)
    {
		DeleteView(&CTreeViewGrammar::Get( ), FALSE);
    }

	if (!theApp.m_pMainFrame->m_bAnalyzerView)
    {
		DeleteView(&CTreeViewAnalyzer::Get( ), FALSE);
    }

	return 0;
}

void CWorkspaceView::OnSelChanging()
{
	CXTPTabView::OnSelChanging();

	// TODO: Add your code to handle tab selection.

}

void CWorkspaceView::OnSelChange()
{
	CXTPTabView::OnSelChange();

	// TODO: Add your code to handle tab selection.

}

void CWorkspaceView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	logmsg(_T("CWorkspaceView::OnRbuttonDown"));	// 05/14/08 AM.

	CPoint pt = point;
	ClientToScreen(&pt);

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_WORKSPACE));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = GetTopLevelFrame();
	
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y,
		pWndPopupOwner);
}

void CWorkspaceView::OnTabViewUpdate() 
{
	// resize all child views.
	POSITION pos;
	for ( pos = m_tcbItems.GetHeadPosition( ); pos; m_tcbItems.GetNext( pos ) )
	{
		ResizeTabView( m_tcbItems.GetAt( pos)->pWnd );
	}

	SetActiveView( theApp.m_pMainFrame->m_nTabIndex );
	GetDocument()->SetTitle( _T( "Workspace" ) );
}

void CWorkspaceView::OnDestroy()
{
	theApp.m_pChildFrame = FALSE;
	
	int iTabIndex = GetTabCtrl().GetCurSel();
	if ( iTabIndex != -1 )
	{
		theApp.m_pMainFrame->m_nTabIndex = iTabIndex;
	}
	
	CXTPTabView::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////////////////////
/*
BOOL CChildFrameHook::Install(CWnd* pParentWnd, CWnd* pWndToHook, CWorkspaceView* pTabView)
{
	ASSERT_VALID( pParentWnd );
	m_pParentWnd = pParentWnd;

	ASSERT_VALID( pTabView );
	m_pTabView = pTabView;

	ASSERT_VALID( pWndToHook );
	return HookWindow( pWndToHook );
}

LRESULT CChildFrameHook::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message )
	{
	case WM_CLOSE:
		{
			// Clean up the tab view array here so the base class destructor
			// does not call DestroyWindow for each tab.
			if (::IsWindow(CTreeViewGrammar::Get().m_hWnd))
			{
				CTreeViewGrammar::Get().SetParent(m_pParentWnd);
				m_pTabView->DeleteView(&CTreeViewGrammar::Get(), FALSE);
			}
			if (::IsWindow(CTreeViewAnalyzer::Get().m_hWnd))
			{
				CTreeViewAnalyzer::Get().SetParent(m_pParentWnd);
				m_pTabView->DeleteView(&CTreeViewAnalyzer::Get(), FALSE);
			}
			if (::IsWindow(CTreeViewText::Get().m_hWnd))
			{
				CTreeViewText::Get().SetParent(m_pParentWnd);
				m_pTabView->DeleteView(&CTreeViewText::Get(), FALSE);
			}
		}
		break;
	}
	
	return CXTWndHook::WindowProc(message, wParam, lParam);
}
*/

CWnd* CWorkspaceView::CreatePane(CWnd* pParentWnd)
{
	if (GetSafeHwnd() == 0)
	{
		VERIFY(Create(_T("STATIC"), NULL, WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CXTPEmptyRect(), pParentWnd, 0));
	}
	return this;
}
