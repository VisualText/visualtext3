/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_TREEVIEWANALYZER_H__6138FE01_E6D1_11D2_A2AA_00105A99059F__INCLUDED_)
#define AFX_TREEVIEWANALYZER_H__6138FE01_E6D1_11D2_A2AA_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeViewAnalyzer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeViewAnalyzer view

#include <afxcview.h>
#include "lite/nlp.h"
#include "PaneView.h"

#define DRAG_X_ADJUST 15
#define DRAG_Y_ADJUST 40
#define DRAG_DELAY 80

#define SPEC_STARTID 800300
#define SPEC_ENDID 800399
#define SPEC_2_STARTID 800400
#define SPEC_2_ENDID 800499

enum MOVE_DESTINATION {
	MOVE_NOWHERE = 0,
	MOVE_BELOW,
	MOVE_ABOVE,
	MOVE_INTO,
	MOVE_PAST_FOLDER
};

class CTreeViewAnalyzer : public CPaneView
{
public:
	CTreeViewAnalyzer();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTreeViewAnalyzer)
	void SetPassDirty(CString rulePathStr=_T(""));
	HTREEITEM SelectPass(int pass);
	CString TreePath(HTREEITEM child);

	CImageList*	m_pDragImage;

	CXTPTreeCtrl m_treeCtrl;

	// Attributes
public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CImageList m_ImageList;

	CString m_strStatus;
	CString m_strAlgo;
	CString m_strData;
	CString m_strComments;
	int m_intPass;
	CString m_strLastFolderAddPassFromFile;
	HANDLE m_AnalyzeThreadID;
	CWinThread* m_AnalyzeThread;
	BOOL m_fTerminateAnalyzeThread;
	BOOL m_fSuspendAnalyzeThread;
	
	VT_APP_STATE m_prevAppState;
	DWORD m_dwTimeStartFile;
	CString m_AnalyzePathStr;
	CString m_AnalyzeOutputFinalMsgStr;
	CStringArray m_AnalyzeOutputStrDirs[2];
	CStringArray m_AnalyzeOutputStrFiles[2];
	CStringArray m_AnalyzeOutputStrFilesTime;
	int m_AnalyzeFileIndex;
	int m_AnalyzePass;
	CStringArray m_AnalyzeFiles;

