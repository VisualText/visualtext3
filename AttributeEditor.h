/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_ATTRIBUTEEDITOR_H__56676361_2A60_11D3_A38A_00105A99059F__INCLUDED_)
#define AFX_ATTRIBUTEEDITOR_H__56676361_2A60_11D3_A38A_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AttributeEditor.h : header file
//

#include "resource.h"
#include "EditListCtrl.h"
#include "EditCR.h"
#include "ListCtrlF.h"

#define WMU_NEXT (WM_USER + 1301)
#define WMU_PREV (WM_USER + 1302)
#define WMU_DOWN (WM_USER + 1303)
#define WMU_UP (WM_USER + 1304)
#define WMU_OPENPATH (WM_USER + 1305)
/////////////////////////////////////////////////////////////////////////////
// CAttributeEditor dialog

class CMainFrame;

class CQuickEditItem {

public:
	CString m_Attribute;
	CString m_Value;

public:
	CQuickEditItem();
	CQuickEditItem(LPCTSTR attribute, LPCTSTR value);
	~CQuickEditItem();
	int FormatData(CString &cData);
	void RetrieveData(LPCTSTR lpDataIn);
};

///////////////////////////////////////////

class CAttributeEditor : public CDialog
{
// Construction
public:
	void SetCGConcept(CONCEPT *concept);
	CAttributeEditor(CWnd* pParent = NULL);   // standard constructor
	void DrawListViewItem(LPDRAWITEMSTRUCT lpDrawItem);
	BOOL InitListViewImageLists(void);
	void InitListWithData(int row = 0);
	void InitListWithData(LPCTSTR attribute, LPCTSTR value);
	void EnableNavButtons(void);
	void LoadConcept(CONCEPT *concept);

	void QESaveColumnWidths(void);
	void QERetrieveColumnWidths(void);
	void QE_ListCtrlInit(void);
	void Cleanup(void);
	int QE_ListCtrl_GetCurSel(CWordArray& selArray);
	int QE_ListCtrl_FindItem(CQuickEditItem* pItemSearch);
	void QE_ListCtrl_SetItemText(int row, LPCTSTR lpAttribute, LPCTSTR lpValue);
	int QE_ListCtrl_AddItem(LPCTSTR lpAttribute, LPCTSTR lpValue);
	void QE_LoadListCtrl(int row);
	void SaveQEData(void);
	void WriteQEData(CStringArray& strAttributes, CStringArray& strValues);
	void ReadQEData(CStringArray& strAttributes, CStringArray& strValues);
	BOOL QEAllFieldsComplete(void);
	void QEClearAllFields(void);
	int FindNumAttrValEntries(LPCTSTR attribute);
	int FindAttributeValue(LPCTSTR attribute, LPCTSTR value);
	void AdjustQEItem(int nItem);
	void AdjustQEList(void);
	int ImageFromType(ValueType type);

	void Open(CONCEPT* concept, BOOL bCalledFromKB=false);
	void OpenKb(CONCEPT *concept);

// Dialog Data
	//{{AFX_DATA(CAttributeEditor)
	enum { IDD = IDD_ATTRIBUTE_EDITOR };
	CEditListCtrl	m_listCtrl;
	CString	m_strSearch;
	BOOL	m_boolNoAttrs;
	CString	m_QE_Attribute;
	CString	m_QE_Value;
	CString	m_strValueType;
	BOOL	m_boolBrowseConcept;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttributeEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CMainFrame *m_mainWnd;
	CImageList m_AE_ImageList;
	CWnd* m_pWndParent;
	CBitmapButton m_BtnPrev;
	CBitmapButton m_BtnNext;
	CBitmapButton m_BtnDown;
	CBitmapButton m_BtnUp;
	CBitmapButton m_BtnKB;
	CBitmapButton m_BtnLookup;
	CBitmapButton m_BtnDeleteWord;
	CEditCR m_EditCR;
	CEditCR m_Edit_QE_Attribute;
	CEditCR m_Edit_QE_Value;
	CListCtrlF m_QEListCtrl;
	CRect m_rDlg[2];
	int m_nDlg;
	BOOL m_bInitListWithData;
	CStringArray m_strURLs;
	int m_ComboWidth;

protected:

	// Generated message map functions
	//{{AFX_MSG(CAttributeEditor)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddAttribute();
	afx_msg void OnAddvalue();
	afx_msg void OnDelete();
	afx_msg void OnItemchangedAttributeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditAttributeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnNext();
	afx_msg void OnPrev();
	afx_msg void OnSearch();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnOpenKb();
	afx_msg void OnQuickedithelp();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnUpdate();
	afx_msg void OnColumnclickListquickedit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListquickedit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitemListquickedit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKbEdit();
	afx_msg void OnDblclkListquickedit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnQuickedit();
	afx_msg void OnDeleteWord();
	afx_msg void OnAddWord();
	afx_msg void OnDropdownComboLookup();
	afx_msg void OnCloseupComboLookup();
	afx_msg void OnSelendokComboLookup();
	afx_msg void OnLookup();
	afx_msg void OnDown();
	afx_msg void OnUp();
	afx_msg void OnDblclkAttributeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBrowseConcept();
	afx_msg void OnSelchangeValueType();
	//}}AFX_MSG
	afx_msg void OnDeleteItemListctrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnDeleteLine(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetImageList(WPARAM wParam, LPARAM lParam);
	afx_msg UINT OnGetDlgCode();
	DECLARE_MESSAGE_MAP()

private:
	int	m_URLcounter;
	CString m_strConceptName;
	CImageList m_imageList;
	CONCEPT * m_concept;
	bool m_boolOpeningKB;
	CLineItem *m_pLineItem;	// Track movement to new line.	// 06/19/02 AM.

	void ReIcon();
	void FillList();
	CLineItem *GetSelectedLine(int &nItem);
	void ItemChangedAttributeList(CLineItem* pLineItem);
public:
	afx_msg void OnEnChangeSearch();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIBUTEEDITOR_H__56676361_2A60_11D3_A38A_00105A99059F__INCLUDED_)
