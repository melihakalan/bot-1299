//---------------------------------------------------------------------------


#pragma hdrstop

#include "tib_hide.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

typedef struct _UNICODE_STRING {
  USHORT  Length;
  USHORT  MaximumLength;
  PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;


typedef struct _ModuleInfoNode
{
   LIST_ENTRY LoadOrder;
   LIST_ENTRY InitOrder;
   LIST_ENTRY MemoryOrder;
   HMODULE baseAddress;
   unsigned long entryPoint;
   unsigned int size;
   UNICODE_STRING fullPath;
   UNICODE_STRING name;
   unsigned long flags;
   unsigned short LoadCount;
   unsigned short TlsIndex;
   LIST_ENTRY HashTable;
   unsigned long timestamp;
} ModuleInfoNode, *pModuleInfoNode;


typedef struct _ProcessModuleInfo
{
   unsigned int size;
   unsigned int initialized;
   HANDLE SsHandle;
   LIST_ENTRY LoadOrder;
   LIST_ENTRY InitOrder;
   LIST_ENTRY MemoryOrder;
} ProcessModuleInfo, *pProcessModuleInfo;



#define UNLINK(x) (x).Blink->Flink = (x).Flink; \
   (x).Flink->Blink = (x).Blink;

int TIBModuleHide( HMODULE hMod )
{

   ProcessModuleInfo *pmInfo = NULL;
   ModuleInfoNode *module;

   _asm
   {
      mov eax, fs:[18h]      // TEB
      mov eax, [eax + 30h]   // PEB
      mov eax, [eax + 0Ch]   // PROCESS_MODULE_INFO
      mov pmInfo, eax
   }

    module = (ModuleInfoNode *)(pmInfo->LoadOrder.Flink);

   while(module->baseAddress && module->baseAddress != hMod)
      module = (ModuleInfoNode *)(module->LoadOrder.Flink);

   if(!module->baseAddress)
      return 0;

   UNLINK(module->LoadOrder);
   UNLINK(module->InitOrder);
   UNLINK(module->MemoryOrder);
   UNLINK(module->HashTable);
   memset(module->fullPath.Buffer, 0, module->fullPath.Length);
   memset(module, 0, sizeof(ModuleInfoNode));

   return 1;
}
