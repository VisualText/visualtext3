/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_PREFLOOKUPPAGE_H__A0E94591_852D_11D5_A97B_00105A99059F__INCLUDED_)
#define AFX_PREFLOOKUPPAGE_H__A0E94591_852D_11D5_A97B_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrefLookupPage.h : header file
//

#include "EditCR.h"

class CLookupItem {

public:
	CString m_Name;
	CString m_URL;

public:
	CLookupItem();
	CLookupItem(LPCTSTR name, LPCTSTR URL);
	~CLookupItem();
	int FormatData(CString &cData);
	void RetrieveData(LPCTSTR lpDataIn);
};
/////////////////////////////////////////////////////////////////////////////
// CPrefLookupPage dialog

class CPrefLookupPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPrefLookupPage)

// Construction
public:
	CPrefLookupPage();
	~CPrefLookupPage();

// Dialog Data
	//{{AFX_DATA(CPrefLookupPage)
	enum { IDD = IDD_PREF_LOOKUP };
	CListCtrl	m_listCtrl;
	CString	m_strName;
	CString	m_strURL;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrefLookupPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Variables
	CEditCR m_Edit_Name;
	CEditCR m_Edit_URL;
	CBitmapButton m_BtnUp;
	CBitmapButton m_BtnDown;

// Implementation
public:
	void ClearAllFields (void);
	BOOL AllFieldsComplete(void);
	int ListCtrl_GetCurSel(CWordArray& selArray);
	void ListCtrl_SetItemText(int row, LPCTSTR lpName, LPCTSTR lpURL);
	void ListCtrl_SelectRow(int index);
	int ListCtrl_FindItem(CLookupItem* pItemSearch);
	int ListCtrl_AddItem(LPCTSTR lpName, LPCTSTR lpURL);
	void ListCtrlInit(void);
	void LoadListCtrl(int row);
	void LKUP_ReadData(CStringArray& strNames, CStringArray& strURLs);
	void LKUP_WriteData(CStringArray& strNames, CStringArray& strURLs);
	void LKUP_SaveData(void);
	static void CPrefLookupPage::GetDictionaryLookupList(CStringArray& strNames, CStringArray& strURLs);
	static void GetDictionaryLookupMenu(CMenu& menu, UINT idBase, CStringArray& strNames, CStringArray& strURLs);
	static void FillDictionaryLookupComboBox(CComboBox& combo, CStringArray& strNames, CStringArray& strURLs);

protected:

	// Generated message map functions
	//{{AFX_MSG(CPrefLookupPage)
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnColumnclickLookupList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitemLookupList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedLookupList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdate();
	afx_msg void OnUp();
	afx_msg void OnDown();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFLOOKUPPAGE_H__A0E94591_852D_11D5_A97B_00105A99059F__INCLUDED_)
