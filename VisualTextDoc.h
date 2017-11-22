/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// VisualTextDoc.h : interface of the CVisualTextDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISUALTEXTDOC_H__5078BC9C_EEA1_11D2_A2C4_00105A99059F__INCLUDED_)
#define AFX_VISUALTEXTDOC_H__5078BC9C_EEA1_11D2_A2C4_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define CDocBase	CXTPSyntaxEditDoc

class CVisualTextDoc : public CRichEditDoc
{
protected: // create from serialization only
	CVisualTextDoc();
	DECLARE_DYNCREATE(CVisualTextDoc)

// Attributes
public:
	DOC_TYPE m_DocType;
	CONCEPT *m_concept;
	BOOL m_bPromptSaveModified; //SRP 2001-02-01

// Operations
public:
	void Update();
	void ChangeType(DOC_TYPE docType);
	void UpdateDocviewContent();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisualTextDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void SetPathName( LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE );
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument(); //SRP 2001-02-01
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame); //SRP 2001-02-01
	protected:
	//}}AFX_VIRTUAL
	virtual CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const;

// Implementation
public:
	virtual ~CVisualTextDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void SetReadOnly(bool bSetting);
	bool GetReadOnly() const;
	LPCTSTR GetTitle() const;

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVisualTextDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	DOC_TYPE DocumentTypeFromFile(const CString &path);
	bool m_bReadOnly;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISUALTEXTDOC_H__5078BC9C_EEA1_11D2_A2C4_00105A99059F__INCLUDED_)
