// WordListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VisualText.h"
#include "Utils.h"
#include "kb.h"
#include "WordListDlg.h"
#include ".\wordlistdlg.h"


// CWordListDlg dialog

IMPLEMENT_DYNAMIC(CWordListDlg, CXTResizeDialog)
CWordListDlg::CWordListDlg(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CWordListDlg::IDD, pParent)
	, m_strHandWords(_T(""))
{
}

CWordListDlg::~CWordListDlg()
{
}

void CWordListDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WORDS, m_listCtrlWords);
	DDX_Text(pDX, IDC_HAND_WORDS, m_strHandWords);
}


BEGIN_MESSAGE_MAP(CWordListDlg, CXTResizeDialog)
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
	ON_NOTIFY(NM_CLICK, IDC_WORDS, OnNMClickWords)
	ON_BN_CLICKED(IDC_CLEAR_WORDS, OnBnClickedClearWords)
	ON_BN_CLICKED(IDC_CLEAR_BOX, OnBnClickedClearBox)
END_MESSAGE_MAP()


// CWordListDlg message handlers

void CWordListDlg::OnBnClickedBrowse()
{
	CString filterStr = _T("Text files (*.txt)|*.txt|All files (*.*)|*.*||");
	CFileDialog fileDlg(true,_T("txt"),InputPath()+_T("\\*.txt"),OFN_ALLOWMULTISELECT,filterStr,this);
	_TCHAR filenameBuff[10001];
	filenameBuff[0] = '\0';
	fileDlg.m_ofn.lpstrFile = filenameBuff;
	fileDlg.m_ofn.nMaxFile = 10000;

	if (fileDlg.DoModal() == IDOK) {
		m_listCtrlWords.DeleteAllItems();
		CString filePathStr = fileDlg.GetPathName();
		CString contentStr = NLPReadFile(filePathStr);
		Tokenize(contentStr);
	}
}

void CWordListDlg::Tokenize(CString str)
{
	CStringList wordList;
	StringToTokenList(wordList,str);
	CString wordStr;
	CONCEPT *concept = NULL;

	POSITION pos = wordList.GetHeadPosition();
	m_intCount = 0;

	while (pos && (wordStr = wordList.GetNext(pos))) {
		if (wordStr.GetLength() <= 2)
			continue;
		m_listCtrlWords.InsertItem(m_intCount,wordStr);
		m_intCount++;
	}
}

BOOL CWordListDlg::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_WORDS, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BROWSE, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_APPEND, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_HAND_WORDS, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_DELETE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_ADD, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_CLEAR_WORDS, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_CLEAR_BOX, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_ADD_BY_HAND, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);

	m_intWordSelected = -1;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CWordListDlg::OnBnClickedOk()
{
	int count = m_listCtrlWords.GetItemCount();

	for (int i=0; i<count; i++)
		m_strListWords.AddTail(m_listCtrlWords.GetItemText(i,0));

	CDialog::OnOK();
}

void CWordListDlg::OnBnClickedDelete()
{
	int count = m_listCtrlWords.GetSelectedCount();
	CString strMsg;
	if (count == 1)
		strMsg.Format(_T("Do you want to delete the word \"%s\"?"),m_strWord);
	else
		strMsg.Format(_T("Do you want to delete the %d selected words?"),count);
	if (IDYES == AfxMessageBox(strMsg, MB_YESNO)) {
		if (count == 1)
			m_listCtrlWords.DeleteItem(m_intWordSelected);
		else {
			POSITION pos = m_listCtrlWords.GetFirstSelectedItemPosition();
			int nItem;
			while ((nItem = m_listCtrlWords.GetNextSelectedItem(pos)) >= 0)
				m_listCtrlWords.DeleteItem(nItem);
		}
	}
}

void CWordListDlg::OnBnClickedAdd()
{
	UpdateData(true);
	Tokenize(m_strHandWords);
}

void CWordListDlg::OnNMClickWords(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int itemNum = pNMLV->iItem;
	if (itemNum >= 0) {
		m_strWord = m_listCtrlWords.GetItemText(itemNum,0);
		m_intWordSelected = itemNum;
	}

	*pResult = 0;
}

void CWordListDlg::OnBnClickedClearWords()
{
	m_listCtrlWords.DeleteAllItems();
}

void CWordListDlg::OnBnClickedClearBox()
{
	UpdateData(true);
	m_strHandWords.Empty();
	UpdateData(false);
}
