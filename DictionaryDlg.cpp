/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// DictionaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Utils.h"
#include "kb.h"
#include "MainFrm.h"
#include "DictionaryDlg.h"

extern CG *cg;
extern bool kbDirty;
extern CVisualTextApp theApp;

_TCHAR *attrDisplayStrs [] = {
	_T("All"),
	_T("With"),
	_T("Without"),
	_T("xxyyzz")
};

_TCHAR *findStrs [] = {
	_T("Find"),
	_T("Find Off"),
	_T("Narrow"),
	_T("xxyyzz")
};

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDictionaryDlg dialog


CDictionaryDlg::CDictionaryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDictionaryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDictionaryDlg)
	m_strWord = _T("");
	m_strCount = _T("");
	m_strAttrsDisplay = _T("");
	m_boolAttributes = false;
	m_strFindType = _T("");
	//}}AFX_DATA_INIT
}


void CDictionaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDictionaryDlg)
	DDX_Control(pDX, IDC_LIST, m_listCtrlList);
	DDX_Control(pDX, IDC_UP, m_buttonUp);
	DDX_Control(pDX, IDC_DICTIONARY_LIST, m_listCtrlDictionary);
	DDX_Control(pDX, IDC_ATTRIBUTES, m_listCtrlAttributes);
	DDX_Text(pDX, IDC_WORD, m_strWord);
	DDX_Text(pDX, IDC_COUNT, m_strCount);
	DDX_CBString(pDX, IDC_ATTRS_DISPLAY, m_strAttrsDisplay);
	DDX_Check(pDX, IDC_ATTRIBUTE_TOGGLE, m_boolAttributes);
	DDX_CBString(pDX, IDC_FIND_TYPE, m_strFindType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDictionaryDlg, CDialog)
	//{{AFX_MSG_MAP(CDictionaryDlg)
	ON_NOTIFY(NM_CLICK, IDC_DICTIONARY_LIST, OnClickDictionaryList)
	ON_NOTIFY(NM_DBLCLK, IDC_DICTIONARY_LIST, OnDblclkDictionaryList)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_BACK_LETTER, OnBackLetter)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	ON_BN_CLICKED(IDC_FORWARD, OnForward)
	ON_BN_CLICKED(IDC_FORWARD_LETTER, OnForwardLetter)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(NM_RCLICK, IDC_DICTIONARY_LIST, OnRclickDictionaryList)
	ON_CBN_SELCHANGE(IDC_ATTRS_DISPLAY, OnAttrComboChange)
	ON_COMMAND(ID_DICTIONRAYWORDS_MARKAS_NOUN_COUNT, OnDictionraywordsMarkasNounCount)
	ON_COMMAND(ID_DICTIONRAYWORDS_MARKAS_NOUN_NOCOUNT, OnDictionraywordsMarkasNounNocount)
	ON_COMMAND(ID_DICTIONRAYWORDS_MARKAS_NOUN_BOTH, OnDictionraywordsMarkasNounBoth)
	ON_COMMAND(ID_DICTIONRAYWORDS_MARKAS_VERB_TRANSITIVE, OnDictionraywordsMarkasVerbTransitive)
	ON_COMMAND(ID_DICTIONRAYWORDS_MARKAS_VERB_INTRANSITIVE, OnDictionraywordsMarkasVerbIntransitive)
	ON_COMMAND(ID_DICTIONRAYWORDS_MARKAS_ADJECTIVE, OnDictionraywordsMarkasAdjective)
	ON_COMMAND(ID_DICTIONRAYWORDS_MARKAS_ADVERB, OnDictionraywordsMarkasAdverb)
	ON_BN_CLICKED(IDC_MOVE_TO_LIST, OnMoveToList)
	ON_BN_CLICKED(IDC_CLEAR_LIST, OnClearList)
	ON_BN_CLICKED(IDC_CLEAR_ALL, OnClearAll)
	ON_BN_CLICKED(IDC_ATTRIBUTE_TOGGLE, OnAttributeToggle)
	ON_BN_CLICKED(IDC_ADD_LIST, OnAddList)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_EN_CHANGE(IDC_WORD, OnChangeWord)
	ON_CBN_SELCHANGE(IDC_FIND_TYPE, OnSelchangeFindType)
	ON_BN_CLICKED(IDC_ADD_ALL, OnAddAll)
	ON_BN_CLICKED(IDC_FILE, OnFile)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_DICTIONARY_LIST, OnCustomDictDraw)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST, OnCustomListDraw)
	ON_BN_CLICKED(IDC_TO_DICT, OnToDict)
	ON_BN_CLICKED(IDC_DELETE_WORD, OnDeleteWord)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDictionaryDlg message handlers

