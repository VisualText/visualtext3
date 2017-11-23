/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// MarkSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Utils.h"
#include "kb.h"
#include "MainFrm.h"
#include "MarkSampleDlg.h"
#include "TreeViewGrammar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString appAskText;
extern CVisualTextApp theApp;
extern CG *cg;

/////////////////////////////////////////////////////////////////////////////
// CMarkSampleDlg dialog


CMarkSampleDlg::CMarkSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMarkSampleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMarkSampleDlg)
	m_strEdit1 = _T("");
	m_strEdit2 = _T("");
	m_strEdit3 = _T("");
	m_strEdit4 = _T("");
	m_strEdit5 = _T("");
	m_strEdit6 = _T("");
	m_strEdit7 = _T("");
	m_strStatic1 = _T("");
	m_strStatic2 = _T("");
	m_strStatic3 = _T("");
	m_strStatic4 = _T("");
	m_strStatic5 = _T("");
	m_strStatic6 = _T("");
	m_strStatic7 = _T("");
	m_boolCheck1 = false;
	m_boolCheck2 = false;
	m_boolCheck3 = false;
	m_boolCheck4 = false;
	m_boolCheck5 = false;
	m_boolCheck6 = false;
	m_boolCheck7 = false;
	m_strSample = _T("");
	m_boolCheck8 = false;
	m_boolCheck9 = false;
	m_strEdit8 = _T("");
	m_strEdit9 = _T("");
	m_strStatic8 = _T("");
	m_strStatic9 = _T("");
	m_boolSpecial = false;
	m_boolDrag = false;
	//}}AFX_DATA_INIT

	m_concept = NULL;
	m_strSample = _T("");
	m_strTextPath = _T("");
	m_longStart = 0;
	m_longEnd = 0;
	m_fieldSelected = 0;
	m_boolGather = false;
	m_boolTear = false;
	m_boolDrag = true;
	m_intConceptCount = 0;
}


void CMarkSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMarkSampleDlg)
	DDX_Text(pDX, IDC_EDIT1, m_strEdit1);
	DDX_Text(pDX, IDC_EDIT2, m_strEdit2);
	DDX_Text(pDX, IDC_EDIT3, m_strEdit3);
	DDX_Text(pDX, IDC_EDIT4, m_strEdit4);
	DDX_Text(pDX, IDC_EDIT5, m_strEdit5);
	DDX_Text(pDX, IDC_EDIT6, m_strEdit6);
	DDX_Text(pDX, IDC_EDIT7, m_strEdit7);
	DDX_Text(pDX, IDC_STATIC_1, m_strStatic1);
	DDX_Text(pDX, IDC_STATIC_2, m_strStatic2);
	DDX_Text(pDX, IDC_STATIC_3, m_strStatic3);
	DDX_Text(pDX, IDC_STATIC_4, m_strStatic4);
	DDX_Text(pDX, IDC_STATIC_5, m_strStatic5);
	DDX_Text(pDX, IDC_STATIC_6, m_strStatic6);
	DDX_Text(pDX, IDC_STATIC_7, m_strStatic7);
	DDX_Check(pDX, IDC_CHECK1, m_boolCheck1);
	DDX_Check(pDX, IDC_CHECK2, m_boolCheck2);
	DDX_Check(pDX, IDC_CHECK3, m_boolCheck3);
	DDX_Check(pDX, IDC_CHECK4, m_boolCheck4);
	DDX_Check(pDX, IDC_CHECK5, m_boolCheck5);
	DDX_Check(pDX, IDC_CHECK6, m_boolCheck6);
	DDX_Check(pDX, IDC_CHECK7, m_boolCheck7);
	DDX_Text(pDX, IDC_SAMPLE, m_strSample);
	DDX_Check(pDX, IDC_CHECK8, m_boolCheck8);
	DDX_Check(pDX, IDC_CHECK9, m_boolCheck9);
	DDX_Text(pDX, IDC_EDIT8, m_strEdit8);
	DDX_Text(pDX, IDC_EDIT9, m_strEdit9);
	DDX_Text(pDX, IDC_STATIC_8, m_strStatic8);
	DDX_Text(pDX, IDC_STATIC_9, m_strStatic9);
	DDX_Check(pDX, IDC_SPECIAL, m_boolSpecial);
	DDX_Check(pDX, IDC_DRAG, m_boolDrag);
	//}}AFX_DATA_MAP
}

