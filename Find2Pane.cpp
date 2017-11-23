// Find2Pane.cpp : implementation file
//

#include "stdafx.h"
#include "VisualText.h"
#include "Find2Pane.h"


// CFind2Pane

IMPLEMENT_DYNCREATE(CFind2Pane, CListView)

CFind2Pane::CFind2Pane()
{

}

CFind2Pane::~CFind2Pane()
{
}

BEGIN_MESSAGE_MAP(CFind2Pane, CListView)
END_MESSAGE_MAP()


// CFind2Pane diagnostics

#ifdef _DEBUG
void CFind2Pane::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CFind2Pane::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFind2Pane message handlers
