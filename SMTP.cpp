/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// SMTP.cpp: implementation of the CSMTP class.
// Copyright (c) 1998, Wes Clyburn
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SMTP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Static member initializers
//

// Note: the order of the entries is important.
//       They must be synchronized with eResponse entries. 
CSMTP::response_code CSMTP::response_table[] =
{
	{ 250, _T("SMTP server error") },						// GENERIC_SUCCESS
	{ 220, _T("SMTP server not available") },				// CONNECT_SUCCESS
	{ 354, _T("SMTP server not ready for data") },			// DATA_SUCCESS
	{ 221, _T("SMTP server didn't terminate session") }  	// QUIT_SUCCESS
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSMTP::CSMTP( LPCTSTR szSMTPServerName, UINT nPort )
{
	ASSERT( szSMTPServerName != NULL );
	AfxSocketInit();
	m_sMailerName = _T( "WC Mail" );
	m_sSMTPServerHostName = szSMTPServerName;
	m_nPort = nPort;
	m_bConnected = false;
	m_sError = _T( "OK" );
}

CSMTP::~CSMTP()
{
	if( m_bConnected )
		Disconnect();
}

CString CSMTP::GetServerHostName()
{
	return m_sSMTPServerHostName;
}

BOOL CSMTP::Connect()
{
	CString sHello;
	TCHAR local_host[ 80 ];	// Warning: arbitrary size
	if( m_bConnected )
		return true;

	if( !m_wsSMTPServer.Create() )
	{
		m_sError = _T( "Unable to create the socket." );
		return false;
	}
	if( !m_wsSMTPServer.Connect( GetServerHostName(), GetPort() ) )
	{
		m_sError = _T( "Unable to connect to server" );
		m_wsSMTPServer.Close();
		return false;
	}
	if( !get_response( CONNECT_SUCCESS ) )
	{
		m_sError = _T( "Server didn't respond." );
		m_wsSMTPServer.Close();
		return false;
	}
	gethostname( TCHAR2A(local_host), 80 );
	sHello.Format( _T("HELO %s\r\n"), local_host );
	m_wsSMTPServer.Send( (LPCTSTR)sHello, sHello.GetLength() );
	if( !get_response( GENERIC_SUCCESS ) )
	{
		m_wsSMTPServer.Close();
		return false;
	}
	m_bConnected = true;
	return true;
}

BOOL CSMTP::Disconnect()
{
	BOOL ret;
	if( !m_bConnected )
		return true;
	// Disconnect gracefully from the server and close the socket
	CString sQuit = _T( "QUIT\r\n" );
	m_wsSMTPServer.Send( (LPCTSTR)sQuit, sQuit.GetLength() );

	// No need to check return value here.
	// If it fails, the message is available with GetLastError
	ret = get_response( QUIT_SUCCESS );
	m_wsSMTPServer.Close();

	m_bConnected = false;
	return ret;
}

UINT CSMTP::GetPort()
{
	return m_nPort;
}

CString CSMTP::GetMailerName()
{
	return m_sMailerName;
}

CString CSMTP::GetLastError()
{
	return m_sError;
}

BOOL CSMTP::SendMessage(CMailMessage * msg)
{
	ASSERT( msg != NULL );
	if( !m_bConnected )
	{
		m_sError = _T( "Must be connected" );
		return false;
	}
	if( FormatMailMessage( msg ) == false )
	{
		return false;
	}
	if( transmit_message( msg ) == false )
	{
		return false;
	}
	return true;
}

BOOL CSMTP::FormatMailMessage( CMailMessage* msg )
{
	ASSERT( msg != NULL );
	if( prepare_header( msg ) == false )
	{
		return false;
	}
	// Append a CR/LF to body if necessary.
	if( msg->m_sBody.Right( 2 ) != _T("\r\n") )
		msg->m_sBody += _T("\r\n");
	return true;
}

void CSMTP::SetServerProperties( LPCTSTR sServerHostName, UINT nPort)
{
	ASSERT( sServerHostName != NULL );
	// Needs to be safe in non-debug too
	if( sServerHostName == NULL )
		return;
	m_sSMTPServerHostName = sServerHostName;
	m_nPort = nPort;
}

// Create the SMTP message header as per RFC822
BOOL CSMTP::prepare_header(CMailMessage * msg)
{
	ASSERT( msg != NULL );
	CString sTo;
	CString sDate;
	CString& sHeader = msg->m_sHeader;	// Shorthand

	if( msg->GetNumRecipients() <= 0 )
	{
		m_sError = _T("No Recipients");
		return false;
	}

	sHeader = _T("");	// Clear it

	// Get the recipients into a single string
	sTo = _T("");
	CString sEmail = _T("");
	CString sFriendly = _T("");
	for( int i = 0; i < msg->GetNumRecipients(); i++ )
	{
		msg->GetRecipient( sEmail, sFriendly, i );
		sTo += ( i > 0 ? _T(",") : _T("") );
		sTo += sFriendly;
		sTo += _T("<");
		sTo += sEmail;
		sTo += _T(">");
	}
	msg->m_tDateTime = msg->m_tDateTime.GetCurrentTime();
	// Format: Mon, 01 Jun 98 01:10:30 GMT
	sDate = msg->m_tDateTime.Format( _T("%a, %d %b %y %H:%M:%S %Z") );
	sHeader.Format( _T("Date: %s\r\n")\
					_T("From: %s\r\n")\
					_T("To: %s\r\n")\
					_T("Subject: %s\r\n")\
					_T("X-Mailer: <%s>\r\n\r\n"),
					// Include other extension lines if desired
					(LPCTSTR)sDate,
					(LPCTSTR)msg->m_sFrom,
					(LPCTSTR)sTo,
					(LPCTSTR)msg->m_sSubject,
					(LPCTSTR)m_sMailerName );
	return true;
}

CString CSMTP::prepare_body(CMailMessage * msg)
{
	ASSERT( msg != NULL );
	CString sTemp;
	CString sCooked = _T("");
	LPTSTR szBad = _T("\r\n.\r\n");
	LPTSTR szGood = _T("\r\n..\r\n");
	int nPos;
	int nStart = 0;
	int nBadLength = _tcsclen( szBad );
	sTemp = msg->m_sBody;
	if( sTemp.Left( 3 ) == _T(".\r\n") )
		sTemp = _T(".") + sTemp;
	//
	// This is a little inefficient because it beings a search
	// at the beginning of the string each time. This was
	// the only thing I could think of that handled ALL variations.
	// In particular, the sequence "\r\n.\r\n.\r\n" is troublesome. 
	// (Even CStringEx's FindReplace wouldn't handle that situation
	// with the global flag set.)
	//
	while( (nPos = sTemp.Find( szBad )) > -1 )
	{
		sCooked = sTemp.Mid( nStart, nPos );
		sCooked += szGood;
		sTemp = sCooked + sTemp.Right( sTemp.GetLength() - (nPos + nBadLength) );
	}
	return sTemp;
}

BOOL CSMTP::transmit_message(CMailMessage * msg)
{
	CString sFrom;
	CString sTo;
	CString sTemp;
	CString sEmail;

	ASSERT( msg != NULL );
	if( !m_bConnected )
	{
		m_sError = _T( "Must be connected" );
		return false;
	}

	// Send the MAIL command
	//
	sFrom.Format( _T("MAIL From: <%s>\r\n"), (LPCTSTR)msg->m_sFrom );
	m_wsSMTPServer.Send( (LPCTSTR)sFrom, sFrom.GetLength() );
	if( !get_response( GENERIC_SUCCESS ) )
		return false;
	
	// Send RCPT commands (one for each recipient)
	//
	for( int i = 0; i < msg->GetNumRecipients(); i++ )
	{
		msg->GetRecipient( sEmail, sTemp, i );
		sTo.Format( _T("RCPT TO: <%s>\r\n"), (LPCTSTR)sEmail );
		m_wsSMTPServer.Send( (LPCTSTR)sTo, sTo.GetLength() );
		get_response( GENERIC_SUCCESS );
	}

	// Send the DATA command
	sTemp = _T("DATA\r\n");
	m_wsSMTPServer.Send( (LPCTSTR)sTemp, sTemp.GetLength() );
	if( !get_response( DATA_SUCCESS ) )
	{
		return false;
	}
	// Send the header
	//
	m_wsSMTPServer.Send( (LPCTSTR)msg->m_sHeader, msg->m_sHeader.GetLength() );

	// Send the body
	//
	sTemp = prepare_body( msg );
	m_wsSMTPServer.Send( (LPCTSTR)sTemp, sTemp.GetLength() );

	// Signal end of data
	//
	sTemp = _T("\r\n.\r\n");
	m_wsSMTPServer.Send( (LPCTSTR)sTemp, sTemp.GetLength() );
	if( !get_response( GENERIC_SUCCESS ) )
	{
		return false;
	}
	return true;
}

BOOL CSMTP::get_response( UINT response_expected )
{
	ASSERT( response_expected >= GENERIC_SUCCESS );
	ASSERT( response_expected < LAST_RESPONSE );

	CString sResponse;
	UINT response;
	response_code* pResp;	// Shorthand

	if( m_wsSMTPServer.Receive( response_buf, RESPONSE_BUFFER_SIZE ) == SOCKET_ERROR )
	{
		m_sError = _T( "Socket Error" );
		return false;
	}
	sResponse = response_buf;
	_stscanf_s( (LPCTSTR)sResponse.Left( 3 ), _T("%d"), &response );
	pResp = &response_table[ response_expected ];
	if( response != pResp->nResponse )
	{
		m_sError.Format( _T("%d:%s"), response, (LPCTSTR)pResp->sMessage );
		return false;
	}
	return true;
}


