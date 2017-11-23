/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// TextEditDoc.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "TextEditDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextEditDoc

IMPLEMENT_DYNCREATE(CTextEditDoc, CDocument)

CTextEditDoc::CTextEditDoc()
{
}

BOOL CTextEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return false;
	return true;
}

CTextEditDoc::~CTextEditDoc()
{
}


BEGIN_MESSAGE_MAP(CTextEditDoc, CDocument)
	//{{AFX_MSG_MAP(CTextEditDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextEditDoc diagnostics

#ifdef _DEBUG
void CTextEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTextEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTextEditDoc serialization

void CTextEditDoc::Serialize(CArchive& ar)
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
// CTextEditDoc commands
