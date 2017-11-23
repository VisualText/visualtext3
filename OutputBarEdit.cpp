/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// OutputBarEdit.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "OutputBarEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBarEdit

COutputBarEdit::COutputBarEdit()
{
}

COutputBarEdit::~COutputBarEdit()
{
}

BEGIN_MESSAGE_MAP(COutputBarEdit, CEdit)
	//{{AFX_MSG_MAP(COutputBarEdit)
	ON_WM_HSCROLL()
	//ON_WM_CREATE()
	ON_WM_SIZE()
	ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputBarEdit message handlers

void COutputBarEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    if (m_ScrollBarH.m_hWnd  &&  
        pScrollBar->GetSafeHwnd() == m_ScrollBarH.m_hWnd)
    {
        SCROLLINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cbSize = sizeof(si);
        si.fMask = SIF_PAGE;
        m_ScrollBarH.GetScrollInfo(&si);

	    int x = m_ScrollBarH.GetScrollPos();
        switch (nSBCode)
        {
	        case SB_TOP:
		        x = 0;
		        break;
	        case SB_BOTTOM:
		        x = INT_MAX;
		        break;
	        case SB_LINEUP:
		        x--;
		        break;
	        case SB_LINEDOWN:
		        x++;
		        break;
	        case SB_PAGEUP:
		        x -= si.nPage;
		        break;
	        case SB_PAGEDOWN:
		        x += si.nPage;
		        break;
	        case SB_THUMBTRACK:
		        x = nPos;
		        break;
        }
        si.nPos = x;
        si.fMask = SIF_POS;
        m_ScrollBarH.SetScrollInfo(&si);
    }

    CEdit::OnHScroll(nSBCode, nPos, pScrollBar);
}

CScrollBar* COutputBarEdit::GetScrollBarCtrl(int nBar) const
{
    if (nBar == SB_HORZ)
    {
        CScrollBar *pSB = (CScrollBar *)&m_ScrollBarH;
        SCROLLINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cbSize = sizeof(si);
        if (m_ScrollBarH.m_hWnd == NULL)
        {
            pSB->Create(WS_CHILD, CRect(0, 0, 0, 0), (CWnd *)this, 0);
            si.fMask = SIF_ALL;
            if (::GetScrollInfo(m_hWnd, SB_HORZ, &si))
            {
	            pSB->SetScrollInfo (&si);
            }
        }
        else
        {
            si.fMask = SIF_RANGE | SIF_POS;
            ::SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);
            ::ShowScrollBar(m_hWnd, SB_HORZ, TRUE);
            ::GetScrollInfo(m_hWnd, SB_HORZ, &si);
            ::ShowScrollBar(m_hWnd, SB_HORZ, FALSE);
            ::SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0);
	        pSB->SetScrollInfo (&si);
        }
        return pSB;
    }
	
	return CEdit::GetScrollBarCtrl(nBar);
}

void COutputBarEdit::OnSize(UINT nType, int cx, int cy) 
{
	CEdit::OnSize(nType, cx, cy);

    if (m_ScrollBarH.m_hWnd)
    {
        SCROLLINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cbSize = sizeof(si);
        CRect r;
        GetClientRect(r);
        si.fMask = SIF_PAGE;
        si.nPage = r.Width() - 1;
        m_ScrollBarH.SetScrollInfo(&si);
    }	
}

void COutputBarEdit::OnUpdate() 
{
    //CXTFlatTabCtrl *pParent = (CXTFlatTabCtrl *)GetParent();
    //ASSERT_KINDOF(CXTFlatTabCtrl, pParent);
    //pParent->SyncScrollBar();
}

void COutputBarEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
    OnUpdate();
}
/*
int COutputBarEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
    m_hookCoolMenu.HookWindow(m_hWnd);
	m_hookCoolMenu.LoadToolbar(IDR_MAINFRAME);
	
	return 0;
}
*/
void COutputBarEdit::OnRButtonDown(UINT nFlags, CPoint point) 
{
	logmsg(_T("COutputBarEdit::OnRbuttonDown"));	// 05/14/08 AM.
	CPoint pt = point;
	ClientToScreen(&pt);

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP));

	CMenu* pPopup = menu.GetSubMenu(0);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y,
		this);
}

BOOL COutputBarEdit::Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	dwStyle |= ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY | WS_VSCROLL;

	if (!CEdit::Create(dwStyle, rect, pParentWnd, nID))
		return FALSE;

	return TRUE;
}

HBRUSH COutputBarEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
    pDC->SetTextColor( RGB(0x00,0x00,0x00) );
    pDC->SetBkColor( RGB(0xFF,0xFF,0xFF) );
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return ::GetSysColorBrush(COLOR_WINDOW);
}
