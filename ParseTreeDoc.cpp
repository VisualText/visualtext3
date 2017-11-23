/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// ParseTreeDoc.cpp : implementation file
//

#include "stdafx.h"
#include "Utils.h"
#include "VisualText.h"
#include "ParseTreeView.h"
#include "ParseTreeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParseTreeDoc

IMPLEMENT_DYNCREATE(CParseTreeDoc, CDocument)

CParseTreeDoc::CParseTreeDoc()
{
}

BOOL CParseTreeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return false;
	return true;
}

CParseTreeDoc::~CParseTreeDoc()
{
}


BEGIN_MESSAGE_MAP(CParseTreeDoc, CDocument)
	//{{AFX_MSG_MAP(CParseTreeDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParseTreeDoc diagnostics

#ifdef _DEBUG
void CParseTreeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CParseTreeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CParseTreeDoc serialization

void CParseTreeDoc::Serialize(CArchive& ar)
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

