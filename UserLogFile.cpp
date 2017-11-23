/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// UserLogFile.cpp : implementation file
//

#include "stdafx.h"
#include <time.h>
#include <string.h> 
#include <ctype.h>
#include <io.h>
#include <direct.h>

#include "kb.h"
#include "Utils.h"
#include "MainFrm.h"
#include "UserLogFile.h"

extern CString appStrVersion;

BOOL bULFDisable = false;
BOOL bULFflag = false;
static CString strUserLogFile = _T("");

void ULF_GetTime(CString& strTime)
{
	_TCHAR sdate[10],stime[10];
	_tstrdate_s(sdate);
	_tstrtime_s(stime);
	strTime.Format(_T("Date: %s  Time: %s\n"),sdate, stime);
}

void ULF_Write(LPCTSTR lpOut)
{
	CFile obFile;
	TRY
	{
		obFile.Open(strUserLogFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
	}
	CATCH( CFileException, e )
	{
#ifdef _DEBUG
		afxDump << _T("UserLogFile could not be opened ") << e->m_cause << _T("\n");
#endif
		return;
	}
	END_CATCH
	obFile.SeekToEnd();
	obFile.Write(lpOut, _tcsclen(lpOut));
	obFile.Flush();
	obFile.Close();
}

void ULF_GetWindowInfo(CString& strOut, HWND hWnd)
{
	CWnd* pWnd = CWnd::FromHandle(hWnd);
	do {
		CString strWnd, strClass, strText;
		strWnd.Format(_T("\tWindow=%x"), pWnd->m_hWnd);

		CRuntimeClass* pRTClass = pWnd->GetRuntimeClass();
		if (pRTClass && pRTClass->m_lpszClassName && *pRTClass->m_lpszClassName)
			strClass.Format(_T("  RunClass=|%s|"), pRTClass->m_lpszClassName);
		strOut += strWnd;
		if (!strClass.IsEmpty())
			strOut += strClass;

		pWnd->GetWindowText(strText);
		if (!strText.IsEmpty()) {
			strText = strText.Left(30);
			CString strT;
			strT.Format(_T("  Text=|%s|"), strText);
			strOut += strT;
		}

		strOut += _T("\n");
		pWnd = pWnd->GetParent();
	} while (pWnd);
}

CString strFolder = _T("C:\\TEMP");
CString strFileName = _T("VTLog");
CString strExt = _T(".txt");

int ULF_GetHighestNum(void)
{
	CFileFind finder;
	CString strPattern;
	int num = 0;

	strPattern.Format(_T("%s\\%s*.%s"), strFolder, strFileName, strExt);
	BOOL bFound = finder.FindFile(strPattern);
	while (bFound) {
		bFound = finder.FindNextFile();
		CString strNumber = finder.GetFileName();
		int pos = strNumber.FindOneOf(_T("0123456789"));
		if (pos != -1) {
			strNumber = strNumber.Mid(pos);
			int numCur = _ttoi(strNumber);
			if (numCur > num)
				num = numCur;
		}
	}
	finder.Close();
	return num;
}

#define ERROR_DISK_FULL_CHECK_FAILED 7001L
int ULF_CheckDiskSpace(LPCTSTR lpFolder)
{
	// Check for sufficient space.
	ULONGLONG ulSpaceNeeded = 1000000;
	CString strDrive = lpFolder;
	strDrive.SetAt(3,0);

	CString cErrorText;
	ULARGE_INTEGER ulFreeBytesAvailableToCaller;
	ULARGE_INTEGER ulTotalNumberofBytes;
	ULARGE_INTEGER ulTotalNumberOfFreeBytes;
	if (ERROR_CALL_NOT_IMPLEMENTED  == GetDiskFreeSpaceEx (strDrive, &ulFreeBytesAvailableToCaller, &ulTotalNumberofBytes, &ulTotalNumberOfFreeBytes))
	{
		DWORD dwSectorsPerCluster;
		DWORD dwBytesPerSector;
		DWORD dwNumberOfFreeClusters;
		DWORD dwTotalNumberOfClusters;
		if (!GetDiskFreeSpace(strDrive,
			&dwSectorsPerCluster,
			&dwBytesPerSector,
			&dwNumberOfFreeClusters,
			&dwTotalNumberOfClusters))
		{
			cErrorText.Format(_T("Disk Space Check FAILED on\n\t%s\nfor User Log File"), strDrive);
			MessageBox(NULL, cErrorText, NULL, MB_OK);
			return ERROR_DISK_FULL_CHECK_FAILED;
		}
		ulFreeBytesAvailableToCaller.QuadPart = (ULONGLONG)dwNumberOfFreeClusters *
			(ULONGLONG)dwSectorsPerCluster *
			(ULONGLONG)dwBytesPerSector;
 
	}
	if (ulFreeBytesAvailableToCaller.QuadPart < ulSpaceNeeded)
	{
		cErrorText.Format(_T("Disk Space Low on\n\t%s\nfor User Log File"), strDrive);
		MessageBox(NULL, cErrorText, NULL, MB_OK);
		return ERROR_DISK_FULL;
	}
	return ERROR_SUCCESS;
}

void AFX_CDECL ULF_Out(LPCTSTR lpszFormat, ...)
{
	if (bULFDisable || !bULFflag)
		return;

	static BOOL bInit = false;
	if (!bInit) {
		bInit = true;
		if (_taccess(strFolder, 0) == -1 && !MakeDirectory(strFolder)) {
			bULFDisable = true;
			return;
		}
		if (ULF_CheckDiskSpace(strFolder) != ERROR_SUCCESS) {
			bULFDisable = true;
			return;
		}
		strUserLogFile.Format(_T("%s\\%s%04.4d.%s"), strFolder, strFileName, ULF_GetHighestNum() + 1, strExt);
		CString strTime;
		ULF_GetTime(strTime);
		CString strInit;
		strInit.Format(_T("%s\n%s\n"),appStrVersion, strTime);
		ULF_Write(strInit);
	}
	ASSERT(AfxIsValidString(lpszFormat));
	va_list argList;
	va_start(argList, lpszFormat);
	CString strOut;
	strOut.FormatV(lpszFormat, argList);
	strOut += _T("\n");
	va_end(argList);

	static HWND hWndFocusLast = 0;
	if (GetFocus() != hWndFocusLast) {
		hWndFocusLast = ::GetFocus();
		CString strInfo = _T("");
		ULF_GetWindowInfo(strInfo, hWndFocusLast);
		if (!strInfo.IsEmpty())
			ULF_Write(strInfo);
	}
	ULF_Write(strOut);
}

