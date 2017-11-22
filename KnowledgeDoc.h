/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_KNOWLEDGEDOC_H__1C09E191_C22C_11D3_A559_00105A99059F__INCLUDED_)
#define AFX_KNOWLEDGEDOC_H__1C09E191_C22C_11D3_A559_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KnowledgeDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKnowledgeDoc document

class CKnowledgeDoc : public CDocument
{
protected:
	CKnowledgeDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CKnowledgeDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKnowledgeDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CKnowledgeDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CKnowledgeDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KNOWLEDGEDOC_H__1C09E191_C22C_11D3_A559_00105A99059F__INCLUDED_)
