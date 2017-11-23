// DictionaryTool.cpp : implementation file
//

#include "stdafx.h"
#include "VisualText.h"
#include "Utils.h"
#include "kb.h"
#include "MainFrm.h"
#include "AttributeEditor.h"
#include "EnglishInflectionsDlg.h"
#include "DictionaryTool.h"
#include "KnowledgeView.h"
#include "WordListDlg.h"
#include ".\dictionarytool.h"

extern CG *cg;
extern bool kbDirty;
extern CVisualTextApp theApp;

#define WMU_DICTIONARY_TOOL_ADD				(WM_USER + 1700)
#define WMU_DICTIONARY_TOOL_UPDATE_DISPLAY	(WM_USER + 1701)

class DictionaryToolWorkItem : public WorkItemBase
{
public:
	DictionaryToolWorkItem(HWND hWnd, CString wordStr, WORD_RESTRICTION_TYPE wordRestrictionType, bool fromExistingFlag) {
		m_hWnd = hWnd;
		m_strWordSearch = wordStr;
		m_wordRestrictionType = wordRestrictionType;
		m_boolFromExistingFlag = fromExistingFlag;
	}
private:
	void   DoWork(void* pThreadContext);
    void   Abort();
	bool   FillDictionaryListExisting();
	bool   FillDictionaryList(CONCEPT *startConcept, int level, int &count);
	HWND   m_hWnd;
	CString m_strWordSearch;
	WORD_RESTRICTION_TYPE m_wordRestrictionType;
	bool m_boolFromExistingFlag;
};

bool DictionaryToolWorkItem::FillDictionaryList(CONCEPT *startConcept, int level, int &count)
{
	if (!startConcept)
		return false;

	CString parentConceptStr = ConceptName(startConcept);
	CONCEPT *concept = startConcept;
	CString conceptStr;
	ATTR *attrs;
	int result = 0;

	while (concept) {
		conceptStr = ConceptName(concept);
		attrs = cg->findAttrs(concept);

		if (level == 3 && (m_strWordSearch.IsEmpty() ||
			(m_wordRestrictionType == WORD_RESTRICTION_CONTAINS && conceptStr.Find(m_strWordSearch) >= 0) ||
			(m_wordRestrictionType == WORD_RESTRICTION_STARTS_WITH && conceptStr.Find(m_strWordSearch) == 0) ||
			(m_wordRestrictionType == WORD_RESTRICTION_ENDS_WITH && conceptStr.Mid(conceptStr.GetLength()-m_strWordSearch.GetLength()) == m_strWordSearch) ||
			(m_wordRestrictionType == WORD_RESTRICTION_EQUALS && conceptStr == m_strWordSearch)
			)) {
			result = ::SendMessage(m_hWnd,WMU_DICTIONARY_TOOL_ADD,(WPARAM)concept,(LPARAM)count);
			if (result == 1)
				break;
			count++;
		}

		if (cg->Down(concept)) {
			if (FillDictionaryList(cg->Down(concept),level+1,count))
				break;
		}

		concept = cg->Next(concept);
		PeekHere(1);
	}

	if (level == 0)
		::SendMessage(m_hWnd,WMU_DICTIONARY_TOOL_UPDATE_DISPLAY,(WPARAM)count,NULL);

	return false;
}

void DictionaryToolWorkItem::DoWork(void* pThreadContext)
{
	if (!m_boolFromExistingFlag) {
		CONCEPT *sys = cg->findConcept(cg->findRoot(),_T("sys"));
		CONCEPT *dict = cg->findConcept(sys,_T("dict"));
		CONCEPT *a1 = cg->findConcept(dict,_T("a"));

		int count = 0;
		FillDictionaryList(a1,0,count);
	}

	delete this;
}

void DictionaryToolWorkItem::Abort()
{
	delete this;
}


unsigned long __stdcall CDictionaryTool::ThreadFuncDestroy( void*  pParam )
{
	CDictionaryTool* dlg = (CDictionaryTool*)pParam;
	dlg->m_WorkQueue.Destroy();
	return 1;
}

// CDictionaryTool dialog

IMPLEMENT_DYNAMIC(CDictionaryTool, CXTResizeDialog)
CDictionaryTool::CDictionaryTool(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDictionaryTool::IDD, pParent)
	, m_strCount(_T(""))
	, m_strWordSearch(_T(""))
	, m_strWordRestriction(_T(""))
	, m_strWord(_T(""))
	, m_strAttr(_T(""))
	, m_strValue(_T(""))
	, m_strPOS(_T(""))
	, m_strPOSMatchSpec(_T(""))
	, m_strAttrWord(_T(""))
	, m_strAttrMatch(_T(""))
	, m_boolAppend(FALSE)
	, m_boolBrowse(FALSE)
	, m_boolMultiSelect(FALSE)
{
}

CDictionaryTool::~CDictionaryTool()
{
}

void CDictionaryTool::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WORDS, m_listCtrlWords);
	DDX_Text(pDX, IDC_MSG, m_strCount);
	DDX_Text(pDX, IDC_WORD_SEARCH, m_strWordSearch);
	DDX_Control(pDX, IDC_POS, m_listBoxPOS);
	DDX_Control(pDX, IDC_INFLECTIONS, m_listCtrlInflections);
	DDX_Control(pDX, IDC_WORD_RESTRICTION, m_comboBoxWordRestriction);
	DDX_CBString(pDX, IDC_WORD_RESTRICTION, m_strWordRestriction);
	DDX_Text(pDX, IDC_WORD, m_strWord);
	DDX_Text(pDX, IDC_ATTRIBUTE, m_strAttr);
	DDX_Text(pDX, IDC_VALUE, m_strValue);
	DDX_Control(pDX, IDC_COMBO_POS, m_comboBoxPOS);
	DDX_CBString(pDX, IDC_COMBO_POS, m_strPOS);
	DDX_Control(pDX, IDC_POS_MATCH_SPECS, m_comboBoxPOSMatchSpecs);
	DDX_CBString(pDX, IDC_POS_MATCH_SPECS, m_strPOSMatchSpec);
	DDX_Text(pDX, IDC_ATTR_WORD, m_strAttrWord);
	DDX_CBString(pDX, IDC_ATTR_MATCH, m_strAttrMatch);
	DDX_Control(pDX, IDC_ATTR_MATCH, m_comboBoxAttrMatch);
	DDX_Check(pDX, IDC_APPEND_CHECKBOX, m_boolAppend);
	DDX_Check(pDX, IDC_BROWSE_CHECKBOX, m_boolBrowse);
	DDX_Check(pDX, IDC_MULTI_SELECT, m_boolMultiSelect);
}


