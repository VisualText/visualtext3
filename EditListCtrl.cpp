/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// EditListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "kb.h"
#include "Utils.h"
#include "MainFrm.h"
#include "InPlaceEdit.h"
#include "EditListCtrl.h"

extern bool kbDirty;
extern CVisualTextApp theApp;
extern CG *cg;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAttribute::CAttribute()
{
	m_bOpen = false;
	m_Name.Empty();
	m_strValues.RemoveAll();
	m_ValTypes.RemoveAll();
}

CAttribute::CAttribute(LPCTSTR lpName)
{
	m_bOpen = false;
	m_Name = lpName;
	m_strValues.RemoveAll();
	m_ValTypes.RemoveAll();
}

CAttribute::~CAttribute()
{
	m_strValues.RemoveAll();
	m_ValTypes.RemoveAll();
}

LPCTSTR lpNullValue = _T("\xff\xff\xff\xff\xff\xff");
BOOL CAttribute::IsNullValue(int nVal)
{
	return GetValue(nVal) == lpNullValue;
}

int CAttribute::AddNullValue(void)
{
	m_strValues.Add(lpNullValue);
	m_ValTypes.Add(CalcValueType(lpNullValue));
	return GetNumValues() - 1;
}

int CAttribute::AddValue(LPCTSTR lpValue, CONCEPT *concept)
{
	m_strValues.Add(lpValue);
	if (concept)
		m_ValTypes.Add(valConcept);
	else
		m_ValTypes.Add(CalcValueType(lpValue));
	m_concepts.AddTail(concept);
	return GetNumValues() - 1;
}

ValueType CAttribute::CalcValueType(LPCTSTR lpValue)
{
	if (lpNullValue == lpValue)
		return valNULL;
	int max = _tcsclen(lpValue);
	if (0 == max)
		return valAlpha;
	// SRP 010413

	if (IsDigit(CString(lpValue)))
		return valNumber;
	return valAlpha;
}

void CAttribute::DeleteValue(int nPos)
{
	if (GetNumValues() > nPos) {
		m_strValues.RemoveAt(nPos);
		m_ValTypes.RemoveAt(nPos);
		POSITION pos = m_concepts.FindIndex(nPos);
		if (pos)	// 06/18/02 AM.
			m_concepts.RemoveAt(pos);
	}
}

CString CAttribute::GetName(void)
{
	return m_Name;
}

BOOL CAttribute::IsOpen(void)
{
	return m_bOpen;
}

void CAttribute::SetName(LPCTSTR lpName)
{
	m_Name = lpName;
}

int CAttribute::GetNumValues(void)
{
	return m_strValues.GetSize();
}

CString CAttribute::GetValue(int nVal)
{
	return m_strValues.GetAt(nVal);
}

CONCEPT *CAttribute::GetConcept(int nVal)
{
	POSITION pos = m_concepts.FindIndex(nVal);
	return (CONCEPT *)m_concepts.GetAt(pos);
}

ValueType CAttribute::GetValueType(int nVal)
{
	switch (m_ValTypes.GetAt(nVal)) {
		case 0: return valNULL;
		case 1: return valAlpha;
		case 2: return valNumber;
		default: return valConcept;
	}
}

void CAttribute::SetOpen(bool bVal)
{
	m_bOpen = bVal;
}

int CAttribute::SetValue(int nIndex, LPCTSTR lpValue, CONCEPT *concept,
	ValueType typ)	// 06/19/02 AM.
{
	m_strValues.SetAt(nIndex, lpValue);
	if (typ == valConcept)	// 06/19/02 AM.
		m_ValTypes.SetAt(nIndex, valConcept);
	else
//		m_ValTypes.SetAt(nIndex, CalcValueType(lpValue));	// 06/19/02 AM.
		m_ValTypes.SetAt(nIndex, typ);	// 06/19/02 AM.
	POSITION pos = m_concepts.FindIndex(nIndex);
	m_concepts.SetAt(pos,concept);
	return nIndex;
}

/////////////////////////////////

CLineItem::CLineItem()
{
	m_pAttribute = NULL;
	m_Index = 0;
	m_bDeleteValue = true;
}

CLineItem::CLineItem(CAttribute* pAttribute, int index)
{
	m_pAttribute = pAttribute;
	m_Index = index;
	m_bDeleteValue = true;
}

