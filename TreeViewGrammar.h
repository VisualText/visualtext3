/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_TREEVIEWGRAMMAR_H__FE22D491_E923_11D2_A2B5_00105A99059F__INCLUDED_)
#define AFX_TREEVIEWGRAMMAR_H__FE22D491_E923_11D2_A2B5_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeViewGrammar.h : header file
//

#define GRAMCONSEP '\\'

//#define SAMPLE_ID_START 200000
//#define SAMPLE_ID_END 201000

#include "PaneView.h"

/////////////////////////////////////////////////////////////////////////////
// CTreeViewGrammar view

class CTreeViewGrammar : public CPaneView
{
public:
	CTreeViewGrammar();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTreeViewGrammar)

	CImageList*	m_pDragImage;
	BOOL		m_bLDragging;
	HTREEITEM	m_hitemDrag,m_hitemDrop;
	HCURSOR     m_dropCursor,m_noDropCursor;
	UINT    m_nTimerIDGen;
	UINT    m_nTimerIDDrag;
	UINT    m_timerticks;	
    DWORD   m_dwDragStart;

// Attributes
public:
    static CTreeViewGrammar& Get();
	virtual BOOL Create(CWnd* pParentWnd);

	CXTPTreeCtrl	m_treeCtrl;
	CImageList		m_ImageList;

