/////////////////////////////////////////////////////////////////////
//	File	: DLLInterface
//	Desc.	:
//		17.Nov.00 (c) Relic Entertainment Inc.
//

#include "pch.h"

#include "RDNDll.h"

#define WIN32_LEAN_AND_MEAN
#define STRICT

#include <windows.h>

/////////////////////////////////////////////////////////////////////
//	Desc.	: Entry point for the ModRDN DLL
//	Result	:
//	Param.	:
//	Author	:
//
BOOL WINAPI DllMain(HMODULE h, ULONG reason, LPVOID)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		//
		DisableThreadLibraryCalls(h);

		//
		RDNDllInterfaceInitialize();
		break;

	case DLL_PROCESS_DETACH:
		//
		RDNDllInterfaceShutdown();
		break;
	}

	return true;
}