void CDictionaryDlg::FillDictionaryList(BOOL bDoSetRedraw) // SRP 010208
{
	StartWaitCursor();
	if (bDoSetRedraw) // SRP 010208
		SetRedraw(false);

	CONCEPT *sys = cg->findConcept(cg->findRoot(),_T("sys"));
	CONCEPT *dict = cg->findConcept(sys,_T("dict"));
	CONCEPT *a1 = cg->findConcept(dict,_T("a"));
	CONCEPT *folder;

	_TCHAR letterBuf[1001];
	_tcscpy_s(letterBuf,_T("a"));

	if (m_dictDisplayType == DICT_DISPLAY_LETTER) {
		StringToChar(letterBuf,m_strLetter,1000);
	}
	else if (m_dictDisplayType == DICT_DISPLAY_LETTER_GROUP) {
		StringToChar(letterBuf,m_strLetter,1000);
		a1 = cg->findConcept(a1,letterBuf);
		StringToChar(letterBuf,m_strLetterGroup,1000);
	}
	folder = cg->findConcept(a1,letterBuf);

	m_listCtrlDictionary.DeleteAllItems();
	m_intDictCount = 0;
	FillDictionaryList(folder,0,m_intDictCount);


	// UPDATE COUNT STRING
	if (m_dictDisplayType == DICT_DISPLAY_ALPHABET)
		m_strCount.Format(_T("%d"),m_intDictCount);
	else if (m_dictDisplayType == DICT_DISPLAY_LETTER)
		m_strCount.Format(_T("%s %d"),m_strLetter,m_intDictCount);
	else if (m_dictDisplayType == DICT_DISPLAY_LETTER_GROUP)
		m_strCount.Format(_T("%s/%s %d"),m_strLetter,m_strLetterGroup,m_intDictCount);
	UpdateData(false);

	if (bDoSetRedraw) { // SRP 010208
		SetRedraw(true);
		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW); // SRP 010208
	}
	StopWaitCursor();
}

void CDictionaryDlg::FillDictionaryList(CONCEPT *startConcept, int level, int &count)
{
	if (!startConcept)
		return;

	CString parentConceptStr = ConceptName(startConcept);
	CONCEPT *concept = startConcept;
	CString conceptStr;
	ATTR *attrs;
	int image = 0;

	while (concept) {
		conceptStr = ConceptName(concept);
		attrs = cg->findAttrs(concept);

		if (m_dictDisplayType == DICT_DISPLAY_ALPHABET && level == 0 ||
		    m_dictDisplayType == DICT_DISPLAY_LETTER && level == 1 ||
		    m_dictDisplayType == DICT_DISPLAY_LETTER_GROUP && level == 1) {

			if (m_boolUpArrow ||
				(m_findType == FIND_TYPE_NARROW && conceptStr.Find(m_strWord) == 0) ||
				(m_findType != FIND_TYPE_NARROW &&
				 (m_attrDisplayType == ATTR_DISPLAY_ALL ||
				  (m_attrDisplayType == ATTR_DISPLAY_WITH && attrs) ||
				  (m_attrDisplayType == ATTR_DISPLAY_WITHOUT && !attrs)))) {

				image = attrs ? 0 : 1;
				m_listCtrlDictionary.InsertItem(count,conceptStr);
				m_listCtrlDictionary.SetItemData(count,(DWORD)concept);
				count++;
			}
		}
		else if (cg->Down(concept) &&
			     ((m_dictDisplayType == DICT_DISPLAY_LETTER && level == 0) ||
		          m_dictDisplayType == DICT_DISPLAY_LETTER_GROUP)) {
			FillDictionaryList(cg->Down(concept),level+1,count);
			break;
		}

		concept = cg->Next(concept);
	}
}

