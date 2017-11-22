/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_TREEVIEWOUTPUT_H__9D9BA240_3DF7_11D3_A3C6_00105A99059F__INCLUDED_)
#define AFX_TREEVIEWOUTPUT_H__9D9BA240_3DF7_11D3_A3C6_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeViewOutput.h : header file
//

enum OUTPUT_NODE_TYPE {
	OUTPUT_NODE_RECORD = 0,
	OUTPUT_NODE_ITEM
};

#define OUTPUTCONSEP '\\'

/////////////////////////////////////////////////////////////////////////////
// CTreeViewOutput view

class CTreeViewOutput : public CXTPTreeView
{
public:
	CTreeViewOutput();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTreeViewOutput)

	CImageList*	m_pDragImage;
	BOOL		m_bLDragging;
	HTREEITEM	m_hitemDrag,m_hitemDrop;
	HCURSOR     m_dropCursor,m_noDropCursor;
	UINT    m_nTimerID;
	UINT    m_timerticks;	
    DWORD   m_dwDragStart;

// Attributes
public:
	CXTPTreeCtrl	m_treeCtrl;
	CImageList		m_ImageList;

// Operations
public:
	CString GetSelection();
	void SetSelection(CString selStr);
	void Load();
	void Clear();
	void MoveSelUp();
	void MoveSelDown();
	void SamplePopup(CWnd *wnd, CPoint point);
	CString GetState();
	void SetState(CString stateStr);
	void GenerateAnalyzer(NLP *nlp);
	bool IsGenerate();
	void SelectUpdate();
	CONCEPT *GetParentConcept(HTREEITEM childItem, bool selected);
	void ChangeRecordName(CString newConceptNameStr);
	void AddItemsFromStateStr(CString stateStr);
	void DeleteSelected();
	void ConvertTextToOutput(CString outputPathStr);
	void OutputPopup(CWnd *wnd, CPoint point);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeViewOutput)
	public:
	virtual void OnInitialUpdate(); // SRP 010207
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeViewOutput();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeViewOutput)
	afx_msg void OnOutputmenuAddoutput();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnOutputmenuArchiveoutput();
	afx_msg void OnOutputmenuLocalarchive();
	afx_msg void OnOutputmenuServerarchive();
	afx_msg void OnOutputmenuUploadoutput();
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOutputmenuAdditem();
	afx_msg void OnOutputmenuDelete();
	afx_msg void OnOutputmenuEditattributes();
	afx_msg void OnUpdateOutputmenuAdditem(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOutputmenuDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOutputmenuEditattributes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOutputmenuArchiveoutput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOutputmenuUploadoutput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOutputmenuAddoutput(CCmdUI* pCmdUI);
	afx_msg void OnOutputmenuProperties();
	afx_msg void OnUpdateOutputmenuProperties(CCmdUI* pCmdUI);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int FillList();
	int FillList(CONCEPT *parentConcept, HTREEITEM parentItem);

private:
	HTREEITEM m_itemSelected;
	CONCEPT *m_conceptSelected;
	OUTPUT_NODE_TYPE m_NodeType;
	CONCEPT *m_conceptOutput;
	CString m_strConcept;

	void InitVars();
	CONCEPT *AddField(CONCEPT *recordConcept, CString fieldNameStr);
	CONCEPT *AddConcept(CString conceptStr,HTREEITEM m_itemSelected=NULL, bool isItem=false);
	CString TreePath(HTREEITEM child);
	CString Archive();
	void GrammarPopupStringList(CStringList &menuList);
	void OnItemAdd(UINT idIn);
	void AddConceptItem(UINT menuID);
	HTREEITEM TreeItemFromConcept(CONCEPT *concept, HTREEITEM item=NULL);
	void OutputPopupStringList(CStringList &menuList);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEVIEWOUTPUT_H__9D9BA240_3DF7_11D3_A3C6_00105A99059F__INCLUDED_)
