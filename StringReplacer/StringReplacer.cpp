#include "pch.h"
#include "StringReplacer.h"

bool IsRegionAccessible(const MEMORY_BASIC_INFORMATION& mbi)
{
	return mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS;
}

void ReplaceStringInRegion(const HANDLE handle, const char* searchString, const char* replaceString,
                           const size_t stringLen, const MEMORY_BASIC_INFORMATION& memInfo)
{
	const auto buffer = new char[memInfo.RegionSize];

	const LPVOID startAddress = memInfo.BaseAddress;
	LPVOID endAddress = static_cast<PBYTE>(memInfo.BaseAddress) + memInfo.RegionSize;

	if (!ReadProcessMemory(handle, startAddress, buffer, memInfo.RegionSize, nullptr))
		return;

	for (size_t i = 0; i < memInfo.RegionSize - stringLen; ++i)
		if (memcmp(buffer + i, searchString, stringLen) == 0)
			if (!WriteProcessMemory(handle, static_cast<PBYTE>(startAddress) + i, replaceString, stringLen, nullptr))
				return;

	delete[] buffer;
}


void ReplaceString(const char* searchString, const char* replaceString)
{
	if (searchString == nullptr || replaceString == nullptr)
		return;

	const size_t stringLen = strlen(searchString);

	if (stringLen == 0 || stringLen != strlen(replaceString))
		return;

	const HANDLE process = GetCurrentProcess();

	MEMORY_BASIC_INFORMATION memInfo;
	LPVOID regionAddress = nullptr;

	while (VirtualQuery(regionAddress, &memInfo, sizeof(memInfo)))
	{
		if (IsRegionAccessible(memInfo))
			ReplaceStringInRegion(process, searchString, replaceString, stringLen, memInfo);
		regionAddress = static_cast<PBYTE>(memInfo.BaseAddress) + memInfo.RegionSize;
	}
}

#pragma section("shrsec", shared)
__declspec(allocate("shrsec")) char sharedSearchString[256] = {0};
__declspec(allocate("shrsec")) char sharedReplaceString[256] = {0};

void SetParams(const char* searchString, const char* replaceString)
{
	if (searchString == nullptr || replaceString == nullptr)
		return;

	strcpy_s(::sharedSearchString, searchString);
	strcpy_s(::sharedReplaceString, replaceString);
}

bool IsParamsSet()
{
	return ::sharedSearchString[0] != '\0' && ::sharedReplaceString[0] != '\0';
}

void ClearParams()
{
	::sharedSearchString[0] = '\0';
	::sharedReplaceString[0] = '\0';
}

void ReplaceString()
{
	if (IsParamsSet())
	{
		ReplaceString(::sharedSearchString, ::sharedReplaceString);
		ClearParams();
	}
}
