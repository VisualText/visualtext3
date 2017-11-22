/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_EDITLISTCTRL_H__55ACD802_3667_11D3_A3AE_00105A99059F__INCLUDED_)
#define AFX_EDITLISTCTRL_H__55ACD802_3667_11D3_A3AE_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditListCtrl.h : header file
//

#include <afxtempl.h>

enum ValueType { valNULL=0, valAlpha, valNumber, valConcept };

class CAttribute
{
protected:
	bool m_bOpen;
	CString m_Name;
	CStringArray m_strValues;
	CByteArray m_ValTypes;
	CList<CONCEPT *, CONCEPT *> m_concepts;

public:
	CAttribute ();
	CAttribute(LPCTSTR lpName);
	~CAttribute();
	BOOL IsNullValue(int nVal);
	int AddNullValue(void);
	int AddValue(LPCTSTR lpValue, CONCEPT *concept=NULL);
	CAttribute::ValueType CalcValueType(LPCTSTR lpValue);
	void DeleteValue(int nPos);
	CString GetName(void);
	int GetNumValues(void);
	CString GetValue(int nPos);
	CONCEPT *GetConcept(int nVal);
	ValueType GetValueType(int nVal);
	BOOL IsOpen(void);
	void SetName(LPCTSTR lpName);
	void SetOpen(bool);
	int SetValue(int nIndex, LPCTSTR lpValue, CONCEPT *concept=NULL,
		ValueType = valAlpha);
};

class CLineItem
{
public:
	bool m_bDeleteValue;
protected:
	CAttribute* m_pAttribute;
	int m_Index;
public:
	CLineItem();
	CLineItem(CAttribute* pAttribute, int index);
	~CLineItem();
	int AddValue(LPCTSTR lpValue, CONCEPT *concept=NULL);
	int DecrementIndex(void);
	void DeleteValue(void);
	CAttribute* GetAttribute(void);
	void DeleteAttribute();	// 06/19/02 AM.
	int GetIndex(void);
	CString GetName(void);
	int GetNumValues(void);
	CString GetValue(void);
	CONCEPT *GetConcept(void);
	ValueType GetValueType(void);
	CString GetValueTypeString(void);
	BOOL IsOpen(void);
	void SetIndex(int newIndex);
	void SetName(LPCTSTR lpName);
	void SetOpen(bool);
	int SetValue(LPCTSTR lpValue, CONCEPT *concept=NULL,
		ValueType typ = valAlpha);	// 06/19/02 AM.
};

/////////////////////////////////////////////////////////////////////////////
// CEditListCtrl window
#define WMU_DELETELINE	(WM_USER+1001)
#define WMU_GETIMAGELIST (WM_USER+1002)

class CEditListCtrl : public CListCtrl
{
// Construction
public:
	CEditListCtrl();

// Attributes
public:
	ValueType m_valueTypeNew;
	BOOL m_IgnoreEditColumn0;

// Operations
public:
	int HitTestEx(CPoint &point, int *col) const;
	CEdit* EditSubLabel( int nItem, int nCol );
	void ExpandValues(int nLineFirst);
	void ContractValues(int nLineFirst);
	void EndLabelEdit(int line, int subItem, CString newStr);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetAttributeType(CString typeStr);
	void SetAttributeType(ValueType type);
	virtual ~CEditListCtrl();
	void SetConcept(CONCEPT *concept);
	CONCEPT *GetConcept();
	ValueType ValueTypeFromString(CString typeStr);	// 06/19/02 AM.

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditListCtrl)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	CONCEPT *m_concept;

	void Init();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITLISTCTRL_H__55ACD802_3667_11D3_A3AE_00105A99059F__INCLUDED_)
