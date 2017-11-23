/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// KnowledgeDoc.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "KnowledgeDoc.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKnowledgeDoc

IMPLEMENT_DYNCREATE(CKnowledgeDoc, CDocument)

CKnowledgeDoc::CKnowledgeDoc()
{
}

BOOL CKnowledgeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return false;
	return true;
}

CKnowledgeDoc::~CKnowledgeDoc()
{
}


BEGIN_MESSAGE_MAP(CKnowledgeDoc, CDocument)
	//{{AFX_MSG_MAP(CKnowledgeDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKnowledgeDoc diagnostics

#ifdef _DEBUG
void CKnowledgeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CKnowledgeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CKnowledgeDoc serialization

void CKnowledgeDoc::Serialize(CArchive& ar)
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
