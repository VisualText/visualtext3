// WorkspaceBarClassTree.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "WorkspaceBarClassTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBarClassTree

CWorkspaceBarClassTree::CWorkspaceBarClassTree()
{
}

CWorkspaceBarClassTree::~CWorkspaceBarClassTree()
{
}

IMPLEMENT_DYNAMIC(CWorkspaceBarClassTree, CXTTreeCtrl)

BEGIN_MESSAGE_MAP(CWorkspaceBarClassTree, CXTTreeCtrl)
	//{{AFX_MSG_MAP(CWorkspaceBarClassTree)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBarClassTree message handlers

BOOL CWorkspaceBarClassTree::PreCreateWindow(CREATESTRUCT& cs) 
{
	if ( !CXTTreeCtrl::PreCreateWindow( cs ) )
		return FALSE;

	// Set the style for the tree control.
	cs.style |= TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_EDITLABELS;
	cs.dwExStyle |= WS_EX_CLIENTEDGE;

	// TODO: Add your specialized code here and/or call the base class
	
	return TRUE;
}

int CWorkspaceBarClassTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// load the tree images bitmap and add it to the image list.
	if (!m_imageList.Create(IDB_CLASSTREE, 16, 1, COLOR_MASK))
		return -1;

	// Set the image list for the tree control.
	SetImageList( &m_imageList, TVSIL_NORMAL );

	// Add the parent item
	HTREEITEM htItem = InsertItem(_T("XtremeDemo classes"));
	SetItemState( htItem, TVIS_BOLD, TVIS_BOLD );
	
	// Add children
	HTREEITEM hti = InsertItem(_T("CAboutDlg"), 3, 3, htItem);
	InsertItem(_T("CAboutDlg()"), 4, 4, hti);
	InsertItem(_T("DoDataExchange(CDataExchange *pDX)"), 5, 5, hti);
	
	hti = InsertItem(_T("CChildFrame"), 3, 3, htItem);
	InsertItem(_T("AssertValid()"), 4, 4, hti);
	InsertItem(_T("CChildFrame()"), 4, 4, hti);
	InsertItem(_T("~CChildFrame()"), 4, 4, hti);
	InsertItem(_T("Dump(CDumpContext &dc)"), 4, 4, hti);
	InsertItem(_T("PreCreateWindow(CREATESTRUCT& cs)"), 4, 4, hti);
	
	hti = InsertItem(_T("CMainFrame"), 3, 3, htItem);
	InsertItem(_T("AssertValid()"), 4, 4, hti);
	InsertItem(_T("CMainFrame()"), 4, 4, hti);
	InsertItem(_T("~CMainFrame()"), 4, 4, hti);
	InsertItem(_T("Dump(CDumpContext &dc)"), 4, 4, hti);
	InsertItem(_T("OnCreate(LPCREATESTRUCT lpCreateStruct)"), 5, 5, hti);
	InsertItem(_T("PreCreateWindow(CREATESTRUCT& cs)"), 4, 4, hti);
	InsertItem(_T("m_wndStatusBar"), 7, 7, hti);
	InsertItem(_T("m_wndToolBar"), 7, 7, hti);
	
	hti = InsertItem(_T("CXtremeDemoApp"), 3, 3, htItem);
	InsertItem(_T("CXtremeDemoApp()"), 4, 4, hti);
	InsertItem(_T("InitInstance()"), 4, 4, hti);
	InsertItem(_T("OnAppAbout()"), 4, 4, hti);
	
	hti = InsertItem(_T("CXtremeDemoDoc"), 3, 3, htItem);
	InsertItem(_T("AssertValid()"), 5, 5, hti);
	InsertItem(_T("CXtremeDemoDoc()"), 4, 4, hti);
	InsertItem(_T("~CXtremeDemoDoc()"), 4, 4, hti);
	InsertItem(_T("Dump(CDumpContext &dc)"), 4, 4, hti);
	InsertItem(_T("OnNewDocument()"), 4, 4, hti);
	InsertItem(_T("Serialize(CArchive& ar)"), 4, 4, hti);
	Expand(hti, TVE_EXPAND);
	
	hti = InsertItem(_T("CXtremeDemoView"), 3, 3, htItem);
	InsertItem(_T("AssertValid()"), 4, 4, hti);
	InsertItem(_T("CXtremeDemoView()"), 5, 5, hti);
	InsertItem(_T("~CXtremeDemoView()"), 4, 4, hti);
	InsertItem(_T("Dump(CDumpContext& dc)"), 4, 4, hti);
	InsertItem(_T("GetDocument()"), 4, 4, hti);
	InsertItem(_T("OnBeginPrinting(CDC* pDC, CPrintInfo *pInfo)"), 5, 5, hti);
	InsertItem(_T("OnDraw(CDC *pDC)"), 4, 4, hti);
	InsertItem(_T("OnEndPrinting(CDC *pDC, CPrintInfo *pInfo)"), 5, 5, hti);
	InsertItem(_T("OnPreparePrinting(CPrintInfo *pInfo)"), 5, 5, hti);
	InsertItem(_T("PreCreateWindow(CREATESTRUCT &cs)"), 4, 4, hti);
	
	hti = InsertItem(_T("Globals"), 1, 2, htItem);
	InsertItem(_T("theApp"), 6, 6, hti);
	Expand(hti, TVE_EXPAND);
	
	Expand(htItem, TVE_EXPAND);
	
	return 0;
}

void CWorkspaceBarClassTree::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
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

CWorkspaceBarClassTree& CWorkspaceBarClassTree::Get()
{
	static CWorkspaceBarClassTree theTree;
	return theTree;
}

BOOL CWorkspaceBarClassTree::Create(CWnd* pParentWnd)
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
