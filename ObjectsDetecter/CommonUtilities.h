#pragma once
#include "IDGenerator.h"
#include "CommonLiterals.h"


string CStringToString(const CString& cstr);
CString StringToCString(const string& str);
CString IntToCString(int);
CRect CalculateRect(int width, int height, CPoint center);
CPoint GetRectCenter(const CRect& rect);
CSize GetRectSize(const CRect& rect);
CString GetDriveName(const CString& path);
DWORD GetCountBytesPerSector(const CString& driveName);
DWORD GetCountBytesPerSectorPathDrive(const CString& path);


CString GetAppExePath();
CString GetAppDataDirectoryPath();


int GetFileSize(const CString& path);
bool EqualFiles(const CString&, const CString&);


bool operator==(const FILETIME&, const FILETIME&);
bool operator!=(const FILETIME&, const FILETIME&);
bool operator>(const FILETIME&, const FILETIME&);
bool operator<(const FILETIME&, const FILETIME&);


typedef bool(*SwapCondition)(const void* a, const void* b);


template<typename T>
void BubbleSortPointers(vector<T*>& v, SwapCondition swapCondition)
{
	for (int i = 0; i < v.size() - 1; i++)
	{
		for (int j = i + 1; j < v.size(); j++)
		{
			if (swapCondition(v[i], v[j]))
			{
				auto temp = v[i];
				v[i] = v[j];
				v[j] = temp;
			}
		}
	}
}


void AFXAPI DDV_MinChars(CDataExchange* pDX, CString const& value, int nChars);
