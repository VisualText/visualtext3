/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#include "stdafx.h"
#include <stdlib.h>						// 05/03/99 AM.

#ifdef UNICODE
#include <sstream>		// 02/20/2005 SRP
#else
#include <strstream>		// For cbuf() output.				// 05/11/02 AM.
#endif

#include "kb.h"
#include "Utils.h"
#include "MainFrm.h"
#include "TreeViewAnalyzer.h"

extern NLP *nlp;	// 07/18/03 AM.

// A global for VisualText RUNTIME SYSTEM.                      // 08/29/02 AM.
VTRun *vtrun = 0;                                               // 08/29/02 AM.

CG *cg = NULL;
bool kbDirty = false;


// GLOBAL LOG FILE.	// 07/24/03 AM.
_TCHAR *VTLOGFILE = _T("C:\\TEMP\\VisualText.log");

extern CVisualTextApp theApp;
extern bool appBoolLoadCompiledKB;

bool LoadKB(bool compileFlag)
{
	logmsg(_T("Enter LoadKB:"));
	bool statusOK = true;

	_TCHAR buff[1001];
	StringToChar(buff,AppPath(),1000);

	if (cg)	// 07/24/03 AM.
		{
		logmsg(_T("  Before deleteCG"));
//		delete cg;
		vtrun->deleteCG(cg);	// 07/18/03 AM.
		logmsg(_T("  After deleteCG"));
		}

	if (nlp)	// 07/18/03 AM.
		{
		logmsg(_T("  Nlp nonempty."));
		nlp->setCG(0);	// 07/18/03 AM.
		logmsg(_T("  Zero out nlp's CG."));
		}

	// Hand the List Manager to the KB.                   // 08/14/02 AM.
//	cg = new CG(buff,compileFlag,(ALIST *)vtrun->alist_); // 08/29/02 AM.
	logmsg(_T("  Making CG"));	// 07/24/03 AM.
	cg = vtrun->makeCG(buff,compileFlag,nlp);					// 07/18/03 AM.
	logmsg(_T("  Made CG"));	// 07/24/03 AM.

//	if (!cg->readKB()) {
	if (!cg) {	// 07/18/03 AM.
		//	vtrun->deleteCG(cg);	// 07/18/03 AM.
		//		cg = 0;
		logmsg(_T("Couldn't load KB"));
		AfxMessageBox(_T("Couldn't load KB"));
		statusOK = false;
	}
	kbDirty = false;

	logmsg(_T("Leave LoadKB:"));	// 07/24/03 AM.
	return statusOK;
}

CString ConceptName(CONCEPT *concept) 
{
	CString conceptNameStr = _T("");
	_TCHAR buff[1001];
	cg->conceptName(concept,buff);
	conceptNameStr = buff;
	return conceptNameStr;
}

int ConceptCount(CONCEPT *concept, bool firstSiblingFlag) 
{
	if (firstSiblingFlag)
		concept = ConceptFirstSibling(concept);

	int count = 0;

	while (concept) {
		count++;
		concept = cg->Next(concept);
	}
	return count;
}

BOOL AutoSaveKB(bool askConfirm)
{
	CString valueStr = theApp.GetProfileString(_T("PREFERENCES"),_T("AUTOSAVEKBLOGOUT"),_T("false"));
	bool autoSaveFlag = BoolValueVT(valueStr);
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	if (autoSaveFlag) {
		if (!kbDirty && askConfirm &&
			IDNO == AfxMessageBox(_T("The Knowledge Base has not been changed.\n\n\tAuto-save anyway?"), MB_YESNO) ) {
			return false;
		}
		StartWaitCursor();
		cg->writeKB();
		kbDirty = false;
		StopWaitCursor();
		CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
		wnd->AddDebugLine(_T("Auto-Saved KB"),_T(""));
		return true;
	}
	return false;
}

