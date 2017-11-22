/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// CntrItem.h : interface of the CVisualTextCntrItem class
//

#if !defined(AFX_CNTRITEM_H__71607A22_E940_11D2_A2B5_00105A99059F__INCLUDED_)
#define AFX_CNTRITEM_H__71607A22_E940_11D2_A2B5_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVisualTextDoc;
class CVisualTextView;

class CVisualTextCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CVisualTextCntrItem)

// Constructors
public:
	CVisualTextCntrItem(REOBJECT* preo = NULL, CVisualTextDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CVisualTextDoc* GetDocument()
		{ return (CVisualTextDoc*)CRichEditCntrItem::GetDocument(); }
	CVisualTextView* GetActiveView()
		{ return (CVisualTextView*)CRichEditCntrItem::GetActiveView(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisualTextCntrItem)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	~CVisualTextCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CNTRITEM_H__71607A22_E940_11D2_A2B5_00105A99059F__INCLUDED_)
