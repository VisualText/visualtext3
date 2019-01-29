/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

/***************************************************/
/* --------------------HISTORY---------------------*/
/*                                                 */
/*  AUTHOR: Dave de Hilster                        */
/*  MODIFICATIONS:                                 */
/*    1/24/2001 Paul Deane - fix function proto-   */
/*        types; minor adjustments to CString      */
/*        partial copying operations; local copies */
/*        of some HTREEITEMs to enable const pass  */
/*        by reference of input items              */
/***************************************************/ 

//#include <curlpp/curlpp.hpp>
//#include <curlpp/Easy.hpp>
//#include <curlpp/Options.hpp>

#include "stdafx.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "lite/lite.h"
#include "io.h"
#include "process.h"
#include "direct.h"
#include "afxinet.h"
#include "afxpriv.h"
#include "mmsystem.h"

#include "kb.h"
#include "Utils.h"
#include "resource.h"
#include "ParseTreeView.h"
#include "MainFrm.h"
#include "CustomMsgDlg.h"
#include "Resource.h"
#include "AskTextDlg.h"
#include "htmlhelp.h"
#include "NLPStrs.h"

extern CString appNameStr;
extern CString appDirectoryStr;
extern CString appAskText;
extern CString appHelpTopic;
extern CString appAnaFilePathStr;
extern CVisualTextApp theApp;
extern bool appBoolSaveLogFiles;
extern bool appBoolTreeSemantics;
extern int appButtonReturn;
extern NLP *nlp;

bool FindFileInSubDirectory(CString filePathStr) { 
   WIN32_FIND_DATA FindFileData;
   HANDLE hFind;

   hFind = FindFirstFile(filePathStr, &FindFileData);
   if (hFind == INVALID_HANDLE_VALUE) {
      return false;
   } 
   else {
      FindClose(hFind);
   }

   return false;
}

CString IndentStr(const int size, const int tabSize)
{
	CString indentStr = _T("");
	int tabCount = 0;

	for (int i=0; i < size; i++) {
		if (tabSize && tabCount++ == tabSize) {
			indentStr += _T("\t");
			tabCount = 1;
		}
		else if (!tabSize)
			indentStr += _T(" ");
	}

	// IF TABSIZE SPECIFIED ADD REMAINING BLANKS
	if (tabCount) {
		if (tabCount == tabSize)
			indentStr += _T("\t");
		else
			for (int i=0; i < tabCount; i++)
				indentStr += _T(" ");
	}

	return indentStr;
}

int StripIndent(CString &indentedStr)
{
	int len = indentedStr.GetLength();
	bool passed = false;
	CString newStr = _T("");
	int strippedCount = 0;

	for (int i=0; i < len; i++) {
		if (passed || indentedStr.GetAt(i) != ' ') {
			newStr += indentedStr.GetAt(i);
			passed = true;
		}
		else
			strippedCount++;
	}
	indentedStr = newStr;

	return strippedCount;
}

bool InitFile(const CString &filePath, const CString &fileName, const CString &subjectStr)
{
	CString contentStr = _T("");
	CString dateStr = SystemDateTime();
	CString authorNameStr = theApp.GetProfileString(_T("PREFERENCES"),_T("AUTHORNAME"),_T(""));

	contentStr += _T("###############################################\n");
	contentStr += _T("# FILE: ") + fileName + _T(".nlp\n");
	contentStr += _T("# SUBJ: ") + subjectStr + _T("\n");
	contentStr += _T("# AUTH: ") + authorNameStr + _T("\n");
	contentStr += _T("# CREATED: ") + dateStr + _T("\n");
	contentStr += _T("# MODIFIED:\n");
	contentStr += _T("###############################################\n");
	contentStr += _T("\n");

	if (fileName == MAINSTR) {
		contentStr += _T("@CODE\n");
		contentStr += _T("L(\"hello\") = 0;\n");	// 06/20/04 AM.
		contentStr += _T("@@CODE\n");
	}
	else {
		contentStr += _T("@CODE\n");
		contentStr += _T("L(\"hello\") = 0;\n");	// 06/20/04 AM.
		contentStr += _T("@@CODE\n\n");
		contentStr += _T("@NODES _ROOT\n");
		contentStr += _T("\n");
		contentStr += _T("@RULES\n");
		contentStr += _T("_xNIL <-\n\t_xNIL\t### (1)\n\t@@\n");
	}

	return NLPWriteFile(filePath, contentStr);
}

// http://www.websina.com/bugzero/kb/unicode-bom.html
// BOM = Byte Order (and Encoding Form) Mark at beginning of a file
#ifndef BOM_STUFF_
#define BOM_STUFF_
char BOM_UTF8x[] = "\xEF\xBB\xBF"; // UTF-8
char BOM_UTF16_LE[] = "\xFF\xFE"; // UTF-16, little-endian (Microsoft standard!)
char BOM_UTF16_BE[] = "\xFE\xFF"; // UTF-16, big-endian (not used)
#endif

#ifdef UNICODE
#define ASCII_7 0 // 0 - 0x7f
#define ASCII_EX 1 // 0 - 0xff
BOOL UTL_IsASCII(LPCWSTR lpwStr, int typeASCII)
{
	int maxVal = ASCII_7 == typeASCII ? 0x7f : 0xff;
	int len = lstrlen(lpwStr);
	for (int n = 0; n < len; n++)
		if ((int)lpwStr[n] > maxVal)
			return false;
	return true;
}

CStringA UTL_GetMBCSFromUnicodeString(LPCWSTR lpwUnicode, UINT nCodePage)
{
	int nChars = wcslen(lpwUnicode) + 1;
	int nMultiByteBufferSize = nChars * 6;  // possible 6 bytes max per character for UTF-8
	wchar_t* pszUnicodeString = new wchar_t[nChars]; 
	char* pszMultiByteString = new char[nMultiByteBufferSize];  

	lstrcpy(pszUnicodeString, lpwUnicode);

	int nCharsWritten = 0;
	if (pszUnicodeString && pszMultiByteString) {
		ZeroMemory(pszMultiByteString, nMultiByteBufferSize);
		if (nCodePage == -1) {
			BOOL bASCII_Ex = UTL_IsASCII(lpwUnicode, ASCII_EX);
			nCodePage = !bASCII_Ex ? CP_UTF8 : CP_ACP;
		}

		try  {
			nCharsWritten = WideCharToMultiByte(nCodePage, 0,
				pszUnicodeString,-1, pszMultiByteString, nMultiByteBufferSize, NULL, NULL);
		}
		catch(...) {
			TRACE(_T("Controlled exception in WideCharToMultiByte!\n"));
		}
	} 
	CStringA strMBCS = CStringA((CP_UTF8 == nCodePage ? BOM_UTF8x : "")) + pszMultiByteString;
	if (pszUnicodeString)
		delete [] pszUnicodeString;
	if (pszMultiByteString)
		delete [] pszMultiByteString;
	return strMBCS;
}
#endif

bool NLPWriteFile(const CString &fileName, const CString &contentStr)
{
	CFile f;
	if (!(f.Open(fileName,CFile::modeCreate | CFile::modeWrite))) {
#ifdef _DEBUG
		afxDump << _T("File could not be opened ") << fileName << _T("\n");
#endif
		return false;
	}

#ifdef UNICODE
	CStringA strMBCS = UTL_GetMBCSFromUnicodeString(contentStr, -1);
	f.Write((const void*)strMBCS, strMBCS.GetLength());
#else
	int len = contentStr.GetLength();
	f.Write(contentStr,len);
#endif
	f.Close();

	return true;
}

bool AppendToFile(const CString &fileName, const CString &appendStr)
{
	CString contentStr = NLPReadFile(fileName);
	contentStr += appendStr;
	NLPWriteFile(fileName,contentStr);
	return true;
}

bool AppendFileToFile(const CString &targetFileStr, const CString &sourceFileStr)
{
	FILE *fpTarget;
	errno_t err;
	if (!(err = _tfopen_s(&fpTarget,(const _TCHAR *)targetFileStr, _T("r"))) != 0 ) {
#ifdef _DEBUG
		afxDump << _T("File could not be opened ") << targetFileStr << _T("\n");
#endif
		return false;
	}

	FILE *fpSource;
	if (!(err = _tfopen_s(&fpSource,(const _TCHAR *)sourceFileStr, _T("r"))) != 0 ) {
#ifdef _DEBUG
		afxDump << _T("File could not be opened ") << sourceFileStr << _T("\n");
#endif
		return false;
	}

	_TCHAR buff[10001];
	while (_fgetts(buff,10000,fpSource)) {
		_fputts(buff,fpTarget);
	}

	fclose(fpTarget);
	fclose(fpSource);

	return true;
}

long GetFileLength(const CString &fileNameStr)
{
	struct _stat buf;
	long fileSize = -1;
	_TCHAR fileChar[1001];
	StringToChar(fileChar,fileNameStr,1000);
	int result = _tstat(fileChar, &buf);
	if (result == 0) {
		fileSize = buf.st_size;
	}
	return fileSize;
}

bool FileExists(const CString &fileName)
{
	FILE *fp;
	errno_t err = _tfopen_s(&fp,(const _TCHAR *)fileName, _T("r"));
	if (!fileName || fileName == _T("") || err != 0) {
		return false;
	}
	fclose(fp);
	return true;
}

CString UTL_GetUnicodeFromMBCS(LPCTSTR lpszMBCS, UINT nCodePage)
{
	int		nChars = lstrlen(lpszMBCS) + 1;
	int		nBufferSize = nChars * sizeof(wchar_t);
	wchar_t* pszUnicodeString = new wchar_t[nChars];
	char*	pszMultiByteString = new char[nChars * 2]; // why *2 ? keeps app from crashing when delete [] called below
	int n = 0;

	for (n = 0; n < nChars; n++)
		pszMultiByteString[n] = (char)lpszMBCS[n];
	pszMultiByteString[n] = 0;
	int		nCharsWritten = 0;
	if (pszUnicodeString && pszMultiByteString)
	{
		if (nCodePage == -1)
			nCodePage = CP_UTF8;
		try  {
			ZeroMemory((void*)pszUnicodeString, nBufferSize);
			nCharsWritten = MultiByteToWideChar(nCodePage,0,pszMultiByteString,-1,pszUnicodeString,nBufferSize);
		}
		catch(...)
		{
			TRACE(_T("Controlled exception in MultiByteToWideChar!\n"));
		}
	}
	CString strUnicode = CString(pszUnicodeString);
	if (pszMultiByteString)
		delete [] pszMultiByteString;
	if (pszUnicodeString)
		delete [] pszUnicodeString;
	return strUnicode;
}

// Want to read a converted file from a specialized input pass.	// 08/20/08 AM.
CString NLPReadInputFile(const CString &fileName)	// 08/20/08 AM.
{
CString reTextStr = fileName + _T("_log") + _T("\\retext.txt");
if (FileExists(reTextStr))
	return NLPReadFile(reTextStr);
else
	return NLPReadFile(fileName);
}


CString NLPReadFile(const CString &fileName)
{
	CString contentStr;
	_TCHAR fileNameBuff[1001];
	StringToChar(fileNameBuff,fileName,1000);
	_TCHAR *contentBuff;
	long fileLength = -1;

	nlp->readConvert(fileNameBuff,contentBuff,fileLength,FILE_BUFF_MAX);

	contentStr = contentBuff;

	nlp->deleteBuf(contentBuff);
	//delete contentBuff;
	if (contentStr.GetLength() > 2) {
		char bom[4];
		ZeroMemory(bom,sizeof(bom));
		for (int n = 0; n < 3; n++)
			bom[n] = (char)contentStr.GetAt(n);
		if (!memcmp(bom, BOM_UTF8x, 3)) {
			contentStr = contentStr.Mid(3);
			contentStr = UTL_GetUnicodeFromMBCS(contentStr, CP_UTF8);
		}
	}

	return contentStr;
}

void NLPFileTextWidthLength(const CString &fileName, long &width, long &length, long &fileLength)
{
	_TCHAR fileNameBuff[1001];
	StringToChar(fileNameBuff,fileName,1000);
	_TCHAR *contentBuff;
	fileLength = -1;

	nlp->readConvert(fileNameBuff,contentBuff,fileLength,FILE_BUFF_MAX);

	_TCHAR *c = contentBuff;
	int lineLength = width = length = 0;

	for (long l=0; l<fileLength; l++) {
		if (*c == '\n') {
			length++;
			if (lineLength > width)
				width = lineLength;
		    lineLength = 0;
		}
		else
		    lineLength++;
		*c++;
	}
	if (lineLength > width)
		width = lineLength;
	if (fileLength)
		length++;

	//delete contentBuff;
	nlp->deleteBuf(contentBuff);
}

//inputs made const, objects passed by reference PD 01/24/2001
void ReadFileToStringList(CStringList &wordList, const CString &fileName)
{ //no longer CStringList* PD 02/14/2001
	if (fileName == _T("")) return ;
	FILE *fp;
	errno_t err;
	if ((err = _tfopen_s(&fp,(const _TCHAR *)fileName, _T("r") ))) {
#ifdef _DEBUG
		afxDump << _T("File could not be opened ") << fileName << _T("\n");
#endif
		return ;
	}

	_TCHAR line[10001];
	CString lineStr = _T("");
	int lastLetter;

	while (_fgetts(line,10000,fp)) {
		lastLetter = _tcsclen(line)-1;
		if (line[lastLetter] == '\n')
			line[lastLetter] = '\0';
		lineStr = line;
		wordList.AddTail(line);
	}

	fclose(fp);
}

CString AnalyzerSequencePath()
{
	return AnalyzerSequencePath(_T("analyzer"));
}

CString AnalyzerSequencePath(const CString &anaNameStr)
{
	return AppPath(_T("spec"),anaNameStr,_T("seq"));
}

CString TemplateDirectory(CString fileStr)
{
	CString pathStr = _T("");
	size_t len;

#ifdef _UNICODE
	wchar_t taiPath[500]; 
	_wgetenv_s( &len, taiPath, 500, L"VISUALTEXT" ); 
#else
	_TCHAR *taiPath = NULL;
	_dupenv_s(&taiPath, &len, _T("VISUALTEXT"));
#endif

	if (taiPath != _T("")) {
		pathStr.Format(_T("%s\\Templates"),taiPath);
		if (fileStr != _T(""))
			pathStr += _T("\\") + fileStr;
	}
	else
//		AfxMessageBox("Environment variable \"%s\" is not set");	// 07/10/02 AM.
		AfxMessageBox(_T("Environment variable 'VISUALTEXT' not set"));	// 07/10/02 AM.
	return pathStr;
}

void SetAppName() {
	appNameStr = StripPath(appAnaFilePathStr,0,true);
	appNameStr = StripPath(appNameStr,0);
}

CString AppPath()
{
	return appDirectoryStr + _T("\\") + appNameStr;
}

CString AppPath(const LPCTSTR dirStr)
{
	if (dirStr != _T("")) return AppPath() + _T("\\") + dirStr;
	return AppPath();
}

CString AppPath(const CString &dirStr, const CString &fileName, const CString &extStr)
{
	CString returnStr = AppPath(dirStr);
	if (fileName != _T("")) returnStr += _T("\\") + fileName;
	if (extStr != _T("")) returnStr += _T(".") + extStr;
	return returnStr;
}

CString AppPathRel()
{
	return _T("\\");
}

CString AppPathRel(const CString &dirStr)
{
	if (dirStr != _T("")) return AppPathRel() + dirStr;
	return AppPathRel();
}

CString AppPathRel(const CString &dirStr, const CString &fileName, const CString &extStr)
{
	CString returnStr = AppPathRel(dirStr);
	if (fileName != _T("")) returnStr += _T("\\") + fileName;
	if (extStr != _T("")) returnStr += _T(".") + extStr;
	return returnStr;
}

CString KBPath(const CString &dirStr)
{
	return AppPath(_T("kb"),dirStr);
}

CString KBPathRel(const CString &dirStr)
{
	return AppPathRel(_T("kb"),dirStr);
}

CString GramPath(const CString &fileName, const CString &extStr)
{
	return AppPath(_T("gram"),fileName, extStr);
}

CString GramPathRel(const CString &fileName, const CString &extStr)
{
	return AppPathRel(_T("gram"),fileName, extStr);
}

CString InputPath(const CString &fileName, const CString &extStr)
{
	return AppPath(_T("input"),fileName, extStr);
}

CString InputPathRel(const CString &fileName, const CString &extStr)
{
	return AppPathRel(_T("input"),fileName, extStr);
}

CString OutputPath(const CString &fileName, const CString &extStr)
{
	return AppPath(_T("output"),fileName, extStr);
}

CString OutputPathRel(const CString &fileName, const CString &extStr)
{
	return AppPathRel(_T("output"),fileName, extStr);
}

CString LogsPath(const CString &fileName, const CString &extStr) // 05/19/01 AM.
{
	return AppPath(_T("logs"),fileName, extStr); // 05/19/01 AM.
}

CString LogsPathRel(const CString &fileName, const CString &extStr) // 05/19/01 AM.
{
	return AppPathRel(_T("logs"),fileName, extStr); // 05/19/01 AM.
}

CString RulesPath(const CString &fileName, const bool noExt, const CString &extStr)
{
	if (noExt || fileName == _T("")) return AppPath(_T("spec"),fileName);
	return AppPath(_T("spec"),fileName) + extStr;
}

CString RulesPathRel(const CString &fileName, const bool noExt)
{
	if (noExt || fileName == _T("")) return AppPathRel(_T("spec"),fileName);
	return AppPathRel(_T("spec"),fileName) + _T(".nlp");
}