CLineItem::~CLineItem()
{
	if (m_bDeleteValue)
		DeleteValue();
}

int CLineItem::AddValue(LPCTSTR lpValue, CONCEPT *concept)
{
	return m_pAttribute->AddValue(lpValue,concept);
}

int CLineItem::DecrementIndex(void)
{
	return --m_Index;
}

void CLineItem::DeleteValue(void)
{
if (m_pAttribute)	// 06/19/02 AM.
	m_pAttribute->DeleteValue(m_Index);
}

CAttribute* CLineItem::GetAttribute(void)
{
	return m_pAttribute;
}

void CLineItem::DeleteAttribute()	// 06/19/02 AM.
{
delete m_pAttribute;	// 06/19/02 AM.
m_pAttribute = 0;	// 06/19/02 AM.
m_bDeleteValue = false;	// 06/19/02 AM.
}

int CLineItem::GetIndex(void)
{
	return m_Index;
}

CString CLineItem::GetName(void)
{
	return m_pAttribute->GetName();
}

int CLineItem::GetNumValues(void)
{
	return m_pAttribute->GetNumValues();
}

CString CLineItem::GetValue(void)
{
	if (m_pAttribute->IsNullValue(m_Index))
		return _T("");
	return m_pAttribute->GetValue(m_Index);
}

ValueType CLineItem::GetValueType(void)
{
	return m_pAttribute->GetValueType(m_Index);
}

CString CLineItem::GetValueTypeString(void)
{
	ValueType type = m_pAttribute->GetValueType(m_Index);
	CString typeStr = _T("");
	if (type == valAlpha)
		typeStr = _T("string");
	else if (type == valNumber)
		typeStr = _T("numeric");
	else if (type == valConcept)
		typeStr = _T("concept");

	return typeStr;
}

CONCEPT *CLineItem::GetConcept(void)
{
	if (m_pAttribute->IsNullValue(m_Index))
		return _T("");
	return m_pAttribute->GetConcept(m_Index);
}

BOOL CLineItem::IsOpen(void)
{
	return m_pAttribute->IsOpen();
}

void CLineItem::SetIndex(int newIndex)
{
	m_Index = newIndex;
}

void CLineItem::SetName(LPCTSTR lpName)
{
	m_pAttribute->SetName(lpName);
}

void CLineItem::SetOpen(bool bVal)
{
	m_pAttribute->SetOpen(bVal);
}

int CLineItem::SetValue(LPCTSTR lpValue, CONCEPT *concept,
	ValueType typ)	// 06/19/02 AM.
{
	return m_pAttribute->SetValue(m_Index, lpValue, concept,
			typ);	// 06/19/02 AM.
}

/////////////////////////////////////////////////////////////////////////////
// CEditListCtrl

CEditListCtrl::CEditListCtrl()
{
	Init();
}

CEditListCtrl::~CEditListCtrl()
{
}

CONCEPT *CEditListCtrl::GetConcept() 
{
	return m_concept;
}

void CEditListCtrl::SetConcept(CONCEPT *concept) 
{
	m_concept = concept;
}

void CEditListCtrl::Init() 
{
	m_IgnoreEditColumn0 = false;
	m_concept = NULL;
	m_valueTypeNew = valNULL;
}

BEGIN_MESSAGE_MAP(CEditListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CEditListCtrl)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditListCtrl message handlers

void CEditListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( GetFocus() != this )
		SetFocus();
	
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CEditListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( GetFocus() != this )
		SetFocus();
	
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CEditListCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM	*plvItem = &pDispInfo->item;

	int nLine = plvItem->iItem;
	CLineItem* pLineItem = (CLineItem*)GetItemData(nLine);
	EndLabelEdit(plvItem->iItem,plvItem->iSubItem,plvItem->pszText);

	*pResult = 0;
}

