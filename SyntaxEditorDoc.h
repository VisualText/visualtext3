// MDITextEditorDoc.h : interface of the CSyntaxEditorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDITEXTEDITORDOC_H__51869C8E_3587_4FB1_9C17_8ACB2424D551__INCLUDED_)
#define AFX_MDITEXTEDITORDOC_H__51869C8E_3587_4FB1_9C17_8ACB2424D551__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define CDocBase	CDocument
#define CDocBase	CXTPSyntaxEditDoc

class CSyntaxEditorDoc : public CDocBase
{
protected: // create from serialization only
	CSyntaxEditorDoc();
	DECLARE_DYNCREATE(CSyntaxEditorDoc)

// Attributes
public:
	CONCEPT *m_concept;

// Operations
public:
	bool IsCurrentPassFile() const;

private:
	bool m_bReadOnly;
	bool m_boolCurrentAnalyzerPassFile;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSyntaxEditorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void SetPathName( LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE );
	virtual void SetTitle(LPCTSTR lpszTitle);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSyntaxEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSyntaxEditorDoc)
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual void OnChangedViewList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDITEXTEDITORDOC_H__51869C8E_3587_4FB1_9C17_8ACB2424D551__INCLUDED_)