void SaveKB(bool askConfirm)
{
	CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
	if (wnd->AnalyzerCreated()) {
		CString questionStr = _T("Do you want to save the Knowledge Base?");
		if (
//			kbDirty &&	// VisualText often wrong about kb status. Force save. // 06/21/03 AM.
// Also, need to get dirty state properly after running an analyzer.
			// Default for saving kb is NO.	// 09/02/08 AM.
			(!askConfirm || AfxMessageBox(questionStr, MB_YESNO|MB_DEFBUTTON2) == IDYES)) {
			StartWaitCursor();
			cg->writeKB();
			kbDirty = false;
			StopWaitCursor();
			CMainFrame *wnd = (CMainFrame *)AfxGetMainWnd();
			wnd->AddDebugLine(_T("Saved KB"),_T(""));
		}
		Sleep(25);	// Trying for Patrice Mellot bug.	// 08/14/03 AM.
	}
}

CONCEPT *ConceptFirstSibling(CONCEPT *concept)
{
	CONCEPT *first = concept;
	while (first) {
		if (!cg->Prev(first))
			break;
		first = cg->Prev(first);
	}
	return first;
}

CONCEPT *NumberToConcept(CONCEPT *concept, int number, int &count)
{
	if (!concept)
		return NULL;

	CONCEPT *sibling = concept;
	CONCEPT *child;
	CONCEPT *returnConcept;
	CString conceptStr;
	_TCHAR buff[1001];
	count++;

	while (sibling) {
		cg->conceptName(sibling,buff);
		if (count == number)
			return sibling;

		if ((child = cg->Down(sibling))) {
			returnConcept = NumberToConcept(child,number,count);
			if (returnConcept)
				return returnConcept;
		}
		sibling = cg->Next(sibling);
		if (sibling)
			count++;
	}

	return NULL;
}

void ConceptHierarchyToStringList(CStringList &list, CONCEPT *concept, int level,
								  bool includeSiblings)
{
	if (!concept)
		return;

	CONCEPT *sibling = concept;
	if (includeSiblings)
		sibling = ConceptFirstSibling(concept);
	CONCEPT *child;
	CString conceptStr;
	CString typeStr;

	while (sibling) {
		conceptStr = IndentStr(level*CONCEPT_MENU_INDENT,0) + ConceptName(sibling);
		typeStr = KBGetValueString(sibling,_T("type"));
		if (typeStr == _T("folder"))
			conceptStr = _T(">>>") + conceptStr;
		list.AddTail(conceptStr);
		if ((child = cg->Down(sibling)))
			ConceptHierarchyToStringList(list,child,level+1);
		sibling = cg->Next(sibling);
		if (!includeSiblings)
			break;
	}
}

void AddConceptHierarchyToStringList(CStringList &menuList, CONCEPT *concept, bool siblingsFlag)
{
	// ADD SELECTED CONCEPT
	CString conceptStr = ConceptName(concept);
	menuList.AddTail(conceptStr);

	// ADD ITS CHILDREN CONCEPTS
	CONCEPT *childConcept = cg->Down(concept);
	ConceptHierarchyToStringList(menuList,childConcept,1,siblingsFlag);
}

void ConceptAttributeAdd(CONCEPT *concept, CString attrStr, CONCEPT *valueConcept)
{
	_TCHAR attrBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	if (valueConcept)
		cg->addVal(concept,attrBuff,valueConcept);
	else
		cg->addAttr(concept,attrBuff);
	kbDirty = true;
}

void ConceptAttributeRename(CONCEPT *concept, CString attrStr, CString newAttr)
{
	_TCHAR attrBuff[1001];
	_TCHAR newAttrBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	StringToChar(newAttrBuff,newAttr,1000);
	cg->renameAttr(concept,attrBuff,newAttrBuff);
	kbDirty = true;
}

void ConceptAttributeUpdate(CONCEPT *concept, CString attrStr, int newVal)
{
	_TCHAR attrBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	long oldVal;

	if (cg->findVal(concept,attrBuff,oldVal))
		cg->replaceVal(concept,attrBuff,oldVal,newVal);
	else
		cg->addVal(concept,attrBuff,(long)newVal);
	kbDirty = true;
}

