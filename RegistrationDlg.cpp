/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// RegistrationDlg.cpp : implementation file
//

//#define _WIN32_WINNT 0x0400

#pragma pack(1)		// Required to ensure correct SMART IOCTL structure setup

#include "stdafx.h"
#include "Utils.h"
#include "kb.h"
#include "VisualText.h"
#include "FTPclient.h"
#include "RegistrationDlg.h"

extern CString appStrVersion;
extern CString appStrCopyright;
extern CFont appFontCopyright;
extern VT_APP_STATE appState;
extern CVisualTextApp theApp;

#define REG_DLG_FTP _T("ftp.textanalysis.com")
#define REG_DLG_URL "www.textanalysis.com"
#define REG_DLG_REMOTE_DIR _T("RegControl")
//#define REG_DLG_REMOTE_FILE "registry"
//#define REG_DLG_REMOTE_FILE "reg.users"
//#define REG_DLG_USERNAME "pftp1"
// #define REG_DLG_PASSWORD "TAIger1"
#define REG_ATTEMPTS_MAX 5

_TCHAR Reg_usr[32];	// 10/22/01 AM.
_TCHAR Reg_pzz[32];	// 10/22/01 AM.

_TCHAR *RegistrationTypes[] = {
	_T("unknown"),
	_T("new"),
	_T("counter"),
	_T("date"),
	_T("days"),
	_T("unlimited"),
	_T("expired"),
	_T("xxxx")
};


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegistrationDlg dialog


CRegistrationDlg::CRegistrationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegistrationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegistrationDlg)
	m_strCopyright = _T("");
	m_strMessage = _T("");
	m_strRegistration = _T("");
	m_strUsername = _T("");
	//}}AFX_DATA_INIT

	Init();
}

void CRegistrationDlg::Init()
{
	m_strCopyright = appStrVersion + _T(" - ") + appStrCopyright;
	m_strMessage = _T("");
	m_typeRegistration = REGISTRATION_TYPE_UNKNOWN;
	m_intCount = 0;
	m_intCounter = 0;
	m_intUser = 0;
	m_intAttempts = 0;
	m_boolInternetPresent = false;
	m_boolLoginSuccessful = false;

	m_regStatus = REG_INIT;

	// Keeping pzzwd out of executable strings.	// 10/22/01 AM.
	_TCHAR *ptr = Reg_pzz - 1;
	*++ptr = 'T';
	*++ptr = 'A';
	*++ptr = 'I';
	*++ptr = 'g';
	*++ptr = 'e';
	*++ptr = 'r';
	*++ptr = '1';
	*++ptr = '\0';

	ptr = Reg_usr - 1;
	*++ptr = 'p';
	*++ptr = 'f';
	*++ptr = 't';
	*++ptr = 'p';
	*++ptr = '1';
	*++ptr = '\0';

	size_t len;

#ifdef _UNICODE
	wchar_t vtpath[500]; 
	_wgetenv_s( &len, vtpath, 500, L"VISUALTEXT" ); 
#else
	_TCHAR *vtpath = NULL;
	_dupenv_s(&vtpath, &len, _T("VISUALTEXT"));
#endif

	m_usernameFilePathStr = vtpath;
	m_strRegLogFile = vtpath;	// 12/02/01 AM.
//	m_usernameFilePathStr += "\\data\\ics.dll";	// 11/21/01 AM.
	m_usernameFilePathStr += _T("\\bin\\ics.dll");	// 11/21/01 AM.
	m_strRegLogFile += _T("\\reg.log");	// 12/012/01 AM.
	NLPWriteFile(m_strRegLogFile,_T("Registration...\n"));	// 12/02/01 AM.
}


void CRegistrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegistrationDlg)
	DDX_Text(pDX, IDC_COPYRIGHT, m_strCopyright);
	DDX_Text(pDX, IDC_MESSAGE, m_strMessage);
	DDX_Text(pDX, IDC_REGISTRATION_NUMBER, m_strRegistration);
	DDX_Text(pDX, IDC_USERNAME, m_strUsername);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegistrationDlg, CDialog)
	//{{AFX_MSG_MAP(CRegistrationDlg)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegistrationDlg message handlers