// Operations
public:
	CWnd* OnCreateView();

	void Load();
	void Refresh();
	CDocument *GetLogFromDocument(CDocument *doc);
	void RunAnalyzer(CString filePathStr, int level);
	void CollectFilesToAnalyze(const CString& filePathStr, int level);
	CDocument *ViewPassLog(int pass=-1, bool selectFlag=true);
	int PassFromDocument(CDocument *doc);
	CString CleanPassNameFromPass(int pass);
	CString GetSelection();
	void SetSelection(CString selStr);
	void ViewFullTree(CString logFilePathStr);
	void Clear();
	void MoveSelUp();
	void MoveSelDown();
	void Save(bool askUserFlag=false);
	bool IsAnalyzerFileDirty();
	void SetAnalyzerFileDirty(bool);	// 06/24/03 AM.
	CString GetState();
	void SetState(CString stateStr);
	int PassFromFile(CString fileName);
	void HighlightMatches();
	void HighlightMatches(CONCEPT *concept);
	void HighlightMatches(int pass);
	bool GetLogFile(CString &logPathStr, bool dontAsk);
	bool GetLogFile(CString &logPathStr);
	bool GetLogFile(int pass, CString &logPathStr, bool force=false, bool dontAsk=false);
	int SelectPassFromConcept(CONCEPT *concept);
	bool CanMoveSelectedUp();
	bool CanMoveSelectedDown();
	void DeleteSelected(bool dontConfirm=false);
	int SelectedPass();
	CString GetFinalLogFile();
	void AddStub(CString stubNameStr, bool addAfterFlag=false);
	bool RenamePass(CONCEPT *concept, CString newNameStr);
	void RetypePass(CONCEPT *concept, ANAL_PASS_TYPE type);
	void OpenSelectedRulesFile();
	bool CanDelete();
	static	// 06/14/03 AM.
		ANAL_PASS_TYPE ConceptType(CONCEPT *concept);
	CString LogFileRule(CString textPathStr, long start, long end);
	void AppendToRuleFile(CString ruleStr);
	void DeleteStubByName(CString stubNameStr);
	CONCEPT *ConceptFromName(CString nameStr);
	CString PassFileFromNumber(int passNum);
	void SetPassActive(CONCEPT *concept, bool activeFlag);
	int GetPassType(int pass);
	void HighlightRuleMatches(CStringList &selList, CString logPathStr, CString conceptStr=_T(""));
	void KillTimers(void);
	void DispatchAnalyzeThread(void);
	void EndAnalyzeThread(void);
	CDocument *EditRulesFile(LPCTSTR lpRulesPath = NULL);
	void GetPassInfo(int passNum=-1);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeViewAnalyzer)
	public:
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeViewAnalyzer();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeViewAnalyzer)
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSequencemenuAddexistingrules();
	afx_msg void OnSequencemenuDelete();
	afx_msg void OnSequencemenuDeletepasses();
	afx_msg void OnSequencemenuAttributes();
	afx_msg void OnSequencemenuDuplicate();
	afx_msg void OnSequencemenuNew();
	afx_msg void OnSequencemenuEditrules();
	afx_msg void OnSequencemenuEditview();
	afx_msg void OnSequencemenuViewfulltree();
	afx_msg void OnSequencemenuHelp();
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSequencemenuViewlog();
	afx_msg void OnSequencemenuServerarchives();
	afx_msg void OnSequencemenuUploadrules();
	afx_msg void OnSequencemenuArchiverules();
	afx_msg void OnSequencemenuLocalarchives();
	afx_msg void OnUpdateSequencemenuDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSequencemenuEditrules(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSequencemenuViewlog(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSequencemenuViewfulltree(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSequencemenuArchiverules(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSequencemenuUploadrules(CCmdUI* pCmdUI);
	afx_msg void OnSequencemenuHighlightmatches();
	afx_msg void OnSequencemenuViewentiretree();
	afx_msg void OnSequencemenuProperties();
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSequencemenuViewtreematches();
	afx_msg void OnSequencemenuViewconcepttree();
	afx_msg void OnSequencemenuAddFolder();
	afx_msg void OnSequencemenuAddMain();
	afx_msg void OnSequencemenuAddToplevelExistingpass();
	afx_msg void OnSequencemenuAddToplevelMain();
	afx_msg void OnSequencemenuAddToplevelNew();
	afx_msg void OnSequencemenuAddToplevelFolder();
	afx_msg void OnSequencemenuAddStub();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDelete();
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();

	//}}AFX_MSG
	afx_msg LRESULT OnAnalyzeOutputMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAnalyzeOutputPassMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPostAnalyzeFileTasks(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAnalyzeCleanup(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAnalyzeHighlight(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChooseSpec(UINT idIn);
	afx_msg void OnChooseSpec2(UINT idIn);
	afx_msg void OnRButtonDown(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
	
private:
	bool m_boolActive;
	bool m_boolKBBased;
	CONCEPT *m_conceptSpec;
	CONCEPT *m_conceptSelected;
	ANAL_PASS_TYPE m_passType;
	HTREEITEM m_itemSelected;
	HTREEITEM m_itemParent;
	bool m_bFillingTree;
	bool m_dirtyFileFlag;
	bool m_dirtyFlag;
	bool m_boolInStub;
	bool m_boolTopLevel;
	int m_intConcept;

	DWORD m_dwDragStart;
	UINT m_timerticks;
	UINT m_nTimerIDDrag;
	HCURSOR m_noDropCursor;
	HCURSOR m_dropCursor;
	HTREEITEM m_hitemDrag;
	HTREEITEM m_hitemDrop;
	bool m_bLDragging;
	MOVE_DESTINATION m_destinationType;

	bool IsDropSource(HTREEITEM hItem);
	HTREEITEM GetDropTarget(HTREEITEM hItem);
	void FillListWithSequence();
	void InitAnalyzer();
	void RemovePassOrder(CString &passNameStr);
	CONCEPT *InsertPass(CString algoStr, CString dataStr,CString commentStr,
						bool active, bool treeInsert=true, bool editLabelFlag=true, bool renumberFlag=true);
	void ProcessLog(CString fileName);
	void MoveLogFiles(CString fileName, CString logDir);
	void MovePass(bool down=false);
	CString ArchiveAnalyzer();
	void ImageType(CString algoStr, int &image, int &imageSelected, ANAL_PASS_TYPE &type);
	void ImageFromType(ANAL_PASS_TYPE type, bool activeFlag, int &image, int &imageSelected);
	void ImageFromConcept(CONCEPT *concept, int &image, int &imageSelected, ANAL_PASS_TYPE &type);
	void InitVars();
	void ResetVars();
	CString PassName(CString algoStr, CString dataStr);
	CString DisplayPassName(CONCEPT *concept);
	CString CleanPassName(HTREEITEM item);
	void RenumberPasses(CONCEPT *concept=NULL);
	void RenumberPasses(int fromPass, int toPass=0);
	int GetPassCount(int start=0);
	bool StubAlreadyExists(CString conceptStr);
	CString RulePathName(CString nameStr, ANAL_PASS_TYPE type);
	HTREEITEM TreeItemFromPass(int pass);
	void AddExistingPass(CString filePathStr=_T(""));
	bool SequenceProperties(CONCEPT *concept=NULL);
	bool InStub(HTREEITEM itemIn=NULL);
	bool GetStubBoundingItems(HTREEITEM selectedItem, HTREEITEM &startItem, HTREEITEM &endItem);
	int PassNumberFromTreeItem(HTREEITEM item);
	void MoveTreeNodesDown(HTREEITEM startItem, HTREEITEM endItem);
	void MoveTreeNodesUp(HTREEITEM startItem, HTREEITEM endItem);
	void MoveTreeNodesDownPastGroup(HTREEITEM startItem, HTREEITEM sItem, HTREEITEM eItem);
	void MoveTreeNodesUpPastGroup(HTREEITEM endItem, HTREEITEM sItem, HTREEITEM eItem);
	HTREEITEM MoveTreeItem(HTREEITEM fromItem, HTREEITEM toItem, HTREEITEM toParent=NULL);
	void MoveConcept(HTREEITEM fromItem, HTREEITEM toItem, HTREEITEM toParent=NULL);
	CString PassPathStr(CONCEPT *concept=NULL, bool noExt=false, CString newNameStr=_T(""), bool directoryFlag=false);
	CString PassDirStr(CString fileNameStr, CONCEPT *concept=NULL);
	HTREEITEM TreeItemFromConcept(CONCEPT *concept, HTREEITEM=NULL);
	CONCEPT *LastFolderPassConcept(CONCEPT *concept=NULL);
	ANAL_PASS_TYPE PassType(CString algoStr=_T(""));
	static ANAL_PASS_TYPE ConceptPassType(CString algoStr=_T(""));	// 06/14/03 AM.
	void LoadKBAnalyzer(CONCEPT *concept=NULL, HTREEITEM item=NULL);
	void AddMain(CONCEPT *conceptParent, HTREEITEM itemParent);
	void AddFolder(CString folderNameStr=_T(""));
	void DeletePass();
	void DeleteStub();
	CString PassTypeStr(ANAL_PASS_TYPE type);
	HTREEITEM SelectItemFromStubName(CString stubNameStr);
	void DeleteGrammarStub();
	void AddExistingPassFile(CString filePathStr, bool editLabelFlag=true, bool renumberFlag=true);
	void RenumberPassesRecursive(CONCEPT *concept, int passNumber);
	ANAL_PASS_TYPE TypeFromItem(HTREEITEM item);
	void DrawSeparateLine(int y, COLORREF color);
	int GetFolderItemCount(CONCEPT *concept);
	CONCEPT *GetItemInfo(HTREEITEM item, int &pass, ANAL_PASS_TYPE &type, int &count);
	int RecalculateFolders(HTREEITEM item=NULL);
	void UpdateKB();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEVIEWANALYZER_H__6138FE01_E6D1_11D2_A2AA_00105A99059F__INCLUDED_)
