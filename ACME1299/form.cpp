//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "form.h"
#include "ThemidaSDK.h"
#include <math.h>
#include <windows.h>
#include <TlHelp32.h>
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tfrmmain *frmmain;

int hpnum			= SKILL_HP45;
int mpnum			= SKILL_MP120;

DWORD MBASE			= 0;
DWORD mthread		= 0;
DWORD pcall			= 0;
std::queue<packet*> qpackets;
DWORD hookptr	= 0;

long hptick = 0;
long mptick = 0;
long minortick = 0;
long wolftick = 0;
long magicnum = 0;
long attacktick = 0;
bool bstate = false;
int attackdelay = 1275;
int imagicidx = -1;
int clantid = 0;

long lchest = 0;
bool bloot = false;
bool bauto = true;
bool bantistun = false;

//---------------------------------------------------------------------------
__fastcall Tfrmmain::Tfrmmain(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void enablehook()
{
	//*(DWORD*)0x6a88ec = (DWORD)&hookrecv;

	BYTE recvp[]		= {0xff,0x15,0,0,0,0,0xeb,0x2c};
	hookptr				= (DWORD)&hookrecv;
	*(DWORD*)(recvp+2)	= (DWORD)&hookptr;
	WriteProcessMemory((void*)-1,(void*)0x64910C,recvp,8,0);
}
//---------------------------------------------------------------------------

bool __stdcall hookrecv(DWORD DataPack, int& iOffset)
{
	int iSize = *(DWORD*)(DataPack + 4);
	char *pBuf = (char*)*(DWORD*)(DataPack + 8);

	BYTE wiz = *pBuf;

	switch(wiz)
	{
		case 0x23:
		if(bloot)
			recvchest(pBuf+1);
		break;

		case 0x24:
		if(bloot)
			recvitem(pBuf+1);
		break;
    }

	asm
	{
		push iOffset
		push DataPack
		mov ecx,[KO_DLGA]
		mov eax,KO_RCVC
		call eax
	}

	return true;
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::CheckBox3Click(TObject *Sender)
{
	if(CheckBox3->Checked)
	{
		BYTE p1[]= {0xe9,0x3a,0x14,0x00,0x00};
		WriteProcessMemory((void*)-1,(void*)0x468DF5,p1,sizeof(p1),0);
	}
	else
	{
		BYTE p0[]= {0x84,0xc0,0x75,0x0d,0xb0};
		WriteProcessMemory((void*)-1,(void*)0x468DF5,p0,sizeof(p0),0);
    }
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::CheckBox6Click(TObject *Sender)
{
	DWORD base = *(DWORD*)KO_CHAR;

	if(CheckBox6->Checked)
		*(float*)(base + KO_VELO) = 1.5;
	else
		*(float*)(base + KO_VELO) = 1.0;
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::CheckBox7Click(TObject *Sender)
{
	DWORD base = *(DWORD*)KO_CHAR;

	if(CheckBox7->Checked)
		*(float*)(base + KO_VELO) = 2.0;
	else
		*(float*)(base + KO_VELO) = 1.0;
}
//---------------------------------------------------------------------------

//	dlga
// 	inv 0x3c
//	m_pMySlot[0]	0x104
// 3c durab
// 38 count
// 30 item
// 34 ext1

void __fastcall Tfrmmain::CheckBox2Click(TObject *Sender)
{
	rpr->Enabled = CheckBox2->Checked;
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::rprTimer(TObject *Sender)
{
	if( CheckLoading() )
		return;

	DWORD dw = *(DWORD*)KO_DLGA;
	dw = *(DWORD*)( dw + 0x3c);

	DWORD slot = dw + 0x104 + (4 * 6);
	DWORD icon = *(DWORD*)slot;

	int durab;
	long itemid;
	char buf[7];

	if(icon)
	{
		durab = *(int*)(icon + 0x3c);
		if(durab < 1000)
		{
			itemid = *(long*)(*(long*)(icon + 0x30));
			itemid += *(long*)(*(long*)(icon + 0x34));

			*buf = 0x3b;
			*(buf+1) = 0x1;
			*(buf+2) = 0x6;
			*(long*)(buf+3) = itemid;
			padd(buf,7);
		}
	}

	slot = dw + 0x104 + (4 * 8);
	icon = *(DWORD*)slot;

	if(icon)
	{
		durab = *(int*)(icon + 0x3c);
		if(durab < 1000)
		{
			itemid = *(long*)(*(long*)(icon + 0x30));
			itemid += *(long*)(*(long*)(icon + 0x34));

			*buf = 0x3b;
			*(buf+1) = 0x1;
			*(buf+2) = 0x8;
			*(long*)(buf+3) = itemid;
			padd(buf,7);
		}
	}
}
//---------------------------------------------------------------------------

int makecast(int num)
{
	int c = *(int*)( *(DWORD*)(KO_CHAR) + KO_CLSS);
	return( c*1000 + num );
}

void __fastcall Tfrmmain::hpChange(TObject *Sender)
{
	switch(hp->ItemIndex)
	{
		case 0:
		hpnum = SKILL_HP45;
		break;

		case 1:
		hpnum = SKILL_HP90;
		break;

		case 2:
		hpnum = SKILL_HP180;
		break;

		case 3:
		hpnum = SKILL_HP360;
		break;

		case 4:
		hpnum = SKILL_HP720;
		break;
	}
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::mpChange(TObject *Sender)
{
	switch(mp->ItemIndex)
	{
		case 0:
		mpnum = SKILL_MP120;
		break;

		case 1:
		mpnum = SKILL_MP240;
		break;

		case 2:
		mpnum = SKILL_MP480;
		break;

		case 3:
		mpnum = SKILL_MP960;
		break;

		case 4:
		mpnum = SKILL_MP1920;
		break;
	}
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::tmpotTimer(TObject *Sender)
{
	if( CheckLoading() )
		return;

	DWORD base = *(DWORD*)KO_CHAR;
	int curhp = *(int*)(base + KO_HP);
	if(!curhp)	return;		//gebermiþ

	long ctick = GetTickCount();

	char buf[24] = {0x31,0x03,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	*(short*)(buf + 6) = (short)myid();
	*(short*)(buf + 8) = (short)myid();

	if(chhp->Checked)
	{
		int hp = *(int*)(base + KO_HP);
		int maxhp = *(int*)(base + KO_MXHP);
		if(hp && ctick - hptick >= 2000)
		{
			if( percent(maxhp,hp) <= ihp->Text.ToInt() )
			{
				*(long*)(buf + 2) = hpnum;
				padd(buf,24);
				hptick = ctick;
			}
		}
	}

	if(chmp->Checked)
	{
		int mp = *(int*)(base + KO_MP);
		int maxmp = *(int*)(base + KO_MXMP);
		if(mp && ctick - mptick >= 2000)
		{
			if( percent(maxmp,mp) <= imp->Text.ToInt() )
			{
				*(long*)(buf + 2) = (long)mpnum;
				padd(buf,24);
				mptick = ctick;
			}
		}
	}
}
//---------------------------------------------------------------------------

float percent(int HiVal, int LoVal)
{
	float fHiVal = (float)HiVal;
	float fLoVal = (float)LoVal;

	return (100.0 / (fHiVal / fLoVal));
}

void __fastcall Tfrmmain::chhpClick(TObject *Sender)
{
	if(chhp->Checked || chmp->Checked)
		tmpot->Enabled = true;
	else
		tmpot->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::chminorClick(TObject *Sender)
{
	minor->Enabled = chminor->Checked;
}
//---------------------------------------------------------------------------


void __fastcall Tfrmmain::minorTimer(TObject *Sender)
{
	if( CheckLoading() )
		return;

	long ctick = GetTickCount();
	int hp = *(int*)(*(DWORD*)(KO_CHAR) + KO_HP);
	int maxhp = *(int*)(*(DWORD*)(KO_CHAR) + KO_MXHP);

	char buf[24] = {0x31,0x03,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	*(short*)(buf + 6) = (short)myid();
	*(short*)(buf + 8) = (short)myid();
	*(long*)(buf + 2) = makecast(705);

	if( hp && ctick - minortick >= 125 )
	{
		if( percent(maxhp,hp) <= iminor->Text.ToInt() )
		{
			padd(buf,24);
			minortick = ctick;
		}
	}
}
//---------------------------------------------------------------------------

void recvchest(char* rbuf)
{
	int corpse = *(short*)rbuf;
	long id = *(long*)(rbuf + 2);

	if(id == -1)
		return;

	char sbuf[5];
	*sbuf = 0x24;
	*(long*)(sbuf + 1) = id;
	padd(sbuf,5);

	lchest = id;
}

void recvitem(char* rbuf)
{
	long id[6];
	int count[6];
	int i;

	for( i = 0; i < 6; i++ )
	{
		id[i] 		= *(long*)(rbuf + (i*4) + (i*2));
		count[i] 	= *(short*)(rbuf + (i*4) + (i*2) + 4);
	}

	char sbuf[9];
	*sbuf = 0x26;
	*(long*)(sbuf + 1) = lchest;

	for( i = 0; i < 6; i++ )
	{
		if(id[i])
		{
			*(long*)(sbuf + 5) = id[i];
			padd(sbuf,9);
        }
    }
}

void __fastcall Tfrmmain::CheckBox1Click(TObject *Sender)
{
	bloot = CheckBox1->Checked;
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::RadioButton1Click(TObject *Sender)
{
	bauto	= true;
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::RadioButton2Click(TObject *Sender)
{
	bauto	= false;
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::CheckBox4Click(TObject *Sender)
{
	wolf->Enabled = CheckBox4->Checked;
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::wolfTimer(TObject *Sender)
{
	if(CheckLoading())
		return;

	int hp = *(int*)(*(DWORD*)(KO_CHAR) + KO_HP);
	if(!hp)
		return;

	int cast = makecast(030);
	if( checkcast(cast) )
		return;

	DWORD base = *(DWORD*)KO_CHAR;

	long ctick = GetTickCount();
	if( ctick - wolftick < 3000 )
		return;

	char buf[24] = {0x31,0x03,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	*(short*)(buf + 6) = (short)myid();
	*(short*)(buf + 10) = (short)( (int)(*(float*)(base + KO_POSX)) * 10 );
	*(short*)(buf + 12) = (short)( (int)(*(float*)(base + KO_POSZ)) * 10 );
	*(short*)(buf + 14) = (short)( (int)(*(float*)(base + KO_POSY)) * 10 );
	*(long*)(buf + 2) = cast;

	padd(buf,24);

	wolftick = ctick;
}
//---------------------------------------------------------------------------

bool checkcast(int num)
{
	//...
	//dlga
	//OFF_STATEBAR 0x50
	// 0xc4

	DWORD dw = *(DWORD*)KO_DLGA;
	dw = *(DWORD*)(dw + 0x4c);

	DWORD listbase = *(DWORD*)(dw + 0x1E0);
	DWORD item = *(DWORD*)listbase;
	DWORD tmp,id;

	while(item != listbase)
	{
		tmp = *(DWORD*)(item + 8);
		if(tmp)
		{
			id = *(DWORD*)tmp;
			if((int)id == num)
				return true;
		}
    	item = *(DWORD*)item;
	}

    return false;
}

void __fastcall Tfrmmain::ListBox1Click(TObject *Sender)
{
	Label1->Caption = ListBox1->Items->operator [](ListBox1->ItemIndex);

	switch( ListBox1->ItemIndex )
	{
		case 0:
		magicnum = makecast(003);	//archery
		break;

		case 1:
		magicnum = makecast(500);	//through
		break;

		case 2:
		magicnum = makecast(515);	//multiple
		break;

		case 3:
		magicnum = makecast(525);	//perfect
		break;

		case 4:
		magicnum = makecast(540);	//arc
		break;

		case 5:
		magicnum = makecast(555);	//shower
		break;

		case 6:
		magicnum = makecast(552);
		break;

		case 7:
		magicnum = makecast(560);
		break;
	}

	imagicidx = ListBox1->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::btnatkClick(TObject *Sender)
{
	if(imagicidx == -1)
	{
		MessageBoxA(this->Handle,"skill secin!","hata",0);
		return;
	}

	if(!bstate)
	{
		bstate = true;
		btnatk->Caption = "durdur";
		sbot->Brush->Color = clLime;

		attackdelay = idelay->Text.ToInt();
		tmatk->Enabled = true;
	}
	else
	{
		bstate = false;
		btnatk->Caption = "baslat";
		sbot->Brush->Color = clMaroon;

		tmatk->Enabled = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::tmatkTimer(TObject *Sender)
{
	if(CheckLoading())
		return;

	DWORD base = *(DWORD*)KO_CHAR;
	int hp = *(int*)(base + KO_HP);
	if(!hp)	return;

	long tick = GetTickCount();

	if(tick - attacktick < attackdelay)
		return;

	long mob	= 0;	//base
	int id		= 0;	//tid

	if(bauto)
	{
		mob = getzbase();
		if(!mob) return;
		id = *(int*)(mob + KO_MID);
	}
	else
	{
		id = tid();
		if(!id || id == -1)	return;
		mob = getbase(id);
		if(!mob)	return;
	}

	float mx = *(float*)(base + KO_POSX);
	float my = *(float*)(base + KO_POSY);

	float tx = *(float*)(mob + KO_POSX);
	float ty = *(float*)(mob + KO_POSY);

	float dist = fdist(mx,my,tx,ty);

	if(dist > 11.5)
		return;

	char buf3[24] = {0x31,0x03,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x0D,0x02,0x06,0x00,0xB7,0x01,0x9B,0xFF,0x00,0x00,0xF0,0x00,0x0F,0x00};

	*(long*)(buf3 + 2) = magicnum;
	*(short*)(buf3 + 6) = myid();
	*(short*)(buf3 + 8) = id;

	if(imagicidx == 2 || imagicidx == 5)
		*(buf3 + 16) = 0x0;

	if(imagicidx == 6)    	// cs
		*(buf3 + 22) = 0xA;

	padd(buf3,24);

	int i;
	if(imagicidx == 2)	//3lü
	{
		for(i = 1; i < 3; i++)
		{
			*(buf3 + 16) = (BYTE)i;
			padd(buf3,24);
			Sleep(5);
		}
	}

	if(imagicidx == 5)	//5li
	{
		for(i = 1; i < 5; i++)
		{
			*(buf3 + 16) = (BYTE)i;
			padd(buf3,24);
			Sleep(5);
		}
	}

	if(ratk->Checked && dist <= 3.0)
	{
		char pr[] = {0x08,0x01,0x01,0xff,0xff,0x77,0x00,0x09,0x00};
		*(short*)(pr + 3) = id;
		padd(pr,sizeof(pr));
    }

    attacktick = tick;
}

long getzbase()
{
	long vpos	= *(DWORD*)KO_CHAR + KO_POSX;
	int nation	= *(int*)(*(DWORD*)KO_CHAR + KO_NAT);

	asm
	{
		push vpos
		push nation
		mov ecx,[KO_AREA]
		mov eax,KO_GETZBASE
		call eax
    }
}

void autoz()
{
	asm
	{
		mov ecx,[KO_DLGA]
		mov eax,KO_GETZ
		call eax
    }
}

long getbase(int id)
{
	long base = mobbase(id);
	if(!base)
		base = upcbase(id);

	return base;
}

long mobbase(int id)
{
	long lret = 0;
	int _id = id;
	asm
	{
		mov ecx,[KO_AREA]
		push 0
		push _id
		mov eax,KO_FMBS
		call eax
		mov [&lret],eax
	}
	return lret;
}

long upcbase(int id)
{
	long lret = 0;
	int _id = id;
	asm
	{
		mov ecx,[KO_AREA]
		push 0
		push _id
		mov eax,KO_FPBS
		call eax
		mov [&lret],eax
	}
	return lret;
}

long __stdcall getskill(int id)
{
	long base = 0;
	int _id = id;

	asm
	{
		lea eax, &_id
		push eax
		lea eax, &base
		push eax
		mov ecx, [KO_PSFN]
		lea ecx, [ecx + 0x14]
		mov eax, KO_CSFN
		call eax
	}

	if(!base)
		return 0;
	else
		return base + 0x10;
}

void attackR(long base)
{
	long cbase = *(long*)KO_CHAR;

	if( *(BYTE*)(cbase + KO_CATK) == 0x1 )
    	return;

	stopmove();
	*(int*)(cbase + KO_TID) = *(int*)(base + KO_MID);
	commandR(base);
}

void commandR(long base)
{
	asm
	{
		push base
		push 1
		mov ecx, [KO_DLGA]
		mov eax, KO_RATK
		call eax
	}
}

void stopmove()
{
	asm
	{
		mov ecx, [KO_DLGA]
		push 1
		push 0
		mov eax, KO_MOVE
		call eax
    }
}

void movemob(int id)
{
	asm
	{
		mov ecx, [KO_DLGA]
		push 1
		push 1
		mov eax, KO_MOVE
		call eax

		mov ecx, [KO_CHAR]
		push id
		mov eax, KO_SETMOVE
		call eax
    }
}

int getoriginid(long base)
{
	 long ref = *(long*)(base + 0x274);
	 if(ref)
		return *(int*)ref;
	 else
     	return -1;
}

void misc()
{
	long base = *(long*)KO_CHAR;

	if(bantistun && *(BYTE*)(base + KO_STUN) == 0x1)
		*(BYTE*)(base + KO_STUN) = 0x0;
}

float fdist(float x1, float y1, float x2, float y2)
{
	return pow((double)((x2-x1)*(x2-x1)) + (double)((y2-y1)*(y2-y1)),0.5) / 4;
}

void __fastcall Tfrmmain::Button1Click(TObject *Sender)
{
	clantid = tid();
	Button2->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::Button2Click(TObject *Sender)
{
	//	3c1101ffffffff

	char buf[7];
	*buf = 0x3c;
	*(buf+1) = 0x11;
	*(buf+2) = 0x1;
	*(short*)(buf+3) = (short)clantid;
	*(short*)(buf+5) = (short) Edit1->Text.ToInt();
	padd(buf,7);
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::chmpClick(TObject *Sender)
{
	if(chhp->Checked || chmp->Checked)
		tmpot->Enabled = true;
	else
		tmpot->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::FormCreate(TObject *Sender)
{
	CODEREPLACE_START

	DWORD vadr = (DWORD)GetProcAddress(GetModuleHandle("kernel32.dll"), "VirtualQuery");
	BYTE pv[] = {0xB8,0,0,0,0,0xFF,0xE0};
	*(long*)(pv + 1) = (long) &_VirtualQuery;
	WriteProcessMemory((void*)-1,(void*)vadr,pv,sizeof(pv),0);

	enablehook();

	mthread = GetMainThread();

	pcall = 0x648210;
	BYTE pc[] = {0x8b,0x3c,0x24,0x83,0xc4,0x04,0x89,0x3d,0x33,0x16,0x40,0x00,0xe8,0,0,0,0,0x8b,0x3d,0x33,0x16,0x40,0x00,0xff,0xe7};
	*(long*)(pc + 13) = 0x451B80 - (pcall + 12) - 5;
	WriteProcessMemory((void*)-1,(void*)pcall,pc,sizeof(pc),0);

	BYTE ptick[] = {0xB8,0,0,0,0,0xFF,0xE0};
	*(long*)(ptick + 1) = (long) &_tick;
	WriteProcessMemory((void*)-1,(void*)0x5C3FE3,ptick,sizeof(ptick),0);

	CODEREPLACE_END
}
//---------------------------------------------------------------------------

DWORD __stdcall GetMainThread()
{
	THREADENTRY32 te32;
	HANDLE hThread;
	ULONGLONG ullMinCreateTime = MAXULONGLONG;
	DWORD dwMain = 0;

	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);

	if(hThreadSnap == (HANDLE)-1)	return 0;

	te32.dwSize = sizeof(THREADENTRY32);
	if(!Thread32First(hThreadSnap,&te32))
	{
		CloseHandle(hThreadSnap);
    	return 0;
	}

	do
	{
		if(te32.th32OwnerProcessID == GetCurrentProcessId())
		{
			hThread = OpenThread(THREAD_QUERY_INFORMATION,false,te32.th32ThreadID);
			if(hThread)
			{
				FILETIME afTimes[4] = {0};
				GetThreadTimes(hThread,&afTimes[0],&afTimes[1],&afTimes[2],&afTimes[3]);
				ULONGLONG ullTest = MAKEULONGLONG(afTimes[0].dwLowDateTime,afTimes[0].dwHighDateTime);
				if (ullTest && ullTest < ullMinCreateTime)
				{
					ullMinCreateTime = ullTest;
					dwMain = te32.th32ThreadID; // let it be main... :)
				}
				CloseHandle(hThread);
			}
		}
	}while(Thread32Next(hThreadSnap,&te32));

	CloseHandle(hThreadSnap);
	return dwMain;
}

__declspec(naked) void __stdcall _tick()
{
	asm{
		pushad
	}

	sendall();
	misc();

	asm{
		popad
		mov eax, [ebp + 0x1c0]
		test eax,eax
		mov eax, 0x5C3FEB
		jmp eax
    }
}

void __stdcall sendall()
{
	if(GetCurrentThreadId() == mthread)
	{
		while(!qpackets.empty())
		{
			packet* pkt = qpackets.front();
			psend(pkt);
			qpackets.pop();
		}
	}
}

void __stdcall padd(char* buf, int size)
{
	CODEREPLACE_START

	packet* pkt = new packet(buf,size);
	qpackets.push(pkt);

	CODEREPLACE_END
}

void __stdcall psend(packet *p)
{
	char *buf = p->buf;
	int size = p->size;

	asm
	{
		mov ecx,[0x818984]
		push size
		push buf
		mov eax,pcall
		call eax
	}
}
//---------------------------------------------------------------------------

void StringToByte(String pStr,BYTE pByte[])
{

   int len = pStr.Length()/2;
   int i, j;
   for (i = 0,j = 1; i < len; i++, j+=2)
   {
	String afe = "0x";

	String msg = afe + pStr.SubString(j,2);
	int number = 0;
	sscanf(msg.c_str() , "0x%X", &number);
	printf("%X\n", number);
	pByte[i]   = number;
   }
}

void __fastcall Tfrmmain::Button5Click(TObject *Sender)
{
	char buf[256];
	int size = Edit3->Text.Length()/2;

	if(!size)
		return;

	StringToByte(Edit3->Text,buf);
	padd(buf,size);
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::CheckBox8Click(TObject *Sender)
{
	tmstun->Enabled = CheckBox8->Checked;
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::tmstunTimer(TObject *Sender)
{
	if(CheckLoading())
		return;

	char	buf[24] = {0x31,0x04,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x0D,0x02,0x06,0x00,0xB7,0x01,0x9B,0xFF,0x00,0x00,0xF0,0x00,0x0F,0x00};
	*(long*)(buf+2) = 210754;
	*(short*)(buf+6) = myid();

	char sendbuf[24];	//asil paket buna kopyalanacak

	DWORD area = *(DWORD*)KO_AREA;
	DWORD listbase = *(DWORD*)(area + 0x3c);

	DWORD itbase = *(DWORD*)listbase;
	DWORD base;
	int id;

	try
	{
		while( itbase != listbase )
		{
			base = *(DWORD*)(itbase + 0x10);
			if(base)
			{
				memcpy(sendbuf,buf,24);
				id = *(int*)(base + KO_MID);
				*(short*)(sendbuf+8) = id;
				padd(sendbuf,24);
				Sleep(10);
			}

			asm
			{
				lea ecx,&itbase
				mov eax,0x46D740
				call eax
			}
		}
	}catch(...){return;};
}

SIZE_T WINAPI _VirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer,SIZE_T dwLength)
{
	int ret = VirtualQueryEx((void*)-1, lpAddress, lpBuffer, dwLength);

	if(ret && lpBuffer->Type == MEM_IMAGE)
		return 0;
	else
    	return ret;
}

void GetModuleBase(HMODULE module)
{
	long adr = (long) GetProcAddress(GetModuleHandle("kernel32.dll"),"GetModuleInformation");

	if(!adr)
		adr = (long) GetProcAddress(LoadLibrary("Psapi.dll"),"GetModuleInformation");

	if(!adr)
    	return;

	LPMODULEINFO mi;
	int cb = sizeof(MODULEINFO);
	HMODULE _module = module;
	BYTE b = false;

	asm{
		push cb
		push mi
		push _module
		push -1
		mov eax, adr
		call eax
		mov b, al
	}

	if(b)
		MBASE = (long) mi->lpBaseOfDll;
}

int WINAPI _GetWindowText(HWND hWnd,LPTSTR lpString,int nMaxCount)
{
	int ret = SendMessage(hWnd,WM_GETTEXT,nMaxCount,(long)lpString);

	if(ret)
	{
		if(	(frmmain != NULL && frmmain->Handle != NULL && hWnd == frmmain->Handle)		||
			!strcmp(lpString,"X4.2")  	||
			!strcmp(lpString,"Xloader")	||
			strstr(lpString,"loader")	||
			strstr(lpString,"acme")		||
			strstr(lpString,"kojd")		||
			strstr(lpString,"Cheat")	||
			strstr(lpString,"Olly")		||
			strstr(lpString,"Process")
		)
		{
			strcpy(lpString,"Chrome");
			return sizeof("Chrome");
        }
	}

	return ret;
}

void __fastcall Tfrmmain::idelayDblClick(TObject *Sender)
{
	//if(idelay->Text == "ma")
	//	GroupBox3->Show();
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::antistunClick(TObject *Sender)
{
	bantistun = antistun->Checked;
}
void __fastcall Tfrmmain::ListBox2Click(TObject *Sender)
{
	int i = ListBox2->ItemIndex;
	if(i==-1 || !ListBox2->Items->Count)
		return;
	Edit4->Text = ListBox2->Items->operator [](i).SubString(5,ListBox2->Items->operator [](i).Length()-4);
}
//---------------------------------------------------------------------------

void AddLog(char* pBuf,int size, int type)
{
	int i;
	String slog;

//	if(*(BYTE*)pBuf != 0x1D && *(BYTE*)pBuf != 0x16)
//		return;

	if(type == 1)	//send log
		slog = "S->\t";
	else 			//recv log
		slog = "R<-\t";

	for(i=0; i<size; i++)
		slog += IntToHex(pBuf[i],2);

	frmmain->ListBox2->AddItem(slog,0);
}

void __stdcall add1(char* buf, int size)
{
	AddLog(buf,size,1);
}

void __stdcall hooksend(DWORD,DWORD,char* buf, int size)
{
//	if( (*buf == 0x30 && *(buf+1) == 0x8 && block08) || (*buf == 0x27 && *(buf+1) == 0x2 && block27))
//	{
//		asm
//		{
//			pop ebp
//			pop eax
//			pop eax
//			ret 8
//		}
//	}
	add1(buf,size);
	asm
	{
		pop ebp
		mov ecx,[0x818984]
		push eax
		mov fs:[0],esp
		push 0x451B95
		ret
	}
}

void __fastcall Tfrmmain::Button4Click(TObject *Sender)
{
	BYTE send[] = {0x68,0,0,0,0,0xC3,0};
	*(DWORD*)(send+1) = (DWORD)&hooksend;
	WriteProcessMemory((void*)-1,(void*)0x451B8D,send,7,0);
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::Button6Click(TObject *Sender)
{
	ListBox2->Clear();
}
//---------------------------------------------------------------------------

//

void __stdcall npc(int originid, int sellid)
{
	int _originid = originid;
	int _sellid = sellid;

	__asm
	{
		mov ecx, [KO_DLGA]
		mov ecx, [ecx + 0x7C]
		push _originid
		push _sellid
		push 0
		mov eax, 0x577C80
		call eax
    }
}
void __fastcall Tfrmmain::Button3Click(TObject *Sender)
{
	if(Edit2->Text.Length() > 3 || Edit2->Text.Length() == 0)
	{
		MessageBoxA(this->Handle,"skill id'sinin son 3 hanesini girin.","hata",0);
		return;
	}

	int id = makecast( Edit2->Text.ToInt() );
	long base = getskill(id);
	if(base)
	{
		*(int*)(base + 0x34) = 140;
		MessageBoxA(this->Handle,"tamam",0,0);
	}
	else
    	MessageBoxA(this->Handle,"skill bulunamadi.",0,0);
}
//---------------------------------------------------------------------------

void __fastcall Tfrmmain::ratkClick(TObject *Sender)
{
	MessageBox(this->Handle,"elinizde bow varken dc eder, dagger ile kullanin.",0,0);
}
//---------------------------------------------------------------------------

HANDLE WINAPI _CreateMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes,BOOL bInitialOwner,LPCTSTR lpName)
{
	if( lpName && !strcmp(lpName, "ACME") )
	{
    	SetLastError(0);
		return (HANDLE)1;
	}

	return CreateMutexEx(lpMutexAttributes, lpName, CREATE_MUTEX_INITIAL_OWNER, MUTEX_ALL_ACCESS);
}


