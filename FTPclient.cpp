/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

/*/////////////////////////////////////////////////////////////////////
FTPclient.cpp (c) GDI 1999
V1.0.0 (10/4/99)
Phil Anderson. philip@gd-ind.com

Simple FTP client functionality. If you have any problems with it,
please tell me about them (or better still e-mail me the fixed
code). Please feel free to use this code however you wish, although
if you make changes please put your name in the source & comment what
you did.

Nothing awesome going on here at all (all sockets are used in
synchronous blocking mode), but it does the following
things WinInet doesn't seem to:
* Supports loads of different firewalls (I think, I don't
  have access to all types so they haven't all been fully
  tested yet)
* Allows you to execute any command on the FTP server
* Adds 10K to your app install rather than 1Mb #;-)

Functions return true if everything went OK, false if there was an,
error. A message describing the outcome (normally the one returned
from the server) will be in m_retmsg on return from the function.
There are a few error msgs in the app's string table that you'll
need to paste into your app, along with this file & FTPclient.h

If you created your app without checking the "Use Windows Sockets"
checkbox in AppWizard, you'll need to add the following bit of code
to you app's InitInstance()

if(!AfxSocketInit())
{
	AfxMessageBox("Could not initialize Windows Sockets!");
	return false;
}

To use:

1/ Create an object of CFTPclient.

2/ Use LogOnToServer() to connect to the server. Any arguments
not used (e.g. if you're not using a firewall), pass an empty
string or zero for numeric args. You must pass a server port
number, use the FTP default of 21 if you don't know what it is.

3/ Use MoveFile() to upload/download a file, 1st arg is local file
path, 2nd arg is remote file path, 3rd arg is true for a PASV
connection (required by some firewalls), false otherwise, 4th arg
is true to upload, false to download file. MoveFile only works in
synchronous mode (ie the function will not return 'till the transfer
is finished). File transfers are always of type BINARY.

4/ You can use FTPcommand() to execute FTP commands (eg
FTPcommand("CWD /home/mydir") to change directory on the server),
note that this function will return false unless the server response
is a 200 series code. This should work fine for most FTP commands, 
otherwise you can use WriteStr() and ReadStr() to send commands & 
interpret the response yourself. Use LogOffServer() to disconnect
when done.

/////////////////////////////////////////////////////////////////////*/

#include "stdafx.h"
#include "Resource.h"
#include "kb.h"
#include "Utils.h"
#include "ArchiveProgressDialog.h"
#include "MainFrm.h"
#include "FTPclient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CString appStrArchiveFTP;
extern CString appStrArchiveDirectoryPath;
extern CString appStrArchiveUsername;
extern CString appStrArchivePassword;
extern CString appAskText;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFTPclient::CFTPclient()
{
	m_pCtrlsokfile=NULL;
	m_pCtrlTxarch=NULL;
	m_pCtrlRxarch=NULL;
	m_Ctrlsok=NULL;
//	bopened = false;		// 11/03/01 AM.
	bnewopen = false;		// 10/17/01 AM.
}


CFTPclient::~CFTPclient()
{
//if (bopened)	// Obsolete.	// 11/03/01 AM.
//	CloseControlChannel();	// Obsolete.	// 11/03/01 AM.
}


//////////////////////////////////////////////////////////////////////
// Public Functions
//////////////////////////////////////////////////////////////////////

// Set socket var.	// 10/17/01 AM.
bool CFTPclient::MakeSock(CSocket *ptr)	// 10/17/01 AM.
{
	if (!ptr) return false;	// 10/17/01 AM.
	m_Ctrlsok = ptr;	// 10/17/01 AM.
	if (!(m_Ctrlsok->Create())) return false;	// 10/17/01 AM.
	return true;	// 10/17/01 AM.
}

// Set socket file var.	// 10/17/01 AM.
bool CFTPclient::SetSockFile(CSocketFile *ptr)	// 10/17/01 AM.
{
	if (!ptr) return false;	// 10/17/01 AM.
	m_pCtrlsokfile = ptr;	// 10/17/01 AM.
	return true;	// 10/17/01 AM.
}

// function to connect & log on to FTP server
//Input functions set as const & wherever possible
//PD 02/14/2001
BOOL CFTPclient::LogOnToServer(CString hostname,const int hostport,const CString &username, const CString &password, const CString &acct, const CString &fwhost,const CString &fwusername, const CString &fwpassword,const int fwport,const int logontype) {
	int port,logonpoint=0;
	const int LO=-2, ER=-1;
	CString buf,temp;
	const int NUMLOGIN=9; // currently supports 9 different login sequences
	int logonseq[NUMLOGIN][100] = {
		// this array stores all of the logon sequences for the various firewalls 
		// in blocks of 3 nums. 1st num is command to send, 2nd num is next point in logon sequence array
		// if 200 series response is rec'd from server as the result of the command, 3rd num is next
		// point in logon sequence if 300 series rec'd
		{0,LO,3, 1,LO,6, 2,LO,ER}, // no firewall
		{3,6,3, 4,6,ER, 5,ER,9, 0,LO,12, 1,LO,15, 2,LO,ER}, // SITE hostname
		{3,6,3, 4,6,ER, 6,LO,9, 1,LO,12, 2,LO,ER}, // USER after logon
		{7,3,3, 0,LO,6, 1,LO,9, 2,LO,ER}, //proxy OPEN
		{3,6,3, 4,6,ER, 0,LO,9, 1,LO,12, 2,LO,ER}, // Transparent
		{6,LO,3, 1,LO,6, 2,LO,ER}, // USER with no logon
		{8,6,3, 4,6,ER, 0,LO,9, 1,LO,12, 2,LO,ER}, //USER fireID@remotehost
		{9,ER,3, 1,LO,6, 2,LO,ER}, //USER remoteID@remotehost fireID
		{10,LO,3, 11,LO,6, 2,LO,ER} // USER remoteID@fireID@remotehost
	};

	if(logontype<0||logontype>=NUMLOGIN) return false; // illegal connect code
	// are we connecting directly to the host (logon type 0) or via a firewall? (logon type>0)
	if(!logontype) {
		temp=hostname;
		port=hostport;
	}
	else {
		temp=fwhost;
		port=fwport;
	}
	if(hostport!=21) hostname.Format(hostname+_T(":%d"),hostport); // add port to hostname (only if port is not 21)
	if(!OpenCtrlChannel(temp,port)) return false;
	if(!FTPcommand(_T(""))) return false; // get initial connect msg off server
	// go through appropriate logon procedure
	while(1) {
		switch(logonseq[logontype][logonpoint]) {
		case 0:
			temp=_T("USER ")+username;
			break;
		case 1:
			temp=_T("PASS ")+password;
			break;
		case 2:
			temp=_T("ACCT ")+acct;
			break;
		case 3:
			temp=_T("USER ")+fwusername;
			break;
		case 4:
			temp=_T("PASS ")+fwpassword;
			break;
		case 5:
			temp=_T("SITE ")+hostname;
			break;
		case 6:
			temp=_T("USER ")+username+_T("@")+hostname;
			break;
		case 7:
			temp=_T("OPEN ")+hostname;
			break;
		case 8:
			temp=_T("USER ")+fwusername+_T("@")+hostname;
			break;
		case 9:
			temp=_T("USER ")+username+_T("@")+hostname+_T(" ")+fwusername;
			break;
		case 10:
			temp=_T("USER ")+username+_T("@")+fwusername+_T("@")+hostname;
			break;
		case 11:
			temp=_T("PASS ")+password+_T("@")+fwpassword;
			break;
		}
		// send command, get response
		if(!WriteStr(temp)) return false;
		if(!ReadStr()) return false;
		// only these responses are valid
		if(m_fc!=2&&m_fc!=3) return false;
		logonpoint=logonseq[logontype][logonpoint+m_fc-1]; //get next command from array
		switch(logonpoint) {
		case ER: // ER means summat has gone wrong
			m_retmsg.LoadString(IDS_FTPMSG1);
			return false;
		case LO: // LO means we're fully logged on
			return true;
		}
	}
}


