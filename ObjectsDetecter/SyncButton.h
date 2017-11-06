#pragma once
#include "SyncImage.h"


class CSyncButton
	: public CButton
{
public:
	typedef SmartPointer<CSyncButton> Ptr;

	CSyncButton(
		COLORREF color, 
		COLORREF hoverColor, 
		UINT imageResourceId = 0,
		UINT hoverUmageResourceId = 0
	);
	~CSyncButton();
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT) override;
protected:
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT, CPoint);
	DECLARE_MESSAGE_MAP()
protected:
	void DrawButtonImage(CDC *dc, CRect btnRect, int index = 0) const;
	int AddImageToImageList(CBitmap &image, COLORREF mask = 0);
protected:
	bool _hovered;
#define IMAGE_INDEX_NOT_DEFINED -1
	int _imageIndex = IMAGE_INDEX_NOT_DEFINED;
	int _hoverImageIndex = IMAGE_INDEX_NOT_DEFINED;
	//States colors
protected:
	COLORREF _hoverButtonColor;
	COLORREF _color;
	COLORREF _prevBkColor;
private:
	int _imageListSize = 0;
	CSize _imageSize;
	HIMAGELIST _imageList;
};
