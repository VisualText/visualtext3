/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_RECORDPROPERTIES_H__AD564FD1_4434_11D3_A3D8_00105A99059F__INCLUDED_)
#define AFX_RECORDPROPERTIES_H__AD564FD1_4434_11D3_A3D8_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RecordProperties.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRecordProperties dialog

class CRecordProperties : public CDialog
{
// Construction
public:
	CRecordProperties(CWnd* pParent = NULL);   // standard constructor

public:
	void SetData(CONCEPT *concept);

// Dialog Data
	//{{AFX_DATA(CRecordProperties)
	enum { IDD = IDD_RECORD_PROPERTIES };
	CString	m_strFirst;
	BOOL	m_boolIncludeLast;
	CString	m_strRecordName;
	CString	m_strZoneConcept;
	CString	m_strLabel;
	CComboBox m_combo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecordProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecordProperties)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnZoneButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CONCEPT *m_concept;

	void SetConceptAttributes();
	void GetConceptAttributes();
	int FillOrderBy();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECORDPROPERTIES_H__AD564FD1_4434_11D3_A3D8_00105A99059F__INCLUDED_)