BOOL CDictionaryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_dictDisplayType = DICT_DISPLAY_ALPHABET;
	m_attrDisplayType = ATTR_DISPLAY_ALL;
	m_findType = FIND_TYPE_FIND_OFF;

	m_intDictSelection = -1;
	m_boolOrphans = false;
	m_boolUpArrow = false;
	
	// SET FOCUS ON ANALYZER NAME
	CEdit *ed = (CEdit *)GetDlgItem(IDC_WORD);
	ed->SetFocus();

	m_comboBoxAttr = (CComboBox *)GetDlgItem(IDC_ATTRS_DISPLAY);
	m_comboBoxAttr->SelectString(-1,_T("All"));

	m_comboBoxFind = (CComboBox *)GetDlgItem(IDC_FIND_TYPE);
	m_comboBoxFind->SelectString(-1,_T("Find Off"));

	m_listCtrlAttributes.InsertColumn(0,_T("Name"),LVCFMT_LEFT,75);
	m_listCtrlAttributes.InsertColumn(1,_T("Value"),LVCFMT_LEFT,300);
	
	FillDictionaryList(false); // SRP 010208

	m_boolAttributes = true;

	m_bitmapButtonUp.LoadBitmap(IDB_UP);
	m_buttonUp.SetBitmap(m_bitmapButtonUp);

	// CREATE TEMP AREA OFR DICTIONARY LIST IN KB
	CONCEPT *tmp = cg->findConcept(cg->findRoot(),_T("tmp"));
	if (!tmp)
		tmp = cg->makeConcept(cg->findRoot(),_T("tmp"));

	m_conceptTemp = cg->findConcept(tmp,_T("dictlist"));
	if (!m_conceptTemp)
		m_conceptTemp = cg->makeConcept(tmp,_T("dictlist"));

	UpdateData(false);

	return false;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

void CDictionaryDlg::OnClickDictionaryList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SelectDictionaryList();
	m_listCtrlAttributes.SetConcept(m_conceptSelected);
	*pResult = 0;
}

void CDictionaryDlg::SelectDictionaryList() 
{
	POSITION pos = m_listCtrlDictionary.GetFirstSelectedItemPosition();

	if (pos) {
		m_intDictSelection = m_listCtrlDictionary.GetNextSelectedItem(pos);
		m_strWord = m_listCtrlDictionary.GetItemText(m_intDictSelection,0);
		m_conceptSelected =
			(CONCEPT *)m_listCtrlDictionary.GetItemData(m_intDictSelection);
		m_listCtrlAttributes.SetConcept(m_conceptSelected);

		if (m_dictDisplayType == DICT_DISPLAY_ALPHABET)
			m_strLetter = m_strWord;
		else if (m_dictDisplayType == DICT_DISPLAY_LETTER)
			m_strLetterGroup = m_strWord;
		else if (m_dictDisplayType == DICT_DISPLAY_LETTER_GROUP)
			FillAttributeList();
	}
	else {
		m_intDictSelection = -1;
	}

	UpdateData(false);
}

void CDictionaryDlg::OnDblclkDictionaryList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DICT_DISPLAY_TYPE type = m_dictDisplayType;

	if (m_dictDisplayType == DICT_DISPLAY_ALPHABET)
		m_dictDisplayType = DICT_DISPLAY_LETTER;
	else if (m_dictDisplayType == DICT_DISPLAY_LETTER)
		m_dictDisplayType = DICT_DISPLAY_LETTER_GROUP;

	if (type != m_dictDisplayType)
		FillDictionaryList();
	
	*pResult = 0;
}

void CDictionaryDlg::OnUp() 
{
	if (m_dictDisplayType == DICT_DISPLAY_LETTER_GROUP) {
		m_dictDisplayType = DICT_DISPLAY_LETTER;
		m_strLetterGroup = _T("");
	}
	else if (m_dictDisplayType == DICT_DISPLAY_LETTER) {
		m_dictDisplayType = DICT_DISPLAY_ALPHABET;
		m_strLetter = _T("");
	}

	m_boolUpArrow = true;
	FillDictionaryList();
	m_boolUpArrow = false;
}

void CDictionaryDlg::FillAttributeList(CONCEPT *concept)
{
	if (!m_boolAttributes)
		return;

	SetRedraw(false);
	m_listCtrlAttributes.DeleteAllItems();
	SetRedraw(true);

	if (!concept && (!m_conceptSelected || m_dictDisplayType != DICT_DISPLAY_LETTER_GROUP))
		return;

	if (!concept)
		concept = m_conceptSelected;

	ATTR *attrs = cg->findAttrs(concept);
	ATTR *attr;
	VAL *vals;
	_TCHAR val[1001];
	_TCHAR buff[1001];

	CString attrStr;
	CString valueStr;
	int count = 0;
	bool first;

	while (attrs) {
		attr = cg->popAttr(attrs);
		cg->attrName(attr,buff,1000);
		attrStr = buff;
		if (KBIsSystemAttribute(attrStr))
			continue;
		m_listCtrlAttributes.InsertItem(count,attrStr);

		vals = cg->attrVals(attr);
		first = true;

		while (vals) {
			val[0] = '\0';
			cg->popSval(vals,val);
			valueStr = val;
			if (!first)
				m_listCtrlAttributes.InsertItem(count,attrStr);
			m_listCtrlAttributes.SetItemText(count++,1,valueStr);
			first = false;
		}
	}
}

