#if !defined(AFX_WORKSPACEBARCLASSTREE_H__AAB90FCA_45D4_46FE_BAD0_96F6FD7991F6__INCLUDED_)
#define AFX_WORKSPACEBARCLASSTREE_H__AAB90FCA_45D4_46FE_BAD0_96F6FD7991F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkspaceBarClassTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBarClassTree window

class CWorkspaceBarClassTree : public CXTTreeCtrl
{
	DECLARE_DYNAMIC(CWorkspaceBarClassTree)

// Construction
	CWorkspaceBarClassTree();

// Attributes
protected:
	CImageList	m_imageList;

// Operations
public:

    static CWorkspaceBarClassTree& Get();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkspaceBarClassTree)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorkspaceBarClassTree();
	virtual BOOL Create(CWnd* pParentWnd);

	// Generated message map functions
protected:
	//{{AFX_MSG(CWorkspaceBarClassTree)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKSPACEBARCLASSTREE_H__AAB90FCA_45D4_46FE_BAD0_96F6FD7991F6__INCLUDED_)