// function to log off & close connection to FTP server
void CFTPclient::LogOffServer() {
	WriteStr(_T("QUIT"));
//if (bopened)		// 11/03/01 AM.
//	CloseControlChannel();	// 11/03/01 AM.
if (bnewopen)		// 10/17/01 AM.
	CloseCtrlChannel();	// 10/17/01 AM.
}


// function to execute commands on the FTP server
//Input functions set as const & wherever possible
//PD 02/14/2001
BOOL CFTPclient::FTPcommand(const CString &command) {
	if(command!=_T("")&&!WriteStr(command))
		return false;
	if((!ReadStr()) || (m_fc!=2))
		return false;
	return true;
}


// function to upload/download files
//Input functions set as const & wherever possible
//PD 02/14/2001
BOOL CFTPclient::MoveFile(const CString &remotefile, const CString &localfile,
						  const FTP_TRANS_TYPE &transType, const BOOL pasv, long fileSize) {
	CString lhost,temp,rhost;
	UINT localsock,serversock,i,j;
	CFile datafile;
	CSocket sockSrvr;
	CAsyncSocket datachannel;
	int num,numread,numsent;
	const int BUFSIZE=1024;
	_TCHAR cbuf[BUFSIZE + 1];
	DWORD lpArgument=0;
	
	bool get = fileSize ? true : false;

	// open local file
	if(!datafile.Open(localfile,(get?CFile::modeWrite|CFile::modeCreate:CFile::modeRead))) {
		m_retmsg.LoadString(IDS_FTPMSG4);
		AfxMessageBox(m_retmsg);
		return false;
	}

	if (!fileSize)
		fileSize = (long)datafile.GetLength();
	CString directionStr;
	if (get)
		directionStr = _T("Downloading:  ");
	else
		directionStr = _T("Uploading:  ");
	CString archiveActionStr = directionStr + remotefile;

	int upper = fileSize / BUFSIZE;
	int minUpper = 10;
	int bytesReceived = 0;
	CArchiveProgressDialog progressDialog;
	if (upper >= minUpper) {
		progressDialog.Create(IDD_ARCHIVE_PROGRESS, AfxGetMainWnd());
		progressDialog.CenterWindow();
		progressDialog.SetArchiveMessage(archiveActionStr);
		progressDialog.SetRange(0,upper);
		progressDialog.SetStep(1);
		progressDialog.SetFileSize(fileSize);
	}

	if(transType == FTP_TRANS_ASCII && !FTPcommand(_T("TYPE A")))
		return false; // request ASCII mode
	else if(transType == FTP_TRANS_BINARY && !FTPcommand(_T("TYPE I")))
		return false; // request BINARY mode

	if(pasv) { // set up a PASSIVE type file transfer
		if(!FTPcommand(_T("PASV"))) return false;
		// extract connect port number and IP from string returned by server
		if((i=m_retmsg.Find(_T("(")))==-1||(j=m_retmsg.Find(_T(")")))==-1) return false;
		temp=m_retmsg.Mid(i+1,(j-i)-1);
		i=temp.ReverseFind(',');
		serversock=_ttol(temp.Right(temp.GetLength()-(i+1))); //get ls byte of server socket
		temp=temp.Left(i);
		i=temp.ReverseFind(',');
		serversock+=256*_ttol(temp.Right(temp.GetLength()-(i+1))); // add ms byte to server socket
		rhost=temp.Left(i);
		while(1) { // convert commas to dots in IP
			if((i=rhost.Find(_T(",")))==-1) break;
			rhost.SetAt(i,'.');
		}
	}
	else { // set up a ACTIVE type file transfer
		m_retmsg.LoadString(IDS_FTPMSG6);
		// get the local IP address off the control channel socket
		if(!m_Ctrlsok->GetSockName(lhost,localsock)) return false;;
		while(1) { // convert returned '.' in ip address to ','
			if((i=lhost.Find(_T(".")))==-1) break;
			lhost.SetAt(i,',');
		}
		// create listen socket (let MFC choose the port) & start the socket listening
		if((!sockSrvr.Create(0,SOCK_STREAM,NULL))||(!sockSrvr.Listen())) return false;
		if(!sockSrvr.GetSockName(temp,localsock)) return false;// get the port that MFC chose
		// convert the port number to 2 bytes + add to the local IP
		lhost.Format(lhost+_T(",%d,%d"),localsock/256,localsock%256);
		if(!FTPcommand(_T("PORT ")+lhost)) return false;// send PORT cmd to server
	}

	// send RETR/STOR command to server
	if(!WriteStr((get?_T("RETR "):_T("STOR "))+remotefile))
		return false;

	if(pasv) {// if PASV create the socket & initiate outbound data channel connection
		if(!datachannel.Create()) {
			m_retmsg.LoadString(IDS_FTPMSG6);
			return false;
		}
		datachannel.Connect(rhost,serversock); // attempt to connect asynchronously (server will tell us if/when we're connected)
	}
	if(!ReadStr()||m_fc!=1)
		return false; // get response to RETR/STOR command

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->AddDebugLine(_T("Opening data connection:"),lhost,true);

	if(!pasv&&!sockSrvr.Accept(datachannel))
		return false; // if !PASV accept inbound data connection from server

	// we're connected & ready to do the data transfer, so set blocking mode on data channel socket
	if((!datachannel.AsyncSelect(0))||(!datachannel.IOCtl(FIONBIO,&lpArgument))) {
		m_retmsg.LoadString(IDS_FTPMSG6);
		return false;
	}

	wnd->AddDebugLine(_T("Transfering file:"),remotefile,true);
	bool loadedAll = false;
	bool cancelFlag = false;

	while(1) { // move data from/to server & read/write local file
		TRY {
			if(get) {
				if(0 && bytesReceived >= fileSize) {
					num = datachannel.Receive(cbuf,BUFSIZE,0);
					datafile.Write(cbuf,num);
					loadedAll = true;
					break;
				}
				else if (!(num=datachannel.Receive(cbuf,BUFSIZE,0)) || num==SOCKET_ERROR)
					break; // (EOF||network error)
				else
					datafile.Write(cbuf,num);
			}
			else {
				if(!(numread=datafile.Read(cbuf,BUFSIZE)))
					break; //EOF

				if((numsent=datachannel.Send(cbuf,numread,0)) == SOCKET_ERROR)
					break;

				// if we sent fewer bytes than we read from file, rewind file pointer
				if(numread!=numsent)
					datafile.Seek(numsent-numread,CFile::current);
			}
		}
		CATCH (CException,e) {
			m_retmsg.LoadString(IDS_FTPMSG5);
			return false;
		}
		END_CATCH

		bytesReceived += BUFSIZE;

		if (upper >= minUpper) {
			cancelFlag = progressDialog.StepIt();
			if (cancelFlag) {
				break;
			}
		}

		PeekHere();
	}

	if (upper >= minUpper) {
		int result = 0;
		progressDialog.EndDialog(result);
	}

	datachannel.Close();
	wnd->AddDebugLine(_T("Closing data connection:"),lhost,true);

	datafile.Close();

	if (cancelFlag) {
		if (!get)
			DeleteCommand(remotefile);
		DeleteFile(localfile);
		wnd->AddDebugLine(_T("Server user interrupt:"),_T("Canceled"),true);
		return false; // check transfer outcome msg from server
	}
	else if (loadedAll && !ReadStr())
		return false; // check transfer outcome msg from server
	else if (!loadedAll && !FTPcommand(_T("")))
		return false; // check transfer outcome msg from server

	wnd->AddDebugLine(_T("Server message complete:"),m_retmsg,true);

	return true; // oh goody it worked.
}