void CDictionaryDlg::OnBackLetter() 
{
	if (m_dictDisplayType == DICT_DISPLAY_LETTER_GROUP) {
		int itemPageCount = m_listCtrlDictionary.GetCountPerPage();
		SelectItemPage(0,itemPageCount);
		CButton *button = (CButton *)GetDlgItem(IDC_BACK_LETTER);
		button->SetFocus();
	}
}

void CDictionaryDlg::OnBack() 
{
	if (m_dictDisplayType == DICT_DISPLAY_LETTER_GROUP) {
		int pageTop = m_listCtrlDictionary.GetTopIndex();
		int itemPageCount = m_listCtrlDictionary.GetCountPerPage();
		SelectItemPage(pageTop-itemPageCount*2-1,pageTop-itemPageCount);
	}
}

void CDictionaryDlg::OnForward() 
{
	if (m_dictDisplayType == DICT_DISPLAY_LETTER_GROUP) {
		int pageTop = m_listCtrlDictionary.GetTopIndex();
		int itemPageCount = m_listCtrlDictionary.GetCountPerPage();
		SelectItemPage(pageTop+itemPageCount, pageTop+itemPageCount*2-1);
	}
}

void CDictionaryDlg::OnForwardLetter() 
{
	if (m_dictDisplayType == DICT_DISPLAY_LETTER_GROUP) {
		int itemPageCount = m_listCtrlDictionary.GetCountPerPage();
		int endItem = m_listCtrlDictionary.GetItemCount() - 1;
		SelectItemPage(endItem-itemPageCount+1,endItem);
		CButton *button = (CButton *)GetDlgItem(IDC_FORWARD_LETTER);
		button->SetFocus();
	}
}

void CDictionaryDlg::SelectItemPage(int startItem, int endItem) 
{
	m_listCtrlDictionary.SetFocus();
	m_listCtrlDictionary.EnsureVisible(startItem,false);
	m_listCtrlDictionary.EnsureVisible(endItem,false);
}

void CDictionaryDlg::OnAdd() 
{
	m_listCtrlAttributes.SetFocus();
	int count = m_listCtrlAttributes.GetItemCount();
	m_listCtrlAttributes.InsertItem(count,_T("unnamed"));
	m_listCtrlAttributes.EditLabel(count);
	kbDirty = true;
}

void CDictionaryDlg::OnDelete() 
{
	if (!m_conceptSelected)
		return;

	POSITION pos = m_listCtrlAttributes.GetFirstSelectedItemPosition();
	
	if (pos != NULL) {
		int nItem;
		CString attrStr,valStr,valueStr;
		_TCHAR attrBuff[1001];
		_TCHAR valBuff[1001];

		if (pos) {
			nItem = m_listCtrlAttributes.GetNextSelectedItem(pos);
			attrStr = m_listCtrlAttributes.GetItemText(nItem,0);
			valStr = m_listCtrlAttributes.GetItemText(nItem,1);
			CString msgStr;
			msgStr.Format(_T("Delete the attribute value pair: \"%s\"?"),attrStr);

			if (AfxMessageBox(msgStr,MB_YESNO) == IDYES) {
				StringToChar(attrBuff,attrStr,1000);
				StringToChar(valBuff,valStr,1000);
				cg->rmAttrval(m_conceptSelected,attrBuff,valBuff);
				kbDirty = true;

				m_listCtrlAttributes.DeleteItem(nItem);
			}
		}
	}
}

void CDictionaryDlg::OnRclickDictionaryList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UINT disables[100];
	for (int i=0; i<100; i++)
		disables[i] = 0;

	CPoint point;
	CRect rect;
	m_listCtrlDictionary.GetItemPosition(m_intDictSelection,&point);
	m_listCtrlDictionary.ClientToScreen(&point);

	CString itemStr = m_listCtrlDictionary.GetItemText(m_intDictSelection,0);
	point.x += m_listCtrlDictionary.GetStringWidth(itemStr) + 5;

	PopupContextMenu(this,IDR_DICTIONARY_WORDS,point,disables);	
	
	*pResult = 0;
}

