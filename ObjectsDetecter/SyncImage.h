#pragma once
#include "afxwin.h"


class CSyncImage
{
public:
	typedef SmartPointer<CSyncImage> Ptr;

	CSyncImage(UINT imageResourceId);
	void Draw(CDC *dc, int x, int y) const;
	CSize GetSize() const;
private:
	mutable CImageList _list;
	CSize _size;
};