void CRegistrationDlg::OnCancel() 
{
	AppendToFile(m_strRegLogFile,_T("OnCancel:\n"));	// 12/02/01 AM.

	if (appState == VT_APP_LOADING) {
		appState = VT_APP_STOP_COMMAND;
	}

	if (m_regStatus == REG_CONNECTING)
		KillTimers();

	CDialog::OnCancel();
}

CString CRegistrationDlg::ProfileGuide()
{
	CString outputStr = _T("");

	HW_PROFILE_INFO HwProfInfo;
	if (GetCurrentHwProfile(&HwProfInfo))	// 01/10/31 AM.
		outputStr = HwProfInfo.szHwProfileGuid;

//	return HwProfInfo.szHwProfileGuid;	// 10/22/01 AM.
	return outputStr;	// 10/22/01 AM.
}

void CRegistrationDlg::CheckRegistration()
{
	AppendToFile(m_strRegLogFile,_T("CheckRegistration:\n"));	// 12/02/01 AM.

	if ((m_boolInternetPresent = IsInternetConnected(REG_DLG_URL) ? true : false)) {

		AppendToFile(m_strRegLogFile,_T("CkReg: Connected.\n"));	// 12/02/01 AM.

		CString userProfileStr = _T("");	// 11/21/01 AM.

		if (m_strUniquePCID != _T("")) {	// 11/21/01 AM.
			m_boolLoginSuccessful = GetRegList();

			userProfileStr = IsInRegList(m_strUniquePCID);
		}
		else	// 11/21/01 AM.
			m_boolLoginSuccessful = true;	// 11/21/01 AM.

		if (userProfileStr == _T(""))
			m_typeRegistration = REGISTRATION_TYPE_NEW;
		else {
			ParseUserProfileStr(userProfileStr);
			DeactivateFields();
		}
		m_regStatus = REG_CONNECTED;
	}
	else
		m_regStatus = REG_FAILED;
	
	MessageFromType();
}

void CRegistrationDlg::DeactivateFields(bool okButtonFlag)
{
	AppendToFile(m_strRegLogFile,_T("DeactivateFields:\n"));	// 12/02/01 AM.

	CEdit *ed = (CEdit *)GetDlgItem(IDC_USERNAME);
	ed->EnableWindow(false);

	ed = (CEdit *)GetDlgItem(IDC_REGISTRATION_NUMBER);
	ed->EnableWindow(false);

	if (okButtonFlag) {
		CWnd *okButton = GetDlgItem(IDOK);
		okButton->EnableWindow(false);
	}
}

void CRegistrationDlg::ActivateFields()
{
	AppendToFile(m_strRegLogFile,_T("ActivateFields:\n"));	// 12/02/01 AM.

	CEdit *ed = (CEdit *)GetDlgItem(IDC_USERNAME);
	ed->EnableWindow(true);

	ed = (CEdit *)GetDlgItem(IDC_REGISTRATION_NUMBER);
	ed->EnableWindow(true);

	CWnd *okButton = GetDlgItem(IDOK);
	okButton->EnableWindow(true);
}

bool CRegistrationDlg::GetRegList()
{
	AppendToFile(m_strRegLogFile,_T("GetRegList:\n"));	// 12/02/01 AM.

	CString contentStr = _T("");

	if (m_strRegDlgRemoteFile == _T(""))	// 11/22/01 AM.
		return false;	// 11/22/01 AM.

	bool returnValue =
		GetRemoteFileContent(REG_DLG_FTP,REG_DLG_REMOTE_DIR,
//			REG_DLG_REMOTE_FILE,REG_DLG_USERNAME,
//			REG_DLG_PASSWORD,contentStr,m_strTimestamp);
			m_strRegDlgRemoteFile,Reg_usr,	// 11/21/01 AM.
			Reg_pzz,contentStr,m_strTimestamp);	// 10/22/01 AM.
	StringToStringList(m_strListReg,contentStr,_T("\r\n"));
	if (returnValue)	// 12/02/01 AM.
		AppendToFile(m_strRegLogFile,_T("GetRegList: Got regfile2\n"));	// 12/02/01 AM.
	else	// 12/02/01 AM.
		AppendToFile(m_strRegLogFile,_T("GetRegList: No regfile2\n"));	// 12/02/01 AM.
		
	return returnValue;
}