void CDictionaryDlg::OnAttrComboChange() 
{
	CString comboText;
	m_comboBoxAttr->GetWindowText(comboText);
	if (comboText == _T(""))
		return;

	ATTR_DISPLAY_TYPE newType = AttrDisplayTypeFromString(comboText);

	if (newType != m_attrDisplayType) {
		m_attrDisplayType = newType;
		FillDictionaryList();
	}
}

ATTR_DISPLAY_TYPE CDictionaryDlg::AttrDisplayTypeFromString(CString attrTypeStr) 
{
	int type = 0;

	while (attrDisplayStrs[type] != _T("xxyyzz")) {
		if (attrDisplayStrs[type] == attrTypeStr)
			break;
		type++;
	}

	return (ATTR_DISPLAY_TYPE)type;
}

CString CDictionaryDlg::AttrDisplayStringFromType(ATTR_DISPLAY_TYPE type) 
{
	return attrDisplayStrs[type];
}

FIND_TYPE CDictionaryDlg::FindTypeFromString(CString findTypeStr) 
{
	int type = 0;

	while (findStrs[type] != _T("xxyyzz")) {
		if (findStrs[type] == findTypeStr)
			break;
		type++;
	}

	return (FIND_TYPE)type;
}

CString CDictionaryDlg::FindStringFromType(FIND_TYPE type) 
{
	return findStrs[type];
}

void CDictionaryDlg::AddWordAttributeValue(_TCHAR *attrChar, _TCHAR *nounChar, bool redisplay)
{
	cg->replaceVal(m_conceptSelected,attrChar,nounChar);
	if (redisplay)
		FillAttributeList();
}

void CDictionaryDlg::OnDictionraywordsMarkasNounCount() 
{
	AddWordAttributeValue(_T("pos"),_T("noun"));
	AddWordAttributeValue(_T("noun"),_T("count"),true);
}

void CDictionaryDlg::OnDictionraywordsMarkasNounNocount() 
{
	AddWordAttributeValue(_T("pos"),_T("noun"));
	AddWordAttributeValue(_T("noun"),_T("nocount"),true);
}

void CDictionaryDlg::OnDictionraywordsMarkasNounBoth() 
{
	AddWordAttributeValue(_T("pos"),_T("noun"));
	AddWordAttributeValue(_T("noun"),_T("countnocount"),true);
}

void CDictionaryDlg::OnDictionraywordsMarkasVerbTransitive() 
{
	AddWordAttributeValue(_T("pos"),_T("verb"));
	AddWordAttributeValue(_T("verb"),_T("transitive"),true);
}

void CDictionaryDlg::OnDictionraywordsMarkasVerbIntransitive() 
{
	AddWordAttributeValue(_T("pos"),_T("verb"));
	AddWordAttributeValue(_T("verb"),_T("intransitive"),true);
}

void CDictionaryDlg::OnDictionraywordsMarkasAdjective() 
{
	AddWordAttributeValue(_T("pos"),_T("adjective"),true);
}

void CDictionaryDlg::OnDictionraywordsMarkasAdverb() 
{
	AddWordAttributeValue(_T("pos"),_T("adverb"),true);
}

void CDictionaryDlg::OnMoveToList() 
{
	POSITION pos = m_listCtrlDictionary.GetFirstSelectedItemPosition();

	if (m_dictDisplayType != DICT_DISPLAY_LETTER_GROUP) {
		AfxMessageBox(_T("Can't move indexes to word list!"));
		return;
	}
	else if (pos == NULL) {
		AfxMessageBox(_T("No words selected!"));
		return;
	}

	int currentSelection = -1;
	CString wordStr;
	CONCEPT *concept;

	while ((currentSelection = m_listCtrlDictionary.GetNextSelectedItem(pos)) >= 0) {
		wordStr = m_listCtrlDictionary.GetItemText(currentSelection,0);
		concept = (CONCEPT *)m_listCtrlDictionary.GetItemData(currentSelection);
		AddWordToList(concept,wordStr);
	}
}

void CDictionaryDlg::OnClearList() 
{
	POSITION pos = m_listCtrlList.GetFirstSelectedItemPosition();

	if (pos == NULL) {
		AfxMessageBox(_T("No words selected!"));
		return;
	}

	CString msgStr;
	msgStr = _T("Do you want to delete the selected words from the list?");

	if (AfxMessageBox(msgStr,MB_YESNO) == IDYES) {
		int currentSelection = -1;
		while ((currentSelection = m_listCtrlList.GetNextSelectedItem(pos)) >= 0)
			m_listCtrlList.DeleteItem(currentSelection);
	}	
}

