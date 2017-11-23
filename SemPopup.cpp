/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

 // SemPopup.cpp : implementation file
 //

 #include "stdafx.h"
 #include "resource.h"
 #include "kb.h"
 #include "Utils.h"
 #include "ParseTreeItemInfo.h"
 #include "ParseTreeView.h"
 #include "MainFrm.h"
 #include "SemPopup.h"
 #include "SemPopupDlg.h"

 extern CFont appFontFixed;
 extern CFont appFontFixedSmall;	// 09/09/08 AM.

 #ifdef _DEBUG
 #define new DEBUG_NEW
 #undef THIS_FILE
 static char THIS_FILE[] = __FILE__;
 #endif


 /////////////////////////////////////////////////////////////////////////////
 // CSemPopup

 CSemPopup::CSemPopup()
 {
        m_pView = NULL;
        m_pInfo = NULL;
        m_cRCreate.SetRectEmpty();
 }

 CSemPopup::CSemPopup(CParseTreeItemInfo* pInfo, CTreeView* pView)
 {
        m_pInfo = pInfo;
        m_pView = pView;
        // TRYING A FIXED FONT FOR YELLOW BOX.	// 09/14/08 AM.
        CFont *pFont = &appFontFixedSmall;	// 09/14/08 AM.
//        CFont* pFont = m_pView->GetFont();	// 09/14/08 AM.
        LOGFONT lf;
        pFont->GetObject(sizeof(lf), &lf);
        m_Font.CreateFontIndirect(&lf);

        CRect cRView;
        pView->GetTreeCtrl().GetItemRect(pInfo->GetTreeItem(), &m_cRDraw, true);
        pView->ClientToScreen(m_cRDraw);
        m_cRDraw.OffsetRect(CPoint(-1,-1));
        m_cRDraw.right = GetSystemMetrics(SM_CXSCREEN);

        CDC* pDC = m_pView->GetDC();
        CFont* pFontOld = pDC->SelectObject(&m_Font);
        m_strText = m_pInfo->GetText();
        // Truncate Text for Tool Tip popup.
        if (m_strText.GetLength() > PARSE_TREE_LINE_MAX) {
                m_strText = m_strText.Left(PARSE_TREE_LINE_MAX);
                m_strText += _T("...");
        }
        if (!m_pInfo->GetSem().IsEmpty()) {
                m_strText += _T("\n");
                if (m_pInfo->GetText().GetAt(m_pInfo->GetText().GetLength() - 1) != '\n')
                        m_strText += _T("\n");
                m_strText += m_pInfo->GetSem();
        }
        m_cRDraw.DeflateRect(3,2);
        pDC->DrawText(m_strText, m_strText.GetLength(), &m_cRDraw,
					DT_NOCLIP |	// Supposedly faster.	// 09/14/08 AM.
//					DT_EXTERNALLEADING |
					DT_WORDBREAK |
					DT_CALCRECT
					// YELLOW BOX. DISPLAY MULTI-COLUMNS	// 09/14/08 AM.
					// THE BELOW DON'T WORK.
//					DT_EXPANDTABS	// 09/14/08 AM.
//					DT_TABSTOP	// 09/14/08 AM.
					);
        m_cRCreate = m_cRDraw;
        m_cRCreate.InflateRect(3,2);
        pDC->SelectObject(pFontOld);
        pView->ReleaseDC(pDC);
 }

 CSemPopup::~CSemPopup()
 {
 }

 BEGIN_MESSAGE_MAP(CSemPopup, CWnd)
        //{{AFX_MSG_MAP(CSemPopup)
        ON_WM_PAINT()
        ON_WM_NCHITTEST()
        ON_WM_LBUTTONDOWN()
        //}}AFX_MSG_MAP
 END_MESSAGE_MAP()


 /////////////////////////////////////////////////////////////////////////////
 // CSemPopup message handlers
 #define CLR_PALEYELLOW  RGB(255,255,128)
 #define CLR_BLACK       RGB(0,0,0)

 void CSemPopup::OnPaint()
 {
        CPaintDC dc(this); // device context for painting

        CRect cR = m_cRCreate;
        cR.OffsetRect(CPoint(-cR.left, -cR.top));
        CBrush brush(CLR_PALEYELLOW);
        CPen pen(PS_SOLID, 1, CLR_BLACK);
        CPen* pPenOld = (CPen*)dc.SelectObject(&pen);
        CBrush* pBrushOld = (CBrush*)dc.SelectObject(&brush);
 #ifdef UNICODE
        CFont* pFontOld = (CFont*)dc.SelectObject(&appFontFixed);
 #else
        CFont* pFontOld = (CFont*)dc.SelectObject(&m_Font);
 #endif

        dc.Rectangle(cR);
        cR.DeflateRect(3,2);
        dc.SetBkMode(TRANSPARENT);
        dc.SetTextColor(CLR_BLACK);
        dc.DrawText(m_strText, m_strText.GetLength(), (LPRECT)&cR,
                DT_EXTERNALLEADING | DT_WORDBREAK);

        dc.SelectObject(pFontOld);
        dc.SelectObject(pPenOld);
        dc.SelectObject(pBrushOld);
 }

 LRESULT CSemPopup::OnNcHitTest(CPoint point)
 {
        LRESULT uRV = CWnd::OnNcHitTest(point);
        TVHITTESTINFO hit;
        ZeroMemory(&hit, sizeof(TVHITTESTINFO));
        hit.pt = point;
        m_pView->GetTreeCtrl().ScreenToClient(&hit.pt);
        HTREEITEM hitItem = m_pView->GetTreeCtrl().HitTest(&hit);
        if (!(hitItem && (hit.flags & (TVHT_ONITEM | TVHT_ONITEMRIGHT | TVHT_ONITEMBUTTON)) ) ||
                hitItem != m_pInfo->GetTreeItem() )
                m_pView->PostMessage(WMU_DESTROY_SEMPOPUP);
        return uRV;
 }

 void CSemPopup::OnLButtonDown(UINT nFlags, CPoint point)
 {
        CWnd::OnLButtonDown(nFlags, point);
        CSemPopupDlg dlg;
        LOGFONT lf;
 #ifdef UNICODE
        appFontFixed.GetObject(sizeof(lf), &lf);
 #else
        m_Font.GetObject(sizeof(lf), &lf);
 #endif
        dlg.m_Font.CreateFontIndirect(&lf);
        dlg.m_pInfo = m_pInfo;
        dlg.m_cR = m_cRCreate;
        dlg.DoModal();
 }