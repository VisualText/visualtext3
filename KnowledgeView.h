/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_KNOWLEDGEVIEW_H__1C09E190_C22C_11D3_A559_00105A99059F__INCLUDED_)
#define AFX_KNOWLEDGEVIEW_H__1C09E190_C22C_11D3_A559_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KnowledgeView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKnowledgeView view

class CKnowledgeView : public CXTPTreeView
{
protected:
	CKnowledgeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CKnowledgeView)

	CImageList*	m_pDragImage;
	BOOL		m_bLDragging;
	HTREEITEM	m_hitemDrag,m_hitemDrop;
	HCURSOR     m_dropCursor,m_noDropCursor;
	UINT    m_nTimerIDDrag;
	UINT    m_timerticks;	
    DWORD   m_dwDragStart;

// Attributes
public:
	void MoveSelUp();
	void MoveSelDown();
	bool CanMoveUp();
	bool CanMoveDown();
	void SetState(CString stateStr, STATE_TYPE state);
	void SetData(CString conceptPathStr, bool oneRootNode);
	void Restore();
	bool SelectFromConcept(CONCEPT *concept, bool updateAttributeEditorFlag=true);
	HTREEITEM TreeItemFromConcept(CONCEPT *concept);
	void ReIcon(CONCEPT *concept);
	void SaveState();
	BOOL InKBSafeEditMode(void);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKnowledgeView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CKnowledgeView();
	virtual bool IsDropSource(HTREEITEM hItem);
	virtual HTREEITEM GetDropTarget(HTREEITEM hItem);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CKnowledgeView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKnowledgeAttributes();
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKnowledgeDelete();
	afx_msg void OnKnowledgeDeletechildren();
	afx_msg void OnKnowledgeAddconcepts();
	afx_msg void OnKnowledgeFind();
	afx_msg void OnKnowledgePhrases();
	afx_msg void OnKnowledgeAddphrase();
	afx_msg void OnKnowledgeMakeroot();
	afx_msg void OnDestroy();
	afx_msg void OnKnowledgeGenerateall();
	afx_msg void OnKnowledgeSave();
	afx_msg void OnSaveKb();
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKbDown();
	afx_msg void OnKbUp();
	afx_msg void OnUpdateKbUp(CCmdUI* pCmdUI);
	afx_msg void OnUpdateKbDown(CCmdUI* pCmdUI);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKnowledgeUp();
	afx_msg void OnKnowledgeKbmodularLoad();
	afx_msg void OnKnowledgeKbmodularSave();
	//}}AFX_MSG
	afx_msg long OnSelectNextItem(WPARAM wParam, LPARAM lParam);
	afx_msg long OnSelectPrevItem(WPARAM wParam, LPARAM lParam);
	afx_msg long OnSelectDownItem(WPARAM wParam, LPARAM lParam);
	afx_msg long OnSelectUpItem(WPARAM wParam, LPARAM lParam);
	afx_msg long OnOpenPath(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	CImageList m_ImageList;
	HTREEITEM m_itemSelected;
	CONCEPT *m_conceptRoot;
	CONCEPT *m_conceptSelected;
	CString m_strConcept;
	CString m_strFind;
	CString m_strSelected;
	int m_intItemCount;
	HICON m_icon;
	bool m_boolOneRootNode;
	bool m_boolFillOneLevel;
	bool m_boolUpdateAttributeEditor;

	void InitVars();
	void FillListExpand(HTREEITEM treeItem);
	void FillList(bool deleteFirst=false);
	bool FillList(CONCEPT *parentConcept, HTREEITEM parentItem, int level, int &count);
	void DeleteSelected();
	void AddConcept(CString conceptStr, HTREEITEM itemSelected);
	void ForceSaveKB();
	CString StateStr();
	HTREEITEM TreeItemFromConcept(CONCEPT *concepts[], HTREEITEM item, int level);
	void IconsFromConcept(CONCEPT *concept, int &icon, int &iconSel);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KNOWLEDGEVIEW_H__1C09E190_C22C_11D3_A559_00105A99059F__INCLUDED_)
