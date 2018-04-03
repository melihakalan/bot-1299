//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "form.h"
#include "windows.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void loadacme()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	char *pid = " USA_KnightOnline";
	CreateProcess("acmeupdate.exe",pid,NULL,NULL,true,NULL,NULL,GetCurrentDir().c_str(),&si,&pi);
	SuspendThread(pi.hThread);
	Sleep(100);

	HMODULE hKernel32 = GetModuleHandle("kernel32.dll");
	FARPROC hLoadLibrary = GetProcAddress(hKernel32, "LoadLibraryA");
	int iMemSize = strlen("gateway.dll") + 1;

	LPVOID lpVirtualMem = VirtualAllocEx(pi.hProcess, NULL, iMemSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if(!lpVirtualMem){
		MessageBox(0,"Hafiza acilamadi!","Hata",MB_ICONEXCLAMATION);
		TerminateProcess(pi.hProcess,0);
		return;
	}

	WriteProcessMemory(pi.hProcess,lpVirtualMem,"gateway.dll",iMemSize - 1,NULL);
	HANDLE hBotThread = CreateRemoteThread(pi.hProcess,NULL,NULL,(LPTHREAD_START_ROUTINE)hLoadLibrary,lpVirtualMem,NULL,NULL);
	CloseHandle(hBotThread);
	VirtualFreeEx(pi.hProcess, lpVirtualMem, iMemSize, MEM_RELEASE);

	ResumeThread(pi.hThread);
	WaitForInputIdle(pi.hProcess,INFINITE);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	Application->Terminate();
}

void loadko()
{
	char pid[8]={0};    *pid = ' ';  		// 1234
	itoa(GetCurrentProcessId(),pid+1,10);

	for(int i = 1; i <= strlen((char*)(pid+1)); i++)
	{
		*(pid+i) = *(pid+i)+1;
	}

	char dir[256];
	GetCurrentDirectoryA(256,dir);

	char* exe = Form1->Edit1->Text.c_str();

	SHELLEXECUTEINFO sinfo;
	sinfo.cbSize = sizeof(SHELLEXECUTEINFO);
	sinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	sinfo.hwnd = NULL;
    sinfo.lpVerb = "open";
	sinfo.lpFile = exe;
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

	Application->Terminate();
}

void __fastcall TForm1::Button1Click(TObject *Sender)
{
	if(FileExists("acmeupdate.exe"))
		loadacme();
	else
    	loadko();
}
//---------------------------------------------------------------------------