bool MakeDirectory(const CString &dirPathStr)
{
	if (IsDirectory(dirPathStr))
		return true;

	CStringList pathPieces;
	TokensBySeparator(pathPieces,dirPathStr,'\\');
	CString makePathStr = _T("");
	CString pieceStr;

	int i = 0;
	while ((pieceStr = NthStringListItem(pathPieces,i++)) != _T("")) {
		if (makePathStr == _T(""))
			makePathStr = pieceStr;
		else
			makePathStr += _T("\\") + pieceStr;
		if (!IsDrive(makePathStr) &&
			!IsDirectory(makePathStr) &&
			_tmkdir((const _TCHAR *)makePathStr) != 0) {
			return false;
		}
	}
	return true;
}

bool IsDrive(const CString &pathStr)
{
	_TCHAR letter = pathStr.GetAt(0);
	bool isDriveLetter = (letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z');
	return (pathStr.GetLength() == 2 && isDriveLetter);
}

CString MakeLogDirectory(const CString &fileName)
{
	CString pathStr = fileName + _T("_log");
	_TCHAR buff[1001];
	StringToChar(buff,pathStr,1000);
	_tmkdir(buff);
	return pathStr;
}

CString OutAnaPath(const CString &pathStr, const int passNum)
{
	CString preStr = pathStr + _T("\\ana");
	if (passNum < 100) preStr += _T("0");
	if (passNum < 10) preStr += _T("0");
	CString returnStr = _T("");
	returnStr.Format(_T("%s%d.log"),preStr,passNum);
	return returnStr;
}

CString OutAnaPath(const int passNum)
{
	CString preStr = appDirectoryStr + _T("\\") + appNameStr + _T("\\output");
	return OutAnaPath(preStr,passNum);
}

CString AnalyzerFile()
{
	return AnalyzerFile(appNameStr);
}

CString AnalyzerFile(const CString &nameStr)
{
	return AppPath() + _T("\\") + nameStr + _T(".ana");
}

CString RelativizePath(const CString &dirPathStr, const CString &removeStr)
{
	CString relPathStr = dirPathStr;
	CString appPathStr = AppPath();

	CString appPathStrLower = appPathStr;
	CString dirPathStrLower = dirPathStr;

	appPathStrLower.MakeLower();
	dirPathStrLower.MakeLower();

	if (dirPathStrLower.Find(appPathStrLower) == 0) {
		ClipLeft(relPathStr,appPathStr.GetLength()+1);
	}
	/*
	else if ((pos = dirPathStr.Find("\\input\\")) > 0) {
		relPathStr = dirPathStr.Mid(pos+1,dirPathStr.GetLength()-pos);
	}
	*/

	return relPathStr;
}

CString FullPath(const CString &relPathStr)
{
	return AppPath() + _T("\\") + relPathStr;
}

int GetDocumentCount()
{	
	CWinApp *wp = AfxGetApp();
	POSITION pos = wp->GetFirstDocTemplatePosition();
	CDocTemplate *pSelectedTemplate;
	POSITION posDoc;
	int count = 0;
	CDocument *doc;

	while (pos != NULL) {
		pSelectedTemplate = (CDocTemplate*)wp->GetNextDocTemplate(pos);
		ASSERT(pSelectedTemplate != NULL);
		posDoc = pSelectedTemplate->GetFirstDocPosition();
		while (posDoc != NULL) {
			count++;
			doc = pSelectedTemplate->GetNextDoc(posDoc);
		}
	}
	return count;
}

CString SystemDateTime()
{
	CTime timeWrite;
	timeWrite = CTime::GetCurrentTime();

	CString str = timeWrite.Format(_T("%d/%b/%y %H:%M:%S"));
	return str;
}

bool EndsWiths(const CString &fileName, const CStringList &extStrs)
{
	POSITION pos = extStrs.GetHeadPosition();
	CString ext = _T("");

	while (pos && (ext = extStrs.GetNext(pos))) {
		if (EndsWith(fileName,ext))
			return true;
	}
	return false;
}

CString RemoveExtension(const CString &fileName)
{
	int pos = fileName.ReverseFind('.');
	if (pos >= 0) return fileName.Left(pos);
	return fileName;
}

bool EndsWith(const CString &str, const CString &endingStr)
{
	int len1 = str.GetLength();
	int len2 = endingStr.GetLength();
	int diff = len1 - len2;
	if (diff < 0) return false;

	int pos = str.Find(endingStr, diff);
	if (pos == -1) return false;
	return diff == pos;
}

CString StripPath(const CString &fileName, const int level, const bool leftSideFlag, const _TCHAR separator)
{
	CString tempString ;
	CString fileNameLeft = fileName;
	int pos;

	for (int i=0; i <= level; i++) {
		pos = fileNameLeft.ReverseFind(separator);
		tempString = fileNameLeft.Left(pos);
		fileNameLeft = tempString;
	}
	CString fileNameStripped = fileName.Right(fileName.GetLength()-pos-1);
	if (leftSideFlag) return fileNameLeft;
	return fileNameStripped;
}

CString StripPathReverse(const CString &fileName, const int level, const bool rightSideFlag, const _TCHAR separator)
{
	CString fileNameRight = fileName;
	CString tempBuf ;
	int pos;

	for (int i=0; i <= level; i++) {
		pos = fileNameRight.Find(separator);
		tempBuf = fileNameRight.Right(fileNameRight.GetLength()-pos-1);
		fileNameRight = tempBuf ;
	}
	CString fileNameStripped = fileName.Left(pos);
	if (rightSideFlag) return fileNameRight;
	return fileNameStripped;
}

int NthPathSegment(const CString &strIn, CString &strOut, const _TCHAR splitter, const int nth)
{
	CString strInRight = strIn;
	CString tempBuf ;
	int pos = strInRight.Find(splitter);
	int i = 0;

	for (i=0; i<nth && pos>=0; i++) {
		tempBuf = strInRight.Right(strInRight.GetLength()-pos-1); 
		strInRight = tempBuf ;
		pos = strInRight.Find(splitter);
	}
	if (pos != -1)
		strOut = strInRight.Left(pos);
	else if (i < nth)
		strOut = _T("");
	else
		strOut = strInRight;

	strOut.TrimLeft();
	strOut.TrimRight();

	// FOR THE LAST EDGE CASE.  WE WANT TO RETURN NON -1 WHEN
	// IT IS THE LAST ONE FOUND.
	if (pos == -1 && strOut != _T(""))
		pos = 0;

	return pos;
}

int NthPathSegmentInt(const CString &strIn, int &intOut, const _TCHAR splitter, const int nth)
{
	intOut = -1;
	CString strOut;
	int pos = NthPathSegment(strIn,strOut,splitter,nth);
	if (pos >= 0 && strOut != _T(""))
		intOut = _ttoi((const _TCHAR *)strOut);
	return pos;
}

int NthPathSegmentBool(const CString &strIn, bool &boolOut, const _TCHAR splitter, const int nth)
{
	boolOut = false;
	CString strOut;
	int pos = NthPathSegment(strIn,strOut,splitter,nth);
	if (pos >= 0 && strOut != _T(""))
		boolOut = _ttoi((const _TCHAR *)strOut) ? true : false;
	return pos;
}

int NthPathSegmentLong(const CString &strIn, long &longOut, const _TCHAR splitter, const int nth)
{
	longOut = -1;
	CString strOut;
	int pos = NthPathSegment(strIn,strOut,splitter,nth);
	if (pos >= 0 && strOut != _T(""))
		longOut = _ttol((const _TCHAR *)strOut);
	return pos;
}

BOOL IsDirectory(const CString &pathStr)
{
	DWORD Code = GetFileAttributes(pathStr);

	return (Code != -1) && (Code & FILE_ATTRIBUTE_DIRECTORY); 
} 

bool IsDirectoryEmpty(const CString &pathStr)
{
	CFileFind finder;
	BOOL bWorking = finder.FindFile(pathStr+_T("\\*.*"));
	CString fileNameStr = _T("");
	CString filePathStr = _T("");
	bool recurseReturn = true;

	while (bWorking) {
		bWorking = finder.FindNextFile();
		fileNameStr = finder.GetFileName();
		filePathStr = finder.GetFilePath();
		if (fileNameStr == _T(".") || fileNameStr == _T(".."))
			continue;

		if (finder.IsDirectory()) {
			recurseReturn = IsDirectoryEmpty(filePathStr);
			if (!recurseReturn)
				return false;
		}
		else
			return false;
	}
	finder.Close();

	return true;
}

BOOL AnyFilesHaveExtension(const CString &extStr, const CString &pathStr)
{
	CFileFind finder;
	return finder.FindFile(pathStr + _T("\\*.") + extStr);
}

BOOL ChangeFilesExtensions(const CString &pathStr, const CString &fromExtStr, const CString &toExtStr)
{
	CFileFind finder;
	BOOL found = finder.FindFile(pathStr + _T("\\*.") + fromExtStr);
	int count = 0;
	CString fileName;

	while (found) {
		found = finder.FindNextFile();
		fileName = finder.GetFileName();
		if (fileName == _T(".") || fileName == _T("..") || finder.IsDirectory())
			continue;
		count++;
	}
	if (count == 0) {
		AfxMessageBox(_T("No files to convert"));
		return true;
	}

	CString msgStr;
	msgStr.Format(_T("Do you want to rename the %i *.pat files to *.nlp?"), count);
	if (AfxMessageBox(msgStr, MB_YESNO) != IDYES) return false;

	CString fromFilePath = _T("");
	CString toFilePath = _T("");
	BOOL renamed = false;
	CMainFrame *mainFrm = (CMainFrame *)AfxGetMainWnd();
	found = finder.FindFile(pathStr + _T("\\*.") + fromExtStr);

	while (found) {
		found = finder.FindNextFile();
		fileName = finder.GetFileName();
		if (fileName == _T(".") || fileName == _T("..") || finder.IsDirectory())
			continue;

		fromFilePath = pathStr + _T("\\") + fileName;
		toFilePath = pathStr + _T("\\") + finder.GetFileTitle() + _T(".") + toExtStr;

		if (!FileExists(toFilePath) && !UTL_Rename(fromFilePath, toFilePath)) {
			break;
		}
		msgStr.Format(_T("%s => %s"),fromFilePath, finder.GetFileTitle() + _T(".") + toExtStr);
		mainFrm->AddDebugLine(_T("Renamed:"), msgStr);

		renamed = true;
		break;
	}

	finder.Close();

	return renamed;
}

void DirectoryFiles(CStringList &files, const CString &pathStr, const bool fullPath,
					const CString &extStr, const bool includeDirFlag)
{//no longer CStringList* PD 02/14/2001
	CFileFind finder;
	CString xStr = extStr;
	if (xStr == _T(""))
		xStr = _T("*.*");
	BOOL bWorking = finder.FindFile(pathStr+_T("\\")+xStr);
	CString fileName;
	CString fullPathStr = _T("");
	bool fullPathFlag = fullPath;
	if (includeDirFlag)
		fullPathFlag = true;

	while (bWorking) {
		bWorking = finder.FindNextFile();
		fileName = finder.GetFileName();
		if (fileName == _T(".") || fileName == _T("..") ||
			(!includeDirFlag && finder.IsDirectory()))
			continue;

		fullPathStr = fileName;
		if (fullPathFlag)
			fullPathStr = pathStr + _T("\\") + fileName;
		files.AddTail(fullPathStr);

		if (includeDirFlag)
			DirectoryFiles(files,fullPathStr,true,extStr,true);
	}
	finder.Close();
}

bool RemoveDirectory(const CString &dirPath)
{
	int returnValue = _trmdir((const _TCHAR *)dirPath);
	return returnValue < 0 ? false : true;
}

void NLPCopyFile(const CString &origFileStr, const CString &destFileStr)
{
	CString contentStr = NLPReadFile(origFileStr);
	NLPWriteFile(destFileStr,contentStr);
}

BOOL UTL_Rename(LPCTSTR oldName, LPCTSTR newName)
{
	if (0 != _trename(oldName, newName)) {
		CString strMsg = _T("");
		switch (errno) {
			case EACCES:
				strMsg.Format(_T("File or directory specified by\n")
					_T("%s already exists or could not be created (invalid path);\n\n")
					_T("%s\n")
					_T("\tis a directory and\n")
					_T("%s\n")
					_T("specifies a different path."),
					newName, oldName, newName);
				break;
			case ENOENT:
				strMsg.Format(_T("File or path specified by\n")
					_T("%s\n")
					_T("not found."),
					oldName);
				break;
			case EINVAL:
				strMsg.Format(_T("File/Path Names contain invalid characters"));
				break;
			default:
				strMsg.Format(_T("Rename Failed: Error Number %d"), errno);
				break;
		}
		AfxMessageBox(strMsg);
		return false;
	}
	return true;
}

void CopyDirectory(const CString &origPathStr, const CString &destPathStr, const CStringList &extLists)
{
	CFileFind finder;
	BOOL bWorking = finder.FindFile(origPathStr+_T("\\*.*"));
	CString fileName;
	CString fullPathStr;
	CString subDestPathStr;

	while (bWorking) {
		PeekHere(10);

		bWorking = finder.FindNextFile();
		fileName = finder.GetFileName();
		if (fileName == _T(".") || fileName == _T("..") ||EndsWiths(fileName,extLists))
			continue;

		fullPathStr = origPathStr + _T("\\") + fileName;
		subDestPathStr = destPathStr + _T("\\") + fileName;
		MakeDirectory(destPathStr);
		CopyDirectory(fullPathStr,subDestPathStr,extLists);
	}
	finder.Close();

	if (IsDirectory(origPathStr))
		MakeDirectory(destPathStr);
	else
		CopyFile(origPathStr,destPathStr,false);
}

bool AskForText(const CString &msgStr, const CString &startStr)
{
	appAskText = msgStr;
	CAskTextDlg askDlg;
	askDlg.m_strUserText = startStr;
	askDlg.DoModal();

	if (appAskText == _T("")) return false;
	return true;
}

bool RecursiveRemove(const CString &pathStr, const bool childrenOnlyFlag, int level, const bool debugMsgFlag)
{
	if (!pathStr || pathStr == _T(""))
		return false;

	CFileFind finder;
	BOOL bWorking = finder.FindFile(pathStr+_T("\\*.*"));
	CString fileName = _T("");
	CString fullPathStr = _T("");

	// Printout to Debug window
	if (debugMsgFlag && level >= 0) {
		CMainFrame *mainFrm = (CMainFrame *)AfxGetMainWnd();
		//if (level == 0 && bWorking)
			//mainFrm->ClearOutput();
		CString indentStr = IndentStr(level*3,0);
		mainFrm->AddDebugLine(indentStr+_T("Deleting"),pathStr);
	}

	int okFlag;
	if (level >= 0)
		level++;
	while (bWorking) {
		bWorking = finder.FindNextFile();
		fileName = finder.GetFileName();
		if (fileName == _T(".") || fileName == _T(".."))
			continue;

		fullPathStr = pathStr + _T("\\") + fileName;
		okFlag = RecursiveRemove(fullPathStr,false,level,debugMsgFlag);
		if (okFlag < 0) {
			finder.Close();
			return false;
		}
	}
	finder.Close();

	bool returnValue = true;

	if (!childrenOnlyFlag) {
		if (IsDirectory(pathStr)) {
			returnValue = DeleteDirectory(pathStr);
#ifdef _DEBUG
			if (!returnValue) {
				DWORD dwErr = GetLastError();
				LPVOID lpMsgBuf;
				FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,
					dwErr,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR)&lpMsgBuf, 0, NULL);
				CString cMsg;
				cMsg.Format (_T("Remove Directory: %s"), pathStr);
				AfxGetMainWnd()->MessageBox((LPTSTR)lpMsgBuf, cMsg, MB_OK|MB_ICONINFORMATION );
				LocalFree( lpMsgBuf );
			}
#endif
		}
		else if (FileExists(pathStr)) {
			returnValue = DeleteFile(pathStr) ? true : false;
		}
	}

	return returnValue;
}

bool DeleteDirectory(LPCTSTR lpszDir, bool noRecycleBin)
{
  int len = _tcslen(lpszDir);

#ifdef _UNICODE
    wchar_t pszFrom[1000];
    wcscpy_s(pszFrom, (wchar_t *)lpszDir);
#else
	TCHAR *pszFrom = new TCHAR[len+2];
	strcpy_s(pszFrom, len+1, lpszDir);
#endif

  pszFrom[len] = 0;
  pszFrom[len+1] = 0;
  
  SHFILEOPSTRUCT fileop;
  fileop.hwnd   = NULL;    // no status display
  fileop.wFunc  = FO_DELETE;  // delete operation
  fileop.pFrom  = pszFrom;  // source file name as double null terminated string
  fileop.pTo    = NULL;    // no destination needed
  fileop.fFlags = FOF_NOCONFIRMATION|FOF_SILENT;  // do not prompt the user
  
  if(!noRecycleBin)
    fileop.fFlags |= FOF_ALLOWUNDO;

  fileop.fAnyOperationsAborted = FALSE;
  fileop.lpszProgressTitle     = NULL;
  fileop.hNameMappings         = NULL;

  int ret = SHFileOperation(&fileop);
  delete [] pszFrom;  
  return (ret == 0);
}