void CEditListCtrl::EndLabelEdit(int line, int subItem, CString newStr)
{
	CLineItem* pLineItem = (CLineItem*)GetItemData(line);
	CString attrStr = _T("");
	CString valStr = _T("");
	ValueType valType = valNULL;	// 06/19/02 AM.
	CONCEPT *con = NULL;
	if (pLineItem) {
		attrStr = pLineItem->GetName();
		valStr = pLineItem->GetValue();
		con = pLineItem->GetConcept();
		valType = pLineItem->GetValueType();	// 06/19/02 AM.
	}

	// IF ATTRIBUTE BEING EDITED
	if (subItem == 0) {
		if (attrStr.IsEmpty() && newStr.IsEmpty()) {
			// need this code if the user begins to edit, blanks the field, then switches to another
			// application, then back to Visual Text - prevents the addition of a blank attribute
			GetParent()->PostMessage(WMU_DELETELINE, (WPARAM)line);
		}
		else if (attrStr != newStr && !newStr.IsEmpty()) {
			// editing an attribute
			if (pLineItem) {
				if (!attrStr.IsEmpty()) {
					ConceptAttributeRename(m_concept,attrStr,newStr);
					pLineItem->SetName(newStr);
				}
			}
			// a new attribute
			else {
				CAttribute* pAttribute = new CAttribute(newStr);
				CLineItem* pLineItemNew = new CLineItem(pAttribute, 0);
				CONCEPT *root = NULL;

				// TEST TO SEE IF NEW ATTRIBUTE VALUE IS TO BE A CONCEPT
				switch (m_valueTypeNew) {		// 06/18/02 AM.
					case valAlpha:
						pLineItemNew->AddValue(_T("New Value"));
						break;
					case valNumber:
						pLineItemNew->AddValue(_T("0"));
						break;
					case valConcept: 
						root = cg->findRoot();
						pLineItemNew->AddValue(ConceptName(root),root);
						break;
					case valNULL:
					default:
						pLineItemNew->AddValue(_T("New Value"));
						break;
				}
//				if (m_valueTypeNew != valNULL) {
//					root = cg->findRoot();
//					pLineItemNew->AddValue(ConceptName(root),root);
//				}
//				else
//					pLineItemNew->AddValue("New Value");

				SetItemData(line, (LPARAM)pLineItemNew);
				ConceptAttributeAdd(m_concept,newStr,root);
				EditSubLabel(line, 1);
			}
		}
	}

	// IF VALUE BEING EDITED
	else if (subItem == 1 && (valStr != newStr
		|| valType != m_valueTypeNew) ) {	// 06/19/02 AM.
		// E.g., "concept" can be alpha or concept type!.	// 06/19/02 AM.
		// E.g., "123" can be alpha/num.
		if (!newStr.IsEmpty()) {
			// SRP 010413 Check for numeric value overflow
			BOOL bError = false;
			ValueType typ = m_valueTypeNew;	// 06/19/02 AM.
//			ValueType typ = ValueTypeFromString(m_strValueType);
//			switch (typ)	// 06/18/02 AM.
			switch (typ)		// 06/19/02 AM.
				{
				case valConcept:	// 06/18/02 AM.
					{
					CONCEPT *newCon = ConceptFromPath(newStr,true);
					if (newCon) {
						if (valStr == _T(""))
							ConceptAttributeUpdate(m_concept,attrStr,newCon);
						else
							ConceptAttributeUpdate(m_concept,attrStr,ConceptFromPath(valStr,true),newCon);
						pLineItem->SetValue(newStr,newCon, typ);
					}
					else {
						CString msgStr;
						msgStr.Format(_T("Concept path does not exist:\n%s"),newStr);
						AfxMessageBox(msgStr);
					}
					}
					break;
				case valNumber:	// 06/18/02 AM.
					{
					if (IsDigit(newStr)) {
						_TCHAR *stopstring;
						double dblVal = _tcstod(newStr, &stopstring);
						if (bError = dblVal < (double)(long)MINLONG || dblVal > (double)(long)MAXLONG) {
							CString strMsg;
							strMsg.Format(_T("Numeric value is out of range:\nMinimum: %ld\nMaximum: %ld"), MINLONG, MAXLONG);
							AfxMessageBox(strMsg, MB_OK);
						}
					}
					else {
						bError = true;	// 06/18/02 AM.
						CString strMsg;
						strMsg.Format(_T("Nonnumeric value."));
						AfxMessageBox(strMsg, MB_OK);
					}

					if (!bError) {
						pLineItem->SetValue(newStr,0,typ);
						if (valStr == _T("")) {
							ConceptAttributeUpdate(m_concept,attrStr,newStr);
						}
						else {
							ConceptAttributeUpdate(m_concept,attrStr,valStr,newStr);
						}
						kbDirty = true;
					}
					}
					break;
				case valAlpha:	// 06/18/02 AM.
					{
					pLineItem->SetValue(newStr,0,typ);
					if (valStr == _T("")) {
						ConceptAttributeUpdate(m_concept,attrStr,newStr);
					}
					else {
						ConceptAttributeUpdate(m_concept,attrStr,valStr,newStr);
					}
					kbDirty = true;
					}
					break;
				default:	// 06/18/02 AM.
					{
					CString strMsg;
					strMsg.Format(_T("Bad value type."));
					AfxMessageBox(strMsg, MB_OK);
					}
					break;
				}
		}
	}
}

void CEditListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	logmsg(_T("CEditListCtrl::OnLbuttonDown"));	// 05/14/08 AM.
	int index;
	CListCtrl::OnLButtonDown(nFlags, point);

	int colnum;
	if( ( index = HitTestEx( point, &colnum )) != -1 )
	{
		CRect rect;
		GetItemRect(index, &rect, LVIR_BOUNDS);
		int colwidth0 = GetColumnWidth(0);
		CImageList* pImageList = (CImageList*)GetParent()->SendMessage(WMU_GETIMAGELIST);
		int cxImage = 0, cyImage = 0; // SRP 010208
		if (pImageList) // SRP 010208
			ImageList_GetIconSize((HIMAGELIST)*pImageList, &cxImage, &cyImage);
		switch (colnum) {
			case 0:
				if (point.x >= rect.left && point.x <= (rect.left + cxImage)) {
					m_IgnoreEditColumn0 = true;
					CLineItem* pLineItem = (CLineItem*)GetItemData(index);
					if (pLineItem) {
						int numVals = pLineItem->GetNumValues();
						if (numVals > 1 && pLineItem->GetIndex() == 0) {
							if (pLineItem->IsOpen())
								ContractValues(index);
							else
								ExpandValues(index);
						}
					}
					return;
				}
				break;
			case 1:
			default:
				rect.left += colwidth0;
				if (point.x >= rect.left && point.x <= (rect.left + cxImage)) {
//					MessageBox("Icon-Column 2");
					return;
				}
				break;
		}
		UINT flag = LVIS_FOCUSED;
		if( (GetItemState( index, flag ) & flag) == flag && colnum > 0)
		{
			// Add check for LVS_EDITLABELS
			if( GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS )
				EditSubLabel( index, colnum );
		}
		else
			SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED ,
				    	LVIS_SELECTED | LVIS_FOCUSED); 
	}
}

