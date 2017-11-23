/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// PrefLookupPage.cpp : implementation file
//

#include "stdafx.h"
#include "string.h"
#include "kb.h"
#include "visualtext.h"
#include "PrefLookupPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CVisualTextApp theApp;
///////////////////////////////////////////

CLookupItem::CLookupItem()
{
	m_Name.Empty();
	m_URL.Empty();
}

CLookupItem::CLookupItem(LPCTSTR name, LPCTSTR URL)
{
	m_Name = name;
	m_URL = URL;
}

CLookupItem::~CLookupItem()
{
}

#define DELIMITER1 0x1
int CLookupItem::FormatData(CString &cData)
{
	cData.Empty ();
	cData.Format (_T("%s%c%s"), m_Name, DELIMITER1, m_URL);
	return cData.GetLength ();
}

void CLookupItem::RetrieveData(LPCTSTR lpData)
{
	CString strData = lpData;
	int pos;
	if ((pos = strData.Find(DELIMITER1)) != -1) {
		m_Name = strData.Left(pos);
		m_URL = strData.Mid(pos + 1);
	}
}

///////////////////////////////////////////////////////

static int iLastSubItem;
static BOOL bAscendSort;

#define COL_NAME	0
#define COL_URL		1
#define NUM_COLUMNS	2

int CALLBACK LKUP_CompareFunction(CLookupItem* pItem1, CLookupItem* pItem2, LPARAM lCol)
{
	int nCmp = 0;
	switch( lCol ) {
		case COL_NAME:
			nCmp = lstrcmpi(pItem1->m_Name, pItem2->m_Name);
			break;
		case COL_URL:
			nCmp = lstrcmpi(pItem1->m_URL, pItem2->m_URL);
			if (nCmp == 0)
				nCmp = lstrcmpi(pItem1->m_Name, pItem2->m_Name);
			break;
		default:
			return 0;
	}
	if (!bAscendSort)
		nCmp = -nCmp;
	return nCmp;
}

//////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CPrefLookupPage property page

IMPLEMENT_DYNCREATE(CPrefLookupPage, CPropertyPage)

CPrefLookupPage::CPrefLookupPage() : CPropertyPage(CPrefLookupPage::IDD)
{
	//{{AFX_DATA_INIT(CPrefLookupPage)
	m_strName = _T("");
	m_strURL = _T("");
	//}}AFX_DATA_INIT
}


void CPrefLookupPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrefLookupPage)
	DDX_Control(pDX, IDC_LOOKUP_LIST, m_listCtrl);
	DDX_Text(pDX, IDE_NAME, m_strName);
	DDX_Text(pDX, IDE_URL, m_strURL);
	//}}AFX_DATA_MAP
}

CPrefLookupPage::~CPrefLookupPage()
{
}