void CMarkSampleDlg::SetInfo(CString sampleStr, CString textPathStr,
							 CONCEPT *concept, long start, long end) 
{
	if (concept)
		m_concept = concept;
	else {
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		m_concept = wnd->GrammarConceptSelected();
	}

	m_strSample = sampleStr;
	m_strTextPath = textPathStr;
	m_longStart = start;
	m_longEnd = end;
}

BEGIN_MESSAGE_MAP(CMarkSampleDlg, CDialog)
	//{{AFX_MSG_MAP(CMarkSampleDlg)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_SPECIAL, OnSpecial)
	ON_BN_CLICKED(IDC_CHECK2, OnCheck2)
	ON_BN_CLICKED(IDC_CHECK3, OnCheck3)
	ON_BN_CLICKED(IDC_CHECK4, OnCheck4)
	ON_BN_CLICKED(IDC_CHECK5, OnCheck5)
	ON_BN_CLICKED(IDC_CHECK6, OnCheck6)
	ON_BN_CLICKED(IDC_CHECK7, OnCheck7)
	ON_BN_CLICKED(IDC_CHECK8, OnCheck8)
	ON_BN_CLICKED(IDC_CHECK9, OnCheck9)
	ON_BN_CLICKED(IDC_GATHER_UP, OnGatherUp)
	ON_BN_CLICKED(IDC_GATHER_DOWN, OnGatherDown)
	ON_BN_CLICKED(IDC_REVERT, OnRevert)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_TEAR_UP, OnTearUp)
	ON_BN_CLICKED(IDC_TEAR_DOWN, OnTearDown)
	ON_BN_CLICKED(IDC_DRAG, OnDrag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMarkSampleDlg message handlers


void CMarkSampleDlg::FillGuesses() 
{
	CStringList samples;
	SeparateSamples(samples,m_strSample,m_longStart,m_longEnd);

	int i = 0;
	CString sampleStr;
	CString sampStr;
	CString conceptStr;
	long start;
	long end;

	ClearFields();

	while ((sampleStr = NthStringListItem(samples,i)) != _T("")) {
		GetSampleLine(sampleStr,sampStr,conceptStr,start,end);
		SetFieldData(i++,sampStr,conceptStr,start,end,false);
	}

	SetFields();
	DeselectOthers();
}

void CMarkSampleDlg::UpdateButtonActivationStatus()
{
	// TEAR APART BUTTON ENABLING LOGIC
	if (IsFieldSelected() && NumSamplePieces() > 1) {
		if (m_fieldSelected == 1)
			Enable(IDC_TEAR_UP,false);
		else
			Enable(IDC_TEAR_UP,true);
		if (m_fieldSelected == m_intConceptCount)
			Enable(IDC_TEAR_DOWN,false);
		else
			Enable(IDC_TEAR_DOWN,true);
	}
	else {
		Enable(IDC_TEAR_UP,false);
		Enable(IDC_TEAR_DOWN,false);
	}

	if (GetState(IDC_TEAR_DOWN) || GetState(IDC_TEAR_UP))
		Enable(IDC_TEAR,true);
	else
		Enable(IDC_TEAR,false);
	
	// GATHER BUTTON ENABLING LOGIC
	if (IsFieldSelected()) {
		CString sampleStr = NthStringListItem(m_strListSamples,m_fieldSelected-2);
		if (sampleStr == _T(""))
			Enable(IDC_GATHER_DOWN,false);
		else
			Enable(IDC_GATHER_DOWN,true);

		sampleStr = NthStringListItem(m_strListSamples,m_fieldSelected);
		if (sampleStr == _T(""))
			Enable(IDC_GATHER_UP,false);
		else
			Enable(IDC_GATHER_UP,true);
	}
	else {
		Enable(IDC_GATHER_UP,false);
		Enable(IDC_GATHER_DOWN,false);
	}
	if (GetState(IDC_GATHER_DOWN) || GetState(IDC_GATHER_UP))
		Enable(IDC_JOIN,true);
	else
		Enable(IDC_JOIN,false);
	
	// DOWN/UP BUTTON ENABLING LOGIC
	if (m_boolDrag) {
		CString firstSampleStr = NthStringListItem(m_strListSamples,0);
		if (firstSampleStr == _T(""))
			Enable(IDC_UP,true);
		else
			Enable(IDC_UP,false);

		CString lastSampleStr = NthStringListItem(m_strListSamples,m_intConceptCount-1);
		if (lastSampleStr == _T(""))
			Enable(IDC_DOWN,true);
		else
			Enable(IDC_DOWN,false);
	}
	else if (IsFieldSelected()) {
		Enable(IDC_UP,true);
		Enable(IDC_DOWN,true);
	}
	else {
		Enable(IDC_UP,false);
		Enable(IDC_DOWN,false);
	}
	if (GetState(IDC_DOWN) || GetState(IDC_UP))
		Enable(IDC_MOVE_ONE,true);
	else
		Enable(IDC_MOVE_ONE,false);
}

void CMarkSampleDlg::Enable(UINT id, BOOL value)
{
	CWnd *wnd = (CWnd *)GetDlgItem(id);
	wnd->EnableWindow(value);
}

BOOL CMarkSampleDlg::GetState(UINT id)
{
	CWnd *wnd = (CWnd *)GetDlgItem(id);
	return wnd->IsWindowEnabled();
}

void CMarkSampleDlg::GetSampleLine(CString sampleStr, CString &sampStr,
										   CString &conceptStr, long &start, long &end)
{
	NthPathSegment(sampleStr, sampStr, SAMPSEP, 0);
	NthPathSegment(sampleStr, conceptStr, SAMPSEP, 1);
	NthPathSegmentLong(sampleStr, start, SAMPSEP, 2);
	NthPathSegmentLong(sampleStr, end, SAMPSEP, 3);
}

void CMarkSampleDlg::SeparateSamples(CStringList &samples, CString sampleStr, long start, long end, bool conceptFlag) 
{
	int i = 0;
	int len = sampleStr.GetLength();
	_TCHAR c;
	_TCHAR lastC = ' ';
	CString gatherStr = _T("");
	CString newStr;
	CString conceptStr;
	end = start;
	CONCEPT *concept = cg->Down(m_concept);

	for (i=0; i<=len || (concept && conceptFlag); i++) {
		if (i < len)
			c = sampleStr.GetAt(i);
		else
			c = '\0';

		if (alphabetic(c) && (gatherStr == _T("") || !_istdigit(lastC))) {
			gatherStr += c;
			end++;
		}
		else if (_istdigit(c) && (gatherStr == _T("") || !alphabetic(lastC))) {
			gatherStr += c;
			end++;
		}
		else {
			if (gatherStr != _T("") || (concept && c == '\0')) {
				conceptStr = ConceptName(concept);
				newStr = SampleInfo(gatherStr,conceptStr,start,end-1);
				samples.AddTail(newStr);
				concept = cg->Next(concept);
			}
			gatherStr = _T("");
			if (_istdigit(c) || alphabetic(c)) {
				gatherStr = c;
				start = end++;
			}
			else
				start = ++end;
		}
		lastC = c;
	}
}

BOOL CMarkSampleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	FillGuesses();

	SetWindowText(ConceptName(m_concept));

	VERIFY(bitmapButtonUp.AutoLoad(IDC_UP,this));
	VERIFY(bitmapButtonDown.AutoLoad(IDC_DOWN,this));
	VERIFY(bitmapButtonGatherDown.AutoLoad(IDC_GATHER_DOWN,this));
	VERIFY(bitmapButtonGatherUp.AutoLoad(IDC_GATHER_UP,this));
	VERIFY(bitmapButtonTearDown.AutoLoad(IDC_TEAR_DOWN,this));
	VERIFY(bitmapButtonTearUp.AutoLoad(IDC_TEAR_UP,this));

	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

void CMarkSampleDlg::Up(int itemNumber) 
{
	GetFields();

	itemNumber--; // ONE TO ZERO NUMBERED

	MoveIt(itemNumber);
	//DeselectOthers(m_fieldSelected);
	SetFields();
}

void CMarkSampleDlg::Down(int itemNumber) 
{
	GetFields();
	itemNumber--; // ONE TO ZERO NUMBERED

	MoveIt(itemNumber,true);
	SetFields();
}

void CMarkSampleDlg::MoveIt(int itemNumber, bool downFlag)
{
	int indexFrom = itemNumber;
	int indexTo = itemNumber - 1;
	if ((downFlag && !m_boolGather) || (!downFlag && m_boolGather))
		indexTo = itemNumber + 1;

	// WRAP THE INDEXTO NUMBER IF BOUNDARY
	if (indexTo < 0)
		indexTo = m_intConceptCount - 1;
	else if (indexTo >= m_intConceptCount)
		indexTo = 0;

	if (m_boolGather) {
		int indexHold = indexFrom;
		indexFrom = indexTo;
		indexTo = indexHold;
	}

	CString sampleFrom = NthStringListItem(m_strListSamples,indexFrom);
	CString sampleTo = NthStringListItem(m_strListSamples,indexTo);

	// DO NOTHING IF EMPTY
	if (!m_boolDrag && sampleFrom == _T("") && sampleTo == _T(""))
		return;

	CString poppedStr;

	// IF NEITHER CHECKED, THEN SIMPLE SWITCH
	if (!m_boolDrag && !m_boolGather && !m_boolTear) {
		SwitchSamples(indexFrom,indexTo);
	}
	else if (m_boolGather || m_boolTear) {
		// GET PART TO MOVE
		if (downFlag)
			poppedStr = PopTail(indexFrom);
		else
			poppedStr = PopHead(indexFrom);

		// MOVE PART
		if (downFlag) {
			if (m_boolDrag) {
				if (m_boolGather)
					PushSamplesDown(0,indexFrom);
				else if (m_boolTear)
					PushSamplesDown(indexTo,m_intConceptCount - 1);
			}
			PushHead(indexTo,poppedStr);
		}
		else {
			if (m_boolDrag) {
				if (m_boolGather)
					PushSamplesUp(indexFrom,m_intConceptCount - 1);
				else if (m_boolTear)
					PushSamplesUp(0,indexTo);
			}
			PushTail(indexTo,poppedStr);
		}
	}
	else if (m_boolDrag) {
		if (IsFieldSelected() && m_fieldSelected > 1) {
			if (downFlag)
				PushSamplesDown(m_fieldSelected-1,m_intConceptCount-1);
			else
				PushSamplesUp(0,m_fieldSelected-1);
		}
		else {
			if (downFlag)
				PushSamplesDown(0,m_intConceptCount-1);
			else
				PushSamplesUp(0,m_intConceptCount-1);
		}
	}

	// MOVE CHECK IF NOT GATHERING OR TEARING
	if ((!m_boolDrag || IsFieldSelected()) && !m_boolGather && !m_boolTear) {
		SetCheckValue(indexTo+1);
		DeselectOthers(indexTo+1);
	}
	else
		DeselectOthers(m_fieldSelected);
}

void CMarkSampleDlg::PushSamplesUp(int indexTop, int indexBottom)
{
	if (indexTop != indexBottom) {
		for (int i=indexTop; i<indexBottom; i++) {
			SwitchSamples(i,i+1);
		}
	}
}

void CMarkSampleDlg::PushSamplesDown(int indexTop, int indexBottom)
{
	if (indexTop != indexBottom) {
		for (int i=indexBottom; i>indexTop; i--) {
			SwitchSamples(i,i-1);
		}
	}
}

CString CMarkSampleDlg::RemoveSample(int index)
{
	CString poppedStr = GetSampleInfo(index);
	ReplaceNthStringListItem(m_strListSamples,index,_T(""));
	return poppedStr;
}

CString CMarkSampleDlg::GetSampleInfo(int index)
{
	CString sampleStr = NthStringListItem(m_strListSamples,index);
	CString conceptStr = NthStringListItem(m_strListConcepts,index);
	long start = m_longStarts[index];
	long end = m_longEnds[index];

	CString infoStr;
	infoStr.Format(_T("%s%c%s%c%d%c%d"),sampleStr,SAMPSEP,conceptStr,SAMPSEP,start,SAMPSEP,end);

	return SampleInfo(sampleStr,conceptStr,start,end);
}

CString CMarkSampleDlg::SampleInfo(CString sampleStr, CString conceptStr, long start, long end)
{
	CString infoStr;
	infoStr.Format(_T("%s%c%s%c%d%c%d"),sampleStr,SAMPSEP,conceptStr,SAMPSEP,start,SAMPSEP,end);

	return infoStr;
}

void CMarkSampleDlg::SetSampleInfo(int index, CString infoStr)
{
	CString sampleStr;
	CString conceptStr;
	long start;
	long end;

	GetSampleLine(infoStr,sampleStr,conceptStr,start,end);
	conceptStr = NthStringListItem(m_strListConcepts,index);
	SetFieldData(index,sampleStr,conceptStr,start,end);
}

CString CMarkSampleDlg::PopTail(int index)
{
	CString sampleStr = NthStringListItem(m_strListSamples,index);
	CStringList samples;
	SeparateSamples(samples,sampleStr,m_longStarts[index],m_longEnds[index],false);

	CString lastSampleStr = samples.RemoveTail();

	if (samples.GetCount()) {
		CString firstSampleStr = samples.GetHead();
		CString penultimateSampleStr = samples.RemoveTail();
		CString mergedStr = Merge(firstSampleStr,penultimateSampleStr);
		SetSampleInfo(index,mergedStr);
	}
	else
		SetSampleInfo(index,_T(""));

	return lastSampleStr;
}

CString CMarkSampleDlg::PopHead(int index)
{
	CString sampleStr = NthStringListItem(m_strListSamples,index);
	CStringList samples;
	SeparateSamples(samples,sampleStr,m_longStarts[index],m_longEnds[index],false);

	CString firstSampleStr = samples.RemoveHead();

	if (samples.GetCount()) {
		CString secondSampleStr = samples.GetHead();
		CString lastSampleStr = samples.GetTail();
		CString mergedStr = Merge(secondSampleStr,lastSampleStr);
		SetSampleInfo(index,mergedStr);
	}
	else
		SetSampleInfo(index,_T(""));

	return firstSampleStr;
}

void CMarkSampleDlg::PushHead(int index, CString poppedStr)
{
	CString mergedStr = Merge(poppedStr,GetSampleInfo(index));
	SetSampleInfo(index,mergedStr);
}

void CMarkSampleDlg::PushTail(int index, CString poppedStr)
{
	CString mergedStr = Merge(GetSampleInfo(index),poppedStr);
	SetSampleInfo(index,mergedStr);
}

CString CMarkSampleDlg::Merge(CString sampleInfoStr1, CString sampleInfoStr2)
{
	CString sampStr1;
	CString conceptStr1;
	long startL1;
	long endL1;
	CString sampStr2;
	CString conceptStr2;
	long startL2;
	long endL2;

	GetSampleLine(sampleInfoStr1,sampStr1,conceptStr1,startL1,endL1);
	GetSampleLine(sampleInfoStr2,sampStr2,conceptStr2,startL2,endL2);

	if (sampStr1 == _T(""))
		return sampleInfoStr2;
	else if (sampStr2 == _T(""))
		return sampleInfoStr1;

	int start = startL1 - m_longStart;
	int len = endL2 - startL1 + 1;
	CString sampleStr = m_strSample.Mid(start,len);

	return SampleInfo(sampleStr,conceptStr1,startL1,endL2);
}

void CMarkSampleDlg::SwitchSamples(int index1, int index2)
{
	CString sampleStr1 = GetSampleInfo(index1);
	CString sampleStr2 = GetSampleInfo(index2);

	if (sampleStr1.GetAt(0) == SAMPSEP && sampleStr2.GetAt(0) == SAMPSEP)
		return;

	if (index1 >=0 )
		SetSampleInfo(index1,sampleStr2);
	if (index2 < m_intConceptCount)
		SetSampleInfo(index2,sampleStr1);
}

void CMarkSampleDlg::ClearFields()
{
	m_fieldSelected = 0;

	m_strListSamples.RemoveAll();
	m_strListConcepts.RemoveAll();

	for (int i=0; i<NUM_SAMPLE_FIELDS; i++) {
		m_strListSamples.AddTail(_T(""));
		m_strListConcepts.AddTail(_T(""));
		SetFieldData(i,_T(""),_T(""),0,0,false);
		m_boolChecks[i] = false;
	}
}

void CMarkSampleDlg::ClearFieldData(int index)
{
	SetFieldData(index,_T(""),_T(""),0,0,false);
}

void CMarkSampleDlg::SetFieldData(int index, CString sampleStr, CString conceptStr,
								  long start, long end, int check)
{
	if (check < 0)
		check = m_boolChecks[index];

	ReplaceNthStringListItem(m_strListSamples,index,sampleStr);
	ReplaceNthStringListItem(m_strListConcepts,index,conceptStr);
	m_longStarts[index] = start;
	m_longEnds[index] = end;
	m_boolChecks[index] = check;
}

void CMarkSampleDlg::SetFields()
{
	m_strEdit1 = NthStringListItem(m_strListSamples,0);
	m_strStatic1 = NthStringListItem(m_strListConcepts,0);
	m_boolCheck1 = m_boolChecks[0];
	if (m_strStatic1 == _T("")) {
		Enable(IDC_EDIT1,false);
		Enable(IDC_CHECK1,false);
	}

	m_strEdit2 = NthStringListItem(m_strListSamples,1);
	m_strStatic2 = NthStringListItem(m_strListConcepts,1);
	m_boolCheck2 = m_boolChecks[1];
	if (m_strStatic2 == _T("")) {
		Enable(IDC_EDIT2,false);
		Enable(IDC_CHECK2,false);
	}

	m_strEdit3 = NthStringListItem(m_strListSamples,2);
	m_strStatic3 = NthStringListItem(m_strListConcepts,2);
	m_boolCheck3 = m_boolChecks[2];
	if (m_strStatic3 == _T("")) {
		Enable(IDC_EDIT3,false);
		Enable(IDC_CHECK3,false);
	}

	m_strEdit4 = NthStringListItem(m_strListSamples,3);
	m_strStatic4 = NthStringListItem(m_strListConcepts,3);
	m_boolCheck4 = m_boolChecks[3];
	if (m_strStatic4 == _T("")) {
		Enable(IDC_EDIT4,false);
		Enable(IDC_CHECK4,false);
	}

	m_strEdit5 = NthStringListItem(m_strListSamples,4);
	m_strStatic5 = NthStringListItem(m_strListConcepts,4);
	m_boolCheck5 = m_boolChecks[4];
	if (m_strStatic5 == _T("")) {
		Enable(IDC_EDIT5,false);
		Enable(IDC_CHECK5,false);
	}

	m_strEdit6 = NthStringListItem(m_strListSamples,5);
	m_strStatic6 = NthStringListItem(m_strListConcepts,5);
	m_boolCheck6 = m_boolChecks[5];
	if (m_strStatic6 == _T("")) {
		Enable(IDC_EDIT6,false);
		Enable(IDC_CHECK6,false);
	}

	m_strEdit7 = NthStringListItem(m_strListSamples,6);
	m_strStatic7 = NthStringListItem(m_strListConcepts,6);
	m_boolCheck7 = m_boolChecks[6];
	if (m_strStatic7 == _T("")) {
		Enable(IDC_EDIT7,false);
		Enable(IDC_CHECK7,false);
	}

	m_strEdit8 = NthStringListItem(m_strListSamples,7);
	m_strStatic8 = NthStringListItem(m_strListConcepts,7);
	m_boolCheck8 = m_boolChecks[7];
	if (m_strStatic8 == _T("")) {
		Enable(IDC_EDIT8,false);
		Enable(IDC_CHECK8,false);
	}

	m_strEdit9 = NthStringListItem(m_strListSamples,8);
	m_strStatic9 = NthStringListItem(m_strListConcepts,8);
	m_boolCheck9 = m_boolChecks[8];
	if (m_strStatic9 == _T("")) {
		Enable(IDC_EDIT9,false);
		Enable(IDC_CHECK9,false);
	}

	for (int i=0; i<NUM_SAMPLE_FIELDS; i++) {
		if (NthStringListItem(m_strListConcepts,i) == _T("")) {
			m_intConceptCount = i;
			break;
		}
	}
	UpdateData(false);
}

int CMarkSampleDlg::NumSamplePieces(int offset)
{
	int num = 0;

	int i = m_fieldSelected - 1 + offset;
	CStringList samples;
	SeparateSamples(samples,NthStringListItem(m_strListSamples,i),m_longStarts[i],m_longEnds[i],false);
	num = samples.GetCount();

	return num;
}

void CMarkSampleDlg::GetFields()
{
	UpdateData(true);

	ReplaceNthStringListItem(m_strListSamples,0,m_strEdit1);
	ReplaceNthStringListItem(m_strListConcepts,0,m_strStatic1);
	m_boolChecks[0] = m_boolCheck1;

	ReplaceNthStringListItem(m_strListSamples,1,m_strEdit2);
	ReplaceNthStringListItem(m_strListConcepts,1,m_strStatic2);
	m_boolChecks[1] = m_boolCheck2;

	ReplaceNthStringListItem(m_strListSamples,2,m_strEdit3);
	ReplaceNthStringListItem(m_strListConcepts,2,m_strStatic3);
	m_boolChecks[2] = m_boolCheck3;

	ReplaceNthStringListItem(m_strListSamples,3,m_strEdit4);
	ReplaceNthStringListItem(m_strListConcepts,3,m_strStatic4);
	m_boolChecks[3] = m_boolCheck4;

	ReplaceNthStringListItem(m_strListSamples,4,m_strEdit5);
	ReplaceNthStringListItem(m_strListConcepts,4,m_strStatic5);
	m_boolChecks[4] = m_boolCheck5;

	ReplaceNthStringListItem(m_strListSamples,5,m_strEdit6);
	ReplaceNthStringListItem(m_strListConcepts,5,m_strStatic6);
	m_boolChecks[5] = m_boolCheck6;

	ReplaceNthStringListItem(m_strListSamples,6,m_strEdit7);
	ReplaceNthStringListItem(m_strListConcepts,6,m_strStatic7);
	m_boolChecks[6] = m_boolCheck7;

	ReplaceNthStringListItem(m_strListSamples,7,m_strEdit8);
	ReplaceNthStringListItem(m_strListConcepts,7,m_strStatic8);
	m_boolChecks[7] = m_boolCheck8;

	ReplaceNthStringListItem(m_strListSamples,8,m_strEdit9);
	ReplaceNthStringListItem(m_strListConcepts,8,m_strStatic9);
	m_boolChecks[8] = m_boolCheck9;
}

void CMarkSampleDlg::OnOK() 
{
	GetFields();

	if (m_strTextPath == _T(""))
		AfxMessageBox(_T("Lost text variable!"));
	else {

		// ADD ENTIRE SAMPLE UNDER PARENT CONCEPT
		theApp.m_pMainFrame->m_wndGrammarView.AddConceptSample(m_strSample,m_strTextPath,m_longStart,m_longEnd,1);										// 12/09/99 AM.

		// ADD SUB SAMPLES
		CString sampleStr;
		for (int i=0; i<NUM_SAMPLE_FIELDS; i++) {
			sampleStr = NthStringListItem(m_strListSamples,i);

			// THE i+2 is:  one for converting zero to one-numbered menu, the other
			// is for skipping past the parent to the child concept.
			if (sampleStr != _T(""))
				theApp.m_pMainFrame->m_wndGrammarView.AddConceptSample(sampleStr,m_strTextPath,m_longStarts[i],m_longEnds[i],i+2);
		}
	}

	CDialog::OnOK();
}

void CMarkSampleDlg::CheckAction(int fieldNum) 
{
	UpdateData(true);
	CString fieldStr = NthStringListItem(m_strListSamples,fieldNum-1);
	DeselectOthers(fieldNum);
	UpdateData(false);
}

void CMarkSampleDlg::DeselectOthers(int fieldNum) 
{

	BOOL value = GetCheckValue(fieldNum);
	if (value)
		m_fieldSelected = fieldNum;
	else
		m_fieldSelected = 0;
	CString fieldStr;

	for (int i=1; i<=m_intConceptCount; i++) {
		fieldStr = NthStringListItem(m_strListSamples,i-1);
		if (fieldStr == _T(""))
			SetCheckValue(i,false);
		else if (i != fieldNum)
			SetCheckValue(i,false);

		if (fieldStr == _T(""))
			EnableCheck(i,false);
		else
			EnableCheck(i,true);
	}

	UpdateButtonActivationStatus();
}

void CMarkSampleDlg::EnableCheck(int fieldNum, BOOL value) 
{
	UINT box;

	switch (fieldNum) {
	case 1: box = IDC_CHECK1; break;
	case 2: box = IDC_CHECK2; break;
	case 3: box = IDC_CHECK3; break;
	case 4: box = IDC_CHECK4; break;
	case 5: box = IDC_CHECK5; break;
	case 6: box = IDC_CHECK6; break;
	case 7: box = IDC_CHECK7; break;
	case 8: box = IDC_CHECK8; break;
	case 9: box = IDC_CHECK9; break;
	}
	
	Enable(box,value);
}

BOOL CMarkSampleDlg::GetCheckValue(int fieldNum) 
{
	BOOL value = false;

	switch (fieldNum) {
	case 1: value = m_boolCheck1; break;
	case 2: value = m_boolCheck2; break;
	case 3: value = m_boolCheck3; break;
	case 4: value = m_boolCheck4; break;
	case 5: value = m_boolCheck5; break;
	case 6: value = m_boolCheck6; break;
	case 7: value = m_boolCheck7; break;
	case 8: value = m_boolCheck8; break;
	case 9: value = m_boolCheck9; break;
	}

	return value;
}

void CMarkSampleDlg::SetCheckValue(int fieldNum, bool value) 
{
	m_boolChecks[fieldNum-1] = value;

	switch (fieldNum) {
	case 1: m_boolCheck1 = value; break;
	case 2: m_boolCheck2 = value; break;
	case 3: m_boolCheck3 = value; break;
	case 4: m_boolCheck4 = value; break;
	case 5: m_boolCheck5 = value; break;
	case 6: m_boolCheck6 = value; break;
	case 7: m_boolCheck7 = value; break;
	case 8: m_boolCheck8 = value; break;
	case 9: m_boolCheck9 = value; break;
	}
}

bool CMarkSampleDlg::IsFieldSelected() 
{
	return m_fieldSelected > 0;
}

void CMarkSampleDlg::OnUp() 
{
	m_boolGather = false;
	m_boolTear = false;

	Up(m_fieldSelected);
}

void CMarkSampleDlg::OnDown() 
{
	m_boolGather = false;
	m_boolTear = false;

	Down(m_fieldSelected);
}

void CMarkSampleDlg::OnSpecial() 
{
	// TODO: Add your control notification handler code here
	
}

void CMarkSampleDlg::OnCheck1() 
{
	CheckAction(1);	
}

void CMarkSampleDlg::OnCheck2() 
{
	CheckAction(2);	
}

void CMarkSampleDlg::OnCheck3() 
{
	CheckAction(3);	
}

void CMarkSampleDlg::OnCheck4() 
{
	CheckAction(4);	
}

void CMarkSampleDlg::OnCheck5() 
{
	CheckAction(5);	
}

void CMarkSampleDlg::OnCheck6() 
{
	CheckAction(6);	
}

void CMarkSampleDlg::OnCheck7() 
{
	CheckAction(7);	
}

void CMarkSampleDlg::OnCheck8() 
{
	CheckAction(8);	
}

void CMarkSampleDlg::OnCheck9() 
{
	CheckAction(9);	
}

void CMarkSampleDlg::OnGatherUp() 
{
	m_boolGather = true;
	m_boolTear = false;

	if (IsFieldSelected()) {
		Up(m_fieldSelected);
	}
}

void CMarkSampleDlg::OnGatherDown() 
{
	m_boolGather = true;
	m_boolTear = false;

	if (IsFieldSelected()) {
		Down(m_fieldSelected);
	}
}

void CMarkSampleDlg::OnTearUp() 
{
	m_boolTear = true;
	m_boolGather = false;

	if (IsFieldSelected()) {
		Up(m_fieldSelected);
	}
}

void CMarkSampleDlg::OnTearDown() 
{
	m_boolTear = true;
	m_boolGather = false;

	if (IsFieldSelected()) {
		Down(m_fieldSelected);
	}
}

void CMarkSampleDlg::OnRevert() 
{
	FillGuesses();
}

void CMarkSampleDlg::OnDrag() 
{
	UpdateData(true);
	UpdateButtonActivationStatus();
}