BEGIN_MESSAGE_MAP(CDictionaryTool, CXTResizeDialog)
	ON_BN_CLICKED(IDC_ATTRIBUTES, OnBnClickedAttributes)
	ON_BN_CLICKED(IDC_SEARCH, OnBnClickedSearch)
	ON_WM_DESTROY()
	ON_MESSAGE(WMU_DICTIONARY_TOOL_ADD, OnAddWord)
	ON_MESSAGE(WMU_DICTIONARY_TOOL_UPDATE_DISPLAY, OnUpdateCountDisplay)
	ON_WM_CHAR()
	ON_NOTIFY(NM_DBLCLK, IDC_WORDS, OnNMDblclkWords)
	ON_NOTIFY(NM_CLICK, IDC_WORDS, OnNMClickWords)
	ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
	ON_CBN_SELCHANGE(IDC_WORD_RESTRICTION, OnCbnSelchangeWordRestriction)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_DBLCLK, IDC_INFLECTIONS, OnNMDblclkInflections)
	ON_BN_CLICKED(IDC_ENGLISH, OnBnClickedEnglish)
	ON_NOTIFY(NM_CLICK, IDC_INFLECTIONS, OnNMClickInflections)
	ON_BN_CLICKED(IDC_CLEAR, OnBnClickedClear)
	ON_BN_CLICKED(IDC_ADD_POS, OnBnClickedAddPos)
	ON_BN_CLICKED(IDC_DELETE_POS, OnBnClickedDeletePos)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDC_CHANGE_INFLECTION, OnBnClickedChangeInflection)
	ON_BN_CLICKED(IDC_CLEAR_WORDS, OnBnClickedClearWords)
	ON_CBN_SELCHANGE(IDC_ATTR_MATCH, OnCbnSelchangeAttrMatch)
	ON_BN_CLICKED(IDC_ADD_INFLECTION, OnBnClickedAddInflection)
	ON_BN_CLICKED(IDC_ADD_CONCEPT, OnBnClickedAddConcept)
	ON_BN_CLICKED(IDC_DELETE_WORD, OnBnClickedDeleteWord)
	ON_BN_CLICKED(IDC_RENAME_POS, OnBnClickedRenamePos)
	ON_BN_CLICKED(IDC_ADD_ATTR_ALL, OnBnClickedAddAttrAll)
	ON_BN_CLICKED(IDC_ADD_LIST, OnBnClickedAddList)
	ON_BN_CLICKED(IDC_REPLACE, OnBnClickedReplace)
	ON_BN_CLICKED(IDC_DELETE_INFLECTION, OnBnClickedDeleteInflection)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_WORDS, OnNMCustomdrawWords)
	ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_OPEN_KB, OnBnClickedKb)
	ON_BN_CLICKED(IDC_REMOVE, OnBnClickedRemove)
	ON_BN_CLICKED(IDC_MULTI_SELECT, OnBnClickedMultiSelect)
END_MESSAGE_MAP()


// CDictionaryTool message handlers

LRESULT CDictionaryTool::OnAddWord(WPARAM wParam, LPARAM lParam)
{
	CONCEPT *concept = (CONCEPT *)wParam;
	m_intScanCount = (int)lParam;
	AddWord(ConceptName(concept),concept);
	PeekHere(1);
	
	SetCountDisplay();

	return m_boolStopThreads ? 1L : 0L;
}

CString CDictionaryTool::WordDisplay(CONCEPT *concept,	CStringList &POSStringList,
									 CStringList &attrStringList, CStringList &valueStringList)
{
	if (!concept)
		return _T("");

	CString wordStr = ConceptName(concept);
	ATTR *attrs = cg->findAttrs(concept);
	ATTR *attr;
	VAL *vals;
	_TCHAR val[1001];
	_TCHAR buff[1001];
	CString valueStr;
	CString wordPostStr;
	CString attrStr;
	wordPostStr.Empty();
	int count = 0;
	bool isConcept = false;
	CONCEPT *valConcept = NULL;

	bool posMatch = true;
	bool attrMatch = true;

	while (attrs) {
		attr = cg->popAttr(attrs);
		cg->attrName(attr,buff,1000);
		attrStr = buff;
		if (KBIsSystemAttribute(attrStr))
			continue;

		vals = cg->attrVals(attr);

		while (vals) {
			isConcept = cg->isValConcept(vals);
			if (isConcept) {
				cg->popVal(vals,valConcept);
				valueStr = ConceptPathStr(valConcept,true);
			}
			else {
				val[0] = '\0';
				cg->popSval(vals,val);
				valueStr = val;
			}
			attrStringList.AddTail(attrStr);
			valueStringList.AddTail(valueStr);

			if (attrStr == _T("root"))
				wordPostStr += " +";
			else if (attrStr == _T("pos")) {
				POSStringList.AddTail(valueStr);
				wordPostStr += " ";
				if (valueStr.GetAt(0) == 'a')
					wordPostStr += valueStr.Mid(0,3);
				else
					wordPostStr += valueStr.Mid(0,1);
			}
		}
	}

	return wordStr + wordPostStr;
}

void CDictionaryTool::AddWord(CString wordStr, CONCEPT *concept, bool forceAdd)
{
	CStringList POSStringList;
	CStringList attrStringList;
	CStringList valueStringList;

	CString wordPostStr = wordStr;
	if (concept)
		wordPostStr = WordDisplay(concept,POSStringList,attrStringList,valueStringList);

	bool attrMatch = AttrMatch(wordStr,attrStringList,valueStringList);
	bool posMatch = POSMatch(POSStringList);

	LVFINDINFO info;
	info.flags = LVFI_STRING;
	info.psz = wordStr;
	int nIndex = m_listCtrlWords.FindItem(&info);

	if (forceAdd || (posMatch && attrMatch && nIndex < 0)) {
		m_listCtrlWords.InsertItem(m_intCount,wordPostStr);
		CONCEPT *concept = FindWordConcept(wordStr);
		if (!concept)
			m_intNewWords.AddTail(m_intCount);
		m_listCtrlWords.SetItemData(m_intCount,(DWORD)concept);
		m_intCount++;
	}
}