void HelpTopic(const CString &topicStr, DWORD dwContextID)
{
	HWND hwndCaller = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	size_t len;

#ifdef _UNICODE
	wchar_t taiPath[500]; 
	_wgetenv_s( &len, taiPath, 500, L"VISUALTEXT" ); 
#else
	_TCHAR *taiPath = NULL;
	_dupenv_s(&taiPath, &len, _T("VISUALTEXT"));
#endif

	CString pathStr = taiPath;
	pathStr += _T("\\Help\\Help.chm");

	if (pathStr == _T("")) {
		CString msgStr = _T("Couldn't find VISUALTEXT environment variable.");
		AfxMessageBox(msgStr);
		return;
	}
	else if (!FileExists(pathStr)) {
		CString msgStr = _T("Couldn't find VisualText Help File.");
		AfxMessageBox(msgStr);
		return;
	}
	if (dwContextID) {
		HtmlHelp(GetDesktopWindow(),pathStr,HH_HELP_CONTEXT,dwContextID);
//	HtmlHelp(GetDesktopWindow(),pathStr,HH_DISPLAY_TOPIC,(DWORD)"Knowledge_Base_Manual.htm");
	}
	else {
		if (topicStr && topicStr != _T(""))
			pathStr += _T("::/")+topicStr+_T(".htm");
		pathStr += _T(">VisualText");
		HtmlHelp(GetDesktopWindow(),pathStr,HH_DISPLAY_TOPIC,NULL);
	}
}

bool IsInList(_TCHAR *list[], CString wordStr, CString endStr, BOOL bCaseSensitive)
{
	_TCHAR *word = list[0];
	CString wStr = word;
	int i = 0;

	if (bCaseSensitive) {
		while (wStr.Compare(endStr)) {
			if (!wStr.Compare(wordStr))
				return true;
			word = list[i++];
			wStr = word;
		}
	}
	else {
		while (wStr.CompareNoCase(endStr)) {
			if (!wStr.CompareNoCase(wordStr))
				return true;
			word = list[i++];
			wStr = word;
		}
	}

	return false;
}

int FindMenuItem(CMenu* Menu, LPCTSTR MenuString)
{
   ASSERT(Menu);
   ASSERT(::IsMenu(Menu->GetSafeHmenu()));

   int count = Menu->GetMenuItemCount();
   for (int i = 0; i < count; i++)
   {
      CString str;
      if (Menu->GetMenuString(i, str, MF_BYPOSITION) &&
		  str.Compare(MenuString) == 0) {

		 Menu->EnableMenuItem(i, MF_BYPOSITION | MF_ENABLED);
         return i;
	  }
   }

   return -1;
}

void PopupContextMenu(CWnd *wnd, const int nID, const CPoint &point)
{
	CMenu menu;
	menu.LoadMenu(nID);
	CMenu *context = menu.GetSubMenu(0);
	context->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y, wnd);
}

void PopupContextMenu(CWnd *wnd, const int nID, const CPoint &point, UINT disables[])
{
	CMenu menu;
	menu.LoadMenu(nID);
	CMenu *context = menu.GetSubMenu(0);

	// GRAY OUT IF DISABLED
	for (int i=0; i<100; i++) {
		if (disables[i]) {
			context->EnableMenuItem(disables[i],MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
		}
	}
	context->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y, wnd);
}

void PopupContextMenu(CWnd *wnd, CMenu *context, const CPoint &point, UINT disables[])
{
	// GRAY OUT IF DISABLED
	for (int i=0; i<100; i++) {
		if (disables[i]) {
			context->EnableMenuItem(disables[i],MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
		}
	}
	context->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y, wnd);
}

void ClipLeft(CString &inStr, const int clipping)
{
	inStr = inStr.Right(inStr.GetLength()-clipping);
}

void ClipRight(CString &inStr, const int clipping)
{
	inStr = inStr.Left(inStr.GetLength()-clipping);
}

int FileContains(CString filePathStr, CString searchStr, CString &lineStr, const int startLine,
				 int &inLineIndex, const bool noCase, const bool multi)
{
	CString cStr = NLPReadFile(filePathStr);
	int len = cStr.GetLength();
	int lineIndex = 0;
	int lineIndex2 = 0;
	int lineNum = 0;
	CString srchStr = searchStr;  // Restore old.   // 03/20/06 AM.
	if (noCase)                   // Restore old.   // 03/20/06 AM.
		srchStr.MakeLower();       // Restore old.   // 03/20/06 AM.
	CString contentStr = _T("");  // Restore to preserve case.  // 03/20/06 AM.
	CString contStr;              // Restore to preserve case.  // 03/20/06 AM.

	if (!multi)	         // Not looking for multiple per line.  // 03/20/06 AM.
		inLineIndex = -1; // RESET for next line.                // 03/20/06 AM.

	while (lineIndex2 >= 0) {
		lineNum++;
		lineIndex2 = cStr.Find(_T("\n"), lineIndex);
		// FIX. Change to greater-equal zero.	// 03/20/06 AM.
		if (lineNum <= startLine && !(lineNum == startLine && inLineIndex >= 0)) {
			lineIndex = lineIndex2 + 1;
			continue;
		}
		if (lineIndex2 == -1)
			lineStr = cStr.Mid(lineIndex);
		else
			lineStr = cStr.Mid(lineIndex, lineIndex2 - lineIndex);
		if (noCase == false)
			inLineIndex = lineStr.Find(searchStr, inLineIndex + 1);
		else
			{
			contentStr = lineStr;             // FIX. Preserve display case.  // 03/20/06 AM.
			contStr = contentStr.MakeLower(); // FIX. Preserve display case.  // 03/20/06 AM.
			inLineIndex = contStr.Find(srchStr, inLineIndex + 1);
			}
		if (inLineIndex >= 0)
			return lineNum;
		inLineIndex = -1;	// FIX. Reset for next line.	// 03/20/06 AM.
		lineIndex = lineIndex2 + 1;
		if (lineIndex2 > len)
			break;
	}

	return 0;
}

// Rewrote for optimization.	// OPT.	// 11/29/06 AM.
// str = caller must set to strIn. Returns position in string after indent.
int IndentSize(
//	const CString &strIn,
	/*DU*/ _TCHAR *&str,	// OPT.	// 11/30/06 AM.
	const int tabSize
	)
{
	if (!str || !*str)	// OPT.	// 11/29/06 AM.
		return 0;
	int siz = 0;
	_TCHAR c;
	--str;					// OPT.	// 11/29/06 AM.
	while (c = *++str)	// OPT.	// 11/29/06 AM.
		{
		switch (c)			// OPT.	// 11/29/06 AM.
			{
			case ' ':
				++siz;
				break;
			case '\t':
				if (tabSize)
					siz += tabSize;
				else
					++siz;
				break;
			default:
				return siz;
			}
		}
	return siz;
}

// Brought back the old one.	// 05/21/07 AM.
int IndentSize(const CString &strIn, const int tabSize)
{
	int len = strIn.GetLength();
	int indentSize = 0;
	_TCHAR c;	// FIX.	// 06/04/07 AM.

	for (int i=0; i<len; i++) {
		c = strIn.GetAt(i);
		if (c == '\t' && tabSize)
			indentSize += tabSize;
		else if (c != ' ')
			break;
		else
			indentSize++;
	}
	return indentSize;
}


CView *GetFirstView(CDocument *doc)
{
	if (!doc) return NULL;
	POSITION docViewPos = doc->GetFirstViewPosition();
	return doc->GetNextView(docViewPos);
}

CString NormalizeURL(CString urlStr) {
	if (urlStr.Find(_T("http://")) < 0) {
		if (urlStr.Find(_T("www.")) == 0) {
			urlStr = _T("http://") + urlStr;
		}
		else if (urlStr.Find(_T(".")) > 0) {
			urlStr = _T("http://www.") + urlStr;
		}
	}
	return urlStr;
}

#define MAXBUF 1000



bool FetchURLString(const CString &urlStr, CString &htmlTextStr)
{
	_TCHAR* buffer = new _TCHAR[MAXBUF];
	UINT nBytesRead = 0;
	htmlTextStr = _T("<!--") + urlStr + _T("-->\n");


// PD 02/19/2001 PD fixes, part 1
// This function in its original form has no protection if we hit a link
// to a file which is not a text file at all, but is in an image format or
// in an unsupported text format like .pdf. It also doesn't have any protection
// against files so big as to overwhelm the VisualText system. Changes to
// safeguard against these issues.

	//if the URL is calling for a file from the default server and we
	// haven't resolved it by this point, it's not going to be there. Bail.
	if (urlStr.GetAt(0) == '/') return false;
	
	bool boolFirstBuffer = true;
	bool boolTextFile = false;
	int nTotalBytesRead = 0;
	int pos = urlStr.ReverseFind('.') ;
	int lSlashPos = urlStr.ReverseFind('\\');
	int rSlashPos = urlStr.ReverseFind('/');
	CString extStr;
	if (pos > lSlashPos && pos > rSlashPos) {
		extStr= urlStr.Right(urlStr.GetLength() - pos-1);
		if (int qPos = extStr.Find('?')) {
			if (qPos >= 0) extStr.SetAt(qPos,'\0');
		}
	}
	else extStr = _T("");
	// trusted file types
	// we only trust file extensions that correspond to text, html, xml
	// files explicitly. With directories we will rely on internal HTML
	// indicators in the default file returned.
	if ( extStr.CompareNoCase(_T("txt")) == 0 || //text
		 extStr.CompareNoCase(_T("htm")) == 0 || //html
		 extStr.CompareNoCase(_T("html")) == 0 || //html
		 extStr.CompareNoCase(_T("asp")) == 0 || //active server page
		 extStr.CompareNoCase(_T("xml")) == 0  // xml
		 ) {
		boolTextFile = true; //trusted file types we'll read without checking further
	}

// end PD fixes, part 1

	CInternetSession session; // can't get status callbacks for OpenURL

	//PD 02/19/2001
	//It's absolutely critical to set timeouts for any massive
	//internet access operation such as spidering
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,2500);
	session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT,2500);
	session.SetOption(INTERNET_OPTION_SEND_TIMEOUT,2500);
	session.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT,2500);
	session.SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT,2500);

	CStdioFile* pFile1 = NULL; // could call ReadString to get 1 line
	try {
		//PD 02/19/2001 
		//it's equally critical not to stick everything we spider onto the cache.
		//that can cause all kinds of problems with the computer itself after
		//extensive spidering
		pFile1 = session.OpenURL(urlStr,1,INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_RELOAD|INTERNET_FLAG_DONT_CACHE);

		if (!pFile1) {
			session.Close();
			return false;
		}
		// read in the file
		// if OpenURL fails, we won't get past here
		while (nBytesRead = pFile1->Read(buffer, MAXBUF - 1)) {
			buffer[nBytesRead] = '\0'; // necessary for message box
			_TCHAR *pEnd = _tcschr(buffer,'\0');
			//PD 02/19/2001
			//end of file is marked by double null in html streams so we
			//don't want to trigger if pEnd is one less than buffer+nBytesRead
			if (pEnd > 0 && pEnd != buffer+nBytesRead && pEnd < buffer+nBytesRead-1) {
					//i.e., we have a unicode file with zeroes thrown in
					//for double byte characters. We don't support this yet.
					pFile1->Close();
					session.Close() ;

					if(pFile1)
					delete pFile1;

					delete [] buffer;
					return false;
			}
			//PD 02/19/2001 PD fixes, part 2
			//test for indications that this is a safe file to read
			if (boolFirstBuffer && !boolTextFile) {
				if (buffer &&
					( _tcsstr(buffer,_T("<HTML>")) ||
					  _tcsstr(buffer,_T("<html>")) ||
					  _tcsstr(buffer,_T("doctype html")) ||
					  _tcsstr(buffer,_T("DOCTYPE html")) ||
					  _tcsstr(buffer,_T("DOCTYPE HTML")) ||
					  _tcsstr(buffer,_T("DOCTYPE XML")) ||
					  _tcsstr(buffer,_T("doctype xml"))
					)
					) {
					boolTextFile = true;
					boolFirstBuffer = false;
				}
				else {
					//we don't have any indication this is an html or xml file,
					//and it's not one of the safe file extensions. So let's bail.
					pFile1->Close();
					session.Close() ;

					if(pFile1)
					delete pFile1;

					delete [] buffer;
					return false;
				}
			}
			// end PD fixes, part 2
			// we just bail if we have no indication that this is a safe file to read
			
			htmlTextStr += buffer;

			//PD 02/19/2001 fixes part 3 well, if we exceed 4 megabytes while spidering, this is just too
			//big a file to safely throw onto the memory heap
			nTotalBytesRead += nBytesRead;
			if (nTotalBytesRead > 4194304) {
					pFile1->Close();
					session.Close() ;

					if(pFile1)
					delete pFile1;

					delete [] buffer;
					return false;
			}
			// end PD fixes, part 3
		}
		pFile1->Close();
		session.Close() ;
	}
	catch(CInternetException* e) {
		e->Delete();
		//PD 02/19/2001 we ought to try to close the file and the session, but that's
		//a dangerous thing to do at this point, so we embed it inside
		//another error handler
		try {		
			session.Close() ;
			if (pFile1)
				delete pFile1;
			delete [] buffer;
			return false;
		}
		catch(...) {
			delete [] buffer;
			return false;
		}
	}
	//and we need an all purpose error handler here too
	//so we won't ever crash VisualText just because
	//buggy MFC crashes
	catch(...) {
		try {		
			session.Close() ;
			if (pFile1)
				delete pFile1;
			delete [] buffer;
			return false;
		}
		catch(...) {
			delete [] buffer;
			return false;
		}
	}

	if (htmlTextStr.Find(_T("HTTP 500")) >= 0 || htmlTextStr.Find(_T("404 Not Found")) >= 0 || htmlTextStr.Find(_T("403 Forbidden")) >= 0)
		return false;

	if(pFile1)
		delete pFile1;

	delete [] buffer;

	return true;
}

CString GetUrlFromUrlFile(const CString &fileName)
{
	FILE *fp;
	errno_t err;
	if (!(err = _tfopen_s(&fp,(const _TCHAR *)fileName, _T("r") )) != 0 ) {
#ifdef _DEBUG
		afxDump << _T("File could not be opened ") << fileName << _T("\n");
#endif
		return _T("");
	}

	_TCHAR line[1001];
	CString urlStr = _T("");
	_fgetts(line,1000,fp);
	urlStr = line;
	fclose(fp);

	TrimString(urlStr);

	if (urlStr.Find(_T("<!--http:")) == 0) {
		ClipLeft(urlStr,4);
		ClipRight(urlStr,3);
	}
	else
		urlStr = _T("");

	return urlStr;
}

CString BoolStr(const BOOL value)
{
	if (value)
		return _T("true");
	return _T("false");
}

#ifdef _OLDSTUFF_
bool BoolValue(const CString &boolStr)
{
	return boolStr == _T("true") ? true : false;
}

bool BoolValue(const _TCHAR * const buff)
{
	return _tcscmp(buff,_T("true")) == 0 ? true : false;
}
#endif

bool BoolValueVT(const CString &boolStr)
{
	return boolStr == _T("true") ? true : false;
}

bool BoolValueVT(const _TCHAR * const buff)
{
	return _tcscmp(buff,_T("true")) == 0 ? true : false;
}

CString TriStateStr(const BOOL value)
{
	if (value == 1)
		return _T("true");
	else if (value == 2)
		return _T("false");
	return _T("");
}
int TriStateValue(const CString &triStateStr)
{
	if (triStateStr == _T("true"))
		return 1;
	else if (triStateStr == _T("false"))
		return 2;
	return 0;
}

//inputs made const, objects passed by reference PD 01/24/2001
void StringToWordList(CStringList &wordList, const CString &strIn, const bool keepBlankLinesFlag)
{//no longer CStringList* PD 02/14/2001
	CString word = _T("");
	_TCHAR c = '\0';
	_TCHAR lastC = '\0';
	int length = strIn.GetLength();

	for (int i=0; i<length; i++) {
		lastC = c;
		c = strIn.GetAt(i);
		if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
			if (word != _T("")) {
				//word.MakeLower();
				wordList.AddTail(word);
				word = _T("");
			}
			if (keepBlankLinesFlag && lastC == '\n')
				wordList.AddTail(_T("\n"));
			continue;
		}
		else
			word += c;
	}
	if (word != _T(""))
		wordList.AddTail(word);
}

void StringToPhraseList(CStringList &wordList, const CString &nodeName, const CString &strIn)
{
	wordList.RemoveAll();
	CString startStr = nodeName + _T(" <- ");
	CString word = startStr;
	_TCHAR c;
	int length = strIn.GetLength();

	for (int i=0; i<length; i++) {
		c = strIn.GetAt(i);
		if (c == '\n' || c == '\r') {
			if (word != startStr) {
				word += _T(" [s] @@");
				wordList.AddTail(word);
				word = startStr;
			}
			continue;
		}
		else if (c == ' ') {
			word += _T(" [s] ");
		}
		else
			word += c;
	}
	if (word != startStr)
		wordList.AddTail(word);
}

void StringToStringList( CStringList &lineList, const CString &linesStr, const CString &sepCharsStr)
{
	CString line = _T("");
	_TCHAR c;
	_TCHAR lastC = ' ';
	int length = linesStr.GetLength();

	lineList.RemoveAll();

	for (int i=0; i<length; i++) {
		c = linesStr.GetAt(i);
		if (sepCharsStr.Find(lastC) >= 0 && sepCharsStr.Find(c) >= 0) {
		}
		else if (sepCharsStr.Find(lastC) < 0 && sepCharsStr.Find(c) >= 0) {
			lineList.AddTail(line);
			line = _T("");
		}
		else
			line += c;
		lastC = c;
	}
	if (line != _T(""))
		lineList.AddTail(line);

}