// function to upload/download files
//Input functions set as const & wherever possible
//PD 02/14/2001
BOOL CFTPclient::GetFileStr(const CString &remotefile, CString &contentStr,
							const FTP_TRANS_TYPE &transType, const BOOL pasv) {
	CString lhost,temp,rhost;
	UINT localsock,serversock,i,j;
	CSocket sockSrvr;
	CAsyncSocket datachannel;
	int num;
	const int BUFSIZE=1024;
	_TCHAR cbuf[BUFSIZE + 1]; // SRP 011023 This is a fix!
	DWORD lpArgument=0;
	
	CString directionStr;

	if(transType == FTP_TRANS_ASCII && !FTPcommand(_T("TYPE A")))
		return false; // request ASCII mode
	else if(transType == FTP_TRANS_BINARY && !FTPcommand(_T("TYPE I")))
		return false; // request BINARY mode

	if(pasv) { // set up a PASSIVE type file transfer
		if(!FTPcommand(_T("PASV"))) return false;
		// extract connect port number and IP from string returned by server
		if((i=m_retmsg.Find(_T("(")))==-1||(j=m_retmsg.Find(_T(")")))==-1) return false;
		temp=m_retmsg.Mid(i+1,(j-i)-1);
		i=temp.ReverseFind(',');
		serversock=_ttol(temp.Right(temp.GetLength()-(i+1))); //get ls byte of server socket
		temp=temp.Left(i);
		i=temp.ReverseFind(',');
		serversock+=256*_ttol(temp.Right(temp.GetLength()-(i+1))); // add ms byte to server socket
		rhost=temp.Left(i);
		while(1) { // convert commas to dots in IP
			if((i=rhost.Find(_T(",")))==-1) break;
			rhost.SetAt(i,'.');
		}
	}
	else { // set up a ACTIVE type file transfer
		m_retmsg.LoadString(IDS_FTPMSG6);
		// get the local IP address off the control channel socket
		if(!m_Ctrlsok->GetSockName(lhost,localsock)) return false;;
		while(1) { // convert returned '.' in ip address to ','
			if((i=lhost.Find(_T(".")))==-1) break;
			lhost.SetAt(i,',');
		}
		// create listen socket (let MFC choose the port) & start the socket listening
		if((!sockSrvr.Create(0,SOCK_STREAM,NULL))||(!sockSrvr.Listen())) return false;
		if(!sockSrvr.GetSockName(temp,localsock)) return false;// get the port that MFC chose
		// convert the port number to 2 bytes + add to the local IP
		lhost.Format(lhost+_T(",%d,%d"),localsock/256,localsock%256);
		if(!FTPcommand(_T("PORT ")+lhost)) return false;// send PORT cmd to server
	}

	// send RETR/STOR command to server
	if(!WriteStr((_T("RETR "))+remotefile))
		return false;

	if(pasv) {// if PASV create the socket & initiate outbound data channel connection
		if(!datachannel.Create()) {
			m_retmsg.LoadString(IDS_FTPMSG6);
			return false;
		}
		datachannel.Connect(rhost,serversock); // attempt to connect asynchronously (server will tell us if/when we're connected)
	}
	if(!ReadStr()||m_fc!=1)
		return false; // get response to RETR/STOR command

	if(!pasv&&!sockSrvr.Accept(datachannel))
		return false; // if !PASV accept inbound data connection from server

	// we're connected & ready to do the data transfer, so set blocking mode on data channel socket
	if((!datachannel.AsyncSelect(0))||(!datachannel.IOCtl(FIONBIO,&lpArgument))) {
		m_retmsg.LoadString(IDS_FTPMSG6);
		return false;
	}

	bool loadedAll = false;
	bool cancelFlag = false;
	int bytesReceived = 0;

	while(1) { // move data from/to server & read/write local file
		TRY {
			if (!(num=datachannel.Receive(cbuf,BUFSIZE,0)) || num==SOCKET_ERROR)
				break; // (EOF||network error)
			else {
				cbuf[num] = '\0';
				contentStr += cbuf;
				PeekHere(1);
				//datafile.Write(cbuf,num);
			}
		}
		CATCH (CException,e) {
			m_retmsg.LoadString(IDS_FTPMSG5);
			return false;
		}
		END_CATCH

		bytesReceived += BUFSIZE;
	}

	datachannel.Close();

	if (cancelFlag) {
		contentStr = _T("");
		return false; // check transfer outcome msg from server
	}
	else if (loadedAll && !ReadStr())
		return false; // check transfer outcome msg from server
	else if (!loadedAll && !FTPcommand(_T("")))
		return false; // check transfer outcome msg from server

	return true; // oh goody it worked.
}