bool CDictionaryTool::AttrMatch(CString wordStr, CStringList &attrStringList, CStringList &valueStringList)
{
	bool matched = true;

	if (m_strAttrMatch == _T(">2 roots")) {
		matched = false;

		POSITION findPos = NULL;
		POSITION dupPos = NULL;

		findPos = attrStringList.Find(_T("root"));
		dupPos = attrStringList.Find(_T("root"),findPos);

		if (findPos && dupPos)
			matched = true;
	}
	else if (m_strAttrMatch == _T("OR")) {
		int count = m_listCtrlInflections.GetItemCount();
		POSITION findPos = NULL;
		POSITION valPos = NULL;
		CString attrStr,valStr,listValStr;
		bool found = false;
		matched = false;

		for (int i=0; i<count; i++) {
			findPos = attrStringList.GetHeadPosition();
			valPos = valueStringList.GetHeadPosition();
			found = false;

			while (findPos && (attrStr = attrStringList.GetNext(findPos))) {
				valStr = valueStringList.GetNext(valPos);
				if (attrStr == m_listCtrlInflections.GetItemText(i,1)) {
					found = true;
					break;
				}
			}

			listValStr = m_listCtrlInflections.GetItemText(i,2);

			if ((found && listValStr.IsEmpty()) || (found && valStr == listValStr)) {
				matched = true;
				break;
			}
		}
	}
	else if (m_strAttrMatch == _T("match") || m_strAttrMatch == _T("AND")) {
		int count = m_listCtrlInflections.GetItemCount();
		POSITION findPos = NULL;
		POSITION valPos = NULL;
		CString attrStr,valStr,listValStr;
		bool found = false;

		for (int i=0; i<count; i++) {
			findPos = attrStringList.GetHeadPosition();
			valPos = valueStringList.GetHeadPosition();
			found = false;

			while (findPos && (attrStr = attrStringList.GetNext(findPos))) {
				valStr = valueStringList.GetNext(valPos);
				if (attrStr == m_listCtrlInflections.GetItemText(i,1)) {
					found = true;
					break;
				}
			}

			listValStr = m_listCtrlInflections.GetItemText(i,2);

			if (!found || (!listValStr.IsEmpty() && valStr != listValStr)) {
				matched = false;
				break;
			}
		}
	}
	else if (m_strAttrMatch == "accumulate") {
		POSITION pos = attrStringList.GetHeadPosition();
		POSITION posVal = valueStringList.GetHeadPosition();
		CString attrStr,valStr;

		while (pos && (attrStr = attrStringList.GetNext(pos))) {
			valStr = valueStringList.GetNext(posVal);
			AddInflection(wordStr,attrStr,valStr);
		}
	}

	return matched;
}

bool CDictionaryTool::POSMatch(CStringList &posStringList)
{
	bool matched = true;
	POSITION pos = posStringList.GetHeadPosition();
	CString posStr;

	if (m_strPOSMatchSpec == _T("duplicates")) {
		POSITION findPos = NULL;
		POSITION dupPos = NULL;
		matched = false;
		while (pos && (posStr = posStringList.GetNext(pos))) {
			findPos = posStringList.Find(posStr);
			dupPos = posStringList.Find(posStr,findPos);
			if (findPos && dupPos) {
				matched = true;
				break;
			}
		}
	}
	else if (m_strPOSMatchSpec == _T("OR")) {
		matched = false;
		while (pos && (posStr = posStringList.GetNext(pos))) {
			if (m_listBoxPOS.FindStringExact(0,posStr) >= 0) {
				matched = true;
				break;
			}
		}
	}
	else if (m_strPOSMatchSpec == _T("match") || m_strPOSMatchSpec == _T("AND")) {
		while (pos && (posStr = posStringList.GetNext(pos))) {
			if (m_listBoxPOS.FindStringExact(0,posStr) < 0) {
				matched = false;
				break;
			}
		}
	}
	else if (m_strPOSMatchSpec == "accumulate") {
		while (pos && (posStr = posStringList.GetNext(pos)))
			AddPOS(posStr);
	}

	return matched;
}

LRESULT CDictionaryTool::OnUpdateCountDisplay(WPARAM wParam, LPARAM lParam)
{
	m_intScanCount = (int)wParam;
	SetCountDisplay();
	m_intAppendCount = 0;
	return 0;
}

void CDictionaryTool::SetCountDisplay()
{
	if (m_intCount < m_intScanCount)
		m_strCount.Format(_T("%d of %d"),m_intCount,m_intScanCount);
	else if (m_intAppendCount)
		m_strCount.Format(_T("%d + %d = %d"),m_intCount,m_intAppendCount,m_intCount+m_intAppendCount);
	else
		m_strCount.Format(_T("Total count: %d"),m_intCount);
	SetTextEditText(IDC_MSG,m_strCount);
}

void CDictionaryTool::SetTextEditText(UINT id, CString textStr)
{
	CWnd *wordText = GetDlgItem(id);
	wordText->SetWindowText(textStr);
}

void CDictionaryTool::OnBnClickedAttributes()
{
	// TODO: Add your control notification handler code here
}

void CDictionaryTool::OnBnClickedSearch()
{
	FillDictionaryList(false);
}


void CDictionaryTool::FillDictionaryList(BOOL bDoSetRedraw)
{
	theApp.BeginWaitCursor();

	UpdateData(true);
	m_boolStopThreads = true;
	m_intAppendCount = 0;
	if (m_boolAppend)
		m_intAppendCount = m_intCount;
	m_intCount = 0;
	m_intScanCount = 0;
	SetCountDisplay();
	PeekHere(20);

	if (!m_boolAppend) {
		m_intNewWords.RemoveAll();
		m_listCtrlWords.DeleteAllItems();
	}

	m_WorkQueue.InsertWorkItem(new DictionaryToolWorkItem(GetSafeHwnd(),m_strWordSearch,m_wordRestrictionType,false));
	m_boolThreadFilling = true;
	m_boolStopThreads = false;

	theApp.EndWaitCursor();
}

