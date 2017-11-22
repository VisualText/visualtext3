#pragma once

// CPassTemplateEditorDlg dialog

#define PASS_TEMPLATE_FILENAME _T("passtemplate.pat")

class CPassTemplateEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(CPassTemplateEditorDlg)

public:
	CPassTemplateEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPassTemplateEditorDlg();

// Dialog Data
	enum { IDD = IDD_PASS_TEMPLATE_EDITOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDatetimeButtton();
	afx_msg void OnBnClickedAuthorButton();
	afx_msg void OnBnClickedPassnameButton();
	afx_msg void OnBnClickedFilePathDir();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CString m_strTemplate;
	CString m_strPathDir;
	afx_msg void OnClose();
	CString DefaultFileContent(void);
	afx_msg void OnBnClickedYearButton();
	void Close(void);
private:
	void InsertTag(CString tagStr);
};