// function to upload/download files
//Input functions set as const & wherever possible
//PD 02/14/2001
BOOL CFTPclient::SetFileStr(const CString &remotefile, CString &conStr,
							const FTP_TRANS_TYPE &transType, const BOOL pasv)
{
	CString lhost,temp,rhost;
	UINT localsock,serversock,i,j;
	CFile datafile;
	CSocket sockSrvr;
	CAsyncSocket datachannel;
	int numread,numsent;
	const int BUFSIZE=1024;
	_TCHAR cbuf[BUFSIZE + 1];
	DWORD lpArgument=0;
	int bytesReceived = 0;
	
	CString directionStr;
	CString archiveActionStr = directionStr + remotefile;

	if(transType == FTP_TRANS_ASCII && !FTPcommand(_T("TYPE A")))
		return false; // request ASCII mode
	else if(transType == FTP_TRANS_BINARY && !FTPcommand(_T("TYPE I")))
		return false; // request BINARY mode

	if(pasv) { // set up a PASSIVE type file transfer
		if(!FTPcommand(_T("PASV"))) return false;
		// extract connect port number and IP from string returned by server
		if((i=m_retmsg.Find(_T("(")))==-1||(j=m_retmsg.Find(_T(")")))==-1) return false;
		temp=m_retmsg.Mid(i+1,(j-i)-1);
		i=temp.ReverseFind(',');
		serversock=_ttol(temp.Right(temp.GetLength()-(i+1))); //get ls byte of server socket
		temp=temp.Left(i);
		i=temp.ReverseFind(',');
		serversock+=256*_ttol(temp.Right(temp.GetLength()-(i+1))); // add ms byte to server socket
		rhost=temp.Left(i);
		while(1) { // convert commas to dots in IP
			if((i=rhost.Find(_T(",")))==-1) break;
			rhost.SetAt(i,'.');
		}
	}
	else { // set up a ACTIVE type file transfer
		m_retmsg.LoadString(IDS_FTPMSG6);
		// get the local IP address off the control channel socket
		if(!m_Ctrlsok->GetSockName(lhost,localsock)) return false;;
		while(1) { // convert returned '.' in ip address to ','
			if((i=lhost.Find(_T(".")))==-1) break;
			lhost.SetAt(i,',');
		}
		// create listen socket (let MFC choose the port) & start the socket listening
		if((!sockSrvr.Create(0,SOCK_STREAM,NULL))||(!sockSrvr.Listen())) return false;
		if(!sockSrvr.GetSockName(temp,localsock)) return false;// get the port that MFC chose
		// convert the port number to 2 bytes + add to the local IP
		lhost.Format(lhost+_T(",%d,%d"),localsock/256,localsock%256);
		if(!FTPcommand(_T("PORT ")+lhost)) return false;// send PORT cmd to server
	}

	// send RETR/STOR command to server
	if(!WriteStr(_T("STOR ")+remotefile))
		return false;

	if(pasv) {// if PASV create the socket & initiate outbound data channel connection
		if(!datachannel.Create()) {
			m_retmsg.LoadString(IDS_FTPMSG6);
			return false;
		}
		datachannel.Connect(rhost,serversock); // attempt to connect asynchronously (server will tell us if/when we're connected)
	}
	if(!ReadStr()||m_fc!=1)
		return false; // get response to RETR/STOR command

	if(!pasv&&!sockSrvr.Accept(datachannel))
		return false; // if !PASV accept inbound data connection from server

	// we're connected & ready to do the data transfer, so set blocking mode on data channel socket
	if((!datachannel.AsyncSelect(0))||(!datachannel.IOCtl(FIONBIO,&lpArgument))) {
		m_retmsg.LoadString(IDS_FTPMSG6);
		return false;
	}

	bool loadedAll = false;
	bool cancelFlag = false;
	int start = 0;
	numsent = 0;
	numread = 0;

	CString contentStr = conStr;
	while (1) { // move data from/to server & read/write local file
		TRY {
			numread = BUFSIZE;
			if ((contentStr.GetLength() - start) < BUFSIZE)
				numread = contentStr.GetLength() - start;
			if (numread == 0)
				break;
			CString strPart = contentStr.Mid(start,numread);
			StringToChar(cbuf,strPart,BUFSIZE);
			start += numread;

			if((numsent=datachannel.Send(cbuf,numread,0)) == SOCKET_ERROR)
				break;

			// if we sent fewer bytes than we read from file, rewind file pointer
			if(numread!=numsent)
				start -= numread-numsent;
		}
		CATCH (CException,e) {
			m_retmsg.LoadString(IDS_FTPMSG5);
			return false;
		}
		END_CATCH

		bytesReceived += numread;
	}

	datachannel.Close();

	if (cancelFlag) {
		DeleteCommand(remotefile);
		return false; // check transfer outcome msg from server
	}
	else if (loadedAll && !ReadStr())
		return false; // check transfer outcome msg from server
	else if (!loadedAll && !FTPcommand(_T("")))
		return false; // check transfer outcome msg from server

	return true; // oh goody it worked.
}


