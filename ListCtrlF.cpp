/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// ListCtrlF.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "VisualText.h"
#include "ListCtrlF.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlF

CListCtrlF::CListCtrlF()
{
}

CListCtrlF::~CListCtrlF()
{
}

BOOL CListCtrlF::AutoLoad(UINT nID, CWnd* pParent, UINT nIDCR, int nCharCol)
{
	if (!SubclassDlgItem(nID, pParent))
		return false;
	m_nIDCR = nIDCR;
	m_nCharCol = nCharCol;
	return true;
}


BEGIN_MESSAGE_MAP(CListCtrlF, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlF)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlF message handlers

void CListCtrlF::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int nStart;

	nStart = GetCurSel();
	// In a multiple-selection List control, unselect all entries
	int max = GetItemCount();
	for (int i = max (0, nStart); i < max; i++)
		SetItemState(i, 0, LVIS_SELECTED);

	int index = -1;
	for (int nLoop = 0; -1 == index && nLoop < 2; nLoop++) {
		int start = 0 == nLoop ? nStart + 1 : 0;
		int end = 0 == nLoop ? max : nStart;
		for (int n = start; n < min(end,max); n++) {
			CString strText = GetItemText(n,m_nCharCol);
			if (strText.GetLength() && to_upper(strText[0]) == to_upper(nChar)) {
				index = n;
				break;
			}
		}
	}
	if (index != -1) {
		SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
		EnsureVisible(index, FALSE);
	}
	CListCtrl::OnChar(nChar, nRepCnt, nFlags);
}

void CListCtrlF::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CWnd *pParent = GetParent();
	CWnd *pBtn;
	if (pParent) {
		BOOL bShift;
		switch (nChar) {
			case VK_ESCAPE:
				pParent->SendMessage(WM_COMMAND, IDCANCEL, 0L);
				return;
			
			case VK_RETURN:
				pBtn = pParent->GetDlgItem(m_nIDCR);
				if (pBtn) {
					pParent->SendMessage(WM_COMMAND,
						(WPARAM)MAKELONG(m_nIDCR, BN_CLICKED),
						(LPARAM)pBtn->m_hWnd);
				}
				return;
				
			case VK_TAB:
				bShift = ISKEYPRESSED(VK_SHIFT);
				pParent->SendMessage(WM_NEXTDLGCTL, bShift, 0);
				return;

			default:
				break;
		}
	}
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

UINT CListCtrlF::OnGetDlgCode() 
{
	return (DLGC_WANTCHARS | DLGC_WANTALLKEYS);
//	return CListCtrl::OnGetDlgCode();
}

int CListCtrlF::GetCurSel (void)
{
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos)
		return GetNextSelectedItem(pos);
	return -1;
}

