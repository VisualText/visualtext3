/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// CustomMsgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "CustomMsgDlg.h"

extern int appButtonReturn;

#define CUSTOM_DLG_BUTTON_1 9989991
#define CUSTOM_DLG_BUTTON_2 9989992
#define CUSTOM_DLG_BUTTON_3 9989993

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomMsgDlg dialog


CCustomMsgDlg::CCustomMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomMsgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomMsgDlg)
	m_strMsg = _T("");
	//}}AFX_DATA_INIT
}


void CCustomMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomMsgDlg)
	DDX_Text(pDX, IDC_MESSAGE, m_strMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomMsgDlg, CDialog)
	//{{AFX_MSG_MAP(CCustomMsgDlg)
	ON_BN_CLICKED(CUSTOM_DLG_BUTTON_1, OnButtonPushed1)
	ON_BN_CLICKED(CUSTOM_DLG_BUTTON_2, OnButtonPushed2)
	ON_BN_CLICKED(CUSTOM_DLG_BUTTON_3, OnButtonPushed3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomMsgDlg message handlers

void CCustomMsgDlg::SetData(CString msgStr, int numButtons, int defaultChoice,
							CString butt1Str, CString butt2Str, CString butt3Str)
{
	m_strMsg = msgStr;
	m_intNumButtons = numButtons;
	m_intDefaultChoice = defaultChoice;
	m_strButton1 = butt1Str;
	m_strButton2 = butt2Str;
	m_strButton3 = butt3Str;
}

BOOL CCustomMsgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

#ifdef UNICODE
	m_fontButton.CreateFont(-12, 0,0,0,0,0,0,0,0,0,0,0,0, _T("Arial Unicode MS"));
#else
	m_fontButton.CreateFont(-12, 0,0,0,0,0,0,0,0,0,0,0,0, _T("Arial"));
#endif
	m_pDC = GetParentFrame()->GetDC();	
	m_pDC->SelectObject(&m_fontButton);

	CreateButtons();

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCustomMsgDlg::CreateButtons() 
{
	CRect dialogRect;
	GetWindowRect(dialogRect);
	CRect rect;
	int margin = 0;
	int buttonHeight = 25;
	int buttonWidth1 = 0;
	int buttonWidth2 = 0;
	int buttonWidth3 = 0;
	int spacer = 5;

	if (m_intNumButtons >= 1)
		buttonWidth1 = ButtonWidthFromStr(m_strButton1);
	if (m_intNumButtons >= 2)
		buttonWidth2 = ButtonWidthFromStr(m_strButton2);
	if (m_intNumButtons >= 3)
		buttonWidth3 = ButtonWidthFromStr(m_strButton3);

	int buttonsWidth = buttonWidth1 + buttonWidth2 + buttonWidth3 + (2 * spacer);
	if (dialogRect.Width() > buttonsWidth) {
		margin = (dialogRect.Width() - buttonsWidth) / 2;
		int x = margin;
		int y = 105;

		if (m_intNumButtons >= 1) {
			rect.SetRect(x,y,x+buttonWidth1,y+buttonHeight);
			m_button1.Create(m_strButton1,BS_PUSHBUTTON,rect,this,CUSTOM_DLG_BUTTON_1);
			m_button1.SetFont(&m_fontButton);
			m_button1.ShowWindow(SW_SHOW);
			if (m_intDefaultChoice == 1)
				m_button1.SetFocus();
		}
		if (m_intNumButtons >= 2) {
			x += buttonWidth1 + spacer;
			rect.SetRect(x,y,x+buttonWidth2,y+buttonHeight);
			m_button2.Create(m_strButton2,BS_PUSHBUTTON,rect,this,CUSTOM_DLG_BUTTON_2);
			m_button2.SetFont(&m_fontButton);
			m_button2.ShowWindow(SW_SHOW);
			if (m_intDefaultChoice == 2)
				m_button1.SetFocus();
		}
		if (m_intNumButtons >= 3) {
			x += buttonWidth2 + spacer;
			rect.SetRect(x,y,x+buttonWidth3,y+buttonHeight);
			m_button3.Create(m_strButton3,BS_PUSHBUTTON,rect,this,CUSTOM_DLG_BUTTON_3);
			m_button3.SetFont(&m_fontButton);
			m_button3.ShowWindow(SW_SHOW);
			if (m_intDefaultChoice == 3)
				m_button1.SetFocus();
		}
	}
	else {
		int x = (dialogRect.Width() - buttonWidth1) / 2;
		int y = 65;
		if (m_intNumButtons >= 1) {
			rect.SetRect(x,y,x+buttonWidth1,y+buttonHeight);
			m_button1.Create(m_strButton1,BS_PUSHBUTTON,rect,this,CUSTOM_DLG_BUTTON_1);
			m_button1.SetFont(&m_fontButton);
			m_button1.ShowWindow(SW_SHOW);
			if (m_intDefaultChoice == 1)
				m_button1.SetFocus();
		}
		x = (dialogRect.Width() - buttonWidth2) / 2;
		y += rect.Height() + spacer;
		if (m_intNumButtons >= 2) {
			rect.SetRect(x,y,x+buttonWidth2,y+buttonHeight);
			m_button2.Create(m_strButton2,BS_PUSHBUTTON,rect,this,CUSTOM_DLG_BUTTON_2);
			m_button2.SetFont(&m_fontButton);
			m_button2.ShowWindow(SW_SHOW);
			if (m_intDefaultChoice == 2)
				m_button1.SetFocus();
		}
		x = (dialogRect.Width() - buttonWidth3) / 2;
		y += rect.Height() + spacer;
		if (m_intNumButtons >= 3) {
			rect.SetRect(x,y,x+buttonWidth3,y+buttonHeight);
			m_button3.Create(m_strButton3,BS_PUSHBUTTON,rect,this,CUSTOM_DLG_BUTTON_3);
			m_button3.SetFont(&m_fontButton);
			m_button3.ShowWindow(SW_SHOW);
			if (m_intDefaultChoice == 3)
				m_button1.SetFocus();
		}
		y += rect.Height() + spacer;
		if (dialogRect.Height() < y)
			SetWindowPos(NULL, 0, 0, dialogRect.Width(), y, SWP_NOMOVE | SWP_NOZORDER);
	}
}

int CCustomMsgDlg::ButtonWidthFromStr(CString buttonStr)
{
	CSize strSize;
	strSize = m_pDC->GetTextExtent(buttonStr);
	return strSize.cx + 10;
}

void CCustomMsgDlg::OnButtonPushed1()
{
	OuttaHere(1);
}

void CCustomMsgDlg::OnButtonPushed2()
{
	OuttaHere(2);
}

void CCustomMsgDlg::OnButtonPushed3()
{
	OuttaHere(3);
}

void CCustomMsgDlg::OuttaHere(int buttonNum)
{
	appButtonReturn = buttonNum;
	OnOK();
}

