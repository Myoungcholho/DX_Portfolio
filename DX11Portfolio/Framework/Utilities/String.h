#pragma once

class CString
{
public:
	static void SplitString(vector<string>* InResult, string InOrigin, string InToken);
	static void SplitString(vector<wstring>* InResult, wstring InOrigin, wstring InToken);

	static bool StartsWith(string InValue, string InCompare);
	static bool StartsWith(wstring InValue, wstring InCompare);

	static bool Contain(string InValue, string InCompare);
	static bool Contain(wstring InValue, wstring InCompare);

	static void Replace(string* InValue, string InCompare, string InRepace);
	static void Replace(wstring* InValue, wstring InCompare, wstring InRepace);

	static wstring ToWString(string InValue);
	static string ToString(wstring InValue);

	static string Format(const string InFormat, ...);
	static wstring Format(const wstring InFormat, ...);

	static string ToColor(const FColor& InValue);
	static FColor FromColor(const string& InValue);
};