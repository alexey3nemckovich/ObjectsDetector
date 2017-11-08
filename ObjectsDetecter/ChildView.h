#pragma once
#include "SyncLabel.h"
#include "cvUtilities.h"
#include <opencv2/imgcodecs.hpp>
#include <future>
#include "SmartPointer.h"


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

    enum class Tab
    {
        SOURCE_IMAGE,
        IMAGE_OBJECTS,
        IMAGE_OBJECTS_GROUPS
    };

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
    afx_msg void OnRangeCmds(UINT id);
    afx_msg void OnLoadImageClick();
    afx_msg void OnProcessImageClick();
    afx_msg void OnShowObjectsClick();
    afx_msg void OnShowObjectsGroupsClick();
    afx_msg LRESULT OnNewObjectsClassifyingAlgSelected(WPARAM index, LPARAM);
	DECLARE_MESSAGE_MAP()

private:
    void ChangeStatus(Status status);
    void SetMatImage(const cv::Mat&);

private:
    int _cx;
    int _cy;

private:
    CStatic _label;
    CButton _buttonLoadImage;
    CButton _buttonProcessImage;
    CButton _buttonShowImageObjects;
    CButton _buttonShowObjectsGroups;
    SmartPointer<CComboBox> _comboBoxAlg;

private:
    Tab _tab;
    Status _status;
    int _selectedAlgIndex;

private:
    cv::Mat _sourceImage;

private:
    std::future<void> _processingImageTaskResult;

private:
    const int _instrumentsPanelHeight = 100;
};
