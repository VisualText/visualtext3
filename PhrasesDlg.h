/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_PHRASESDLG_H__0DEE0721_C2CB_11D3_A55B_00105A99059F__INCLUDED_)
#define AFX_PHRASESDLG_H__0DEE0721_C2CB_11D3_A55B_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PhrasesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPhrasesDlg dialog

class CPhrasesDlg : public CDialog
{
// Construction
public:
	BOOL m_bReadOnlyMode;
	void SetData(CONCEPT *concept);
	CPhrasesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPhrasesDlg)
	enum { IDD = IDD_PHRASES_DLG };
	CListCtrl	m_listCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhrasesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPhrasesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelete();
	afx_msg void OnAttributes();
	afx_msg void OnAdd();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDragUp();
	afx_msg void OnDragDown();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void FillPhraseList();
	CONCEPT * m_concept;

	CBitmapButton bitmapButtonUp;
	CBitmapButton bitmapButtonDown;

	void Enable(UINT id, BOOL value);
	int GetFirstSelectedItemNumber(bool warnFlag=true);
	void InvokeAttributeEditor();
	void SetButtonStates();
	void SelectAndFocus(int item);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHRASESDLG_H__0DEE0721_C2CB_11D3_A55B_00105A99059F__INCLUDED_)