void CDictionaryDlg::OnClearAll() 
{
	POSITION pos = m_listCtrlList.GetFirstSelectedItemPosition();

	CString msgStr;
	msgStr = _T("Do you want to delete the selected words from the list?");

	if (AfxMessageBox(msgStr,MB_YESNO) == IDYES) {
		SetRedraw(false);
		m_listCtrlList.DeleteAllItems();
		SetRedraw(true);
	}	
}

void CDictionaryDlg::OnAttributeToggle() 
{
	UpdateData(true);

	if (m_boolAttributes) {
		SelectDictionaryList();
	}
	else {
		SetRedraw(false);
		m_listCtrlAttributes.DeleteAllItems();
		SetRedraw(true);
	}
}

void CDictionaryDlg::OnAddList() 
{
	UpdateData(true);
	_TCHAR buff[1001];
	StringToChar(buff,m_strWord,1000);

	CONCEPT *wordConcept = cg->getWordConcept(buff);

	AddWordToList(wordConcept,m_strWord);

	m_strWord = _T("");

	UpdateData(false);
}

void CDictionaryDlg::AddWordToList(CONCEPT *concept, CString wordStr)
{
	if (wordStr == _T(""))
		return;

	LV_FINDINFO findStruct;
	findStruct.flags = LVFI_STRING;
	findStruct.psz = m_strWord;

	int findResult = m_listCtrlList.FindItem(&findStruct,-1);

	if (findResult >= 0) {
		AfxMessageBox(_T("Already exists!"));
		return;
	}

	int newItem = InsertItemAlphabetically(wordStr);
	if (!concept)
		concept = AddToTempConcept(wordStr);

	m_listCtrlList.SetItemData(newItem,(DWORD)concept);
}

void CDictionaryDlg::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!m_boolAttributes)
		return;

	POSITION pos = m_listCtrlList.GetFirstSelectedItemPosition();

	if (pos) {
		int itemNum = m_listCtrlList.GetNextSelectedItem(pos);
		m_strWord = m_listCtrlList.GetItemText(itemNum,0);
		m_conceptSelected =
			(CONCEPT *)m_listCtrlList.GetItemData(itemNum);

		CONCEPT *dictCon = WordLookup(m_strWord);
		if (dictCon)
			CopyConceptAttributes(dictCon,m_conceptSelected);

		m_listCtrlAttributes.SetConcept(m_conceptSelected);

		FillAttributeList(m_conceptSelected);
	}
	else {
		m_intDictSelection = -1;
	}

	UpdateData(false);
	
	*pResult = 0;
}

void CDictionaryDlg::OnChangeWord() 
{
	UpdateData(true);

	if (m_findType != FIND_TYPE_FIND_OFF && m_strWord != _T("") && IsAlpha(m_strWord)) {

		_TCHAR wordBuff[1001];
		_TCHAR pathBuff[1001];
		StringToChar(wordBuff,m_strWord,1000);

		CONCEPT *con = cg->wordIndex(wordBuff);
		cg->conceptPath(con,pathBuff);

		CString pathStr = pathBuff;
		CStringList paths;
		StringToWordList(paths,pathStr);
		for (int i=0; paths.GetCount() && i<4; i++)
			paths.RemoveHead();

		CString lastStrLetter = m_strLetter;
		CString lastStrLetterGroup = m_strLetterGroup;

		m_strLetter = paths.RemoveHead();
		m_strLetter.Remove('\"');
		m_strLetterGroup = paths.RemoveHead();
		m_strLetterGroup.Remove('\"');

		if (m_strWord.GetLength() == 1)
			m_dictDisplayType = DICT_DISPLAY_LETTER;
		else
			m_dictDisplayType = DICT_DISPLAY_LETTER_GROUP;

		if (m_findType == FIND_TYPE_NARROW)
			FillDictionaryList();
		else if (m_findType == FIND_TYPE_FIND)
			Find(m_strWord);

		// SET FOCUS ON ANALYZER NAME
		CEdit *ed = (CEdit *)GetDlgItem(IDC_WORD);
		ed->SetFocus();
	}

	UpdateData(false);
}

bool CDictionaryDlg::Find(CString findStr) 
{
	CString wordStr;
	int nItem = 0;
	int result = 0;

	while (result >= 0) {
		result = m_listCtrlDictionary.GetNextItem(nItem,LVNI_ALL);

		wordStr = m_listCtrlDictionary.GetItemText(nItem,0);

		if (wordStr.Find(findStr) >= 0) {
			m_listCtrlDictionary.SetItemState(nItem,
				LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED);
			m_listCtrlDictionary.SetFocus();
			m_listCtrlDictionary.EnsureVisible(nItem,false);
			return true;
			break;
		}
		nItem++;
	}

	return false;
}