void StringToStringListString(CStringList &lineList, const CString &linesStr, const CString &separatorStr)
{
	lineList.RemoveAll();

	CString line = _T("");
	int length = linesStr.GetLength();
	int separatorStrLength = separatorStr.GetLength();
	int i = 0;
	int lastI = 0;

	while (i<length) {
		lastI = i;
		i = linesStr.Find(separatorStr,i);
		if (i >= 0) {
			line = linesStr.Mid(lastI,i-lastI);
			lineList.AddTail(line);
			i += separatorStrLength;
		}
		else
			break;
	}

	line = linesStr.Mid(lastI);
	lineList.AddTail(line);
}

CString StringListToLines(CStringList &list, const bool extraReturn)
{
	POSITION pos = list.GetHeadPosition();
	CString word = _T("");
	CString words = _T("");
	while (pos && (word = list.GetNext(pos))) {
		if (words != _T("")) {
			if (extraReturn)
				words += _T("\r\n");
			else
				words += _T("\n");
		}
		words += word;
	}
	return words;
}

bool IntegerInStringList(CStringList &list, const int count)
{
	POSITION pos = list.GetHeadPosition();
	CString word = _T("");
	CString countStr;
	countStr.Format(_T("%d"),count);
	while (pos && (word = list.GetNext(pos))) {
		if (word == countStr)
			return true;
	}
	return false;
}

void SortStringList(CStringList &list)
{
	CString word;
	CString wordBelow;
	POSITION pos = list.GetHeadPosition();
	POSITION lastPos = pos;
	POSITION posBelow;
	POSITION lastPosBelow;

	while (pos && (word = list.GetNext(pos))) {
		posBelow = lastPosBelow = pos;
		while (posBelow && (wordBelow = list.GetNext(posBelow))) {
			if (wordBelow.CompareNoCase(word) < 0) {
				list.SetAt(lastPos,wordBelow);
				list.SetAt(lastPosBelow,word);
				word = wordBelow;
			}
			lastPosBelow = posBelow;
		}
		lastPos = pos;
	}
}

void RemoveSuccessiveEquals(CStringList &list)
{
	POSITION pos = list.GetHeadPosition();
	CString word;
	CString lastWord = _T("");
	POSITION posLast;

	while (pos && (word = list.GetNext(pos))) {
		if (word != _T("") && word == lastWord)
			list.RemoveAt(posLast);
		lastWord = word;
		posLast = pos;
	}
}

int CALLBACK ListCompareFunc(const LPARAM lParam1, const LPARAM lParam2, const LPARAM lParamSort)
{
	CString str1 = ((ListData*)lParam1)->strText;
	CString str2 = ((ListData*)lParam2)->strText;
	return str1.CompareNoCase(str2);
}

bool ParseLine(CStringList &parseList, const CString &lineStr, const CString &patternStr)
{
	int pass = -1;
	int lenLine = lineStr.GetLength();
	int lenPattern = patternStr.GetLength();
	int p = 0;
	_TCHAR cLine;
	_TCHAR cPattern;
	_TCHAR cNextPattern;
	_TCHAR type = ' ';
	CString parseStr = _T("");
	bool matches;
	bool empty = true;

	parseList.RemoveAll();

	for (int i=0; i<lenLine; i++) {
		cLine = lineStr.GetAt(i);
		cPattern = patternStr.GetAt(p);
		if (cPattern == '%') {
			type = patternStr.GetAt(++p);
			//lenPattern--;  // The true length must be without the % character
		}
		if (p+1 < lenPattern)
			cNextPattern = patternStr.GetAt(p+1);
		else
			cNextPattern = '\0';
		
		matches = false;
		if ((type == 'd' && _istdigit(cLine)) ||
			(type == 'w' && cLine != ' ' && cNextPattern != cLine) ||
			(type == 'a' && cNextPattern != cLine)
			) {
			matches = true;
		}

		if (parseStr != _T("")) {
			if (matches)
				parseStr += cLine;
			else {
				i--;
				p++;
				parseList.AddTail(parseStr);
				empty = false;
				parseStr = _T("");
				type = ' ';
			}
		}
		else if (matches) {
			parseStr = cLine;
		}
		else if (cPattern == cLine) {
			p++;
		}
		else {
			p = 0;
			parseStr = _T("");
			type = ' ';
			parseList.RemoveAll();
			empty = true;
		}

		// TEST FOR ENTIRE PATTERN MATCHED
		if (!matches && p >= lenPattern) {
			break;
		}
	}

	if (p < lenPattern - 1) {
		parseList.RemoveAll();
		return false;
	}

	if (parseStr != _T("")) {
		parseList.AddTail(parseStr);
		empty = false;
	}

	if (empty) {
		parseList.RemoveAll();
		return false;
	}
	
	return parseList.GetCount() ? true : false;
}

//inputs made const, objects passed by reference PD 01/24/2001
CString NthStringListItem(CStringList &list, const int nth)
{   //no longer CStringList* PD 02/14/2001
	if (list.IsEmpty()) return _T("") ;
	if ( nth >= list.GetCount()) return _T("") ;

	POSITION pos = list.GetHeadPosition();
	CString word = _T("");
	int i = 0;

	while (pos && (word = list.GetNext(pos))) {
		if (i++ == nth)
			return word;
	}
	return _T("");
}

void ReplaceNthStringListItem(CStringList &list, const int nth, const CString &newStr)
{
	POSITION pos = list.GetHeadPosition();
	POSITION lastPos = pos;
	CString word = _T("");
	int i = 0;

	while (pos && (word = list.GetNext(pos))) {
		if (i++ == nth) {
			list.SetAt(lastPos,newStr);
			break;
		}
		lastPos = pos;
	}
}

void InsertAfterNthStringListItem(CStringList &list, const int nth, const CString &newStr)
{
	POSITION pos = list.GetHeadPosition();
	POSITION lastPos = pos;
	CString word = _T("");
	int i = 0;

	while (pos && (word = list.GetNext(pos))) {
		if (i++ == nth) {
			list.InsertAfter(lastPos,newStr);
			break;
		}
		lastPos = pos;
	}
}

void TokensBySeparator(CStringList &tokenList, const CString &inStr, const _TCHAR sepChar, const bool noSeps)
{
	if (inStr == _T("") || !sepChar)
		return;

	CString currentToken;
	int segDone = 0;
	tokenList.RemoveAll();

	for (int i=0; segDone >= 0; i++) {
		segDone = NthPathSegment(inStr,currentToken,sepChar,i);
		if (segDone >= 0 && (!noSeps || currentToken == sepChar))
			tokenList.AddTail(currentToken);
	}
}

void ExpandFromPath(CXTPTreeCtrl *m_TreeCtrl, const CString &pathStr, const bool expand)
{
	if (pathStr == _T("")) return;

	HTREEITEM item = m_TreeCtrl->GetRootItem();
	int i = 0;
	CString itemTextStr;
	CString currentLevel;
	int segDone = NthPathSegment(pathStr,currentLevel,'\\',i++);

	while (item) {
		itemTextStr = m_TreeCtrl->GetItemText(item);
		if (itemTextStr == currentLevel) {
			if (expand)
				m_TreeCtrl->Expand(item,TVE_EXPAND);
			segDone = NthPathSegment(pathStr,currentLevel,'\\',i++);
			if (segDone < 0) {
				m_TreeCtrl->SelectItem(item);
				break;
			}
			item = m_TreeCtrl->GetChildItem(item);
		}
		else
			item = m_TreeCtrl->GetNextSiblingItem(item);
	}
}

bool FindMenuPos(CMenu *pBaseMenu, const UINT myID, CMenu * & pMenu, int & mpos)
{
	// REMARK: pMenu is a pointer to a Cmenu-Pointer
	int myPos;
	if( pBaseMenu == NULL )
	{
		// Sorry, Wrong Number
		pMenu = NULL;
		mpos = -1;
		return true;
	}
	for( myPos = pBaseMenu->GetMenuItemCount() -1; myPos >= 0; myPos-- )
	{
		int Status = pBaseMenu->GetMenuState( myPos, MF_BYPOSITION);
		CMenu* mNewMenu;
		
		if( Status == 0xFFFFFFFF )
		{
			// That was not an legal Menu/Position-Combination
			pMenu = NULL;
			mpos = -1;
			return true;
		}
		// Is this the real one?
		if( pBaseMenu->GetMenuItemID(myPos) == myID )
		{
			// Yep!
			pMenu = pBaseMenu;
			mpos = myPos;
			return true;
		}
		// Maybe a subMenu?
		mNewMenu = pBaseMenu->GetSubMenu(myPos);
		// This function will return NULL if ther is NO SubMenu
		if( mNewMenu != NULL )
		{
			// rekursive!
			bool found = FindMenuPos( mNewMenu, myID, pMenu, mpos);
			if(found)
				return true;	// return this loop
		}
		// I have to check the next in my loop
	}
	return false; // iterate in the upper stackframe
}

bool FindMenuPosFromString(CMenu *pBaseMenu, const CString &matchStr, int & mpos)
{
	// REMARK: pMenu is a pointer to a Cmenu-Pointer
	int myPos;
	if( pBaseMenu == NULL )
	{
		// Sorry, Wrong Number
		mpos = -1;
		return true;
	}

	CString itemNameStr;

	for( myPos = pBaseMenu->GetMenuItemCount() -1; myPos >= 0; myPos-- )
	{
		int Status = pBaseMenu->GetMenuState( myPos, MF_BYPOSITION);
		CMenu* mNewMenu;
		
		if( Status == 0xFFFFFFFF )
		{
			// That was not an legal Menu/Position-Combination
			mpos = -1;
			return true;
		}
		pBaseMenu->GetMenuString(myPos,itemNameStr,MF_BYPOSITION);

		if (itemNameStr == matchStr) {
			mpos = myPos;
			return true;
		}
		// Maybe a subMenu?
		mNewMenu = pBaseMenu->GetSubMenu(myPos);
		// I have to check the next in my loop
	}
	return false; // iterate in the upper stackframe
}

void StringToChar(_TCHAR *buff, const CString &strIn, const int len)
{
	//_tcsnccpy_s(buff,sizeof(buff),(const _TCHAR *)strIn,len);
	_tcsnccpy_s(buff,len,(const _TCHAR *)strIn,len);
	buff[len] = '\0';
	buff[strIn.GetLength()] = '\0';
}

HTREEITEM CopyBranch(CTreeCtrl *treeCtrl, const HTREEITEM &htiBranch, const HTREEITEM &htiNewParent, const HTREEITEM &htiAfter)
{
	HTREEITEM hChild;

    HTREEITEM hNewItem = CopyItem(treeCtrl,htiBranch,htiNewParent,htiAfter);
    hChild = treeCtrl->GetChildItem(htiBranch);
    while( hChild != NULL) {
		// recursively transfer all the items
		CopyBranch(treeCtrl, hChild, hNewItem);  
		hChild = treeCtrl->GetNextSiblingItem( hChild );
    }
    return hNewItem;
}

HTREEITEM CopyBranch(CXTPTreeCtrl *treeCtrl, const HTREEITEM &htiBranch, const HTREEITEM &htiNewParent, const HTREEITEM &htiAfter)
{
	HTREEITEM hChild;

    HTREEITEM hNewItem = CopyItem(treeCtrl,htiBranch,htiNewParent,htiAfter);
    hChild = treeCtrl->GetChildItem(htiBranch);
    while( hChild != NULL) {
		// recursively transfer all the items
		CopyBranch(treeCtrl, hChild, hNewItem);  
		hChild = treeCtrl->GetNextSiblingItem( hChild );
    }
    return hNewItem;
}

HTREEITEM CopyItem(CTreeCtrl *treeCtrl, const HTREEITEM &hItem, const HTREEITEM &htiNewParent, const HTREEITEM &htiAfter)
{
	TV_INSERTSTRUCT tvstruct;
	HTREEITEM hNewItem;
	CString sText;

	// get information of the source item
	tvstruct.item.hItem = hItem;
	tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | 
							TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	treeCtrl->GetItem(&tvstruct.item);  
	sText = treeCtrl->GetItemText( hItem );

	tvstruct.item.cchTextMax = sText.GetLength();
	tvstruct.item.pszText = sText.LockBuffer();

	// Insert the item at proper location
	tvstruct.hParent = htiNewParent;
	tvstruct.hInsertAfter = htiAfter;
	tvstruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	hNewItem = treeCtrl->InsertItem(&tvstruct);
	sText.ReleaseBuffer();

	// Now copy item data and item state.
	treeCtrl->SetItemData(hNewItem, treeCtrl->GetItemData(hItem));
	treeCtrl->SetItemState(hNewItem, treeCtrl->GetItemState(hItem, TVIS_STATEIMAGEMASK), 
													TVIS_STATEIMAGEMASK );

	// Call virtual function to allow further processing in derived class
	//treeCtrl->OnItemCopied(hItem, hNewItem);

	return hNewItem;
}

HTREEITEM CopyItem(CXTPTreeCtrl *treeCtrl, const HTREEITEM &hItem, const HTREEITEM &htiNewParent, const HTREEITEM &htiAfter)
{
	TV_INSERTSTRUCT tvstruct;
	HTREEITEM hNewItem;
	CString sText;

	// get information of the source item
	tvstruct.item.hItem = hItem;
	tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | 
							TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	treeCtrl->GetItem(&tvstruct.item);  
	sText = treeCtrl->GetItemText( hItem );

	tvstruct.item.cchTextMax = sText.GetLength();
	tvstruct.item.pszText = sText.LockBuffer();

	// Insert the item at proper location
	tvstruct.hParent = htiNewParent;
	tvstruct.hInsertAfter = htiAfter;
	tvstruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	hNewItem = treeCtrl->InsertItem(&tvstruct);
	sText.ReleaseBuffer();

	// Now copy item data and item state.
	treeCtrl->SetItemData(hNewItem, treeCtrl->GetItemData(hItem));
	treeCtrl->SetItemState(hNewItem, treeCtrl->GetItemState(hItem, TVIS_STATEIMAGEMASK), 
													TVIS_STATEIMAGEMASK );

	// Call virtual function to allow further processing in derived class
	//treeCtrl->OnItemCopied(hItem, hNewItem);

	return hNewItem;
}

int MoveTreeNodeUp(CTreeCtrl *treeCtrl, const HTREEITEM &selected, const int imageBoundary, const int imageSelBoundary)
{
	HTREEITEM before = treeCtrl->GetPrevSiblingItem(selected);
	int image;
	int imageSel;
	treeCtrl->GetItemImage(before,image,imageSel);

	if (!before ||
		(imageBoundary >= 0 && image == imageBoundary && imageSel == imageSelBoundary)) {
		AfxMessageBox(_T("At top!"));
		return 0;
	}

	int skippedCount = 1;
	CString itemStr = treeCtrl->GetItemText(before);
	if (before && treeCtrl->GetChildItem(before))
		skippedCount = GetTreeCtrlChildrenCount(treeCtrl,before);

	before = treeCtrl->GetPrevSiblingItem(before);
	if (!before)
		before = TVI_FIRST;

	HTREEITEM parent = treeCtrl->GetParentItem(selected);
	HTREEITEM selectedCopy = CopyBranch(treeCtrl,selected,parent,before);
	treeCtrl->DeleteItem(selected);
	treeCtrl->SelectItem(selectedCopy);

	return skippedCount;
}

int MoveTreeNodeUp(CXTPTreeCtrl *treeCtrl, const HTREEITEM &selected, const int imageBoundary, const int imageSelBoundary)
{
	HTREEITEM before = treeCtrl->GetPrevSiblingItem(selected);
	int image;
	int imageSel;
	treeCtrl->GetItemImage(before,image,imageSel);

	if (!before ||
		(imageBoundary >= 0 && image == imageBoundary && imageSel == imageSelBoundary)) {
		AfxMessageBox(_T("At top!"));
		return 0;
	}

	int skippedCount = 1;
	CString itemStr = treeCtrl->GetItemText(before);
	if (before && treeCtrl->GetChildItem(before))
		skippedCount = GetTreeCtrlChildrenCount(treeCtrl,before);

	before = treeCtrl->GetPrevSiblingItem(before);
	if (!before)
		before = TVI_FIRST;

	HTREEITEM parent = treeCtrl->GetParentItem(selected);
	HTREEITEM selectedCopy = CopyBranch(treeCtrl,selected,parent,before);
	treeCtrl->DeleteItem(selected);
	treeCtrl->SelectItem(selectedCopy);

	return skippedCount;
}

int MoveTreeNodeDown(CXTPTreeCtrl *treeCtrl, const HTREEITEM &selected, const int imageBoundary, const int imageSelBoundary)
{
	HTREEITEM after = treeCtrl->GetNextSiblingItem(selected);
	int image;
	int imageSel;
	treeCtrl->GetItemImage(after,image,imageSel);

	if (!after ||
		(imageBoundary >= 0 && image == imageBoundary && imageSel == imageSelBoundary)) {
		AfxMessageBox(_T("At bottom!"));
		return 0;
	}

	HTREEITEM parent = treeCtrl->GetParentItem(selected);
	HTREEITEM fromCopy = CopyBranch(treeCtrl,selected,parent,after);
	treeCtrl->DeleteItem(selected);
	treeCtrl->SelectItem(fromCopy);

	int skippedCount = 1;
	if (after && treeCtrl->GetChildItem(after))
		skippedCount = GetTreeCtrlChildrenCount(treeCtrl,after);
	return skippedCount;
}

