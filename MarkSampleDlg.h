/*******************************************************************************
Copyright (c) 1999-2010 by Text Analysis International, Inc.
All rights reserved.
*******************************************************************************/

#if !defined(AFX_MARKSAMPLEDLG_H__30699D81_5B38_11D3_A414_00105A99059F__INCLUDED_)
#define AFX_MARKSAMPLEDLG_H__30699D81_5B38_11D3_A414_00105A99059F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MarkSampleDlg.h : header file
//

#define NUM_SAMPLE_FIELDS 9
#define SAMPSEP '\t'

/////////////////////////////////////////////////////////////////////////////
// CMarkSampleDlg dialog

class CMarkSampleDlg : public CDialog
{
// Construction
public:
	CMarkSampleDlg(CWnd* pParent = NULL);   // standard constructor

public:
	void SetInfo(CString sampleStr, CString textPathStr, CONCEPT *concept, long start, long end);
	

// Dialog Data
	//{{AFX_DATA(CMarkSampleDlg)
	enum { IDD = IDD_CHOOSE_SAMPLE };
	CString	m_strEdit1;
	CString	m_strEdit2;
	CString	m_strEdit3;
	CString	m_strEdit4;
	CString	m_strEdit5;
	CString	m_strEdit6;
	CString	m_strEdit7;
	CString	m_strStatic1;
	CString	m_strStatic2;
	CString	m_strStatic3;
	CString	m_strStatic4;
	CString	m_strStatic5;
	CString	m_strStatic6;
	CString	m_strStatic7;
	BOOL	m_boolCheck1;
	BOOL	m_boolCheck2;
	BOOL	m_boolCheck3;
	BOOL	m_boolCheck4;
	BOOL	m_boolCheck5;
	BOOL	m_boolCheck6;
	BOOL	m_boolCheck7;
	CString	m_strSample;
	BOOL	m_boolCheck8;
	BOOL	m_boolCheck9;
	CString	m_strEdit8;
	CString	m_strEdit9;
	CString	m_strStatic8;
	CString	m_strStatic9;
	BOOL	m_boolSpecial;
	BOOL	m_boolDrag;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMarkSampleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMarkSampleDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void OnSpecial();
	afx_msg void OnCheck2();
	afx_msg void OnCheck3();
	afx_msg void OnCheck4();
	afx_msg void OnCheck5();
	afx_msg void OnCheck6();
	afx_msg void OnCheck7();
	afx_msg void OnCheck8();
	afx_msg void OnCheck9();
	afx_msg void OnGatherUp();
	afx_msg void OnGatherDown();
	afx_msg void OnRevert();
	afx_msg void OnCheck1();
	afx_msg void OnTearUp();
	afx_msg void OnTearDown();
	afx_msg void OnDrag();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CONCEPT *m_concept;
	int m_intConceptCount;
	CStringList m_strListConcepts;
	CStringList m_strListSamples;
	long m_longStarts[NUM_SAMPLE_FIELDS];
	long m_longEnds[NUM_SAMPLE_FIELDS];
	int m_boolChecks[NUM_SAMPLE_FIELDS];

	int m_fieldSelected;

	CString m_strTextPath;
	long m_longStart;
	long m_longEnd;
	bool m_boolGather;
	bool m_boolTear;

	CBitmapButton bitmapButtonUp;
	CBitmapButton bitmapButtonDown;
	CBitmapButton bitmapButtonGatherDown;
	CBitmapButton bitmapButtonGatherUp;
	CBitmapButton bitmapButtonTearDown;
	CBitmapButton bitmapButtonTearUp;

	CString PopHead(int index);
	CString PopTail(int index);
	CString RemoveSample(int index);
	void SwitchSamples(int index1, int index2);
	void PushHead(int index, CString poppedStr);
	void PushTail(int index, CString poppedStr);
	void PushSamplesUp(int indexTop, int indexBottom);
	void PushSamplesDown(int indexTop, int indexBottom);
	CString GetSampleInfo(int index);
	void SetSampleInfo(int index, CString infoStr);
	CString SampleInfo(CString sampleStr, CString conceptStr, long start, long end);
	void SeparateSamples(CStringList &samples, CString sampleStr, long start, long end, bool conceptFlag=TRUE);
	void GetSampleLine(CString sampleStr, CString &sampStr,
					   CString &conceptStr, long &start, long &end);
	void FillGuesses();
	void Up(int itemNumber);
	void Down(int itemNumber);
	void ClearFields();
	void CombineDown(int itemNumber, int itemNumber1);
	void MoveIt(int itemNumber, bool downFlag=FALSE);
	CString Merge(CString sampleInfoStr1, CString sampleInfoStr2);
	void SetFieldData(int index, CString sampleStr, CString conceptStr, long start, long end, int check=-1);
	void GetFields();
	void SetFields();
	void ClearFieldData(int index);
	bool IsFieldSelected();
	void DeselectOthers(int fieldNum=0);
	void CheckAction(int fieldNum);
	void SetCheckValue(int fieldNum, bool value=TRUE);
	BOOL GetCheckValue(int fieldNum);
	void EnableCheck(int fieldNum,BOOL value);
	void Enable(UINT id, BOOL value);
	void UpdateButtonActivationStatus();
	int NumSamplePieces(int offset=0);
	BOOL GetState(UINT id);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MARKSAMPLEDLG_H__30699D81_5B38_11D3_A414_00105A99059F__INCLUDED_)
