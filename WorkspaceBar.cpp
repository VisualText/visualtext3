/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// WorkspaceBar.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "MainFrm.h"
#include "TreeViewGrammar.h"
#include "TreeViewAnalyzer.h"
#include "TreeViewText.h"
#include "WorkspaceBar.h"
#include ".\workspacebar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar

CWorkspaceBar::CWorkspaceBar()
{
}

CWorkspaceBar::~CWorkspaceBar()
{
}

BEGIN_MESSAGE_MAP(CWorkspaceBar, CXTTabCtrlBar)
	//{{AFX_MSG_MAP(CWorkspaceBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar message handlers

int CWorkspaceBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTTabCtrlBar::OnCreate(lpCreateStruct) == -1)
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

	// Create the image list used with the tab control bar.
	if (!m_imageList.Create(IDB_IL_TAB, 16, 1, COLOR_MASK))
	{
		TRACE0("Failed to create image list.\n");
		return -1;
	}

	// Associate the image list with the tab control bar.
	SetTabImageList(&m_imageList);
	SetTabMenuID(IDR_POPUP_WORKSPACE, 0);

	// add tabs
	RefreshTabDisplay();
	
	return 0;
}

void CWorkspaceBar::RefreshTabDisplay()
{
	// first remove everything.
	RemoveAllTabs(FALSE);

	int iIndex = -1;

	// only add tabs that are active
	if ( theApp.m_pMainFrame->m_bGrammarView ) {
		AddControl( TAB_GRAM, &CTreeViewGrammar::Get( ), ++iIndex, 0 );
	}
	if ( theApp.m_pMainFrame->m_bAnalyzerView ) {
		AddControl( TAB_ANA, &CTreeViewAnalyzer::Get( ), ++iIndex, 1 );
	}
	if ( theApp.m_pMainFrame->m_bTextView ) {
		AddControl( TAB_TEXT, &CTreeViewText::Get( ), ++iIndex, 2 );
	}

	SetActiveView( theApp.m_pMainFrame->m_nTabIndex );
}


// FIX: Adding this to eliminate crash on right-click on "Ana" tab name, etc // 05/14/08 AM.
void CWorkspaceBar::OnRButtonDown(UINT nFlags, CPoint point)
{
	logmsg(_T("CWorkspaceBAr::OnRbuttonDown"));	// 05/14/08 AM.

	// No menu and so on for right click here, so remove this. // 05/14/08 AM.
//	CXTTabCtrlBar::OnRButtonDown(nFlags, point);
}