BOOL CDictionaryTool::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_WORDS, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_INFLECTIONS, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_VALUE, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_ATTRIBUTE, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_MSG, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STOP, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_SEARCH, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_WORD_SEARCH, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_WORD_RESTRICTION, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_ENGLISH, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_WORD, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_CLEAR, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_ADD_POS, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_DELETE_POS, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_COMBO_POS, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_POS, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDOK, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_ADD_INFLECTION, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_CHANGE_INFLECTION, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_DELETE_INFLECTION, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_DICT_GROUP, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_POS_GROUP, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_INFL_GROUP, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_ATTR_WORD, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_WORD_GROUP, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_POS_MATCH_SPECS, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_CLEAR_WORDS, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_ATTR_MATCH, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_ATTR_MATCH_TITLE, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_POS_MATCH_TITLE, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_DELETE_WORD, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_APPEND_CHECKBOX, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_MATCH_WORD, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_WORKAREA_BOX, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_BROWSE_CHECKBOX, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_ADD_CONCEPT, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_ADD_LIST, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_RENAME_POS, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_ADD_ATTR_ALL, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_REPLACE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_OPEN_KB, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_REMOVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_MULTI_SELECT, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_REFRESH, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);

	m_WorkQueue.Create(10);

	m_intDictSelection = -1;
	m_boolOrphans = false;
	m_boolUpArrow = false;
	m_boolThreadFilling = false;
	m_boolStopThreads = false;
	m_boolOpeningKB = false;
	m_conceptSelected = NULL;
	m_intWordSelected = 0;
	m_intAttrSelected = 0;

	int wordColumnWidth = 60;
	int attrColumnWidth = 75;
	CRect rect;

	//m_buttonKB.AutoLoad(IDC_OPEN_KB, this);

	m_listCtrlInflections.SetEditable(TRUE);

	m_listCtrlInflections.GetWindowRect(&rect);
	int valueWidth = rect.Width()-attrColumnWidth-wordColumnWidth-5;
	m_listCtrlInflections.InsertColumn(0,_T("Word"),LVCFMT_LEFT,wordColumnWidth);
	m_listCtrlInflections.InsertColumn(1,_T("Attr"),LVCFMT_LEFT,attrColumnWidth);
	m_listCtrlInflections.InsertColumn(2,_T("Value"),LVCFMT_LEFT,valueWidth);
	m_listCtrlInflections.SetExtendedStyle(LVS_EX_TWOCLICKACTIVATE | LVS_EX_FULLROWSELECT);	

	m_comboBoxPOSMatchSpecs.AddString(_T("none"));
	m_comboBoxPOSMatchSpecs.AddString(_T("accumulate"));
	m_comboBoxPOSMatchSpecs.AddString(_T("AND"));
	m_comboBoxPOSMatchSpecs.AddString(_T("OR"));
	m_comboBoxPOSMatchSpecs.AddString(_T("match"));
	m_comboBoxPOSMatchSpecs.AddString(_T("duplicates"));
	m_strPOSMatchSpec = _T("none");

	m_comboBoxAttrMatch.AddString(_T("none"));
	m_comboBoxAttrMatch.AddString(_T("match"));
	m_comboBoxAttrMatch.AddString(_T("AND"));
	m_comboBoxAttrMatch.AddString(_T("OR"));
	m_comboBoxAttrMatch.AddString(_T(">2 roots"));
	m_comboBoxAttrMatch.AddString(_T("accumulate"));
	m_strAttrMatch = _T("none");

	m_comboBoxWordRestriction.AddString(_T("contains"));
	m_comboBoxWordRestriction.AddString(_T("starts with"));
	m_comboBoxWordRestriction.AddString(_T("ends with"));
	m_comboBoxWordRestriction.AddString(_T("equals"));
	m_strWordRestriction = _T("equals");
	m_wordRestrictionType = WORD_RESTRICTION_EQUALS;

	CONCEPT *root = cg->findRoot();
	CONCEPT *sys = cg->findConcept(root,_T("sys"));
	CONCEPT *dict = cg->findConcept(sys,_T("dict"));
	CONCEPT *m_conA = cg->findConcept(sys,_T("a"));
	CONCEPT *nlp = cg->findConcept(sys,_T("nlp"));
	m_conPOS = cg->findConcept(nlp,_T("pos"));

	m_boolAppend = false;
	m_boolBrowse = true;

	FillPOS();
	
	//FillDictionaryList(false);

	// SET FOCUS ON ANALYZER NAME
	CEdit *ed = (CEdit *)GetDlgItem(IDC_WORD_SEARCH);
	ed->SetFocus();

	UpdateData(false);

	return FALSE;
}

void CDictionaryTool::FillPOS()
{
	CONCEPT *POS = cg->Down(m_conPOS);

	m_comboBoxPOS.ResetContent();
	while (POS) {
		m_comboBoxPOS.AddString(ConceptName(POS));
		POS = cg->Next(POS);
	}
}

void CDictionaryTool::OnDestroy()
{
	theApp.BeginWaitCursor();

	m_boolStopThreads = true;
	PeekHere(20);

	DWORD dwThreadId;
	HANDLE hThreade = CreateThread(NULL,
		0,
		CDictionaryTool::ThreadFuncDestroy,
		this,
		0,
		&dwThreadId);

	while (TRUE)
	{
		// block-local variable 
		DWORD result ; 
		MSG msg ; 
		
		// Read all of the messages in this next loop, 
		// removing each message as we read it.
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{ 
			// If it's a quit message, we're out of here.
			if (msg.message == WM_QUIT)  
				return; 
			// Otherwise, dispatch the message.
			DispatchMessage(&msg); 
		} // End of PeekMessage while loop.
		
		// Wait for any message sent or posted to this queue 
		// or for one of the passed handles be set to signaled.
		result = MsgWaitForMultipleObjects(1, &hThreade , 
			FALSE, INFINITE, QS_ALLINPUT); 
		
		// The result tells us the type of event we have.
		if (result == WAIT_OBJECT_0)
		{
			// New messages have arrived. 
			// Continue to the top of the always while loop to 
			// dispatch them and resume waiting.
			break;
		} 
		else 
		{ 
			continue;
				
		} // End of else clause.
	} // End of the always while loop.

	theApp.EndWaitCursor();

}

BOOL CDictionaryTool::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		CWnd *focusWnd = GetFocus();
		CWnd *wordWnd = GetDlgItem(IDC_WORD);
		if (focusWnd = wordWnd) {
			OnBnClickedSearch();
			if (m_boolAppend) {
			}
			return true;
		}
	}

	return CXTResizeDialog::PreTranslateMessage(pMsg);
}

void CDictionaryTool::SetEnable(UINT id, bool value)
{
	CWnd *itemWnd = GetDlgItem(id);
	itemWnd->EnableWindow(value);
}

