/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// FileCharViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include <strstream>	// 05/11/02 AM.
#include "kb.h"
#include "Utils.h"
#include "VisualText.h"
#include "lite/nlp.h"
#include "FileCharViewerDlg.h"
#include ".\filecharviewerdlg.h"

extern NLP *nlp;
extern CFont appFontFixed;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileCharViewerDlg dialog


CFileCharViewerDlg::CFileCharViewerDlg(CWnd* pParent /*=NULL*/)
	: CXTPResizeDialog(CFileCharViewerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileCharViewerDlg)
	m_strDisplay = _T("");
	//}}AFX_DATA_INIT
}


void CFileCharViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTPResizeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileCharViewerDlg)
	DDX_Text(pDX, IDC_CHAR_DISPLAY, m_strDisplay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileCharViewerDlg, CXTPResizeDialog)
	//{{AFX_MSG_MAP(CFileCharViewerDlg)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileCharViewerDlg message handlers

void CFileCharViewerDlg::SetData(CString filePathStr)
{
	m_strFilePath = filePathStr;
}

void CFileCharViewerDlg::GenerateDisplayStr()
{
	m_strDisplay = _T("");
	int i;
	int lineCount = 1;
	int lineCharCount = 0;
	int charCount = 0;
	int scaleCount = 0;
	CString lineStr = _T("");
	CString hexStr = _T("");
	CString workStr = _T("");
	CString codeStr = _T("");
	CString scaleStr = _T("");

	_TCHAR c;
	long fileLength = -1;
	int scaleIndent = 30;
#ifdef UNICODE
	scaleIndent = 45;
#endif
	int scaleSize = 0;
	int sc = 0;
	int lw = 0;
	int lc = 0;
	int l = 0;
	int adjust = 0;
	int every = 10;
	bool firstOne = true;

	CString cStr = NLPReadFile(m_strFilePath);
	int len = cStr.GetLength();

	for (i=0; i<len; i++) {
		c = cStr.GetAt(i);
		lineCharCount++;
		charCount++;

		codeStr.Format(_T("%2x "),c);

#ifdef UNICODE
		lc = codeStr.GetLength();
		for (int j=0; j<5-lc; j++) {
			codeStr += _T(" ");
		}
#endif

		adjust = 0;
		if (c == '\n')
			workStr = _T("\\n ");
		else if (c == '\t')
			workStr = _T("\\t ");
		else if (l == 5) {
			workStr = c;
			adjust = 1;
		} else
			workStr.Format(_T("%2c"), c);

		lw = 4;
#ifdef UNICODE
		lw = workStr.GetLength() + adjust;
#endif
		for (int j=0; j<5-lw; j++) {
			workStr += _T(" ");
		}

		lineStr += workStr;
		hexStr += codeStr;

		// Print out the counter number every 10
		if (!(scaleCount % every)) {
			workStr.Format(_T("%d"),scaleCount);
			sc = workStr.GetLength();
			if (sc > scaleSize) {
				scaleIndent--;
				scaleSize = sc;
			}
			if (!firstOne)
				scaleStr += IndentStr(scaleIndent,0);
			firstOne = false;
			scaleStr += workStr;
		}

		scaleCount++;

		if (c == '\n') {
			workStr.Format(_T("%05d:%08d:%05d:"),lineCount,charCount,lineCharCount);
			m_strDisplay += workStr;
#ifdef UNICODE
			m_strDisplay += _T("\r\n");
			m_strDisplay += IndentStr(21,0);
#endif
			m_strDisplay += lineStr;
			m_strDisplay += _T("\r\n");
			m_strDisplay += IndentStr(21,0);
			m_strDisplay += hexStr;
			m_strDisplay += _T("\r\n");
			m_strDisplay += IndentStr(22,0);
			m_strDisplay += scaleStr;
			m_strDisplay += _T("\r\n");
			lineStr = _T("");
			hexStr = _T("");
			scaleStr = _T("");
			lineCount++;
			lineCharCount = 0;
			//scaleCount = 0;

			firstOne = true;
			workStr.Format(_T("%d"),scaleCount);
			sc = workStr.GetLength();
			if (scaleCount % every) {
				scaleStr += IndentStr((every - (scaleCount % every)) * 3 - sc + 1,0);
			}
		}
	}

	// Very bad coding. This should be in a subroutine.  de Hilster 2012-05-20
	if (m_strDisplay == _T("")) {
		workStr.Format(_T("%05d:%08d:%05d:"),lineCount,charCount,lineCharCount);
		m_strDisplay += workStr;
#ifdef UNICODE
		m_strDisplay += _T("\r\n");
		m_strDisplay += IndentStr(21,0);
#endif
		m_strDisplay += lineStr;
		m_strDisplay += _T("\r\n");
		m_strDisplay += IndentStr(21,0);
		m_strDisplay += hexStr;
		m_strDisplay += _T("\r\n");
		m_strDisplay += IndentStr(22,0);
		m_strDisplay += scaleStr;
		m_strDisplay += _T("\r\n");
		lineStr = _T("");
		hexStr = _T("");
		scaleStr = _T("");
		lineCount++;
		lineCharCount = 0;
		//scaleCount = 0;

		firstOne = true;
		workStr.Format(_T("%d"),scaleCount);
		sc = workStr.GetLength();
		if (scaleCount % every)
			scaleStr += IndentStr((every - (scaleCount % every)) * 3 - sc + 1,0);
	}
}

BOOL CFileCharViewerDlg::OnInitDialog() 
{
	CXTPResizeDialog::OnInitDialog();
	
	CEdit *ed = (CEdit *)GetDlgItem(IDC_CHAR_DISPLAY);
	ed->SetFont(&appFontFixed);

	GenerateDisplayStr();

	SetResize(IDC_CHAR_DISPLAY, XTP_ANCHOR_TOPLEFT, XTP_ANCHOR_BOTTOMRIGHT);
	SetResize(IDOK, XTP_ANCHOR_BOTTOMCENTER, XTP_ANCHOR_BOTTOMCENTER);
	
	LoadPlacement(_T("CFileCharViewerDlg"));

	UpdateData(FALSE);

	CString strWindowTitle;
	strWindowTitle.Format(_T("Character Viewer: %s"),m_strFilePath);
	SetWindowText(strWindowTitle);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileCharViewerDlg::OnDestroy()
{
	CXTPResizeDialog::OnDestroy();

	SavePlacement(_T("CFileCharViewerDlg"));

	// TODO: Add your message handler code here
}
