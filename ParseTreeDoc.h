/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_PARSETREEDOC_H__648B4550_C6F3_11D2_8C4C_00105A99059F__INCLUDED_)
#define AFX_PARSETREEDOC_H__648B4550_C6F3_11D2_8C4C_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParseTreeDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParseTreeDoc document

class CParseTreeDoc : public CDocument
{
protected:
	CParseTreeDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CParseTreeDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParseTreeDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CParseTreeDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CParseTreeDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARSETREEDOC_H__648B4550_C6F3_11D2_8C4C_00105A99059F__INCLUDED_)
