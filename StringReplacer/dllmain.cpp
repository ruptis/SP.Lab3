// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "StringReplacer.h"

BOOL APIENTRY DllMain(HMODULE hModule,
                      const DWORD reasonForCall,
                      LPVOID lpReserved)
{
	switch (reasonForCall)
	{
	case DLL_PROCESS_ATTACH:
		{
			ReplaceString();
			break;
		}
	case DLL_PROCESS_DETACH:
		{
			ClearParams();
			break;
		}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
