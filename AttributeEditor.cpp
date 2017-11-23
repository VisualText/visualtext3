/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// AttributeEditor.cpp : implementation file
//

#include "stdafx.h"
#include "string.h"
#include "kb.h"
#include "Utils.h"

#include "MainFrm.h"
#include "KnowledgeDoc.h"
#include "KnowledgeView.h"
#include "AttributeEditor.h"
#include "EditListCtrl.h"
#include "PrefLookupPage.h"
#include ".\attributeeditor.h"

extern CVisualTextApp theApp;
extern CONCEPT *appAttrConcept;
extern CG *cg;
extern bool kbDirty;

#ifdef UNICODE
extern CFont appFontFixed;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Fixing problem with overly long values.	// 07/12/02 AM.
#define DRAWSTRSIZ 256

///////////////////////////////////////////

CQuickEditItem::CQuickEditItem()
{
	m_Attribute.Empty();
	m_Value.Empty();
}

CQuickEditItem::CQuickEditItem(LPCTSTR attribute, LPCTSTR value)
{
	m_Attribute = attribute;
	m_Value = value;
}

CQuickEditItem::~CQuickEditItem()
{
}

#define DELIMITER1 0x1
int CQuickEditItem::FormatData(CString &cData)
{
	cData.Empty ();
	cData.Format (_T("%s%c%s"), m_Attribute, DELIMITER1, m_Value);
	return cData.GetLength ();
}

void CQuickEditItem::RetrieveData(LPCTSTR lpData)
{
	CString strData = lpData;
	int pos;
	if ((pos = strData.Find(DELIMITER1)) != -1) {
		m_Attribute = strData.Left(pos);
		m_Value = strData.Mid(pos + 1);
	}
}

//////////////////////////////////////////

BOOL CalcStringEllipsis(HDC hdc, LPTSTR lpszString, int cchMax, UINT uColWidth);
void DrawItemColumn(HDC hdc, LPTSTR lpsz, LPRECT prcClip, LPSIZE lpSize);

/////////////////////////////////////////////////////////////////////////////
// CAttributeEditor dialog

CAttributeEditor::CAttributeEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CAttributeEditor::IDD, pParent)
{
	m_bInitListWithData = false;
	m_concept = NULL;
	m_URLcounter = 0;

	//{{AFX_DATA_INIT(CAttributeEditor)
	m_strSearch = _T("");
	m_boolNoAttrs = FALSE;
	m_QE_Attribute = _T("");
	m_QE_Value = _T("");
	m_strValueType = _T("");
	m_boolBrowseConcept = FALSE;
	//}}AFX_DATA_INIT

	m_mainWnd = theApp.m_pMainFrame;
}

void CAttributeEditor::Cleanup(void)
{
	m_strURLs.RemoveAll();
	theApp.WriteProfileString(_T("PREFERENCES"),_T("QuickEditOpenState"), 0 == m_nDlg ? _T("0") : _T("1"));
	QESaveColumnWidths ();
	SaveQEData();
}

void CAttributeEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAttributeEditor)
	DDX_Control(pDX, IDC_ATTRIBUTE_LIST, m_listCtrl);
	DDX_Text(pDX, IDE_SEARCH, m_strSearch);
	DDX_Check(pDX, IDC_NO_ATTRS, m_boolNoAttrs);
	DDX_Text(pDX, IDE_ATTRIBUTE, m_QE_Attribute);
	DDX_Text(pDX, IDE_VALUE, m_QE_Value);
	DDX_CBString(pDX, IDC_VALUE_TYPE, m_strValueType);
	DDX_Check(pDX, IDC_BROWSE_CONCEPT, m_boolBrowseConcept);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAttributeEditor, CDialog)
	//{{AFX_MSG_MAP(CAttributeEditor)
	ON_BN_CLICKED(IDC_ADD, OnAddAttribute)
	ON_BN_CLICKED(IDC_ADDVALUE, OnAddvalue)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ATTRIBUTE_LIST, OnItemchangedAttributeList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_ATTRIBUTE_LIST, OnEndlabeleditAttributeList)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_OPEN_KB, OnOpenKb)
	ON_BN_CLICKED(IDB_QUICKEDITHELP, OnQuickedithelp)
	ON_BN_CLICKED(IDB_ADD, OnAdd)
	ON_BN_CLICKED(IDB_REMOVE, OnRemove)
	ON_BN_CLICKED(IDB_UPDATE, OnUpdate)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LISTQUICKEDIT, OnColumnclickListquickedit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTQUICKEDIT, OnItemchangedListquickedit)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LISTQUICKEDIT, OnDeleteitemListquickedit)
	ON_BN_CLICKED(IDB_KB_EDIT, OnKbEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTQUICKEDIT, OnDblclkListquickedit)
	ON_BN_CLICKED(IDC_QUICKEDIT, OnQuickedit)
	ON_BN_CLICKED(IDC_DELETE_WORD, OnDeleteWord)
	ON_BN_CLICKED(IDC_ADD_WORD, OnAddWord)
	ON_CBN_DROPDOWN(IDC_COMBO_LOOKUP, OnDropdownComboLookup)
	ON_CBN_CLOSEUP(IDC_COMBO_LOOKUP, OnCloseupComboLookup)
	ON_CBN_SELENDOK(IDC_COMBO_LOOKUP, OnSelendokComboLookup)
	ON_BN_CLICKED(IDC_LOOKUP, OnLookup)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_NOTIFY(NM_DBLCLK, IDC_ATTRIBUTE_LIST, OnDblclkAttributeList)
	ON_BN_CLICKED(IDC_BROWSE_CONCEPT, OnBrowseConcept)
	ON_CBN_SELCHANGE(IDC_VALUE_TYPE, OnSelchangeValueType)
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_DELETEITEM, IDC_ATTRIBUTE_LIST, OnDeleteItemListctrl)
	ON_MESSAGE(WMU_DELETELINE, OnDeleteLine)
	ON_MESSAGE(WMU_GETIMAGELIST, OnGetImageList)
	ON_EN_CHANGE(IDE_SEARCH, OnEnChangeSearch)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttributeEditor message handlers



void CAttributeEditor::FillList()
{
	if (!m_concept)
		return;

	CWaitCursor wait;
	ATTR *attrs = cg->findAttrs(m_concept);
	_TCHAR val[1001];
	_TCHAR buff[1001];
	int count = 0;
	CONCEPT *con;
	CString valueStr;

	while (attrs) {
		ATTR* attr = cg->popAttr(attrs);
		cg->attrName(attr,buff,1000);
		CString attrStr = buff;
		if (KBIsSystemAttribute(attrStr))
			continue;
		CAttribute* pAttribute = new CAttribute(attrStr);

		VAL* vals = cg->attrVals(attr);
		if (vals) {
			while (vals) {
				val[0] = '\0';
				con = NULL;
				if (cg->isValConcept(vals)) {
					cg->popVal(vals,con);
					valueStr = ConceptPathStr(con,true);
				}
				else {
					cg->popSval(vals,val);
					valueStr = val;
				}
				pAttribute->AddValue(valueStr,con);
			}
		}
		else {
			pAttribute->AddNullValue();
		}
		int numVals = pAttribute->GetNumValues();
		pAttribute->SetOpen(numVals > 1);
		for (int n = 0; n < numVals; n++) {
			CLineItem* pLineItem = new CLineItem(pAttribute, n);
			LV_ITEM lvi;
			memset (&lvi, 0, sizeof(lvi));
			lvi.mask = LVIF_STATE | LVIF_PARAM;
			lvi.lParam = (LPARAM)pLineItem;
			lvi.iItem = count++;
			int index = m_listCtrl.InsertItem(&lvi);
		}
	}
}

void CAttributeEditor::SetCGConcept(CONCEPT *concept)
{
	m_concept = concept;
	m_strConceptName = ConceptName(m_concept);
	m_listCtrl.SetConcept(concept);
	EnableNavButtons();
	m_URLcounter = 0;
}

void CAttributeEditor::EnableNavButtons(void)
{
	BOOL bEnable = (BOOL)cg->Next(m_concept);
	GetDlgItem(IDC_NEXT)->EnableWindow(bEnable);
	bEnable = (BOOL)cg->Prev(m_concept);
	GetDlgItem(IDC_PREV)->EnableWindow(bEnable);

	bEnable = (BOOL)cg->Down(m_concept);
	GetDlgItem(IDC_DOWN)->EnableWindow(bEnable);
	bEnable = (BOOL)cg->Up(m_concept);
	GetDlgItem(IDC_UP)->EnableWindow(bEnable);
}

