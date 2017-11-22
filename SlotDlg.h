/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_SLOTDLG_H__4818A7A1_C3A4_11D3_A55E_00105A99059F__INCLUDED_)
#define AFX_SLOTDLG_H__4818A7A1_C3A4_11D3_A55E_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SlotDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSlotDlg dialog

class CSlotDlg : public CDialog
{
// Construction
public:
	CSlotDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSlotDlg)
	enum { IDD = IDD_SLOT_DLG };
	CListCtrl	m_listCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlotDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSlotDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void FillList();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLOTDLG_H__4818A7A1_C3A4_11D3_A55E_00105A99059F__INCLUDED_)