int MoveTreeNodeDown(CTreeCtrl *treeCtrl, const HTREEITEM &selected, const int imageBoundary, const int imageSelBoundary)
{
	HTREEITEM after = treeCtrl->GetNextSiblingItem(selected);
	int image;
	int imageSel;
	treeCtrl->GetItemImage(after,image,imageSel);

	if (!after ||
		(imageBoundary >= 0 && image == imageBoundary && imageSel == imageSelBoundary)) {
		AfxMessageBox(_T("At bottom!"));
		return 0;
	}

	HTREEITEM parent = treeCtrl->GetParentItem(selected);
	HTREEITEM fromCopy = CopyBranch(treeCtrl,selected,parent,after);
	treeCtrl->DeleteItem(selected);
	treeCtrl->SelectItem(fromCopy);

	int skippedCount = 1;
	if (after && treeCtrl->GetChildItem(after))
		skippedCount = GetTreeCtrlChildrenCount(treeCtrl,after);
	return skippedCount;
}

void TreeViewSelectItem(CTreeView* pTreeView, CPoint point) // SRP 010207
{
	CPoint pt(point);
	pTreeView->ClientToScreen( &pt );
	pTreeView->GetTreeCtrl().ScreenToClient( &pt );
	HTREEITEM hitItem;
	TVHITTESTINFO	hitTest;
	ZeroMemory( &hitTest, sizeof( TVHITTESTINFO ) );
	hitTest.pt = pt;
	hitItem = pTreeView->GetTreeCtrl().HitTest( &hitTest );
	if ( hitTest.flags & ( TVHT_ONITEM ) ) {
		pTreeView->GetTreeCtrl().EnsureVisible( hitTest.hItem );
		pTreeView->GetTreeCtrl().SelectItem( hitTest.hItem );
	}
}

int TreeCtrlItemPosition(CXTPTreeCtrl* m_treeCtrl, const HTREEITEM &child)
{
	HTREEITEM sibling = child;
	int i = 0;
	while (sibling) {
		sibling = m_treeCtrl->GetPrevSiblingItem(sibling);
		i++;
	}
	return i;
}

CString StateString(CTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state)
{
	int count = 0;
	CString stateStr = _T("");
	StateString(treeCtrl,treeItem,state,stateStr,count);
	return stateStr;
}

CString StateString(CXTPTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state)
{
	int count = 0;
	CString stateStr = _T("");
	StateString(treeCtrl,treeItem,state,stateStr,count);
	return stateStr;
}

void StateString(CTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state,
				 CString &allStr, int &count)
{
	if (!treeItem)
		return;

    HTREEITEM sibling = treeItem;
	CString itemStr;
	CString currentParentStr;
	UINT stateFound;
	count++;

    while (sibling) {
		itemStr = treeCtrl->GetItemText(sibling);

		if (state == STATE_GET_SELECTED)
			stateFound = treeCtrl->GetItemState(sibling,TVIS_SELECTED) & TVIS_SELECTED;
		else if (state == STATE_GET_EXPANDED)
			stateFound = treeCtrl->GetItemState(sibling,TVIS_EXPANDED) & TVIS_EXPANDED;
		else if (state == STATE_GET_CHECKED)
			stateFound = treeCtrl->GetCheck(sibling);

		if (stateFound) {
			currentParentStr.Format(_T("%d"),count);
			if (allStr != _T(""))
				allStr += _T(":");
			allStr += currentParentStr;
		}

		StateString(treeCtrl,treeCtrl->GetChildItem(sibling),state,allStr,count);

		sibling = treeCtrl->GetNextSiblingItem(sibling);
		if (sibling)
			count++;
    }
}

void StateString(CXTPTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state,
				 CString &allStr, int &count)
{
	if (!treeItem || !treeCtrl)
		return;

    HTREEITEM sibling = treeItem;
	CString itemStr;
	CString currentParentStr;
	UINT stateFound;
	count++;

    while (sibling) {
		itemStr = treeCtrl->GetItemText(sibling);

		if (state == STATE_GET_SELECTED)
			stateFound = treeCtrl->GetItemState(sibling,TVIS_SELECTED) & TVIS_SELECTED;
		else if (state == STATE_GET_EXPANDED)
			stateFound = treeCtrl->GetItemState(sibling,TVIS_EXPANDED) & TVIS_EXPANDED;
		else if (state == STATE_GET_CHECKED)
			stateFound = treeCtrl->GetCheck(sibling);

		if (stateFound) {
			currentParentStr.Format(_T("%d"),count);
			if (allStr != _T(""))
				allStr += _T(":");
			allStr += currentParentStr;
		}

		StateString(treeCtrl,treeCtrl->GetChildItem(sibling),state,allStr,count);

		sibling = treeCtrl->GetNextSiblingItem(sibling);
		if (sibling)
			count++;
    }
}

void StateFromString(CXTPTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state,
					 const CString &stateStr)
{
	if (!treeItem || stateStr == _T(""))
		return;

	int count = 0;
	CStringList numberList;
	TokensBySeparator(numberList,stateStr,':');
	if (numberList.GetCount() == 0) {
		numberList.RemoveAll();
		return;
	}

	StateFromString(treeCtrl,treeItem,state,numberList,count);
}

void StateFromString(CTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state,
					 const CString &stateStr)
{
	if (!treeItem || stateStr == _T(""))
		return;

	int count = 0;
	CStringList numberList;
	TokensBySeparator(numberList,stateStr,':');
	if (numberList.GetCount() == 0) {
		numberList.RemoveAll();
		return;
	}

	StateFromString(treeCtrl,treeItem,state,numberList,count);
}

void StateFromString(CTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state,
					 CStringList &numberList, int &count)
{
	if (!treeItem || numberList.GetCount() == 0)
		return;

    HTREEITEM sibling = treeItem;
	count++;

    while (sibling) {
		if (IntegerInStringList(numberList,count)) {
			if (state == STATE_EXPAND || state == STATE_EXPAND_COLLAPSE)
				treeCtrl->Expand(sibling,TVE_EXPAND);
			else if (state == STATE_SELECT)
				treeCtrl->SelectItem(sibling);
		}
		else if (state == STATE_EXPAND_COLLAPSE)
			treeCtrl->Expand(sibling,TVE_COLLAPSE);

		StateFromString(treeCtrl,treeCtrl->GetChildItem(sibling),state,numberList,count);

		sibling = treeCtrl->GetNextSiblingItem(sibling);
		if (sibling)
			count++;
    }
}

void StateFromString(CXTPTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state,
					 CStringList &numberList, int &count)
{
	if (!treeItem || numberList.GetCount() == 0)
		return;

    HTREEITEM sibling = treeItem;
	count++;

    while (sibling) {
		if (IntegerInStringList(numberList,count)) {
			if (state == STATE_EXPAND || state == STATE_EXPAND_COLLAPSE)
				treeCtrl->Expand(sibling,TVE_EXPAND);
			else if (state == STATE_SELECT)
				treeCtrl->SelectItem(sibling);
		}
		else if (state == STATE_EXPAND_COLLAPSE)
			treeCtrl->Expand(sibling,TVE_COLLAPSE);

		StateFromString(treeCtrl,treeCtrl->GetChildItem(sibling),state,numberList,count);

		sibling = treeCtrl->GetNextSiblingItem(sibling);
		if (sibling)
			count++;
    }
}

void ZipFiles(HZIP zf, const CString &findFileStr, const CString &findWildStr,
			  const CString &zipFileStr, const CString &pathStr, bool directoriesFlag)
{
	CFileFind finder;
	BOOL bWorking = finder.FindFile(findFileStr+_T("\\")+findWildStr);
	CString filePathStr;
	CString fileNameStr;
	CString fileZipPath;
	bool logTest;

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
//	unsigned long fileSize;	// .NET COMPLAINT	// 06/11/03 AM.
	ULONGLONG fileSize;		// 06/11/03 AM.
    TCHAR buff1[1001];
    TCHAR buff2[1001];

	while (bWorking) {
		bWorking = finder.FindNextFile();
		filePathStr = finder.GetFilePath();
		fileNameStr = finder.GetFileName();
		if (pathStr == _T(""))
			fileZipPath = fileNameStr;
		else
			fileZipPath = pathStr + _T("\\") + fileNameStr;
		fileSize = finder.GetLength();

		if (fileNameStr.Find('.') == 0 ||
			EndsWith(fileNameStr,_T("zip")) ||
			EndsWith(fileNameStr,_T("tmp"))
			)
			continue;

		logTest = appBoolSaveLogFiles || filePathStr.Find(_T("_log")) < 0;

		if (directoriesFlag && finder.IsDirectory() && logTest) {
			wnd->AddDebugLine(_T("Archiving directory:"),filePathStr,true);
			ZipFiles(zf,filePathStr,findWildStr,zipFileStr,fileZipPath);
		}
		else if (logTest) {
			wnd->AddDebugLine(_T("Archiving file:"),filePathStr,true);
			fileZipPath = fileZipPath.Mid(1);
			StringToChar(buff1,fileZipPath,1000);
			StringToChar(buff2,filePathStr,1000);
			ZipAdd(zf,buff1,buff2,0,ZIP_FILENAME);
			PeekHere(10);
		}
	}
}

void UnarchiveFiles(const CString &zipFileStr, const CString &localPathStr)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->AddDebugLine(_T("Unzipping file:"),zipFileStr);

	UnzipFiles(zipFileStr,localPathStr);

	wnd->AddDebugLine(_T("Unzipping file done:"),zipFileStr);
}

CString ArchiveAutoGenName()
{
	//CString archiveNameStr = theApp.GetProfileString("PREFERENCES","ARCHIVENAME","");

	CTime timeWrite;
	timeWrite = CTime::GetCurrentTime();

	CString timeStr = timeWrite.Format(_T("_%y%m%d_%H%M%S"));

	return appNameStr + timeStr;
}

#define ZIPFILEBUFSIZE 1024
/*
bool ZipFile(HZIP zf, const CString &sourceFileStr, const CString &zipPathStr)
{
    CString strMessage;
    CString strZipFile;
    int errclose = ZIP_OK;
    int err=0;
    
	CFile fileToZip;

	if(!fileToZip.Open(sourceFileStr,(CFile::modeRead))) {
		CString msgStr;
		msgStr.Format("Couldn't open file for zipping: \"%s\"",sourceFileStr);
		AfxMessageBox(msgStr);
		return false;
	}

    zip_fileinfo zi;
	
    TCHAR buffSource[1001];
	StringToChar(buffSource,sourceFileStr,1000);

    int opt_compress_level=Z_BEST_COMPRESSION;
    //Best compression to save bandwidth at a maximum.
    zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour = 
        zi.tmz_date.tm_mday = zi.tmz_date.tm_min = zi.tmz_date.tm_year = 0;
    zi.dosDate = 0;
    zi.internal_fa = 0;
    zi.external_fa = 0;
    filetime(buffSource,&zi.dosDate);

    ZipAdd(hz,"src1.txt",  "c:\\src1.txt",0,ZIP_FILENAME);

	return true;
}
*/
bool UnzipFiles(const CString &zipFileStr, const CString &localPathStr)
{
    HZIP hz = NULL;
	_TCHAR buff[ZIPFILEBUFSIZE];
	CString slashStr = zipFileStr;
	slashStr.Replace(_T("\\"),_T("\\\\"));
	StringToChar(buff,slashStr,ZIPFILEBUFSIZE);

	hz = OpenZip(buff,0,ZIP_FILENAME);
	
#ifdef UNICODE
	ZIPENTRYW ze;
#else
	ZIPENTRY ze;
#endif
	GetZipItem(hz,-1,&ze);
	int itemCount = ze.index;

	SetCurrentDirectory(localPathStr);

	for (int i=0; i<itemCount; i++) {
		GetZipItem(hz,i,&ze);
		UnzipFile(hz,i,ze.name,localPathStr);
	}

	CloseZip(hz);

	return true;
}

bool UnzipFile(HZIP hz, int index, const CString &targetFileStr, const CString &localPathStr)
{
	CString msgStr;

	CString fileNameStr;
	fileNameStr.Format(_T("%s/%s"),localPathStr,targetFileStr);
	fileNameStr.Replace(_T("/"),_T("\\"));
	CString dirPathStr = StripPath(fileNameStr,0,true);

	if (!MakeDirectory(dirPathStr)) {
		msgStr.Format(_T("Couldn't make directory: \"%s\""),dirPathStr);
		AfxMessageBox(msgStr);
		return false;
	}

	TCHAR buff[1001];
	StringToChar(buff,targetFileStr,1000);
	UnzipItem(hz,index,buff,0,ZIP_FILENAME);

	return true;
}

CString ArchiveFiles(const CString &pathStr, const CString &archiveTypeStr, const CString &findWildStr)
{
	if (!AskForText(_T("Enter archive name:"),ArchiveAutoGenName()))
		return _T("");
	CString archiveFile = appAskText;

	CString targetFileStr = pathStr + _T("\\") + archiveFile;
	CString zipFileStr = targetFileStr + _T(".zip");

	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	wnd->ClearOutput();
	wnd->AddDebugLine(_T("Archiving to file:"),zipFileStr);

    TCHAR buff[1001];
	StringToChar(buff,zipFileStr,1000);
	HZIP hz = CreateZip(buff, 0, ZIP_FILENAME);
	ZipFiles(hz,pathStr,findWildStr,zipFileStr,_T(""));
	CloseZip(hz);

	return zipFileStr;
}

unsigned long filetime(TCHAR *f, unsigned long * dt)
/*f: name of file to get info on */
/* dt: dostime */
{
  int ret = 0;
  {
      FILETIME ftLocal;
      HANDLE hFind;
      WIN32_FIND_DATA  ff32;

      hFind = FindFirstFile(f,&ff32);
      if (hFind != INVALID_HANDLE_VALUE)
      {
        FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);
        FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
        FindClose(hFind);
        ret = 1;
      }
  }
  return ret;
}

bool IsDigit(const CString &numStr)
{
	if (numStr == _T(""))
		return false;

	int strLen = numStr.GetLength();

	for (int i=0; i<strLen; i++) {
		// SRP 010413 Handle negative numbers
		// Undecided at this point if we'll ignore spaces...
		if (0 == i && '-' == numStr.GetAt(0))
			continue;
		if (!_istdigit(numStr.GetAt(i)))
			return false;
	}

	return true;
}

bool IsAlpha(const CString &alphaStr)
{
	if (alphaStr == _T(""))
		return false;

	int strLen = alphaStr.GetLength();

	for (int i=0; i<strLen; i++) {
		if (!_istalpha(alphaStr.GetAt(i)))
			return false;
	}

	return true;
}

HTREEITEM FindLastConceptSibling(CTreeCtrl *treeCtrl, const HTREEITEM &startingItem)
{	
	HTREEITEM treeItem = startingItem;
	HTREEITEM lastTreeItem = NULL;
	int image;
	int imageSel;

	while (treeItem) {
		treeCtrl->GetItemImage(treeItem,image,imageSel);
		lastTreeItem = treeItem;
		treeItem = treeCtrl->GetNextSiblingItem(treeItem);
	}
	return lastTreeItem;
}

HTREEITEM FindLastConceptSibling(CXTPTreeCtrl *treeCtrl, const HTREEITEM &startingItem)
{	
	HTREEITEM treeItem = startingItem;
	HTREEITEM lastTreeItem = NULL;
	int image;
	int imageSel;

	while (treeItem) {
		treeCtrl->GetItemImage(treeItem,image,imageSel);
		lastTreeItem = treeItem;
		treeItem = treeCtrl->GetNextSiblingItem(treeItem);
	}
	return lastTreeItem;
}

void PopupFromStringList(CWnd *wnd, const CPoint &point, CStringList &list, const int startID)
{
	if (!list.GetCount())
		return;

	CString conceptStr;
	CMenu menu;
	menu.CreatePopupMenu();

	POSITION pos = list.GetHeadPosition();
	CString itemStr = _T("");
	int i = 0;
	bool gray;

	while (pos && (conceptStr = list.GetNext(pos))) {
		gray = conceptStr.Find(_T(">>>")) ? false : true;
		if (conceptStr == _T("- - - - -"))
			menu.AppendMenu(MF_SEPARATOR,startID+i);
		else {
			if (gray)
				menu.AppendMenu(MF_STRING,startID+i,conceptStr.Mid(3));
			else
				menu.AppendMenu(MF_STRING,startID+i,conceptStr);
		}
		if (gray)
			menu.EnableMenuItem(startID+i,MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
		i++;
	}

	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, wnd);
}

int TreeItemNumberFromItem(CXTPTreeCtrl *treeCtrl, const HTREEITEM &itemIn)
{
	HTREEITEM item = treeCtrl->GetRootItem();
	int count = 0;

	while (item) {
		count++;
		if (item == itemIn)
			return count;

		item = treeCtrl->GetNextSiblingItem(item);
	}
	
	return -1;
}

bool TreeItemNumberFromItemRecurse(CXTPTreeCtrl *treeCtrl, const HTREEITEM &currentItem, const HTREEITEM &itemIn, int &count)
{
	HTREEITEM item = currentItem ;
	if (!item)
		item = treeCtrl->GetRootItem();

	HTREEITEM child;

	while (item) {
		count++;

		if (item == itemIn)
			return true;

		if ((child = treeCtrl->GetChildItem(item))) {
			if (TreeItemNumberFromItemRecurse(treeCtrl,child,itemIn,count))
				return true;
		}

		item = treeCtrl->GetNextSiblingItem(item);
	}
	
	return false;
}


BOOL IsItemChecked(CXTPTreeCtrl *treeCtrl, const HTREEITEM &hItem)
{
	return treeCtrl->GetItemState(hItem, TVIS_STATEIMAGEMASK )>>12 == 2;
}

