/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// CntrItem.cpp : implementation of the CVisualTextCntrItem class
//

#include "stdafx.h"
#include "VisualText.h"

#include "kb.h"
#include "VisualTextDoc.h"
#include "VisualTextView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVisualTextCntrItem implementation

IMPLEMENT_SERIAL(CVisualTextCntrItem, CRichEditCntrItem, 0)

CVisualTextCntrItem::CVisualTextCntrItem(REOBJECT* preo, CVisualTextDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
	// TODO: add one-time construction code here
	
}

CVisualTextCntrItem::~CVisualTextCntrItem()
{
	// TODO: add cleanup code here
	
}

/////////////////////////////////////////////////////////////////////////////
// CVisualTextCntrItem diagnostics

#ifdef _DEBUG
void CVisualTextCntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

void CVisualTextCntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
