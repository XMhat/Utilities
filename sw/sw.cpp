#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "Kernel32")
#pragma comment(lib, "User32")
#pragma comment(lib, "Gdi32")
#pragma comment(lib, "BufferOverflowU")

#pragma comment(linker, "/NODEFAULTLIB:LIBCMT")
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")

DWORD       dwScreenWidth, dwScreenHeight;
DWORD       dwWindowWidth, dwWindowHeight;
DWORD       dwCellWidth, dwCellHeight;

const char *WindowName = "SimpleWatch";
HWND        WindowHandle;
HFONT       WindowFont;
HDC         WindowContext;
WNDCLASSEX  WindowClass;
DWORD       ThreadId;
HANDLE      ThreadHandle;
HANDLE      ProcessMutex;

VOID        (CALLBACK *ThreadCallback)(VOID);
FLOAT       TimeElapsed = 0;
CHAR        TimerStr[1024];
DWORD       TimerStrLen;
FLOAT       FrameStart, FrameEnd, FrameTime, FramesSec;
UINT64      Frequency, Counter;

extern "C" { int _fltused=0; }

VOID CALLBACK Started(VOID) { TimeElapsed += FrameTime; }

VOID CALLBACK Stopped(VOID) { }

VOID Exception(BOOL Error, const char *Format, ...)
{
  CHAR FormatBuffer[1024], ErrorBuffer[1024], FinalBuffer[1024];
  PVOID Arguments = &Format + 1;

  if(!wvsprintf(FormatBuffer, Format, (va_list)Arguments))
    wsprintf(FormatBuffer, "Unspecified error");

  if(Error && FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(),
       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), ErrorBuffer,
       sizeof(ErrorBuffer), 0))
    wsprintf(FinalBuffer, "%s. %s", FormatBuffer, ErrorBuffer);
  else
    wsprintf(FinalBuffer, "%s.", FormatBuffer);

  MessageBox(WindowHandle, FinalBuffer, WindowName, MB_ICONSTOP);
  ExitProcess(1);
}

FLOAT GetTime(VOID)
{
  if(QueryPerformanceFrequency((PLARGE_INTEGER)&Frequency) != TRUE)
    Exception(TRUE, "Failed to query performance frequency!");
  QueryPerformanceCounter((PLARGE_INTEGER)&Counter);
  return (FLOAT)Counter / (FLOAT)Frequency;
}

DWORD WINAPI ThreadProc(LPVOID)
{
  ThreadCallback = Stopped;
  FrameStart = GetTime();
  for(;;)
  {
    do
    {
      Sleep(10);
      FrameEnd = GetTime();
    }
    while(FrameEnd == FrameStart);

    ThreadCallback();

    if(SetBkColor(WindowContext, RGB(0, 0, 0)) == CLR_INVALID)
      Exception(TRUE, "Failed to set window background colour!");

    if(SetTextColor(WindowContext, RGB(255, 255, 255)) == CLR_INVALID)
      Exception(TRUE, "Failed to set text colour!");

    if(TimeElapsed >= 86400)
      TimerStrLen = wsprintf(TimerStr, "%02u-%02u:%02u",
        (unsigned)TimeElapsed/86400%100, (unsigned)TimeElapsed/3600%24,
        (unsigned)TimeElapsed/60%60);
    else if(TimeElapsed >= 3600)
      TimerStrLen = wsprintf(TimerStr, "%02u:%02u:%02u",
        (unsigned)TimeElapsed/3600%24, (unsigned)TimeElapsed/60%60,
        (unsigned)TimeElapsed%60);
    else
      TimerStrLen = wsprintf(TimerStr, "%02u:%02u.%02u",
        (unsigned)TimeElapsed/60%60, (unsigned)TimeElapsed%60,
        (unsigned)(TimeElapsed*100)%100);

    if(TextOut(WindowContext, 0, 0, TimerStr, TimerStrLen) == 0)
      Exception(TRUE, "Failed to write updated text!");

    FrameTime = FrameEnd - FrameStart;
    FrameStart = FrameEnd;
    FramesSec = 1.0f / FrameTime;
  }
  return 0;
}

