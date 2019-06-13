// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

void __stdcall HookFunc(DWORD iEdx)
{
	//burada edx ile istedi�in i�lemleri yapabilirsin
	MessageBoxA(0, "Hookland�", "Hookland�", 0);
}

void __declspec(naked) HookAsm()
{
	_asm
	{
		pushad // fonksiyon i�i hook att���m�z i�in registerlerin bozulmamas� gerekiyor
		pushfd // flaglar�nda bozulmamas� gerekiyor
		push edx          // bizim HookFunc fonksiyonumuza parametre yolluyoruz EDX parametresini
		call HookFunc	  // Fonksiyonumuzu cag�r�yoruz parametresi yolland�.
		popfd
		popad

		PUSH EBP
		MOV EBP, ESP
		SUB ESP, 0xC0

		push 0x008D16E9 //fonksiyona geri d�n�yoruz
		ret
	}
}

void HookFunc()
{
	BYTE patch[] = { 0x68,0,0,0,0,0xC3 };
	*(DWORD*)(patch + 1) = DWORD(HookAsm);
	//virtualprotect ile adresi PAGE_EXECUTE_READWRITE yapman gerekebilir

	DWORD dwfuncPatchAddr = 0x008D16E0;

	DWORD oldp, bkup;
	VirtualProtect((LPVOID)dwfuncPatchAddr, 6, PAGE_EXECUTE_WRITECOPY, &oldp);

	memcpy(PVOID(dwfuncPatchAddr), patch, sizeof patch);

	VirtualProtect((LPVOID)dwfuncPatchAddr, 6, oldp, &bkup);


}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		MessageBoxA(0, "injected", "", 0);
		HookFunc();
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