#define NUM_COLUMNS 2
#define WIDTH_NAME_COLUMN	100
#define WIDTH_VALUE_COLUMN	300
BOOL CAttributeEditor::OnInitDialog() 
{
	CRect rListCtrl;
	GetWindowRect(m_rDlg[1]);
	GetDlgItem(IDC_ATTRIBUTE_LIST)->GetWindowRect(rListCtrl);
	ScreenToClient(rListCtrl);
	m_rDlg[0] = m_rDlg[1];
	m_rDlg[0].right = m_rDlg[0].left + rListCtrl.Width() + MulDiv(rListCtrl.left,2,1) + MulDiv(GetSystemMetrics(SM_CXDLGFRAME),2,1);
	m_nDlg = theApp.GetProfileString(_T("PREFERENCES"),_T("QuickEditOpenState"),_T("0")) == _T("0") ? 0 : 1;
	switch (m_nDlg) {
		case 0:
			SetWindowPos(NULL,0,0,m_rDlg[m_nDlg].Width(),m_rDlg[m_nDlg].Height(),SWP_NOMOVE);
			break;
		case 1:
			SetDlgItemText(IDC_QUICKEDIT, _T("<< &Quick Edit"));
			break;
	}
	
	m_boolOpeningKB = false;
	m_pLineItem = 0;	// 06/19/02 AM.

	CDialog::OnInitDialog();
		
	QE_ListCtrlInit();
	QE_LoadListCtrl(0);

	m_listCtrl.InsertColumn(0,_T("Name"),LVCFMT_LEFT,WIDTH_NAME_COLUMN);
	m_listCtrl.InsertColumn(1,_T("Value"),LVCFMT_LEFT,WIDTH_VALUE_COLUMN);
	
	InitListViewImageLists();
	InitListWithData();

	m_BtnPrev.AutoLoad(IDC_PREV, this);
	m_BtnNext.AutoLoad(IDC_NEXT, this);
	m_BtnDown.AutoLoad(IDC_DOWN, this);
	m_BtnUp.AutoLoad(IDC_UP, this);
	m_BtnKB.AutoLoad(IDC_OPEN_KB, this);
	m_BtnLookup.AutoLoad(IDC_LOOKUP, this);
	m_BtnDeleteWord.AutoLoad(IDC_DELETE_WORD, this);
	m_EditCR.AutoLoad(IDE_SEARCH, this, IDC_SEARCH);
	m_Edit_QE_Attribute.AutoLoad(IDE_ATTRIBUTE, this, IDB_ADD);
	m_Edit_QE_Value.AutoLoad(IDE_VALUE, this, IDB_ADD);

	// DICTIONARY COMBO BOX
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LOOKUP);
	CStringArray strNames;
	CPrefLookupPage::FillDictionaryLookupComboBox(*pCombo, strNames, m_strURLs);
	/*
	pCombo->SetWindowPos(NULL, 0, 0,
		GetSystemMetrics(SM_CXSMICON) + (2*GetSystemMetrics(SM_CXBORDER)),
		GetSystemMetrics(SM_CYSMICON) + (2*GetSystemMetrics(SM_CYBORDER)),
		SWP_NOMOVE);
	*/

	CDC* pDC = pCombo->GetDC();
	CFont* pFontOld = pDC->SelectObject(GetFont());
	m_ComboWidth = 0;
	int max = strNames.GetSize();
	for (int n = 0; n < max; n++) {
		CSize size = pDC->GetTextExtent(strNames[n]);
		if (m_ComboWidth < size.cx)
			m_ComboWidth = size.cx;
	}
	pDC->SelectObject(pFontOld);
	pCombo->ReleaseDC(pDC);

	m_strValueType = _T("string");	// 06/19/02 AM.
	UpdateData(false);	// 06/19/02 AM.

#ifdef UNICODE
	SetFont(&appFontFixed);
	m_EditCR.SetFont(&appFontFixed);
	m_Edit_QE_Attribute.SetFont(&appFontFixed);
	m_Edit_QE_Value.SetFont(&appFontFixed);
	m_QEListCtrl.SetFont(&appFontFixed);
	m_listCtrl.SetFont(&appFontFixed);
#endif

	return true;
}
 
void CAttributeEditor::InitListWithData(LPCTSTR attribute, LPCTSTR value)
{
	InitListWithData(-1);
	int row = FindAttributeValue(attribute, value);
	if (-1 != row) {
		m_listCtrl.SetItemState(row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_listCtrl.EnsureVisible(row, FALSE);
	}
}

void CAttributeEditor::InitListWithData(int row)
{
	m_bInitListWithData = true;
	if (m_listCtrl.GetItemCount() > 0)
		m_listCtrl.DeleteAllItems();
	FillList();
	AdjustQEList();
	CString titleStr;
	titleStr.Format(_T("Attribute Editor: %s"),m_strConceptName);
	SetWindowText(titleStr);
	appAttrConcept = m_concept;
	int count = m_listCtrl.GetItemCount();
	if (row >= count) row = count - 1;
	if (count > 0 && row >= 0 && row < count) {
		m_listCtrl.SetItemState(row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_listCtrl.EnsureVisible(row, FALSE);
	}

	m_bInitListWithData = false;

	if (count) {
		CLineItem *pLineItem = (CLineItem *)m_listCtrl.GetItemData(0);
		ItemChangedAttributeList(pLineItem);
	}
}

void CAttributeEditor::OnDelete() 
{
	int nItem;
	CLineItem *pLineItemDel = GetSelectedLine(nItem);

	if (pLineItemDel) {
		_TCHAR attrBuff[1001];
		_TCHAR valBuff[1001];
		if (pLineItemDel->GetNumValues() > 1 && !pLineItemDel->IsOpen())
			m_listCtrl.ExpandValues(nItem);
		CString attrStr = pLineItemDel->GetName();
		CString valStr = pLineItemDel->GetValue();
		CString msgStr;
		if (pLineItemDel->GetNumValues() > 1)			// 05/04/01 AM.
			msgStr.Format(_T("Delete the value: \"%s\"?"),valStr);	// 05/04/01 AM.
		else														// 05/04/01 AM.
			msgStr.Format(_T("Delete the attribute value pair: \"%s\" - \"%s\"?"),attrStr,valStr);

		if (AfxMessageBox(msgStr,MB_YESNO) == IDYES) {
			StringToChar(attrBuff,attrStr,1000);
			StringToChar(valBuff,valStr,1000);
			bool bSuccess = false;
			ValueType type = pLineItemDel->GetValueType();
			if (type == valNumber) {
				int valInt = _ttoi((const _TCHAR *)valStr);
				bSuccess = cg->rmAttrval(m_concept,attrBuff,(long)valInt);
			}
			else if (type == valConcept) {
				CONCEPT *con = pLineItemDel->GetConcept();
				bSuccess = cg->rmAttrval(m_concept,attrBuff,con);
			}
			else {
				bSuccess = cg->rmAttrval(m_concept,attrBuff,valBuff);
			}
			if (bSuccess) {
				kbDirty = true;
				// Need to decrement m_Index for certain lines displaying this Attribute
				if (pLineItemDel->GetIndex() < pLineItemDel->GetNumValues() - 1) {
					// Back up to the start of this particular Attribute Listing
					CAttribute* pAttr = pLineItemDel->GetAttribute();
					int startIdx = 0;
					for (startIdx = nItem; startIdx > 0; --startIdx) {
						CLineItem* pLI = (CLineItem*)m_listCtrl.GetItemData(startIdx - 1);
						if (!pLI || pLI->GetAttribute() != pAttr)
							break;
					}
					// Run through this attributes value lines and decrement all lines
					// whose indices are greater than the deleted line's index.
					int nValIndex = pLineItemDel->GetIndex();
					int max = m_listCtrl.GetItemCount();
					int n = 0;
					for (n = startIdx; n < max; n++) {
						CLineItem* pLI = (CLineItem*)m_listCtrl.GetItemData(n);
						if (!pLI || pLI->GetAttribute() != pAttr)
							break;
						if (pLI->GetIndex() > nValIndex)
							pLI->DecrementIndex();
					}
				}
				m_listCtrl.DeleteItem(nItem);
				int nCount = m_listCtrl.GetItemCount();
				if (nCount) {
					if (nItem >= nCount)
						nItem = nCount - 1;
					m_listCtrl.SetItemState (nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				}
			}
		}
	}
}

CLineItem *CAttributeEditor::GetSelectedLine(int &nItem)
{
	CLineItem* line = NULL;
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		nItem = m_listCtrl.GetNextSelectedItem(pos);
		line = (CLineItem*)m_listCtrl.GetItemData(nItem);
	}
	return line;
}

void CAttributeEditor::OnAddvalue() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (pos) {
		m_listCtrl.SetFocus();
		int nLineFirst = m_listCtrl.GetNextSelectedItem(pos);
		CLineItem* pLIFirst = (CLineItem*)m_listCtrl.GetItemData(nLineFirst);
		CAttribute* pAttribute = pLIFirst->GetAttribute();
		if (!pLIFirst->IsOpen())
			m_listCtrl.ExpandValues(nLineFirst);
		int nLineInsert = nLineFirst + 1;
		if (pLIFirst->IsOpen()) {
			int max = m_listCtrl.GetItemCount();
			for (; nLineInsert < max; nLineInsert++) {
				CLineItem* pLI = (CLineItem*)m_listCtrl.GetItemData(nLineInsert);
				if (!pLI || pLI->GetAttribute() != pAttribute)
					break;
			}
		}
		CLineItem* pLineItem = new CLineItem(pAttribute, pLIFirst->GetNumValues());

		UpdateData(true);
		if (m_strValueType == _T("concept")) {
			CONCEPT *root = cg->findRoot();
			pLineItem->AddValue(ConceptName(root),root);
		}
		else if (m_strValueType == _T("numeric"))	// 06/19/02 AM.
			pLineItem->AddValue(_T("0"));				// 06/19/02 AM.
		else
			pLineItem->AddValue(_T("New Value"));

		LV_ITEM lvi;
		memset (&lvi, 0, sizeof(lvi));
		lvi.mask = LVIF_STATE | LVIF_PARAM;
		lvi.lParam = (LPARAM)pLineItem;
		lvi.iItem = nLineInsert;
		lvi.state = LVIS_SELECTED | LVIS_FOCUSED;
		nLineInsert = m_listCtrl.InsertItem(&lvi);
		m_listCtrl.EditSubLabel(nLineInsert, 1);
	}
}

void CAttributeEditor::OnAddAttribute() 
{
	UpdateData(true);
	m_listCtrl.SetFocus();
	int count = m_listCtrl.GetItemCount();
	m_listCtrl.InsertItem(count,_T("Name"));
	m_listCtrl.SetAttributeType(m_strValueType);
	m_listCtrl.EditLabel(count);
}

void CAttributeEditor::OnItemchangedAttributeList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Want to get new value type if any.	// 06/19/02 AM.
	UpdateData(true);	// 06/19/02 AM.
	m_listCtrl.SetAttributeType(m_strValueType);	// 06/19/02 AM.

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	CLineItem* pLineItem = (CLineItem*)pNMListView->lParam;

	ItemChangedAttributeList(pLineItem);

	*pResult = 0;
}

