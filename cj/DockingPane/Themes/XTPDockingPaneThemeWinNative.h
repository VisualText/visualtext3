//===========================================================================
// Summary:
//     CXTPDockingPaneWinNativeTheme is CXTPDockingPaneVisualStudio2003Theme derived class, represents
//     Win XP theme for docking panes.
// Remarks:
//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeWinNative); to set this theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneWinNativeTheme : public CXTPDockingPaneVisualStudio2003Theme
{
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPaneWinNativeTheme object.
	//-------------------------------------------------------------------------
	CXTPDockingPaneWinNativeTheme();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of the pane.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this member function to draw a caption.
	// Parameters:
	//     pDC - Reference to the device context in which to draw.
	//     pPane - Pointer to the tabbed container.
	//     rc - Client rectangle of the tabbed container.
	//-----------------------------------------------------------------------
	virtual void DrawCaption(CDC* pDC, CXTPDockingPaneTabbedContainer* pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to draw frame of floating window
	// Parameters:
	//     pDC    - Pointer to a valid device context
	//     pPane - Floating window pointer
	//     rc    - CLient rectangle of floating frame
	//-----------------------------------------------------------------------
	void DrawFloatingFrame(CDC* pDC, CXTPDockingPaneMiniWnd* pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this member function to draw the side panel.
	// Parameters:
	//     pDC    - Reference to the device context in which to draw.
	//     pPane - Pointer to the side panel.
	//     rc    - Client rectangle of the side panel.
	//-----------------------------------------------------------------------
	void DrawSidePanel(CDC* pDC, CXTPDockingPaneSidePanel* pPane, CRect rc);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Draw common part of tabbed and floating caption.
	// Parameters:
	//     pDC        - Reference to the device context in which to draw.
	//     pPane     - Container which caption need to draw.
	//     rcCaption - Caption bounding rectangle
	//     strTitle  - Caption text.
	//     bActive   - TRUE if caption is active.
	//     bVertical - TRUE to draw caption vertically
	//-----------------------------------------------------------------------
	virtual void DrawCaptionPart(CDC* pDC, CXTPDockingPaneBase* pPane, CRect rcCaption, const CString& strTitle, BOOL bActive, BOOL bVertical);

protected:
};
