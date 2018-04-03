#include <windows.h>
#include <TlHelp32.h>
#pragma argsused

void __stdcall load()
{
	char pid[8]={0};    *pid = ' ';  		// 1234
	itoa(GetCurrentProcessId(),pid+1,10);

	for(int i = 1; i <= strlen((char*)(pid+1)); i++)
	{
		*(pid+i) = *(pid+i)+1;
	}

	char dir[256];
	GetCurrentDirectoryA(256,dir);

	SHELLEXECUTEINFO sinfo;
	sinfo.cbSize = sizeof(SHELLEXECUTEINFO);
	sinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	sinfo.hwnd = NULL;
    sinfo.lpVerb = "open";
	sinfo.lpFile = "KnightOnline.exe";
	sinfo.lpParameters = pid;
	sinfo.lpDirectory = dir;
	sinfo.nShow = 1;
	sinfo.hInstApp = NULL;
	ShellExecuteExA(&sinfo);

	HMODULE hKernel32 = GetModuleHandle("kernel32.dll");
	FARPROC hLoadLibrary = GetProcAddress(hKernel32, "LoadLibraryA");
	int iMemSize = strlen("kojd.dll") + 1;

	LPVOID lpVirtualMem = VirtualAllocEx(sinfo.hProcess, NULL, iMemSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if(!lpVirtualMem){
		MessageBox(0,"Hafiza acilamadi!","Hata",MB_ICONEXCLAMATION);
		TerminateProcess(sinfo.hProcess,0);
		return;
	}

	WriteProcessMemory(sinfo.hProcess,lpVirtualMem,"kojd.dll",iMemSize - 1,NULL);
	HANDLE hBotThread = CreateRemoteThread(sinfo.hProcess,NULL,NULL,(LPTHREAD_START_ROUTINE)hLoadLibrary,lpVirtualMem,NULL,NULL);
	VirtualFreeEx(sinfo.hProcess, lpVirtualMem, iMemSize, MEM_RELEASE);
	CloseHandle(sinfo.hProcess);
}

DWORD getProcessBase(char* lpszModuleName)
{
   HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
   DWORD dwModuleBaseAddress = 0;
   if(hSnapshot != INVALID_HANDLE_VALUE)
   {
	  MODULEENTRY32 ModuleEntry32 = {0};
      ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
      if(Module32First(hSnapshot, &ModuleEntry32))
      {
         do
         {
			if(strcmp(ModuleEntry32.szModule, lpszModuleName) == 0)
            {
               dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
               break;
			}
         }
         while(Module32Next(hSnapshot, &ModuleEntry32));
      }
      CloseHandle(hSnapshot);
   }
   return dwModuleBaseAddress;
}

void DllThread()
{
	DWORD base = getProcessBase("acmeupdate.exe") + 0x1000;
	DWORD p_adr = base + 0x275C;

	BYTE b = 0;
	while(b != 0x6A)
		ReadProcessMemory((void*)-1,(void*)p_adr,&b,1,0);

	BYTE p[] = {0x60,0xB8,0,0,0,0,0xFF,0xD0,0x61,0xEB,0x18};
	*(long*)(p + 2) = (long)&load;
	WriteProcessMemory((void*)-1,(void*)p_adr,p,sizeof(p),0);
}

WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	switch (reason)
	{
		case DLL_PROCESS_ATTACH:
		CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DllThread,NULL,0,0);
		break;
		case DLL_PROCESS_DETACH:
		ExitProcess(1);
		break;
	}
	return 1;
}