int FileLineNumberFromIndex(const CString &fileStr, const long charIndex)
{
	CStringList lines;
	ReadFileToStringList(lines,fileStr);
	int count = lines.GetCount();
	CString lineStr;
	long lineIndex = 0;
	int i = 0;

	for (i=0; i<count; i++) {
		lineStr = NthStringListItem(lines,i);
		lineIndex += lineStr.GetLength() + 1;  // Add 1 for stripped \n from ReadFileToStringList
		if (charIndex < lineIndex)
			break;
	}
	return i;
}

void AdjustFileStartEnd(long &start, long &end, const CString &textPathStr)
{
	if (textPathStr != _T("")) {
		int numExtraChars = FileLineNumberFromIndex(InputPath(textPathStr),start);
		start += numExtraChars;
		end += numExtraChars;
	}
}

void TreeCtrlExpandAll(CXTPTreeCtrl *treeCtrl, const HTREEITEM &currentItem, const UINT nCode)
{
	HTREEITEM item = currentItem ;
	HTREEITEM childItem;
	CString debugStr;

	while (item) {
		debugStr = treeCtrl->GetItemText(item);
		childItem = treeCtrl->GetChildItem(item);
		TreeCtrlExpandAll(treeCtrl,childItem,nCode);
		treeCtrl->Expand(item,nCode);
		item = treeCtrl->GetNextSiblingItem(item);
	}
}

void TreeCtrlExpandAll(CTreeCtrl *treeCtrl, const HTREEITEM &currentItem, const UINT nCode)
{
	HTREEITEM item = currentItem ;
	HTREEITEM childItem;
	CString debugStr;

	while (item) {
		debugStr = treeCtrl->GetItemText(item);
		childItem = treeCtrl->GetChildItem(item);
		TreeCtrlExpandAll(treeCtrl,childItem,nCode);
		treeCtrl->Expand(item,nCode);
		item = treeCtrl->GetNextSiblingItem(item);
	}
}

void TreeCtrlExpandOneLevel(CTreeCtrl *treeCtrl, const HTREEITEM &currentItem, const UINT nCode)
{
	HTREEITEM item = currentItem ;
	HTREEITEM childItem;
	CString debugStr;
	UINT expanded = false;

	while (item) {
		debugStr = treeCtrl->GetItemText(item);
		expanded = treeCtrl->GetItemState(item,TVIS_EXPANDED) & TVIS_EXPANDED;
		childItem = treeCtrl->GetChildItem(item);

		if ((nCode == TVE_EXPAND && !expanded) || (nCode == TVE_COLLAPSE && expanded))
			treeCtrl->Expand(item,nCode);
		else
			TreeCtrlExpandOneLevel(treeCtrl,childItem,nCode);
		item = treeCtrl->GetNextSiblingItem(item);
	}
}

void TreeCtrlExpandOneLevel(CXTPTreeCtrl *treeCtrl, const HTREEITEM &currentItem, const UINT nCode)
{
	HTREEITEM item = currentItem ;
	HTREEITEM childItem;
	CString debugStr;
	UINT expanded = false;

	while (item) {
		debugStr = treeCtrl->GetItemText(item);
		expanded = treeCtrl->GetItemState(item,TVIS_EXPANDED) & TVIS_EXPANDED;
		childItem = treeCtrl->GetChildItem(item);

		if ((nCode == TVE_EXPAND && !expanded) || (nCode == TVE_COLLAPSE && expanded))
			treeCtrl->Expand(item,nCode);
		else
			TreeCtrlExpandOneLevel(treeCtrl,childItem,nCode);
		item = treeCtrl->GetNextSiblingItem(item);
	}
}

void DeleteTreeCtrlChildren(CXTPTreeCtrl *treeCtrl, const HTREEITEM &currentItem, const int image, const int imageSel,
							const bool oneLevel)
{
	HTREEITEM item = currentItem ;
	HTREEITEM childItem = treeCtrl->GetChildItem(item);
	HTREEITEM nextItem;
	CString str;
	int img;
	int imgSel;

	while (childItem) {
		str = treeCtrl->GetItemText(childItem);
		nextItem = treeCtrl->GetNextSiblingItem(childItem);
		if (!oneLevel)
			DeleteTreeCtrlChildren(treeCtrl,childItem,image,imageSel,oneLevel);
		treeCtrl->GetItemImage(childItem,img,imgSel);
		if (image == -1 || img == image || imgSel == imageSel)
			treeCtrl->DeleteItem(childItem);
		childItem = nextItem;
	}
}

void DeleteTreeCtrlChildren(CTreeCtrl *treeCtrl, const HTREEITEM &currentItem, const int image, const int imageSel,
							const bool oneLevel)
{
	HTREEITEM item = currentItem ;
	HTREEITEM childItem = treeCtrl->GetChildItem(item);
	HTREEITEM nextItem;
	CString str;
	int img;
	int imgSel;

	while (childItem) {
		str = treeCtrl->GetItemText(childItem);
		nextItem = treeCtrl->GetNextSiblingItem(childItem);
		if (!oneLevel)
			DeleteTreeCtrlChildren(treeCtrl,childItem,image,imageSel,oneLevel);
		treeCtrl->GetItemImage(childItem,img,imgSel);
		if (image == -1 || img == image || imgSel == imageSel)
			treeCtrl->DeleteItem(childItem);
		childItem = nextItem;
	}
}

int GetTreeCtrlChildrenCount(CTreeCtrl *treeCtrl, const HTREEITEM &item)
{
	HTREEITEM childItem = treeCtrl->GetChildItem(item);
	HTREEITEM grandChildItem;
	int count = 0;

	while (childItem) {
		if ((grandChildItem = treeCtrl->GetChildItem(childItem)))
			count += GetTreeCtrlChildrenCount(treeCtrl,grandChildItem);
		childItem = treeCtrl->GetNextSiblingItem(childItem);
		count++;
	}

	return count;
}

int GetTreeCtrlChildrenCount(CXTPTreeCtrl *treeCtrl, const HTREEITEM &item)
{
	HTREEITEM childItem = treeCtrl->GetChildItem(item);
	HTREEITEM grandChildItem;
	int count = 0;

	while (childItem) {
		if ((grandChildItem = treeCtrl->GetChildItem(childItem)))
			count += GetTreeCtrlChildrenCount(treeCtrl,grandChildItem);
		childItem = treeCtrl->GetNextSiblingItem(childItem);
		count++;
	}

	return count;
}

void TreeCtrlChildrenStringList(CStringList &childrenList, CXTPTreeCtrl *treeCtrl,
								const HTREEITEM &item, const int image, const int imageSel)
{
	HTREEITEM childItem = treeCtrl->GetChildItem(item);
	CString str;
	int img;
	int imgSel;
	childrenList.RemoveAll();

	while (childItem) {
		str = treeCtrl->GetItemText(childItem);
		treeCtrl->GetItemImage(childItem,img,imgSel);
		if (image == -1 || img == image || imgSel == imageSel)
			childrenList.AddTail(str);
		childItem = treeCtrl->GetNextSiblingItem(childItem);
	}
}

HTREEITEM TreeItemFromName(CXTPTreeCtrl *treeCtrl, const HTREEITEM &item, const CString &itemNameStr)
{
	if (!treeCtrl || !item || itemNameStr == _T(""))
		return NULL;

	CString str;
	HTREEITEM sibling = item;
	HTREEITEM children = NULL;
	HTREEITEM child = NULL;

	while (sibling) {
		str = treeCtrl->GetItemText(sibling);
		if (str == itemNameStr)
			return sibling;
		children = treeCtrl->GetChildItem(sibling);
		if (children) {
			if ((child = TreeItemFromName(treeCtrl,children,itemNameStr)))
				return child;
		}

		sibling = treeCtrl->GetNextSiblingItem(sibling);
	}

	return NULL;
}

HTREEITEM NextTreeCtrlItem(CXTPTreeCtrl *treeCtrl, const HTREEITEM &currentItem, const bool backward,
						   const int image, const int imageSel)
{
	HTREEITEM item = currentItem;
	HTREEITEM nextItem = NULL;
	int img = -1;
	int imgSel = -1;

	while (item) {
		if (backward)
			nextItem = treeCtrl->GetPrevSiblingItem(item);
		else
			nextItem = treeCtrl->GetNextSiblingItem(item);
		// Don't I want to check the NEXT item before enabling it??	// 05/21/08 AM.
//		treeCtrl->GetItemImage(item,img,imgSel);	// 05/21/08 AM.
		treeCtrl->GetItemImage(nextItem,img,imgSel);	// 05/21/08 AM.
		if (img >= image && imgSel <= imageSel)
			break;
		item = nextItem;
		nextItem = 0;	// 05/21/08 AM.
	}
	return nextItem;
}

void PeekHere(const int times)
{
	for (int i=0; i<times; i++)
		PeekHere();
}

void PeekHere()
{
	MSG message;

	if (::PeekMessage(&message,NULL,0,0,PM_REMOVE)) {
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}
}

HTREEITEM TreeCtrlFirstSibling(CTreeCtrl *treeCtrl, const HTREEITEM &currentItem)
{
	HTREEITEM item = currentItem ;
	HTREEITEM firstItem = item;

	while (item) {
		firstItem = item;
		item = treeCtrl->GetPrevSiblingItem(item);
	}
	return firstItem;
}

HTREEITEM TreeCtrlFirstSibling(CXTPTreeCtrl *treeCtrl, const HTREEITEM &currentItem)
{
	HTREEITEM item = currentItem ;
	HTREEITEM firstItem = item;

	while (item) {
		firstItem = item;
		item = treeCtrl->GetPrevSiblingItem(item);
	}
	return firstItem;
}

HTREEITEM TreeCtrlLastSibling(CXTPTreeCtrl *treeCtrl, const HTREEITEM &currentItem)
{
	HTREEITEM item = currentItem ;
	HTREEITEM lastItem = item;

	while (item) {
		lastItem = item;
		item = treeCtrl->GetNextSiblingItem(item);
	}
	return lastItem;
}

HTREEITEM TreeCtrlLastSibling(CTreeCtrl *treeCtrl, const HTREEITEM &currentItem)
{
	HTREEITEM item = currentItem ;
	HTREEITEM lastItem = item;

	while (item) {
		lastItem = item;
		item = treeCtrl->GetNextSiblingItem(item);
	}
	return lastItem;
}

void TrimString(CString &strIn)
{
	strIn.TrimLeft(_T(" \t\n\r"));
	strIn.TrimRight(_T(" \t\n\r"));
}

bool AllUpper(const CString &str)
{
	CString upperStr = str;
	upperStr.MakeUpper();
	return str == upperStr;
}

void SeparateByChar(CString &str, const _TCHAR sep, CStringList &list) {
	CString item;
	int segDone = 0;
	int nth = 0;

	for (int i =0; i < (int)NUM_RECENT_ANALYZERS; i++) {
		segDone = NthPathSegment(str,item,sep,nth++);
		if (segDone < 0)
			break;
		list.AddTail(item);
	}
}

void UniqueStringList(CStringList &startList) {
	CStringList uniqueList;
	CString itemStr;

	POSITION pos = startList.GetHeadPosition();
	while (pos && (itemStr = startList.GetNext(pos))) {
		if (itemStr != _T("") && !uniqueList.Find(itemStr))
			uniqueList.AddTail(itemStr);
	}

	startList.RemoveAll();
	pos = uniqueList.GetHeadPosition();
	while (pos && (itemStr = uniqueList.GetNext(pos))) {
		startList.AddTail(itemStr);
	}
}

void MergeStringLists(CStringList &stringList, CStringList &appendThisList)
{
	POSITION pos = appendThisList.GetHeadPosition();
	CString itemStr = _T("");
	while (pos && (itemStr = appendThisList.GetNext(pos)))
		stringList.AddTail(itemStr);
}

void SwitchInt(int &firstInt, int &secondInt)
{
	int oldFirst = firstInt;
	firstInt = secondInt;
	secondInt = oldFirst;
}

CString PrettyNumber(const int number)
{
	CString numStr;
	numStr.Format(_T("%d"),number);
	CString newNumStr = _T("");
	int len = numStr.GetLength();
	_TCHAR c;

	for (int i=0; i<len; i++) {
		c = numStr.GetAt(i);
		if (i && (len-i) % 3 == 0)
			newNumStr += _T(",");
		newNumStr += c;
	}

	return newNumStr;
}

CString PrettySize(const int number)
{
	CString newNumStr;
	CString numStr;
	newNumStr.Format(_T("%d"),number);

	if (number > 1000000) {
		newNumStr.Format(_T("%.2f megabytes"),((float)number)/1000000);
	}
	else if (number > 1000) {
		newNumStr.Format(_T("%.2f Kbytes"),((float)number)/1000);
	}

	return newNumStr;
}

CString SeparateByCaps(const CString &inStr)
{
	int len = inStr.GetLength();
	_TCHAR c,c2;
	CString newStr = _T("");

	for (int i=0; i<len; i++) {
		c = inStr.GetAt(i);
		if (i+1 < len)
			c2 = inStr.GetAt(i+1);
		else
			c2 = '\0';
		if (c == '_')
			c = ' ';
		else if ('A' <= c && c <= 'Z' && i > 0 && !('A' <= c2 && c2 <= 'Z') && c2 != '_')
			newStr += ' ';
		newStr += c;
	}

	return newStr;
}

int StringToInteger(const CString &inStr)
{
	int intOut = _ttoi((const _TCHAR *)inStr);

	return intOut;
}

int AskUser(const CString &msgStr, const UINT waveID, const int numButtons, const int defaultChoice,
			const CString &butt1Str, const CString &butt2Str, const CString &butt3Str)
{
	CCustomMsgDlg dlg;
	dlg.SetData(msgStr,numButtons,defaultChoice,butt1Str,butt2Str,butt3Str);
	if (waveID)
		PlayWave(waveID); 
	dlg.DoModal();

	return appButtonReturn;
}

bool PlayWave(const UINT waveID)
{
	return false; // SRP 010322
#if 0
	// FROM CODE GURU

    BOOL bRtn;
    LPTSTR lpRes;
    HANDLE hRes;
    HRSRC hResInfo;
    HINSTANCE Nl=AfxGetInstanceHandle();

    /* Find the WAVE resource. */
    //hResInfo= FindResource(Nl,lpName,"WAVE");
    hResInfo = FindResource(Nl,MAKEINTRESOURCE(waveID),_T("WAVE"));
    if(hResInfo == NULL)
       return false;
    /* Load the WAVE resource. */

    hRes = LoadResource(Nl,hResInfo);
    if (hRes == NULL)
      return false;

    /* Lock the WAVE resource and play it. */
    lpRes=(LPTSTR)LockResource(hRes);
    if(lpRes==NULL)
      return false;

    bRtn = sndPlaySound(lpRes, SND_MEMORY | SND_SYNC);
    if(bRtn == NULL)
      return false;

    /* Free the WAVE resource and return success or failure. */
    FreeResource(hRes);
    return true;
#endif
}

int TreeCtrlLongestItem(CTreeCtrl *treeCtrl, const HTREEITEM &currentItem)
{
	int maxLength = 0;
	HTREEITEM item = currentItem ;

	if (!item)
		item = treeCtrl->GetRootItem();
	HTREEITEM child;
	CString itemTextStr;
	int ll;

	while (item) {
		itemTextStr = treeCtrl->GetItemText(item);
		if (itemTextStr.GetLength() > maxLength)
			maxLength = itemTextStr.GetLength();

		child =	treeCtrl->GetChildItem(item);

		if (child) {
			ll = TreeCtrlLongestItem(treeCtrl,child);
			if (ll > maxLength)
				maxLength = ll;
		}

		item = treeCtrl->GetNextSiblingItem(item);
	}

	return maxLength;
}

int TreeCtrlLongestItem(CXTPTreeCtrl *treeCtrl, const HTREEITEM &currentItem)
{
	int maxLength = 0;
	HTREEITEM item = currentItem ;

	if (!item)
		item = treeCtrl->GetRootItem();
	HTREEITEM child;
	CString itemTextStr;
	int ll;

	while (item) {
		itemTextStr = treeCtrl->GetItemText(item);
		if (itemTextStr.GetLength() > maxLength)
			maxLength = itemTextStr.GetLength();

		child =	treeCtrl->GetChildItem(item);

		if (child) {
			ll = TreeCtrlLongestItem(treeCtrl,child);
			if (ll > maxLength)
				maxLength = ll;
		}

		item = treeCtrl->GetNextSiblingItem(item);
	}

	return maxLength;
}

bool TreeCtrlIsToplevelItem(CXTPTreeCtrl *treeCtrl, const HTREEITEM &currentItem)
{
	if (!currentItem)
		return false;

	HTREEITEM item = currentItem ;
	HTREEITEM sibling = treeCtrl->GetRootItem();
	CString itemTextStr;

	while (sibling) {
		itemTextStr = treeCtrl->GetItemText(item);
		if (sibling == item)
			return true;
		sibling = treeCtrl->GetNextSiblingItem(sibling);
	}

	return false;
}

int TreeCtrlVisibleCount(CTreeCtrl *treeCtrl, int &depth, const HTREEITEM &currentItem)
{
	int count = 0;

	HTREEITEM item = currentItem ;
	if (!item) {
		item = treeCtrl->GetRootItem();
		depth = 0;
	}

	HTREEITEM child;
	int state;

	while (item) {
		state = treeCtrl->GetItemState(item,TVIS_EXPANDED) & TVIS_EXPANDED;
		count++;

		child =	treeCtrl->GetChildItem(item);

		if (state && child) {
			count += TreeCtrlVisibleCount(treeCtrl,depth,child);
			depth++;
		}

		item = treeCtrl->GetNextSiblingItem(item);
	}

	return count;
}

