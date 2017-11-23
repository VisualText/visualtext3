#if !defined(AFX_WORKSPACEBARRESTREE_H__3B27A34B_BE80_4346_99B5_18B8C60D074A__INCLUDED_)
#define AFX_WORKSPACEBARRESTREE_H__3B27A34B_BE80_4346_99B5_18B8C60D074A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkspaceBarResTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBarResTree window

class CWorkspaceBarResTree : public CXTTreeCtrl
{
	DECLARE_DYNAMIC(CWorkspaceBarResTree)

// Construction
	CWorkspaceBarResTree();

// Attributes
protected:
	CImageList	m_imageList;

// Operations
public:

    static CWorkspaceBarResTree& Get();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkspaceBarResTree)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorkspaceBarResTree();
	virtual BOOL Create(CWnd* pParentWnd);

	// Generated message map functions
protected:
	//{{AFX_MSG(CWorkspaceBarResTree)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKSPACEBARRESTREE_H__3B27A34B_BE80_4346_99B5_18B8C60D074A__INCLUDED_)