// Operations
public:
	CWnd* OnCreateView();

	int TreeItemCount();
	CString GetSelection();
	void SetSelection(CString selStr);
	void AddConceptSample(CString newExampleStr, CString textPath, long start, long end,
		UINT menuID=0, CONCEPT *concept=NULL);
	void Load();
	void Clear();
	void MoveSelUp();
	void MoveSelDown();
	void SamplePopup(CWnd *wnd, CPoint point);
	void SampleViewPopup(CWnd *wnd, CPoint point, CONCEPT *concept=NULL);
	CString GetState();
	void SetState(CString stateStr);
	void GenerateAnalyzer(GRAM_GEN_TYPE type=GRAM_GEN_ALL, bool askConfirm=true);
	bool IsGenerate();
	CONCEPT *ConceptSelected();
	CONCEPT *SafeConceptSelected();
	CONCEPT *ConceptFromMenuIndex(int menuIndex);
	CONCEPT *ConceptFromName(CString conceptStr);
	void ChangeConceptName(CString newConceptNameStr);
	CString GetSelectedConcept();
	void SelectConcept(CString conceptStr);
	void RegenerateSampleFile(CONCEPT *concept, CString filePathStr);
	CONCEPT *ConceptWithAttribute(CONCEPT *concept, CString attrStr, CString valStr);
	bool DeleteSelected(bool dontAsk=false);
	void AddTopLevelConcept(CString conceptStr, bool addStub=true);
	void SelectFromStringList(CStringList &heirList);
	bool CanMoveSelectedUp();
	bool CanMoveSelectedDown();
	void GenerateSamples(HTREEITEM item=NULL);
	void DeleteStubByName(CString stubNameStr);
	void DeleteSelectedAndAnalyzerPass();
	int PassNumber();
	BOOL ForbiddenDragDropCombination(HTREEITEM hItemDrag, HTREEITEM hItemDrop);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeViewGrammar)
	public:
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeViewGrammar();
	virtual bool IsDropSource(HTREEITEM hItem);
	virtual HTREEITEM GetDropTarget(HTREEITEM hItem);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeViewGrammar)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGrammarAddconcept();
	afx_msg void OnGrammarAddtopconcept();
	afx_msg void OnGrammarEditexamples();
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGrammarDeleteconcept();
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnGrammarHelp();
	afx_msg void OnGrammarDownloadgrammar();
	afx_msg void OnGrammarUploadgrammar();
	afx_msg void OnGrammarLocalarchive();
	afx_msg void OnGrammarArchivegrammar();
	afx_msg void OnGrammarEditrules();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnGrammarEditattributes();
	afx_msg void OnUpdateGrammarAddconcept(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGrammarDeleteconcept(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGrammarArchivegrammar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGrammarUploadgrammar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGrammarEditattributes(CCmdUI* pCmdUI);
	afx_msg void OnGrammarProperties();
	afx_msg void OnGrammarDeletechildren();
	afx_msg void OnGrammarAddlist();
	afx_msg void OnGrammarDeletesamples();
	afx_msg void OnGrammarSavesamplefile();
	afx_msg void OnGrammarHighlightmatches();
	afx_msg void OnGrammarViewtree();
	afx_msg void OnGrammarMarkforgeneration();
	afx_msg void OnUpdateGrammarSavesamplefile(CCmdUI* pCmdUI);
	afx_msg void OnGrammarAssociatesamplefile();
	afx_msg void OnGrammarOpensamplefile();
	afx_msg void OnGrammarSynchronizewithfile();
	afx_msg void OnGrammarDisassociatefile();
	afx_msg void OnGrammarFind();
	afx_msg void OnGrammarAddstub();
	afx_msg void OnGrammarUpdate();
	afx_msg void OnGrammarSamplefileConsolidate();
	afx_msg void OnGrammarSamplefileOpen();
	afx_msg void OnGrammarSamplefileSet();
	afx_msg void OnGrammarSamplefileUpdate();
	afx_msg void OnGrammarAddFolder();
	afx_msg void OnGrammarAddLabel();
	afx_msg void OnGrammarAddRule();
	afx_msg void OnGrammarSamplefileExport();
	afx_msg void OnGrammarSamplefileImport();
	//}}AFX_MSG

	afx_msg void OnRButtonDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();

	DECLARE_MESSAGE_MAP()

	void FillList();
	void FillList(CONCEPT *parentConcept, HTREEITEM parentItem, int &count, bool parentHasSamples=false);

private:
	bool m_boolGenerate;
	bool m_boolDirty;
	CONCEPT * m_sampleSelectedConcept;
	HTREEITEM m_sampleSelectedItem;
	bool m_boolSampleSiblings;
	HTREEITEM m_itemSelected;
	CONCEPT *m_conceptSelected;
	GRAM_NODE_TYPE m_GrammarNodeType;
	CONCEPT *gram;
	CString m_strConcept;
	int m_intItemCount;
	int m_intPass;

	void InitVars();
	void AddConcept(CString conceptStr,HTREEITEM selectedItem=NULL, GRAM_NODE_TYPE type=GRAM_NODE_STUB);
	CString TreePath(HTREEITEM child);
	CONCEPT *GetParentConcept(HTREEITEM childItem, bool selected=false);
	void SelectUpdate();
	void SamplePopupStringList(CStringList &menuList);
	void SampleViewPopupStringList(CStringList &menuList, CONCEPT *concept);
	HTREEITEM TreeItemFromSampleMenu(int menuIndex, HTREEITEM startItem=NULL);
	void StringListFromTreeItem(CStringList &treeList, HTREEITEM treeItem);
	CONCEPT *ConceptFromStringList(CStringList &conceptList);
	CString ArchiveGrammar();
	bool IsConceptSampleRepeat(CONCEPT *sampleConcept, CString textPath, long start, long end);
	GRAM_NODE_TYPE NodeType(HTREEITEM item);
	void GrammarAddlist(CString filePathStr);
	void GrammarDeleteSamples(HTREEITEM item, bool oneLevel=false);
	void GrammarHighlightmatches(bool warning=true);
	void AddToSampleFile(CString filePathStr, CString newSampleStr, long &start, long &end);
	HTREEITEM TreeItemFromConcept(CONCEPT *concept, HTREEITEM item);
	void ChangeDocType(CString relFilePathStr, DOC_TYPE docType);
	bool TopLevelConceptExists(CString conceptStr);
	void AdjustLabelAttributesToSampleFile(CString textPathStr);
	void AdjustLabelAttributeToSampleFile(CONCEPT *concept, int newStart, int newEnd, CString textPathStr);
	bool SamplesInOneFile(HTREEITEM treeItem, CString &sampleFileStr);
	bool SamplesInOneFile(CONCEPT *concept, CString &sampleFileStr, bool topLevel, int &sampleLength);
	bool GenerateSampleFromConcept(HTREEITEM treeItem=NULL);
	bool GenerateSampleFromConcept(CONCEPT *concept, CString sampleFileStr, CStringList &sampleList);
	void GrammarDeleteSamplesRecursive(HTREEITEM item);
	void ImageFromType(GRAM_NODE_TYPE type, int &image, int &imageSel);
	CString TypeString(GRAM_NODE_TYPE type);
	GRAM_NODE_TYPE TypeFromString(CString typeStr);
	HTREEITEM TreeItemFromNumber(CXTPTreeCtrl *treeCtrl, const HTREEITEM &currentItem, int &count,
		const int nth, const bool includeSiblingsFlag=true, bool bAddingSamples=false);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEVIEWGRAMMAR_H__FE22D491_E923_11D2_A2B5_00105A99059F__INCLUDED_)