void ConceptAttributeUpdate(CONCEPT *concept, CString attrStr, CString valStr)
{
	ConceptAttributeUpdate(concept,attrStr,_T(""),valStr);
	return;
	if (IsDigit(valStr)) {
		int valInt = _ttoi((const _TCHAR *)valStr);
		ConceptAttributeUpdate(concept,attrStr,valInt);
	}
	else {
		_TCHAR attrBuff[1001];
		_TCHAR valBuff[1001];
		_TCHAR oldValBuff[1001];
		StringToChar(attrBuff,attrStr,1000);
		StringToChar(valBuff,valStr,1000);

		if (cg->findVal(concept,attrBuff,oldValBuff))
			cg->replaceVal(concept,attrBuff,oldValBuff,valBuff);
		else
			cg->addVal(concept,attrBuff,valBuff);
	}
}

void ConceptAttributeUpdate(CONCEPT *concept, CString attrStr, int oldVal, int newVal)
{
	_TCHAR attrBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	cg->replaceVal(concept,attrBuff,(long)oldVal,(long)newVal);
	kbDirty = true;
}

void ConceptAttributeUpdate(CONCEPT *concept, CString attrStr, CONCEPT *newVal)
{
	_TCHAR attrBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	cg->replaceVal(concept,attrBuff,newVal);
	kbDirty = true;
}

void ConceptAttributeUpdate(CONCEPT *concept, CString attrStr, CONCEPT *oldVal, CONCEPT *newVal)
{
	_TCHAR attrBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	cg->replaceVal(concept,attrBuff,oldVal,newVal);
	kbDirty = true;
}

// This function sucks!	// 06/19/02 AM.
void ConceptAttributeUpdate(CONCEPT *concept, CString attrStr, CString oldValStr, CString newValStr)
{
	_TCHAR attrBuff[1001];
	_TCHAR oldValBuff[1001];
	_TCHAR newValBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	StringToChar(oldValBuff,oldValStr,1000);
	StringToChar(newValBuff,newValStr,1000);

	int oldInt = _ttoi((const _TCHAR *)oldValStr);
	int newInt = _ttoi((const _TCHAR *)newValStr);
	CString clippedNewValStr = newValStr;

	if (newValStr.IsEmpty()) { // 11/25/00 AM.{
		cg->rmAttr(concept,attrBuff); // 11/25/00 AM.
		kbDirty = true; // 11/25/00 AM.
		return; // 11/25/00 AM.
	}

	// SRP 010607 Restore PD, fix Mantis bug 69
	// trim flanking quotation marks from an input value.
	// A solitary quote will not be trimmed, as it may be
	// an intended input value. PD 01/30/2001
	if (newValStr.GetAt(0) == '\"' &&
		newValStr.GetAt(newValStr.GetLength()-1) == '\"' &&
		newValStr.GetLength() != 1
		) {
		ClipLeft(clippedNewValStr,1);
		ClipRight(clippedNewValStr,1);
		StringToChar(newValBuff,clippedNewValStr,1000);
	}

	if (oldValStr == _T("")) {
		ATTR *attrs = cg->findAttrs(concept);
		if (cg->attrVals(attrs)) {
			if (!IsDigit(newValStr)) {
				cg->replaceVal(concept,attrBuff,newValBuff);
				kbDirty = true;
			}
			else if (IsDigit(oldValStr) && IsDigit(newValStr)) {
				cg->replaceVal(concept,attrBuff,(long)newInt);
				kbDirty = true;
			}
		}
		else {
			if (!IsDigit(newValStr)) {
				cg->addVal(concept,attrBuff,newValBuff);
				kbDirty = true;
			}
			else if (IsDigit(oldValStr) && IsDigit(newValStr)) {
				cg->addVal(concept,attrBuff,(long)newInt);
				kbDirty = true;
			}
		}
	}
	else if (!IsDigit(oldValStr) && !IsDigit(newValStr)) {
		cg->replaceVal(concept,attrBuff,oldValBuff,newValBuff);
		kbDirty = true;
	}
	else if (IsDigit(oldValStr) && IsDigit(newValStr)) {
		cg->replaceVal(concept,attrBuff,oldInt,(long)newInt);
		kbDirty = true;
	}
	else if (IsDigit(oldValStr) && !IsDigit(newValStr)) {
		cg->rmAttrval(concept,attrBuff,(long)oldInt);
		cg->addVal(concept,attrBuff,newValBuff);
		kbDirty = true;
	}
	else if (!IsDigit(oldValStr) && IsDigit(newValStr)) {
		cg->rmAttrval(concept,attrBuff,oldValBuff);
		cg->addVal(concept,attrBuff,(long)newInt);
		kbDirty = true;
	}
}

