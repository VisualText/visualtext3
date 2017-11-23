/*******************************************************************************
Copyright (c) 1998-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// ArchiveListDialog.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "resource.h"
#include "Utils.h"
#include "MainFrm.h"
#include "FTPclient.h"
#include "TreeViewAnalyzer.h"
#include "TreeViewGrammar.h"
#include "TreeViewText.h"
#include "ArchiveListDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CVisualTextApp theApp;
extern CString appAskText;
extern CString appNameStr;
extern bool appBoolAnalyzerLoaded;

/////////////////////////////////////////////////////////////////////////////
// CArchiveListDialog dialog


CArchiveListDialog::CArchiveListDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CArchiveListDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CArchiveListDialog)
	m_strLocalPath = _T("");
	//}}AFX_DATA_INIT
}


void CArchiveListDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArchiveListDialog)
	DDX_Control(pDX, IDC_ARCHIVE_LIST, m_listCtrlArchive);
	DDX_Text(pDX, IDC_PATH, m_strLocalPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArchiveListDialog, CDialog)
	//{{AFX_MSG_MAP(CArchiveListDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_ARCHIVE_LIST, OnDblclkArchiveList)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_DOWNLOAD, OnDownload)
	ON_BN_CLICKED(IDC_RENAME, OnRename)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_ARCHIVE_LIST, OnColumnclickArchiveList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_ARCHIVE_LIST, OnDeleteitemArchiveList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArchiveListDialog message handlers

class CArchiveItem {

public:
	CString m_Name;
	CTime m_Time;
	DWORD m_Size;

public:
	CArchiveItem();
	CArchiveItem(LPCTSTR name, time_t time, DWORD dwSize);
	~CArchiveItem();
};

CArchiveItem::CArchiveItem()
{
	m_Name.Empty();
	m_Time = CTime(0);
	m_Size = 0;
}

CArchiveItem::CArchiveItem(LPCTSTR name, time_t time, DWORD dwSize)
{
	m_Name = name;
	m_Time = CTime(time);
	m_Size = dwSize;
}

CArchiveItem::~CArchiveItem()
{
}

#define COL_NAME 0
#define COL_DATE 1
#define COL_SIZE 2

static int iLastSubItem;
static BOOL bAscendSort;

int CALLBACK ARCHV_CompareFunction (CArchiveItem* pItem1, CArchiveItem* pItem2, LPARAM lCol)
{
	int nCmp = 0;
	switch( lCol ) {
		case COL_NAME:
			nCmp = lstrcmpi(pItem1->m_Name, pItem2->m_Name);
			break;
		case COL_DATE:
			nCmp = pItem1->m_Time > pItem2->m_Time ? 1 : pItem1->m_Time < pItem2->m_Time ? -1 : 0;
			break;
		case COL_SIZE:
			nCmp = pItem1->m_Size > pItem2->m_Size ? 1 : pItem1->m_Size < pItem2->m_Size ? -1 : 0;
			break;
		default:
			return 0;
	}
	if (!bAscendSort)
		nCmp = -nCmp;
	return nCmp;
}

void CArchiveListDialog::OnColumnclickArchiveList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (iLastSubItem != pNMListView->iSubItem) {
		iLastSubItem = pNMListView->iSubItem;
		bAscendSort = TRUE;
	}
	// TODO: Add your control notification handler code here
	m_listCtrlArchive.SortItems((PFNLVCOMPARE)ARCHV_CompareFunction, pNMListView->iSubItem);
	*pResult = 0;
	bAscendSort = !bAscendSort;
}

void CArchiveListDialog::OnDeleteitemArchiveList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	CArchiveItem* pItem = (CArchiveItem*)pNMListView->lParam;
	if (pItem) delete pItem;
	pNMListView->lParam = 0;
	*pResult = 0;
}

//////////////////////////////////////////////////

BOOL CArchiveListDialog::OnInitDialog() 
{
	m_listCtrlArchive.AutoLoad(IDC_ARCHIVE_LIST, this, IDC_LOAD, COL_NAME);
	CDialog::OnInitDialog();

	m_listCtrlArchive.InsertColumn(0,_T("Archive"),LVCFMT_LEFT,200);
	m_listCtrlArchive.InsertColumn(1,_T("Date"),LVCFMT_LEFT,100);
	m_listCtrlArchive.InsertColumn(2,_T("Size"),LVCFMT_LEFT,75);
	m_listCtrlArchive.SetExtendedStyle(LVS_EX_TWOCLICKACTIVATE |
		LVS_EX_FULLROWSELECT | LVS_EX_UNDERLINECOLD | LVS_EX_UNDERLINEHOT);	
	
	if (appBoolAnalyzerLoaded) {
		if (m_strArchiveType == _T("Analyzer"))
			m_strLocalPath = AppPath();
		else if (m_strArchiveType == _T("Grammar"))
			m_strLocalPath = KBPath(_T("user"));
		else if (m_strArchiveType == _T("Rules"))
			m_strLocalPath = RulesPath();
		else if (m_strArchiveType == _T("Text"))
			m_strLocalPath = InputPath();
	}
	else {
		m_strLocalPath = theApp.GetAppsDir();
	}

	CString destTitleStr;
	if (m_destinationType == ARCHIVE_DEST_SERVER) {
		LoadServerDirectory();
		destTitleStr = _T("View Remote");	// 05/19/01 AM.
	}
	else {
		LoadLocalDirectory();
		destTitleStr = _T("View Local");	// 05/19/01 AM.
		CWnd *downloadButton = GetDlgItem(IDC_DOWNLOAD);
		downloadButton->SetWindowText(_T("Upload"));
	}

	if (m_destinationType != ARCHIVE_DEST_LOCAL) {
		CWnd *browseButton = GetDlgItem(IDC_BROWSE);
		browseButton->EnableWindow(false);
	}

	if (m_strArchiveType == _T("Grammar"))	// 05/18/01 AM.
		SetWindowText(_T("Gram Archive > ")+destTitleStr);	// 05/18/01 AM.
	else if (m_strArchiveType == _T("Rules"))	// 05/18/01 AM.
		SetWindowText(_T("Ana Archive > ")+destTitleStr);	// 05/18/01 AM.
	else	// 05/18/01 AM.
		SetWindowText(m_strArchiveType+_T(" Archive > ")+destTitleStr);//05/18/01 AM.

	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

CTime GetCTime(const CString& monthStr, const CString& dayStr, const CString& hourYearStr)
{
	int yr, month, day, hour, min;
	int pos = hourYearStr.Find(':');
	CTime currentTime = CTime::GetCurrentTime();

	if (-1 == pos) {
		yr = _ttoi(hourYearStr);
		hour = 0;
		min = 0;
	}
	else {	
		yr = currentTime.GetYear();
		CString strHour = hourYearStr.Left(pos);
		CString strMin = hourYearStr.Mid(pos + 1);
		hour = _ttoi(strHour);
		min = _ttoi(strMin);
	}
	day = _ttoi(dayStr);
	CString strMonthList[] = {_T(""), _T("Jan"), _T("Feb"), _T("Mar"), _T("Apr"), _T("May"), _T("Jun"), _T("Jul"), _T("Aug"), _T("Sep"), _T("Oct"), _T("Nov"), _T("Dec")};
	for (month = 1; month <= 12; month++) {
		if (!monthStr.CompareNoCase(strMonthList[month]))
			break;
	}
	CTime genTime = CTime(yr,month,day,hour,min,0);
	CTimeSpan span = genTime - currentTime;
	if (genTime > currentTime && span.GetDays() > 1)
		genTime = CTime(--yr,month,day,hour,min,0);

	return genTime;
}

void CArchiveListDialog::LoadServerDirectory()
{
	if (appAskText != _T("")) {
		CString archiveStr;
		CStringList dirItems;

		CStringList archives;
		ArchiveStringList(archives,appAskText);
		POSITION pos = archives.GetHeadPosition();
		CString monthStr,dayStr,hourYearStr,yearStr;
		CString archiveNameStr,fileSizeStr,dateStr;
		CString word = _T("");
		CTime obTime;
		int count = 0;

		while (pos && (archiveStr = archives.GetNext(pos))) {
			StringToStringList(dirItems,archiveStr,_T(" "));
			archiveNameStr = NthStringListItem(dirItems,8);
			fileSizeStr = NthStringListItem(dirItems,4);
			if (archiveNameStr.Find('.') != 0) {
				monthStr = NthStringListItem(dirItems,5);
				dayStr = NthStringListItem(dirItems,6);
				hourYearStr = NthStringListItem(dirItems,7);
				yearStr = NthStringListItem(dirItems,8);
				obTime = GetCTime(monthStr,dayStr,hourYearStr);
				if (-1 == hourYearStr.Find(':'))
					dateStr = obTime.Format(_T("%y %b %d"));
				else
					dateStr = obTime.Format(_T("%y %b %d, %H:%M"));

				ClipRight(archiveNameStr,4);
				m_listCtrlArchive.InsertItem(count,archiveNameStr);
				m_listCtrlArchive.SetItemText(count,1,dateStr);
				m_listCtrlArchive.SetItemText(count,2,fileSizeStr);

				CArchiveItem* pData = new CArchiveItem(archiveNameStr, obTime.GetTime(), _ttoi(fileSizeStr));
				m_listCtrlArchive.SetItemData(count, (DWORD)pData);

				count++;
			}
		}

		m_strLocalPath = theApp.GetProfileString(_T("PREFERENCES"),_T("ARCHIVEDIRECTORY"),_T(""));
		UpdateData(false);
		bAscendSort = true;
		m_listCtrlArchive.SortItems((PFNLVCOMPARE)ARCHV_CompareFunction, 0);
		iLastSubItem = 0;
		bAscendSort = false;
	}
}

void CArchiveListDialog::LoadLocalDirectory()
{
	CFileFind finder;
	BOOL bWorking = finder.FindFile(m_strLocalPath+_T("\\*.zip"));
	CString fileHeaderStr;
	CString fileNameStr;
	CString fileSizeStr;
	CString fileDateStr;
	CTime time;
	int count = 0;
	m_listCtrlArchive.DeleteAllItems();
		
	while (bWorking) {
		bWorking = finder.FindNextFile();
		fileNameStr = finder.GetFileName();
		finder.GetLastWriteTime(time);
		fileDateStr = time.Format(_T("%y %b %d, %H:%M"));
		fileSizeStr.Format(_T("%d"),finder.GetLength());

		ClipRight(fileNameStr,4);
		m_listCtrlArchive.InsertItem(count,fileNameStr);
		m_listCtrlArchive.SetItemText(count,1,fileDateStr);
		m_listCtrlArchive.SetItemText(count,2,fileSizeStr);

		CArchiveItem* pData = new CArchiveItem(fileNameStr, time.GetTime(), static_cast<DWORD>(finder.GetLength()));
		m_listCtrlArchive.SetItemData(count, (DWORD)pData); 

		count++;
	}
	UpdateData(false);

	bAscendSort = true;
	m_listCtrlArchive.SortItems((PFNLVCOMPARE)ARCHV_CompareFunction, 0);
	iLastSubItem = 0;
	bAscendSort = false;
}

void CArchiveListDialog::ArchiveStringList(CStringList &archiveList, CString dirStr)
{
	CStringList dirList;
	StringToStringList(dirList,dirStr,_T("\r\n"));

	POSITION pos = dirList.GetHeadPosition();
	CString line = _T("");
	while (pos && (line = dirList.GetNext(pos))) {
		if (line != _T("") &&
			line.Find(_T("total ")) != 0 &&
			line.GetAt(0) != '.')
			archiveList.AddTail(line);
	}
}

void CArchiveListDialog::OnDblclkArchiveList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnDownload();
		
	*pResult = 0;
}

void CArchiveListDialog::DownloadSelectedArchives(bool upload) 
{
	POSITION pos = m_listCtrlArchive.GetFirstSelectedItemPosition();
	
	if (pos != NULL) {
		int nItem;
		CString remoteFileStr;
		CString pathStr;
		CString fileSizeStr;
		long fileSize;

		CString localFileStr;
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		wnd->ClearOutput();

		while (pos) {
			nItem = m_listCtrlArchive.GetNextSelectedItem(pos);
			remoteFileStr = m_listCtrlArchive.GetItemText(nItem,0) + _T(".zip");
			fileSizeStr = m_listCtrlArchive.GetItemText(nItem,2);
			fileSize = _ttol((const _TCHAR *)fileSizeStr);
			localFileStr = m_strLocalPath + _T("\\") + remoteFileStr;

			if (upload)
				DownloadArchive(m_strArchiveType,remoteFileStr,localFileStr,0);
			else
				DownloadArchive(m_strArchiveType,remoteFileStr,localFileStr,fileSize);
		}
	}

}

void CArchiveListDialog::OnDelete() 
{
	POSITION pos = m_listCtrlArchive.GetFirstSelectedItemPosition();
	
	if (pos != NULL) {
		int nItem;
		CString archiveStr;
		CString archiveStrs = _T("");

		while (pos) {
			nItem = m_listCtrlArchive.GetNextSelectedItem(pos);
			archiveStr = m_listCtrlArchive.GetItemText(nItem,0);
			if (archiveStrs != _T(""))
				archiveStrs += _T(",");
			archiveStrs += archiveStr;
		}

		CString questionStr = _T("Do you really want to delete these archives:\n");
		if (AfxMessageBox(questionStr+archiveStrs, MB_YESNO) == IDYES) {
			pos = m_listCtrlArchive.GetFirstSelectedItemPosition();
			while (pos) {
				nItem = m_listCtrlArchive.GetNextSelectedItem(pos);
				archiveStr = m_listCtrlArchive.GetItemText(nItem,0) + _T(".zip");

				if (m_destinationType == ARCHIVE_DEST_LOCAL)
					DeleteFile(m_strLocalPath+_T("\\")+archiveStr);
				else if (m_destinationType == ARCHIVE_DEST_SERVER)
					DeleteArchive(m_strArchiveType,archiveStr);

				// BAD CODE!!! MUST FIX IF EVER MULTIPLE CHOOSING FOR DELETE!!!!
				m_listCtrlArchive.DeleteItem(nItem);

				UpdateData(false);
			}
		}
	}
}

void CArchiveListDialog::OnRename() 
{
	if (m_listCtrlArchive.GetSelectedCount() == 0) {
		AfxMessageBox(_T("Must choose an archive to rename!"));
		return;
	}
	else if (m_listCtrlArchive.GetSelectedCount() > 1) {
		AfxMessageBox(_T("Choose only one archive to rename at a time!"));
		return;
	}

	POSITION pos = m_listCtrlArchive.GetFirstSelectedItemPosition();
	
	if (pos != NULL) {
		int nItem = m_listCtrlArchive.GetNextSelectedItem(pos);
		CString archiveStr = m_listCtrlArchive.GetItemText(nItem,0) + _T(".zip");
		CString msgStr;
		msgStr.Format(_T("Rename archive \"%s\" to:"),archiveStr);

		if (AskForText(msgStr)) {
			if (m_destinationType == ARCHIVE_DEST_LOCAL)
				_trename(m_strLocalPath+_T("\\")+archiveStr,m_strLocalPath+_T("\\")+appAskText+_T(".zip"));
			else if (m_destinationType == ARCHIVE_DEST_SERVER)
				RenameArchive(m_strArchiveType,archiveStr,appAskText);

			m_listCtrlArchive.SetItemText(nItem,0,appAskText);
			UpdateData(false);
		}
	}
}

void CArchiveListDialog::OnDownload() 
{
	DownloadSelectedArchives(m_destinationType == ARCHIVE_DEST_LOCAL);
}

void CArchiveListDialog::SetArchiveType(CString archiveStr)
{
	m_strArchiveType = archiveStr;
}

void CArchiveListDialog::SetDestinationType(ARCHIVE_DEST_TYPE destinationType)
{
	m_destinationType = destinationType;
}

void CArchiveListDialog::OnLoad() 
{
	POSITION pos = m_listCtrlArchive.GetFirstSelectedItemPosition();
	
	if (pos != NULL) {
		CString oldLocalPathStr = m_strLocalPath;
		if (m_strArchiveType == _T("Analyzer")) {
			if (theApp.CreateNewAnalyzer(false,false))
				m_strLocalPath = AppPath();
			else {
				return;
			}
		}

		int nItem;
		bool notCanceled = true;
		CString archiveStr;
		pos = m_listCtrlArchive.GetFirstSelectedItemPosition();
		nItem = m_listCtrlArchive.GetNextSelectedItem(pos);
		archiveStr = m_listCtrlArchive.GetItemText(nItem,0) + _T(".zip");
		CString fileSizeStr = m_listCtrlArchive.GetItemText(nItem,2);
		long fileSize = _ttol((const _TCHAR *)fileSizeStr); 
		CString localPathStr = m_strLocalPath + _T("\\") + archiveStr;

		// IF LOCAL FILE, COPY THE ZIP TO THE NEW DIRECTORY
		if (m_destinationType == ARCHIVE_DEST_LOCAL) {
			CString zipOrigPathStr = oldLocalPathStr + _T("\\") + archiveStr;
			CopyFile(zipOrigPathStr,localPathStr,false);
		}

		if (m_destinationType == ARCHIVE_DEST_SERVER)
			notCanceled = DownloadArchive(m_strArchiveType,archiveStr,localPathStr,fileSize);

		if (notCanceled) {
			LoadArchive(localPathStr);

			// UPDATE TITLE
			if (m_strArchiveType == _T("Analyzer")) {
				CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
				wnd->OnUpdateFrameTitle(false);
				theApp.AddRecentAnalyzer(AppPath());
			}
		}
	}
}

void CArchiveListDialog::LoadArchive(CString localFileStr)
{
	UnarchiveFiles(localFileStr,m_strLocalPath);
	bool loaded = false;

	if (m_strArchiveType == _T("Analyzer")) {
		CStringList files;
		DirectoryFiles(files,m_strLocalPath,true,_T("*.ana"));
		if (files.GetCount() == 0) {
			return;
		}

		theApp.CloseAnalyzer();

		// CREATE NEW NAME
		//CString anaFileStr = NthStringListItem(files,0);
		//appNameStr = StripPath(anaFileStr,0);
		CString anaFileStr = NthStringListItem(files,0);
		CString newAppPathStr = StripPath(localFileStr,0,true);
		CString appNameStr = StripPath(newAppPathStr,0);
		CString newAnaPathStr;
		newAnaPathStr.Format(_T("%s\\%s.ana"),newAppPathStr,appNameStr);
		theApp.ReadAnalyzerStatusFile(newAnaPathStr);

		loaded = theApp.OpenAnalyzer();

		// DELETE THE OLD .ANA FILES IF DIFFERENT
		if (files.GetCount() > 1) {
			POSITION pos = files.GetHeadPosition();
			CString fileStr;
			while (pos && (fileStr = files.GetNext(pos))) {
				if (fileStr.CompareNoCase(newAnaPathStr))
					DeleteFile(fileStr);
			}
		}
		else if (anaFileStr.CompareNoCase(newAnaPathStr)) {
			_trename(anaFileStr,newAnaPathStr);
		}
	}
	else if (m_strArchiveType == _T("Grammar")) {
		theApp.m_pMainFrame->m_wndGrammarView.Clear();
		theApp.m_pMainFrame->m_wndGrammarView.Load();
		loaded = true;
	}
	else if (m_strArchiveType == _T("Rules")) {
		theApp.m_pMainFrame->m_wndAnalyzerView.Clear();
		theApp.m_pMainFrame->m_wndAnalyzerView.Load();
		loaded = true;
	}
	else if (m_strArchiveType == _T("Text")) {
		theApp.m_pMainFrame->m_wndTextView.Clear();
		theApp.m_pMainFrame->m_wndTextView.Load();
		loaded = true;
	}

	if (!loaded)
		AfxMessageBox(_T("Couldn't load Analyzer!"));
}

void CArchiveListDialog::OnBrowse() 
{
	if (UTLSelectFolder(m_hWnd,_T("Please choose an archive folder location"),m_strLocalPath,true)) {
		LoadLocalDirectory();
	}
}

