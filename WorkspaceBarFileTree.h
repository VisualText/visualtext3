#if !defined(AFX_TABTREECTRL_H__8474628B_4E97_46B1_BEA8_E679171AA83B__INCLUDED_)
#define AFX_TABTREECTRL_H__8474628B_4E97_46B1_BEA8_E679171AA83B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkspaceBarFileTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBarFileTree window

class CWorkspaceBarFileTree : public CXTPTreeCtrl
{
	DECLARE_DYNAMIC(CWorkspaceBarFileTree)

// Construction
	CWorkspaceBarFileTree();

// Attributes
protected:
	CImageList	m_imageList;

// Operations
public:

    static CWorkspaceBarFileTree& Get();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkspaceBarFileTree)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorkspaceBarFileTree();
	virtual BOOL Create(CWnd* pParentWnd);

	// Generated message map functions
protected:
	//{{AFX_MSG(CWorkspaceBarFileTree)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABTREECTRL_H__8474628B_4E97_46B1_BEA8_E679171AA83B__INCLUDED_)