BEGIN_MESSAGE_MAP(CPrefLookupPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPrefLookupPage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LOOKUP_LIST, OnColumnclickLookupList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LOOKUP_LIST, OnDeleteitemLookupList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LOOKUP_LIST, OnItemchangedLookupList)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_BN_CLICKED(IDB_UP, OnUp)
	ON_BN_CLICKED(IDB_DOWN, OnDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefLookupPage message handlers

BOOL CPrefLookupPage::AllFieldsComplete(void)
{
	if (m_strName.IsEmpty()) {
		MessageBox (_T("Need a Name!"));
		GetDlgItem(IDE_NAME)->SetFocus();
		return false;
	}
	if (m_strURL.IsEmpty()) {
		MessageBox(_T("Need a URL!"));
		GetDlgItem(IDE_URL)->SetFocus();
		return false;
	}
	return true;
}

void CPrefLookupPage::ClearAllFields (void)
{
	m_strName.Empty();
	m_strURL.Empty();
	UpdateData(false);
}

int CPrefLookupPage::ListCtrl_GetCurSel(CWordArray& selArray)
{
	selArray.RemoveAll();
	if (m_listCtrl.m_hWnd) {
		POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
		while (pos)
			selArray.Add(m_listCtrl.GetNextSelectedItem(pos));
	}
	return selArray.GetSize();
}

void CPrefLookupPage::ListCtrl_SetItemText(int row, LPCTSTR lpName, LPCTSTR lpURL)
{
	_TCHAR* strName = _tcsdup(lpName);
	_TCHAR* strURL = _tcsdup(lpURL);
	m_listCtrl.SetItemText(row, COL_NAME, strName);
	m_listCtrl.SetItemText(row, COL_URL, strURL);
	free(strName);
	free(strURL);
}

// return -1 if not found, otherwise index
int CPrefLookupPage::ListCtrl_FindItem(CLookupItem* pItemSearch)
{
	LV_ITEM lvi;
	int max = m_listCtrl.GetItemCount();
	for (int i = 0; i < max; i++) {
		memset (&lvi, 0, sizeof(lvi));
		lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
		lvi.iItem = i;
		m_listCtrl.GetItem (&lvi);
		CLookupItem* pItem = (CLookupItem*)lvi.lParam;
		if (!pItemSearch->m_Name.CompareNoCase (pItem->m_Name))
			return i;
	}
	return -1;
}

int CPrefLookupPage::ListCtrl_AddItem(LPCTSTR lpName, LPCTSTR lpURL)
{
	LV_ITEM lvi;
	memset(&lvi, 0, sizeof(lvi));

	int row = m_listCtrl.GetItemCount();

	CLookupItem* pItem =  new CLookupItem(lpName, lpURL);
	lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
	lvi.lParam = (LONG)pItem;
	lvi.iItem = row;
	lvi.iSubItem = 0;
	lvi.pszText = 0;
	int index = m_listCtrl.InsertItem(&lvi);
	ListCtrl_SetItemText(row, lpName, lpURL);

	return index;
}

void CPrefLookupPage::LoadListCtrl(int row)
{
	if (m_listCtrl.m_hWnd) {
		int i;
		m_listCtrl.DeleteAllItems ();

		CStringArray strNames;
		CStringArray strURLs;
		LKUP_ReadData(strNames, strURLs);
		int nItems = strNames.GetSize ();
		for (i = 0; i < nItems; i++) {
			ListCtrl_AddItem(strNames[i], strURLs[i]);
		}
		strNames.RemoveAll ();
		strURLs.RemoveAll ();
#if 0
		if (nItems > 1) {
			bAscendSort = true;
			m_listCtrl.SortItems((PFNLVCOMPARE)LKUP_CompareFunction, 0);
			iLastSubItem = 0;
			bAscendSort = false;
		}
#endif
		if (nItems > 0)
			ListCtrl_SelectRow(row);
	}
}

void CPrefLookupPage::LKUP_SaveData(void)
{
	CStringArray strNames, strURLs;
	LV_ITEM lvi;
	int max = m_listCtrl.GetItemCount();
	for (int i = 0; i < max; i++) {
		memset (&lvi, 0, sizeof(lvi));
		lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
		lvi.iItem = i;
		m_listCtrl.GetItem (&lvi);
		CLookupItem* pItem = (CLookupItem*)lvi.lParam;
		strNames.Add (pItem->m_Name);
		strURLs.Add (pItem->m_URL);
	}
	LKUP_WriteData (strNames, strURLs);
	strNames.RemoveAll ();
	strURLs.RemoveAll ();
}

#define DELIMITER2 0x2
void CPrefLookupPage::LKUP_WriteData(CStringArray& strNames, CStringArray& strURLs)
{
	int nItems = strNames.GetSize ();
	CString strEntries = _T("");
	for (int i = 0; i < nItems; i++) {
		CLookupItem obItem(strNames[i], strURLs[i]);
		CString strData;
		obItem.FormatData(strData);
		strEntries += strData;
		strEntries += 
			(CString)(_TCHAR)	// 01/12/03 AM.
			DELIMITER2;
	}
	theApp.WriteProfileString(_T("PREFERENCES"),_T("LookupData"),strEntries);
}

void CPrefLookupPage::LKUP_ReadData(CStringArray& strNames, CStringArray& strURLs)
{
	CString strItems = theApp.GetProfileString(_T("PREFERENCES"),_T("LookupData"),_T(""));
	if (strItems.IsEmpty()) {
		strNames.Add(_T("WordNet"));
		strURLs.Add(_T("http://www.cogsci.princeton.edu/cgi-bin/webwn/?stage=1&word="));
		return;
	}
	int posStart = 0;
	int posEnd;
	while (-1 != (posEnd = strItems.Find(DELIMITER2,posStart)) ) {
		CString strData = strItems.Mid(posStart, posEnd - posStart);
		CLookupItem obItem;
		obItem.RetrieveData(strData);
		strNames.Add(obItem.m_Name);
		strURLs.Add(obItem.m_URL);
		posStart = posEnd + 1;
	}
}

void CPrefLookupPage::GetDictionaryLookupList(CStringArray& strNames, CStringArray& strURLs)
{
	CPrefLookupPage dlg;
	strNames.RemoveAll();
	strURLs.RemoveAll();
	dlg.LKUP_ReadData(strNames, strURLs);
}

void CPrefLookupPage::GetDictionaryLookupMenu(CMenu& menu, UINT idBase, CStringArray& strNames, CStringArray& strURLs)
{
	GetDictionaryLookupList(strNames, strURLs);
	menu.CreatePopupMenu();
	int max = strNames.GetSize();
	for (int n = 0; n < max; n++)
		menu.AppendMenu(MF_STRING,idBase + n,strNames[n]);
}

void CPrefLookupPage::FillDictionaryLookupComboBox(CComboBox& combo, CStringArray& strNames, CStringArray& strURLs)
{
	GetDictionaryLookupList(strNames, strURLs);
	combo.ResetContent();
	int max = strNames.GetSize();
	for (int n = 0; n < max; n++)
		combo.AddString(strNames[n]);
}

void CPrefLookupPage::ListCtrl_SelectRow(int index)
{
	int max = m_listCtrl.GetItemCount();
	for (int n = 0; n < max; n++)
		m_listCtrl.SetItemState(n, 0, LVIS_SELECTED);
	m_listCtrl.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
}

void CPrefLookupPage::OnAdd() 
{
	UpdateData();
	if (!AllFieldsComplete())
		return;
	CLookupItem obItem(m_strName, m_strURL);
	int index;
	if ((index = ListCtrl_FindItem(&obItem)) == -1) {
		ListCtrl_AddItem(m_strName, m_strURL);
#if 0
		bAscendSort = true;
		m_listCtrl.SortItems((PFNLVCOMPARE)LKUP_CompareFunction, 0);
#endif
		if ((index = ListCtrl_FindItem(&obItem)) != -1) {
			ListCtrl_SelectRow(index);
		}
		ClearAllFields();
		GetDlgItem(IDE_NAME)->SetFocus();
	}
	else {
		CString strMsg;
		strMsg.Format(_T("The Name \"%s\" is already in the list!"), m_strName);
		MessageBox(strMsg);
		ListCtrl_SelectRow(index);
	}
}

void CPrefLookupPage::OnUpdate() 
{
	UpdateData();
	if (!AllFieldsComplete ())
		return;

	CWordArray selArray;
	if (ListCtrl_GetCurSel(selArray) != 1) return;

	int index = selArray[0];
	CLookupItem* pItem = (CLookupItem*)m_listCtrl.GetItemData(index);
	if (pItem) {
		pItem->m_Name = m_strName;
		pItem->m_URL = m_strURL;
		ListCtrl_SetItemText(index, m_strName, m_strURL);
#if 0
		bAscendSort = true;
		m_listCtrl.SortItems((PFNLVCOMPARE)LKUP_CompareFunction, 0);
#endif
		if ((index = ListCtrl_FindItem(pItem)) != -1)
			ListCtrl_SelectRow(index);
	}
}

void CPrefLookupPage::OnDelete() 
{
	CWordArray selArray;
	int max = ListCtrl_GetCurSel(selArray);
	if (0 == max) return;

	CString msg;
	msg.Format(_T("Delete current selection%s?"),max > 1 ? _T("s") : _T(""));
	if (IDYES != MessageBox(msg, NULL, MB_YESNO))
		return;

	for (int n = max - 1; n >= 0; --n) {
		int index = selArray[n];
		m_listCtrl.DeleteItem(index);
	}
	int nCount = m_listCtrl.GetItemCount();
	if (nCount) {
		int lowIndex = selArray[0];
		if (lowIndex >= nCount)
			lowIndex = nCount - 1;
		ListCtrl_SelectRow(lowIndex);
	}
}

static _TCHAR *_szColumnLabel[NUM_COLUMNS] = {
	_T("Name"),_T("URL")
};

static int _nColumnFmt[NUM_COLUMNS] = {
	LVCFMT_LEFT, LVCFMT_LEFT
};

static int _nColumnWidth[NUM_COLUMNS] = {
	100, 300
};

void CPrefLookupPage::ListCtrlInit(void)
{
	iLastSubItem = -1;
	bAscendSort = true;
	DWORD dwOldStyle = m_listCtrl.GetExtendedStyle ();
	m_listCtrl.SetExtendedStyle(dwOldStyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_SHOWSELALWAYS);
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(int i = 0; i<NUM_COLUMNS; i++) {
		lvc.iSubItem = i;
		lvc.pszText = _szColumnLabel[i];
		lvc.cx = _nColumnWidth[i];
		lvc.fmt = _nColumnFmt[i];
		m_listCtrl.InsertColumn(i,&lvc);
	}
}

BOOL CPrefLookupPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	ListCtrlInit();
	LoadListCtrl(0);

	m_Edit_Name.AutoLoad(IDE_NAME, this, IDC_ADD);
	m_Edit_URL.AutoLoad(IDE_URL, this, IDC_ADD);
	m_BtnUp.AutoLoad(IDB_UP, this);
	m_BtnDown.AutoLoad(IDB_DOWN, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrefLookupPage::OnDestroy() 
{
	LKUP_SaveData();
	CPropertyPage::OnDestroy();
}

void CPrefLookupPage::OnColumnclickLookupList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (iLastSubItem != pNMListView->iSubItem) {
		iLastSubItem = pNMListView->iSubItem;
		bAscendSort = true;
	}
	// TODO: Add your control notification handler code here
	m_listCtrl.SortItems((PFNLVCOMPARE)LKUP_CompareFunction, pNMListView->iSubItem);
	*pResult = 0;
	bAscendSort = !bAscendSort;
}

void CPrefLookupPage::OnDeleteitemLookupList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	CLookupItem* pItem = (CLookupItem*)pNMListView->lParam;
	if (pItem) delete pItem;
	pNMListView->lParam = 0;
	*pResult = 0;
}

