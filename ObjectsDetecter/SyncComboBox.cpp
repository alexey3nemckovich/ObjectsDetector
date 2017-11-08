#include "stdafx.h"
#include "ControlsMessages.h"
//
#include "SyncComboBox.h"


CSyncComboBox::CSyncComboBox()
{
	_selectionIndex = 0;
}


CSyncComboBox::~CSyncComboBox()
{

}


BEGIN_MESSAGE_MAP(CSyncComboBox, CComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CSyncComboBox::OnCbnSelchange)
END_MESSAGE_MAP()


afx_msg void CSyncComboBox::OnCbnSelchange()
{
	GetParent()->SendMessage(SELECTION_CHANGED_MSG, GetCurSel());
}
