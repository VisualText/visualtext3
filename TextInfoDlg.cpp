/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// TextInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sys/stat.h"
#include "kb.h"
#include "Utils.h"
#include "VisualText.h"
#include "TextInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextInfoDlg dialog


CTextInfoDlg::CTextInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextInfoDlg)
	m_stringCharacterCount = _T("");
	m_stringFileSize = _T("");
	m_stringLastModified = _T("");
	m_stringLineCount = _T("");
	m_stringWordCount = _T("");
	m_stringFilePath = _T("");
	m_stringCreated = _T("");
	//}}AFX_DATA_INIT
}


void CTextInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextInfoDlg)
	DDX_Text(pDX, IDC_CHARACTER_COUNT, m_stringCharacterCount);
	DDX_Text(pDX, IDC_FILE_SIZE, m_stringFileSize);
	DDX_Text(pDX, IDC_LAST_MODIFIED, m_stringLastModified);
	DDX_Text(pDX, IDC_LINE_COUNT, m_stringLineCount);
	DDX_Text(pDX, IDC_WORD_COUNT, m_stringWordCount);
	DDX_Text(pDX, IDC_FILE_PATH, m_stringFilePath);
	DDX_Text(pDX, IDC_CREATED, m_stringCreated);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CTextInfoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextInfoDlg message handlers


void CTextInfoDlg::SetData(CString fullFilePathStr)
{
	m_stringFilePath = fullFilePathStr;
}

#define TIME_SIZE 26

void CTextInfoDlg::GetFileInfo()
{
	if (m_stringFilePath == _T(""))
		return;

	// DO FILE STAT STUFF
	_TCHAR fileChar[1001];
	StringToChar(fileChar,m_stringFilePath,1000);
	struct _stat buf;
	int result = _tstat(fileChar, &buf);
	int fileSize = 0;
	if (result == 0) {
		_TCHAR buffer[101];

		time_t t = buf.st_ctime;
		_tctime_s(buffer, TIME_SIZE, &t);
		m_stringCreated = buffer;

		t = buf.st_mtime;
		_tctime_s(buffer, TIME_SIZE, &t);
		m_stringLastModified = buffer;
	}

	CString contentStr = NLPReadFile(m_stringFilePath);
	int len = contentStr.GetLength();
	int characterCount = 0;
	int wordCount = 0;
	int lineCount = 0;
	_TCHAR c;
	_TCHAR lastC = '\0';
	bool moreThanOneLine = false;

	for (int i=0; i<len; i++) {
		c = contentStr.GetAt(i);

		if (c == '\n') {
			lineCount++;
			moreThanOneLine = true;
		}

		if (_istspace(lastC) && !_istspace(c))
			wordCount++;
		lastC = c;

		characterCount++;
	}

	if (moreThanOneLine) {
		lineCount++;
		wordCount++;
		characterCount--;
	}

	m_stringLineCount = PrettyNumber(lineCount);
	m_stringWordCount = PrettyNumber(wordCount);
	m_stringCharacterCount = PrettyNumber(characterCount);
	m_stringFileSize = PrettySize(fileSize);

	//fclose(fp);
	//f.Close();

	UpdateData(false);
}

BOOL CTextInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetFileInfo();
	
	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}
