#include "stdafx.h"
#include "SyncImage.h"


CSyncImage::CSyncImage(UINT imageResourceId)
{
	CBitmap image;
	image.LoadBitmap(imageResourceId);
	BITMAP bm;
	image.GetBitmap(&bm);
	_size = CSize(bm.bmWidth, bm.bmHeight);
	_list.Create(_size.cx, _size.cy, ILC_COLOR32 | ILC_MASK, 8, 8);
	_list.Add(&image, RGB(0, 0, 0));
}


void CSyncImage::Draw(CDC *dc, int x, int y) const
{
	static CPoint point;

	point.x = x;
	point.y = y;
	_list.Draw(dc, 0, point, ILD_NORMAL);
}


CSize CSyncImage::GetSize() const
{
	return _size;
}
