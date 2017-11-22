/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_TREECHOOSEDLG_H__A8A3F651_446E_11D3_A3D9_00105A99059F__INCLUDED_)
#define AFX_TREECHOOSEDLG_H__A8A3F651_446E_11D3_A3D9_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeChooseDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeChooseDlg dialog

class CTreeChooseDlg : public CDialog
{
// Construction
public:
	CTreeChooseDlg(CWnd* pParent = NULL);   // standard constructor

public:
	void SetConcept(CString conceptStr, CONCEPT *concept, bool singleChoice=FALSE);

// Dialog Data
	//{{AFX_DATA(CTreeChooseDlg)
	enum { IDD = IDD_TREE_CHOOSE };
	CXTPTreeCtrl	m_TreeCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeChooseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTreeChooseDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUp();
	virtual void OnOK();
	afx_msg void OnChildren();
	afx_msg void OnUncheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_strConceptName;
	bool m_boolSingleChoice;
	CImageList m_ImageList;
	CONCEPT *m_concept;
	HTREEITEM m_itemSelected;
	HTREEITEM m_prevItemSelected;

	void FillTree();
	void SetChildrenChecked(HTREEITEM item);
	void CheckAll(HTREEITEM item, bool value);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREECHOOSEDLG_H__A8A3F651_446E_11D3_A3D9_00105A99059F__INCLUDED_)
