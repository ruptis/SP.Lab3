#pragma once

extern "C" __declspec(dllexport) void ReplaceString(const char* searchString, const char* replaceString);

void ReplaceString();

extern "C" __declspec(dllexport) void SetParams(const char* searchString, const char* replaceString);

void ClearParams();