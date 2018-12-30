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
/*        types so that input items are const if   */
/*        not modified by the function and so that */
/*        objects are passed by reference not value*/
/*        to avoid problems with side effects      */
/***************************************************/ 

#include <afxole.h>         // MFC OLE classes
#include "XZip.h"
#include "XUnzip.h"

//data structure for setdata
struct ListData
{
	ListData(const CString &strTxt);
	CString strText;
};

enum DEBUG_TYPE {
	DEBUG_NOOP = 0,
	DEBUG_ERROR,
	DEBUG_FIND,
	DEBUG_PASS_LINE,
	DEBUG_RULE,
};

enum STATE_TYPE {
	STATE_GET_EXPANDED = 0,
	STATE_GET_SELECTED,
	STATE_GET_CHECKED,
	STATE_EXPAND,
	STATE_COLLAPSE,
	STATE_EXPAND_ONLY,
	STATE_EXPAND_COLLAPSE,
	STATE_SELECT,
};

// Sync with LITE library.	// 10/07/05 AM.
//#define FILE_BUFF_MAX 5120
#define FILE_BUFF_MAX 20000

//#define FILE_BUFF_MAX 5000
#define MAINSTR _T("MAIN")

bool FindFileInSubDirectory(CString filePathStr);
int FindMenuItem(CMenu* Menu, LPCTSTR MenuString);
bool DeleteDirectory(LPCTSTR lpszDir, bool noRecycleBin = true);
bool FileExists(const CString &fileName);
long GetFileLength(const CString &fileNameStr);
CString IndentStr(const int size, const int tabsize);
int StripIndent(CString &indentedStr);
CString AnalyzerSequencePath();
CString AnalyzerSequencePath(const CString &anaNameStr);
void SetAppName();
CString AppPath();
CString AppPath(const LPCTSTR dirStr);
CString AppPath(const CString &dirStr, const CString &fileName, const CString &extStr=_T(""));
CString AppPathRel();
CString AppPathRel(const CString &dirStr);
CString AppPathRel(const CString &dirStr, const CString &fileName, const CString &extStr=_T(""));
CString KBPath(const CString &dirStr=_T(""));
CString KBPathRel(const CString &dirStr=_T(""));
CString GramPath(const CString &fileName=_T(""), const CString &extStr=_T(""));
CString GramPathRel(const CString &fileName=_T(""), const CString &extStr=_T(""));
CString InputPath(const CString &fileName=_T(""), const CString &extStr=_T(""));
CString InputPathRel(const CString &fileName=_T(""), const CString &extStr=_T(""));
CString OutputPath(const CString &fileName=_T(""), const CString &extStr=_T(""));
CString OutputPathRel(const CString &fileName=_T(""), const CString &extStr=_T(""));
CString LogsPath(const CString &fileName=_T(""), const CString &extStr=_T(""));
CString LogsPathRel(const CString &fileName=_T(""), const CString &extStr=_T(""));
CString OutAnaPath(int passNum);
CString OutAnaPath(const CString &pathStr, const int passNum);
CString RulesPath(const CString &fileName=_T(""), const bool noExt=false, const CString &extStr=_T(".nlp"));
CString RulesPathRel(const CString &fileName=_T(""), const bool noExt=false);
CString AnalyzerFile();
CString AnalyzerFile(const CString &nameStr);
bool NLPWriteFile(const CString &fileName, const CString &contentStr);
bool AppendToFile(const CString &fileName, const CString &appendStr);
bool AppendFileToFile(const CString &targetFileStr, const CString &sourceFileStr);
CString NLPReadFile(const CString &fileName);
CString NLPReadInputFile(const CString &fileName);	// 08/20/08 AM.
void NLPFileTextWidthLength(const CString &fileName, long &width, long &length, long &fileLength);
void ReadFileToStringList(CStringList &wordList, const CString &fileName);
bool InitFile(const CString &filePath, const CString &fileName, const CString &subjectStr);
bool InitSequenceFile();
int GetDocumentCount();
CString SystemDateTime();
bool FileHasExtensions(const CString &fileName, const CStringList &extStrs);
CString StripPath(const CString &fileName, const int level, const bool leftSideFlag = false, const _TCHAR separator='\\');
CString StripPathReverse(const CString &fileName, const int level, const bool rightSideFlag = false, const _TCHAR separator='\\');
BOOL IsDirectory(const CString &pathStr);
BOOL AnyFilesHaveExtension(const CString &ext, const CString &pathStr);
BOOL ChangeFilesExtensions(const CString &path, const CString &fromExt, const CString &toExt);
void DirectoryFiles(CStringList &outList, const CString &pathStr, const bool fullPath=FALSE,  const CString &extStr=_T(""), const bool includeDirFlag=false);
bool RemoveDirectory(const CString &dirPath);
bool AskForText(const CString &msgStr, const CString &startStr = _T(""));
CString RemoveExtension(const CString &fileName);
CString MakeLogDirectory(const CString &fileName);
bool EndsWith(const CString &str, const CString &endingStr);
void NLPCopyFile(const CString &origFileStr, const CString &destFileStr);
BOOL UTL_Rename(LPCTSTR oldName, LPCTSTR newName);
void CopyDirectory(const CString &origPathStr, const CString &destPathStr, const CStringList &extStrs);
bool RecursiveRemove(const CString &pathStr, const bool childrenOnly=false, const int level=-1, const bool debugMsgFlag=true);
CString RelativizePath(const CString &dirPathStr, const CString &removeStr=_T(""));
int NthPathSegment(const CString &strIn, CString &strOut, const _TCHAR splitter, const int nth);
int NthPathSegmentInt(const CString &strIn, int &intOut, const _TCHAR splitter, const int nth);
int NthPathSegmentBool(const CString &strIn, bool &boolOut, const _TCHAR splitter, const int nth);
int NthPathSegmentLong(const CString &strIn, long &longOut, const _TCHAR splitter, const int nth);
bool MakeDirectory(const CString &dirPathStr);
void HelpTopic(const CString &topicStr = _T(""), DWORD dwContextID=0);
void PopupContextMenu(CWnd *wnd, const int nID, const CPoint &point);
void PopupContextMenu(CWnd *wnd, CMenu *context, const CPoint &point, UINT disables[]);
void PopupContextMenu(CWnd *wnd, const int nID, const CPoint &point, UINT disables[]);
void ClipLeft(CString &inStr, const int clipping);
void ClipRight(CString &inStr, const int clipping);
int FileContains(CString filePathStr, CString searchStr, CString &lineStr, const int startLine, int &inLineIndex, const bool noCase=true,const bool multi=false);
int IndentSize(const CString &str, const int tabSize=0);	// Old one.	// 05/21/07 AM.
int IndentSize(_TCHAR *&str, const int tabSize=0);	// OPT.	// 11/30/06 AM.
CView *GetFirstView(CDocument *doc);
bool FetchURLString(const CString &urlStr, CString &htmlTextStr);
CString GetUrlFromUrlFile(const CString &fileName);
CString BoolStr(const BOOL value);
void UniqueStringList(CStringList &startList);
void SeparateByChar(CString &str, const _TCHAR sep, CStringList &list);
#ifdef _OLDSTUFF_
bool BoolValue(const CString &boolStr);
bool BoolValue(const _TCHAR * const buff);
#endif
bool BoolValueVT(const CString &boolStr);
bool BoolValueVT(const _TCHAR * const buff);
CString TriStateStr(const BOOL value);
int TriStateValue(const CString &triStateStr);
void StringToWordList(CStringList &outList, const CString &str, const bool keepBlankLinesFlag=false);
CString StringListToLines(CStringList &list, const bool extraReturn=true);
bool IntegerInStringList(CStringList &list, const int count);
void SortStringList(CStringList &list);
void RemoveSuccessiveEquals(CStringList &list);
void StringToPhraseList(CStringList &wordList, const CString &nodeName, const CString &strIn);
void StringToStringList(CStringList &lineList, const CString &linesStr, const CString &sepCharsStr);
void StringToStringListString(CStringList &lineList, const CString &linesStr, const CString &separatorStr);
int CALLBACK ListCompareFunc(const LPARAM Parm1, const LPARAM lParam2, const LPARAM lParamSort);
bool ParseLine(CStringList &parseList, const CString &lineStr, const CString &patternStr);
CString NthStringListItem(CStringList &list, const int nth);
void ReplaceNthStringListItem(CStringList &list, const int nth, const CString &newStr);
void InsertAfterNthStringListItem(CStringList &list, const int nth, const CString &newStr);
void TokensBySeparator(CStringList &tokenList, const CString &inStr, const _TCHAR sepChar, const bool noSeps=false);
void ExpandFromPath(CXTPTreeCtrl *m_TreeCtrl, const CString &pathStr, bool expand=true);
bool FindMenuPos(CMenu *pBaseMenu, const UINT myID, CMenu * & pMenu, int & mpos);
bool FindMenuPosFromString(CMenu *pBaseMenu, const CString &matchStr, int & mpos);
void StringToChar(_TCHAR *buff, const CString &strIn, const int len);
CString NormalizeURL(CString urlStr);

