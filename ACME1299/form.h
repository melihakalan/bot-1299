//---------------------------------------------------------------------------

#ifndef formH
#define formH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "def.h"
#include <queue>
#include <Psapi.h>

#ifndef MAKEULONGLONG
#define MAKEULONGLONG(ldw, hdw) ((ULONGLONG(hdw) << 32) | ((ldw) & 0xFFFFFFFF))
#endif

#ifndef MAXULONGLONG
#define MAXULONGLONG ((ULONGLONG)~((ULONGLONG)0))
#endif

typedef struct packet_s
{
	char* buf;
	int size;

	packet_s(char* pbuf, int psize)
	{
		buf = new char[psize];
		memcpy(buf,pbuf,psize);
		size = psize;
	}

	~packet_s()
	{
		delete [] buf;
		delete this;
    }
}packet;

void __stdcall padd(char* buf, int size);
DWORD __stdcall GetMainThread();
__declspec(naked) void __stdcall _tick();
void __stdcall psend(packet *p);
void __stdcall sendall();
void StringToByte(String pStr,BYTE pByte[]);

int WINAPI _GetWindowText(HWND hWnd,LPTSTR lpString,int nMaxCount);
SIZE_T WINAPI _VirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer,SIZE_T dwLength);
HANDLE WINAPI _CreateMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes,BOOL bInitialOwner,LPCTSTR lpName);
void GetModuleBase(HMODULE module);

extern DWORD MBASE;
extern DWORD mthread;
extern DWORD pcall;
extern std::queue<packet*> qpackets;
extern DWORD hookptr;

extern int hpnum;
extern int mpnum;
extern long hptick;
extern long mptick;
extern long minortick;
extern long wolftick;
extern long magicnum;
extern long attacktick;
extern int attackdelay;
extern bool bstate;
extern int imagicidx;
extern int clantid;

extern long lchest;
extern bool bloot;
extern bool bauto;
extern bool bantistun;

void enablehook();
bool __stdcall hookrecv(DWORD DataPack, int& iOffset);
float percent(int HiVal, int LoVal);

inline int myid() { return *(int*)(*(DWORD*)(KO_CHAR) + KO_MID); }
inline int tid() { return *(int*)(*(DWORD*)(KO_CHAR) + KO_TID); }
inline bool CheckLoading(){return (bool)(!*(DWORD*)(*(DWORD*)(KO_DLGA) + 0x1c0)); }

void recvchest(char* rbuf);
void recvitem(char* rbuf);
bool checkcast(int num);
void autoz();
long getbase(int id);
long mobbase(int id);
long upcbase(int id);
long __stdcall getskill(int id);
void attackR(long base);
void commandR(long base);
void stopmove();
void movemob(int id);
int getoriginid(long base);
long getzbase();
float fdist(float x1, float y1, float x2, float y2);

//---------------------------------------------------------------------------
class Tfrmmain : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TGroupBox *GroupBox1;
	TListBox *ListBox1;
	TLabel *Label1;
	TRadioButton *RadioButton1;
	TRadioButton *RadioButton2;
	TEdit *idelay;
	TLabel *Label2;
	TGroupBox *GroupBox2;
	TCheckBox *CheckBox1;
	TCheckBox *CheckBox2;
	TCheckBox *CheckBox3;
	TCheckBox *chhp;
	TComboBox *hp;
	TLabel *Label3;
	TEdit *ihp;
	TEdit *imp;
	TCheckBox *chmp;
	TComboBox *mp;
	TLabel *Label4;
	TCheckBox *CheckBox6;
	TCheckBox *CheckBox7;
	TCheckBox *chminor;
	TLabel *Label5;
	TEdit *iminor;
	TTimer *rpr;
	TTimer *tmpot;
	TTimer *minor;
	TCheckBox *CheckBox4;
	TTimer *wolf;
	TTimer *tmatk;
	TButton *btnatk;
	TShape *sbot;
	TLabel *Label7;
	TGroupBox *GroupBox3;
	TLabel *Label6;
	TButton *Button1;
	TEdit *Edit1;
	TButton *Button2;
	TEdit *Edit3;
	TButton *Button5;
	TCheckBox *CheckBox8;
	TTimer *tmstun;
	TCheckBox *antistun;
	TGroupBox *GroupBox4;
	TLabel *Label8;
	TEdit *Edit2;
	TButton *Button3;
	TListBox *ListBox2;
	TButton *Button4;
	TEdit *Edit4;
	TButton *Button6;
	TCheckBox *ratk;
	void __fastcall CheckBox3Click(TObject *Sender);
	void __fastcall CheckBox6Click(TObject *Sender);
	void __fastcall CheckBox7Click(TObject *Sender);
	void __fastcall CheckBox2Click(TObject *Sender);
	void __fastcall rprTimer(TObject *Sender);
	void __fastcall hpChange(TObject *Sender);
	void __fastcall mpChange(TObject *Sender);
	void __fastcall tmpotTimer(TObject *Sender);
	void __fastcall chhpClick(TObject *Sender);
	void __fastcall chminorClick(TObject *Sender);
	void __fastcall minorTimer(TObject *Sender);
	void __fastcall CheckBox1Click(TObject *Sender);
	void __fastcall RadioButton1Click(TObject *Sender);
	void __fastcall RadioButton2Click(TObject *Sender);
	void __fastcall CheckBox4Click(TObject *Sender);
	void __fastcall wolfTimer(TObject *Sender);
	void __fastcall ListBox1Click(TObject *Sender);
	void __fastcall btnatkClick(TObject *Sender);
	void __fastcall tmatkTimer(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall chmpClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall CheckBox8Click(TObject *Sender);
	void __fastcall tmstunTimer(TObject *Sender);
	void __fastcall idelayDblClick(TObject *Sender);
	void __fastcall antistunClick(TObject *Sender);
	void __fastcall ListBox2Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Button6Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall ratkClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall Tfrmmain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tfrmmain *frmmain;
//---------------------------------------------------------------------------
#endif