// function to upload/download files
//Input functions set as const & wherever possible
//PD 02/14/2001
BOOL CFTPclient::DirectoryListing(CString &dirStr, const bool shortList)
{
	CString lhost,temp,rhost,msbyte;
	UINT localsock,i;
	CSocket sockSrvr;
	CAsyncSocket datachannel;
	int num;
	const int BUFSIZE=4096;
	_TCHAR cbuf[BUFSIZE + 1];
	DWORD lpArgument=0;

	dirStr = _T("");
	
	if(!FTPcommand(_T("TYPE A")))
		return false; // request ASCII mode

	m_retmsg.LoadString(IDS_FTPMSG6);
	// get the local IP address off the control channel socket
	if(!m_Ctrlsok->GetSockName(lhost,localsock)) return false;;
	while(1) { // convert returned '.' in ip address to ','
		if((i=lhost.Find(_T(".")))==-1) break;
		lhost.SetAt(i,',');
	}
	// create listen socket (let MFC choose the port) & start the socket listening
	if((!sockSrvr.Create(0,SOCK_STREAM,NULL))||(!sockSrvr.Listen())) return false;
	if(!sockSrvr.GetSockName(temp,localsock)) return false;// get the port that MFC chose
	// convert the port number to 2 bytes + add to the local IP
	lhost.Format(lhost+_T(",%d,%d"),localsock/256,localsock%256);
	if(!FTPcommand(_T("PORT ")+lhost))
		return false;// send PORT cmd to server

	CString listType = _T("LIST");
	if (shortList)
		listType = _T("NLST");
	if(!WriteStr(listType))
		return false;
	if(!ReadStr() || m_fc!=1)
		return false; // get response to RETR/STOR command

	if(!sockSrvr.Accept(datachannel))
		return false; // if !PASV accept inbound data connection from server

	// we're connected & ready to do the data transfer, so set blocking mode on data channel socket
	if((!datachannel.AsyncSelect(0))||(!datachannel.IOCtl(FIONBIO,&lpArgument))) {
		m_retmsg.LoadString(IDS_FTPMSG6);
		return false;
	}

	// we're connected & ready to do the data transfer, so set blocking mode on data channel socket
	if((!datachannel.AsyncSelect(0))||(!datachannel.IOCtl(FIONBIO,&lpArgument))) {
		m_retmsg.LoadString(IDS_FTPMSG6);
		return false;
	}
	while(1) { // move data from/to server & read/write local file
		TRY {
			if(!(num=datachannel.Receive(cbuf,BUFSIZE,0))||num==SOCKET_ERROR)
				break; // (EOF||network error)
			else {
				cbuf[num] = '\0';
				dirStr += cbuf;
			}
		}
		CATCH (CException,e) {
			m_retmsg.LoadString(IDS_FTPMSG5);
			return false;
		}
		END_CATCH
	}
	datachannel.Close();

	if(!FTPcommand(_T("")))
		return false; // check transfer outcome msg from server

	return true; // oh goody it worked.
}


// function to send a command string on the server control channel
//Input functions set as const & wherever possible
//PD 02/14/2001
BOOL CFTPclient::WriteStr(const CString &outputstring) {
	m_retmsg.LoadString(IDS_FTPMSG6); // pre-load "network error" msg (in case there is one) #-)
	TRY {
		m_pCtrlTxarch->WriteString(outputstring+_T("\r\n"));
		m_pCtrlTxarch->Flush();
	}
	CATCH(CException,e) {
		return false;
	}
	END_CATCH
	return true;
}


// this function gets the server response line
BOOL CFTPclient::ReadStr() {
	int retcode;

	if(!ReadStr2()) return false;
	if(m_retmsg.GetLength()<4||m_retmsg.GetAt(3)!='-') return true;
	retcode=_ttol(m_retmsg);
	retcode = 0;
	while(1) { //handle multi-line server responses
		if(m_retmsg.GetLength()>3&&(m_retmsg.GetAt(3)==' '&&_ttol(m_retmsg)==retcode)) return true;
		if(!ReadStr2()) return false;
	}
}



//////////////////////////////////////////////////////////////////////
// Private functions
//////////////////////////////////////////////////////////////////////


// read a single response line from the server control channel
BOOL CFTPclient::ReadStr2() {
	TRY {
		if(!m_pCtrlRxarch->ReadString(m_retmsg)) {
			m_retmsg.LoadString(IDS_FTPMSG6);
			return false;
		}
	}
	CATCH(CException,e) {
		m_retmsg.LoadString(IDS_FTPMSG6);
		return false;
	}
	END_CATCH
	if(m_retmsg.GetLength()>0) m_fc=m_retmsg.GetAt(0)-48; // get 1st digit of the return code (indicates primary result)
	return true;
}