HTREEITEM CopyBranch(CTreeCtrl *treeCtrl, const HTREEITEM &htiBranch, const HTREEITEM &htiNewParent, const HTREEITEM &htiAfter=TVI_LAST);
HTREEITEM CopyItem(CTreeCtrl *treeCtrl, const HTREEITEM &hItem, const HTREEITEM &htiNewParent, const HTREEITEM &htiAfter=TVI_LAST);
HTREEITEM CopyBranch(CXTPTreeCtrl *treeCtrl, const HTREEITEM &htiBranch, const HTREEITEM &htiNewParent, const HTREEITEM &htiAfter=TVI_LAST);
HTREEITEM CopyItem(CXTPTreeCtrl *treeCtrl, const HTREEITEM &hItem, const HTREEITEM &htiNewParent, const HTREEITEM &htiAfter=TVI_LAST);
int MoveTreeNodeUp(CTreeCtrl *treeCtrl, const HTREEITEM &selected, const int imageBoundary=-1, const int imageSelBoundary=-1);
int MoveTreeNodeUp(CXTPTreeCtrl *treeCtrl, const HTREEITEM &selected, const int imageBoundary=-1, const int imageSelBoundary=-1);
int MoveTreeNodeDown(CTreeCtrl *treeCtrl, const HTREEITEM &selected, const int imageBoundary=-1, const int imageSelBoundary=-1);
int MoveTreeNodeDown(CXTPTreeCtrl *treeCtrl, const HTREEITEM &selected, const int imageBoundary=-1, const int imageSelBoundary=-1);
void TreeViewSelectItem(CTreeView* pTreeView, CPoint point); // SRP 010207
int TreeCtrlItemPosition(CXTPTreeCtrl* m_treeCtrl, const HTREEITEM &child);
CString StateString(CTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state);
CString StateString(CXTPTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state);
void StateString(CTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state, CString &allStr, int &count);
void StateString(CXTPTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state, CString &allStr, int &count);
void StateFromString(CTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state, const CString &stateStr);
void StateFromString(CXTPTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state, const CString &stateStr);
void StateFromString(CTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state, CStringList &numberList, int &count);
void StateFromString(CXTPTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state, CStringList &numberList, int &count);
HTREEITEM FindLastConceptSibling(CTreeCtrl *treeCtrl, const HTREEITEM &startingItem);
HTREEITEM FindLastConceptSibling(CXTPTreeCtrl *treeCtrl, const HTREEITEM &startingItem);
int TreeItemNumberFromItem(CXTPTreeCtrl *treeCtrl, const HTREEITEM &itemIn);
BOOL IsItemChecked(CXTPTreeCtrl *treeCtrl, const HTREEITEM &hItem);
void DeleteTreeCtrlChildren(CTreeCtrl *treeCtrl, const HTREEITEM &item, const int image=-1, const int imageSel=-1, const bool oneLevel=false);
void DeleteTreeCtrlChildren(CXTPTreeCtrl *treeCtrl, const HTREEITEM &item, const int image=-1, const int imageSel=-1, const bool oneLevel=false);
HTREEITEM NextTreeCtrlItem(CXTPTreeCtrl *treeCtrl, const HTREEITEM &currentItem, const bool backward, const int image, const int imageSel);
int GetTreeCtrlChildrenCount(CTreeCtrl *treeCtrl, const HTREEITEM &item);
int GetTreeCtrlChildrenCount(CXTPTreeCtrl *treeCtrl, const HTREEITEM &item);
void TreeCtrlExpandAll(CXTPTreeCtrl *treeCtrl, const HTREEITEM &item, const UINT nCode=TVE_EXPAND);
void TreeCtrlExpandAll(CTreeCtrl *treeCtrl, const HTREEITEM &item, const UINT nCode=TVE_EXPAND);
void TreeCtrlExpandOneLevel(CTreeCtrl *treeCtrl, const HTREEITEM &item, const UINT nCode=TVE_EXPAND);
void TreeCtrlExpandOneLevel(CXTPTreeCtrl *treeCtrl, const HTREEITEM &item, const UINT nCode=TVE_EXPAND);
HTREEITEM TreeCtrlFirstSibling(CTreeCtrl *treeCtrl, const HTREEITEM &item);
HTREEITEM TreeCtrlFirstSibling(CXTPTreeCtrl *treeCtrl, const HTREEITEM &item);
HTREEITEM TreeCtrlLastSibling(CTreeCtrl *treeCtrl, const HTREEITEM &item);
HTREEITEM TreeCtrlLastSibling(CXTPTreeCtrl *treeCtrl, const HTREEITEM &item);
HTREEITEM TreeItemFromName(CXTPTreeCtrl *treeCtrl, const HTREEITEM &item, const CString &itemNameStr);

