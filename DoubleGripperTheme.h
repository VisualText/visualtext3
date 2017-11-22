#pragma once
#include "CommandBars\xtppaintmanager.h"

class CDoubleGripperTheme :
	public CXTPDefaultTheme
{
public:

    virtual CSize DrawCommandBarGripper(
		CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw);

};