// open the control channel to the FTP server
//Input functions set as const & wherever possible
//PD 02/14/2001
// Obsolete.	// 11/03/01 AM.
#ifdef OBSOLETE_
BOOL CFTPclient::OpenControlChannel(const CString &serverhost,const int serverport) {
	bopened = true;	// 10/17/01 AM.
	m_retmsg.LoadString(IDS_FTPMSG2);
	if(!(m_Ctrlsok=new CSocket)) return false;
	if(!(m_Ctrlsok->Create())) return false;
//	m_retmsg.LoadString(IDS_FTPMSG3);	// 10/17/01 AM.
//	if(!(m_Ctrlsok->Connect(serverhost,serverport))) return false;	// 10/17/01 AM.
//	m_retmsg.LoadString(IDS_FTPMSG2);	// 10/17/01 AM.
	if(!(m_pCtrlsokfile=new CSocketFile(m_Ctrlsok))) return false;
	if(!(m_pCtrlRxarch=new CArchive(m_pCtrlsokfile,CArchive::load))) return false;
	if(!(m_pCtrlTxarch=new CArchive(m_pCtrlsokfile,CArchive::store))) return false;
	m_retmsg.LoadString(IDS_FTPMSG3);	// 10/17/01 AM.
	if(!(m_Ctrlsok->Connect(serverhost,serverport))) return false;	// 10/17/01 AM.
	return true;
}
#endif

// open the control channel to the FTP server
// Variant to handle FMM error.	// 10/17/01 AM.
BOOL CFTPclient::OpenCtrlChannel(const CString &serverhost,const int serverport) {
	bnewopen = true;	// 10/17/01 AM.
	m_retmsg.LoadString(IDS_FTPMSG2);
//	if(!(m_Ctrlsok=new CSocket)) return false;
//	if(!(m_Ctrlsok->Create())) return false;
//	if(!(m_pCtrlsokfile=new CSocketFile(m_Ctrlsok))) return false;
	if(!(m_pCtrlRxarch=new CArchive(m_pCtrlsokfile,CArchive::load))) return false;
	if(!(m_pCtrlTxarch=new CArchive(m_pCtrlsokfile,CArchive::store))) return false;
	m_retmsg.LoadString(IDS_FTPMSG3);	// 10/17/01 AM.
	if(!(m_Ctrlsok->Connect(serverhost,serverport))) return false;	// 10/17/01 AM.
	return true;
}

// close the control channel to the FTP server
// Obsolete.	// 11/03/01 AM.
#ifdef OBSOLETE_
void CFTPclient::CloseControlChannel()
{
	if(m_pCtrlTxarch) delete m_pCtrlTxarch;
	m_pCtrlTxarch=NULL;
	if(m_pCtrlRxarch) delete m_pCtrlRxarch;
	m_pCtrlRxarch=NULL;
	if(m_pCtrlsokfile) delete m_pCtrlsokfile;
	m_pCtrlsokfile=NULL;
	if(m_Ctrlsok) delete m_Ctrlsok;
	m_Ctrlsok=NULL;
	bopened = false;		// 10/17/01 AM.
	return;
}
#endif

// close the control channel to the FTP server
// Variant to handle FMM error.	// 10/17/01 AM.
void CFTPclient::CloseCtrlChannel()
{
	if(m_pCtrlTxarch) delete m_pCtrlTxarch;
	m_pCtrlTxarch=NULL;
	if(m_pCtrlRxarch) delete m_pCtrlRxarch;
	m_pCtrlRxarch=NULL;
//	if(m_pCtrlsokfile) delete m_pCtrlsokfile;
	m_pCtrlsokfile=NULL;
//	if(m_Ctrlsok) delete m_Ctrlsok;
	m_Ctrlsok=NULL;
	bnewopen = false;		// 10/17/01 AM.
	return;
}


//Input functions set as const & wherever possible
//PD 02/14/2001
bool CFTPclient::LoginConnectToArchiveServer(const CString &archiveDirectoryStr)
{
	if (appStrArchiveFTP == _T("") || appStrArchiveUsername == _T("") ||
		appStrArchivePassword == _T("")) {
		AfxMessageBox(_T("Please fill in the archive information in the\nVisual Text Preferences window!"),true);
		return false;
	}

	if(!LogOnToServer(appStrArchiveFTP,21,appStrArchiveUsername,appStrArchivePassword,_T(""),_T(""),_T(""),_T(""),0,0)) {
		AfxMessageBox(m_retmsg);
		return false;
	}

	CString dirPathStr = appStrArchiveDirectoryPath + _T("/") + archiveDirectoryStr;
	CString cwdStr = _T("CWD ") + dirPathStr;

	if(!FTPcommand(cwdStr)) {
		AfxMessageBox(m_retmsg);
		LogOffServer();
		return false;
	}

	return true;
}

//Input functions set as const & wherever possible
//PD 02/14/2001
bool CFTPclient::LoginConnectToServer(const CString &ftpStr, const CString &dirPathStr, const CString &usernameStr,
									  const CString &passwordStr, const bool noWarnings)
{
	if (!noWarnings &&
		(appStrArchiveFTP == _T("") || appStrArchiveUsername == _T("") || appStrArchivePassword == _T(""))) {
		AfxMessageBox(_T("Please fill in the archive information in the\nVisual Text Preferences window!"),true);
		return false;
	}

	if(!LogOnToServer(ftpStr,21,usernameStr,passwordStr,_T(""),_T(""),_T(""),_T(""),0,0)) {
		if (!noWarnings)
			AfxMessageBox(m_retmsg);
		return false;
	}

	CString cwdStr = _T("CWD ") + dirPathStr;

	if(!FTPcommand(cwdStr)) {
		AfxMessageBox(m_retmsg);
		LogOffServer();
		return false;
	}

	return true;
}

//Input functions set as const & wherever possible
//PD 02/14/2001
bool CFTPclient::DeleteCommand(const CString &remoteFileStr)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->AddDebugLine(_T("Deleting archive:"),remoteFileStr,true);
	CString ftpCommandStr = _T("DELE ") + remoteFileStr;
	if (!FTPcommand(ftpCommandStr)) {
		AfxMessageBox(m_retmsg);
		LogOffServer();
		return false;
	}

	return true;
}

