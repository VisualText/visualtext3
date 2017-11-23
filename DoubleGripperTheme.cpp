#include "stdafx.h"
#include "DoubleGripperTheme.h"

// DrawCommandBarGripper function. 
// if bDraw if FALSE must return gripper size.
// if bDraw is TRUE must draw gripper.  
CSize CDoubleGripperTheme::DrawCommandBarGripper(CDC* pDC,
	CXTPCommandBar* pBar, BOOL bDraw)
{ 
	// If Toolbar is vertical docked
	if (pBar->GetPosition() == xtpBarRight || 
		pBar->GetPosition() == xtpBarLeft)
	{
		if (bDraw)
		{
			CXTPClientRect rc(pBar);
			Draw3dRect(pDC, CRect(3, 3, rc.right - 3, 6),
				COLOR_BTNHILIGHT, COLOR_3DSHADOW);
			Draw3dRect(pDC, CRect(3, 7, rc.right - 3, 10),
				COLOR_BTNHILIGHT, COLOR_3DSHADOW);
		}
		return CSize(0, 10);
	}
	// if Toolbar is horizontal  docked 
	else 
	if (pBar->GetPosition() == xtpBarTop || 
		pBar->GetPosition() == xtpBarBottom)
	{
		CXTPClientRect rc(pBar);
		if (bDraw)
		{
			Draw3dRect(pDC, CRect(3, 3, 6, rc.bottom - 3),
				COLOR_BTNHILIGHT, COLOR_3DSHADOW);
			Draw3dRect(pDC, CRect(7, 3, 10, rc.bottom - 3),
				COLOR_BTNHILIGHT, COLOR_3DSHADOW);
		}
		return CSize(10, 0);
	}
	else return CXTPDefaultTheme::DrawCommandBarGripper(pDC, pBar, bDraw);
}
