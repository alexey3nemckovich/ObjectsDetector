#include "stdafx.h"
#include "WindowUtilities.h"


namespace wutils
{
    
    void SetBackground(CDC* pDC, COLORREF color)
    {
        CBrush backBrush(color);

        CBrush* pOldBrush = pDC->SelectObject(&backBrush);

        CRect rect;
        pDC->GetClipBox(&rect);

        pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
        pDC->SelectObject(pOldBrush);
    }


    bool CreateSimpleWindow(CWnd* wnd, CWnd* parentWnd, UINT id/* = 0*/, UINT style/* = 0*/, CRect* rect/* = nullptr*/)
    {
        return wnd->Create(
            NULL,
            NULL,
            style ? style : WS_VISIBLE | WS_CHILD,
            rect? *rect : CRect(),
            parentWnd,
            id ? id : IDGenerator::GetInstance()->GenerateFreeIDForControlOf(parentWnd)
        );
    }


    bool CreateSimpleButton(CButton* button, CWnd* parentWnd, LPCTSTR text/* = 0*/, UINT id/* = 0*/, UINT style/* = 0*/)
    {
		#define ST_STYLE WS_VISIBLE | WS_CHILD
        return button->Create(
            text ? text : L"",
            style ? style : ST_STYLE,
            CRect(),
            parentWnd,
            id ? id : IDGenerator::GetInstance()->GenerateFreeIDForControlOf(parentWnd)
        );
    }


	bool CreateSimpleCheckBox(CButton *button, CWnd* parentWnd, LPCTSTR text/* = 0*/, UINT id/* = 0*/, UINT style/* = 0*/)
	{
		return CreateSimpleButton(button, parentWnd, text, id, style ? style : WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX);
	}


	bool CreateSimpleLabel(CSyncLabel* label, CWnd* parentWnd, CString text/* = 0*/, UINT id/* = 0*/, UINT style/* = 0*/)
	{
		#define ST_STYLE (WS_VISIBLE | WS_CHILD | SS_CENTER)
		return label->Create(
			text ? text : L"",
			style ? style : ST_STYLE,
			CRect(),
			parentWnd,
			id ? id : IDGenerator::GetInstance()->GenerateFreeIDForControlOf(parentWnd)
		);
	}


	bool CreateSimpleComboBox(CComboBox* box, CWnd* parentWnd, UINT id/* = 0*/, UINT style/* = 0*/)
	{
		#define CBS_STYLE (WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS)
		return box->Create(
			style ? style : CBS_STYLE,
			CRect(),
			parentWnd,
			id ? id : IDGenerator::GetInstance()->GenerateFreeIDForControlOf(parentWnd)
		);
	}


    CFont* CreateSimpleFont(
        CWnd* wnd,
        CString Typeface,
        int Size,
        BOOL Bold/* = false*/,
        BOOL Italic/* = false*/,
		BOOL UnderLine/* = false*/
    )
    {
        // Получим контекст окна
        CWindowDC winDC(wnd);
        // Узнаем, сколько пикселей в одном логическом дюйме
        int pixelsPerInch = winDC.GetDeviceCaps(LOGPIXELSY);
        // Узнаем высоту в пикселях шрифта размером Size пунктов
        int fontHeight = -MulDiv(Size, pixelsPerInch, 72);
        int Weight = FW_NORMAL;
        if (Bold)
            Weight = FW_BOLD;
        // Удаляем предыдущий экземпляр шрифта -- нельзя дважды 
        // инициализировать шрифт вызовом CreateFont().
        CFont* newFont = new CFont;
        newFont->CreateFont(fontHeight, 0, 0, 0, Weight, Italic, UnderLine, 0,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, Typeface);

        return newFont;
    }


	CPoint GetWindowCenter(CWnd* wnd)
	{
		CRect rect;
		wnd->GetClientRect(&rect);
		return GetRectCenter(rect);
	}


	CPoint GetWindowCenterRelativeToParent(CWnd* wnd)
	{
		CRect rect = GetRectRelativeToParent(wnd);
		return GetRectCenter(rect);
	}

