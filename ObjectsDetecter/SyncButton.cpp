#include "stdafx.h"
#include "SyncButton.h"
#include "WindowUtilities.h"


CSyncButton::CSyncButton(
	COLORREF color,
	COLORREF hoverColor,
	UINT imageResourceId/* = 0*/,
	UINT hoverImageResourceId/* = 0*/
)
{
	_hovered = false;
	_color = color;
	_hoverButtonColor = hoverColor;
	_imageList = nullptr;

	if (imageResourceId || hoverImageResourceId)
	{
		CBitmap image;

		if (imageResourceId)
		{
			image.LoadBitmap(imageResourceId);
			_imageIndex = AddImageToImageList(image);
			image.DeleteObject();
		}
		
		if (hoverImageResourceId)
		{
			image.LoadBitmap(hoverImageResourceId);
			_hoverImageIndex = AddImageToImageList(image);
		}		
	}
}


CSyncButton::~CSyncButton()
{
	ImageList_Destroy(_imageList);
}


void CSyncButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	CRect btnRect;
	btnRect = lpDrawItemStruct->rcItem;

	COLORREF newBkColor;
	int imageIndex = _imageIndex;
	if (_hovered)
	{
		newBkColor = _hoverButtonColor;

		if (IMAGE_INDEX_NOT_DEFINED != _hoverImageIndex)
		{
			imageIndex = _hoverImageIndex;
		}
	}
	else
	{
		newBkColor = _color;
	}

	dc.FillSolidRect(btnRect, newBkColor);

	if (IMAGE_INDEX_NOT_DEFINED != imageIndex)
	{
		DrawButtonImage(&dc, btnRect, imageIndex);
	}

	dc.Detach();
}


void CSyncButton::DrawButtonImage(CDC *dc, CRect btnRect, int index/* = 0*/) const
{
	if (_imageList)
	{
		CPoint btnCenter = GetRectCenter(btnRect);
		int x = btnCenter.x - _imageSize.cx / 2;
		int y = btnCenter.y - _imageSize.cy / 2;
		ImageList_DrawEx(_imageList, index, *dc, x, y, 0, 0, CLR_NONE, CLR_NONE, ILD_NORMAL);
	}
}


int CSyncButton::AddImageToImageList(CBitmap &image, COLORREF mask/* = 0*/)
{
	if (nullptr == _imageList)
	{
		BITMAP bm;
		image.GetBitmap(&bm);
		_imageSize = CSize(bm.bmWidth, bm.bmHeight);
		_imageList = ImageList_Create(_imageSize.cx, _imageSize.cy, ILC_COLOR32 | ILC_MASK, 8, 8);
	}

	ImageList_AddMasked(_imageList, image, mask);
	return _imageListSize++;
}


BEGIN_MESSAGE_MAP(CSyncButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()


afx_msg void CSyncButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!_hovered)
	{
		wutils::StartTrackingMouseEvent(this);
		_hovered = true;

		RedrawWindow();
	}

	CWnd::OnMouseMove(nFlags, point);
}


afx_msg void CSyncButton::OnMouseLeave()
{
	_hovered = false;
	RedrawWindow();
}
