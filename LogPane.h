#pragma once

#include "ListCtrlWMenuLog.h"

// CLogPane view

class CLogPane : public CWnd
{
	DECLARE_DYNCREATE(CLogPane)

	// Attributes
public:
	ListCtrlWMenuLog	m_listCtrl;

public:
	CLogPane();           // protected constructor used by dynamic creation
	virtual ~CLogPane();

protected:
	//{{AFX_MSG(CLogPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	afx_msg void OnRButtonDown(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};


