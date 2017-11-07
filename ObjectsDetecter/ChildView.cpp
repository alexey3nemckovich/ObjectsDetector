#include "stdafx.h"
#include "ObjectsDetecter.h"
#include "ChildView.h"
#include "WindowUtilities.h"
#include <future>
#include <opencv2/imgcodecs.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CChildView::CChildView()
{

}


CChildView::~CChildView()
{

}


BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CWnd::PreCreateWindow(cs))
        return FALSE;

    cs.dwExStyle |= WS_EX_CLIENTEDGE;
    cs.style &= ~WS_BORDER;
    cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
        ::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);

    return TRUE;
}


void CChildView::ChangeStatus(Status status)
{
    _status = status;

    switch (_status)
    {
    case Status::IMAGE_NOT_LOADED:
        {
            _buttonProcessImage.EnableWindow(false);
            _buttonShowImageObjects.EnableWindow(false);
            _buttonShowObjectsGroups.EnableWindow(false);
        }
        break;
    case Status::IMAGE_LOADED:
        {
            _buttonProcessImage.EnableWindow(true);
            _buttonShowImageObjects.EnableWindow(false);
            _buttonShowObjectsGroups.EnableWindow(false);
            _label.SetWindowText(L"");
        }
        break;
    case Status::IMAGE_PROCESSING:
        {
            _buttonLoadImage.EnableWindow(false);
            _buttonProcessImage.EnableWindow(false);
            _buttonProcessImage.SetWindowText(L"Processing");
            _buttonShowImageObjects.EnableWindow(false);
            _buttonShowObjectsGroups.EnableWindow(false);
        }
        break;
    case Status::IMAGE_PROCESSED:
        {
            _buttonLoadImage.EnableWindow(true);
            _buttonProcessImage.EnableWindow(false);
            _buttonProcessImage.SetWindowText(L"Process");
            _buttonShowImageObjects.EnableWindow(true);
            _buttonShowObjectsGroups.EnableWindow(true);
        }
        break;
    }
}


void CChildView::SetMatImage(const cv::Mat& mat)
{
    cv::Mat copyMat;
    mat.copyTo(copyMat);

    CDC memDC;
    CDC* dc = GetDC();
    memDC.CreateCompatibleDC(dc);

    cv::resize(copyMat, copyMat, cv::Size(_cx, _cy - _instrumentsPanelHeight));
    HBITMAP hBmp = cvutils::ConvertCVMatToBMP(copyMat);
    memDC.SelectObject(hBmp);

    dc->BitBlt(0, _instrumentsPanelHeight, _cx, _cy, &memDC, 0, 0, SRCCOPY);
    DeleteObject(hBmp);
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_COMMAND_RANGE(0, 65000, OnRangeCmds)
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


afx_msg int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    wutils::CreateSimpleButton(&_buttonLoadImage, this, L"Load image");
    wutils::CreateSimpleButton(&_buttonProcessImage, this, L"Process image");
    wutils::CreateSimpleButton(&_buttonShowImageObjects, this, L"Show objects");
    wutils::CreateSimpleButton(&_buttonShowObjectsGroups, this, L"Show objects groups");
    
    _label.Create(
        L"",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        CRect(),
        this,
        IDGenerator::GetInstance()->GenerateFreeIDForControlOf(this)
    );

    wutils::ResizeToInnerText(&_buttonLoadImage);
    wutils::ResizeToInnerText(&_buttonProcessImage);
    wutils::ResizeToInnerText(&_buttonShowImageObjects);
    wutils::ResizeToInnerText(&_buttonShowObjectsGroups);

    ChangeStatus(Status::IMAGE_NOT_LOADED);

    return 0;
}


afx_msg void CChildView::OnRangeCmds(UINT id)
{
    if (id == _buttonLoadImage.GetDlgCtrlID())
    {
        OnLoadImage();
    }

    if (id == _buttonProcessImage.GetDlgCtrlID())
    {
        OnProcessImage();
    }

    if (id == _buttonShowImageObjects.GetDlgCtrlID())
    {
        OnShowObjects();
    }

    if (id == _buttonShowObjectsGroups.GetDlgCtrlID())
    {
        OnShowTypes();
    }
}


afx_msg void CChildView::OnLoadImage()
{
    CFileDialog dialog(true);

    int res = dialog.DoModal();
    if (IDOK == res)
    {
        CString imagePath = dialog.GetPathName();

        char* pPath = new char[imagePath.GetLength() + 1];
        pPath[imagePath.GetLength()] = 0;
        std::wcstombs(pPath, imagePath.GetBuffer(), imagePath.GetLength());

        _sourceImage = cv::imread(cv::String(pPath));
        SetMatImage(_sourceImage);
        delete pPath;

        ChangeStatus(Status::IMAGE_LOADED);
    }
}


afx_msg void CChildView::OnProcessImage()
{
    ChangeStatus(Status::IMAGE_PROCESSING);
    _processingImageTaskResult = async([this]()
    {
        _processResult = cvutils::ProcessImage(_sourceImage);
        this->ChangeStatus(Status::IMAGE_PROCESSED);
    });
}


afx_msg void CChildView::OnShowObjects()
{
    SetMatImage(_processResult.objectsImage);

    CString res;
    res.Format(L"Total objects count %d", _processResult.detectedObjects.size());
    _label.SetWindowText(res);
}


afx_msg void CChildView::OnShowTypes()
{
    SetMatImage(_processResult.groupsImage);

    int j = 0;
    wstringstream str;
    for (int i = 0; i < _processResult.detectedObjectsGroups.size(); i++)
    {
        str << "Group" << i << "(" << _processResult.detectedObjectsGroups[i].size() << ")";
        j++;

        if (j == 0)
        {
            str << " ";
        }

        if (j == 2)
        {
            str << "\n\r";
            j = 0;
        }
    }

    CString res(str.str().c_str());
    _label.SetWindowText(res);
}


afx_msg void CChildView::OnPaint()
{
	CPaintDC dc(this);
}


afx_msg void CChildView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    _cx = cx;
    _cy = cy;

    constexpr int _buttonHeigth = 60;

    constexpr int marginX = 50;
    constexpr int marginY = 20;
    CRect thisRect = wutils::GetRectRelativeToParent(this);
    CPoint center = wutils::GetWindowCenter(this);

    CSize btnSize = wutils::GetSize(&_buttonLoadImage);
    
    CRect rect{
        marginX,
        marginY,
        marginX + btnSize.cx + 10,
        marginY + _buttonHeigth
    };
    _buttonLoadImage.MoveWindow(rect);   

    rect.left += marginX + btnSize.cx;
    btnSize = wutils::GetSize(&_buttonProcessImage);
    rect.right += marginX + btnSize.cx;

    _buttonProcessImage.MoveWindow(rect);
    
    rect.left += marginX + btnSize.cx;
    btnSize = wutils::GetSize(&_buttonShowImageObjects);
    rect.right += marginX + btnSize.cx;

    _buttonShowImageObjects.MoveWindow(rect);

    rect.left += marginX + btnSize.cx;
    btnSize = wutils::GetSize(&_buttonShowObjectsGroups);
    rect.right += marginX + btnSize.cx;

    _buttonShowObjectsGroups.MoveWindow(rect);

    rect.left += marginX + btnSize.cx;
    btnSize = wutils::GetSize(&_buttonShowObjectsGroups);
    rect.right += marginX + btnSize.cx;

    _label.MoveWindow(rect);

    RedrawWindow();
}


BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}