void CAttributeEditor::ItemChangedAttributeList(CLineItem* pLineItem) 
{
	if (pLineItem && !m_bInitListWithData) {

		ValueType typ = pLineItem->GetValueType();	// 06/19/02 AM.

		// DO CONCEPT BROWSING IF ON
		bool isConcept = typ == valConcept;
		GetDlgItem(IDC_BROWSE_CONCEPT)->EnableWindow(isConcept);
		if (isConcept)
			OnBrowseConcept();

		// UPDATE DICTIONARY SEARCH TEXT
		if (typ == valAlpha)	// 06/19/02 AM.
			m_strSearch = pLineItem->GetValue();
		else
			m_strSearch = _T("");	// 06/19/02 AM.

		// UPDATE ATTRIBUTE COMBO TYPE BOX

		CComboBox *typeCombo = (CComboBox *)GetDlgItem(IDC_VALUE_TYPE);

		// Get type from line if moved to new line.	// 06/19/02 AM.
		if (m_pLineItem && m_pLineItem != pLineItem)	// 06/19/02 AM.
			{
			m_strValueType = pLineItem->GetValueTypeString();	// 06/19/02 AM.
			m_listCtrl.SetAttributeType(m_strValueType);	// 06/19/02 AM.
			}
		m_pLineItem = pLineItem;	// 06/19/02 AM.

		UpdateData(false);
	}
}

void CAttributeEditor::OnEndlabeleditAttributeList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	*pResult = 0;
}

LRESULT CAttributeEditor::OnGetImageList(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)&m_AE_ImageList;
}

LRESULT CAttributeEditor::OnDeleteLine(WPARAM wParam, LPARAM lParam)
{
	int nItem = (int)wParam;
	m_listCtrl.DeleteItem(nItem);
	return 0L;
}

void CAttributeEditor::OnDeleteItemListctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	CLineItem* pLineItem = (CLineItem*)pNMListView->lParam;
	if (pLineItem) {
		// If the index is 0, and there is only one value, delete the attribute
		if (pLineItem->GetIndex() == 0 && pLineItem->GetNumValues() == 1) {
			if (pLineItem->GetAttribute())
//				delete (pLineItem->GetAttribute());	// FIX.	// 06/19/02 AM.
				pLineItem->DeleteAttribute();	// FIX.	// 06/19/02 AM.
		}
		delete pLineItem;
	}
	pNMListView->lParam = 0;
	
	*pResult = 0;
}

void CAttributeEditor::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	switch (lpDrawItemStruct->CtlType) {
		case ODT_LISTVIEW:
			DrawListViewItem(lpDrawItemStruct);
			break;
		default:
			CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
			break;
	}
}

void CAttributeEditor::DrawListViewItem(LPDRAWITEMSTRUCT lpDrawItem)
{
	LV_ITEM lvi;
	int cxImage = 0, cyImage = 0;
	RECT rcClip;
	int iColumn = 1;
	UINT uiFlags = ILD_NORMAL;

	int colWidth[NUM_COLUMNS];
	memset (colWidth, 0, sizeof(colWidth));
	for (int i = 0; i < NUM_COLUMNS; i++)
		colWidth[i] = m_listCtrl.GetColumnWidth (i);

	// Get the item image to be displayed
	lvi.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.iItem = lpDrawItem->itemID;
	lvi.iSubItem = 0;
	ListView_GetItem(lpDrawItem->hwndItem, &lvi);
	CLineItem* pLineItem = (CLineItem*)lvi.lParam;

	COLORREF clrBk;
	// Check to see if this item is selected
	if (lpDrawItem->itemState & ODS_SELECTED) {
		SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBk = GetSysColor(COLOR_HIGHLIGHT);
	}
	else {
		SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOWTEXT));
		clrBk = GetSysColor(COLOR_WINDOW);
	}
	// Erase any former focus rectangle
	SetBkColor(lpDrawItem->hDC, clrBk);
	m_AE_ImageList.SetBkColor(clrBk);
	HPEN hPenBk = CreatePen(PS_SOLID, 1, clrBk);
	HBRUSH hBrushBk = CreateSolidBrush(clrBk);
	HPEN hPenOld = (HPEN)SelectObject (lpDrawItem->hDC, hPenBk);
	HBRUSH hBrushOld = (HBRUSH)SelectObject(lpDrawItem->hDC, hBrushBk);
	Rectangle(lpDrawItem->hDC, lpDrawItem->rcItem.left, lpDrawItem->rcItem.top, lpDrawItem->rcItem.right, lpDrawItem->rcItem.bottom);
	SelectObject(lpDrawItem->hDC, hPenOld);
	SelectObject(lpDrawItem->hDC, hBrushOld);
	DeleteObject(hPenBk);
	DeleteObject(hBrushBk);

