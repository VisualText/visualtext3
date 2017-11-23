/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// EditCR.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "VisualText.h"
#include "EditCR.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditCR

CEditCR::CEditCR()
{
}

CEditCR::~CEditCR()
{
}


BEGIN_MESSAGE_MAP(CEditCR, CEdit)
	//{{AFX_MSG_MAP(CEditCR)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditCR message handlers

BOOL CEditCR::AutoLoad(UINT nID, CWnd* pParent, UINT nIDCR)
{
	if (!SubclassDlgItem(nID, pParent))
		return FALSE;
	m_nIDCR = nIDCR;
	return true;
}

UINT CEditCR::OnGetDlgCode() 
{
	// TODO: Add your message handler code here and/or call default
	return (DLGC_WANTALLKEYS);
//	return CEdit::OnGetDlgCode();
}

#define ID_EDITCTRL_IN_COMBOBOX	1001
void CEditCR::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CWnd *pParent = GetParent();
	CWnd *pBtn;
	if (pParent && ::GetWindowLong(m_hWnd, GWL_ID) == ID_EDITCTRL_IN_COMBOBOX)
		pParent = pParent->GetParent();
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

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