CString CRegistrationDlg::IsInRegList(const CString &pcIDStr)
{
	AppendToFile(m_strRegLogFile,_T("IsInRegList:\n"));	// 12/02/01 AM.

	if (pcIDStr == _T(""))
		return _T("");

	POSITION pos = m_strListReg.GetHeadPosition();
	CString userProfileStr = _T("");
	CString idStr,nameStr;
	m_intUser = 0;

	while (pos && (userProfileStr = m_strListReg.GetNext(pos))) {
		if (userProfileStr.GetAt(0) == '#')
			continue;
		m_intUser++;
		NthPathSegment(userProfileStr,idStr,'\t',0);
		NthPathSegment(userProfileStr,nameStr,'\t',2);
		if (idStr == pcIDStr) {
			AppendToFile(m_strRegLogFile,_T("IsInRegList: Got profile.\n"));	// 12/02/01 AM.
			return userProfileStr;
		}
	}

	AppendToFile(m_strRegLogFile,_T("IsInRegList: No profile.\n"));	// 12/02/01 AM.

	m_intUser = 0;

	return _T("");
}

int CRegistrationDlg::HasUsernameRegistration(const CString &usernameStr, const CString &registrationStr)
{
	AppendToFile(m_strRegLogFile,_T("HasUsernameRegistration:\n"));	// 12/02/01 AM.

	POSITION pos = m_strListReg.GetHeadPosition();
	CString userProfileStr = _T("");
	CString itemUserNameStr;
	CString itemRegistrationStr;
	m_intUser = 0;
	bool found = false;

	while (pos && (userProfileStr = m_strListReg.GetNext(pos))) {
		if (userProfileStr.GetAt(0) == '#')
			continue;
		m_intUser++;
		NthPathSegment(userProfileStr,itemUserNameStr,'\t',2);
		NthPathSegment(userProfileStr,itemRegistrationStr,'\t',3);

		if (usernameStr == itemUserNameStr && registrationStr == itemRegistrationStr) {
			ParseUserProfileStr(userProfileStr);
			found = true;
			break;
		}
	}

	if (!found) {
		m_strMessage = _T("Username and Registration not valid.  To activate your copy, please enter your username and registration.");
		m_intUser = 0;
	}
	return m_intUser;
}

void CRegistrationDlg::ParseUserProfileStr(const CString &userProfileStr)
{
	AppendToFile(m_strRegLogFile,_T("ParseUserProfileStr:\n"));	// 12/02/01 AM.

	// GET REGISTRATION TYPE FIRST
	NthPathSegment(userProfileStr,m_strRegistrationType,'\t',4);
	m_typeRegistration = RegistrationType(m_strRegistrationType);

	NthPathSegment(userProfileStr,m_strTimestampLast,'\t',1);
	if (m_typeRegistration == REGISTRATION_TYPE_DAYS) {
		if (m_strTimestampLast == _T(""))
			m_strExpirationDate = m_strTimestamp;
		else
			m_strExpirationDate = m_strTimestampLast;
	}

	NthPathSegment(userProfileStr,m_strUsername,'\t',2);
	NthPathSegment(userProfileStr,m_strRegistration,'\t',3);

	if (m_typeRegistration == REGISTRATION_TYPE_COUNTER) {
		NthPathSegmentInt(userProfileStr,m_intCount,'\t',5);
		NthPathSegmentInt(userProfileStr,m_intCounter,'\t',6);
	}
	else if (m_typeRegistration == REGISTRATION_TYPE_DAYS) {
		NthPathSegmentInt(userProfileStr,m_intCount,'\t',5);
	}
	else if (m_typeRegistration == REGISTRATION_TYPE_DATE) {
		NthPathSegment(userProfileStr,m_strExpirationDate,'\t',5);
	}
}