void CPrefLookupPage::OnItemchangedLookupList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	CWordArray selArray;
	if (ListCtrl_GetCurSel(selArray) == 1) {
		int index = selArray[0];
		CLookupItem* pItem = (CLookupItem*)m_listCtrl.GetItemData(index);
		if (pItem) {
			m_strName = pItem->m_Name;
			m_strURL = pItem->m_URL;
			UpdateData(false);
		}
	}
	*pResult = 0;
}


void CPrefLookupPage::OnUp() 
{
	CWordArray selArray;
	if (ListCtrl_GetCurSel(selArray) == 1) {
		int index = selArray[0];
		int count = m_listCtrl.GetItemCount();
		if (index > 0 && count > 1) {
			CLookupItem* pItem1 = (CLookupItem*)m_listCtrl.GetItemData(index);
			CLookupItem* pItem2 = (CLookupItem*)m_listCtrl.GetItemData(index - 1);
			m_listCtrl.SetItemData(index, (DWORD)pItem2);
			m_listCtrl.SetItemData(index-1, (DWORD)pItem1);
			ListCtrl_SetItemText(index, pItem2->m_Name, pItem2->m_URL);
			ListCtrl_SetItemText(index-1, pItem1->m_Name, pItem1->m_URL);
			ListCtrl_SelectRow(index-1);
			iLastSubItem = -1;
			bAscendSort = true;
		}
	}
}

void CPrefLookupPage::OnDown() 
{
	CWordArray selArray;
	if (ListCtrl_GetCurSel(selArray) == 1) {
		int index = selArray[0];
		int count = m_listCtrl.GetItemCount();
		if (index < count - 1 && count > 1) {
			CLookupItem* pItem1 = (CLookupItem*)m_listCtrl.GetItemData(index);
			CLookupItem* pItem2 = (CLookupItem*)m_listCtrl.GetItemData(index + 1);
			m_listCtrl.SetItemData(index, (DWORD)pItem2);
			m_listCtrl.SetItemData(index+1, (DWORD)pItem1);
			ListCtrl_SetItemText(index, pItem2->m_Name, pItem2->m_URL);
			ListCtrl_SetItemText(index+1, pItem1->m_Name, pItem1->m_URL);
			ListCtrl_SelectRow(index+1);
			iLastSubItem = -1;
			bAscendSort = true;
		}
	}
}