//Input functions set as const & wherever possible
//PD 02/14/2001
bool GetServerArchiveDirectoryString(const CString &archiveDirectoryStr)
{
	CFTPclient ftp;

	// Fix to FMM leak.	// 11/03/01 AM.
	ftp.m_retmsg.LoadString(IDS_FTPMSG2);	// 11/03/01 AM.
	CSocket sock;	// 10/17/01 AM.
	if (!ftp.MakeSock(&sock)) return false;	// 11/03/01 AM.
	CSocketFile sockfile(&sock);	// 11/03/01 AM.
	if (!ftp.SetSockFile(&sockfile)) return false;	// 11/03/01 AM.

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->AddDebugLine(_T("Opening FTP connection:"),appStrArchiveFTP,true);

	// connect to FTP server
	if(!ftp.LoginConnectToArchiveServer(archiveDirectoryStr))
		return false;

	// upload a file
	CString dirPathStr = appStrArchiveDirectoryPath + _T("/") + archiveDirectoryStr;

	wnd->AddDebugLine(_T("Downloading directory:"),dirPathStr,true);
	if(!ftp.DirectoryListing(appAskText,false)) {
		AfxMessageBox(ftp.m_retmsg);
		ftp.LogOffServer();
		return false;
	}

	// disconnect from server
	ftp.LogOffServer();
	wnd->AddDebugLine(_T("Disconnected FTP"),appStrArchiveFTP,true);
	return true ;
}

//Input functions set as const & wherever possible
//PD 02/14/2001
bool UploadArchive(const CString &archiveDirectoryStr, const CString &remoteFileStr, const CString &localFileStr)
{
	CFTPclient ftp;

	// Fix to FMM leak.	// 11/03/01 AM.
	ftp.m_retmsg.LoadString(IDS_FTPMSG2);	// 11/03/01 AM.
	CSocket sock;	// 10/17/01 AM.
	if (!ftp.MakeSock(&sock)) return false;	// 11/03/01 AM.
	CSocketFile sockfile(&sock);	// 11/03/01 AM.
	if (!ftp.SetSockFile(&sockfile)) return false;	// 11/03/01 AM.

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->AddDebugLine(_T("Opening FTP connection:"),appStrArchiveFTP,true);

	// connect to FTP server
	if(!ftp.LoginConnectToArchiveServer(archiveDirectoryStr))
		return false;

	if (!ftp.MoveFile(remoteFileStr,localFileStr,FTP_TRANS_BINARY,false,false))
		return false;

	// disconnect from server
	ftp.LogOffServer();
	wnd->AddDebugLine(_T("Disconnected FTP"),appStrArchiveFTP,true);
	return true;
}

//Input functions set as const & wherever possible
//PD 02/14/2001
bool DownloadArchive(const CString &archiveDirectoryStr, const CString &remoteFileStr, const CString &localFileStr,
					 const long fileSize)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->AddDebugLine(_T("Opening FTP connection:"),appStrArchiveFTP,true);

	CFTPclient ftp;

	// Fix to FMM leak.	// 11/03/01 AM.
	ftp.m_retmsg.LoadString(IDS_FTPMSG2);	// 11/03/01 AM.
	CSocket sock;	// 10/17/01 AM.
	if (!ftp.MakeSock(&sock)) return false;	// 11/03/01 AM.
	CSocketFile sockfile(&sock);	// 11/03/01 AM.
	if (!ftp.SetSockFile(&sockfile)) return false;	// 11/03/01 AM.

	// connect to FTP server
	if(!ftp.LoginConnectToArchiveServer(archiveDirectoryStr))
		return false;

	if (!ftp.MoveFile(remoteFileStr,localFileStr,FTP_TRANS_BINARY,false,fileSize))
		return false ;

	// disconnect from server
	ftp.LogOffServer();

	wnd->AddDebugLine(_T("Disconnected FTP"),appStrArchiveFTP,true);

	return true ;
}

//Input functions set as const & wherever possible
//PD 02/14/2001
bool GetRemoteFileContent(const CString &ftpStr, const CString &localDirectoryStr, const CString &remoteFileStr,
			              const CString &usernameStr, const CString &passwordStr, CString &contentStr,
						  CString &timestampStr)
{
	CFTPclient ftp;

	ftp.m_retmsg.LoadString(IDS_FTPMSG2);	// 10/17/01 AM.
	CSocket sock;	// 10/17/01 AM.
	if (!ftp.MakeSock(&sock)) return false;	// 10/17/01 AM.
	CSocketFile sockfile(&sock);	// 10/17/01 AM.
	if (!ftp.SetSockFile(&sockfile)) return false;	// 10/17/01 AM.

	// connect to FTP server
	if(!ftp.LoginConnectToServer(ftpStr,localDirectoryStr,usernameStr,passwordStr,true))
		return false;

	// USE THIS FILE AS A TIME STAMP FOR LOGINS
	CString remoteStr = _T("timestamp.txt");
	CString contStr = _T("noop");
	if (!ftp.SetFileStr(remoteStr,contStr,FTP_TRANS_BINARY,false))
		return false ;

	// GET TIME STAMP
	CString commandStr;
	commandStr.Format(_T("MDTM %s"),remoteStr);
	if(!ftp.WriteStr(commandStr))
		return false;
	if(!ftp.ReadStr())
		return false; // get response to MDTM command
	timestampStr = ftp.m_retmsg;
	int space = timestampStr.Find(_T(" "),0);
	timestampStr = timestampStr.Right(timestampStr.GetLength()-space-1);

	if (!ftp.GetFileStr(remoteFileStr,contentStr,FTP_TRANS_BINARY,false))
		return false ;

	// disconnect from server
	ftp.LogOffServer();
	return true ;
}

//Input functions set as const & wherever possible
//PD 02/14/2001
bool SetRemoteFileContent(const CString &ftpStr, const CString &localDirectoryStr, const CString &remoteFileStr,
			              const CString &usernameStr, const CString &passwordStr, CString &contentStr)
{
	CFTPclient ftp;

	ftp.m_retmsg.LoadString(IDS_FTPMSG2);	// 10/17/01 AM.
	CSocket sock;	// 10/17/01 AM.
	if (!ftp.MakeSock(&sock)) return false;	// 10/17/01 AM.
	CSocketFile sockfile(&sock);	// 10/17/01 AM.
	if (!ftp.SetSockFile(&sockfile)) return false;	// 10/17/01 AM.

	// connect to FTP server
	if(!ftp.LoginConnectToServer(ftpStr,localDirectoryStr,usernameStr,passwordStr,true))
		return false;

	if (!ftp.SetFileStr(remoteFileStr,contentStr,FTP_TRANS_BINARY,false))
		return false ;

	// disconnect from server
	ftp.LogOffServer();
	return true ;
}