void CDictionaryTool::OnNMDblclkWords(NMHDR *pNMHDR, LRESULT *pResult)
{
	CAttributeEditor::Open(m_conceptSelected);
	*pResult = 0;
}

void CDictionaryTool::OnNMClickWords(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int itemNum = pNMLV->iItem;
	if (itemNum >= 0)
		SelectItem(itemNum);
	*pResult = 0;
}

void CDictionaryTool::SelectItem(int itemNum, bool runFlag)
{
	if (m_boolMultiSelect == false)
		DeselectListCtrlItems(&m_listCtrlWords,itemNum);

	UpdateData(true);

	if (!m_boolBrowse && !runFlag)
		return;

	m_intWordSelected = itemNum;
	m_conceptSelected = (CONCEPT *)m_listCtrlWords.GetItemData(itemNum);
	if (m_conceptSelected)
		m_strWord = ConceptName(m_conceptSelected);
	else
		m_strWord = m_listCtrlWords.GetItemText(itemNum,0);

	ATTR *attrs = cg->findAttrs(m_conceptSelected);
	ATTR *attr;
	VAL *vals;
	_TCHAR val[1001];
	_TCHAR buff[1001];
	CString attrStr;
	CString valueStr;
	int count = 0;
	bool isConcept = false;
	CONCEPT *valConcept = NULL;

	if (m_strAttrMatch != _T("accumulate")) {
		m_listCtrlInflections.DeleteAllItems();
	}
	if (m_strPOSMatchSpec != _T("accumulate")) {
		m_listBoxPOS.ResetContent();
	}

	while (attrs) {
		attr = cg->popAttr(attrs);
		cg->attrName(attr,buff,1000);
		attrStr = buff;
		if (KBIsSystemAttribute(attrStr))
			continue;

		vals = cg->attrVals(attr);

		while (vals) {
			isConcept = cg->isValConcept(vals);
			if (isConcept) {
				cg->popVal(vals,valConcept);
				valueStr = ConceptPathStr(valConcept,true);
			}
			else {
				val[0] = '\0';
				cg->popSval(vals,val);
				valueStr = val;
			}

			if (attrStr == _T("pos")) {
				if (m_listBoxPOS.FindStringExact(0,valueStr) < 0)
					m_listBoxPOS.AddString(valueStr);
			}
			else
				AddInflection(m_strWord,attrStr,valueStr);
		}
	}

	UpdateData(false);
}

void CDictionaryTool::OnBnClickedStop()
{
	m_boolStopThreads = true;
}

void CDictionaryTool::OnCbnSelchangeWordRestriction()
{
	int nIndex = m_comboBoxWordRestriction.GetCurSel();
	CString wrStr;
	m_comboBoxWordRestriction.GetLBText(nIndex,wrStr);

	if (wrStr == _T("contains"))
		m_wordRestrictionType = WORD_RESTRICTION_CONTAINS;
	else if (wrStr == _T("starts with"))
		m_wordRestrictionType = WORD_RESTRICTION_STARTS_WITH;
	else if (wrStr == _T("ends with"))
		m_wordRestrictionType = WORD_RESTRICTION_ENDS_WITH;
	else if (wrStr == _T("equals"))
		m_wordRestrictionType = WORD_RESTRICTION_EQUALS;
}

void CDictionaryTool::OnNMDblclkInflections(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nLine = pNMListView->iItem;

	if (nLine >= 0) {
		CString conceptPathStr;
		conceptPathStr = m_listCtrlInflections.GetItemText(nLine,1);
		GotoConceptPath(conceptPathStr);
	}
	*pResult = 0;
}

void CDictionaryTool::OnBnClickedEnglish()
{
	UpdateData(true);

	m_listCtrlInflections.DeleteAllItems();

	CEnglishInflectionsDlg dlg;
	dlg.m_strWord = m_strWord;
	dlg.AddPOS(_T("noun"));
	dlg.AddPOS(_T("verb"));

	if (dlg.DoModal() == IDOK) {

		if (dlg.HasPOS(_T("noun"))) {
			AddInflection(m_strWord,_T("pos"),_T("noun"));
			AddInflection(m_strWord,_T("plural"),dlg.m_strPlural,true);

			AddInflection(dlg.m_strPlural,_T("pos"),_T("noun"));
			AddInflection(dlg.m_strPlural,_T("root"),m_strWord,true);
			AddInflection(dlg.m_strPlural,_T("noun"),_T("plural"),true);
		}

		if (dlg.HasPOS(_T("verb"))) {
			AddInflection(m_strWord,_T("pos"),_T("verb"));
			AddInflection(m_strWord,_T("3rd person"),dlg.m_strPlural,true);
			AddInflection(m_strWord,_T("past"),dlg.m_strPast,true);
			AddInflection(m_strWord,_T("gerund"),dlg.m_strGerund,true);

			AddInflection(dlg.m_strPlural,_T("pos"),_T("verb"));
			AddInflection(dlg.m_strPlural,_T("root"),m_strWord,true);
			AddInflection(dlg.m_strPlural,_T("tense"),_T("3rd person"));

			AddInflection(dlg.m_strPast,_T("pos"),_T("verb"));
			AddInflection(dlg.m_strPast,_T("root"),m_strWord,true);
			AddInflection(dlg.m_strPast,_T("tense"),_T("past"));

			AddInflection(dlg.m_strGerund,_T("pos"),_T("verb"));
			AddInflection(dlg.m_strGerund,_T("root"),m_strWord,true);
			AddInflection(dlg.m_strGerund,_T("tense"),_T("gerund"));
		}

		POSITION pos = dlg.m_strListPOS.GetHeadPosition();
		CString posStr;
		while (pos && (posStr = dlg.m_strListPOS.GetNext(pos))) {
			if (posStr != _T("noun") && posStr != _T("verb"))
				AddInflection(m_strWord,_T("pos"),posStr);
		}
	}
}

void CDictionaryTool::AddPOS(CString posStr)
{
	int index = m_listBoxPOS.FindStringExact(0,posStr);
	if (index < 0)
		m_listBoxPOS.AddString(posStr);
}