	void ResizeToInnerText(CWnd* wnd, int maxWidth/* = -1*/)
	{
		CString csText;
		wnd->GetWindowText(csText);

		CPaintDC dc(wnd);
		CFont* font = wnd->GetFont();
		dc.SelectObject(font);
		CSize textSize = dc.GetTextExtent(csText, csText.GetLength());
		CRect rect = GetRectRelativeToParent(wnd);
		if (-1 == maxWidth || textSize.cx <= maxWidth)
		{
			rect.right = rect.left + textSize.cx;
			rect.bottom = rect.top + textSize.cy;
		}
		else
		{
			auto scale = (float)textSize.cx / maxWidth;
			rect.right = rect.left + maxWidth;
			int sc = round(scale);
			if (0 != scale - sc)
			{
				sc++;
			}
			rect.bottom = rect.top + textSize.cy * sc;
		}
		wnd->MoveWindow(&rect, TRUE);
	}

	void CenterWindowHorizontallyInParent(CWnd* wnd)
	{
		CRect wndRect = GetRectRelativeToParent(wnd);

		CPoint parentCenter = GetWindowCenter(wnd->GetParent());
		CPoint wndCenter = GetRectCenter(wndRect);

		int xDiff = parentCenter.x - wndCenter.x;
		wndRect.left += xDiff;
		wndRect.right += xDiff;

		wnd->MoveWindow(wndRect);
	}


	void CenterWindowVerticallyInParent(CWnd* wnd)
	{
		CRect wndRect = GetRectRelativeToParent(wnd);

		CPoint parentCenter = GetWindowCenter(wnd->GetParent());
		CPoint wndCenter = GetRectCenter(wndRect);

		int yDiff = parentCenter.y - wndCenter.y;
		wndRect.top += yDiff;
		wndRect.bottom += yDiff;

		wnd->MoveWindow(wndRect);
	}


	void CenterWindowInParent(CWnd* wnd)
	{
		CenterWindowHorizontallyInParent(wnd);
		CenterWindowVerticallyInParent(wnd);
	}


	CRect GetRectRelativeToParent(CWnd* wnd)
	{
		CRect wndRect;
		wnd->GetWindowRect(wndRect);
		wnd->GetParent()->ScreenToClient(&wndRect);
		return wndRect;
	}


	CSize GetSize(CWnd *wnd)
	{
		CRect rect = GetRectRelativeToParent(wnd);
		return GetRectSize(rect);
	}


	void StartTrackingMouseEvent(CWnd* wnd)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = wnd->GetSafeHwnd();
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 1;
		_TrackMouseEvent(&tme);
	}


	void MakeParentToRedrawWindowRect(CWnd* wnd)
	{
		CRect rect = GetRectRelativeToParent(wnd);
		wnd->GetParent()->InvalidateRect(&rect, true);
		wnd->GetParent()->UpdateWindow();
	}

	
	void AlignWindowToTheBorder(Border border, CWnd* targetWindow, CWnd* window)
	{
		int diff;
		CRect targetWindowRect = GetRectRelativeToParent(targetWindow);
		CRect windowRect = GetRectRelativeToParent(window);

		switch (border)
		{
		case Border::Left:
			diff = targetWindowRect.left - windowRect.left;
			break;
		case Border::Top:
			diff = targetWindowRect.top - windowRect.top;
			break;
		case Border::Right:
			diff = targetWindowRect.right - windowRect.right;
			break;
		case Border::Bottom:
			diff = targetWindowRect.bottom - windowRect.bottom;
			break;
		}

		if (Border::Left == border || Border::Right == border)
		{
			MoveWindowHorizontally(window, diff);
		}
		else
		{
			MoveWindowVertically(window, diff);
		}
	}


	void MoveWindowHorizontally(CWnd *wnd, int dx)
	{
		CRect rect = GetRectRelativeToParent(wnd);
		rect.left += dx;
		rect.right += dx;
		wnd->MoveWindow(&rect, TRUE);
	}


	void MoveWindowVertically(CWnd *wnd, int dy)
	{
		CRect rect = GetRectRelativeToParent(wnd);
		rect.top += dy;
		rect.bottom += dy;
		wnd->MoveWindow(&rect, TRUE);
	}

	Gdiplus::Bitmap* LoadBitmapImageFromResource(const int resourceID)
	{
		Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromResource(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceID));
		ASSERT(bitmap);
		return bitmap;
	}

	void DrawImageGdi(Gdiplus::Bitmap* image, HDC hDC, CRect rect)
	{
		Gdiplus::Graphics gr(hDC);
		gr.DrawImage(image, rect.left, rect.top);
	}

}