int FileLineNumberFromIndex(const CString &fileStr, const long charIndex);
void ZipFiles(HZIP zf, const CString &findFileStr, const CString &findWildStr, const CString &targetFileStr, const CString &pathStr, bool directoriesFlag=false);
void UnarchiveFiles(const CString &zipFileStr, const CString &localPathStr);
CString ArchiveAutoGenName();
bool ZipFile(HZIP hz, const CString &soureFileStr, const CString &zipFileStr);
bool UnzipFiles(const CString &zipFileStr, const CString &localPathStr);
bool UnzipFile(HZIP hz, int index, const CString &targetFileStr, const CString &localPathStr);
CString ArchiveFiles(const CString &pathStr, const CString &archiveTypeStr, const CString &findWildStr);
unsigned long filetime(TCHAR *f, unsigned long * dt);
bool IsDrive(const CString &pathStr);
bool IsDigit(const CString &numStr);
bool IsAlpha(const CString &alphaStr);
void PopupFromStringList(CWnd *wnd, const CPoint &point, CStringList &list, const int startID);
void TreeCtrlChildrenStringList(CStringList &childrenList, CXTPTreeCtrl *treeCtrl, const HTREEITEM &item, const int image, const int imageSel);
void PeekHere(const int times);
void PeekHere();  // CANNOT COMBINE! AMBIGUOUS PROBLEM!
CString FullPath(const CString &relPathStr);
void TrimString(CString &strIn);
bool AllUpper(const CString &str);
void MergeStringLists(CStringList &stringList, CStringList &appendThisList);
void SwitchInt(int &firstInt, int &secondInt);
CString PrettyNumber(const int number);
CString PrettySize(const int number);
void AdjustFileStartEnd(long &start, long &end, const CString &textPathStr);
CString SeparateByCaps(const CString &inStr);
int StringToInteger(const CString &inStr);
int AskUser(const CString &msgStr, const UINT waveID, const int numButtons, const int defaultChoice,
			const CString &butt1Str, const CString &butt2Str, const CString &butt3Str);