LRESULT CALLBACK WinProc(HWND H, UINT M, WPARAM W, LPARAM L)
{
  switch(M)
  {
    case WM_LBUTTONUP:
      ThreadCallback = ThreadCallback == Started ? Stopped : Started;
      if(SendMessage(WindowHandle, WM_NCLBUTTONUP, HTCAPTION, 0) != 0)
        Exception(TRUE, "Failed to toggle state message!");
      break;
    case WM_LBUTTONDOWN:
      if(GetKeyState(VK_SHIFT)<0 &&
        SendMessage(WindowHandle, WM_NCLBUTTONDOWN, HTCAPTION, 0) != 0)
          Exception(TRUE, "Failed to send move state message!");
      break;
    case WM_RBUTTONUP:
      if(ThreadCallback == Stopped)
        TimeElapsed = 0;
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(H, M, W, L);
  }
  return TRUE;
}

VOID WinMainCRTStartup(VOID)
{
  dwScreenWidth = GetSystemMetrics(SM_CXSCREEN);
  dwScreenHeight = GetSystemMetrics(SM_CYSCREEN);
  dwWindowWidth = (DWORD)((FLOAT)dwScreenWidth * 0.15f);
  dwWindowHeight = (DWORD)((FLOAT)dwScreenHeight * 0.05f);
  dwCellWidth = dwWindowWidth / 8;
  dwCellHeight = dwWindowHeight;
  SecureZeroMemory(&WindowClass, sizeof(WindowClass));
  WindowClass.cbSize = sizeof(WindowClass);
  WindowClass.style = CS_HREDRAW | CS_VREDRAW;
  WindowClass.lpfnWndProc = (WNDPROC)WinProc;
  WindowClass.hInstance = GetModuleHandle(0);
  WindowClass.hIcon = (HICON)LoadImage(WindowClass.hInstance,
    MAKEINTRESOURCE(1), IMAGE_ICON, GetSystemMetrics(SM_CXICON),
    GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
  WindowClass.hIconSm = (HICON)LoadImage(WindowClass.hInstance,
    MAKEINTRESOURCE(1), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON),
    GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
  WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
  WindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
  WindowClass.lpszClassName = WindowName;
  if(!RegisterClassEx(&WindowClass))
    Exception(TRUE, "Failed to register window class!");
  WindowHandle = CreateWindowEx(WS_EX_PALETTEWINDOW, WindowClass.lpszClassName,
    WindowClass.lpszClassName, WS_POPUP | WS_VISIBLE, 8, 8, dwWindowWidth,
    dwWindowHeight, 0, 0, WindowClass.hInstance, 0);
  if(!WindowHandle)
    Exception(TRUE, "Failed to create window!");
  WindowFont = CreateFont(-dwCellHeight, -dwCellWidth, 0, 0, 0, 0, 0, 0,
    OEM_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
    DEFAULT_PITCH, "Courier New");
  if(!WindowFont)
    Exception(TRUE, "Failed to create font for window!");
  WindowContext = GetDC(WindowHandle);
  if(!WindowContext)
    Exception(TRUE, "Failed to acquire window device context!");
  if(SelectObject(WindowContext, WindowFont) == HGDI_ERROR)
    Exception(TRUE, "Failed to select font into device context!");
  ThreadHandle = CreateThread(NULL, 0, ThreadProc, NULL, 0, &ThreadId);
  if(!ThreadHandle)
    Exception(TRUE, "Failed to create timer thread!");
  MSG Msg;
  while(GetMessage(&Msg, 0, 0, 0))
  {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }
  TerminateThread(ThreadHandle, 1);
  ExitProcess(0);
}