#define IMAGE_PLUSSIGN	0
#define IMAGE_NEGSIGN	1
#define IMAGE_ALPHA		2
#define IMAGE_NUMBER	3
#define IMAGE_CONCEPT	4
#define IMAGE_BLANK		5
#define IMAGE_NULL		6
#define IMAGE_OPEN		7 // No image-icons exist for IMAGE_OPEN or IMAGE_OPEN_LAST
#define IMAGE_OPEN_LAST	8 // Use BLANK image and draw graphics by hand below
	ImageList_GetIconSize((HIMAGELIST)m_AE_ImageList, &cxImage, &cyImage);
	int nImageIndex = IMAGE_BLANK;
	if (pLineItem  && 0 == pLineItem->GetIndex() && pLineItem->GetNumValues() > 1)
		nImageIndex = pLineItem->IsOpen() ? IMAGE_NEGSIGN : IMAGE_PLUSSIGN;
	else if (pLineItem  && pLineItem->GetIndex() > 0) {
		if (pLineItem->GetNumValues() - 1 == pLineItem->GetIndex())
			nImageIndex = IMAGE_OPEN_LAST;
		else
			nImageIndex = IMAGE_OPEN;
	}
	// Set up the new clipping rect for the Name column text and draw it
	rcClip.left = lpDrawItem->rcItem.left;
	rcClip.right = lpDrawItem->rcItem.left + colWidth[0];
	rcClip.top = lpDrawItem->rcItem.top;
	rcClip.bottom = lpDrawItem->rcItem.bottom;
	m_AE_ImageList.Draw(CDC::FromHandle(lpDrawItem->hDC),
		nImageIndex == IMAGE_OPEN || nImageIndex == IMAGE_OPEN_LAST ? IMAGE_BLANK : nImageIndex,
		CPoint(rcClip.left, rcClip.top), uiFlags);

#define CLR_MEDIUMGRAY	RGB(160,160,164)
	switch (nImageIndex) {
		case IMAGE_NEGSIGN:
		case IMAGE_OPEN:
		case IMAGE_OPEN_LAST:
			{
				int cyBottom = rcClip.top + (nImageIndex == IMAGE_OPEN_LAST ? cyImage/2 : cyImage + 1);
				int cyMiddle = rcClip.top + cyImage/2;
				int cxLeft = rcClip.left + MulDiv(cxImage,7,16);
				HPEN hPenMatrix = CreatePen(PS_SOLID, 1, CLR_MEDIUMGRAY);
				hPenOld = (HPEN)SelectObject(lpDrawItem->hDC, hPenMatrix);
				switch (nImageIndex) {
					case IMAGE_OPEN:
					case IMAGE_OPEN_LAST:
						MoveToEx(lpDrawItem->hDC, cxLeft, rcClip.top, NULL);
						LineTo(lpDrawItem->hDC, cxLeft, cyBottom);
						MoveToEx(lpDrawItem->hDC, cxLeft, cyMiddle, NULL);
						LineTo(lpDrawItem->hDC, rcClip.left + colWidth[0], cyMiddle);
						break;
					case IMAGE_NEGSIGN:
						MoveToEx(lpDrawItem->hDC, cxLeft, rcClip.top + MulDiv(cyImage,13,16), NULL);
						LineTo(lpDrawItem->hDC, cxLeft, cyBottom);
						break;
					default:
						break;
				}
				SelectObject(lpDrawItem->hDC, hPenOld);
				DeleteObject(hPenMatrix);
			}
			break;
	}
	
	rcClip.left += cxImage;

	CString strColumn1 = _T(" ");
	if (pLineItem && 0 == pLineItem->GetIndex()) {
		strColumn1 += pLineItem->GetName();
		CSize size;
		DrawItemColumn(lpDrawItem->hDC, (LPTSTR)(LPCTSTR)strColumn1, &rcClip, IMAGE_NEGSIGN == nImageIndex ? &size : NULL);
		if (IMAGE_NEGSIGN == nImageIndex) {
			int cyMiddle = rcClip.top + cyImage/2;
			int cxLeft = rcClip.left + size.cx;
			HPEN hPenMatrix = CreatePen(PS_SOLID, 1, CLR_MEDIUMGRAY);
			hPenOld = (HPEN)SelectObject(lpDrawItem->hDC, hPenMatrix);
			MoveToEx(lpDrawItem->hDC, cxLeft, cyMiddle, NULL);
			LineTo(lpDrawItem->hDC, rcClip.left + colWidth[0], cyMiddle);
			SelectObject(lpDrawItem->hDC, hPenOld);
			DeleteObject(hPenMatrix);
		}
	}

	// Update the clip rect to the Value column
	rcClip.left = rcClip.right;
	rcClip.right = rcClip.left + colWidth[1];
	nImageIndex = 3;
	if (pLineItem)
		nImageIndex = ImageFromType(pLineItem->GetValueType());
	else
		nImageIndex = ImageFromType(m_listCtrl.m_valueTypeNew);

	m_AE_ImageList.Draw(CDC::FromHandle(lpDrawItem->hDC), nImageIndex, CPoint(rcClip.left, rcClip.top), uiFlags);
	rcClip.left += cxImage;
	CString strColumn2 = _T(" ");
	if (pLineItem) {
		strColumn2 += pLineItem->GetValue();
		DrawItemColumn(lpDrawItem->hDC, (LPTSTR)(LPCTSTR)strColumn2,&rcClip, NULL);
	}

	// If we changed the colors for the selected item, undo it
	if (lpDrawItem->itemState & ODS_SELECTED) {
		// Set the text background and foreground colors
		SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOWTEXT));
		SetBkColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOW));
	}

	// If the item is focused, now draw a focus rect around the entire row
	if (lpDrawItem->itemState & ODS_FOCUS) {
		// Adjust the left edge to exclude the image
		rcClip = lpDrawItem->rcItem;
//		rcClip.left += cxImage;

		// Draw the focus rect
		DrawFocusRect(lpDrawItem->hDC, &rcClip);
	}

	return;
}

int CAttributeEditor::ImageFromType(ValueType type)
{
	int index = IMAGE_NUMBER;

	switch (type) {
		case valNULL:
			index = IMAGE_NULL;
			break;
		case valAlpha:
			index = IMAGE_ALPHA;
			break;
		case valNumber:
			index = IMAGE_NUMBER;
			break;
		case valConcept: 
		default:
			index = IMAGE_CONCEPT;
			break;
	}

	return index;
}

//
//  FUNCTION:   InitListViewImageLists
//
//  PURPOSE:    Creates the image lists and add them to the listview.
//
//  PARAMETERS:
//      hwndLV  - handle of the listview to add the images to
//
//  RETURN VALUE:
//      (BOOL) Returns TRUE if the image list is created and added to the
//             control successfully, FALSE otherwise.
//
#define CX_SMICON   16              // Size of the small images
#define CY_SMICON   16
BOOL CAttributeEditor::InitListViewImageLists(void)
{
	m_AE_ImageList.Create(CX_SMICON, CY_SMICON, ILC_MASK, 1 , 10);
	CBitmap bmRes;
	bmRes.LoadBitmap (MAKEINTRESOURCE(IDB_ATTR_EDITOR_VALUE_ICONS));
	m_AE_ImageList.Add(&bmRes, RGB(0xff,0,0));
	bmRes.DeleteObject();
	m_listCtrl.SetImageList(&m_AE_ImageList, LVSIL_SMALL);
	return TRUE;
}

//
//  FUNCTION:   DrawItemColumn(HDC, LPTSTR, LPRECT)
//
//  PURPOSE:    Draws the text for one of the columns in the list view.
//
//  PARAMETERS:
//      hdc     - Handle of the DC to draw the text into.
//      lpsz    - String to draw.
//      prcClip - Rectangle to clip the string to.
//
void DrawItemColumn(HDC hdc, LPTSTR lpsz, LPRECT prcClip, LPSIZE lpSize)
{
	TCHAR szString[DRAWSTRSIZ];

	// Check to see if the string fits in the clip rect.  If not, truncate
	// the string and add "...".
	lstrcpyn(szString,lpsz,DRAWSTRSIZ);	// Fixes crash on long str.	// 07/12/02 AM.
//	lstrcpy(szString, lpsz);	// 07/12/02 AM.
	CalcStringEllipsis(hdc, szString, DRAWSTRSIZ, prcClip->right - prcClip->left);

	// print the text
	ExtTextOut(hdc, prcClip->left + 2, prcClip->top + 1, ETO_CLIPPED | ETO_OPAQUE,
		prcClip, szString, lstrlen(szString), NULL);
	if (lpSize) {
		GetTextExtentPoint(hdc, szString, lstrlen(szString), lpSize);
		lpSize->cx += 4;
	}
}

