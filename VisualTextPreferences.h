/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_VisualTextPREFERENCES_H__1A95DAB1_B610_11D2_8BFC_00105A99059F__INCLUDED_)
#define AFX_VisualTextPREFERENCES_H__1A95DAB1_B610_11D2_8BFC_00105A99059F__INCLUDED_

#include "PrefArchivePage.h"
#include "PrefDisplayPage.h"
#include "PrefEmailPage.h"
#include "PrefGeneralPage.h"
#include "PrefLookupPage.h"
#include "PrefAnalyzerDependent.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VisualTextPreferences.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVisualTextPreferences dialog

class CVisualTextPreferences : public CDialog
{
// Construction
public:
	CVisualTextPreferences(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVisualTextPreferences)
	enum { IDD = IDD_VISUALTEXT_PREFERENCES };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisualTextPreferences)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:

	// Generated message map functions
	//{{AFX_MSG(CVisualTextPreferences)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CPropertySheet m_dlgPropSheet;
	CPrefArchivePage m_pageArchive;
	CPrefDisplayPage m_pageDisplay;
	CPrefEmailPage m_pageEmail;
	CPrefGeneralPage m_pageGeneral;
	CPrefLookupPage m_pageLookup;
	CPrefAnalyzerDependent m_pageAnalyzerDependent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VisualTextPREFERENCES_H__1A95DAB1_B610_11D2_8BFC_00105A99059F__INCLUDED_)
