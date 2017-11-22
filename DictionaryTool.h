#pragma once
#include "afxcmn.h"
#include "WorkQueue.h"
#include "afxwin.h"
#include "ReportCtrl.h"

enum WORD_RESTRICTION_TYPE {
	WORD_RESTRICTION_CONTAINS = 0,
	WORD_RESTRICTION_STARTS_WITH,
	WORD_RESTRICTION_ENDS_WITH,
	WORD_RESTRICTION_EQUALS
};

// CDictionaryTool dialog

class CDictionaryTool : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDictionaryTool)

public:
	CDictionaryTool(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDictionaryTool();

	static unsigned long __stdcall ThreadFuncDestroy( void*  pParam );
	CWorkQueue m_WorkQueue;

// Dialog Data
	enum { IDD = IDD_DICTIONARY_TOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnAddWord(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateCountDisplay(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedAttributes();
	afx_msg void OnBnClickedSearch();

	CList <int, int> m_intNewWords;
	CListCtrl m_listCtrlWords;
	void FillDictionaryList(CONCEPT *startConcept, int level, int &count);
	virtual BOOL OnInitDialog();
	CString m_strCount;
	WORD_RESTRICTION_TYPE m_wordRestrictionType;
	int m_intDictSelection;
	CBitmap m_bitmapButtonUp;
	CString m_strLetter;
	CString m_strLetterGroup;
	int m_intScanCount;
	int m_intCount;
	int m_intAppendCount;
	CONCEPT *m_conceptSelected;
	CONCEPT *m_conPOS;
	CONCEPT *m_conA;
	int m_intAttrSelected;
	bool m_boolOrphans;
	bool m_boolUpArrow;
	bool m_boolStopThreads;
	bool m_boolThreadFilling;
	bool m_boolOpeningKB;
	void FillDictionaryList(BOOL bDoSetRedraw);
	void SetCountDisplay();
	void GotoConceptPath(CString conceptPathStr);
	void GotoWordConcept(CONCEPT *concept);
	void SelectItem(int itemNum, bool runFlag=false);
	void AddPOS(CString posStr);
	void AddInflection(CString wordStr, CString attrStr, CString valueStr, bool isConcept=false);
	void SetTextEditText(UINT id, CString textStr);
	void FillPOS();
	void AddWord(CString wordStr, CONCEPT *concept, bool forceAdd=false);
	bool POSMatch(CStringList &posStringList);
	bool AttrMatch(CString wordStr, CStringList &attrStringList, CStringList &valueStringList);
	void AddAttribute(CONCEPT *concept, CString attrStr, CString valueStr, bool replaceFlag=false);
	CString GetValueString(CString valueStr);
	void SetEnable(UINT id, bool value);
	bool RenamePOS(CONCEPT *startConcept, int level, int &count, CString posStr, CString oldValStr, CString newValStr);
	void SaveAttributes(bool replaceFlag=false);
	CString WordDisplay(CONCEPT *concept, CStringList &POSStringList,
									 CStringList &attrStringList, CStringList &valueStringList);
	void RefreshWordsDisplay();
	void RefreshWordDisplay(CONCEPT *concept, int index);
	void RefreshWordDisplay(CString wordStr);
	void OpenKb(CONCEPT *concept);
	void LoadConcept(CONCEPT *concept);
	void DeleteNewWordIndex(int index);

	CReportCtrl m_listCtrlInflections;
	CComboBox m_comboBoxWordRestriction;
	CString m_strWordRestriction;
	CString m_strWordSearch;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CListBox m_listBoxPOS;
	CONCEPT *m_conceptValue;
	bool m_boolIsConcept;
	int m_intWordSelected;

	afx_msg void OnDestroy();
	afx_msg void OnNMDblclkWords(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickWords(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedStop();
	afx_msg void OnCbnSelchangeWordRestriction();
	afx_msg void OnBnClickedGotoRoot();
	afx_msg void OnNMDblclkInflections(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedEnglish();
	CString m_strWord;
	afx_msg void OnBnClickedGotoWord();
	CString m_strAttr;
	CString m_strValue;
	afx_msg void OnNMClickInflections(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_comboBoxPOS;
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedAddPos();
	CString m_strPOS;
	afx_msg void OnBnClickedDeletePos();
	afx_msg void OnBnClickedRun();
	CComboBox m_comboBoxPOSMatchSpecs;
	CString m_strPOSMatchSpec;
	afx_msg void OnBnClickedSave();
	CString m_strAttrWord;
	afx_msg void OnBnClickedChangeInflection();
	afx_msg void OnBnClickedClearWords();
	afx_msg void OnBnClickedSameRoots();
	CString m_strAttrMatch;
	CComboBox m_comboBoxAttrMatch;
	afx_msg void OnCbnSelchangeAttrMatch();
	afx_msg void OnBnClickedAddInflection();
	afx_msg void OnBnClickedAddConcept();
	afx_msg void OnBnClickedDeleteWord();
	BOOL m_boolAppend;
	BOOL m_boolBrowse;
	afx_msg void OnBnClickedRenamePos();
	afx_msg void OnBnClickedAddAttrAll();
	afx_msg void OnBnClickedAddList();
	afx_msg void OnBnClickedReplace();
	afx_msg void OnBnClickedDeleteInflection();
	afx_msg void OnNMCustomdrawWords(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedKb();
	//CBitmapButton m_buttonKB;
	afx_msg void OnBnClickedRemove();
	BOOL m_boolMultiSelect;
	afx_msg void OnBnClickedMultiSelect();
};