bool PlayWave(const UINT waveID);
int TreeCtrlLongestItem(CTreeCtrl *treeCtrl, const HTREEITEM &item=NULL);
int TreeCtrlLongestItem(CXTPTreeCtrl *treeCtrl, const HTREEITEM &item=NULL);
int TreeCtrlVisibleCount(CTreeCtrl *treeCtrl, int &depth, const HTREEITEM &item=NULL);
int TreeCtrlVisibleCount(CXTPTreeCtrl *treeCtrl, int &depth, const HTREEITEM &item=NULL);
void SetWindowSize(CView *view, CTreeCtrl *treeCtrl, const bool dontMove=false);
void SetWindowSize(CView *view, CXTPTreeCtrl *treeCtrl, const bool dontMove=false);
bool TreeCtrlIsToplevelItem(CXTPTreeCtrl *treeCtrl, const HTREEITEM &item);
bool TreeItemNumberFromItemRecurse(CXTPTreeCtrl *treeCtrl, const HTREEITEM &item, const HTREEITEM &itemIn, int &count);
void TreeItemMove(CXTPTreeCtrl *treeCtrl, const HTREEITEM &fromItem, const HTREEITEM &toItem, const bool alphabeticFlag=true);
CString StringWrap(const CString &str, const int wrapSize);
void ParseLogLine(const CString &fileTextStr, CString &lineStr, const int line,
				  long &startSel, long &endSel, int &passNum, int &lineNum, int &indent,
				  CString &conceptStr, CString &semStr, CString &semVars, CString &rangeStr,
				  bool &firedFlag, bool &builtFlag, bool bQuick=true, BOOL bPrettif=true);