BOOL CalcStringEllipsis(HDC hdc, LPTSTR lpszString, int cchMax, UINT uColWidth)
{
	const TCHAR szEllipsis[] = TEXT("...");
	SIZE   sizeString;
	SIZE   sizeEllipsis;
	int    cbString;
	LPTSTR lpszTemp;
	BOOL   fSuccess = FALSE;
	static BOOL fOnce = TRUE;

	// Adjust the column width to take into account the edges
	uColWidth -= 4;
	__try
	{
		// Allocate a string for us to work with.  This way we can mangle the
		// string and still preserve the return value
		lpszTemp = (LPTSTR) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cchMax);
		if (!lpszTemp)
            __leave;

		lstrcpyn(lpszTemp,lpszString,cchMax);	// Safer copy.	// 07/12/02 AM.
//		lstrcpy(lpszTemp, lpszString);	// 07/12/02 AM.

		// Get the width of the string in pixels
		cbString = lstrlen(lpszTemp);
		if (!GetTextExtentPoint32(hdc, lpszTemp, cbString, &sizeString))
            __leave;

		// If the width of the string is greater than the column width shave
		// the string and add the ellipsis
		if ((ULONG)sizeString.cx > uColWidth) {
			if (!GetTextExtentPoint32(hdc, szEllipsis, lstrlen(szEllipsis), &sizeEllipsis))
				__leave;

			while (cbString > 0) {
				lpszTemp[--cbString] = 0;
				if (!GetTextExtentPoint32(hdc, lpszTemp, cbString, &sizeString))
                    __leave;

				if ((ULONG)(sizeString.cx + sizeEllipsis.cx) <= uColWidth)
				{
					// The string with the ellipsis finally fits, now make sure
					// there is enough room in the string for the ellipsis
					if (cchMax >= (cbString + lstrlen(szEllipsis)))
					{
						// Concatenate the two strings and break out of the loop
						lstrcat(lpszTemp, szEllipsis);
						lstrcpy(lpszString, lpszTemp);
						fSuccess = TRUE;
						__leave;
					}
				}
			}
		}
		else {
			// No need to do anything, everything fits great.
			fSuccess = TRUE;
		}
	}
	__finally
	{
		// Free the memory
		HeapFree(GetProcessHeap(), 0, (LPVOID)lpszTemp);
//		return (fSuccess);	// .NET COMPLAINS	// 03/19/03 AM.
	}
return fSuccess;	// 03/19/03 AM.
}

void CAttributeEditor::OnNext() 
{
	CONCEPT* concept = cg->Next(m_concept);

	UpdateData(true);
	if (m_boolNoAttrs) {
		while (concept && cg->findAttrs(concept)) {
			concept = cg->Next(concept);
		}
	}

	if (concept) {
		SetCGConcept(concept);
		InitListWithData();
		CView* pView = theApp.GetKBEditor();
		if (pView)
			pView->PostMessage(WMU_NEXT);
	}
}

void CAttributeEditor::OnPrev() 
{
	CONCEPT* concept = cg->Prev(m_concept);

	UpdateData(true);
	if (m_boolNoAttrs) {
		while (concept && cg->findAttrs(concept)) {
			concept = cg->Prev(concept);
		}
	}

	if (concept) {
		SetCGConcept(concept);
		InitListWithData();
		CView* pView = theApp.GetKBEditor();
		if (pView)
			pView->PostMessage(WMU_PREV);
	}
}

void CAttributeEditor::OnDown() 
{
	CONCEPT* concept = cg->Down(m_concept);

	UpdateData(true);

	if (concept) {
		SetCGConcept(concept);
		InitListWithData();
		CView* pView = theApp.GetKBEditor();
		if (pView)
			pView->PostMessage(WMU_DOWN);
	}	
}

void CAttributeEditor::OnUp() 
{
	CONCEPT* concept = cg->Up(m_concept);

	UpdateData(true);

	if (concept) {
		SetCGConcept(concept);
		InitListWithData();
		CView* pView = theApp.GetKBEditor();
		if (pView)
			pView->PostMessage(WMU_UP);
	}
}

void CAttributeEditor::OnDeleteWord() 
{
	UpdateData(true);
	m_strSearch.TrimLeft();
	m_strSearch.TrimRight();
	if (m_strSearch.IsEmpty()) {
		AfxMessageBox(_T("Enter a word to delete."));
		CEdit* pEdit = (CEdit*)GetDlgItem(IDE_SEARCH);
		pEdit->SetFocus();
		pEdit->SetSel(-1,-1);
		return;
	}
	CString str = m_strSearch;
	int pos = str.FindOneOf(_T("\r\n\t "));
	if (pos != -1) {
		CString strAdd;
		strAdd.Format(_T("Normally, only single words are deleted.")
			_T("\nSelect YES to delete just the first word (%s)")
			_T("\nSelect NO to Cancel"), str.Left(pos));
		if (IDNO == AfxMessageBox(strAdd, MB_YESNO))
			return;
		str = str.Left(pos);
	}
	CMainFrame *wnd = theApp.m_pMainFrame;
	CONCEPT* concept = cg->getWordConcept((LPTSTR)(LPCTSTR)str);
#if 0
	if (NULL == concept) {
		CString strLower = str;
		strLower.MakeLower();
		if (concept = cg->getWordConcept((LPTSTR)(LPCTSTR)strLower))
			str = strLower;
	}
#endif
	if (concept) {
		if (cg->rmWord((LPTSTR)(LPCTSTR)str)) {
			if (str.CompareNoCase(m_strConceptName)) {
				SetCGConcept(NULL);
				InitListWithData();
			}
			m_mainWnd->AddDebugLine(_T("Deleted word from dictionary:"),str,true);
			CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
			if (pView)
				pView->Restore();
		}
		else {
			m_mainWnd->AddDebugLine(_T("Unable to delete word from dictionary:"),str,true);
		}
	}
	else {
		m_mainWnd->AddDebugLine(_T("Delete Word: Not in dictionary:"),str,true);
	}
}

void CAttributeEditor::OnAddWord() 
{
	UpdateData(true);
	m_strSearch.TrimLeft();
	m_strSearch.TrimRight();
	if (m_strSearch.IsEmpty()) {
		AfxMessageBox(_T("Enter a word to add."));
		CEdit* pEdit = (CEdit*)GetDlgItem(IDE_SEARCH);
		pEdit->SetFocus();
		pEdit->SetSel(-1,-1);
		return;
	}
	CString str = m_strSearch;
	int pos = str.FindOneOf(_T("\r\n\t "));
	if (pos != -1) {
		CString strAdd;
		strAdd.Format(_T("Normally, only single words are added.")
			_T("\nSelect YES to add just the first word (%s)")
			_T("\nSelect NO to Cancel"), str.Left(pos));
		if (IDNO == AfxMessageBox(strAdd, MB_YESNO))
			return;
		str = str.Left(pos);
	}
	CONCEPT* concept = cg->getWordConcept((LPTSTR)(LPCTSTR)str);
#if 0
	if (NULL == concept) {
		CString strLower = str;
		strLower.MakeLower();
		if (concept = cg->getWordConcept((LPTSTR)(LPCTSTR)strLower))
			str = strLower;
	}
#endif
	if (concept) {
		m_mainWnd->AddDebugLine(_T("Word already exists in dictionary:"),str,true);
		SetCGConcept(concept);
		InitListWithData();
		CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
		if (pView)
			pView->SelectFromConcept(concept);
		return;
	}

	if (concept = cg->addWord((LPTSTR)(LPCTSTR)str)) {
		m_mainWnd->AddDebugLine(_T("Added word to dictionary:"),str,true);
		SetCGConcept(concept);
		InitListWithData();
		CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
		if (pView) {
			pView->Restore();
			pView->SelectFromConcept(concept);
		}
	}
	else {
		m_mainWnd->AddDebugLine(_T("Unable to add word to dictionary:"),str,true);
	}
	
}

void CAttributeEditor::OnSearch() 
{
	UpdateData(true);
	m_strSearch.TrimLeft();
	m_strSearch.TrimRight();
	if (m_strSearch.IsEmpty()) {
		AfxMessageBox(_T("Enter a word to search for."));
		CEdit* pEdit = (CEdit*)GetDlgItem(IDE_SEARCH);
		pEdit->SetFocus();
		pEdit->SetSel(-1,-1);
		return;
	}
	CString str = m_strSearch;
	CONCEPT *wordConcept = cg->getWordConcept((LPTSTR)(LPCTSTR)str);
#if 0
	if (NULL == wordConcept) {
		CString strLower = str;
		strLower.MakeLower();
		if (wordConcept = cg->getWordConcept((LPTSTR)(LPCTSTR)strLower))
			str = strLower;
	}
#endif
	if (wordConcept) {
		SetCGConcept(wordConcept);
		InitListWithData();
		CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
		if (pView)
			pView->SelectFromConcept(wordConcept);
	}
	else {
		m_mainWnd->AddDebugLine(_T("Search: Not in dictionary:"),str,true);
	}

}

