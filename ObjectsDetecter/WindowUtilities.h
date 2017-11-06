#pragma once
#include "SyncLabel.h"


namespace wutils
{
	const int CheckBoxDefaultSize = 20;

	bool CreateSimpleWindow(CWnd* wnd, CWnd* parentWnd, UINT id = 0, UINT style = 0, CRect* rect = nullptr);
	bool CreateSimpleButton(CButton* button, CWnd* parentWnd, LPCTSTR text = 0, UINT id = 0, UINT style = 0);
	bool CreateSimpleCheckBox(CButton *button, CWnd* parentWnd, LPCTSTR text = 0, UINT id = 0, UINT style = 0);
	bool CreateSimpleLabel(CSyncLabel* staticCtrl, CWnd* parentWnd, CString text = 0, UINT id = 0, UINT style = 0);
	bool CreateSimpleComboBox(CComboBox* box, CWnd* parentWnd, UINT id = 0, UINT style = 0);
	CFont* CreateSimpleFont(CWnd* wnd, CString Typeface, int Size, BOOL Bold = false, BOOL Italic = false, BOOL UnderLine = false);

	enum Border
	{
		Left, Top, Right, Bottom
	};

	void MakeParentToRedrawWindowRect(CWnd*);

	void AlignWindowToTheBorder(Border border, CWnd* targetWindow, CWnd* window);

	void MoveWindowVertically(CWnd*, int dy);
	void MoveWindowHorizontally(CWnd*, int dx);
	
	void CenterWindowInParent(CWnd* wnd);
	void CenterWindowVerticallyInParent(CWnd* wnd);
	void CenterWindowHorizontallyInParent(CWnd* wnd);

	void ResizeToInnerText(CWnd* wnd, int maxWidth = -1);
	void StartTrackingMouseEvent(CWnd* wnd);
    void SetBackground(CDC* pDC, COLORREF color);

	CSize GetSize(CWnd *wnd);
	CRect GetRectRelativeToParent(CWnd* wnd);
	CPoint GetWindowCenter(CWnd* wnd);
	CPoint GetWindowCenterRelativeToParent(CWnd* wnd);

	Gdiplus::Bitmap* LoadBitmapImageFromResource(const int resourceID);
	void DrawImageGdi(Gdiplus::Bitmap* image, HDC hDC, CRect rect);

}
