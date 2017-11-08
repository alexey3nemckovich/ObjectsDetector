#pragma once

//next WM_USER + 119

//SyncTable
#define CLICKED_DELETE_ROW_MSG					WM_USER + 100
#define DELETE_ROW_MSG							WM_USER + 117
#define OPTION_ROW_MSG							WM_USER + 101
#define SET_CELL_TEXT_MSG						WM_USER + 114
#define ADD_COLUMN_MSG							WM_USER + 115
#define DELETE_COLUMN_MSG						WM_USER + 116

//SyncHeader
#define SHOW_ADD_SYNC_BUNDLES_DLG_MSG			WM_USER + 102
#define HIDE_ADD_SYNC_BUNDLES_DLG_MSG			WM_USER + 103
#define SHOW_TAB_MSG							WM_USER + 104

//SyncEmptyBundleListWnd
#define ADD_SYNC_BUNDLE_MSG						WM_USER + 105

//SyncAddFileBundleItem
#define FILE_ITEM_CLICK_MSG						WM_USER + 106

//SyncAddFolderBundleItem
#define FOLDER_ITEM_CLICK_MSG					WM_USER + 107

//SyncComboBox
#define SELECTION_CHANGED_MSG					WM_USER + 108

//SyncView
#define SHOW_EDIT_BUNDLE_WND_MSG				WM_USER + 109
#define SHOW_CREATE_BUNDLE_WND_MSG				WM_USER + 110
#define SET_CONTENT_WND_MSG						WM_USER + 111
#define IMITATE_TAB_OPEN_MSG					WM_USER + 112
#define SHOW_YES_NO_DLG							WM_USER + 113

//SyncCheckBox
#define STATE_CHANGED_MSG						WM_USER + 118
