/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_WORKSPACEVIEW_H__6EA5D5E3_1954_4CFA_BE4B_3C22B10D8DBF__INCLUDED_)
#define AFX_WORKSPACEVIEW_H__6EA5D5E3_1954_4CFA_BE4B_3C22B10D8DBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkspaceView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChildFrameHook:
//
/*
class CWorkspaceView;
class CChildFrameHook : public CXTWndHook
{
public:
	virtual BOOL Install(CWnd* pParentWnd, CWnd* pWndToHook, CWorkspaceView* pTabView);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	CWnd*			m_pParentWnd;
	CWorkspaceView* m_pTabView;
};
*/

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceView:
//
class CMainFrame;
class CWorkspaceView : public CXTPTabView
{
protected: // create from serialization only
	CWorkspaceView();
	DECLARE_DYNCREATE(CWorkspaceView)

// Attributes
public:

// Operations
public:

	int				m_iHitTest;
	CImageList		m_imageList;
	//CChildFrameHook m_hookFrame;

	CWnd* CreatePane(CWnd* pParentWnd);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkspaceView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorkspaceView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	void OnSelChanging();
	void OnSelChange();
//	void HideShowTabView(BOOL& bTabView, CView* pView, LPCTSTR lpszText, int iIndex, int iIconIndex);

// Generated message map functions
protected:
	//{{AFX_MSG(CWorkspaceView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnTabViewUpdate();
	DECLARE_MESSAGE_MAP()

	friend class CMainFrame;
public:
	afx_msg void OnDestroy();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKSPACEVIEW_H__6EA5D5E3_1954_4CFA_BE4B_3C22B10D8DBF__INCLUDED_)
