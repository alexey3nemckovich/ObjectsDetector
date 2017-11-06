#include "stdafx.h"
#include "SyncLabel.h"
#include "WindowUtilities.h"


CSyncLabel::CSyncLabel(
	int textSize/* = 10*/,
	COLORREF textColor/* = RGB(0, 0, 0)*/,
	bool cursiveText/* = false*/,
	bool boldText/* = false*/,
	bool underlinedText/* = false*/
)
{
	_textColor = textColor;
	_textSize = textSize;
	_underlined = underlinedText;
	_cursive = cursiveText;
	_bold = boldText;
}


BEGIN_MESSAGE_MAP(CSyncLabel, CStatic)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SETTEXT()
	//ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


afx_msg int CSyncLabel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CStatic::OnCreate(lpCreateStruct);

	lpCreateStruct->dwExStyle |= WS_EX_TRANSPARENT;
	_font = wutils::CreateSimpleFont(this, L"Arial", _textSize, _bold, _cursive, _underlined);
	SetFont(_font, TRUE);
	wutils::ResizeToInnerText(this);
	return 0;
}


HBRUSH CSyncLabel::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetTextColor(_textColor);
	pDC->SetBkMode(TRANSPARENT);
	return (HBRUSH)GetStockObject(NULL_BRUSH);
}


afx_msg int CSyncLabel::OnSetText(LPCTSTR text)
{
	auto res = Default();
	wutils::ResizeToInnerText(this);

	wutils::MakeParentToRedrawWindowRect(this);
	return res;
}
