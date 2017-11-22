/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_PREFANALYZERDEPENDENT_H__BA1707FC_443B_407C_9E8A_840C81452262__INCLUDED_)
#define AFX_PREFANALYZERDEPENDENT_H__BA1707FC_443B_407C_9E8A_840C81452262__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrefAnalyzerDependent.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrefAnalyzerDependent dialog

class CPrefAnalyzerDependent : public CPropertyPage
{
	DECLARE_DYNCREATE(CPrefAnalyzerDependent)

// Construction
public:
	CPrefAnalyzerDependent();
	~CPrefAnalyzerDependent();

// Dialog Data
	//{{AFX_DATA(CPrefAnalyzerDependent)
	enum { IDD = IDD_PREF_ANALYZER_DEPENDENT_PAGE };
	BOOL	m_boolLoadCompiledKB;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPrefAnalyzerDependent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPrefAnalyzerDependent)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFANALYZERDEPENDENT_H__BA1707FC_443B_407C_9E8A_840C81452262__INCLUDED_)
