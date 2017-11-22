/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// BrowserView.h : interface of the CBrowserView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BROWSERVIEW_H__03B34D41_ABE0_11D2_8BD2_00105A99059F__INCLUDED_)
#define AFX_BROWSERVIEW_H__03B34D41_ABE0_11D2_8BD2_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxhtml.h>

class CBrowserView : public CHtmlView
{
protected: // create from serialization only
	CBrowserView();
	DECLARE_DYNCREATE(CBrowserView)

// Attributes
public:
	CBrowserDoc* GetDocument();
	void FetchURLText();
	CString StateStr();
	void SaveState();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowserView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnNavigateComplete2(LPCTSTR strURL);
	virtual void OnTitleChange(LPCTSTR lpszText);
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBrowserView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CBrowserView)
	afx_msg void OnBackUrl();
	afx_msg void OnOptionsBack();
	afx_msg void OnOptionsForward();
	afx_msg void OnOptionsStop();
	afx_msg void OnUrlStop();
	afx_msg void OnForwardUrl();
	afx_msg void OnClearUrl();
	afx_msg void OnAddUrl();
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnRunHtml();
	afx_msg void OnSpider();
	afx_msg void OnUpdateSpider(CCmdUI* pCmdUI);
	afx_msg void OnFileNew();
	afx_msg void OnNewBrowser();
	afx_msg void OnUpdateBackUrl(CCmdUI* pCmdUI);
	afx_msg void OnUpdateForwardUrl(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	HICON m_icon;

	void GoUrl();
	void MergeDumpFiles(CString urlPathStr, CString homePagePathStr, CString spideredPathStr, CStringList &levelUrls, int depth);
	CString DumpDirectory(CString filePathStr);
	bool MergeUrlAndAnchor(CString urlStr, CString anchorStr, CString &newUrlStr);
	void MergeUrlsWithAnchors(CStringList &newList, CStringList &anchorList, CString urlPathStr);
};

#ifndef _DEBUG  // debug version in BrowserView.cpp
inline CBrowserDoc* CBrowserView::GetDocument()
   { return (CBrowserDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BROWSERVIEW_H__03B34D41_ABE0_11D2_8BD2_00105A99059F__INCLUDED_)
