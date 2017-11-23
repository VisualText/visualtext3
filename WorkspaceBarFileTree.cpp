// WorkspaceBarFileTree.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "WorkspaceBarFileTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBarFileTree

CWorkspaceBarFileTree::CWorkspaceBarFileTree()
{
}

CWorkspaceBarFileTree::~CWorkspaceBarFileTree()
{
}

IMPLEMENT_DYNAMIC(CWorkspaceBarFileTree, CXTTreeCtrl)

BEGIN_MESSAGE_MAP(CWorkspaceBarFileTree, CXTTreeCtrl)
	//{{AFX_MSG_MAP(CWorkspaceBarFileTree)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBarFileTree message handlers

BOOL CWorkspaceBarFileTree::PreCreateWindow(CREATESTRUCT& cs) 
{
	if ( !CXTTreeCtrl::PreCreateWindow( cs ) )
		return FALSE;

	// Set the style for the tree control.
	cs.style |= TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_EDITLABELS;
	cs.dwExStyle |= WS_EX_CLIENTEDGE;

	// TODO: Add your specialized code here and/or call the base class
	
	return TRUE;
}

int CWorkspaceBarFileTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// load the tree images bitmap and add it to the image list.
	if (!m_imageList.Create(IDB_FILETREE, 16, 1, COLOR_MASK))
		return -1;

	// Set the image list for the tree control.
	SetImageList( &m_imageList, TVSIL_NORMAL );

	// add the parent item, make it bold
	HTREEITEM htiParent = InsertItem(_T("Workspace 'XtremeDemo': 1 project(s)"));
	HTREEITEM htiChild; // child item
	
	htiChild = InsertItem (_T("XtremeDemo Files"), 1, 1, htiParent);
	SetItemState (htiChild, TVIS_BOLD, TVIS_BOLD);
	
	// add the children of the parent item
	HTREEITEM hti = InsertItem(_T("Source Files"), 2, 3, htiChild);
	InsertItem(_T("ChildFrm.cpp"), 4, 4, hti);
	InsertItem(_T("MainFrm.cpp"), 4, 4, hti);
	InsertItem(_T("StdAfx.cpp"), 4, 4, hti);
	InsertItem(_T("XtremeDemo.cpp"), 4, 4, hti);
	InsertItem(_T("XtremeDemo.rc"), 4, 4, hti);
	InsertItem(_T("XtremeDemoDoc.cpp"), 4, 4, hti);
	InsertItem(_T("XtremeDemoView.cpp"), 4, 4, hti);
	Expand(hti, TVE_EXPAND);
	
	hti = InsertItem(_T("Header Files"), 2, 3, htiChild);
	InsertItem(_T("ChildFrm.h"), 5, 5, hti);
	InsertItem(_T("MainFrm.h"), 5, 5, hti);
	InsertItem(_T("Resource.rc"), 5, 5, hti);
	InsertItem(_T("StdAfx.h"), 5, 5, hti);
	InsertItem(_T("XtremeDemo.h"), 5, 5, hti);
	InsertItem(_T("XtremeDemoDoc.h"), 5, 5, hti);
	InsertItem(_T("XtremeDemoView.h"), 5, 5, hti);
	
	hti = InsertItem(_T("Resource Files"), 2, 3, htiChild);
	InsertItem(_T("Toolbar.bmp"), 5, 5, hti);
	InsertItem(_T("XtremeDemo.ico"), 5, 5, hti);
	InsertItem(_T("XtremeDemo.rc2"), 5, 5, hti);
	InsertItem(_T("XtremeDemoDoc.ico"), 5, 5, hti);
	
	InsertItem(_T("ReadMe.txt"), 5, 5, htiChild);
	InsertItem(_T("External Dependencies"), 2, 3, htiChild);
	
	Expand(htiParent, TVE_EXPAND);
	Expand(htiChild, TVE_EXPAND);
	
	return 0;
}

void CWorkspaceBarFileTree::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	// TODO: Add your control notification handler code here
	if ( pTVDispInfo->item.pszText )
	{
		SetItemText( pTVDispInfo->item.hItem,
			pTVDispInfo->item.pszText );
	}
	
	*pResult = 0;
}

CWorkspaceBarFileTree& CWorkspaceBarFileTree::Get()
{
	static CWorkspaceBarFileTree theTree;
	return theTree;
}

BOOL CWorkspaceBarFileTree::Create(CWnd* pParentWnd)
{
	if (::IsWindow(m_hWnd))
	{
		SetParent(pParentWnd);
	}
	else if (!CXTTreeCtrl::Create( WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
		CRect(0,0,0,0), pParentWnd, 0xFF ))
	{
		TRACE0( "Unable to create tree control.\n" );
		return false;
	}

	return true;
}
