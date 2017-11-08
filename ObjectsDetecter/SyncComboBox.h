#pragma once


class CSyncComboBox :
	public CComboBox
{
public:
	CSyncComboBox();
	virtual ~CSyncComboBox();

protected:
	afx_msg void OnCbnSelchange();
	DECLARE_MESSAGE_MAP()

private:
	int _selectionIndex;
};
