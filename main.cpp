#include "mainwindow.h"

#include <QApplication>

#include <windows.h>
#include <UIAutomation.h>
#include <tchar.h>

#include "runguard.h"

// Handle to Desktop ListView, global declaration
HWND  g_hFolderView;
DWORD g_tcLastLeftButtonClickTime = 0; // Global declaration
HHOOK g_hDesktopHook; // Also declared global
MainWindow *wPtr;
 ////////////////////////////////
// Find Desktop ListView, Part 1
////////////////////////////////

BOOL CALLBACK FindDLV(HWND hWndPM, LPARAM lParam)
{
   HWND hWnd = FindWindowEx(hWndPM, NULL, _T("SHELLDLL_DefView"), NULL);

   if(hWnd)
   {
      // Gotcha!
      HWND *phWnd = (HWND *)lParam;
      *phWnd      = hWnd;

      return false;
   }

   return true;
}

////////////////////////////////
// Find Desktop ListView, Part 2
////////////////////////////////

HWND FindDesktopListView()
{
    HWND hWndPM = FindWindowEx(NULL, NULL, _T("Progman"), NULL);

    if(!hWndPM)
        return NULL;

   HWND hWnd = FindWindowEx(hWndPM, NULL, _T("SHELLDLL_DefView"), NULL);

   if(!hWnd)
   {
       EnumWindows(FindDLV, LPARAM((HWND *)&hWnd));

       // Strange, no Desktop ListView found!?
       if(!hWnd)
           return NULL;
   }

   HWND hWndLV = FindWindowEx(hWnd, NULL, _T("SysListView32"), NULL);

   return hWndLV;
}

/////////////////////////////////////
// Callback-Function for desktop hook
/////////////////////////////////////

LRESULT CALLBACK OnDTMouseEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
    // Doesn't concern us
    if(nCode < 0)
        return CallNextHookEx(g_hDesktopHook, nCode, wParam, lParam);

    if(nCode == HC_ACTION)
    {
        // Left button pressed somewhere
        if(wParam == WM_LBUTTONDOWN)
        {
            wPtr->mouseLeftPress();
        }
    }
    return CallNextHookEx(g_hDesktopHook, nCode, wParam, lParam);
}

int main(int argc, char *argv[])
{
    RunGuard guard( "CLICK PER BUTTON" );
    if (!guard.tryToRun())
        return 0;

    QApplication a(argc, argv);
    FindDesktopListView();
    if((g_hDesktopHook = SetWindowsHookEx(WH_MOUSE_LL, OnDTMouseEvent, NULL, 0)) == NULL)
    {
        // Sorry, no hook for you...
        qDebug() << "Failed to hook WH_MOUSE_LL";
        a.exit(-1);
    }
    MainWindow w;
    wPtr = &w;
    w.show();
    int ret(a.exec());
    if(g_hDesktopHook)
            UnhookWindowsHookEx(g_hDesktopHook);
    return ret;
}
