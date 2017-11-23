// OutputPane.cpp : implementation file
//

#include "stdafx.h"
#include "VisualText.h"
#include "OutputPane.h"


// COutputPane

IMPLEMENT_DYNCREATE(COutputPane, CListView)

COutputPane::COutputPane()
{

}

COutputPane::~COutputPane()
{
}

BEGIN_MESSAGE_MAP(COutputPane, CListView)
END_MESSAGE_MAP()


// COutputPane diagnostics

#ifdef _DEBUG
void COutputPane::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void COutputPane::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// COutputPane message handlers