void CRegistrationDlg::OnOK() 
{
	AppendToFile(m_strRegLogFile,_T("OnOK:\n"));	// 12/02/01 AM.

	bool notOk = false;

	StartWaitCursor();
	REGISTRATION_TYPE lastType = m_typeRegistration;

	UpdateData(true);


//	if (!m_boolInternetPresent || !m_boolLoginSuccessful) {	// 12/02/01 AM.
	if (!m_boolInternetPresent) {	// 12/02/01 AM.
		DeactivateFields(true);
		CheckRegistration();
		return;
	}

	// Get registration file based on username-reg num.	// 11/21/01 AM.
	if (m_strUniquePCID == _T("") && m_strUsername != _T("")) {	// 11/21/01 AM.
			m_strUniquePCID = m_strUsername + m_strRegistration;	// 11/21/01 AM.
			m_strRegDlgRemoteFile = m_strUniquePCID;	// 11/22/01 AM.
//			NLPWriteFile(m_usernameFilePathStr,m_strUniquePCID);	// 12/02/01 AM.

			m_boolLoginSuccessful = GetRegList();	// 11/21/01 AM.

			// Only write the reg file if user-supplied information is ok!
			if (m_boolLoginSuccessful)	// 12/02/01 AM.
				NLPWriteFile(m_usernameFilePathStr,m_strUniquePCID);// 12/02/01 AM.

			CString userProfileStr = IsInRegList(m_strUniquePCID); // 11/21/01 AM.
			if (userProfileStr != _T("")) {	// 11/21/01 AM.
				ParseUserProfileStr(userProfileStr);	// 11/21/01 AM.
				DeactivateFields();	// 11/21/01 AM.
			}
			else {	// 12/02/01 AM.
				m_strUniquePCID = _T("");	// Zero this out.	// 12/02/01 AM.
			}
	}

	if (m_typeRegistration == REGISTRATION_TYPE_NEW &&
		!HasUsernameRegistration(m_strUsername,m_strRegistration)) {

		WindowBell();

		if (++m_intAttempts >= REG_ATTEMPTS_MAX) {
			m_strMessage = _T("Too many attempts.  Visual Text is exiting.");
			UpdateData(false);
			Sleep(5000);
			OnCancel();
		}
		else
			UpdateData(false);

		notOk = true;
	}

	if (m_typeRegistration == REGISTRATION_TYPE_EXPIRED) {
		OnCancel();
		notOk = true;
	}
	else if (m_typeRegistration == REGISTRATION_TYPE_COUNTER) {

		if (m_intCounter == 0) {
			MessageFromType();
			DeactivateFields();
			notOk = true;
		}
		else if (lastType == REGISTRATION_TYPE_NEW) {
			WindowBell();
			MessageFromType();
			DeactivateFields();
			notOk = true;
		}
		else
			m_intCounter--;
	}
	else if (m_typeRegistration == REGISTRATION_TYPE_DAYS) {

		CString daysLeft = DaysLeftPrettyStr(m_strTimestamp,m_strExpirationDate,m_intCount);

		if (daysLeft == _T("")) {
			MessageFromType();
			DeactivateFields();
			notOk = true;
		}
		else if (lastType == REGISTRATION_TYPE_NEW) {
			WindowBell();
			MessageFromType();
			DeactivateFields();
			notOk = true;
		}
	}
	else if (m_typeRegistration == REGISTRATION_TYPE_DATE) {

		CString diffStr = TimeLeftPrettyStr(m_strTimestamp,m_strExpirationDate);

		if (diffStr == _T("")) {
			MessageFromType();
			DeactivateFields();
			notOk = true;
		}
		else if (lastType == REGISTRATION_TYPE_NEW) {
			WindowBell();
			MessageFromType();
			DeactivateFields();
			notOk = true;
		}
	}

	if (notOk) {
		StopWaitCursor();
		return;
	}

	WriteListToServer();
	
	StopWaitCursor();

	CDialog::OnOK();
}

