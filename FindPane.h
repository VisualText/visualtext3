#pragma once

#include "ListCtrlWMenu.h"

// CFindPane view

class CFindPane : public CWnd
{
	DECLARE_DYNCREATE(CFindPane)

	// Attributes
public:
	ListCtrlWMenu	m_listCtrl;

public:
	CFindPane();           // protected constructor used by dynamic creation
	virtual ~CFindPane();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFindPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



