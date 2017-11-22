#pragma once
#include "afxcmn.h"

class VTTreeCtrl :
	public CTreeCtrl
{
public:
	VTTreeCtrl(void);
	~VTTreeCtrl(void);
/*
HTREEITEM CopyBranch(CTreeCtrl *treeCtrl, const HTREEITEM &htiBranch, const HTREEITEM &htiNewParent, const HTREEITEM &htiAfter=TVI_LAST);
HTREEITEM CopyItem(CTreeCtrl *treeCtrl, const HTREEITEM &hItem, const HTREEITEM &htiNewParent, const HTREEITEM &htiAfter=TVI_LAST);
int MoveTreeNodeUp(CTreeCtrl *treeCtrl, const HTREEITEM &selected, const int imageBoundary=-1, const int imageSelBoundary=-1);
int MoveTreeNodeDown(CTreeCtrl *treeCtrl, const HTREEITEM &selected, const int imageBoundary=-1, const int imageSelBoundary=-1);
void TreeViewSelectItem(CTreeView* pTreeView, CPoint point); // SRP 010207
int TreeCtrlItemPosition(CTreeCtrl* m_treeCtrl, const HTREEITEM &child);
CString StateString(CTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state);
void StateString(CTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state, CString &allStr, int &count);
void StateFromString(CTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state, const CString &stateStr);
void StateFromString(CTreeCtrl *treeCtrl, const HTREEITEM &treeItem, const STATE_TYPE &state, CStringList &numberList, int &count);
HTREEITEM FindLastConceptSibling(CTreeCtrl *treeCtrl, const HTREEITEM &startingItem);
int TreeItemNumberFromItem(CTreeCtrl *treeCtrl, const HTREEITEM &itemIn);
BOOL IsItemChecked(CTreeCtrl *treeCtrl, const HTREEITEM &hItem);
void DeleteTreeCtrlChildren(CTreeCtrl *treeCtrl, const HTREEITEM &item, const int image=-1, const int imageSel=-1, const bool oneLevel=false);
HTREEITEM NextTreeCtrlItem(CTreeCtrl *treeCtrl, const HTREEITEM &currentItem, const bool backward, const int image, const int imageSel);
int GetTreeCtrlChildrenCount(CTreeCtrl *treeCtrl, const HTREEITEM &item);
void TreeCtrlExpandAll(CTreeCtrl *treeCtrl, const HTREEITEM &item, const UINT nCode=TVE_EXPAND);
void TreeCtrlExpandOneLevel(CTreeCtrl *treeCtrl, const HTREEITEM &item, const UINT nCode=TVE_EXPAND);
HTREEITEM TreeCtrlFirstSibling(CTreeCtrl *treeCtrl, const HTREEITEM &item);
HTREEITEM TreeCtrlLastSibling(CTreeCtrl *treeCtrl, const HTREEITEM &item);
HTREEITEM TreeItemFromName(CTreeCtrl *treeCtrl, const HTREEITEM &item, const CString &itemNameStr);
*/
};
