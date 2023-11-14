#include <iostream>
#include <string>

#include <windows.h>
#include <tlhelp32.h>

typedef void (*SetParamsFunc)(const char*, const char*);

DWORD GetProcessIdByName(const std::wstring& processName)
{
	DWORD processId = 0;
	const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(snapshot, &processEntry))
	{
		do
		{
			if (processName == processEntry.szExeFile)
			{
				processId = processEntry.th32ProcessID;
				break;
			}
		}
		while (Process32Next(snapshot, &processEntry));
	}

	CloseHandle(snapshot);
	return processId;
}

bool InjectDll(const DWORD pid, const char* searchString, const char* replaceString)
{
	const HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (handle == nullptr)
		return false;

	const auto dll = "StringReplacer.dll";
	const HMODULE dllHandle = LoadLibraryA(dll);
	const auto setParamsFunction = reinterpret_cast<SetParamsFunc>(GetProcAddress(dllHandle, "SetParams"));
	setParamsFunction(searchString, replaceString);

	const HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
	const FARPROC loadLibrary = GetProcAddress(kernel32, "LoadLibraryA");
	const FARPROC freeLibrary = GetProcAddress(kernel32, "FreeLibrary");

	const LPVOID dllPath = VirtualAllocEx(handle, nullptr, strlen(dll) + 1, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(handle, dllPath, dll, strlen(dll) + 1, nullptr);

	HANDLE thread = CreateRemoteThread(handle, nullptr, NULL,
	                                         reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibrary), dllPath, NULL,
	                                         nullptr);
	WaitForSingleObject(thread, INFINITE);

	HMODULE injectedDll;
	GetExitCodeThread(thread, reinterpret_cast<LPDWORD>(&injectedDll));

	thread = CreateRemoteThread(handle, nullptr, NULL,
			                            reinterpret_cast<LPTHREAD_START_ROUTINE>(freeLibrary), injectedDll, NULL, nullptr);
	WaitForSingleObject(thread, INFINITE);

	CloseHandle(thread);
	VirtualFreeEx(handle, dllPath, strlen(dll) + 1, MEM_RELEASE);

	return true;
}

int main()
{
	const std::wstring processName = L"InjectionTest.exe";

	const DWORD processId = GetProcessIdByName(processName);

	if (processId == 0)
	{
		std::cout << "Process not found!" << std::endl;
		return 1;
	}

	const std::string searchString = "Hello";
	const std::string replaceString = "Adios";

	InjectDll(processId, searchString.c_str(), replaceString.c_str());

	return 0;
}