void PasteTextToClipboard(const CString &textStr);
void GetClipboardText(CString& strClipboardText);
HTREEITEM AlphabeticalInsertPoint(CXTPTreeCtrl *treeCtrl, const HTREEITEM &parentItem, const CString &itemNameStr);
void WindowBell(DWORD tone=475);
bool IsDirectoryEmpty(const CString &pathStr);
void CheckFileAssociation(void);
CString TemplateDirectory(CString fileStr=_T(""));
BOOL UTLSelectFolder(HWND hWnd, LPCTSTR prompt, CString &dirSpec, BOOL startAtMyComputer);
void UTLReadRect (CRect& cR, LPCTSTR lpString);
void UTL_GetDesktopRect (CRect &cR);
void UTLGetFileVersion(CString& strFileVersionInResourceFile);
bool IsInList(_TCHAR *list[], CString wordStr, CString endStr, BOOL bCaseSensitive=true);

#define ISKEYPRESSED(vk) (HIBYTE(GetKeyState(vk)))
#define WMU_BRINGWINDOWTOTOP			(WM_USER + 1201)
#define WMU_DESTROY_ATTRIBUTEEDITOR		(WM_USER + 1202)
#define WMU_DESTROY_BROWSER_DICT_LOOKUP	(WM_USER + 1203)
#define WMU_SETACTIVEVIEW				(WM_USER + 1204)
#define MSG_GETLOADEDANALYZER 1
extern LPCTSTR lpMainFrameClassName;
LPCTSTR UTLRegisterSimilarClass(LPCTSTR lpszNewClassName, LPCTSTR lpszOldClassName, UINT nIDResource);
BOOL UTLFindRunningVTInst (LPCTSTR lpAnalyzerFile);
int CharCount(CString str, _TCHAR c);
CString RepeatStr(CString str, int count);
bool GetTextString(_TCHAR *&ptr,CString &str, _TCHAR *&pbuf);	// 12/01/06 AM. // 09/09/08 AM.
void StringsToColumns(CStringArray &arr, CString &semVars,int len);	// 09/09/08 AM.

void StartWaitCursor();
void StopWaitCursor();