void CAttributeEditor::OnCancel()
{
	m_mainWnd->PostMessage(WMU_DESTROY_ATTRIBUTEEDITOR);
}

void CAttributeEditor::OnOK() 
{
	m_mainWnd->PostMessage(WMU_DESTROY_ATTRIBUTEEDITOR);
//	CDialog::OnOK();
}

void CAttributeEditor::OnClose() 
{
	CDialog::OnClose();
	m_mainWnd->PostMessage(WMU_DESTROY_ATTRIBUTEEDITOR);
}

void CAttributeEditor::Open(CONCEPT* concept, BOOL bCalledFromKB)
{
	if (!m_mainWnd->m_pAttributeEditor)
		m_mainWnd->SendMessage(WM_COMMAND, ID_ATTRIBUTEEDITOR);
	CAttributeEditor* editor = (CAttributeEditor*)m_mainWnd->m_pAttributeEditor;
	editor->SetCGConcept(concept);
	editor->InitListWithData();
	if (!bCalledFromKB) {
		CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
		if (pView)
			pView->SelectFromConcept(concept);
	}
}

void CAttributeEditor::LoadConcept(CONCEPT *concept) 
{
	if (concept && !m_boolOpeningKB) {
		UpdateData(true);
		bool changeConcept = true;

		if (m_boolBrowseConcept) {
			int nItem;
			changeConcept = false;
			CLineItem *pLineItem = GetSelectedLine(nItem);
			if (pLineItem->GetValueType() == valConcept) {
				m_listCtrl.EndLabelEdit(nItem,1,ConceptPathStr(concept,true));
				RedrawWindow();
			}
			else
				changeConcept = true;
		}
		if (changeConcept) {
			SetCGConcept(concept);
			InitListWithData();
		}
	}
}

void CAttributeEditor::OnOpenKb() 
{
	OpenKb(m_concept);
}

void CAttributeEditor::OpenKb(CONCEPT *concept) 
{
	m_boolOpeningKB = true;
	m_mainWnd->SendMessage(WM_COMMAND,ID_KNOWLEDGE_BASE);
	m_boolOpeningKB = false;

	CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
	if (pView && concept)
		pView->SelectFromConcept(concept,false);
}

void CAttributeEditor::ReIcon() 
{
	if (m_concept) {
		m_mainWnd->ReIconKB(m_concept);
	}
}

///////////////////////////////////////////////////////

static int iLastSubItem;
static BOOL bAscendSort;

#define QE_COL_ACTION		0
#define QE_COL_ATTRIBUTE	1
#define QE_COL_VALUE		2
#define QE_NUM_COLUMNS		3

int CALLBACK AE_CompareFunction(CQuickEditItem* pItem1, CQuickEditItem* pItem2, LPARAM lCol)
{
	int nCmp = 0;
	switch( lCol ) {
		case QE_COL_ATTRIBUTE:
			nCmp = lstrcmpi(pItem1->m_Attribute, pItem2->m_Attribute);
			if (nCmp == 0)
				nCmp = lstrcmpi(pItem1->m_Value, pItem2->m_Value);
			break;
		case QE_COL_VALUE:
			nCmp = lstrcmpi(pItem1->m_Value, pItem2->m_Value);
			if (nCmp == 0)
				nCmp = lstrcmpi(pItem1->m_Attribute, pItem2->m_Attribute);
			break;
		default:
			return 0;
	}
	if (!bAscendSort)
		nCmp = -nCmp;
	return nCmp;
}

//////////////////////////////////////////////////////

static _TCHAR *_szQEColumnLabel[QE_NUM_COLUMNS] = {
	_T("Action"),_T("Attribute"), _T("Value")
};

static int _nQEColumnFmt[QE_NUM_COLUMNS] = {
	LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_LEFT
};

static int _nQEColumnWidth[QE_NUM_COLUMNS] = {
	42, 75, 75
};

void CAttributeEditor::QESaveColumnWidths(void)
{
	int i = 0;
	if (NULL == m_QEListCtrl.m_hWnd)
		return;
	int width[QE_NUM_COLUMNS];
	memset(width, 0, sizeof(width));
	for (i = 0; i < QE_NUM_COLUMNS; i++)
		width[i] = m_QEListCtrl.GetColumnWidth(i);
	_TCHAR szTemp[512];
	int n = 0;
	for (i = 0; i < QE_NUM_COLUMNS; i++)
		n += wsprintf(szTemp + n, _T("%d%c"), width[i], DELIMITER1);
	theApp.WriteProfileString(_T("PREFERENCES"),_T("QuickEditColumnWidths"),szTemp);
}

void CAttributeEditor::QERetrieveColumnWidths(void)
{
	_TCHAR szTemp[512];
	CString strColumnWidths = theApp.GetProfileString(_T("PREFERENCES"),_T("QuickEditColumnWidths"),_T(""));
	_tcscpy_s(szTemp, strColumnWidths);
	if (szTemp[0]) {
		LPTSTR lp1, lp2;
		int n = 0;
		for (lp1 = szTemp; n < QE_NUM_COLUMNS && *lp1 && (lp2 = _tcschr(lp1, DELIMITER1)); lp1 = lp2 + 1)
			_nQEColumnWidth[n++] = _ttoi(lp1);
	}
}

void CAttributeEditor::QE_ListCtrlInit(void)
{
	iLastSubItem = -1;
	bAscendSort = true;
	// Insert Column Headings
	QERetrieveColumnWidths();
	m_QEListCtrl.AutoLoad(IDC_LISTQUICKEDIT, this, IDB_KB_EDIT, QE_COL_ATTRIBUTE);
	DWORD dwOldStyle = m_QEListCtrl.GetExtendedStyle ();
	m_QEListCtrl.SetExtendedStyle(dwOldStyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_SHOWSELALWAYS);
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(int i = 0; i<QE_NUM_COLUMNS; i++) {
		lvc.iSubItem = i;
		lvc.pszText = _szQEColumnLabel[i];
		lvc.cx = _nQEColumnWidth[i];
		lvc.fmt = _nQEColumnFmt[i];
		m_QEListCtrl.InsertColumn(i,&lvc);
	}
}

int CAttributeEditor::QE_ListCtrl_GetCurSel(CWordArray& selArray)
{
	selArray.RemoveAll();
	if (m_QEListCtrl.m_hWnd) {
		POSITION pos = m_QEListCtrl.GetFirstSelectedItemPosition();
		while (pos)
			selArray.Add(m_QEListCtrl.GetNextSelectedItem(pos));
	}
	return selArray.GetSize();
}

void CAttributeEditor::QE_ListCtrl_SetItemText(int row, LPCTSTR lpAttribute, LPCTSTR lpValue)
{
	_TCHAR* strAttr = _tcsdup(lpAttribute);
	_TCHAR* strVal = _tcsdup(lpValue);
	m_QEListCtrl.SetItemText(row, QE_COL_ATTRIBUTE, strAttr);
	m_QEListCtrl.SetItemText(row, QE_COL_VALUE, strVal);
	free(strAttr);
	free(strVal);
}

// return -1 if not found, otherwise index
int CAttributeEditor::QE_ListCtrl_FindItem(CQuickEditItem* pItemSearch)
{
	LV_ITEM lvi;
	int max = m_QEListCtrl.GetItemCount();
	for (int i = 0; i < max; i++) {
		memset (&lvi, 0, sizeof(lvi));
		lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
		lvi.iItem = i;
		m_QEListCtrl.GetItem (&lvi);
		CQuickEditItem* pItem = (CQuickEditItem*)lvi.lParam;
		if (!pItemSearch->m_Attribute.CompareNoCase (pItem->m_Attribute) &&
			!pItemSearch->m_Value.CompareNoCase (pItem->m_Value))
			return i;
	}
	return -1;
}