void CDictionaryDlg::OnSelchangeFindType() 
{
	CString comboText;
	m_comboBoxFind->GetWindowText(comboText);
	if (comboText == _T(""))
		return;
	FIND_TYPE newType = FindTypeFromString(comboText);

	if (newType != m_findType) {
		m_findType = newType;
		FillDictionaryList();
	}
}

void CDictionaryDlg::OnAddAll() 
{
	CString msgStr;
	msgStr = _T("Do you want to add the current attributes to all concepts in the list?");

	if (AfxMessageBox(msgStr,MB_YESNO) == IDYES) {
		CONCEPT *con = m_listCtrlAttributes.GetConcept();
		CONCEPT *tmp = cg->Down(m_conceptTemp);
		CString tmpStr;

		while (tmp) {
			tmpStr = ConceptName(tmp);
			if (con != tmp)
				CopyConceptAttributes(con,tmp);
			tmp = cg->Next(tmp);
		}
	}
}

void CDictionaryDlg::OnFile() 
{
	CFileDialog fileDlg(true);

	if (fileDlg.DoModal() == IDOK) {

		CString fileNameStr = fileDlg.GetPathName();
		if (!FileExists(fileNameStr)) {
			CString msgStr;
			msgStr.Format(_T("File \"%s\" doesn't exist."),fileNameStr);
			AfxMessageBox(msgStr);
			return;
		}

		StartWaitCursor();

		CStringList lines;
		ReadFileToStringList(lines,fileNameStr);
		CStringList lineWords,words;

		CString lineStr,wordStr;
		POSITION pos = lines.GetHeadPosition();
		POSITION p;

		while (pos && (lineStr = lines.GetNext(pos))) {
			StringToWordList(lineWords,lineStr);
			p = lineWords.GetHeadPosition();
			while (p && (wordStr = lineWords.GetNext(p))) {
				StripWord(wordStr);
				wordStr.MakeLower();
				if (wordStr != _T(""))
					words.AddTail(wordStr);
			}
			lineWords.RemoveAll();
		}

		SortStringList(words);
		RemoveSuccessiveEquals(words);

		pos = words.GetHeadPosition();
		_TCHAR buff[1001];
		int newItem;
		int image = 0;
		CONCEPT *wordConcept = NULL;

		while (pos && (wordStr = words.GetNext(pos))) {
			StringToChar(buff,wordStr,1000);
			wordConcept = cg->getWordConcept(buff);
			image = wordConcept ? 0 : 1;

			newItem = m_listCtrlList.InsertItem(0,wordStr,image);
			m_listCtrlList.SetItemData(newItem,(DWORD)wordConcept);
		}

		StopWaitCursor();
	}
}

void CDictionaryDlg::StripWord(CString &wordStr)
{
	CString newStr;
	int length = wordStr.GetLength();
	_TCHAR c;

	for (int i=0; i<length; i++) {
		c = wordStr.GetAt(i);
		if (alphabetic(c))	// 12/16/01 AM.
			newStr += c;
		else
			break;
	}

	wordStr = newStr;
}

void CDictionaryDlg::OnCustomDictDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;

	int iRow;
	CONCEPT *con;
	ATTR *attrs;
	_TCHAR buff[1001];
	CString itemStr;

	switch(lplvcd->nmcd.dwDrawStage) {
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;          // ask for item notifications.
	break;

	case CDDS_ITEMPREPAINT:
		*pResult = CDRF_DODEFAULT;

		iRow = lplvcd->nmcd.dwItemSpec;
		itemStr = m_listCtrlDictionary.GetItemText(iRow,0);
		StringToChar(buff,itemStr,1000);
		con = cg->getWordConcept(buff);
		attrs = cg->findAttrs(con);

		if (m_dictDisplayType == DICT_DISPLAY_LETTER_GROUP) {

			if (attrs) {
				lplvcd->clrTextBk = RGB(255,0,0);
				lplvcd->clrText = RGB(255,255,0);
				*pResult = CDRF_NEWFONT;
			}
		}

	break;

	default:
		*pResult = CDRF_DODEFAULT;
	}
}