int PassFromConcept(CONCEPT *concept)
{
	return Agram::smartPassnum(concept,cg);
}

void RemoveAttribute(CONCEPT *concept, CString attrStr)
{
	_TCHAR attrBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	cg->rmAttr(concept,attrBuff);
	kbDirty = true;
}

void RemoveAllAttributes(CONCEPT *concept)
{
	ATTR *attrs = cg->findAttrs(concept);
	ATTR *attr;
	_TCHAR buff[1001];

	CString attrStr;

	while (attrs) {
		attr = cg->popAttr(attrs);
		cg->attrName(attr,buff,1000);
		attrStr = buff;
		if (KBIsSystemAttribute(attrStr))
			continue;

		cg->rmAttr(concept,attr);
		kbDirty = true;
	}
}

bool IsPathSampleFile(CString pathStr)
{
	if (pathStr.IsEmpty())
		return false;
	_TCHAR buff[1001];
	StringToChar(buff,RelativizePath(pathStr),1000);
	_TCHAR gramWord[5];
	_tcscpy_s(gramWord,_T("gram"));
	CONCEPT *gram = cg->findConcept(cg->findRoot(),gramWord);
	return cg->attrExists(gram,_T("sampfile"),buff);
}

bool ConceptHasSamples(CONCEPT *concept)
{
	if (!concept)
		return false;

	if (cg->findPhrase(concept))
		return true;

	CONCEPT *child = cg->Down(concept);
	CString conceptStr;

	_TCHAR buff[1001];
	while (child) {
		cg->conceptName(child,buff);  // FOR DEBUGGING
		if (cg->findPhrase(child))
			return true;
		if (ConceptHasSamples(cg->Down(child)))
			return true;
		child = cg->Next(child);
	}
	return false;
}

CONCEPT *NthConcept(CONCEPT *concept, int nth)
{
	CONCEPT *c = ConceptFirstSibling(concept);
	int count = 1;

	while (c) {
		if (count++ == nth)
			break;
		c = cg->Next(c);
	}
	return c;
}

BOOL IsConceptFirstChild(CONCEPT *child, CONCEPT *parent)
{
	return cg->Down(cg->Up(child)) == parent;
}

int ConceptNumber(CONCEPT *concept)
{
	CONCEPT *c = ConceptFirstSibling(concept);
	int count = 1;
	CString debugStr;

	while (c) {
		debugStr = ConceptName(c);
		if (c == concept)
			break;
		c = cg->Next(c);
		count++;
	}
	return count;
}

bool FileHasSamples(CString pathStr)
{
	if (pathStr.IsEmpty())
		return false;
	_TCHAR buff[1001];
	StringToChar(buff,RelativizePath(pathStr),1000);
	_TCHAR gramWord[5];
	_tcscpy_s(gramWord,_T("gram"));
	CONCEPT *gram = cg->findConcept(cg->findRoot(),gramWord);
	return cg->nodeAttrexists(gram,_T("text"),buff);
}

void FindConcept(CStringList &conceptList, CONCEPT *concept,
				 CString findStr, CString conceptPathStr, bool noCase)
{
	CONCEPT *child;
	CString conceptStr;
	CString conStr;
	CString fStr;

	while (concept) {
		conceptStr = ConceptName(concept);
		if (conceptPathStr != _T(""))
			conceptStr = conceptPathStr + _T("/") + conceptStr;
		conStr = conceptStr;
		fStr = findStr;
		if (noCase) {
			conStr.MakeLower();
			fStr.MakeLower();
		}
		if (conStr.Find(fStr) >= 0)
			conceptList.AddTail(conceptStr);
		if ((child = cg->Down(concept)))
			FindConcept(conceptList,child,findStr,conceptStr,noCase);
		concept = cg->Next(concept);
	}
}

