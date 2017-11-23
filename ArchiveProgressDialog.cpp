/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// ArchiveProgressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "VisualText.h"
#include "ArchiveProgressDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArchiveProgressDialog dialog


CArchiveProgressDialog::CArchiveProgressDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CArchiveProgressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CArchiveProgressDialog)
	m_strArchiveMessage = _T("");
	m_strProgressMessage = _T("");
	//}}AFX_DATA_INIT

	m_intLastPercent = 0;
	m_longFileSize = 0L;
	m_boolCancel = false;
}


void CArchiveProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArchiveProgressDialog)
	DDX_Control(pDX, IDC_PROGRESS, m_progressCtrl);
	DDX_Text(pDX, IDC_ARCHIVE_MESSAGE, m_strArchiveMessage);
	DDX_Text(pDX, IDC_ARCHIVE_PROGRESS_MESSAGE, m_strProgressMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArchiveProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CArchiveProgressDialog)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArchiveProgressDialog message handlers

void CArchiveProgressDialog::SetProgressMessage(CString msgStr)
{
	m_strProgressMessage = msgStr;
	UpdateData(false);
}

void CArchiveProgressDialog::SetArchiveMessage(CString archiveMsgStr)
{
	m_strArchiveMessage = archiveMsgStr;
	UpdateData(false);
}

void CArchiveProgressDialog::SetRange(int lower, int upper)
{
	m_progressCtrl.SetRange(lower,upper);
}

void CArchiveProgressDialog::SetStep(int step)
{
	m_progressCtrl.SetStep(step);
}

void CArchiveProgressDialog::SetFileSize(long fileSize)
{
	m_longFileSize = fileSize;
}

bool CArchiveProgressDialog::StepIt()
{
	int lower,upper;
	m_progressCtrl.GetRange(lower,upper);
	int current = m_progressCtrl.GetPos();
	int percent = (current * 100) / upper;

	if (percent <= 100 && percent != m_intLastPercent) {
		CString progressStr;
//		long soFar = percent * m_longFileSize / LARGE_TEXT_SIZE;
		long total = m_longFileSize / 1000;
		long soFar = MulDiv(percent, total, 100); // SRP 010503
		progressStr.Format(_T("Loaded %d%%   (%d kb of %d kb)"),percent,soFar,total);
		SetProgressMessage(progressStr);
	}

	if (percent < 100)
		m_progressCtrl.StepIt();
	m_intLastPercent = percent;

	return m_boolCancel;
}

void CArchiveProgressDialog::OnCancel() 
{
	m_boolCancel = true;
}