void CDictionaryDlg::OnCustomListDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;

	int iRow;
	CONCEPT *dictCon;
	CONCEPT *itemCon;
	ATTR *attrs;
	CString itemStr;

	switch(lplvcd->nmcd.dwDrawStage) {
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;          // ask for item notifications.
	break;

	case CDDS_ITEMPREPAINT:
		*pResult = CDRF_DODEFAULT;

		iRow = lplvcd->nmcd.dwItemSpec;
		itemStr = m_listCtrlList.GetItemText(iRow,0);
		itemCon = (CONCEPT *)m_listCtrlList.GetItemData(iRow);
		dictCon = WordLookup(itemStr);
		attrs = cg->findAttrs(dictCon);

		if (!dictCon) {
			lplvcd->clrTextBk = RGB(255,255,255);
			lplvcd->clrText = RGB(0,0,0);
			*pResult = CDRF_NEWFONT;
		}

		else if (!attrs) {
			lplvcd->clrTextBk = RGB(255,0,0);
			lplvcd->clrText = RGB(255,255,255);
			*pResult = CDRF_NEWFONT;
		}

		else if (attrs) {
			lplvcd->clrTextBk = RGB(255,0,0);
			lplvcd->clrText = RGB(255,255,0);
			*pResult = CDRF_NEWFONT;
		}

		else if (itemCon) {
			attrs = cg->findAttrs(itemCon);
			if (attrs) {
				lplvcd->clrTextBk = RGB(0,0,0);
				lplvcd->clrText = RGB(255,0,0);
				*pResult = CDRF_NEWFONT;
			}
		}

	break;

	default:
		*pResult = CDRF_DODEFAULT;
	}
}


BOOL CDictionaryDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {

		CEdit *ed = (CEdit *)GetDlgItem(IDC_WORD);
		CRect rectEdit;
		ed->GetWindowRect(rectEdit);

		if (rectEdit.PtInRect(pMsg->pt)) {
			AddWordToList(NULL,m_strWord);
			m_strWord = _T("");
			UpdateData(false);
		}
	}
	else	
		return CDialog::PreTranslateMessage(pMsg);

	return true;
}

int CDictionaryDlg::InsertItemAlphabetically(CString wordStr)
{
	int count = m_listCtrlList.GetItemCount();

	// IF FIRST ONE
	if (!count) {
		m_listCtrlList.InsertItem(0,wordStr);
		return 0;
	}

	bool added = false;
	CString itemTextStr;
	int i = 0;

	for (i=0; i<count; i++) {

		itemTextStr = m_listCtrlList.GetItemText(i,0);

		if (wordStr.CompareNoCase(itemTextStr) < 0) {
			m_listCtrlList.InsertItem(i,wordStr);
			added = true;
			break;
		}
	}

	// IF LAST ONE
	if (!added && count) {
		m_listCtrlList.InsertItem(count,wordStr);
		return count;
	}

	return i;
}

CONCEPT *CDictionaryDlg::AddToTempConcept(CString wordStr)
{
	_TCHAR wordBuff[1001];
	StringToChar(wordBuff,wordStr,1000);

	CONCEPT *wordConcept = cg->makeConcept(m_conceptTemp,wordBuff);
	return wordConcept;
}

void CDictionaryDlg::DeleteTempConcepts()
{
	cg->rmChildren(m_conceptTemp);
}


void CDictionaryDlg::OnOK() 
{
	DeleteTempConcepts();
	
	CDialog::OnOK();
}

void CDictionaryDlg::OnToDict() 
{
	CString msgStr;
	msgStr = _T("Do you want to add the current concepts and their attributes to the KB?");

	if (AfxMessageBox(msgStr,MB_YESNO) == IDYES) {
		CONCEPT *con;
		CONCEPT *tmp = cg->Down(m_conceptTemp);
		CString tmpStr;
		CONCEPT *dictCon;
		_TCHAR wordBuff[1001];
		_TCHAR pathBuff[1001];

		while (tmp) {
			tmpStr = ConceptName(tmp);
			StringToChar(wordBuff,tmpStr,1000);
			dictCon = WordLookup(tmpStr);

			con = m_listCtrlAttributes.GetConcept();
			if (dictCon) {
				RemoveAllAttributes(dictCon);
			}
			else {
				cg->wordPath(wordBuff,pathBuff,1000);
				con = cg->getWordConcept(wordBuff);
			}
			CopyConceptAttributes(tmp,con);
			tmp = cg->Next(tmp);
		}
	}
}

void CDictionaryDlg::OnDeleteWord() 
{
	// TODO: Add your control notification handler code here
	
}
