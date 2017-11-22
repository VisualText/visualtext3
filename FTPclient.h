/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// FTPclient.h: interface for the CFTPclient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTPCLIENT_H__05003AE0_E234_11D2_970B_00A024EBF6AB__INCLUDED_)
#define AFX_FTPCLIENT_H__05003AE0_E234_11D2_970B_00A024EBF6AB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxsock.h>

enum FTP_TRANS_TYPE {
	FTP_TRANS_ASCII = 0,
	FTP_TRANS_BINARY,
};

bool GetServerArchiveDirectoryString(const CString &archiveDirectoryStr);
bool UploadArchive(const CString &archiveDirectoryStr, const CString &remoteFileStr, const CString &localFileStr);
bool DownloadArchive(const CString &archiveDirectoryStr, const CString &remoteFileStr, const CString &localFileStr, const long fileSize);
bool DeleteArchive(const CString &archiveDirectoryStr, const CString &remoteFileStr);
bool RenameArchive(const CString &archiveDirectoryStr, const CString &remoteFileStr, const CString &newRemoteFileStr);
bool GetRemoteFileContent(const CString &ftpStr, const CString &localDirectoryStr, const CString &remoteFileStr,
						  const CString &usernameStr, const CString &passwordStr, CString &contentStr,
						  CString &timestampStr);
bool SetRemoteFileContent(const CString &ftpStr, const CString &localDirectoryStr, const CString &remoteFileStr,
						  const CString &usernameStr, const CString &passwordStr, CString &contentStr);
BOOL IsInternetConnected(const CString &serverPathStr=_T(""));

class CFTPclient
{
public:
	CString m_retmsg;
	int m_fc;
	BOOL MoveFile(const CString &remotefile, const CString &localfile, const FTP_TRANS_TYPE &transType,
		const BOOL pasv, const long fileSize);
	BOOL GetFileStr(const CString &remotefile, CString &contentStr,
				    const FTP_TRANS_TYPE &transType, const BOOL pasv);
	BOOL CFTPclient::DirectoryListing(CString &dirStr, const bool shortList=FALSE);
	void LogOffServer();
	BOOL LogOnToServer(CString hostname,const int hostport,const CString &username, const CString &password, const CString &acct, const CString &fwhost,const CString &fwusername, const CString &fwpassword,const int fwport,const int logontype) ;
	CFTPclient();
	~CFTPclient();
	BOOL FTPcommand(const CString &command);
	BOOL ReadStr();
	BOOL WriteStr(const CString &outputstring);
	bool LoginConnectToArchiveServer(const CString &archiveDirectoryStr);
	bool LoginConnectToServer(const CString &ftpStr, const CString &dirPathStr, const CString &usernameStr,
							  const CString &passwordStr, const bool noWarnings=false);
	bool DeleteCommand(const CString &remoteFileStr);
	BOOL SetFileStr(const CString &remotefile, CString &contentStr,
					  const FTP_TRANS_TYPE &transType, const BOOL pasv);
	bool MakeSock(CSocket*);	// 10/17/01 AM.
	bool SetSockFile(CSocketFile*);	// 10/17/01 AM.

private:
//	bool bopened;	// Obsolete.	// 11/03/01 AM.
	bool bnewopen;	// 10/17/01 AM.
	CArchive* m_pCtrlRxarch;
	CArchive* m_pCtrlTxarch;
	CSocketFile* m_pCtrlsokfile;
	CSocket* m_Ctrlsok;
	BOOL ReadStr2();
	BOOL OpenControlChannel(const CString &serverhost,const int serverport);
	void CloseControlChannel();
	BOOL OpenCtrlChannel(const CString &,const int);	// 10/17/01 AM.
	void CloseCtrlChannel();	// 10/17/01 AM.

protected:

};


#endif // !defined(AFX_FTPCLIENT_H__05003AE0_E234_11D2_970B_00A024EBF6AB__INCLUDED_)
