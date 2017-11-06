#pragma once


class CSyncLabel
	: public CStatic
{
public:
	typedef SmartPointer<CSyncLabel> Ptr;

	CSyncLabel(
		int textSize = 10, 
		COLORREF textColor = RGB(0, 0, 0), 
		bool cursiveText = false, 
		bool boldText = false, 
		bool underlinedText = false
	);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg int OnSetText(LPCTSTR);
	DECLARE_MESSAGE_MAP()
private:
	CFontPtr _font;
	int _textSize;
	COLORREF _textColor;
	bool _underlined, _cursive, _bold;
};