CONCEPT *ConceptFromPath(CString conceptPathStr, bool colonFlag)
{
	CONCEPT *concept = cg->findRoot();
	_TCHAR conceptBuff[1001];

	CStringList conceptList;
	if (colonFlag) {
		CString newPathStr = conceptPathStr;
		newPathStr.Replace(_T(":"),_T("\" \""));
		conceptPathStr.Format(_T("\"%s\""),newPathStr);
	}
	PathStringToWordList(conceptList,conceptPathStr);
	POSITION pos = conceptList.GetHeadPosition();	// 06/19/02 AM.
	CString first = conceptList.GetAt(pos);	// 06/19/02 AM.
	if (first != _T("\"concept\"")	// 06/19/02 AM.
	 && first != _T("concept"))	// 06/19/02 AM.
		return 0;	// 06/19/02 AM.
	conceptList.RemoveHead();
	pos = conceptList.GetHeadPosition();
	CString conceptStr = _T("");

	while (concept && pos && (conceptStr = conceptList.GetNext(pos))) {
		StringToChar(conceptBuff,conceptStr,1000);
		concept = cg->findConcept(concept,conceptBuff);
	}

	return concept;
}

CString ConceptPathStr(CONCEPT *concept, bool colonFlag)
{
	CString conceptPathStr = _T("");
	_TCHAR conceptBuff[1001];

	cg->conceptPath(concept,conceptBuff,1000);

	conceptPathStr = conceptBuff;
	if (colonFlag) {
		conceptPathStr.Replace(_T("\" \""),_T(":"));
		conceptPathStr.Replace(_T("\""),_T(""));
	}
	return conceptPathStr;
}

void PathStringToWordList(CStringList &wordList, CString strIn)
{
	CString word = _T("");
	_TCHAR c = '\0';
	int length = strIn.GetLength();
	bool collectingWord = false;
	bool waitingForQuote = true;

	for (int i=0; i<length; i++) {
		c = strIn.GetAt(i);
		if (c == '"') {
			if (waitingForQuote) {
				waitingForQuote = false;
				collectingWord = true;
			}
			else if (collectingWord) {
				if (word != _T("")) {
					wordList.AddTail(word);
					word = _T("");
				}
				collectingWord = false;
				waitingForQuote = true;
			}
			else
				collectingWord = true;
		}
		else if (!waitingForQuote)
			word += c;
	}
	if (word != _T(""))
		wordList.AddTail(word);
}

void CopyConceptAttributes(CONCEPT *fromConcept, CONCEPT *toConcept)
{
	ATTR *attrs = cg->findAttrs(fromConcept);
	ATTR *attr;
	VAL *vals;
	_TCHAR val[1001];
	_TCHAR buff[1001];

	CString attrStr;
	CString valueStr;

	while (attrs) {
		attr = cg->popAttr(attrs);
		cg->attrName(attr,buff,1000);
		attrStr = buff;
		if (KBIsSystemAttribute(attrStr))
			continue;

		vals = cg->attrVals(attr);

		while (vals) {
			val[0] = '\0';
			cg->popSval(vals,val);
			valueStr = val;

			ConceptAttributeUpdate(toConcept,attrStr,valueStr);
		}
	}
}

CONCEPT *WordLookup(CString wordStr)
{
	_TCHAR buff[1001];
	StringToChar(buff,wordStr,1000);
	CONCEPT *dictCon = cg->getWordConcept(buff);
	return dictCon;
}

void KBFindValue(CONCEPT *concept, CString attrStr, CString &valStr)
{
	_TCHAR attrBuff[1001];
	_TCHAR valBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	cg->findVal(concept,attrBuff,valBuff);
	valStr = valBuff;
}

void KBFindValue(CONCEPT *concept, CString attrStr, long &valInt)
{
	_TCHAR attrBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	cg->findVal(concept,attrBuff,valInt);
}