int CAttributeEditor::QE_ListCtrl_AddItem(LPCTSTR lpAttribute, LPCTSTR lpValue)
{
	LV_ITEM lvi;
	memset(&lvi, 0, sizeof(lvi));

	int row = m_QEListCtrl.GetItemCount();

	CQuickEditItem* pItem =  new CQuickEditItem(lpAttribute, lpValue);
	lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
	lvi.lParam = (LONG)pItem;
	lvi.iItem = row;
	lvi.iSubItem = 0;
	lvi.pszText = 0;
	int index = m_QEListCtrl.InsertItem(&lvi);
	QE_ListCtrl_SetItemText(row, lpAttribute, lpValue);

	return index;
}

void CAttributeEditor::QE_LoadListCtrl(int row)
{
	if (m_QEListCtrl.m_hWnd) {
		int i;
		m_QEListCtrl.DeleteAllItems ();

		CStringArray strAttributes;
		CStringArray strValues;
		ReadQEData(strAttributes, strValues);
		int nItems = strAttributes.GetSize ();
		for (i = 0; i < nItems; i++) {
			QE_ListCtrl_AddItem(strAttributes[i], strValues[i]);
		}
		strAttributes.RemoveAll ();
		strValues.RemoveAll ();
		if (nItems > 1) {
			bAscendSort = true;
			m_QEListCtrl.SortItems((PFNLVCOMPARE)AE_CompareFunction, 0);
			iLastSubItem = 0;
			bAscendSort = false;
		}
		if (nItems > 0) {
			m_QEListCtrl.SetItemState(row, LVIS_SELECTED, LVIS_SELECTED);
			UpdateData(false);
			//m_QEListCtrl.SetItemState(row, LVIS_SELECTED, LVIS_SELECTED);
		}
	}
}

void CAttributeEditor::SaveQEData(void)
{
	CStringArray strAttributes, strValues;
	LV_ITEM lvi;
	int max = m_QEListCtrl.GetItemCount();
	for (int i = 0; i < max; i++) {
		memset (&lvi, 0, sizeof(lvi));
		lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
		lvi.iItem = i;
		m_QEListCtrl.GetItem (&lvi);
		CQuickEditItem* pItem = (CQuickEditItem*)lvi.lParam;
		strAttributes.Add (pItem->m_Attribute);
		strValues.Add (pItem->m_Value);
	}
	WriteQEData (strAttributes, strValues);
	strAttributes.RemoveAll ();
	strValues.RemoveAll ();
}

#define DELIMITER2 0x2
void CAttributeEditor::WriteQEData(CStringArray& strAttributes, CStringArray& strValues)
{
	int nItems = strAttributes.GetSize ();
	CString strEntries = _T("");
	for (int i = 0; i < nItems; i++) {
		CQuickEditItem obItem(strAttributes[i], strValues[i]);
		CString strData;
		obItem.FormatData(strData);
		strEntries += strData;
		strEntries += 
			(CString)(_TCHAR)	// 01/12/03 AM.
			DELIMITER2;
	}
	theApp.WriteProfileString(_T("PREFERENCES"),_T("QuickEditData"),strEntries);
}

void CAttributeEditor::ReadQEData(CStringArray& strAttributes, CStringArray& strValues)
{
	CString strItems = theApp.GetProfileString(_T("PREFERENCES"),_T("QuickEditData"),_T(""));
	int posStart = 0;
	int posEnd;
	while (-1 != (posEnd = strItems.Find(DELIMITER2,posStart)) ) {
		CString strData = strItems.Mid(posStart, posEnd - posStart);
		CQuickEditItem obItem;
		obItem.RetrieveData(strData);
		strAttributes.Add(obItem.m_Attribute);
		strValues.Add(obItem.m_Value);
		posStart = posEnd + 1;
	}
}

BOOL CAttributeEditor::QEAllFieldsComplete(void)
{
	if (m_QE_Attribute.IsEmpty()) {
		MessageBox (_T("Need an Attribute!"));
		GetDlgItem(IDE_ATTRIBUTE)->SetFocus();
		return false;
	}
#if 0
	if (m_QE_Value.IsEmpty()) {
		MessageBox(_T("Need a Value!"));
		GetDlgItem(IDE_VALUE)->SetFocus();
		return false;
	}
#endif
	return true;
}

void CAttributeEditor::QEClearAllFields (void)
{
	m_QE_Attribute.Empty();
	m_QE_Value.Empty();
	UpdateData(false);
}

