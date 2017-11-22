/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#include <strstream>           // For cbuf().                  // 05/11/02 AM.
#ifdef QDBM_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>											// Upgrade	// 01/24/01 AM.
#include <fstream>											// Upgrade	// 01/24/01 AM.
using namespace std;											// Upgrade	// 01/24/01 AM.
#include <string.h>
#include "qkbm/libqkbm.h"
#include "qkbm/defs.h"
#include "qkbm/Aqkbm.h"
#include "qconsh/libqconsh.h"
#include "qconsh/Aconsh.h"
#include "qconsh/cg.h"
#else
#include <iostream>			// 08/18/08 AM.
#include <fstream>			// 08/18/08 AM.
using namespace std;			// 08/18/08 AM.
#include <string.h>
#include "consh/libconsh.h"
#include "consh/Aconsh.h"
#include "consh/cg.h"
#endif
#include "lite/lite.h"
#include "lite/global.h"
#include "lite/Gui.h"
#include "lite/nlp.h"
#include "lite/rug.h"
#include "lite/Agram.h"
#include "prim/libprim.h"                                      // 08/14/02 AM.
#include "prim/list_s.h"                                       // 08/14/02 AM.
#include "prim/list.h"                                         // 08/14/02 AM.
#include "lite/slist.h"														// 01/06/03 AM.
#include "lite/vtrun.h"                                        // 08/29/02 AM.

extern VTRun *vtrun;	// Global for VisualText RUNTIME SYSTEM.  // 08/29/02 AM.

// From LITE/STR.H.	// 12/16/01 AM.
LITE_API bool alphabetic(_TCHAR ch);                             // 09/22/99 AM.
LITE_API bool is_upper(_TCHAR ch);                               // 12/16/01 AM.
LITE_API bool is_lower(_TCHAR ch);                               // 12/16/01 AM.
LITE_API _TCHAR to_upper(_TCHAR ch);                               // 12/16/01 AM.
LITE_API _TCHAR to_lower(_TCHAR ch);                               // 12/16/01 AM.


#define DRAG_X_ADJUST 15
#define DRAG_Y_ADJUST 40
#define DRAG_DELAY 80
#define CONCEPT_MENU_INDENT 3

#define TEXTVIEW_ID_START 200000
#define TEXTVIEW_ID_END 201000
//#define OUTPUT_ID_START 210000
//#define OUTPUT_ID_END 211000
#define SAMPLE_ID_START 220000
#define SAMPLE_ID_END 221000

bool LoadKB(bool compileFlag=false);
CString ConceptName(CONCEPT *concept);
int ConceptCount(CONCEPT *concept, bool firstSiblingFlag=false);
BOOL AutoSaveKB(bool askConfirm=true);
void SaveKB(bool askConfirm=false);
CONCEPT *ConceptFirstSibling(CONCEPT *concept);
CONCEPT *NumberToConcept(CONCEPT *concept, int number, int &count);
void ConceptHierarchyToStringList(CStringList &list, CONCEPT *concept, int level, bool includeSiblings=true);
void AddConceptHierarchyToStringList(CStringList &menuList, CONCEPT *concept, bool siblingsFlag=false);
void ConceptAttributeAdd(CONCEPT *concept, CString attrStr, CONCEPT *valueConcept=NULL);
void ConceptAttributeRename(CONCEPT *concept, CString attrStr, CString newAttr);
void ConceptAttributeUpdate(CONCEPT *concept, CString attrStr, int newVal);
void ConceptAttributeDialogUpdate(CONCEPT *concept, CString attrStr, CString valStr, CString newValStr);
void ConceptAttributeUpdate(CONCEPT *concept, CString attrStr, int oldVal, int newVal);
void ConceptAttributeUpdate(CONCEPT *concept, CString attrStr, CONCEPT *newVal);
void ConceptAttributeUpdate(CONCEPT *concept, CString attrStr, CONCEPT *oldVal, CONCEPT *newVal);
void ConceptAttributeUpdate(CONCEPT *concept, CString attrStr, CString valStr);
void ConceptAttributeUpdate(CONCEPT *concept, CString attrStr, CString oldValStr, CString newValStr);
//void ConceptAttributeUpdate(CONCEPT *concept, ATTR *attr, bool changeValue, CString valStr);
int PassFromConcept(CONCEPT *concept);
void RemoveAttribute(CONCEPT *concept, CString attrStr);
void RemoveAllAttributes(CONCEPT *concept);
bool IsPathSampleFile(CString pathStr);
bool FileHasSamples(CString pathStr);
void FindConcept(CStringList &conceptList, CONCEPT *concept, CString findStr, CString pathStr, bool noCase=true);
CONCEPT *ConceptFromPath(CString conceptPathStr, bool colonFlag=false);
CString ConceptPathStr(CONCEPT *concept, bool colonFlag=false);
void CopyConceptAttributes(CONCEPT *fromConcept, CONCEPT *toConcept);
CONCEPT *WordLookup(CString wordStr);
bool ConceptHasSamples(CONCEPT *concept);
CONCEPT *NthConcept(CONCEPT *parentConcept, int nth);
int ConceptNumber(CONCEPT *concept);
CString KBGetValueString(CONCEPT *concept, CString attrStr);
long KBGetValueLong(CONCEPT *concept, CString attrStr);
bool KBGetValueBool(CONCEPT *concept, CString attrStr);
void KBFindValue(CONCEPT *concept, CString attrStr, CString &valStr);
void KBFindValue(CONCEPT *concept, CString attrStr, long &valInt);
void KBFindValue(CONCEPT *concept, CString attrStr, bool &boolValue);
void KBReplaceValue(CONCEPT *concept, CString attrStr, const _TCHAR *valStr, bool setDirtyFlag=true);
void KBReplaceValue(CONCEPT *concept, CString attrStr, int intVal, bool setDirtyFlag=true);
void KBReplaceValue(CONCEPT *concept, CString attrStr, bool boolValue, bool setDirtyFlag=true);
CONCEPT *KBInsertConceptAlphabetically(CONCEPT *parentConcept, CString newConceptNameStr, bool setDirtyFlag=true);
void KBInsertConceptAlphabetically(CONCEPT *parentConcept, CONCEPT *newConcept, bool setDirtyFlag=true);
bool KBPassNumberRecursive(CONCEPT *rootPassConcept, CONCEPT *concept, int &number);
int KBCalculatePassNumber(CONCEPT *rootPassConcept, CONCEPT *concept);
int KBPassNumber(CONCEPT *concept);
bool KBIsSystemAttribute(CString attrNameStr);
bool KBCompile(bool warnFlag=true);
void PathStringToWordList(CStringList &wordList, CString strIn);
BOOL IsConceptFirstChild(CONCEPT *child, CONCEPT *parent);

void logmsg(_TCHAR*);	// 10/29/02 AM.
void logcounts();		// 07/18/03 AM.