int TreeCtrlVisibleCount(CXTPTreeCtrl *treeCtrl, int &depth, const HTREEITEM &currentItem)
{
	int count = 0;

	HTREEITEM item = currentItem ;
	if (!item) {
		item = treeCtrl->GetRootItem();
		depth = 0;
	}

	HTREEITEM child;
	int state;

	while (item) {
		state = treeCtrl->GetItemState(item,TVIS_EXPANDED) & TVIS_EXPANDED;
		count++;

		child =	treeCtrl->GetChildItem(item);

		if (state && child) {
			count += TreeCtrlVisibleCount(treeCtrl,depth,child);
			depth++;
		}

		item = treeCtrl->GetNextSiblingItem(item);
	}

	return count;
}

void SetWindowSize(CView *view, CTreeCtrl *treeCtrl, const bool dontMove)
{
	// SET WINDOW SIZE
	int depth;
	int visibleCount = TreeCtrlVisibleCount(treeCtrl,depth);
	int height = (visibleCount * (treeCtrl->GetItemHeight()+1));
	int width = TreeCtrlLongestItem(treeCtrl) * 8 + (treeCtrl->GetIndent() * depth);
	theApp.SetWindowSize(view,width,height,treeCtrl->GetCount(),dontMove);
}

void SetWindowSize(CView *view, CXTPTreeCtrl *treeCtrl, const bool dontMove)
{
	// SET WINDOW SIZE
	int depth;
	int visibleCount = TreeCtrlVisibleCount(treeCtrl,depth);
	int height = (visibleCount * (treeCtrl->GetItemHeight()+1));
	int width = TreeCtrlLongestItem(treeCtrl) * 8 + (treeCtrl->GetIndent() * depth);
	theApp.SetWindowSize(view,width,height,treeCtrl->GetCount(),dontMove);
}

void TreeItemMove(CXTPTreeCtrl *treeCtrl, const HTREEITEM &fromItem, const HTREEITEM &toItem, const bool alphabeticFlag)
{
	treeCtrl->Expand(toItem, TVE_EXPAND);
	CString nameStr = treeCtrl->GetItemText(fromItem);
	HTREEITEM insertItem = TVI_FIRST;
	if (alphabeticFlag)
		insertItem = AlphabeticalInsertPoint(treeCtrl,toItem,nameStr);
	HTREEITEM newItem = CopyBranch(treeCtrl, fromItem, toItem, insertItem);
	treeCtrl->DeleteItem(fromItem);
	treeCtrl->SelectItem(newItem);
}

HTREEITEM AlphabeticalInsertPoint(CXTPTreeCtrl *treeCtrl, const HTREEITEM &parentItem, const CString &itemNameStr)
{
	CString itemTextStr;
	HTREEITEM childBefore = NULL;
	HTREEITEM startItem;
	if (!parentItem)
		startItem = treeCtrl->GetRootItem();
	else
		startItem = treeCtrl->GetChildItem(parentItem);
	if (!startItem)
		startItem = parentItem;

	while (startItem) {
		itemTextStr = treeCtrl->GetItemText(startItem);
		if (itemNameStr.CompareNoCase(itemTextStr) < 0) {
			if (!childBefore)
				childBefore = TVI_FIRST;
			break;
		}
		childBefore = startItem;
		startItem = treeCtrl->GetNextSiblingItem(startItem);
	}
	return childBefore;
}

CString StringWrap(const CString &str, const int wrapSize)
{
	if (str == _T("") || str.GetLength() < wrapSize)
		return str;

	CStringList words;
	StringToWordList(words,str,true);
	POSITION pos = words.GetHeadPosition();
	CString wordStr;
	CString newStr = _T("");
	CString lineStr = _T("");

	while (pos && (wordStr = words.GetNext(pos))) {
		if (wordStr == _T("\n")) {
			if (newStr.GetLength() > 0)
				newStr += _T("\n");
			newStr += lineStr;
			newStr += _T("\n");
			lineStr = _T("");
		}
		else if (lineStr.GetLength() + wordStr.GetLength() + 1 >= wrapSize) {
			if (newStr.GetLength() > 0)
				newStr += _T("\n");
			newStr += lineStr;
			lineStr = wordStr;
		}
		else {
			if (lineStr.GetLength() > 0)
				lineStr += _T(" ");
			lineStr += wordStr;
		}
	}
	// SRP 010412 - forgot last line!
	if (lineStr.GetLength() > 0) {
		if (newStr.GetLength() > 0)
			newStr += _T("\n");
		newStr += lineStr;
	}

	return newStr;
}

#ifdef _OLD_
void ParseLogLine(const CString &fileTextStr, CString &lineStr, const int line,
				  long &startSel, long &endSel, int &passNum, int &lineNum, int &indent,
				  CString &conceptStr, CString &semStr, CString &rangeStr,
				  bool &firedFlag, bool &builtFlag, BOOL bPrettify) 
{
	indent = IndentSize(lineStr);

	lineStr.TrimLeft();
	lineStr.TrimRight();

	int start = lineStr.ReverseFind('[') + 1;
	int end = lineStr.ReverseFind(']');
	CString attrStr = lineStr.Mid(start,end-start);
	firedFlag = attrStr.Find(_T(",fired")) > 0 ? true : false;
	builtFlag = attrStr.Find(_T(",blt")) > 0 ? true : false;

	CStringList tokens;
	TokensBySeparator(tokens,attrStr,',');
	NthPathSegmentLong(attrStr,startSel,',',0);
	NthPathSegmentLong(attrStr,endSel,',',1);
	NthPathSegmentInt(attrStr,passNum,',',2);
	NthPathSegmentInt(attrStr,lineNum,',',3);

	// GATHER SEM STUFF
	int n = 4;
	semStr = _T("");
	bool blockSemantics = false;

	CString semPieceStr;
	while (NthPathSegment(attrStr,semPieceStr,',',n++) >= 0) {
		// Collecting tree node semantics in every case.	// 10/29/03 AM.
//		if (!appBoolTreeSemantics && semPieceStr.GetAt(0) == '(') {	// 10/29/03 AM.
//			blockSemantics = true;	// 10/29/03 AM.
//			break;	// 10/29/03 AM.
//		}
		if (semStr != _T(""))
			semStr += _T(" ");
		semStr += semPieceStr;
	}
	// Collecting tree node semantics in every case.	// 06/27/02 AM.
//	if (!blockSemantics && semPieceStr != "") {	// 06/27/02 AM.
	if (semPieceStr != _T("")) {	// 06/27/02 AM.
		if (semStr != _T(""))
			semStr += _T(" ");
		semStr += semPieceStr;
	}

	conceptStr = lineStr.Mid(0,start-2);
	// if (startSel+line < m_longFileLength)
	if (endSel <= fileTextStr.GetLength())      // TRYING // 09/27/00 AM.
		rangeStr = fileTextStr.Mid(startSel,endSel-startSel+1);
	else
		rangeStr = _T("<TEXT HAS BEEN EDITED SINCE LAST RUN!>");
	//rangeStr = MidStr(m_strLogText,startSel,endSel-startSel+1);
	TrimString(rangeStr);

	if (bPrettify) { // SRP 010608
		// CHOP IF TOO LONG FOR DISPLAY
		if (rangeStr.GetLength() > PARSE_TREE_LINE_MAX) {
			rangeStr = rangeStr.Mid(0,PARSE_TREE_LINE_MAX-3);
			rangeStr += _T("...");
		}

		// REPLACE SLASH N FOR DISPLAY PURPOSES
		rangeStr.Replace(_T("\n"),_T(" "));
		rangeStr.Replace(_T("\r"),_T(" "));
		rangeStr.Replace(_T("\t"),_T(" "));
	}
}
#endif

// OPTIMIZING:	Very slow to load even a couple of MB log file.	// 11/29/06 AM.
// EXAMPLE:	   _head [7,12,4,27,node, ("name" "head") ("tag" 1)]
// COLOR: Adding a fifth number, default 000000000 for RGB colors.	// 12/09/09 AM.
void ParseLogLine(const CString &fileTextStr, CString &lineStr, const int line,
				  long &startSel, long &endSel, int &passNum, int &lineNum, int &indent,
				  CString &conceptStr, CString &semStr,
				  CString &semVars,	// OPT.	// 12/01/06 AM.
				  CString &rangeStr,	// Rename for consistency.	// 12/02/06 AM.
				  bool &firedFlag, bool &builtFlag,
				  bool bQuick,	// Just getting offsets.	// OPT.	// 12/02/06 AM.
				  BOOL bPrettify) 
{
	LPCTSTR x = lineStr;
	_TCHAR *ptr = (_TCHAR *) x;
//	_TCHAR *ptr = (LPCTSTR) lineStr;	// OPT. // 11/30/06 AM.
	indent = IndentSize(ptr);

	// Parse to get concept name in log file.
	_TCHAR buf[4096];	// Temporary buffer for string being collected.
	_TCHAR *pbuf = buf;
	*pbuf = *ptr;
	while ((*++pbuf = *++ptr) != ' ');
	*pbuf = '\0';	// Terminate.
	conceptStr = buf;
	
	// Get to left bracket.
	while (*ptr != '[' && *ptr != '\0') {
		*++ptr;
	}
	
	// Get start offset in text.
	pbuf = buf - 1;
	while ((*++pbuf = *++ptr) != ',');
	*pbuf = '\0';	// Terminate.
	startSel = _ttol(buf);
	
	// Get end offset in text.
	pbuf = buf - 1;
	while ((*++pbuf = *++ptr) != ',');
	*pbuf = '\0';	// Terminate.
	endSel = _ttol(buf);

	// Get pass number.
	pbuf = buf - 1;
	while ((*++pbuf = *++ptr) != ',');
	*pbuf = '\0';	// Terminate.
	passNum = _ttol(buf);

	// Get rule line number.
	pbuf = buf - 1;
	while ((*++pbuf = *++ptr) != ',');
	*pbuf = '\0';	// Terminate.
	lineNum = _ttol(buf);
	
	if (bQuick)	// Parsing just for some quick info.
		return;

	// Type. node, alpha, num, punct, white, or ctrl.
	// un = unsealed node attr.
	// Because we're not sure what is present or how it's ordered,
	// Gather stuff up into "semantics" from here.
	firedFlag = builtFlag = false;
	_TCHAR cc;
	semStr = _T("");
	semVars = _T("");
	bool foundvars = false;

	for (;;)	// Gather rest of attributes.
		{
		pbuf = buf - 1;
		while ((cc = *++ptr)
		  && cc != ','
		  && cc != ']')
			*++pbuf = cc;
		*++pbuf = '\0';	// Terminate.
		
		if (!_tcscmp(buf,_T("fired")))
			firedFlag = true;
		else if (!_tcscmp(buf,_T("blt")))
			builtFlag = true;
		else	// Collect into semantic string.
			{
			if (semStr != _T(""))
				semStr += _T(" ");
			semStr += buf;
			}

		if (!cc || cc == ']')
		  break;
		if (*(ptr+1) == ' ' && *(ptr+2) == '(')
			{
			ptr += 2;
			foundvars = true;	// Start of variables.
			break;
			}
		}

	if (!foundvars)
		{
		semVars = _T("");
		}
	else	// Found semantic var-val pairs.
		{
		// Parse variables properly.  Watch for embedded parens, quotes.
		// When debugged, can remove the nitty-gritty error checking.

		// SUPPORTING MULTI-COLUMN VAR-VAL PAIR DISPLAY (IN "YELLOW BOX").	// 09/09/08 AM.
		// Collect into an array of strings, then format as you like.			// 09/09/08 AM.
		int vcount = 0;       // Count var.              // 09/09/08 AM.
		CStringArray varr;    // Var-val strings array.  // 09/09/08 AM.
		varr.SetSize(128,128);                           // 09/09/08 AM.
		_TCHAR *pair = buf;	// Point to start of var-val pair.	// 09/09/08 AM.

		pbuf = buf - 1;	// Collect all var-val pairs as is.
		bool gotname = false;	// If got variable name. (Toggle yes/no).
		cc = *ptr;	// Lookahead char.
		for (;;)	// For each variable.
			{
			if (cc != '(')		// Start of variable.
				goto varerr;
			*++pbuf = '(';	// 09/09/08 AM.
			varr[vcount] += '(';	// 09/09/08 AM.
			++ptr;	// Move past left paren.
			if (!GetTextString(ptr,varr[vcount],pbuf))	// Get var name.	// 09/09/08 AM.
				goto varerr;
			if (*ptr != ' ')	// Get intervening space.
				goto varerr;
			*++pbuf = ' ';	// 09/09/08 AM.
			varr[vcount] += ' ';	// 09/09/08 AM.
			for (;;)	// GET VAR VALUE(S)	// FIX	// 05/09/07 AM.
				{
				if (*++ptr == '"')
					{
					if (!GetTextString(ptr,varr[vcount],pbuf))	// Get var value.	// 09/09/08 AM.
						goto varerr;
					cc = *ptr;
					}
				else
					{
					// Collect non-string value.
					*++pbuf = *ptr;	// 09/09/08 AM.
					varr[vcount] += *ptr;	// 09/09/08 AM.
					while ((cc = *++ptr) && cc != ')' && cc != ' ')
						{
						*++pbuf = cc;	// 09/09/08 AM.
						varr[vcount] += cc;	// 09/09/08 AM.
						}
					}
				if (cc == ' ')	// Multiple values.	// 05/09/07 AM.
					{
					*++pbuf = ' ';							// 09/09/08 AM.
					varr[vcount] += ' ';	// 09/09/08 AM.
					continue;								// 05/09/07 AM.
					}
				if (cc != ')')	// End of variable.
					goto varerr;
				break;	// Done with values.	// 05/09/07 AM.

				}	// END VAR VALUE(S).	// FIX	// 05/09/07 AM.

			*++pbuf = ')';	// 09/09/08 AM.
			varr[vcount] += ')';	// 09/09/08 AM.
			cc = *++ptr;
			if (cc == ']')	// End of all var-val pairs.
				{
				// Terminate buffers etc.
				goto vargood;
				}
			if (cc != ' ')	// Separator between var-val pairs.
				goto varerr;
			*++pbuf = '\0';	// Hack. Terminate to grab var-val pair.	// 09/09/08 AM.
			++vcount;	// Track number of pairs.	// 09/09/08 AM.
			*pbuf = ' ';		// Track as-is.	// 09/09/08 AM.
			cc = *++ptr;	// Lookahead and loop to next var-val.
			}
varerr:
		semVars = _T("");	// Report error here.
		goto vardone;
vargood:
		;  // Got good variables.
		++vcount;	// FIX.	// 09/15/08 AM.
		// Arrange var-val pairs into columns.	// 09/09/08 AM.
		StringsToColumns(varr,semVars,vcount);	// 09/09/08 AM.
//		semStr += " ";	// 05/09/07 AM.
//		semStr += buf;	// 05/09/07 AM.
vardone:
		;
		}

	// Get corresponding text.
	// Get and munge the text efficiently.	// OPT.	// 12/01/06 AM.

	long siz = endSel - startSel + 1;	// 12/02/06 AM.
	if (siz > 0 && (endSel <= fileTextStr.GetLength()))
		{
		if (siz > PARSE_TREE_LINE_MAX)
			siz = PARSE_TREE_LINE_MAX;
		rangeStr = fileTextStr.Mid(startSel,siz);
		TrimString(rangeStr);

		// REPLACE SLASH N FOR DISPLAY PURPOSES
		//rangeStr.Replace(_T("\n"),_T(" "));
		//rangeStr.Replace(_T("\r"),_T(" "));
		//rangeStr.Replace(_T("\t"),_T(" "));
		}
	else if (endSel > fileTextStr.GetLength())
		rangeStr = _T("<TEXT HAS BEEN EDITED SINCE LAST RUN!>");
	else
		rangeStr = _T("");
}
////////////////// end ParseLogLine

bool GetTextString(
	_TCHAR *&ptr,	// Pointer to first double quote of string.
	CString &str,	// Another tracking string.	// 09/09/08 AM.
	_TCHAR *&pbuf	// Pointer to as-is tracking buffer.
	)
{
_TCHAR cc = *ptr;
if (cc != '"')
	return false;
*++pbuf = '"';	// 09/09/08 AM.
str += '"';	// 09/09/08 AM.
bool bspending = false;	// If backslash is pending.
while (cc = *++ptr)
	{
	*++pbuf = cc;	// 09/09/08 AM.
	str += cc;	// 09/09/08 AM.
	switch (cc)
		{
		case '"':
			if (!bspending)
				{
				++ptr;			// Move to next char.
				return true;	// Good end of string.
				}
			// Escaped double quote. Continue.
			break;
		case '\\':
			// Toggle pending or not (odd num means pending).
			bspending = !bspending;
			break;
		default:
			bspending = false;	// Some other char may have been escaped.
			// Continue.
			break;
		}
	}
// Error. Ran out of characters.
return false;
}

//////////////////////////////////////////

