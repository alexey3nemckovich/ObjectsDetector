#include "stdafx.h"
#include "CommonUtilities.h"


string CStringToString(const CString &cstr)
{
    CT2CA pszConvertedAnsiString(cstr);
    string str(pszConvertedAnsiString);
    return str;
}


CString StringToCString(const string& str)
{
	return CString(str.c_str());
}


CString IntToCString(int number)
{
	CString str;
	str.Format(_T("%d"), number);
	return str;
}


CRect CalculateRect(int width, int height, CPoint center)
{
	return CRect{
		center.x - width / 2,
		center.y - height / 2,
		center.x + width / 2,
		center.y + height / 2
	};
}


CPoint GetRectCenter(const CRect& rect)
{
	return CPoint{
		(rect.right + rect.left) / 2,
		(rect.bottom + rect.top) / 2
	};
}


CSize GetRectSize(const CRect& rect)
{
	return CSize(rect.right - rect.left, rect.bottom - rect.top);
}


CString GetDriveName(const CString& path)
{
	int driveNumber = PathGetDriveNumber(path);
	char driveLetter = _T('A') + driveNumber;
	CString driveName = CString(driveLetter) + _T(":\\");
	return driveName;
}


DWORD GetCountBytesPerSector(const CString& driveName)
{
	DWORD bytesPerSector;
	GetDiskFreeSpace(driveName, NULL, &bytesPerSector, NULL, NULL);
	return bytesPerSector;
}


DWORD GetCountBytesPerSectorPathDrive(const CString& path)
{
	auto driveName = GetDriveName(path);
	return GetCountBytesPerSector(driveName);
}


CString GetAppExePath()
{
	TCHAR exePath[MAX_PATH + 1];

	GetModuleFileName(NULL, exePath, MAX_PATH + 1);
	return CString(exePath);
}


CString GetAppDataDirectoryPath()
{
	TCHAR path[MAX_PATH]; 
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path);
	return path + CString(SEPARATOR);
}


int GetFileSize(const CString& path)
{
	return 10;
}


bool EqualFiles(const CString&, const CString&)
{
	return true;
}


bool operator==(const FILETIME& a, const FILETIME& b)
{
	if (0 == CompareFileTime(&a, &b))
	{
		return true;
	}
	
	return false;
}


bool operator!=(const FILETIME& a, const FILETIME& b)
{
	return !(a == b);
}


bool operator>(const FILETIME& a, const FILETIME& b)
{
	if (1 == CompareFileTime(&a, &b))
	{
		return true;
	}

	return false;
}


bool operator<(const FILETIME& a, const FILETIME& b)
{
	if (-1 == CompareFileTime(&a, &b))
	{
		return true;
	}

	return false;
}


void AFXAPI DDV_MinChars(CDataExchange* pDX, CString const& value, int nChars)
{
	if (pDX->m_bSaveAndValidate && value.GetLength() < nChars)
	{
		TCHAR szT[32];
		_stprintf_s(szT, _countof(szT), _T("%d"), nChars);
		CString prompt;
		AfxFormatString1(prompt, AFX_IDP_PARSE_STRING_SIZE, szT);
		AfxMessageBox(prompt, MB_ICONEXCLAMATION, AFX_IDP_PARSE_STRING_SIZE);
		prompt.Empty(); // exception prep
		pDX->Fail();
	}
	else if (pDX->m_idLastControl != 0 && pDX->m_bEditLastControl)
	{
		HWND hWndLastControl;
		pDX->m_pDlgWnd->GetDlgItem(pDX->m_idLastControl, &hWndLastControl);
		// limit the control max-chars automatically
		// send messages for both an edit control and a combobox control--one will
		// be understood and one will be disregarded, but this is the only way to
		// ensure that the characters will be limited for both kinds of controls.
		::SendMessage(hWndLastControl, EM_SETLIMITTEXT, nChars, 0);
		::SendMessage(hWndLastControl, CB_LIMITTEXT, nChars, 0);
	}
}
