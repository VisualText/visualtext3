/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_PARSETREEVIEW_H__648B4551_C6F3_11D2_8C4C_00105A99059F__INCLUDED_)
#define AFX_PARSETREEVIEW_H__648B4551_C6F3_11D2_8C4C_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParseTreeView.h : header file
//

#define PARSE_TREE_LINE_MAX 250

/////////////////////////////////////////////////////////////////////////////
// CParseTreeView view

#include <afxcview.h>
#include "ParseTreeItemInfo.h"

class CParseTreeView : public CXTPTreeView
{
protected:
	CParseTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CParseTreeView)

public:
	//{{AFX_DATA(ParseTreeView)
	enum { IDD = IDD_PARSETREE };
	//}}AFX_DATA

	// Attributes
public:
	UINT		m_SemPopupTimerID;
	HTREEITEM	m_SemPopupItem;
	CWnd*		m_pSemPopup;

// Operations
public:
	void ChangeData(CString logPathStr, CString textPathStr);
	bool SetData(CString logPathStr, CString textPathStr);
	bool SetData(CString logPathStr, CString textPathStr, CString conceptStr,
		long startSel=-1, long endSel=-1, bool appendFlag=false);
	void ExpandOneLevel();
	void Redisplay();
	CString GetLogPathStr();
	void SetState(CString stateStr, STATE_TYPE state);
	CString GetFancyTitle();
	void SetConceptOnlyFlag(bool conceptsOnlyFlag);
	CString StateStr();
	CString LogFilePiece(long start, long end);
	void FillParseTree(bool appendFlag=false);
	void GetToolTipTextInfo(CString& strLine, CString& strText, CString& strSem,
		CString& lineSem);	// 06/27/02 AM.
	void SaveState();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParseTreeView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CParseTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CParseTreeView)
	afx_msg void OnParsetreeOpenall();
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnParsetreeCloseall();
	afx_msg void OnParsetreeOpenitemall();
	afx_msg void OnParsetreeMakeitemroot();
	afx_msg void OnParsetreeDisplayentiretree();
	afx_msg void OnParsetreeOpenonelevel();
	afx_msg void OnParsetreeFind();
	afx_msg void OnParsetreeHighlighttext();
	afx_msg void OnDestroy();
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnParsetreeEditdictionaryentry();
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFileNew();
	afx_msg void OnUpdateParsetreeDisplayentiretree(CCmdUI* pCmdUI);
	afx_msg void OnUpdateParsetreeOpenitemall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateParsetreeMakeitemroot(CCmdUI* pCmdUI);
	afx_msg void OnUpdateParsetreeHighlighttext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateParsetreeFind(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnParsetree();
	afx_msg void OnUpdateParsetree(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnDestroySemPopup(WPARAM wParam, LPARAM lParam);
	BOOL OnToolTipNeedText(UINT id, NMHDR * pNMHDR, LRESULT * pResult);
	DECLARE_MESSAGE_MAP()

private:
	bool m_boolProgramExpanding;
	CImageList	m_ImageList;
	CXTPTreeCtrl m_treeCtrl;
	HTREEITEM m_itemRoot;
	HTREEITEM m_itemCurrentRoot;
	HTREEITEM m_itemSelected;
	HTREEITEM m_itemParent;
	HICON m_icon;

    CParseTreeItemInfo *m_itemInfoRoot;
	CParseTreeItemInfo *m_lastItemInfo;
	CImageList m_imageList;
	CString m_strRootConcept;
	CString m_strConcept;
	CString m_strLogPath;
	CString m_strLogText;
	CString m_strTextPath;
	long m_longStart;
	long m_longEnd;
	long m_longFileLength;
	int m_intLevel;
//	bool m_boolPrettyText;	// 07/11/08 AM.
	bool m_boolConceptsOnly;
	bool m_boolFillOneLevel;
	bool m_boolFillingTree;
	bool m_boolEntireTree;

	void DeleteItemData();
	CParseTreeItemInfo *GetItemDataFromItem(HTREEITEM treeItem);
	CString OffsetDataStr(int startSel, int endSel);
	void MakeTreeItemRoot();
	void ExpandAllSiblings(HTREEITEM item, UINT nCode);
	void ExpandAll(HTREEITEM item, UINT nCode);
	void InitVars();
	void ViewRuleFired();

	inline void CParseTreeView::FormatTreeItemText(			// 07/12/08 AM.
		CString &lineStr,
		CString conStr,
		long startSel,
		long endSel,
		CString rangeStr,
		CString semStr,
		bool bTreeSem,	// If tree semantics
		bool bTreeOff,	// If tree offsets
		bool bConceptNode,	// If nonliteral node ("concept node")
		bool bPrettyText	// If getting text from tree rather than input buffer
		);

	inline void MakeTreeItemText(CString &cp_lineStr,CString lineStr,CString rangeStr,
		bool bConceptNode,bool bPrettyText,HTREEITEM hParents[],int indent);	// 07/11/08 AM.

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARSETREEVIEW_H__648B4551_C6F3_11D2_8C4C_00105A99059F__INCLUDED_)
