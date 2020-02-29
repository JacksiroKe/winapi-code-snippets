#include <windows.h>  //include all the basics
#include <tchar.h>    //string and other mapping macros
#include <string>
#include "resources.h"

//define an unicode string type alias
typedef std::basic_string<TCHAR> ustring;
//=============================================================================
//message processing function declarations
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

//non-message function declarations
inline int ErrMsg(const ustring&);
//=============================================================================
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR pStr,int nCmd)
{
ustring classname=_T("SIMPLEWND");
WNDCLASSEX wcx={0};  //used for storing information about the wnd 'class'

wcx.cbSize      = sizeof(WNDCLASSEX);           
wcx.lpfnWndProc = WndProc;             //wnd Procedure pointer
wcx.hInstance   = hInst;               //app instance
//use 'LoadImage' to load wnd class icon and cursor as it supersedes the 
//obsolete functions 'LoadIcon' and 'LoadCursor', although these functions will 
//still work. Once the window is destroyed, gdi resources (brush, cursor, icon)
//will automatically be freed by the system; you can free these resources
//manually if you prefer by calling UnregisterClass once the window is
//destroyed.
wcx.hIcon = reinterpret_cast<HICON>(LoadImage(hInst,
                                              MAKEINTRESOURCE(IDI_ICON1),
                                              IMAGE_ICON,0,0,
                                              LR_DEFAULTCOLOR));
wcx.hCursor = reinterpret_cast<HICON>(LoadImage(hInst,
                                                MAKEINTRESOURCE(IDC_CURSOR1),
                                                IMAGE_CURSOR,0,0,
                                                LR_DEFAULTCOLOR));                                                      
//load the bitmap resource and create a brush from it which will be used to
//paint the window background.
HBITMAP hBmp=reinterpret_cast<HBITMAP>(LoadImage(hInst,
                                                 MAKEINTRESOURCE(IDB_BITMAP1),
                                                 IMAGE_BITMAP,0,0,
                                                 LR_CREATEDIBSECTION));        
wcx.hbrBackground = CreatePatternBrush(hBmp); 
DeleteObject(hBmp);  //release bitmap resources
wcx.lpszClassName = classname.c_str(); 
//the window 'class' (not c++ class) has to be registered with the system
//before windows of that 'class' can be created
if (!RegisterClassEx(&wcx))
  {
  ErrMsg(_T("Failed to register wnd class"));
  return -1;
  }

int desktopwidth=GetSystemMetrics(SM_CXSCREEN);
int desktopheight=GetSystemMetrics(SM_CYSCREEN);

HWND hwnd=CreateWindowEx(0,                     //extended styles
                         classname.c_str(),     //name: wnd 'class'
                         _T("Resources: Images"),  //wnd title
                         WS_OVERLAPPEDWINDOW,   //wnd style
                         desktopwidth/4,        //position:left
                         desktopheight/4,       //position: top
                         desktopwidth/2,        //width
                         desktopheight/2,       //height
                         0,                     //parent wnd handle
                         0,                     //menu handle/wnd id
                         hInst,                 //app instance
                         0);                    //user defined info
if (!hwnd)
  {
  ErrMsg(_T("Failed to create wnd"));
  return -1;
  }

ShowWindow(hwnd,nCmd); 
UpdateWindow(hwnd);
//start message loop - windows applications are 'event driven' waiting on user,
//application or system signals to determine what action, if any, to take. Note 
//that an error may cause GetMessage to return a negative value so, ideally,  
//this result should be tested for and appropriate action taken to deal with 
//it(the approach taken here is to simply quit the application).
MSG msg;
while (GetMessage(&msg,0,0,0)>0)
  {
  TranslateMessage(&msg);
  DispatchMessage(&msg);
  }

UnregisterClass(classname.c_str(),hInst);

return static_cast<int>(msg.wParam);
}
//=============================================================================
LRESULT CALLBACK WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
switch (uMsg)
  {
  case WM_DESTROY:
    PostQuitMessage(0);    //signal end of application
    return 0;
  default:
    //let system deal with msg
    return DefWindowProc(hwnd,uMsg,wParam,lParam);  
  }
}
//=============================================================================
inline int ErrMsg(const ustring& s)
{
return MessageBox(0,s.c_str(),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
}