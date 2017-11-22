/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_DICTIONARYDLG_H__FB8A6FA1_2C50_11D4_A648_00105A99059F__INCLUDED_)
#define AFX_DICTIONARYDLG_H__FB8A6FA1_2C50_11D4_A648_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DictionaryDlg.h : header file
//

#include "EditListCtrl.h"

enum DICT_DISPLAY_TYPE {
	DICT_DISPLAY_ALPHABET = 0,
	DICT_DISPLAY_LETTER,
	DICT_DISPLAY_LETTER_GROUP,
};

enum ATTR_DISPLAY_TYPE {
	ATTR_DISPLAY_ALL = 0,
	ATTR_DISPLAY_WITH,
	ATTR_DISPLAY_WITHOUT,
};

enum FIND_TYPE {
	FIND_TYPE_FIND = 0,
	FIND_TYPE_FIND_OFF,
	FIND_TYPE_NARROW,
};

#define DICT_MENU_STARTID 800200
#define DICT_MENU_ENDID 800299

/////////////////////////////////////////////////////////////////////////////
// CDictionaryDlg dialog

class CDictionaryDlg : public CDialog
{
// Construction
public:
	CDictionaryDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDictionaryDlg)
	enum { IDD = IDD_DICTIONARY_DLG };
	CListCtrl	m_listCtrlList;
	CButton	m_buttonNoAttrs;
	CButton	m_buttonUp;
	CListCtrl	m_listCtrlDictionary;
	CEditListCtrl	m_listCtrlAttributes;
	CString	m_strWord;
	CString	m_strCount;
	CString	m_strAttrsDisplay;
	BOOL	m_boolAttributes;
	CString	m_strFindType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDictionaryDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDictionaryDlg)
	afx_msg void OnClickDictionaryList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkDictionaryList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUp();
	afx_msg void OnBackLetter();
	afx_msg void OnBack();
	afx_msg void OnForward();
	afx_msg void OnForwardLetter();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnRclickDictionaryList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAttrComboChange();
	virtual BOOL OnInitDialog();
	afx_msg void OnDictionraywordsMarkasNounCount();
	afx_msg void OnDictionraywordsMarkasNounNocount();
	afx_msg void OnDictionraywordsMarkasNounBoth();
	afx_msg void OnDictionraywordsMarkasVerbTransitive();
	afx_msg void OnDictionraywordsMarkasVerbIntransitive();
	afx_msg void OnDictionraywordsMarkasAdjective();
	afx_msg void OnDictionraywordsMarkasAdverb();
	afx_msg void OnMoveToList();
	afx_msg void OnClearList();
	afx_msg void OnClearAll();
	afx_msg void OnAttributeToggle();
	afx_msg void OnAddList();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeWord();
	afx_msg void OnSelchangeFindType();
	afx_msg void OnAddAll();
	afx_msg void OnFile();
	afx_msg void OnCustomDictDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomListDraw(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnToDict();
	afx_msg void OnDeleteWord();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	DICT_DISPLAY_TYPE m_dictDisplayType;
	ATTR_DISPLAY_TYPE m_attrDisplayType;
	FIND_TYPE m_findType;
	int m_intDictSelection;
	CBitmap m_bitmapButtonUp;
	CString m_strLetter;
	CString m_strLetterGroup;
	int m_intDictCount;
	CONCEPT *m_conceptSelected;
	CONCEPT *m_conceptTemp;
	bool m_boolOrphans;
	bool m_boolUpArrow;

	CComboBox* m_comboBoxAttr;
	CComboBox* m_comboBoxFind;

	void FillAttributeList(CONCEPT *concept=NULL);
	void FillDictionaryList(BOOL bDoSetRedraw=true); // SRP 010208
	void FillDictionaryList(CONCEPT *concept, int level, int &count);
	void SelectItemPage(int startitem, int endItem);
	ATTR_DISPLAY_TYPE AttrDisplayTypeFromString(CString attrTypeStr);
	void AddWordAttributeValue(_TCHAR *attrChar, _TCHAR *valChar, bool redisplay=false);
	void SelectDictionaryList();
	void AddWordToList(CONCEPT *concept, CString wordStr);
	bool Find(CString findStr);
	CString AttrDisplayStringFromType(ATTR_DISPLAY_TYPE type);
	FIND_TYPE FindTypeFromString(CString findTypeStr);
	CString FindStringFromType(FIND_TYPE type);
	void StripWord(CString &wordStr);
	int InsertItemAlphabetically(CString wordStr);
	CONCEPT *AddToTempConcept(CString wordStr);
	void DeleteTempConcepts();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DICTIONARYDLG_H__FB8A6FA1_2C50_11D4_A648_00105A99059F__INCLUDED_)
