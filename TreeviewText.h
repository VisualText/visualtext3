/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_TREEVIEWTEXT_H__55A9CEF1_E795_11D2_A2AD_00105A99059F__INCLUDED_)
#define AFX_TREEVIEWTEXT_H__55A9CEF1_E795_11D2_A2AD_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeViewText.h : header file
//

#include "PaneView.h"

/////////////////////////////////////////////////////////////////////////////
// CTreeViewText view

enum TEXT_TREE_TYPE {
	TEXT_TREE_UNKNOWN = 0,
	TEXT_TREE_FOLDER,
	TEXT_TREE_TEXT,
	TEXT_TREE_HTML,
	TEXT_TREE_LOG,
};

class CTreeViewText : public CPaneView
{
public:
	CTreeViewText();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTreeViewText)

	CImageList*	m_pDragImage;

// Attributes
public:
	virtual BOOL Create(CWnd* pParentWnd);

	CXTPTreeCtrl	m_treeCtrl;

	CImageList m_imageList;
	BOOL m_bRedrawKB;
	CStringArray m_strURLs;
	CStringArray m_strNames;

// Operations
public:
	CWnd* OnCreateView();

	HTREEITEM m_hitemDrop;
	void Load();
	void Clear();
	void SelectText(CDocument *doc);
	bool SelectTextFromPath(CString relPathStr);
	CString AddTextFile(CString fileNameStr, CString contentStr, CString extStr=_T(""), bool updateDisplay=true);
	CString GetSelection();
	void SetSelection(CString selStr);
	CString GetState();
	void SetState(CString stateStr);
	CString NextSlog(bool backward=false);
	void SelectSlog();
	void UpdateTextTreeItem(HTREEITEM treeItem);
	void SetToReslog();
	void Reload(CString fullPathStr);
	void DeleteSelected();
	bool HasAnaLogFiles(CString filePathStr);
	void RetypeItem(CString fullPathStr);
	void PrevText();
	void NextText();
	bool OpenProcessedFiles(CString extStr=_T(""), bool openFiles=true, bool excludeOutputTxtFlag=false);
	bool OpenOutputFile();
	void ProcessedFiles(CStringList &processedFiles, CString extStr, CString processPathStr=_T(""));
	CString ProcessedFileStr(CString extStr, CString processPathStr=_T(""));
	void SetPageModeState(CString stateStr, bool appendFlag=true);
	CString GetPageModeState();
	CString GetPageModeStateStr();
	void RestorePageModeState(CString stateStr, bool clearFirst=false);
	void SetTextAsAnalyzed(CString fullPathStr);
	void MoveLogFiles(CString pathStr);
	bool HasNextText();
	int TreeItemCount();
	bool IsSelectedEmptyFolder();
	TEXT_TREE_TYPE GetTextTreeType();
	void AddNewText(CString filePathStr, CString contentStr=_T(""), bool topLevelFlag=false);
	void Refresh(void);
	void AddWordsFromFile(LPCTSTR lpPath, int level);
	void RunDictionaryLookupOnSelection(LPCTSTR lpURL=NULL, LPCTSTR lpName=NULL);
	CONCEPT *ConceptFromTextPath(CString pathStr, bool fromGuiFlag=false);
	bool CanMoveSelectedDown();
	bool CanMoveSelectedUp();
	void MoveSelDown();
	void MoveSelUp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeViewText)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeViewText();
	virtual bool IsDropSource(HTREEITEM hItem);
	virtual HTREEITEM GetDropTarget(HTREEITEM hItem);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeViewText)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDirtreemenuAddexistingtext();
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDirtreemenuNewtext();
	afx_msg void OnDirtreemenuAddfolder();
	afx_msg void OnDirtreemenuMovetotoplevel();
	afx_msg void OnDirtreemenuDelete();
	afx_msg void OnDirtreemenuDeletechildren();
	afx_msg void OnDirtreemenuEditview();
	afx_msg void OnDirtreemenuRename();
	afx_msg void OnDirtreemenuGotowebpage();
	afx_msg void OnDirtreemenuUpdatewebpage();
	afx_msg void OnDirtreemenuViewaswebpage();
	afx_msg void OnDirtreemenuViewoutputfile();
	afx_msg void OnDirtreemenuRunanalyzer();
	afx_msg void OnDirtreemenuHelp();
	afx_msg void OnUpdateDirtreemenuAddexistingtext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDirtreemenuViewaswebpage(CCmdUI* pCmdUI);
	afx_msg void OnDirtreemenuServerarchives();
	afx_msg void OnDirtreemenuUploadtext();
	afx_msg void OnDirtreemenuLocalarchive();
	afx_msg void OnUpdateDirtreemenuMovetotoplevel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDirtreemenuDeletechildren(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDirtreemenuEditview(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDirtreemenuRename(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDirtreemenuUpdatewebpage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDirtreemenuArchivetext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDirtreemenuUploadtext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDirtreemenuRunanalyzer(CCmdUI* pCmdUI);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDirtreemenuDeletelogs();
	afx_msg void OnDirtreemenuRefresh();
	afx_msg void OnDirtreemenuMask();
	afx_msg void OnDirtreemenuDictionize();
	afx_msg void OnDirtreemenuToolsWordnet();
	afx_msg void OnDirtreemenuTextfileinfo();
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDirtreemenuToolsHexviewer();
	afx_msg void OnDirtreemenuToolsAsciitable();
	afx_msg void OnDirtreemenuToolsCharacterviewer();
	afx_msg void OnDirtreemenuToolsEmail();
	afx_msg void OnDirtreemenuAddToplevelExistingtextfile();
	afx_msg void OnDirtreemenuAddToplevelNewtextfile();
	afx_msg void OnDirtreemenuAddToplevelFolder();
	afx_msg void OnDirtreemenuArchivetext();
	afx_msg void OnUpdateDirtreemenuDelete(CCmdUI* pCmdUI);
	afx_msg void OnDirtreemenuAttributes();
	afx_msg void OnUpdateDirtreemenuAttributes(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDirtreemenuToolsJumble();
	afx_msg void OnDirtreemenuDictionaryAddwords();
	//}}AFX_MSG
	afx_msg void OnRButtonDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDictionaryURLLookup(UINT uID);
	DECLARE_MESSAGE_MAP()
		
private:
	bool m_boolKBBased;
	bool m_boolLeftButton;
	HTREEITEM m_treeItemSlog;
	DWORD m_dwDragStart;
	UINT m_timerticks;
	UINT m_nTimerIDDrag;
	HCURSOR m_noDropCursor;
	HCURSOR m_dropCursor;
	HTREEITEM m_hitemDrag;
	bool m_bLDragging;
	bool m_boolBlockComboAdd;
	bool m_bFillingTree;
	HTREEITEM m_itemSelected;
	HTREEITEM m_itemLastSelected;
	HTREEITEM m_lastFolderSelected;
	HTREEITEM m_itemToAddTo;
	TEXT_TREE_TYPE m_textTreeType;
	bool m_boolProcessedText;
	bool m_boolFillOneLevel;
	bool m_boolHasFolder;
	bool m_boolSelectedIsLogFile;
	bool m_boolFolderEmpty;
	CStringList m_stringListStates;

	CONCEPT *m_conceptText;
	CONCEPT *m_conceptSelected;
	CONCEPT *m_conceptParent;
	CONCEPT *m_conceptFolder;
	BOOL m_bOpeningTextFile;

	void InitVars();
	CString CurrentTreeSelection(bool fullPathFlag, bool addingToFlag=false);
	CString TreePath(HTREEITEM child, bool noCheck=false);
	CString TreePath();
	bool IsChecked(HTREEITEM inItem, bool parentFlag=false);
	void SetTextTreeType();
	TEXT_TREE_TYPE TextTreeType(HTREEITEM item);
	void FillInputTree();
	void FillInputTreeRecurse(CONCEPT *concept, HTREEITEM item=NULL);
	void FillKBRecurse(CString pathStr=_T(""), CONCEPT *conceptParent=NULL);
	void FillInputTreeRecurse(CString pathStr, HTREEITEM hParent=NULL);
	void OpenDirectoryTreeFile();
	CString ArchiveText();
	HTREEITEM TreeItemFromPath(CString relOrFullPathStr, bool parentFlag=false);
	void RetypeItem(HTREEITEM item, CString fullPathStr=_T(""));
	bool PrevTextFromPath(CString currentPathStr);
	bool NextTextFromPath(CString currentPathStr);
	void MoveToProcessedText(bool changeBrowser=false);
	void DeleteClearIfNone();
	HTREEITEM InsertItemAlphabetically(CString fullPathStr, HTREEITEM parentItem=NULL,
		bool dontOpenFlag=false, bool selectNewItemFlag=true);
	void FillTreeExpand(HTREEITEM treeItem);
	void AddNewText(bool topLevelFlag=false);
	void AddExistingTextFile(bool topLevelFlag=false);
	void AddExistingFolder(CString folderPathStr, bool topLevelFlag=false);
	void AddFolder(bool topLevelFlag=false);
	void DeleteTreeItemChild(HTREEITEM item=NULL);
	CString CurrentTreeSelectionDir(bool fullPathFlag, bool addingToFlag=false);
	CString GetItemState(HTREEITEM item);
	void SetItemState(HTREEITEM item, CString stateStr);
	void DeleteTreeItemRecursive(HTREEITEM item=NULL, bool parentProcessed=false);
	void ImageTypeFromFile(CString fullPathStr, int &image, int &imageSel, TEXT_TREE_TYPE &type);
	void ImagesFromType(TEXT_TREE_TYPE type, bool processed, int &image, int &imageSel);
	void InitKB();
	void RecurseRemoveLogs(CString pathStr, bool displayMsgFlag=true);
	bool DictionaryLookupRecurse(HTREEITEM item, LPCTSTR lpURL=NULL, LPCTSTR lpName=NULL);
	void DeleteChildren(CString fullPathStr=_T(""), bool askToConfirmFlag=true);
public:
	afx_msg void OnAddExistingfolder();
	afx_msg void OnDirtreemenuExploredirectory();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEVIEWTEXT_H__55A9CEF1_E795_11D2_A2AD_00105A99059F__INCLUDED_)