// HitTestEx	- Determine the row index and column index for a point
// Returns	- the row index or -1 if point is not over a row
// point	- point to be tested.
// col		- to hold the column index
int CEditListCtrl::HitTestEx(CPoint &point, int *col) const
{
	int colnum = 0;
	int row = HitTest( point, NULL );
	
	if( col ) *col = 0;

	// Make sure that the ListView is in LVS_REPORT
	if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
		return row;

	// Get the top and bottom row visible
	row = GetTopIndex();
	int bottom = row + GetCountPerPage();
	if( bottom > GetItemCount() )
		bottom = GetItemCount();
	
	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows
	for( ;row <=bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetItemRect( row, &rect, LVIR_BOUNDS );
		if( rect.PtInRect(point) )
		{
			// Now find the column
			for( colnum = 0; colnum < nColumnCount; colnum++ )
			{
				int colwidth = GetColumnWidth(colnum);
				if( point.x >= rect.left 
					&& point.x <= (rect.left + colwidth ) )
				{
					if( col ) *col = colnum;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}

// EditSubLabel		- Start edit of a sub item label
// Returns		- Temporary pointer to the new edit control
// nItem		- The row index of the item to edit
// nCol			- The column of the sub item.
CEdit* CEditListCtrl::EditSubLabel( int nItem, int nCol )
{
	// The returned pointer should not be saved

	// Make sure that the item is visible
	if( !EnsureVisible( nItem, true ) ) return NULL;

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( nCol >= nColumnCount || GetColumnWidth(nCol) < 5 )
		return NULL;

	// Get the column offset
	int offset = 0;
	for( int i = 0; i < nCol; i++ )
		offset += GetColumnWidth( i );

	CRect rect;
	GetItemRect( nItem, &rect, LVIR_BOUNDS );

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect( &rcClient );
	if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll( size );
		rect.left -= size.cx;
	}

	// Get Column alignment
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn( nCol, &lvcol );
	DWORD dwStyle ;
	if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else dwStyle = ES_CENTER;

	rect.left += offset+4;
	rect.right = rect.left + GetColumnWidth( nCol ) - 3 ;
	if( rect.right > rcClient.right) rect.right = rcClient.right;

	CImageList* pImageList = (CImageList*)GetParent()->SendMessage(WMU_GETIMAGELIST);
	int cxImage = 0, cyImage = 0;
	if (pImageList)
		ImageList_GetIconSize((HIMAGELIST)*pImageList, &cxImage, &cyImage);
	rect.left += cxImage;

	CString strItemText = _T("");
	CLineItem* pLineItem = (CLineItem*)GetItemData(nItem);
	if (pLineItem) {
		if (0 == nCol)
			strItemText = pLineItem->GetName();
		else if (1 == nCol)
			strItemText = pLineItem->GetValue();
	}
	dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
	CEdit *pEdit = new CInPlaceEdit(nItem, nCol, strItemText);
	pEdit->Create( dwStyle, rect, this, IDC_ATTRIBUTE_LIST );

	return pEdit;
}

void CEditListCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nIndex = pDispInfo->item.iItem;
	CLineItem* pLineItem = (CLineItem*)GetItemData(nIndex);
	BOOL bAbleToEdit = NULL == pLineItem || 0 == pLineItem->GetIndex();
	if (bAbleToEdit && !m_IgnoreEditColumn0)
		EditSubLabel(nIndex, 0);
	m_IgnoreEditColumn0 = false;
	
	*pResult = 1; // we'll do it ourselves
}

void CEditListCtrl::ExpandValues(int nLineFirst)
{
	CLineItem* pLIFirst = (CLineItem*)GetItemData(nLineFirst);
	if (pLIFirst) {
		CAttribute* pAttribute = pLIFirst->GetAttribute();
		int numVals = pLIFirst->GetNumValues();
		int nLineInsert = nLineFirst + 1;
		for (int n = 1; n < numVals; n++) {
			CLineItem* pLineItemNew = new CLineItem(pAttribute, n);
			LV_ITEM lvi;
			memset (&lvi, 0, sizeof(lvi));
			lvi.mask = LVIF_STATE | LVIF_PARAM;
			lvi.lParam = (LPARAM)pLineItemNew;
			lvi.iItem = nLineInsert++;
			int index =InsertItem(&lvi);
		}
	}
	pLIFirst->SetOpen(true);
	CRect rect;
	GetItemRect(nLineFirst, &rect, LVIR_BOUNDS);
	InvalidateRect(rect, false);
}

void CEditListCtrl::ContractValues(int nLineFirst)
{
	CLineItem* pLIFirst = (CLineItem*)GetItemData(nLineFirst);
	if (pLIFirst) {
		CAttribute* pAttribute = pLIFirst->GetAttribute();
		int max = GetItemCount();
		int nLineLast = nLineFirst;
		for (; nLineLast + 1 < max; nLineLast++) {
			CLineItem* pLI = (CLineItem*)GetItemData(nLineLast + 1);
			if (!pLI || pLI->GetAttribute() != pAttribute)
				break;
		}
		for (int n = nLineLast; n > nLineFirst; --n) {
			CLineItem* pLI = (CLineItem*)GetItemData(n);
			if (pLI)
				pLI->m_bDeleteValue = false;
			DeleteItem(n);
		}
	}
	pLIFirst->SetOpen(false);
	CRect rect;
	GetItemRect(nLineFirst, &rect, LVIR_BOUNDS);
	InvalidateRect(rect, false);
}

void CEditListCtrl::SetAttributeType(ValueType type)
{
	m_valueTypeNew = type;
}

void CEditListCtrl::SetAttributeType(CString typeStr)
{
	if (typeStr == _T("concept"))
		m_valueTypeNew = valConcept;
	else if (typeStr == _T("numeric"))
		m_valueTypeNew = valNumber;
	else
		m_valueTypeNew = valAlpha;
}

// 06/19/02 AM.
ValueType CEditListCtrl::ValueTypeFromString(CString typeStr)
{
	if (typeStr == _T("concept"))
		return valConcept;
	else if (typeStr == _T("numeric"))
		return valNumber;
	else if (typeStr == _T("string"))
		return valAlpha;
	else
		return valNULL;
}
