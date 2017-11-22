/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_PASSPROPERTIESDLG_H__66B06A11_9872_11D3_A4B9_00105A99059F__INCLUDED_)
#define AFX_PASSPROPERTIESDLG_H__66B06A11_9872_11D3_A4B9_00105A99059F__INCLUDED_

#include "TreeViewAnalyzer.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PassPropertiesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPassPropertiesDlg dialog

class CPassPropertiesDlg : public CDialog
{
// Construction
public:
	CPassPropertiesDlg(CWnd* pParent = NULL);   // standard constructor
	void SetData(CString nameStr, int intPass, ANAL_PASS_TYPE type);
	void SetData(CONCEPT *concept);

// Dialog Data
	//{{AFX_DATA(CPassPropertiesDlg)
	enum { IDD = IDD_PASS_PROPERTIES_DLG };
	CString	m_strName;
	CString	m_strType;
	BOOL	m_boolActive;
	CString	m_strFrom;
	CString	m_strTo;
	BOOL	m_boolToEnd;
	BOOL	m_boolAll;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPassPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPassPropertiesDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnToEnd();
	afx_msg void OnSelchangeFrom();
	afx_msg void OnSelchangeTo();
	afx_msg void OnAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CONCEPT *m_concept;
	int m_intPass;
	CString m_strNameOrig;
	ANAL_PASS_TYPE m_typePassOrig;
	ANAL_PASS_TYPE m_typePass;
	bool m_boolTypeable;

	CString StringFromType(ANAL_PASS_TYPE type);
	ANAL_PASS_TYPE TypeFromString(CString typeStr);
	ANAL_PASS_TYPE TypeFromUserString(CString typeStr);
	void FillCombos();
	void FillCombo(UINT comboID, int from, int to, int selected);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSPROPERTIESDLG_H__66B06A11_9872_11D3_A4B9_00105A99059F__INCLUDED_)