void CAttributeEditor::OnAdd() 
{
	UpdateData();
	if (!QEAllFieldsComplete())
		return;
	CQuickEditItem obItem(m_QE_Attribute, m_QE_Value);
	int index;
	if ((index = QE_ListCtrl_FindItem(&obItem)) == -1) {
		QE_ListCtrl_AddItem(m_QE_Attribute, m_QE_Value);
		bAscendSort = true;
		m_QEListCtrl.SortItems((PFNLVCOMPARE)AE_CompareFunction, 0);
		if ((index = QE_ListCtrl_FindItem(&obItem)) != -1) {
			int max = m_QEListCtrl.GetItemCount();
			for (int n = 0; n < max; n++)
				m_QEListCtrl.SetItemState(n, 0, LVIS_SELECTED);
			m_QEListCtrl.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
			AdjustQEItem(index);
		}
		QEClearAllFields();
		GetDlgItem(IDE_ATTRIBUTE)->SetFocus();
	}
	else {
		MessageBox(_T("Attribute-Value Combination is already in the list!"));
		m_QEListCtrl.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CAttributeEditor::OnRemove() 
{
	CWordArray selArray;
	int max = QE_ListCtrl_GetCurSel(selArray);
	if (0 == max) return;

	CString msg;
	msg.Format(_T("Delete current selection%s?"),max > 1 ? _T("s") : _T(""));
	if (IDYES != MessageBox(msg, NULL, MB_YESNO))
		return;

	for (int n = max - 1; n >= 0; --n) {
		int index = selArray[n];
		m_QEListCtrl.DeleteItem(index);
	}
	int nCount = m_QEListCtrl.GetItemCount();
	if (nCount) {
		int lowIndex = selArray[0];
		if (lowIndex >= nCount)
			lowIndex = nCount - 1;
		m_QEListCtrl.SetItemState(lowIndex, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CAttributeEditor::OnUpdate() 
{
	UpdateData();
	if (!QEAllFieldsComplete ())
		return;

	CWordArray selArray;
	if (QE_ListCtrl_GetCurSel(selArray) != 1) return;

	int index = selArray[0];
	CQuickEditItem* pItem = (CQuickEditItem*)m_QEListCtrl.GetItemData(index);
	if (pItem) {
		pItem->m_Attribute = m_QE_Attribute;
		pItem->m_Value = m_QE_Value;
		QE_ListCtrl_SetItemText(index, m_QE_Attribute, m_QE_Value);
		bAscendSort = true;
		m_QEListCtrl.SortItems((PFNLVCOMPARE)AE_CompareFunction, 0);
		if ((index = QE_ListCtrl_FindItem(pItem)) != -1) {
			m_QEListCtrl.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
			AdjustQEItem(index);
		}
	}
}

void CAttributeEditor::OnKbEdit() 
{
	if (!m_concept) return;

	CWordArray selArray;
	int max = QE_ListCtrl_GetCurSel(selArray);
	if (0 == max) return;

	CString strAttrAdd, strValAdd;
	CString strAttrRem, strValRem;
	strAttrAdd.Empty();
	strValAdd.Empty();
	strAttrRem.Empty();
	strValRem.Empty();
	for (int n = 0; n < max; n++) {
		int index = selArray[n];
		CQuickEditItem* pItem = (CQuickEditItem*)m_QEListCtrl.GetItemData(index);
		if (pItem) {
			_TCHAR attribute[256];
			_TCHAR value[256];
			_tcscpy_s(attribute, pItem->m_Attribute);
			_tcscpy_s(value, pItem->m_Value);
			BOOL bFound = false;
			if (value[0])
				bFound = cg->attrWithval(m_concept,attribute,value);
			else
				bFound = (BOOL)cg->findAttr(m_concept, attribute);
			if (bFound) {
				if (value[0])
					cg->rmAttrval(m_concept,attribute,value);
				else if (!cg->findVals(m_concept,attribute))
					cg->rmAttr(m_concept,attribute);
				if (strAttrRem.IsEmpty()) {
					strAttrRem = attribute;
					strValRem = value;
				}
			}
			else {
				cg->addVal(m_concept,attribute,value);
				if (strAttrAdd.IsEmpty()) {
					strAttrAdd = attribute;
					strValAdd = value;
				}
			}
			ReIcon();
		}
	}
	if (!strAttrAdd.IsEmpty()) {
		InitListWithData(strAttrAdd, strValAdd);
	}
	else if (!strAttrRem.IsEmpty()) {
		int row = FindAttributeValue(strAttrRem, strValRem);
		int rowStart = FindAttributeValue(strAttrRem, NULL);
		if (-1 != row) {
			if (row > 0)
				--row;
			if (row < rowStart)
				row = rowStart;
			InitListWithData(row);
		}
	}
}

int CAttributeEditor::FindNumAttrValEntries(LPCTSTR attribute)
{
	int max = m_listCtrl.GetItemCount();
	int num = 0;
	for (int n = 0; n < max; n++) {
		CLineItem* pLineItem = (CLineItem*)m_listCtrl.GetItemData(n);
		if (pLineItem) {
			CString strAttr = pLineItem->GetName();
			CString strVal = pLineItem->GetValue();
			if (!strAttr.CompareNoCase(attribute) && !strVal.IsEmpty())
				num++;
		}
	}
	return num;
}

int CAttributeEditor::FindAttributeValue(LPCTSTR attribute, LPCTSTR value)
{
	int max = m_listCtrl.GetItemCount();
	for (int n = 0; n < max; n++) {
		CLineItem* pLineItem = (CLineItem*)m_listCtrl.GetItemData(n);
		if (pLineItem) {
			CString strAttr = pLineItem->GetName();
			CString strVal = pLineItem->GetValue();
			if (!strAttr.CompareNoCase(attribute) && (value == NULL || !strVal.CompareNoCase(value)) )
				return n;
		}
	}
	return -1;
}

void CAttributeEditor::AdjustQEItem(int nItem)
{
	CString strAttr = m_QEListCtrl.GetItemText(nItem,1);
	CString strValue = m_QEListCtrl.GetItemText(nItem,2);
	int index = FindAttributeValue(strAttr, strValue);
	BOOL bNone = strValue.IsEmpty() ? FindNumAttrValEntries(strAttr) > 0 : false;
	m_QEListCtrl.SetItemText(nItem, 0, bNone ? _T("None") : -1 == index ? _T("Add") : _T("Del"));
}

void CAttributeEditor::AdjustQEList(void)
{
	int max = m_QEListCtrl.GetItemCount();
	for (int n = 0; n < max; n++)
		AdjustQEItem(n);
}

void CAttributeEditor::OnQuickedithelp() 
{
	MessageBox(
  _T("                QUICK EDIT INSTRUCTIONS")
  _T("\n")
  _T("\n         You can create a list of commonly used")
  _T("\n          attributes or attribute-value pairs")
  _T("\n             using the Quick Edit Table.")
  _T("\n")
  _T("\n      Enter attribute or attribute/value information")
  _T("\n           in the Attribute and Value boxes.")
  _T("\nUse the Add button to add an attribute or attribute/value pair.")
  _T("\n       Use the Delete button to delete the selected")
  _T("\n           attribute(s) or attribute/value(s).")
  _T("\n")
  _T("\n    To edit an existing attribute or attribute-value pair,")
  _T("\n       make changes in the Attribute or Value boxes")
  _T("\n              and click the Update button.")
  _T("\n")
  _T("\n      To assign attributes or attribute-value pairs")
  _T("\n              to a Knowledge Base concept")
  _T("\n   either double-click on the entry you want to assign, or")
  _T("\n      select the entries and click on the KB Edit button.")
	,_T("Attribute Editor: Quick Edit Help"), MB_OK);
}

void CAttributeEditor::OnColumnclickListquickedit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (iLastSubItem != pNMListView->iSubItem) {
		iLastSubItem = pNMListView->iSubItem;
		bAscendSort = true;
	}
	// TODO: Add your control notification handler code here
	m_QEListCtrl.SortItems((PFNLVCOMPARE)AE_CompareFunction, pNMListView->iSubItem);
	*pResult = 0;
	bAscendSort = !bAscendSort;
}

void CAttributeEditor::OnDeleteitemListquickedit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	CQuickEditItem* pItem = (CQuickEditItem*)pNMListView->lParam;
	if (pItem) delete pItem;
	pNMListView->lParam = 0;
	*pResult = 0;
}

void CAttributeEditor::OnItemchangedListquickedit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	CWordArray selArray;
	if (QE_ListCtrl_GetCurSel(selArray) == 1) {
		int index = selArray[0];
		CQuickEditItem* pItem = (CQuickEditItem*)m_QEListCtrl.GetItemData(index);
		if (pItem) {
			m_QE_Attribute = pItem->m_Attribute;
			m_QE_Value = pItem->m_Value;
			UpdateData(false);
		}
	}
	*pResult = 0;
}

void CAttributeEditor::OnDblclkListquickedit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnKbEdit();
	*pResult = 0;
}

void CAttributeEditor::OnQuickedit()
{
	CString strText;
	if (0 == m_nDlg) {
		m_nDlg = 1;
		strText = _T("<< &Quick Edit");
	}
	else {
		m_nDlg = 0;
		strText = _T("&Quick Edit >>");
	}
	SetWindowPos(NULL,0,0,m_rDlg[m_nDlg].Width(),m_rDlg[m_nDlg].Height(),SWP_NOMOVE);
	SetDlgItemText(IDC_QUICKEDIT, strText);
}

void CAttributeEditor::OnDropdownComboLookup() 
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LOOKUP);
	/*
	pCombo->SetWindowPos(NULL, 0, 0,
		m_ComboWidth + GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXBORDER),
		GetSystemMetrics(SM_CYSMICON), SWP_NOMOVE);
	*/
}

void CAttributeEditor::OnCloseupComboLookup() 
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LOOKUP);
	/*
	pCombo->SetWindowPos(NULL, 0, 0,
		GetSystemMetrics(SM_CXSMICON) + (2*GetSystemMetrics(SM_CXBORDER)),
		GetSystemMetrics(SM_CYSMICON) + (2*GetSystemMetrics(SM_CYBORDER)),
		SWP_NOMOVE);
	*/
}

void CAttributeEditor::OnSelendokComboLookup() 
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_LOOKUP);
	int idx = pCombo->GetCurSel();
	if (CB_ERR == idx || idx >= m_strURLs.GetSize()) return;
	if (m_strConceptName.IsEmpty()) return;
	m_mainWnd->OpenBrowserDictLookup(m_strURLs[idx], m_strConceptName);
}

void CAttributeEditor::OnLookup() 
{
	int idx = m_URLcounter++;
	if (m_URLcounter >= m_strURLs.GetSize())
		m_URLcounter = 0;
	if (idx >= m_strURLs.GetSize()) return;
	if (m_strConceptName.IsEmpty()) return;
	m_mainWnd->OpenBrowserDictLookup(m_strURLs[idx], m_strConceptName);
}

void CAttributeEditor::OnDblclkAttributeList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->iItem >= 0) {
		CLineItem* pLineItem = (CLineItem*)m_listCtrl.GetItemData(pNMListView->iItem);

		if (pLineItem && !m_bInitListWithData && pLineItem->GetValueType() == valConcept) {
			//OnOpenKb();
		}
	}
	
	*pResult = 0;
}

void CAttributeEditor::OnBrowseConcept() 
{
	UpdateData(true);
	int nItem;
	CLineItem *pLineItem = GetSelectedLine(nItem);

	if (pLineItem && pLineItem->GetValueType() == valConcept) {
		if (m_boolBrowseConcept)
			OpenKb(pLineItem->GetConcept());
		//else
			//OpenKb(m_concept);
	}
}

// Trying one of my own.	// 06/19/02 AM.
void CAttributeEditor::OnSelchangeValueType() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);

	CComboBox *cb = (CComboBox *)GetDlgItem(IDC_VALUE_TYPE);
	int nIndex = cb->GetCurSel();
	switch (nIndex)
		{
		case 0:
			m_strValueType = _T("string");	// Consistent with doc.	// 06/19/02 AM.
			break;
		case 1:
			m_strValueType = _T("concept");
			break;
		case 2:
			m_strValueType = _T("numeric");
			break;
		default:
			break;
		}

	m_listCtrl.SetAttributeType(m_strValueType);
}

void CAttributeEditor::OnEnChangeSearch()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
