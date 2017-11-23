#pragma once
#include "afxcmn.h"


// CWordListDlg dialog

class CWordListDlg : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CWordListDlg)

public:
	CWordListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWordListDlg();

// Dialog Data
	enum { IDD = IDD_WORD_LIST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strWord;
	int m_intCount;
	int m_intWordSelected;
	CStringList m_strListWords;
	void Tokenize(CString str);

	afx_msg void OnBnClickedBrowse();
	CListCtrl m_listCtrlWords;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnNMClickWords(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedClearWords();
	afx_msg void OnBnClickedClearBox();
	CString m_strHandWords;
};