void CRegistrationDlg::MessageFromType()
{
	AppendToFile(m_strRegLogFile,_T("MessageFromType:\n"));	// 12/02/01 AM.

	bool expired = false;

	if (!m_boolInternetPresent) {
		WindowBell();
		m_strMessage.Format(_T("You must have an active internet connection to use this evaluation copy of Visual Text. Check your internet connection and then click on Ok to try again or Cancel to exit."));
	}

	else if (!m_boolLoginSuccessful) {
		WindowBell();
		m_strMessage.Format(_T("The TAI server or user file is not responding. Please try again or contact Text Analysis International, Inc."));
	}

	else if (m_typeRegistration == REGISTRATION_TYPE_EXPIRED)
		expired = true;

	else if (m_typeRegistration == REGISTRATION_TYPE_COUNTER) {
		if (m_intCounter == 0)
			expired = true;
		else if (m_intCounter > 1)
			m_strMessage.Format(_T("You have %d uses left out of %d for this evaluation copy of Visual Text."),m_intCounter,m_intCount);
		else
			m_strMessage.Format(_T("You have %d use left out of %d for this evaluation copy of Visual Text."),m_intCounter,m_intCount);
	}

	else if (m_typeRegistration == REGISTRATION_TYPE_UNLIMITED)
		m_strMessage = _T("You are free to use this evaluation version of Visual Text.");

	else if (m_typeRegistration == REGISTRATION_TYPE_DATE) {
		CString diffStr = TimeLeftPrettyStr(m_strTimestamp,m_strExpirationDate);
		m_strMessage.Format(_T("You have %s left to use Visual Text."),diffStr);
		if (diffStr == _T(""))
			expired = true;
	}
	else if (m_typeRegistration == REGISTRATION_TYPE_DAYS) {
		CString daysLeftStr = DaysLeftPrettyStr(m_strTimestamp,m_strExpirationDate,m_intCount);
		m_strMessage.Format(_T("You have %s left out of %d to use Visual Text."),daysLeftStr,m_intCount);
		if (daysLeftStr == _T(""))
			expired = true;
	}

	else if (m_typeRegistration == REGISTRATION_TYPE_NEW)
		m_strMessage = _T("This is an evaluation copy of Visual Text. To activate your copy, please enter your username and registration."); 


	if (expired) {
		WindowBell();
		m_strMessage = _T("Your evaluation copy has expired. Please contact Text Analysis International, Inc. if you are interested in purchasing working copy of Visual Text.");
		m_typeRegistration = REGISTRATION_TYPE_EXPIRED;
		CWnd *butt = (CEdit *)GetDlgItem(IDOK);
		butt->EnableWindow(false);
	}
	else if (m_typeRegistration != REGISTRATION_TYPE_NEW &&
			 m_typeRegistration != REGISTRATION_TYPE_UNKNOWN)
		m_strMessage = m_strMessage + _T("\nClick Ok to start VisualText or Cancel to exit.");

	if (!expired) {
		CWnd *butt = (CEdit *)GetDlgItem(IDOK);
		butt->EnableWindow(true);
	}

	UpdateData(false);
}

