/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// BrowserDoc.cpp : implementation of the CBrowserDoc class
//

#include "stdafx.h"
#include "Resource.h"
#include "kb.h"
#include "Utils.h"
#include "BrowserDoc.h"
#include "BrowserView.h"

extern CString appURL;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrowserDoc

IMPLEMENT_DYNCREATE(CBrowserDoc, CDocument)

BEGIN_MESSAGE_MAP(CBrowserDoc, CDocument)
	//{{AFX_MSG_MAP(CBrowserDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowserDoc construction/destruction

CBrowserDoc::CBrowserDoc()
{
	// TODO: add one-time construction code here

}

CBrowserDoc::~CBrowserDoc()
{
}

BOOL CBrowserDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return false;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CBrowserDoc serialization

void CBrowserDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBrowserDoc diagnostics

#ifdef _DEBUG
void CBrowserDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBrowserDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBrowserDoc commands

CString CBrowserDoc::GetUrl()
{
	CBrowserView *view = (CBrowserView *)GetFirstView(this);
	return view->GetLocationURL();
}