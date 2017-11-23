// EnglishInflectionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Utils.h"
#include "kb.h"
#include "VisualText.h"
#include "EnglishInflectionsDlg.h"
#include ".\englishinflectionsdlg.h"

extern CG *cg;

// CEnglishInflectionsDlg dialog

IMPLEMENT_DYNAMIC(CEnglishInflectionsDlg, CDialog)
CEnglishInflectionsDlg::CEnglishInflectionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEnglishInflectionsDlg::IDD, pParent)
	, m_strWord(_T(""))
	, m_strPlural(_T(""))
	, m_strPast(_T(""))
	, m_strGerund(_T(""))
	, m_boolYPlural(FALSE)
{
}

CEnglishInflectionsDlg::~CEnglishInflectionsDlg()
{
}

void CEnglishInflectionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_WORD, m_strWord);
	DDX_Control(pDX, IDC_POS, m_listBoxPOS);
	DDX_Text(pDX, IDC_PLURAL, m_strPlural);
	DDX_Text(pDX, IDC_PAST, m_strPast);
	DDX_Text(pDX, IDC_GERUND, m_strGerund);
	DDX_Check(pDX, IDC_Y, m_boolYPlural);
}


BEGIN_MESSAGE_MAP(CEnglishInflectionsDlg, CDialog)
	ON_BN_CLICKED(IDC_GUESS, OnBnClickedGuess)
	ON_BN_CLICKED(IDC_CLEAR, OnBnClickedClear)
	ON_LBN_SELCHANGE(IDC_POS, OnLbnSelchangePos)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_Y, OnBnClickedY)
END_MESSAGE_MAP()

BOOL CEnglishInflectionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	FillPOS();

	POSITION pos = m_strListPOS.GetHeadPosition();
	CString posStr;

	OnBnClickedGuess();

	m_listBoxPOS.SetFocus();
	int i = 0;
	while (pos && (posStr = m_strListPOS.GetNext(pos))) {
		i = m_listBoxPOS.FindStringExact(-1,posStr);
		m_listBoxPOS.SetSel(i);
	}

	UpdateItemsActivitation();

	// SET FOCUS ON ANALYZER NAME
	CEdit *ed = (CEdit *)GetDlgItem(IDC_WORD);
	ed->SetFocus();

	UpdateData(false);

	return FALSE;
}

void CEnglishInflectionsDlg::FillPOS()
{
	CONCEPT *root = cg->findRoot();
	CONCEPT *sys = cg->findConcept(root,_T("sys"));
	CONCEPT *nlp = cg->findConcept(sys,_T("nlp"));
	CONCEPT *pos = cg->findConcept(nlp,_T("pos"));
	CONCEPT *posses = cg->Down(pos);

	m_listBoxPOS.ResetContent();
	while (posses) {
		m_listBoxPOS.AddString(ConceptName(posses));
		posses = cg->Next(posses);
	}
}

void CEnglishInflectionsDlg::OnBnClickedGuess()
{
	if (!m_strWord.IsEmpty()) {
		int len = m_strWord.GetLength();

		m_strPlural.Format(_T("%ss"),m_strWord);
		m_strPast.Format(_T("%sed"),m_strWord);
		m_strGerund.Format(_T("%sing"),m_strWord);

		CString wordRightTrimmed = m_strWord.Mid(0,len-1);
		_TCHAR c = m_strWord.GetAt(m_strWord.GetLength()-1);

		if (m_boolYPlural) {
			if (c == 'y' && len > 3) {
				m_strPlural.Format(_T("%sies"),wordRightTrimmed);
				m_strPast.Format(_T("%sied"),wordRightTrimmed);
			}
		}
	}
}

bool CEnglishInflectionsDlg::HasPOS(CString posStr)
{
	POSITION pos = m_strListPOS.GetHeadPosition();
	CString pStr;

	while (pos && (pStr = m_strListPOS.GetNext(pos))) {
		if (pStr == posStr)
			return true;
	}
	return false;
}

void CEnglishInflectionsDlg::OnBnClickedClear()
{
	int count = m_listBoxPOS.GetCount();

	for (int i=0; i<count; i++)
		m_listBoxPOS.SetSel(i,0);

	m_strListPOS.RemoveAll();
}

void CEnglishInflectionsDlg::UpdateItemsActivitation()
{
	CArray<int,int> aryListBoxSel;
	int selCount = m_listBoxPOS.GetSelCount();
	aryListBoxSel.SetSize(selCount);
	m_listBoxPOS.GetSelItems(selCount, aryListBoxSel.GetData()); 
	CString strPOS;

	SetEnable(IDC_PLURAL,false);
	SetEnable(IDC_PAST,false);
	SetEnable(IDC_GERUND,false);

	for (int i=0; i<selCount; i++) {
		m_listBoxPOS.GetText(aryListBoxSel.GetAt(i),strPOS);
		if (strPOS == _T("noun"))
			SetEnable(IDC_PLURAL,true);
		if (strPOS == _T("verb")) {
			SetEnable(IDC_PLURAL,true);
			SetEnable(IDC_PAST,true);
			SetEnable(IDC_GERUND,true);
		}
	}
}

void CEnglishInflectionsDlg::SetEnable(UINT id, bool value)
{
	CWnd *itemWnd = GetDlgItem(id);
	itemWnd->EnableWindow(value);
}

void CEnglishInflectionsDlg::OnLbnSelchangePos()
{
	UpdateItemsActivitation();
	UpdatePOSStringList();
}

void CEnglishInflectionsDlg::AddPOS(CString posStr)
{
	m_strListPOS.AddTail(posStr);
}

void CEnglishInflectionsDlg::OnClose()
{
	UpdatePOSStringList();
}

void CEnglishInflectionsDlg::UpdatePOSStringList()
{
	CArray<int,int> aryListBoxSel;
	int selCount = m_listBoxPOS.GetSelCount();
	aryListBoxSel.SetSize(selCount);
	m_listBoxPOS.GetSelItems(selCount, aryListBoxSel.GetData()); 
	CString strPOS;
	m_strListPOS.RemoveAll();

	for (int i=0; i<selCount; i++) {
		m_listBoxPOS.GetText(aryListBoxSel.GetAt(i),strPOS);
		m_strListPOS.AddTail(strPOS);
	}

	CDialog::OnClose();
}

void CEnglishInflectionsDlg::OnBnClickedY()
{
	UpdateData(true);
	OnBnClickedGuess();
	UpdateData(false);
}
