/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// ParseTreeItemInfo.h: interface for the CParseTreeItemInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYITEMINFO_H__CBAA6794_F21D_11D1_9F9F_84E5D3000000__INCLUDED_)
#define AFX_MYITEMINFO_H__CBAA6794_F21D_11D1_9F9F_84E5D3000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ItemInfo.h"

class CParseTreeItemInfo : public CItemInfo  
{
public:

	CParseTreeItemInfo(int iItem, HTREEITEM treeItem, CString conceptStr,
					   int startSel, int endSel, int passNum, int lineNum,
					   int level, CString strText, CString strSem) :
		CItemInfo(iItem),
		m_treeItem(treeItem),
		m_strConcept(conceptStr),
		m_intStart(startSel),
		m_intEnd(endSel),
		m_intPass(passNum),
		m_intLine(lineNum),
		m_intLevel(level),
 		m_strText(strText),
 		m_strSem(strSem),
		m_next(NULL)
		{};

	virtual ~CParseTreeItemInfo() {};

	CString GetConcept() {return m_strConcept;}
	int GetStart() {return m_intStart;}
	int GetEnd() {return m_intEnd;}
	int GetLevel() {return m_intLevel;}
	int GetPass() {return m_intPass;}
	int GetLine() {return m_intLine;}
	void SetNext(CParseTreeItemInfo *nextInfo) {m_next = nextInfo;}
	HTREEITEM GetTreeItem() {return m_treeItem;}
	CParseTreeItemInfo *GetNext() {return m_next;}
	CString GetText() {return m_strText;}
	CString GetSem() {return m_strSem;}

private:

	CParseTreeItemInfo();

 	CString m_strConcept;
 	int m_intStart;
	int m_intEnd;
	int m_intLevel;
	int m_intPass;
	int m_intLine;
	HTREEITEM m_treeItem;
	CParseTreeItemInfo *m_next;
 	CString m_strText;
 	CString m_strSem;

};

#endif // !defined(AFX_MYITEMINFO_H__CBAA6794_F21D_11D1_9F9F_84E5D3000000__INCLUDED_)
