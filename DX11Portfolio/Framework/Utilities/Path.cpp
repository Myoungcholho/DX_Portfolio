#include "Framework.h"
#include <string>
#include <Shlwapi.h>
#include "Path.h"
#pragma comment(lib, "shlwapi.lib")

bool CPath::ExistFile(string InPath)
{
	return ExistFile(CString::ToWString(InPath));
}

bool CPath::ExistFile(wstring InPath)
{
	DWORD fileValue = GetFileAttributes(InPath.c_str());

	return fileValue < 0xFFFFFFFF;
}

bool CPath::ExistDirectory(string InPath)
{
	return ExistDirectory(CString::ToWString(InPath));
}

bool CPath::ExistDirectory(wstring InPath)
{
	DWORD attribute = GetFileAttributes(InPath.c_str());

	BOOL temp = (attribute != INVALID_FILE_ATTRIBUTES &&
		(attribute & FILE_ATTRIBUTE_DIRECTORY));

	return temp == TRUE;
}

string CPath::Combine(string InPath1, string InPath2)
{
	return InPath1 + InPath2;
}

wstring CPath::Combine(wstring InPath1, wstring InPath2)
{
	return InPath1 + InPath2;
}

string CPath::Combine(vector<string> InPaths)
{
	string temp = "";
	for (string name : InPaths)
		temp += name;

	return temp;
}

wstring CPath::Combine(vector<wstring> InPaths)
{
	wstring temp = L"";
	for (wstring name : InPaths)
		temp += name;

	return temp;
}

string CPath::GetDirectoryName(string InPath)
{
	CString::Replace(&InPath, "\\", "/");
	size_t index = InPath.find_last_of('/');

	return InPath.substr(0, index + 1);
}

wstring CPath::GetDirectoryName(wstring InPath)
{
	CString::Replace(&InPath, L"\\", L"/");
	size_t index = InPath.find_last_of('/');

	return InPath.substr(0, index + 1);
}

string CPath::GetExtension(string InPath)
{
	CString::Replace(&InPath, "\\", "/");
	size_t index = InPath.find_last_of('.');

	return InPath.substr(index + 1, InPath.length());;
}

wstring CPath::GetExtension(wstring InPath)
{
	CString::Replace(&InPath, L"\\", L"/");
	size_t index = InPath.find_last_of('.');

	return InPath.substr(index + 1, InPath.length());;
}

string CPath::GetFileName(string InPath)
{
	CString::Replace(&InPath, "\\", "/");
	size_t index = InPath.find_last_of('/');

	return InPath.substr(index + 1, InPath.length());
}

wstring CPath::GetFileName(wstring InPath)
{
	CString::Replace(&InPath, L"\\", L"/");
	size_t index = InPath.find_last_of('/');

	return InPath.substr(index + 1, InPath.length());
}

string CPath::GetFileNameWithoutExtension(string InPath)
{
	string fileName = GetFileName(InPath);

	size_t index = fileName.find_last_of('.');
	return fileName.substr(0, index);
}

wstring CPath::GetFileNameWithoutExtension(wstring InPath)
{
	wstring fileName = GetFileName(InPath);

	size_t index = fileName.find_last_of('.');
	return fileName.substr(0, index);
}

bool CPath::IsRelativePath(string InPath)
{
	return IsRelativePath(CString::ToWString(InPath));
}

bool CPath::IsRelativePath(wstring InPath)
{
	BOOL b = PathIsRelative(InPath.c_str());

	return b >= TRUE;
}

void CPath::CreateFolder(string path)
{
	CreateFolder(CString::ToWString(path));
}

void CPath::CreateFolder(wstring path)
{
	if (ExistDirectory(path) == false)
		CreateDirectory(path.c_str(), NULL);
}

void CPath::CreateFolders(string path)
{
	CreateFolders(CString::ToWString(path));
}

void CPath::CreateFolders(wstring path)
{
	CString::Replace(&path, L"\\", L"/");

	vector<wstring> folders;
	CString::SplitString(&folders, path, L"/");

	wstring temp = L"";
	for (wstring folder : folders)
	{
		temp += folder + L"/";

		CreateFolder(temp);
	}
}

void CPath::GetFiles(vector<string>* OutFiles, string InPath, string InFileFilter)
{
	vector<wstring> files;
	wstring path = CString::ToWString(InPath);
	wstring filter = CString::ToWString(InFileFilter);

	GetFiles(&files, path, filter);

	for (wstring file : files)
		OutFiles->push_back(CString::ToString(file));
}

//path: ../Temp/
//filter : *.txt
void CPath::GetFiles(vector<wstring>* OutFiles, wstring InPath, wstring InFileFilter)
{
	wstring file = InPath + InFileFilter;

	WIN32_FIND_DATA findData;
	HANDLE handle = FindFirstFile(file.c_str(), &findData);

	if (handle == INVALID_HANDLE_VALUE)
		return;

	do
	{
		wstring fileName = InPath + wstring(findData.cFileName);
		OutFiles->push_back(fileName);
	} while (FindNextFile(handle, &findData));

	FindClose(handle);
}


const WCHAR* CPath::ImageFilter = L"Image \0*.png;*.bmp;*.jpg;*.tga\0";
const WCHAR* CPath::FbxModelFilter = L"Fbx Model\0*.fbx;*.obj\0";
const WCHAR* CPath::ShaderFilter = L"HLSL\0*.fx\0";
const WCHAR* CPath::ParticleFilter = L"Particle\0*.particle\0";

void CPath::OpenFileDialog(wstring InDefaultFile, const WCHAR* InFileFilter, wstring InDefaultFolder, function<void(wstring)> InFunction, HWND InDefaultHandle)
{
	WCHAR name[255];
	wcscpy_s(name, 255, InDefaultFile.c_str());

	wstring folder = InDefaultFolder;
	CString::Replace(&folder, L"/", L"\\");

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = InDefaultHandle;
	ofn.lpstrFilter = InFileFilter;
	ofn.lpstrFile = name;
	wchar_t title[255] = L"불러오기";
	ofn.lpstrFileTitle = title;
	ofn.nMaxFile = 255;
	ofn.lpstrInitialDir = folder.c_str();
	ofn.Flags = OFN_NOCHANGEDIR;

	if (GetOpenFileName(&ofn) == FALSE)
		return;


	wstring loadName = name;
	CString::Replace(&loadName, L"\\", L"/");

	InFunction(loadName);
}

void CPath::SaveFileDialog(wstring InDefaultFile, const WCHAR* InFileFilter, wstring InDefaultFolder, function<void(wstring)> InFunction, HWND InDefaultHandle)
{
	WCHAR name[255];
	wcscpy_s(name, 255, InDefaultFile.c_str());


	wstring folder = InDefaultFolder;
	CString::Replace(&folder, L"/", L"\\");

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = InDefaultHandle;
	ofn.lpstrFilter = InFileFilter;
	ofn.lpstrFile = name;
	wchar_t title[255] = L"저장하기";
	ofn.lpstrFileTitle = title;
	ofn.nMaxFile = 255;
	ofn.lpstrInitialDir = folder.c_str();
	ofn.Flags = OFN_NOCHANGEDIR;

	if (GetSaveFileName(&ofn) == FALSE)
		return;


	wstring loadName = name;
	CString::Replace(&loadName, L"\\", L"/");

	InFunction(loadName);
}