// Split var-val pairs into two columns.
// Todo: Can generalize to more columns.
void StringsToColumns(CStringArray &arr, CString &semVars, int len)
{
semVars = _T("");	// Clear this.

int ncolumns = 2;	// Split into two columns.
int field = 30;	// Length to fill for column1. To align columns nicely.

if (!len || len > arr.GetSize())
	return;

int n1 = 0;
int n2 = len / ncolumns;

if (len % ncolumns)
  ++n2;

int col2 = n2;

// Traverse the columns simultaneously.
int ll = 0;
for (;;)
	{
	if (n1 >= col2 && n2 >= len)
		return;
	if (n1 < col2)
		{
		CString s = arr[n1++];
		ll = field - s.GetLength();
		if (ll <= 0)
			ll = 1;	// Always a space between columns.
		semVars += s;
		}
	if (n2 < len)
		{
		// NOTE: Tab doesn't display in the yellow box.  How to fix?  // 09/14/08 AM.
		// (DT_TABSTOP and DT_EXPANDTABS seem to do nothing, in SemPopup.	// 09/14/08 AM.
//		semVars += '\t';	// 09/14/08 AM.
		for (; ll > 0;--ll)	// 09/14/08 AM.
		  semVars += ' ';	// 09/14/08 AM.
		semVars += arr[n2++];
		}
	semVars += '\n';
	}
}

//////////////////////////////////////////

void PasteTextToClipboard(const CString &textStr)
{
	COleDataSource*	pSource = new COleDataSource();
	CSharedFile	sf(GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT);

	sf.Write(textStr, textStr.GetLength());

	HGLOBAL hMem = sf.Detach();
	if (!hMem) return;
#ifdef UNICODE
	pSource->CacheGlobalData(CF_UNICODETEXT, hMem);
#else
	pSource->CacheGlobalData(CF_TEXT, hMem);
#endif
	pSource->SetClipboard();
}

void GetClipboardText(CString& strClipboardText)
{
#ifdef UNICODE
	if (IsClipboardFormatAvailable (CF_UNICODETEXT)) {
#else
	if (IsClipboardFormatAvailable (CF_TEXT)) {
#endif
		if (OpenClipboard (AfxGetMainWnd()->GetSafeHwnd())) {
			HANDLE hDataCB;
			LPTSTR lpDataCB;
#ifdef UNICODE
			if (hDataCB = GetClipboardData (CF_UNICODETEXT)) {
#else
			if (hDataCB = GetClipboardData (CF_TEXT)) {
#endif
				if (lpDataCB = (LPTSTR)GlobalLock (hDataCB)) {
					strClipboardText = lpDataCB;
					GlobalUnlock (hDataCB);
				}
			}
			CloseClipboard();
		}
	}
}

void WindowBell(DWORD tone)
{
	MessageBeep(tone);
}

/****************
// SRP 010417
// Creates a File Association between VT and .ana files, if not already created
// Need to create the following entries

HKEY_CLASSES_ROOT\.ana
  (Default)				REG_SZ		VT ANA File

HKEY_CLASSES_ROOT\VT ANA File
  (Default)				REG_SZ		VisualText Analyzer Project
HKEY_CLASSES_ROOT\VT ANA File\shell
  (Default)		 		REG_SZ		open
HKEY_CLASSES_ROOT\VT ANA File\shell\open
  (Default)				REG_SZ		(value not set)
HKEY_CLASSES_ROOT\VT ANA File\shell\open\command
  (Default)				REG_SZ		"C:\Program Files\TextAI\VisualText\bin\VisualText.exe" "%1"
****************/

void CheckFileAssociation(void)
{
	HKEY	hKeyExtension;
	HKEY	hKeyProgID;
	HKEY	hKeyShell;
	HKEY	hKeyOpen;
	HKEY	hKeyCommand;
	long	keyResult;
	DWORD	valType = REG_SZ;
	_TCHAR	value[1024];
	DWORD	regSize = sizeof(value);
    DWORD keyDisposition;	// set via RegCreateKeyEx
	LPCTSTR	strExtension = _T(".ana");
	CString	strProgID = _T("VT Ana File");
	CString	strProgDescription = _T("Visual Text Project");
	CString	strShell = _T("shell");
	CString	strOpen = _T("open");
	CString	strCommand = _T("command");

	_TCHAR exePath[MAX_PATH];
	GetModuleFileName (AfxGetInstanceHandle(), exePath, sizeof(exePath));
	CString strExePath;
	strExePath.Format(_T("\"%s\" \"%%1\""), (LPCTSTR)exePath);

	BOOL bExists = false;
	SECURITY_ATTRIBUTES	sa;
	SECURITY_ATTRIBUTES	*saPtr = NULL;
	PSECURITY_DESCRIPTOR pSD    = NULL;

	{
		//  Setup a security attribute that provides access. Don't inherit 
		pSD = (PSECURITY_DESCRIPTOR) LocalAlloc( LPTR,
						 SECURITY_DESCRIPTOR_MIN_LENGTH );
		if ( pSD == NULL )
			return;

		if ( !InitializeSecurityDescriptor( pSD, SECURITY_DESCRIPTOR_REVISION ) )
			goto _exit;

		// add a NULL disc. ACL to the security descriptor.
		if ( !SetSecurityDescriptorDacl( pSD, TRUE, ( PACL ) NULL, FALSE ) )
			goto _exit;

		sa.nLength		= sizeof( sa );
		sa.lpSecurityDescriptor = pSD;
		sa.bInheritHandle	= FALSE;
		saPtr = &sa;
    }

//	HKEY_CLASSES_ROOT\.ana
//		(Default)				REG_SZ		VT ANA File
	keyResult = RegCreateKeyEx(HKEY_CLASSES_ROOT, strExtension, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | KEY_EXECUTE | KEY_SET_VALUE,
		saPtr, &hKeyExtension, &keyDisposition);
	if (keyResult != ERROR_SUCCESS)
		goto _exit;
	bExists = false;
	regSize = sizeof(value);
	keyResult = RegQueryValueEx(hKeyExtension, NULL, 0, &valType, (LPBYTE)value, &regSize);
	if (keyResult == ERROR_SUCCESS) {
		if (!strProgID.Compare(value))
			bExists = true;
	}
	if (!bExists) {
		keyResult = RegSetValueEx(hKeyExtension, NULL, 0, REG_SZ, (LPBYTE)(LPCTSTR)strProgID, sizeof(TCHAR) * (strProgID.GetLength() + 1) );
		int rv = keyResult == ERROR_SUCCESS;
	}
	RegCloseKey (hKeyExtension);


//	HKEY_CLASSES_ROOT\VT ANA File
//		(Default)				REG_SZ		VisualText Analyzer Project
	keyResult = RegCreateKeyEx(HKEY_CLASSES_ROOT, strProgID, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | KEY_EXECUTE | KEY_SET_VALUE,
		saPtr, &hKeyProgID, &keyDisposition);
	if (keyResult != ERROR_SUCCESS)
		goto _exit;
	bExists = false;
	regSize = sizeof(value);
	keyResult = RegQueryValueEx(hKeyProgID, NULL, 0, &valType, (LPBYTE)value, &regSize);
	if (keyResult == ERROR_SUCCESS)
		bExists = !strProgDescription.Compare(value);
	if (!bExists) {
		keyResult = RegSetValueEx(hKeyProgID, NULL, 0, REG_SZ, (LPBYTE)(LPCTSTR)strProgDescription, sizeof(TCHAR) * (strProgDescription.GetLength() + 1) );
		int rv = keyResult == ERROR_SUCCESS;
	}

	
//	HKEY_CLASSES_ROOT\VT ANA File\shell
	keyResult = RegCreateKeyEx(hKeyProgID, strShell, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | KEY_EXECUTE | KEY_SET_VALUE,
		saPtr, &hKeyShell, &keyDisposition);
	if (keyResult != ERROR_SUCCESS) {
		RegCloseKey (hKeyProgID);
		goto _exit;
	}

//	HKEY_CLASSES_ROOT\VT ANA File\shell\open
	keyResult = RegCreateKeyEx(hKeyShell, strOpen, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | KEY_EXECUTE | KEY_SET_VALUE,
		saPtr, &hKeyOpen, &keyDisposition);
	if (keyResult != ERROR_SUCCESS) {
		RegCloseKey (hKeyShell);
		RegCloseKey (hKeyProgID);
		goto _exit;
	}

//	HKEY_CLASSES_ROOT\VT ANA File\shell\command
//  (Default)				REG_SZ		"C:\Program Files\TextAI\VisualText\bin\VisualText.exe" "%1"
	keyResult = RegCreateKeyEx(hKeyOpen, strCommand, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | KEY_EXECUTE | KEY_SET_VALUE,
		saPtr, &hKeyCommand, &keyDisposition);
	if (keyResult != ERROR_SUCCESS) {
		RegCloseKey (hKeyOpen);
		RegCloseKey (hKeyShell);
		RegCloseKey (hKeyProgID);
		goto _exit;
	}
	bExists = false;
	regSize = sizeof(value);
	keyResult = RegQueryValueEx(hKeyCommand, NULL, 0, &valType, (LPBYTE)value, &regSize);
	if (keyResult == ERROR_SUCCESS)
		bExists = !strExePath.Compare(value);
	if (!bExists) {
		keyResult = RegSetValueEx(hKeyCommand, NULL, 0, REG_SZ, (LPBYTE)(LPCTSTR)strExePath, sizeof(TCHAR) * (strExePath.GetLength() + 1) );
		int rv = keyResult == ERROR_SUCCESS;
	}

	RegCloseKey (hKeyCommand);
	RegCloseKey (hKeyOpen);
	RegCloseKey (hKeyShell);
	RegCloseKey (hKeyProgID);

_exit:
	if (pSD)
		LocalFree ( (HLOCAL)pSD );
}

BOOL UTLSelectFolder(HWND hWnd, LPCTSTR prompt, CString &dirSpec, BOOL startAtMyComputer)
{
    BROWSEINFO	    bi;
    TCHAR			displayName[MAX_PATH]		= {0};
    TCHAR			selectedFolder[MAX_PATH]	= {0};
    LPITEMIDLIST	rootList					= NULL;
	BOOL			folderSelected				= FALSE;
    LPMALLOC		pMalloc;

	SHGetMalloc( &pMalloc );

    if ( startAtMyComputer )
    {
		SHGetSpecialFolderLocation( hWnd, CSIDL_DRIVES, &rootList );
    }

    ZeroMemory( &bi, sizeof( BROWSEINFO ) );

    bi.hwndOwner    = hWnd;
    bi.pidlRoot	    = rootList;
    bi.lpszTitle    = prompt;
    bi.ulFlags	    = BIF_RETURNONLYFSDIRS;

    LPITEMIDLIST pItem = SHBrowseForFolder( &bi );

    if ( pItem != NULL )
    {
		if ( SHGetPathFromIDList( pItem, selectedFolder ) )
		{
			dirSpec			= selectedFolder;
			folderSelected	= TRUE;
		}

		pMalloc->Free( pItem );
    }

	if ( startAtMyComputer )
	{
		pMalloc->Free( rootList );
	}

	pMalloc->Release();
	

    return folderSelected;

} // end UTLSelectFolder

void UTLReadRect (CRect& cR, LPCTSTR lpString)
{
	LPTSTR p = (LPTSTR)lpString;
	for (int i = 0; i < 4; i++) {
		switch (i) {
			case 0:	cR.left = _ttoi(p); break;
			case 1:	cR.top = _ttoi(p); break;
			case 2:	cR.right = _ttoi(p); break;
			case 3:	cR.bottom = _ttoi(p); break;
			default: break;
		}
		if (i == 4) break;
		while (*p && _istdigit(*p))
			p++;
		while (*p && !_istdigit(*p))
			p++;
	}
}

#define SM_XVIRTUALSCREEN       76
#define SM_YVIRTUALSCREEN       77
#define SM_CXVIRTUALSCREEN      78
#define SM_CYVIRTUALSCREEN      79
void UTL_GetDesktopRect (CRect &cR)
{
	OSVERSIONINFO osVer;
    ZeroMemory( &osVer, sizeof( OSVERSIONINFO ) );
    osVer.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &osVer );
	BOOL bIsWindows98orLater = (osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
		( (osVer.dwMajorVersion > 4) ||
		( (osVer.dwMajorVersion == 4) && (osVer.dwMinorVersion > 0) ) );
	BOOL bIsNT5orLater = (osVer.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
		osVer.dwMajorVersion >= 5;
    if (bIsWindows98orLater || bIsNT5orLater) {
		cR.left = GetSystemMetrics (SM_XVIRTUALSCREEN);
		cR.top = GetSystemMetrics (SM_YVIRTUALSCREEN);
		cR.right = GetSystemMetrics (SM_CXVIRTUALSCREEN);
		cR.bottom = GetSystemMetrics (SM_CYVIRTUALSCREEN);
	}
	else {
		cR.left = 0;
		cR.top = 0;
		cR.right = GetSystemMetrics (SM_CXSCREEN);
		cR.bottom = GetSystemMetrics (SM_CYSCREEN);
	}
}

void UTLGetFileVersion(CString& strFileVersionInResourceFile)
{
	VS_FIXEDFILEINFO	*fVer = NULL;
	UINT				fVerLen = 0;
	BYTE				*fInfo  = NULL;
	DWORD				fInfoSize;
	DWORD				fInfoHandle;
	WORD				major1, major2, minor1, minor2;
	_TCHAR				exePath[MAX_PATH];
	GetModuleFileName (AfxGetInstanceHandle(), exePath, sizeof(exePath));
	if ( (fInfoSize = GetFileVersionInfoSize(exePath, &fInfoHandle)) != 0) {
		if ( (fInfo = new BYTE[fInfoSize]) != NULL) {
			if (GetFileVersionInfo(exePath, fInfoHandle, fInfoSize, fInfo)) {
				if (VerQueryValue( fInfo, _T("\\"), (LPVOID*)&fVer, &fVerLen)) {
					major1 = HIWORD(fVer->dwFileVersionMS);
					major2 = LOWORD(fVer->dwFileVersionMS);
					minor1 = HIWORD(fVer->dwFileVersionLS);
					minor2 = LOWORD(fVer->dwFileVersionLS);
					strFileVersionInResourceFile.Format(_T("%u.%u.%u.%u"), major1, major2, minor1, minor2);
				}
			}
			delete [] fInfo;
		}
	}
}

////////////////////////////////////////////

//////////////////
// Helper function to register a new window class based on an already
// existing window class, but with a different name and icon. 
// Returns new name if successful; otherwise NULL.
//
LPCTSTR UTLRegisterSimilarClass(LPCTSTR lpszNewClassName, LPCTSTR lpszOldClassName, UINT nIDResource)
{
	// Get class info for old class.
	HINSTANCE hInst = AfxGetInstanceHandle();
	WNDCLASS wc;
	if (!::GetClassInfo(hInst, lpszOldClassName, &wc)) {
		TRACE(_T("Can't find window class %s\n"), lpszOldClassName);
		return NULL;
	}

	// Register new class with same info, but different name and icon.
	wc.lpszClassName = lpszNewClassName;
	wc.hIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(nIDResource));
	if (!AfxRegisterClass(&wc)) {
		TRACE(_T("Unable to register window class%s\n"), lpszNewClassName);
		return NULL;
	}
	return lpszNewClassName;
}

typedef struct tag_EnumWindow1 {
	HWND hWndFound;
	CString strAnalyzerFile;
} ENUMWIN1;
typedef ENUMWIN1 FAR * LPENUMWIN1;

LPCTSTR lpMainFrameClassName = _T("VisualText MainFrame");
BOOL FAR PASCAL EnumWinProc1(HWND hWndTop, LPENUMWIN1 lpTW)
{
	if (AfxGetMainWnd()->GetSafeHwnd() == hWndTop)
		return 1;

	_TCHAR szClassName[128];
	GetClassName(hWndTop, (LPTSTR)szClassName, sizeof(szClassName));
	if (lstrcmpi((LPTSTR)szClassName, lpMainFrameClassName))
		return 1;

	COPYDATASTRUCT cds;
	_TCHAR data[1024];
	_tcscpy_s(data, lpTW->strAnalyzerFile);
	cds.cbData = sizeof(data);
	cds.lpData = (LPVOID)data;
	cds.dwData = MSG_GETLOADEDANALYZER;
	if (::SendMessage (hWndTop, WM_COPYDATA, (WPARAM)AfxGetMainWnd()->GetSafeHwnd(), (LPARAM)(PCOPYDATASTRUCT)&cds)) {
		lpTW->hWndFound = hWndTop;
		return 0;
	}

	return 1;
}

BOOL UTLFindRunningVTInst (LPCTSTR lpAnalyzerFile)
{
	ENUMWIN1 tw;
	tw.hWndFound = 0;
	tw.strAnalyzerFile = lpAnalyzerFile;
	EnumWindows ((WNDENUMPROC)EnumWinProc1, (LPARAM)(LPENUMWIN1)&tw);
	if (tw.hWndFound) {
		CString strMsg;
		strMsg.Format(_T("\tThe analyzer:\n\n%s\n\n\tcannot be opened!\n\n")
			_T("It has already been opened by another running instance of VisualText.\n")
			_T("     Would you like to switch to that instance of VisualText?"), lpAnalyzerFile);
		if (IDYES == AfxMessageBox(strMsg, MB_YESNO))
			AfxGetMainWnd()->PostMessage(WMU_BRINGWINDOWTOTOP, (WPARAM)tw.hWndFound);
		return true;
	}
	return false;
}

int CharCount(CString str, _TCHAR c)
{
	int count = 0;
	int len = str.GetLength();

	for (int i=0; i<len; i++) {
		if (str.GetAt(i) == c)
			count++;
	}
	return count;
}

CString RepeatStr(CString str, int count)
{
	CString charStr = _T("");
	for (int i=0; i<count; i++)
		charStr += str;
	return charStr;
}

void StartWaitCursor()
{
	theApp.BeginWaitCursor();
}

void StopWaitCursor()
{
	theApp.EndWaitCursor();
}