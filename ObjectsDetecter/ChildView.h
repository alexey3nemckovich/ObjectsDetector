#pragma once
#include "SyncLabel.h"
#include "cvUtilities.h"
#include <opencv2/imgcodecs.hpp>
#include <future>


class CChildView : public CWnd
{
public:
	CChildView();
    virtual ~CChildView();

    enum class Status
    {
        IMAGE_NOT_LOADED,
        IMAGE_LOADED,
        IMAGE_PROCESSING,
        IMAGE_PROCESSED
    };

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
    afx_msg void OnRangeCmds(UINT id);
    afx_msg void OnLoadImage();
    afx_msg void OnProcessImage();
    afx_msg void OnShowObjects();
    afx_msg void OnShowTypes();
	DECLARE_MESSAGE_MAP()

private:
    void ChangeStatus(Status status);
    void SetMatImage(const cv::Mat&);

private:
    CStatic _label;
    CButton _buttonLoadImage;
    CButton _buttonProcessImage;
    CButton _buttonShowImageObjects;
    CButton _buttonShowObjectsGroups;

private:
    Status _status;

private:
    int _cx;
    int _cy;

private:
    cv::Mat _sourceImage;
    cvutils::ImageProcessResult _processResult;

private:
    std::future<void> _processingImageTaskResult;

private:
    const int _instrumentsPanelHeight = 100;
};
