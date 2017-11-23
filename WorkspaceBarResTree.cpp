// WorkspaceBarResTree.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "WorkspaceBarResTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBarResTree

CWorkspaceBarResTree::CWorkspaceBarResTree()
{
}

CWorkspaceBarResTree::~CWorkspaceBarResTree()
{
}

IMPLEMENT_DYNAMIC(CWorkspaceBarResTree, CXTTreeCtrl)

BEGIN_MESSAGE_MAP(CWorkspaceBarResTree, CXTTreeCtrl)
	//{{AFX_MSG_MAP(CWorkspaceBarResTree)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBarResTree message handlers

BOOL CWorkspaceBarResTree::PreCreateWindow(CREATESTRUCT& cs) 
{
	if ( !CXTTreeCtrl::PreCreateWindow( cs ) )
		return FALSE;

	// Set the style for the tree control.
	cs.style |= TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_EDITLABELS;
	cs.dwExStyle |= WS_EX_CLIENTEDGE;

	// TODO: Add your specialized code here and/or call the base class
	
	return TRUE;
}

int CWorkspaceBarResTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// load the tree images bitmap and add it to the image list.
	if (!m_imageList.Create(IDB_RESTREE, 16, 1, COLOR_MASK))
		return -1;

	// Set the image list for the tree control.
	SetImageList( &m_imageList, TVSIL_NORMAL );

	// add the parent item
	HTREEITEM htItem = InsertItem(_T("XtremeDemo resources"), 0, 1);
	SetItemState(htItem, TVIS_BOLD, TVIS_BOLD);
	
	HTREEITEM hti = InsertItem(_T("Accelerator"), 0, 1, htItem);
	InsertItem(_T("IDR_MAINFRAME"), 2, 2, hti);
	Expand(hti, TVE_EXPAND);
	
	hti = InsertItem(_T("Dialog"), 0, 1, htItem);
	InsertItem(_T("IDD_ABOUTBOX"), 3, 3, hti);
	Expand(hti, TVE_EXPAND);
	
	hti = InsertItem(_T("Icon"), 0, 1, htItem);
	InsertItem(_T("IDR_MAINFRAME"), 4, 4, hti);
	InsertItem(_T("IDR_XTREMETYPE"), 4, 4, hti);
	Expand(hti, TVE_EXPAND);
	
	hti = InsertItem(_T("Menu"), 0, 1, htItem);
	InsertItem(_T("IDR_MAINFRAME"), 5, 5, hti);
	InsertItem(_T("IDR_XTREMETYPE"), 5, 5, hti);
	
	hti = InsertItem(_T("String Table"), 0, 1, htItem);
	InsertItem(_T("String Table"), 6, 6, hti);

	hti = InsertItem(_T("Toolbar"), 0, 1, htItem);
	InsertItem(_T("IDR_MAINFRAME"), 7, 7, hti);

	hti = InsertItem(_T("Version"), 0, 1, htItem);
	InsertItem(_T("VS_VERSION_INFO"), 8, 8, hti);

	Expand (htItem, TVE_EXPAND);
	
	return 0;
}

void CWorkspaceBarResTree::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
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

CWorkspaceBarResTree& CWorkspaceBarResTree::Get()
{
	static CWorkspaceBarResTree theTree;
	return theTree;
}

BOOL CWorkspaceBarResTree::Create(CWnd* pParentWnd)
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
