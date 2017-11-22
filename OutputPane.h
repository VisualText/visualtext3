#pragma once


// COutputPane view

class COutputPane : public CListView
{
	DECLARE_DYNCREATE(COutputPane)

protected:
	COutputPane();           // protected constructor used by dynamic creation
	virtual ~COutputPane();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