//Input functions set as const & wherever possible
//PD 02/14/2001
bool DeleteArchive(const CString &archiveDirectoryStr, const CString &remoteFileStr)
{
	CFTPclient ftp;

	// Fix to FMM leak.	// 11/03/01 AM.
	ftp.m_retmsg.LoadString(IDS_FTPMSG2);	// 11/03/01 AM.
	CSocket sock;	// 10/17/01 AM.
	if (!ftp.MakeSock(&sock)) return false;	// 11/03/01 AM.
	CSocketFile sockfile(&sock);	// 11/03/01 AM.
	if (!ftp.SetSockFile(&sockfile)) return false;	// 11/03/01 AM.

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->AddDebugLine(_T("Opening FTP connection:"),appStrArchiveFTP,true);

	// connect to FTP server
	if (!ftp.LoginConnectToArchiveServer(archiveDirectoryStr))
		return false;

	if (!ftp.DeleteCommand(remoteFileStr))
		return false;

	// disconnect from server
	ftp.LogOffServer();
	wnd->AddDebugLine(_T("Disconnected FTP"),appStrArchiveFTP,true);
	return true;
}

//Input functions set as const & wherever possible
//PD 02/14/2001
bool RenameArchive(const CString &archiveDirectoryStr, const CString &remoteFileStr, const CString &newRemoteFileStr)
{
	CFTPclient ftp;

	// Fix to FMM leak.	// 11/03/01 AM.
	ftp.m_retmsg.LoadString(IDS_FTPMSG2);	// 11/03/01 AM.
	CSocket sock;	// 10/17/01 AM.
	if (!ftp.MakeSock(&sock)) return false;	// 11/03/01 AM.
	CSocketFile sockfile(&sock);	// 11/03/01 AM.
	if (!ftp.SetSockFile(&sockfile)) return false;	// 11/03/01 AM.

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->AddDebugLine(_T("Opening FTP connection:"),appStrArchiveFTP,true);

	// connect to FTP server
	if(!ftp.LoginConnectToArchiveServer(archiveDirectoryStr))
		return false;

	wnd->AddDebugLine(_T("Rename archive:"),remoteFileStr,true);
	CString ftpCommandStr = _T("RNFR ") + remoteFileStr;
	ftp.WriteStr(ftpCommandStr);
	if (!ftp.ReadStr() || ftp.m_fc!=3) {
		AfxMessageBox(ftp.m_retmsg);
		ftp.LogOffServer();
		return false;
	}

	ftpCommandStr = _T("RNTO ") + newRemoteFileStr;
	if(!ftp.FTPcommand(ftpCommandStr)) {
		AfxMessageBox(ftp.m_retmsg);
		ftp.LogOffServer();
		return false;
	}

	// disconnect from server
	ftp.LogOffServer();
	wnd->AddDebugLine(_T("Disconnected FTP"),appStrArchiveFTP,true);
	return true;
}

BOOL IsInternetConnected(const CString &serverPathStr)
{
	CString pathStr = serverPathStr;
	if (pathStr == _T(""))
		pathStr = _T("www.textanalysis.com");

	bool bResult = true;
	CSocket m_Server;

	if (!m_Server.Create()) {                                                       
		bResult = false;
	}   

	if (!m_Server.Connect(pathStr,80)) {
		m_Server.Close();
		bResult = false;
	}

	m_Server.Close();

	return bResult;
}

/*
BOOL IsInternetConnection()
{
 char szHostName[128];

 BOOL bPrivateAdr = false; // Private Address area
 BOOL bClassA = false;     // Class A definition
 BOOL bClassB = false;     // Class B definition
 BOOL bClassC = false;     // Class C definition
 BOOL bAutoNet = false;    // AutoNet definition
 CString str;

 if (gethostname(szHostName, 128) == 0 )
 {
  // Get host adresses
  struct hostent * pHost;
  int i;
  UINT ipb;
  pHost = gethostbyname(szHostName); 

  for (i = 0; 
       pHost!= NULL && pHost->h_addr_list[i]!= NULL; 
       i++ )
  {
   int j;
   str="";
   bClassA = bClassB = bClassC = false;
   for( j = 0; j < pHost->h_length; j++ )
   {
    CString addr;

    if( j > 0 )	str += ".";
    ipb = (unsigned int)((unsigned char*)pHost->h_addr_list[i])[j];

    // Define the IP range for exclusion
    if(j==0)
    {
     if(bClassA = (ipb == 10)) break; // Class A defined
     bClassB = (ipb == 172); 
     bClassC = (ipb == 192);
     bAutoNet = (ipb == 169);
    }
    else if (j==1)
    {
     // Class B defined
     if(bClassB = (bClassB && ipb >= 16 && ipb <= 31)) break;

     // Class C defined
     if(bClassC = (bClassC && ipb == 168)) break;

     //AutoNet pasibility defined
     if(bAutoNet = (bAutoNet && ipb == 254)) break;
    }

    addr.Format("%u", ipb );
    str += addr;
   }
   // If any address of Private Address 
   // area has been found bPrivateAdr = true
   bPrivateAdr = bPrivateAdr || bClassA || bClassB || bClassC;

   // If any address of Internet Address area 
   // has been found returns TRUE
   if (!bClassA 
   && !bClassB 
   && !bClassC 
   && !bAutoNet 
   && str != "127.0.0.1" 
   && !str.IsEmpty()) 
    return TRUE;
  }
 }

 if (bPrivateAdr)
 {
  // The system has IP address from Private Address 
  // area,only. Internet from the computer can be accessable 
  // via Proxy. If user turn on proxy connection flag, the 
  // function believe Internet accessable.
  //return bProxyConnection;
  return false;
 }
 
 return false;
}
*/
