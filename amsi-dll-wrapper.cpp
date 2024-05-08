// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include <malloc.h>

typedef struct HAMSICONTEXT {
	DWORD       Signature;         // "AMSI" or 0x49534D41
	PWCHAR      AppName;           // set by AmsiInitialize
	DWORD       Antimalware;       // set by AmsiInitialize
	DWORD       SessionCount;      // increased by AmsiOpenSession
} HAMSICONTEXT;

typedef enum AMSI_RESULT {
	AMSI_RESULT_CLEAN,
	AMSI_RESULT_NOT_DETECTED,
	AMSI_RESULT_BLOCKED_BY_ADMIN_START,
	AMSI_RESULT_BLOCKED_BY_ADMIN_END,
	AMSI_RESULT_DETECTED
} AMSI_RESULT;

typedef struct HAMSISESSION {
	DWORD test;
} HAMSISESSION;

typedef struct r {
	DWORD r;
};

/*
	============== AMSI Functions ==============
*/
typedef HRESULT(*AmsiInitializeT)(LPCWSTR, HAMSICONTEXT*);
AmsiInitializeT pAmsiInitialize;
extern "C" __declspec(dllexport) HRESULT AmsiInitialize(LPCWSTR appName, HAMSICONTEXT * amsiContext) {
	return pAmsiInitialize(appName, amsiContext);
}

typedef HRESULT(*AmsiOpenSessionT)(HAMSICONTEXT, HAMSISESSION*);
AmsiOpenSessionT pAmsiOpenSession;
extern "C" __declspec(dllexport) HRESULT AmsiOpenSession(HAMSICONTEXT amsiContext, HAMSISESSION * amsiSession) {
	return pAmsiOpenSession(amsiContext, amsiSession);
}

typedef void(*AmsiCloseSessionT)(HAMSICONTEXT, HAMSISESSION);
AmsiCloseSessionT pAmsiCloseSession;
extern "C" __declspec(dllexport) void AmsiCloseSession(HAMSICONTEXT amsiContext, HAMSISESSION amsiSession) {
	pAmsiCloseSession(amsiContext, amsiSession);
}

typedef HRESULT(*AmsiScanBufferT)(HAMSICONTEXT, PVOID, ULONG, LPCWSTR, HAMSISESSION, AMSI_RESULT*);
AmsiScanBufferT pAmsiScanBuffer;
extern "C" __declspec(dllexport) HRESULT AmsiScanBuffer(HAMSICONTEXT amsiContext, PVOID buffer, ULONG length, LPCWSTR contentName, HAMSISESSION amsiSession, AMSI_RESULT * result) {
	/*
		@buffer is basically LPCWSTR without null-terminator.
	*/
	HRESULT hResult = pAmsiScanBuffer(amsiContext, buffer, length, contentName, amsiSession, result);

	if (*result > AMSI_RESULT_NOT_DETECTED) {
		wprintf(L"[+] AmsiScanBuffer: %.*s | result=%d\n", length, buffer, *result);
	}

	return hResult;
}

typedef HRESULT(*AmsiScanStringT)(HAMSICONTEXT, LPCWSTR, LPCWSTR, HAMSISESSION, AMSI_RESULT*);
AmsiScanStringT pAmsiScanString;
extern "C" __declspec(dllexport) HRESULT AmsiScanString(HAMSICONTEXT amsiContext, LPCWSTR string, LPCWSTR contentName, HAMSISESSION amsiSession, AMSI_RESULT * result) {
	wprintf(L"[+] AmsiScanString: %s\n", string);

	return pAmsiScanString(amsiContext, string, contentName, amsiSession, result);
}

typedef void(*AmsiUninitializeT)(HAMSICONTEXT);
AmsiUninitializeT pAmsiUninitialize;
extern "C" __declspec(dllexport) void AmsiUninitialize(HAMSICONTEXT amsiContext) {
	return pAmsiUninitialize(amsiContext);
}

extern "C" __declspec(dllexport) void AmsiResultIsMalware(r);

void DllInit() {
	HMODULE hAmsiDll = LoadLibraryA("C:\\Windows\\System32\\amsi.dll");
	pAmsiInitialize = (AmsiInitializeT)GetProcAddress(hAmsiDll, "AmsiInitialize");
	pAmsiOpenSession = (AmsiOpenSessionT)GetProcAddress(hAmsiDll, "AmsiOpenSession");
	pAmsiCloseSession = (AmsiCloseSessionT)GetProcAddress(hAmsiDll, "AmsiCloseSession");
	pAmsiScanBuffer = (AmsiScanBufferT)GetProcAddress(hAmsiDll, "AmsiScanBuffer");
	pAmsiScanString = (AmsiScanStringT)GetProcAddress(hAmsiDll, "AmsiScanString");
	pAmsiUninitialize = (AmsiUninitializeT)GetProcAddress(hAmsiDll, "AmsiUninitialize");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DllInit();
		printf("[+] Custom AMSI.DLL loaded\n");
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}