void KBFindValue(CONCEPT *concept, CString attrStr, bool &boolValue)
{
	_TCHAR attrBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	long longValue;
	cg->findVal(concept,attrBuff,longValue);
	boolValue = longValue ? true : false;
}

CString KBGetValueString(CONCEPT *concept, CString attrStr)
{
	_TCHAR attrBuff[1001];
	_TCHAR valBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	cg->findVal(concept,attrBuff,valBuff);
	CString valStr = valBuff;
	return valStr;
}

long KBGetValueLong(CONCEPT *concept, CString attrStr)
{
	_TCHAR attrBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	long valLong;
	cg->findVal(concept,attrBuff,valLong);
	return valLong;
}

bool KBGetValueBool(CONCEPT *concept, CString attrStr)
{
	_TCHAR attrBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	long longValue;
	cg->findVal(concept,attrBuff,longValue);
	return longValue ? true : false;
}

void KBReplaceValue(CONCEPT *concept, CString attrStr, const _TCHAR *valStr, bool setDirtyFlag)
{
	_TCHAR attrBuff[1001];
	_TCHAR valBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	StringToChar(valBuff,valStr,1000);
	cg->replaceVal(concept,attrBuff,valBuff);
	if (setDirtyFlag)
		kbDirty = true;
}

void KBReplaceValue(CONCEPT *concept, CString attrStr, int intVal, bool setDirtyFlag)
{
	_TCHAR attrBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	cg->replaceVal(concept,attrBuff,(long)intVal);
	if (setDirtyFlag)
		kbDirty = true;
}

void KBReplaceValue(CONCEPT *concept, CString attrStr, bool boolValue, bool setDirtyFlag)
{
	_TCHAR attrBuff[1001];
	StringToChar(attrBuff,attrStr,1000);
	cg->replaceVal(concept,attrBuff,boolValue ? (long)1 : (long)0);	// 03/19/03 AM.
	if (setDirtyFlag)
		kbDirty = true;
}

CONCEPT *KBInsertConceptAlphabetically(CONCEPT *parentConcept, CString newConceptNameStr, bool setDirtyFlag)
{
	if (!parentConcept || newConceptNameStr == _T(""))
		return NULL;

	CONCEPT *concept = cg->Down(parentConcept);
	CONCEPT *newConcept = NULL;
	CString conceptStr;
	int pos = 0;
	bool found = false;

	while (concept) {
		conceptStr = ConceptName(concept);
		pos++;
		if (newConceptNameStr.CompareNoCase(conceptStr) < 0) {
			found = true;
			break;
		}
		concept = cg->Next(concept);
	}

	if (!found) {
		pos = 0;
		found = true;
	}

	if (found) {
		_TCHAR buff[1001];
		StringToChar(buff,newConceptNameStr,1000);
		newConcept = cg->makeConcept(parentConcept,buff,pos);
		cg->addConcept(parentConcept,newConcept,pos);
		if (setDirtyFlag)
			kbDirty = true;
	}

	return newConcept;
}

void KBInsertConceptAlphabetically(CONCEPT *parentConcept, CONCEPT *newConcept, bool setDirtyFlag)
{
	CONCEPT *concept = cg->Down(parentConcept);
	CString newConceptNameStr = ConceptName(newConcept);
	CString conceptStr;
	int pos = 1;
	bool added = false;

	while (concept) {
		conceptStr = ConceptName(concept);
		if (newConceptNameStr.CompareNoCase(conceptStr) < 0) {
			cg->addConcept(parentConcept,newConcept,pos);
			added = true;
			break;
		}
		concept = cg->Next(concept);
		pos++;
	}

	if (!added)
		cg->addConcept(parentConcept,newConcept,0);
	else if (setDirtyFlag)
		kbDirty = true;
}

int KBPassNumber(CONCEPT *concept)
{
	return (int)KBGetValueLong(concept,_T("pass"));
}

