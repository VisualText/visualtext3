/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_OUTPUTBAREDIT_H__B6F15EAE_695B_463E_AE68_73BDE70A6E8A__INCLUDED_)
#define AFX_OUTPUTBAREDIT_H__B6F15EAE_695B_463E_AE68_73BDE70A6E8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutputBarEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COutputBarEdit window

class COutputBarEdit : public CEdit
{
// Construction
public:
	COutputBarEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputBarEdit)
	public:
	virtual BOOL Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
	//}}AFX_VIRTUAL

    //CXTCoolMenu m_hookCoolMenu;

// Implementation
public:
	virtual ~COutputBarEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(COutputBarEdit)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdate();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

    CScrollBar m_ScrollBarH;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTBAREDIT_H__B6F15EAE_695B_463E_AE68_73BDE70A6E8A__INCLUDED_)
