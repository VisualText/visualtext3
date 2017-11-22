#pragma once


// CFind2Pane view

class CFind2Pane : public CListView
{
	DECLARE_DYNCREATE(CFind2Pane)

protected:
	CFind2Pane();           // protected constructor used by dynamic creation
	virtual ~CFind2Pane();

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