void CDictionaryTool::AddInflection(CString wordStr, CString attrStr, CString valueStr, bool isConcept)
{
	int count = m_listCtrlInflections.GetItemCount();
	bool found = false;
	CString wStr,aStr,vStr;

	if (isConcept)
		valueStr = NonExistentWordPathStr(valueStr);

	for (int row=0; row<count; row++) {
		wStr = m_listCtrlInflections.GetItemText(row,0);
		aStr = m_listCtrlInflections.GetItemText(row,1);
		vStr = m_listCtrlInflections.GetItemText(row,2);
		if (wStr == wordStr && aStr == attrStr && vStr == valueStr) {
			found = true;
			break;
		}
	}

	if (!found) {
		m_listCtrlInflections.InsertItem(count,wordStr);
		m_listCtrlInflections.SetItemText(count,1,attrStr);
		m_listCtrlInflections.SetItemText(count,2,valueStr);
	}
}

void CDictionaryTool::GotoConceptPath(CString conceptPathStr)
{
	CONCEPT *concept = ConceptFromPath(conceptPathStr,true);
	GotoWordConcept(concept);
}

void CDictionaryTool::GotoWordConcept(CONCEPT *concept)
{
	CString wordStr = ConceptName(concept);
	LVFINDINFO info;
	int nIndex;
	info.flags = LVFI_STRING | LVFI_PARTIAL;
	info.psz = wordStr;

	nIndex = m_listCtrlWords.FindItem(&info);

	if (nIndex < 0) {
		m_listCtrlWords.InsertItem(m_intCount,wordStr);
		CONCEPT *concept = FindWordConcept(wordStr);
		if (!concept)
			m_intNewWords.AddTail(m_intCount);
		m_listCtrlWords.SetItemData(m_intCount,(DWORD)concept);
		m_intCount++;
	}
	m_listCtrlWords.SetItemState(nIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	m_listCtrlWords.EnsureVisible(nIndex,false);
}

void CDictionaryTool::OnBnClickedGotoWord()
{
	UpdateData(true);
	CONCEPT *concept = WordLookup(m_strWord);
	GotoWordConcept(concept);
}

void CDictionaryTool::OnNMClickInflections(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(true);

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_intAttrSelected = pNMListView->iItem;

	if (m_intAttrSelected >= 0) {
		m_strAttrWord = m_listCtrlInflections.GetItemText(m_intAttrSelected,0);
		m_strAttr = m_listCtrlInflections.GetItemText(m_intAttrSelected,1);
		m_conceptValue = NULL;
		m_boolIsConcept = false;
		m_strValue = GetValueString(m_listCtrlInflections.GetItemText(m_intAttrSelected,2));
		UpdateData(false);
	}

	*pResult = 0;
}

void CDictionaryTool::OnBnClickedClear()
{
	UpdateData(true);
	m_listCtrlInflections.DeleteAllItems();
	m_listBoxPOS.ResetContent();
	m_strAttrWord.Empty();
	m_strAttr.Empty();
	m_strValue.Empty();
	UpdateData(false);
}

void CDictionaryTool::OnBnClickedAddPos()
{
	UpdateData(true);

	if (m_listBoxPOS.FindStringExact(0,m_strPOS) < 0) {
		m_listBoxPOS.AddString(m_strPOS);
	}
}

void CDictionaryTool::OnBnClickedDeletePos()
{
	CArray<int,int> aryListBoxSel;
	int selCount = m_listBoxPOS.GetSelCount();
	aryListBoxSel.SetSize(selCount);
	m_listBoxPOS.GetSelItems(selCount, aryListBoxSel.GetData()); 

	for (int i=selCount-1; i>=0; i--)
		m_listBoxPOS.DeleteString(aryListBoxSel.GetAt(i));
}

CString CDictionaryTool::GetValueString(CString valueStr)
{
	if (valueStr.Find(_T(":"),0) > 0) {
		valueStr = valueStr.Mid(valueStr.ReverseFind(':')+1);
		m_conceptValue = WordLookup(valueStr);
		m_boolIsConcept = m_conceptValue ? true : false;
	}
	return valueStr;
}

void CDictionaryTool::OnBnClickedChangeInflection()
{
	UpdateData(true);

	if (m_strAttr.IsEmpty() && m_strValue.IsEmpty() && m_conceptValue == NULL) {
		AfxMessageBox(_T("Nothing to change"));
		return;
	}
	if (m_boolIsConcept)
		m_strValue = WordPathStr(m_strValue);

	int nItem = -1;
	POSITION pos = m_listCtrlInflections.GetFirstSelectedItemPosition();

	if (pos && (nItem = m_listCtrlInflections.GetNextSelectedItem(pos)) >= 0) {
		m_listCtrlInflections.SetItemText(nItem,0,m_strAttrWord);
		m_listCtrlInflections.SetItemText(nItem,1,m_strAttr);
		m_listCtrlInflections.SetItemText(nItem,2,m_strValue);
	}

	UpdateData(false);
}

void CDictionaryTool::OnBnClickedSave()
{
	SaveAttributes();
}

void CDictionaryTool::OnBnClickedReplace()
{
	SaveAttributes(true);
}

void CDictionaryTool::SaveAttributes(bool replaceFlag)
{
	int count = m_listCtrlInflections.GetItemCount();
	CONCEPT *concept = NULL;
	CString wordStr,attrStr,valueStr;
	_TCHAR wordBuff[1001];
	CStringList words;
	int emptyCount = 0;

	for (int i=0; i<count; i++) {
		wordStr = m_listCtrlInflections.GetItemText(i,0);
		if (wordStr.IsEmpty())
			emptyCount++;
	}

	if (emptyCount > 0) {
		CString msgStr;
		msgStr.Format(_T("There are %d attribute(s) without word specifications.\nDo you want to fill them with the selected word \"%s\"?"),
			emptyCount,m_strWord);
		if (AfxMessageBox(msgStr,MB_YESNO) == IDYES) {
			for (int i=0; i<count; i++) {
				wordStr = m_listCtrlInflections.GetItemText(i,0);
				if (wordStr.IsEmpty())
					m_listCtrlInflections.SetItemText(i,0,m_strWord);
			}
		}
		else
			return;
	}

	for (int i=0; i<count; i++) {
		wordStr = m_listCtrlInflections.GetItemText(i,0);
		attrStr= m_listCtrlInflections.GetItemText(i,1);
		valueStr = m_listCtrlInflections.GetItemText(i,2);

		concept = WordLookup(wordStr);
		if (!concept) {
			StringToChar(wordBuff,wordStr,1000);
			concept = cg->addWord(wordBuff);
		}

		AddAttribute(concept,attrStr,valueStr,replaceFlag);

		if (words.Find(wordStr) == NULL)
			words.AddTail(wordStr);
	}
	
	POSITION pos = words.GetHeadPosition();

	while (pos && (wordStr = words.GetNext(pos)))
		RefreshWordDisplay(wordStr);
}

void CDictionaryTool::AddAttribute(CONCEPT *concept, CString attrStr, CString valueStr, bool replaceFlag)
{
	if (concept) {
		_TCHAR attrBuff[1001];
		_TCHAR valBuff[1001];
		StringToChar(attrBuff,attrStr,1000);
		StringToChar(valBuff,valueStr,1000);

		// If exists, then return
		if (cg->attrWithval(concept,attrBuff,valBuff))
			return;

		// See if value is a concept
		if (valueStr.Find(_T(":"),0) > 0) {
			CONCEPT *valConcept = ConceptFromPath(valueStr,true);
			if (!valConcept) {
				valueStr = valueStr.Mid(valueStr.ReverseFind(':')+1);
				StringToChar(valBuff,valueStr,1000);
				valConcept = cg->addWord(valBuff);
			}
			if (replaceFlag)
				cg->replaceVal(concept,attrBuff,valConcept);
			else
				cg->addVal(concept,attrBuff,valConcept);
		}
		else if (IsDigit(valueStr)) {
			if (replaceFlag)
				cg->replaceVal(concept,attrBuff,_ttol(valueStr));
			else
				cg->addVal(concept,attrBuff,_ttol(valueStr));
		}
		else {
			if (replaceFlag)
				cg->replaceVal(concept,attrBuff,valBuff);
			else
				cg->addVal(concept,attrBuff,valBuff);
		}
	}
}

void CDictionaryTool::OnBnClickedClearWords()
{
	m_intNewWords.RemoveAll();
	m_listCtrlWords.DeleteAllItems();
}

void CDictionaryTool::OnCbnSelchangeAttrMatch()
{
	// TODO: Add your control notification handler code here
}

void CDictionaryTool::OnBnClickedAddInflection()
{
	UpdateData(true);
	AddInflection(m_strAttrWord,m_strAttr,m_strValue);
	UpdateData(false);
}

void CDictionaryTool::OnBnClickedAddConcept()
{
	UpdateData(true);
	AddInflection(m_strAttrWord,m_strAttr,m_strValue,true);
	UpdateData(false);
}

void CDictionaryTool::OnBnClickedDeleteWord()
{
	UpdateData(true);
	int count = m_listCtrlWords.GetItemCount();
	CString strMsg;

	if (count > 1)
		strMsg.Format(_T("Do you want to delete the %d selected words from the KB?"),count);
	else
		strMsg.Format(_T("Do you want to delete the word \"%s\"?"),m_strWord);

	if (IDYES == AfxMessageBox(strMsg, MB_YESNO)) {

		POSITION pos = m_listCtrlWords.GetFirstSelectedItemPosition();
		CString wordStr;
		int item;
		CONCEPT *concept = NULL;
		CList<int,int> intList;

		while (pos) {
			item = m_listCtrlWords.GetNextSelectedItem(pos);
			intList.AddHead(item);
		}		

		pos = intList.GetHeadPosition();
		while (pos && (item = intList.GetNext(pos))) {
			concept = (CONCEPT *)m_listCtrlWords.GetItemData(item);
			if (concept) {
				wordStr = ConceptName(concept);
				cg->rmWord((LPTSTR)(LPCTSTR)m_strWord);
			}
			m_listCtrlWords.DeleteItem(item);
			DeleteNewWordIndex(item);
		}
	}
}

void CDictionaryTool::DeleteNewWordIndex(int index)
{
	m_intNewWords.RemoveAt(m_intNewWords.Find(index));
	int i = 0;

	POSITION pos = m_intNewWords.GetHeadPosition();
	POSITION lastPos;
	while (pos) {
		lastPos = pos;
		i = m_intNewWords.GetNext(pos);
		if (i > index)
			m_intNewWords.SetAt(lastPos,i-1);
	}
}

void CDictionaryTool::OnBnClickedRenamePos()
{
	int count = m_listBoxPOS.GetCount();
	if (count != 2)
		AfxMessageBox(_T("You must put the new and old POS in the POS listbox.\nThe first POS is the old and the second is the new POS.\nThere must be 2."));
	else {
		CString oldStr,newStr,msgStr,scopeStr;
		m_listBoxPOS.GetText(0,oldStr);
		m_listBoxPOS.GetText(1,newStr);
		int wordCount = m_listCtrlWords.GetItemCount();
		if (wordCount == 0)
			scopeStr = _T(" for the entire dictionary");
		else if (wordCount == 1)
			scopeStr = _T(" for one word in the list");
		else
			scopeStr.Format(_T(" for all %d words in the list"),wordCount);

		msgStr.Format(_T("Do you want to rename the POS \"%s\" to \"%s\" %s?"),oldStr,newStr,scopeStr);

		if (IDYES == AfxMessageBox(msgStr,MB_YESNO)) {
			theApp.BeginWaitCursor();

			if (wordCount == 0) {
				CONCEPT *sys = cg->findConcept(cg->findRoot(),_T("sys"));
				CONCEPT *dict = cg->findConcept(sys,_T("dict"));
				CONCEPT *a1 = cg->findConcept(dict,_T("a"));

				int count = 0;
				RenamePOS(a1,0,count,_T("pos"),oldStr,newStr);
			}
			else {
				CONCEPT *concept;
				for (int i=0; i<wordCount; i++) {
					concept = FindWordConcept(m_listCtrlWords.GetItemText(i,0));
					ConceptAttributeUpdate(concept,oldStr,newStr);
				}
			}
			theApp.EndWaitCursor();
		}
	}
}

bool CDictionaryTool::RenamePOS(CONCEPT *startConcept, int level, int &count, CString posStr, CString oldValStr, CString newValStr)
{
	if (!startConcept)
		return false;

	CString parentConceptStr = ConceptName(startConcept);
	CONCEPT *concept = startConcept;
	CString conceptStr,valStr;
	ATTR *attrs;
	int result = 0;

	while (concept) {
		conceptStr = ConceptName(concept);
		attrs = cg->findAttrs(concept);
		KBFindValue(concept,_T("pos"),valStr);

		if (valStr == oldValStr) {
			ConceptAttributeUpdate(concept,oldValStr,newValStr);
			AddWord(conceptStr,concept);
			m_intCount = count;
			count++;
		}

		if (cg->Down(concept)) {
			if (RenamePOS(cg->Down(concept),level+1,count,posStr,oldValStr,newValStr))
				break;
		}

		concept = cg->Next(concept);
		PeekHere(1);
	}

	if (level == 0)
		SetCountDisplay();

	return false;
}

void CDictionaryTool::OnBnClickedAddAttrAll()
{
	if (AfxMessageBox(_T("Do you want to add the current attributes and\nvalues to all the words in the list?"),MB_YESNO) == IDYES) {
		int count = m_listCtrlWords.GetItemCount();
		int countInflections = m_listCtrlInflections.GetItemCount();
		CONCEPT *concept = NULL;
		CString wordStr,attrStr,valueStr;
		_TCHAR wordBuff[1001];

		for (int i=0; i<count; i++) {
			wordStr = m_listCtrlWords.GetItemText(i,0);
			concept = (CONCEPT *)m_listCtrlWords.GetItemData(i);
			if (!concept)
				concept = FindWordConcept(wordStr);
			if (!concept) {
				StringToChar(wordBuff,wordStr,1000);
				concept = cg->addWord(wordBuff);
				m_listCtrlWords.SetItemData(i,(DWORD)concept);
				m_intNewWords.RemoveAt(m_intNewWords.Find(i));
			}
			else
				wordStr = ConceptName(concept);

			for (int j=0; j<countInflections; j++) {
				attrStr= m_listCtrlInflections.GetItemText(j,1);
				valueStr = m_listCtrlInflections.GetItemText(j,2);

				AddAttribute(concept,attrStr,valueStr);
			}
			RefreshWordDisplay(wordStr);
		}
	}
}

void CDictionaryTool::OnBnClickedAddList()
{
	CWordListDlg dlg;

	if (dlg.DoModal() == IDOK) {
		CString wordStr;
		CONCEPT *concept = NULL;

		POSITION pos = dlg.m_strListWords.GetHeadPosition();
		m_intCount = 0;

		while (pos && (wordStr = dlg.m_strListWords.GetNext(pos))) {
			if (wordStr.GetLength() <= 2)
				continue;
			concept = FindWordConcept(wordStr);
			AddWord(wordStr,concept,true);
		}

		RefreshWordsDisplay();
	}
}

void CDictionaryTool::OnBnClickedDeleteInflection()
{
	m_listCtrlInflections.DeleteItem(m_intAttrSelected);
	if (m_listCtrlInflections.GetItemCount()) {
		int newSel = m_intAttrSelected;
		if (newSel > m_listCtrlInflections.GetItemCount() - 1)
			newSel--;
		m_listCtrlInflections.SetItemState(newSel,LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
}

void CDictionaryTool::OnNMCustomdrawWords(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	*pResult = CDRF_DODEFAULT;

    if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage) {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage) {
        COLORREF crText, crTextBk;
        
		if (m_intNewWords.GetCount() && m_intNewWords.Find(pLVCD->nmcd.dwItemSpec) != NULL) {
            crText = RGB(255,0,0);
            crTextBk = RGB(255,255,255);
		}
		else {
            crText = RGB(0,0,0);
            crTextBk = RGB(255,255,255);
		}

        // Store the colors back in the NMLVCUSTOMDRAW struct.
        pLVCD->clrText = crText;
        pLVCD->clrTextBk = crTextBk;
		*pResult = CDRF_NEWFONT;
	}
}

void CDictionaryTool::RefreshWordsDisplay()
{
	CString wordStr;
	int count = m_listCtrlWords.GetItemCount();
	CONCEPT *concept = NULL;

	for (int i=0; i<count; i++) {
		concept = (CONCEPT *)m_listCtrlWords.GetItemData(i);
		RefreshWordDisplay(concept,i);
	}
}

void CDictionaryTool::RefreshWordDisplay(CONCEPT *concept, int index)
{
	CString wordStr;
	if (concept) {
		CStringList POSStringList;
		CStringList attrStringList;
		CStringList valueStringList;
		wordStr = WordDisplay(concept,POSStringList,attrStringList,valueStringList);
		m_listCtrlWords.SetItemText(index,0,wordStr);
	}
}

void CDictionaryTool::RefreshWordDisplay(CString wordStr)
{
	if (!wordStr.IsEmpty()) {
		LVFINDINFO info;
		info.flags = LVFI_STRING | LVFI_PARTIAL;
		info.psz = wordStr;
		int nIndex = m_listCtrlWords.FindItem(&info);

		if (nIndex >= 0) {
			CONCEPT *concept = (CONCEPT *)m_listCtrlWords.GetItemData(nIndex);

			if (concept) {
				CStringList POSStringList;
				CStringList attrStringList;
				CStringList valueStringList;
				wordStr = WordDisplay(concept,POSStringList,attrStringList,valueStringList);
				m_listCtrlWords.SetItemText(nIndex,0,wordStr);
			}
		}
	}
}

void CDictionaryTool::OnBnClickedRefresh()
{
	RefreshWordsDisplay();
}

void CDictionaryTool::OpenKb(CONCEPT *concept) 
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	m_boolOpeningKB = true;
	wnd->SendMessage(WM_COMMAND,ID_KNOWLEDGE_BASE);
	m_boolOpeningKB = false;

	CKnowledgeView* pView = (CKnowledgeView *)theApp.GetKBEditor();
	if (pView && concept)
		pView->SelectFromConcept(concept,false);
}

void CDictionaryTool::OnBnClickedKb()
{
	UpdateData(true);
	OpenKb(NULL); 
	UpdateData(false);
}

void CDictionaryTool::LoadConcept(CONCEPT *concept) 
{
	UpdateData(true);
	m_strValue = ConceptPathStr(concept,true);
	UpdateData(false);
}

void CDictionaryTool::OnBnClickedRemove()
{
	POSITION pos = m_listCtrlWords.GetFirstSelectedItemPosition();
	CList<int,int> intList;
	int item;

	while (pos) {
		item = m_listCtrlWords.GetNextSelectedItem(pos);
		intList.AddHead(item);
	}
		
	pos = intList.GetHeadPosition();
	while (pos && (item = intList.GetNext(pos))) {
		m_listCtrlWords.DeleteItem(item);
		DeleteNewWordIndex(item);
	}
}

void CDictionaryTool::OnBnClickedMultiSelect()
{
	UpdateData(true);
	if (m_boolMultiSelect == false)
		DeselectListCtrlItems(&m_listCtrlWords);
}
