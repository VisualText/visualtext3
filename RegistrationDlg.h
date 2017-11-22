/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_REGISTRATIONDLG_H__88C484A1_1A4B_11D5_A844_00105A99059F__INCLUDED_)
#define AFX_REGISTRATIONDLG_H__88C484A1_1A4B_11D5_A844_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegistrationDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegistrationDlg dialog

enum REGISTRATION_TYPE {
	REGISTRATION_TYPE_UNKNOWN = 0,
	REGISTRATION_TYPE_NEW,
	REGISTRATION_TYPE_COUNTER,
	REGISTRATION_TYPE_DATE,
	REGISTRATION_TYPE_DAYS,
	REGISTRATION_TYPE_UNLIMITED,
	REGISTRATION_TYPE_EXPIRED,
};

enum REG_STATUS {
	REG_INIT = 0,
	REG_CONNECTING,
	REG_CONNECTED,
	REG_FAILED
};

#define ID_TIMER_CHECK_REGISTRATION 200

class CRegistrationDlg : public CDialog
{
// Construction
public:
	CRegistrationDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegistrationDlg)
	enum { IDD = IDD_REGISTRATION_DLG };
	CString	m_strCopyright;
	CString	m_strMessage;
	CString	m_strRegistration;
	CString	m_strUsername;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegistrationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegistrationDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	REGISTRATION_TYPE m_typeRegistration;
	CString m_strUniquePCID;
	CString m_strRegistrationType;
	CString m_strTimestamp;
	CString m_strTimestampLast;
	CString m_strExpirationDate;
	CString m_usernameFilePathStr;
	CString m_strRegDlgRemoteFile;	// 11/21/01 AM.
	CString m_strRegLogFile;			// 12/02/01 AM.
	CStringList m_strListReg;
	int m_intCount;
	int m_intCounter;
	int m_intUser;
	int m_intAttempts;
	bool m_boolLoginSuccessful;
	bool m_boolInternetPresent;
	UINT m_nTimerCheckRegistration;
	REG_STATUS m_regStatus;

	void Init();
	CString ProfileGuide();
	bool GetRegList();
	CString IsInRegList(const CString &pcIDStr);
	void CheckRegistration();
	int HasUsernameRegistration(const CString &usernameStr, const CString &registrationStr);
	REGISTRATION_TYPE RegistrationType(const CString &registrationStr);
	CString RegistrationTypeStr(const REGISTRATION_TYPE type);
	void ParseUserProfileStr(const CString &cont);
	void DeactivateFields(bool okButtonFlag=false);
	void ActivateFields();
	CString UserProfileStr();
	void WriteListToServer();
	void MessageFromType();
	void ParseDateStr(CString dateStr, int &year, int &month,
		int &day, int &hour, int &minute);
	CString TimeLeftPrettyStr(CString dateStr1, CString dateStr2);
	CString DaysLeftPrettyStr(CString dateStr1, CString dateStr2, int dayCount=0);
	void AppendPrettyDate(CString &prettyStr, int date, CString dateStr);
	void KillTimers();
	void FetchID();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTRATIONDLG_H__88C484A1_1A4B_11D5_A844_00105A99059F__INCLUDED_)
