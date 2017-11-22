/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

// Undo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Undo Class

#define MAX_UNDO 500

typedef struct tag_Parm {
	BOOL bLink;
	_TCHAR ch;
	CHARRANGE sel;
	CHARRANGE sel2;
	CString *pStr;
} OPPARM;
typedef OPPARM FAR* LPOPPARM;

enum Operation {
	NoOp,
	InsertCharOp,
	ReverseInsertCharOp,
	ReplaceCharOp,
	ReverseReplaceCharOp,
	DeleteOp,
	ReverseDeleteOp,
	BackspaceOp,
	ReverseBackspaceOp,
	PasteOp,
	ReversePasteOp,
	InsertStringOp,
	ReverseInsertStringOp,
	ReplaceTextAtNewLocOp,
	ReverseReplaceTextAtNewLocOp,
	DragOp,
	ReverseDragOp,
	DropOp,
	ReverseDropOp
};

class CUndoItem {

public:
	Operation m_opRedo, m_opUndo;
	OPPARM m_PUndo;
	OPPARM m_PRedo;

public:
	CUndoItem ();
	CUndoItem (Operation opUndo, Operation opRedo, LPCTSTR lpStrUndo, LPCTSTR lpStrRedo,
		CHARRANGE selUndo, CHARRANGE selRedo, CHARRANGE sel2Undo, CHARRANGE sel2Redo,
		BOOL bLinkUndo=false, BOOL bLinkRedo=false);
	CUndoItem (Operation opUndo, Operation opRedo, LPCTSTR lpStrUndo, LPCTSTR lpStrRedo,
		CHARRANGE selUndo, CHARRANGE selRedo);
	CUndoItem (Operation opUndo, Operation opRedo, LPCTSTR lpStrUndo, _TCHAR chRedo,
		CHARRANGE selUndo, CHARRANGE selRedo);
	CUndoItem (Operation opUndo, Operation opRedo, _TCHAR chUndo, _TCHAR chRedo,
		CHARRANGE selUndo, CHARRANGE selRedo);
	CUndoItem (Operation opUndo, Operation opRedo, LPCTSTR lpStrUndo,
		CHARRANGE selUndo, CHARRANGE selRedo);
	~CUndoItem();
};

