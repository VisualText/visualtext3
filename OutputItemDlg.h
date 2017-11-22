/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_OUTPUTITEMDLG_H__66819D91_4901_11D3_A3E4_00105A99059F__INCLUDED_)
#define AFX_OUTPUTITEMDLG_H__66819D91_4901_11D3_A3E4_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutputItemDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COutputItemDlg dialog

class COutputItemDlg : public CDialog
{
// Construction
public:
	COutputItemDlg(CWnd* pParent = NULL);   // standard constructor

public:
	void SetData(CONCEPT *concept);

// Dialog Data
	//{{AFX_DATA(COutputItemDlg)
	enum { IDD = IDD_OUTPUT_ITEM_DLG };
	CString	m_strType;
	CString	m_strNormalForm;
	CString	m_strPrintName;
	CString	m_strFieldName;
	//}}AFX_DATA

private:
	CString origStrType;
	CString origStrNormalForm;
	CString origStrPrintName;
	CString origStrFieldName;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputItemDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COutputItemDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeFieldName();
	afx_msg void OnKillfocusFieldName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool m_boolFirstTimeField;
	CONCEPT * m_concept;

	void GetConceptAttributes();
	void SetConceptAttributes();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTITEMDLG_H__66819D91_4901_11D3_A3E4_00105A99059F__INCLUDED_)