BOOL CRegistrationDlg::OnInitDialog() 
{
	AppendToFile(m_strRegLogFile,_T("OnInitDialog:\n"));	// 12/02/01 AM.

	CDialog::OnInitDialog();
	
	DeactivateFields(true);
	m_strMessage = _T("Attempting internet connection for trial version control...");

	GetDlgItem(IDC_COPYRIGHT)->SetFont(&appFontCopyright,true);

	CEdit *ed = (CEdit *)GetDlgItem(IDC_USERNAME);
	ed->SetFocus();

	FetchID();

	m_nTimerCheckRegistration = SetTimer(ID_TIMER_CHECK_REGISTRATION,75,NULL);

	UpdateData(false);

	return false;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRegistrationDlg::FetchID()
{
	AppendToFile(m_strRegLogFile,_T("FetchID:\n"));	// 12/02/01 AM.

	//m_strUniquePCID = ProfileGuide();
	m_strUniquePCID = _T("");

	if (m_strUniquePCID == _T("")) {
		if (FileExists(m_usernameFilePathStr)) {
			m_strUniquePCID = NLPReadFile(m_usernameFilePathStr);
			AppendToFile(m_strRegLogFile,_T("FetchID: Got regfile1.\n"));// 12/02/01 AM.
		}
	}

	// Each user gets his own registry file on server.			// 11/21/01 AM.
	m_strRegDlgRemoteFile = m_strUniquePCID;	// Same name!	// 11/21/01 AM.
}

REGISTRATION_TYPE CRegistrationDlg::RegistrationType(const CString &registrationStr)
{
	AppendToFile(m_strRegLogFile,_T("RegistrationType:\n"));	// 12/02/01 AM.

	CString locAlgoStr = registrationStr ; 

	REGISTRATION_TYPE type = REGISTRATION_TYPE_UNKNOWN;
	CString aStr;

	for (int i=0; i < 1000; i++) {
		aStr = RegistrationTypes[i];
		if (aStr == _T("xxxx"))
			break;
		else if (aStr.CompareNoCase(registrationStr) == 0) {
			type = (REGISTRATION_TYPE)i;
			break;
		}
	}

	return type;
}

CString CRegistrationDlg::RegistrationTypeStr(const REGISTRATION_TYPE type)
{
	CString aStr;

	for (int i=0; i < 1000; i++) {
		aStr = RegistrationTypes[i];
		if (type == (REGISTRATION_TYPE)i)
			break;
	}

	return aStr;
}

void CRegistrationDlg::WriteListToServer()
{
	AppendToFile(m_strRegLogFile,_T("WriteListToServer:\n"));	// 12/02/01 AM.

	POSITION pos = m_strListReg.GetHeadPosition();
	CString userProfileStr = _T("");
	CString contentStr = _T("");
//	int counter = 1;	// BUG.	// 12/02/01 AM.
	int counter = 0;	// Profile could be 1st line of file.	// 12/02/01 AM.

	while (pos && (userProfileStr = m_strListReg.GetNext(pos))) {
		if (contentStr != _T(""))
			contentStr += _T("\r\n");

		if (userProfileStr.GetAt(0) == '#') {
			contentStr += userProfileStr;
			continue;
		}
		else if (counter == m_intUser)
			contentStr += UserProfileStr();
		else
			contentStr += userProfileStr;

		counter++;
	}
	if (contentStr != _T("")) {
		CString commentStr;	// 11/22/01 AM.
		commentStr.Format(_T("\r\n# %s"), m_strTimestamp);	// 11/22/01 AM.
		contentStr += commentStr;	// 11/22/01 AM.
		SetRemoteFileContent(REG_DLG_FTP,REG_DLG_REMOTE_DIR,
//			REG_DLG_REMOTE_FILE,REG_DLG_USERNAME,
//			REG_DLG_PASSWORD,contentStr);
			m_strRegDlgRemoteFile,Reg_usr,	// 11/21/01 AM.
			Reg_pzz,contentStr);	// 10/22/01 AM.
	}
}

CString CRegistrationDlg::UserProfileStr()
{
	AppendToFile(m_strRegLogFile,_T("UserProfileStr:\n"));	// 12/02/01 AM.

	CString userProfileStr;

	if (m_strUniquePCID == _T("")) {
		m_strUniquePCID = m_strUsername + m_strRegistration;
		NLPWriteFile(m_usernameFilePathStr,m_strUniquePCID);
	}

	switch (m_typeRegistration) {

	case REGISTRATION_TYPE_COUNTER:
		userProfileStr.Format(_T("%s\t%s\t%s\t%s\t%s\t%d\t%d"),
			m_strUniquePCID,
			m_strTimestamp,
			m_strUsername,
			m_strRegistration,
			RegistrationTypeStr(m_typeRegistration),
			m_intCount,
			m_intCounter
			);
		break;

	case REGISTRATION_TYPE_DATE:
		userProfileStr.Format(_T("%s\t%s\t%s\t%s\t%s\t%s"),
			m_strUniquePCID,
			m_strTimestamp,
			m_strUsername,
			m_strRegistration,
			RegistrationTypeStr(m_typeRegistration),
			m_strExpirationDate
			);
		break;

	case REGISTRATION_TYPE_DAYS:
		if (m_strUniquePCID && m_strExpirationDate)	// 12/02/01 AM.
			AppendToFile(m_strRegLogFile,_T("UserProfileStr: Days ok.\n"));	// 12/02/01 AM.
		else	// 12/02/01 AM.
			AppendToFile(m_strRegLogFile,_T("UserProfileStr: Days not ok.\n"));	// 12/02/01 AM.
		userProfileStr.Format(_T("%s\t%s\t%s\t%s\t%s\t%d"),
			m_strUniquePCID,
			m_strExpirationDate,
			m_strUsername,
			m_strRegistration,
			RegistrationTypeStr(m_typeRegistration),
			m_intCount
			);
		break;

	case REGISTRATION_TYPE_UNLIMITED:
		userProfileStr.Format(_T("%s\t%s\t%s\t%s\t%s"),
			m_strUniquePCID,
			m_strTimestamp,
			m_strUsername,
			m_strRegistration,
			RegistrationTypeStr(m_typeRegistration)
			);
		break;
	}
	return userProfileStr;
}

CString CRegistrationDlg::TimeLeftPrettyStr(CString dateStr1, CString dateStr2)
{
	int year1,month1,day1,hour1,minute1;
	int year2,month2,day2,hour2,minute2;

	ParseDateStr(dateStr1,year1,month1,day1,hour1,minute1);
	ParseDateStr(dateStr2,year2,month2,day2,hour2,minute2);

	CTime time1(year1,month1,day1,hour1,minute1,00);
	CTime time2(year2,month2,day2,hour2,minute2,00);
	CTimeSpan timeDiff = time2 - time1;

	CString prettyStr = _T("");

	if (time2 > time1) {
		AppendPrettyDate(prettyStr,(int)timeDiff.GetDays(),_T("day"));
		AppendPrettyDate(prettyStr,(int)timeDiff.GetHours(),_T("hour"));
		AppendPrettyDate(prettyStr,(int)timeDiff.GetMinutes(),_T("minute"));
	}

	return prettyStr;
}

CString CRegistrationDlg::DaysLeftPrettyStr(CString dateStr1, CString dateStr2, int dayCount)
{
	int year1,month1,day1,hour1,minute1;
	int year2,month2,day2,hour2,minute2;

	ParseDateStr(dateStr1,year1,month1,day1,hour1,minute1);
	ParseDateStr(dateStr2,year2,month2,day2,hour2,minute2);

	CTime time1(year1,month1,day1,hour1,minute1,00);
	CTime time2(year2,month2,day2,hour2,minute2,00);
	CTimeSpan timeDiff = time2 - time1;
	int daysLeft = dayCount + (int)timeDiff.GetDays();

	CString prettyStr = _T("");

	if (daysLeft > 0)
		AppendPrettyDate(prettyStr,daysLeft,_T("day"));

	return prettyStr;
}

void CRegistrationDlg::AppendPrettyDate(CString &prettyStr, int date, CString dateStr)
{
	CString formatStr;

	if (date) {

		if (prettyStr != _T(""))
			prettyStr += _T(", ");

		formatStr.Format(_T("%d %s"),date,dateStr);
		prettyStr += formatStr;

		if (date > 1)
			prettyStr += _T("s");
	}
}

void CRegistrationDlg::ParseDateStr(CString dateStr, int &year, int &month,
									int &day, int &hour, int &minute)
{
	CString pieceStr;

	pieceStr = dateStr.Mid(0,4);
	year = _ttoi((const _TCHAR *)pieceStr);

	pieceStr = dateStr.Mid(4,2);
	month = _ttoi((const _TCHAR *)pieceStr);

	pieceStr = dateStr.Mid(6,2);
	day = _ttoi((const _TCHAR *)pieceStr);

	pieceStr = dateStr.Mid(8,2);
	hour = _ttoi((const _TCHAR *)pieceStr);

	pieceStr = dateStr.Mid(10,2);
	minute = _ttoi((const _TCHAR *)pieceStr);
}

void CRegistrationDlg::OnTimer(UINT nIDEvent) 
{
	AppendToFile(m_strRegLogFile,_T("OnTimer:\n"));	// 12/02/01 AM.

	switch (nIDEvent) {
		case ID_TIMER_CHECK_REGISTRATION:
			if (m_regStatus == REG_INIT) {
				m_regStatus = REG_CONNECTING;
				CheckRegistration();
			}
			else if (m_regStatus == REG_CONNECTING)
// CRASHES WIN2000 Fix due to Paul.	// 10/19/01 AM.
//				PeekHere(10);	// 10/19/01 AM.
				;	// 10/19/01 AM.
			else if (m_regStatus == REG_CONNECTED || m_regStatus == REG_FAILED)
				KillTimers();
				if (m_typeRegistration == REGISTRATION_TYPE_NEW)
					ActivateFields();
				break;
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CRegistrationDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	AppendToFile(m_strRegLogFile,_T("OnDestroy:\n"));	// 12/02/01 AM.
	KillTimers();
}

void CRegistrationDlg::KillTimers() 
{
	AppendToFile(m_strRegLogFile,_T("KillTimers:\n"));	// 12/02/01 AM.
	if (m_nTimerCheckRegistration) {
		KillTimer(m_nTimerCheckRegistration);
		m_nTimerCheckRegistration = 0;
	}	
}
