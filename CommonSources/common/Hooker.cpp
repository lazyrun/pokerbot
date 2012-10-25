#include "Hooker.h"

static HHOOK g_hhook;

LRESULT WINAPI LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
   MSLLHOOKSTRUCT * msll = (MSLLHOOKSTRUCT *) lParam;
   msll->flags = 0;
   return(CallNextHookEx(g_hhook, nCode, wParam, lParam));
}

LRESULT WINAPI LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
   KBDLLHOOKSTRUCT * kbll = (KBDLLHOOKSTRUCT *) lParam;
   kbll->flags ^= 16;
   return(CallNextHookEx(g_hhook, nCode, wParam, lParam));
}

Hooker::Hooker(HookType ht)
{
   if (ht == Mouse)
   {
      g_hhook = NULL;
      g_hhook = SetWindowsHookEx(WH_MOUSE_LL, 
         LowLevelMouseProc, GetModuleHandle(NULL), NULL); 
   }
   else if (ht == Keyboard)
   {
      g_hhook = NULL;
      g_hhook = SetWindowsHookEx(WH_KEYBOARD_LL, 
         LowLevelKeyboardProc, GetModuleHandle(NULL), NULL); 
   }
}

Hooker::~Hooker()
{
   UnhookWindowsHookEx(g_hhook);
   g_hhook = NULL;
}

