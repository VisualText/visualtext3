/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// BrowserDoc.h : interface of the CBrowserDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BROWSERDOC_H__03B34D3F_ABE0_11D2_8BD2_00105A99059F__INCLUDED_)
#define AFX_BROWSERDOC_H__03B34D3F_ABE0_11D2_8BD2_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBrowserDoc : public CDocument
{
protected: // create from serialization only
	CBrowserDoc();
	DECLARE_DYNCREATE(CBrowserDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowserDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBrowserDoc();
	CString GetUrl();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CBrowserDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BROWSERDOC_H__03B34D3F_ABE0_11D2_8BD2_00105A99059F__INCLUDED_)
