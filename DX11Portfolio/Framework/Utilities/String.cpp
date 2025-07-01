#include "Framework.h"
#include "String.h"

#include <locale>
#include <codecvt>

#pragma warning(disable : 4996)

//////////////////////////////////////////////////////////////////////////
///@brief 문자열 자르기
///@param orgin : 원본 문자열
///@param InToken : 자를 기준이 되는 문자열
///@return 완료된 문자열 배열
//////////////////////////////////////////////////////////////////////////
void CString::SplitString(vector<string>* InResult, string InOrigin, string InToken)
{
	InResult->clear();

	int cutAt = 0; //자를 위치s
	while ((cutAt = (int)InOrigin.find_first_of(InToken)) != InOrigin.npos)
	{
		if (cutAt > 0) //자르는 위치가 0보다크면
			InResult->push_back(InOrigin.substr(0, cutAt));

		InOrigin = InOrigin.substr(cutAt + 1);
	}

	if (InOrigin.length() > 0) //자르고도 남은 것이 있다면
		InResult->push_back(InOrigin.substr(0, cutAt));
}

void CString::SplitString(vector<wstring>* InResult, wstring InOrigin, wstring InToken)
{
	InResult->clear();

	int cutAt = 0; //자를 위치s
	while ((cutAt = (int)InOrigin.find_first_of(InToken)) != InOrigin.npos)
	{
		if (cutAt > 0) //자르는 위치가 0보다크면
			InResult->push_back(InOrigin.substr(0, cutAt));

		InOrigin = InOrigin.substr(cutAt + 1);
	}

	if (InOrigin.length() > 0) //자르고도 남은 것이 있다면
		InResult->push_back(InOrigin.substr(0, cutAt));
}


//////////////////////////////////////////////////////////////////////////
///@brief 시작 문자열이 같은지 체크
///@param InValue : 체크하려는 문자열
///@param InCompare : 시작 비교문자열
//////////////////////////////////////////////////////////////////////////
bool CString::StartsWith(string InValue, string InCompare)
{
	wstring::size_type index = InValue.find(InCompare);
	if (index != wstring::npos && (int)index == 0)
		return true;

	return false;
}

bool CString::StartsWith(wstring InValue, wstring InCompare)
{
	wstring::size_type index = InValue.find(InCompare);
	if (index != wstring::npos && (int)index == 0)
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
///@brief 해당 문자열이 포함되어 있는지
///@param InValue : 체크하려는 문자열
///@param InCompare : 비교문자열
//////////////////////////////////////////////////////////////////////////
bool CString::Contain(string InValue, string InCompare)
{
	size_t found = InValue.find(InCompare);

	return found != wstring::npos;
}

bool CString::Contain(wstring InValue, wstring InCompare)
{
	size_t found = InValue.find(InCompare);

	return found != wstring::npos;
}


//////////////////////////////////////////////////////////////////////////
///@brief 해당 문자열에서 InCompare 문자를 InReplace로 변경
///@param InValue : 체크하려는 문자열
///@param InCompare : 비교문자열
///@param InReplace : 바꿀문자열
//////////////////////////////////////////////////////////////////////////
void CString::Replace(string* InValue, string InCompare, string InReplace)
{
	string temp = *InValue;

	size_t start_pos = 0;
	while ((start_pos = temp.find(InCompare, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, InCompare.length(), InReplace);
		start_pos += InReplace.length();
	}

	*InValue = temp;
}

void CString::Replace(wstring* InValue, wstring InCompare, wstring InReplace)
{
	wstring temp = *InValue;

	size_t start_pos = 0;
	while ((start_pos = temp.find(InCompare, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, InCompare.length(), InReplace);
		start_pos += InReplace.length();
	}

	*InValue = temp;
}

//////////////////////////////////////////////////////////////////////////
///@brief string형을 wstring형으로 변경
///@param value : 변환할 문자열
///@return 변환 완료 문자열
//////////////////////////////////////////////////////////////////////////
wstring CString::ToWString(string value)
{
	wstring temp = L"";
	temp.assign(value.begin(), value.end());

	return temp;
}

//////////////////////////////////////////////////////////////////////////
///@brief wstring형을 string형으로 변경
///@param value : 변환할 문자열
///@return 변환 완료 문자열
//////////////////////////////////////////////////////////////////////////
string CString::ToString(wstring value)
{
	wstring_convert<codecvt_utf8<wchar_t>> converter;

	return converter.to_bytes(value);
}
string CString::Format(const string format, ...)
{
	va_list args;

	va_start(args, format);
	size_t len = vsnprintf(NULL, 0, format.c_str(), args);
	va_end(args);

	vector<char> vec(len + 1);

	va_start(args, format);
	vsnprintf(&vec[0], len + 1, format.c_str(), args);
	va_end(args);

	return &vec[0];
}

wstring CString::Format(const wstring format, ...)
{
	va_list args;

	va_start(args, format);
	size_t len = _vsnwprintf(NULL, 0, format.c_str(), args);
	va_end(args);

	vector<WCHAR> vec(len + 1);

	va_start(args, format);
	_vsnwprintf(&vec[0], len + 1, format.c_str(), args);
	va_end(args);

	return &vec[0];
}

string CString::ToColor(const FColor& InValue)
{
	return Format("%f,%f,%f,%f", InValue.R, InValue.G, InValue.B, InValue.A);
}

FColor CString::FromColor(const string& InValue)
{
	vector<string> v;
	SplitString(&v, InValue, ",");

	return FColor(stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3]));
}

//////////////////////////////////////////////////////////////////////////