#ifndef _CMYDOCMULTI
#define _CMYDOCMULTI
#include <afxwin.h>

// MDI support (zero or more documents)
class CmyMultiDocTemplate : public CMultiDocTemplate
{
	DECLARE_DYNAMIC(CmyMultiDocTemplate)

// Constructors
public:
	CmyMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);

// Implementation
public:
	// Menu and accel table for MDI Child windows of this type
	HMENU m_hMenuShared;
	HACCEL m_hAccelTable;

	virtual ~CmyMultiDocTemplate();
	virtual void LoadTemplate();
	virtual void AddDocument(CDocument* pDoc);
	virtual void RemoveDocument(CDocument* pDoc);
	virtual POSITION GetFirstDocPosition() const;
	virtual CDocument* GetNextDoc(POSITION& rPos) const;
	virtual CDocument* OpenDocumentFile(
		LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE, int iconID=0);
	virtual void SetDefaultTitle(CDocument* pDocument);
	virtual CFrameWnd* CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther) ;

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

protected:  // standard implementation
	CPtrList m_docList;          // open documents of this type
	UINT m_nUntitledCount;   // start at 0, for "Document1" title
};


/////////////////////////////////////////////////////////////////////////////
// CDocManager

class CMyDocManager : public CObject
{
	DECLARE_DYNAMIC(CMyDocManager)
public:

// Constructor
	CMyDocManager();

	//Document functions
	virtual void AddDocTemplate(CDocTemplate* pTemplate);
	virtual POSITION GetFirstDocTemplatePosition() const;
	virtual CDocTemplate* GetNextDocTemplate(POSITION& pos) const;
	virtual void RegisterShellFileTypes(BOOL bCompat);
	void UnregisterShellFileTypes();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName); // open named file
	virtual BOOL SaveAllModified(); // save before exit
	virtual void CloseAllDocuments(BOOL bEndSession); // close documents before exiting
	virtual int GetOpenDocumentCount();

	// helper for standard commdlg dialogs
	virtual BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle,
			DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate);

//Commands
	// Advanced: process async DDE request
	virtual BOOL OnDDECommand(LPTSTR lpszCommand);
	virtual void OnFileNew();
	virtual void OnFileOpen();

// Implementation
protected:
	CPtrList m_templateList;
	int GetDocumentCount(); // helper to count number of total documents

public:
	void OnFileNewTemplateDoc(CDocTemplate* pTemplate) ;
	static CPtrList* pStaticList;       // for static CDocTemplate objects
	static BOOL bStaticInit;            // TRUE during static initialization
	static CMyDocManager* pStaticDocManager;  // for static CDocTemplate objects

public:
	virtual ~CMyDocManager();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

#endif
