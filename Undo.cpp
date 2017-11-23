/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// Undo.cpp : implementation file
//

#include "stdafx.h"
#include "undo.h"

CUndoItem::CUndoItem ()
{
	m_opRedo = NoOp;
	m_opUndo = NoOp;
	memset(&m_PRedo, 0, sizeof(m_PRedo));
	memset(&m_PUndo, 0, sizeof(m_PUndo));
}

CUndoItem::~CUndoItem()
{
	if (m_PUndo.pStr)
		delete m_PUndo.pStr;
	if (m_PRedo.pStr)
		delete m_PRedo.pStr;
}

CUndoItem::CUndoItem (Operation opUndo, Operation opRedo, LPCTSTR lpStrUndo, LPCTSTR lpStrRedo,
	CHARRANGE selUndo, CHARRANGE selRedo, CHARRANGE sel2Undo, CHARRANGE sel2Redo, BOOL bLinkUndo, BOOL bLinkRedo)
{
	m_opUndo = opUndo;
	m_opRedo = opRedo;

	memset(&m_PRedo, 0, sizeof(m_PRedo));
	memset(&m_PUndo, 0, sizeof(m_PUndo));

	if (lpStrUndo)
		m_PUndo.pStr = new CString(lpStrUndo);
	if (lpStrRedo)
		m_PRedo.pStr = new CString(lpStrRedo);

	m_PUndo.sel = selUndo;
	m_PUndo.sel2 = sel2Undo;
	m_PRedo.sel = selRedo;
	m_PRedo.sel2 = sel2Redo;

	m_PUndo.bLink = bLinkUndo;
	m_PRedo.bLink = bLinkRedo;
}

CUndoItem::CUndoItem (Operation opUndo, Operation opRedo, LPCTSTR lpStrUndo, LPCTSTR lpStrRedo,
	CHARRANGE selUndo, CHARRANGE selRedo)
{
	m_opUndo = opUndo;
	m_opRedo = opRedo;

	memset(&m_PRedo, 0, sizeof(m_PRedo));
	memset(&m_PUndo, 0, sizeof(m_PUndo));

	if (lpStrUndo)
		m_PUndo.pStr = new CString(lpStrUndo);
	if (lpStrRedo)
		m_PRedo.pStr = new CString(lpStrRedo);

	m_PUndo.sel = selUndo;
	m_PRedo.sel = selRedo;
}

CUndoItem::CUndoItem (Operation opUndo, Operation opRedo, LPCTSTR lpStrUndo, _TCHAR chRedo,
	CHARRANGE selUndo, CHARRANGE selRedo)
{
	m_opUndo = opUndo;
	m_opRedo = opRedo;

	memset(&m_PRedo, 0, sizeof(m_PRedo));
	memset(&m_PUndo, 0, sizeof(m_PUndo));

	if (lpStrUndo)
		m_PUndo.pStr = new CString(lpStrUndo);
	m_PRedo.ch = chRedo;

	m_PUndo.sel = selUndo;
	m_PRedo.sel = selRedo;
}

CUndoItem::CUndoItem (Operation opUndo, Operation opRedo, _TCHAR chUndo, _TCHAR chRedo,
	CHARRANGE selUndo, CHARRANGE selRedo)
{
	m_opUndo = opUndo;
	m_opRedo = opRedo;

	memset(&m_PRedo, 0, sizeof(m_PRedo));
	memset(&m_PUndo, 0, sizeof(m_PUndo));

	m_PUndo.sel = selUndo;
	m_PRedo.sel = selRedo;

	m_PUndo.ch = chUndo;
	m_PRedo.ch = chRedo;
}

CUndoItem::CUndoItem (Operation opUndo, Operation opRedo, LPCTSTR lpStrUndo,
	CHARRANGE selUndo, CHARRANGE selRedo)
{
	m_opUndo = opUndo;
	m_opRedo = opRedo;

	memset(&m_PRedo, 0, sizeof(m_PRedo));
	memset(&m_PUndo, 0, sizeof(m_PUndo));

	m_PUndo.sel = selUndo;
	m_PRedo.sel = selRedo;

	if (lpStrUndo)
		m_PUndo.pStr = new CString(lpStrUndo);
}

