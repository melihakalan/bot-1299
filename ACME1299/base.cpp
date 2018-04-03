#include <vcl.h>
#include <windows.h>
#include "form.h"
#include "tib_hide.h"
#include "ThemidaSDK.h"
#pragma hdrstop
#pragma argsused

bool bOsko = false;
char* arPatch[4][2] = { {"USER32.dll", "GetWindowTextA"}, {"SHELL32.dll", "ShellExecuteA"}, {"kernel32.dll", "CreateMutexA"}, {"kernel32.dll", "CreateRemoteThread"} };

void __stdcall _exit(){
	ExitProcess(0);
}

__declspec(naked) void __stdcall _ShellExecuteA(){
	CODEREPLACE_START
	__asm{
		xor eax,eax
		mov al, bOsko
		test al,al
		jnz _osko

		mov eax,[esp]
		cmp word ptr ds:[eax], 0xC483
		jnz _down
		add esp, 0x1c
		add esp, 0x18
    	leave
		retn

		_osko:
		mov eax,[esp]
		cmp word ptr ds:[eax], 0x3D83
		jnz _down
		add esp, 0x1c
    	leave
		retn

		_down:
		call _exit
	}
	CODEREPLACE_END
}

void __stdcall DllThread()
{
	HMODULE module;
	GetModuleHandleExA(0, "kojd.dll", &module);
	GetModuleBase(module);
	TIBModuleHide(module);

	BYTE pc[] = {0xb8,0,0,0,0,0xff,0xe0};
	*(long*)(pc + 1) = (long) &_CreateMutex;
	long cadr = (long) GetProcAddress( GetModuleHandle(arPatch[2][0]), arPatch[2][1]);
	WriteProcessMemory((void*)-1,(void*)cadr,pc,sizeof(pc),0);

	WaitForInputIdle((void*)-1, 10000);

	char file[256] = {0};
	GetModuleFileName(NULL, file, 255);
	if( strstr(file,"OldSchoolKO.exe") > 0 || strstr(file,"oldschoolko.exe") > 0 )
    	bOsko = true;

	BYTE pw[] = {0xb8,0,0,0,0,0xff,0xe0};
	*(long*)(pw + 1) = (long) &_GetWindowText;
	long wadr = (long) GetProcAddress( GetModuleHandle(arPatch[0][0]), arPatch[0][1]);
	WriteProcessMemory((void*)-1,(void*)wadr,pw,sizeof(pw),0);

	BYTE ps[] = {0xb8,0,0,0,0,0xff,0xe0};
	*(long*)(ps + 1) = (long) &_ShellExecuteA;
	long sadr = (long) GetProcAddress( GetModuleHandle(arPatch[1][0]), arPatch[1][1]);
	WriteProcessMemory((void*)-1,(void*)sadr,ps,sizeof(ps),0);

	while( *(DWORD*)KO_CHAR == 0 || *(DWORD*)(*(DWORD*)(KO_CHAR) + KO_MID) == 0 || CheckLoading() )
		Sleep(500);

	Application->CreateForm(__classid(Tfrmmain),&frmmain);
	Application->Run();
	Application->ProcessMessages();
}

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	if(reason == DLL_PROCESS_ATTACH)
	{
    	CreateRemoteThread((void*)-1, 0, 0, (LPTHREAD_START_ROUTINE)DllThread, 0, 0, 0);
	}

	return 1;
}