bool KBPassNumberRecursive(CONCEPT *rootPassConcept, CONCEPT *concept, int &number)
{
	CONCEPT *c = rootPassConcept;
	CONCEPT *child = NULL;
	CString debugStr;
	int count = 0;
	bool found = false;
	ANAL_PASS_TYPE type;

	while (c) {
		type = (ANAL_PASS_TYPE)KBGetValueLong(c,_T("type"));
		debugStr = ConceptName(c);

		if (c == concept) {
			found = true;
			break;
		}
		else {
			child = cg->Down(c);
			if (child && KBPassNumberRecursive(child,concept,number)) {
				found = true;
				break;
			}
			else {
				c = cg->Next(c);
				if (type == ANAL_PASS_PAT || type == ANAL_PASS_REC || type == ANAL_PASS_SYSTEM)
					number++;
			}
		}
	}

	return found;
}

int KBCalculatePassNumber(CONCEPT *rootPassConcept, CONCEPT *concept)
{
	ANAL_PASS_TYPE type = (ANAL_PASS_TYPE)KBGetValueLong(concept,_T("type"));
	if (type == ANAL_PASS_FOLDER || type == ANAL_PASS_MAIN)
		return 0;

	CONCEPT *c = rootPassConcept;
	int passNumber = 1;
	if (!KBPassNumberRecursive(rootPassConcept,concept,passNumber))
		passNumber = 0;

	return passNumber;
}

bool KBIsSystemAttribute(CString attrNameStr)
{
	bool returnValue = false;
	if (attrNameStr == _T("atom") || attrNameStr == _T("phrase")|| attrNameStr == _T("var"))
		returnValue = true;
	return returnValue;
}

// Todo: Need a flag that says if compiled/interp KB is already loaded,
// which may be different from the Preferences flag.
bool KBCompile(bool warnFlag)
{
	StartWaitCursor();

	bool loadCompiledKB = appBoolLoadCompiledKB;

	// SAVE INTERP KB BEFORE COMPILING, TO KEEP BOTH IN SYNC.	// 04/27/01 AM.
	if (kbDirty) {
		cg->writeKB();
		kbDirty = false;
	}

#ifdef QDBM_
	// Copy from tmp area to qkbm area.									// 03/04/07 AM.
	if (!cg->saveKBMS())														// 03/04/07 AM.
		{
		AfxMessageBox(_T("Could not save KBMS."));					// 03/04/07 AM.
		StopWaitCursor();												// 03/04/07 AM.
		return false;															// 03/04/07 AM.
		}
#else
	if (!cg->genKB())	{			// 04/27/01 AM.
		if (warnFlag)				// 04/27/01 AM.
			AfxMessageBox(_T("Compile KB failed to generate code.")); // 04/27/01 AM.
		StopWaitCursor();	// 04/27/01 AM.
		return false;				// 04/27/01 AM.
	}


// Compile debug/release config library appropriately.
#ifdef _DEBUG
	if (!cg->compileKB(false))	{	// 04/27/01 AM.
#else
	if (!cg->compileKB(true))	{	// 04/27/01 AM.
#endif
		if (warnFlag)				// 04/27/01 AM.
			AfxMessageBox(_T("Compile KB failed to compile. You may need to add files to the Visual C++ KB project manually.")); // 04/27/01 AM.
		StopWaitCursor();	// 04/27/01 AM.
		return false;				// 04/27/01 AM.
	}
#endif

	StopWaitCursor();

	return true;					// 04/27/01 AM.
}

// This is not just KB, but logging for the GUI as a whole.	// 10/29/02 AM.
// Opening and closing file after each write, in case there's a crash.
void logmsg(_TCHAR *msg)
{
// Setting up logging.	// 10/29/02 AM.
_t_ofstream *fout = new _t_ofstream(
//	vtrun->logfile_,
	TCHAR2CA(VTLOGFILE),	// 07/24/03 AM.
	ios::app);
*fout << msg << endl;
delete fout;  fout = 0;
}

// Counts of objects.	// 07/18/03 AM.
void logcounts()			// 07/18/03 AM.
{
_t_ofstream *fout = new _t_ofstream(
//	vtrun->logfile_,
	TCHAR2CA(VTLOGFILE),	// 07/24/03 AM.
	ios::app);
object_counts(fout);
delete fout;  fout = 0;
}
