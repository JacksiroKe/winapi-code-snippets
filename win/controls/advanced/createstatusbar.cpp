#include <windows.h>  //include all the basics
#include <tchar.h>    //string and other mapping macros

#if defined __MINGW_H
#define _WIN32_IE 0x0400
#endif
#include <commctrl.h>

#include <string>

//define an unicode string type alias
typedef std::basic_string<TCHAR> ustring;
//=============================================================================
//message processing function declarations
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
int OnCreate(const HWND,CREATESTRUCT*);
void OnSize(const HWND hwnd,int,int,UINT);

//non-message function declarations
HWND CreateStatusbar(const HWND,const HINSTANCE,DWORD,const RECT&,const int);
inline int ErrMsg(const ustring&);                     
void StartCommonControls(DWORD);

//setup some control id's
enum {
  IDC_STATUSBAR=200,
};
//=============================================================================
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR pStr,int nCmd)
{
ustring classname=_T("SIMPLEWND");
WNDCLASSEX wcx={0};  //used for storing information about the wnd 'class'

wcx.cbSize         = sizeof(WNDCLASSEX);           
wcx.lpfnWndProc    = WndProc;             //wnd Procedure pointer
wcx.hInstance      = hInst;               //app instance
//use 'LoadImage' to load wnd class icon and cursor as it supersedes the 
//obsolete functions 'LoadIcon' and 'LoadCursor', although these functions will 
//still work. Because the icon and cursor are loaded from system resources ie 
//they are shared, it is not necessary to free the image resources with either 
//'DestroyIcon' or 'DestroyCursor'.
wcx.hIcon         = reinterpret_cast<HICON>(LoadImage(0,IDI_APPLICATION,
                                            IMAGE_ICON,0,0,LR_SHARED));
wcx.hCursor       = reinterpret_cast<HCURSOR>(LoadImage(0,IDC_ARROW,
                                              IMAGE_CURSOR,0,0,LR_SHARED));
wcx.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE+1);   
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
                         _T("Common Controls - Statusbar"), //wnd title
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
return static_cast<int>(msg.wParam);
}
//=============================================================================
LRESULT CALLBACK WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
switch (uMsg)
  {
  case WM_CREATE:
    {
    return OnCreate(hwnd,reinterpret_cast<CREATESTRUCT*>(lParam));
    }
  case WM_DESTROY:
    {
    PostQuitMessage(0);    //signal end of application
    return 0;
    }
  case WM_SIZE:
    {
    OnSize(hwnd,LOWORD(lParam),HIWORD(lParam),static_cast<UINT>(wParam));
    return 0;
    }
  default:
    //let system deal with msg
    return DefWindowProc(hwnd,uMsg,wParam,lParam);  
  }
}
//=============================================================================
int OnCreate(const HWND hwnd,CREATESTRUCT *cs)
{
//handles the WM_CREATE message of the main, parent window; return -1 to fail
//window creation
RECT rc={0,0,0,0};  //set dimensions in parent window's WM_SIZE handler 
StartCommonControls(ICC_BAR_CLASSES);

HWND hStatusbar=CreateStatusbar(hwnd,cs->hInstance,SBARS_SIZEGRIP,rc,
                                IDC_STATUSBAR);
                           
//now store the statusbar control handle as the user data associated with the
//parent window so that it can be retrieved for later use. This will emit a 
//C4244 warning if /wp64 is enabled with ms compilers under win32 due to how 
//SetWindowLongPtr is typedef'd for 32bit and 64bit compatibility. The warning
//in this context can be safely ignored. Despite this being identified as a 
//glitch under msvc.net 2003, it still exists in the later msvc express 2005. 
//A workaround would be to wrap the offending call in #pragma warning 
//directives, or to typedef the fn properly for 32/64 bit compatibility. 
//See http://msdn.microsoft.com/msdnmag/issues/01/08/bugslayer/
//for details.
SetWindowLongPtr(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(hStatusbar));

//establish the number of partitions or 'parts' the status bar will have,
//their actual dimensions will be set in the parent window's WM_SIZE handler
int nParts[4]={0};
SendMessage(hStatusbar,SB_SETPARTS,4,reinterpret_cast<LPARAM>(&nParts));

//now put some text into each part of the status bar common control and setup 
//each part
SendMessage(hStatusbar,SB_SETTEXT,0,
            reinterpret_cast<LPARAM>(_T("Status Bar: 1st Part")));
//draw next 3 parts with a raised edge. 
//Note the syntax: 'status bar part index'|'drawing flag' for the WPARAM of 
//SB_SETTEXT msg.
SendMessage(hStatusbar,SB_SETTEXT,1|SBT_POPOUT,
            reinterpret_cast<LPARAM>(_T("2nd Part")));
SendMessage(hStatusbar,SB_SETTEXT,2|SBT_POPOUT,
            reinterpret_cast<LPARAM>(_T("3rd Part")));
SendMessage(hStatusbar,SB_SETTEXT,3|SBT_POPOUT,
            reinterpret_cast<LPARAM>(_T("4th Part")));
                   
return 0;
}
//=============================================================================
void OnSize(const HWND hwnd,int cx,int cy,UINT flags)
{
//get the statusbar control handle which has been previously stored in the user
//data associated with the parent window.
HWND hStatusbar=reinterpret_cast<HWND>(static_cast<LONG_PTR>
                                   (GetWindowLongPtr(hwnd,GWLP_USERDATA)));
                                   
//partition the statusbar here to keep the ratio of the sizes of its parts 
//constant.Each part is set by specifing the coordinates of the right edge of 
//each part. -1 signifies the rightmost part of the parent.
int nHalf=cx/2;
int nParts[]={nHalf,nHalf+nHalf/3,nHalf+nHalf*2/3,-1};
SendMessage(hStatusbar,SB_SETPARTS,4,reinterpret_cast<LPARAM>(&nParts));
//resize statusbar so it's always same width as parent's client area
SendMessage(hStatusbar,WM_SIZE,0,0);
}
//=============================================================================
HWND CreateStatusbar(const HWND hParent,const HINSTANCE hInst,DWORD dwStyle,
                     const RECT& rc,const int id)
{
dwStyle|=WS_CHILD|WS_VISIBLE;
return CreateWindowEx(0,                  //extended styles
                      STATUSCLASSNAME,    //control 'class' name
                      0,                  //control caption
                      dwStyle,            //wnd style
                      rc.left,            //position: left
                      rc.top,             //position: top
                      rc.right,           //width
                      rc.bottom,          //height
                      hParent,            //parent window handle
                      //control's ID
                      reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                      hInst,              //instance
                      0);                 //user defined info
}
//=============================================================================
inline int ErrMsg(const ustring& s)
{
return MessageBox(0,s.c_str(),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
}
//=============================================================================
void StartCommonControls(DWORD flags)
{
//load the common controls dll, specifying the type of control(s) required 
INITCOMMONCONTROLSEX iccx;
iccx.dwSize=sizeof(INITCOMMONCONTROLSEX);
iccx.dwICC=flags;
InitCommonControlsEx(&iccx);
}
