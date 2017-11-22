#pragma once
#include "afxwin.h"


// CEnglishInflectionsDlg dialog

class CEnglishInflectionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CEnglishInflectionsDlg)

public:
	CEnglishInflectionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEnglishInflectionsDlg();
	bool HasPOS(CString posStr);
	void AddPOS(CString posStr);


// Dialog Data
	enum { IDD = IDD_ENGLISH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void FillPOS();
	void UpdateItemsActivitation();
	void SetEnable(UINT id, bool value);
	void UpdatePOSStringList();

	DECLARE_MESSAGE_MAP()
public:
	CString m_strWord;
	CListBox m_listBoxPOS;
	CString m_strPlural;
	CString m_strPast;
	CString m_strGerund;
	CStringList m_strListPOS;
	afx_msg void OnBnClickedGuess();
	afx_msg void OnBnClickedClear();
	afx_msg void OnLbnSelchangePos();
	afx_msg void OnClose();
	BOOL m_boolYPlural;
	afx_msg void OnBnClickedY();
};
