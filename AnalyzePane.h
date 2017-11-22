#pragma once

#include "ListCtrlWMenuLog.h"

// CFindPane view

class CAnalyzePane : public CWnd
{
	DECLARE_DYNCREATE(CAnalyzePane)

	// Attributes
public:
	ListCtrlWMenuLog	m_listCtrl;

public:
	CAnalyzePane();           // protected constructor used by dynamic creation
	virtual ~CAnalyzePane();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFindPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

