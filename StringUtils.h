#pragma once
#include <string>
#include <format>

void Log(const std::string& str);
void Log(const std::wstring& str);
// wstringに変換
std::wstring ConvertString(const std::string& str);
// wstringから変換
std::string ConvertString(const std::wstring& str);