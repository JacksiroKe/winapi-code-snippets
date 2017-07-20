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

//non-message function declarations
HWND CreateAnimation(const HWND,const HINSTANCE,DWORD,const RECT&,const int);  
inline int ErrMsg(const ustring&);
void StartCommonControls(DWORD flags);

//setup some control id's
enum {
  IDC_ANIMATION=200
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
                         _T("Common Controls - Animation"), //wnd title
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
    FreeLibrary(reinterpret_cast<HINSTANCE>(static_cast<LONG_PTR>
                (GetWindowLongPtr(hwnd,GWLP_USERDATA))));
    PostQuitMessage(0);    //signal end of application
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
RECT rc={10,10,200,40};
StartCommonControls(ICC_ANIMATE_CLASS);
//Get an instance handle for shell32.dll from where an avi resource
//will be loaded for display. Note that this instance handle is used
//as the instance in the call to CreateWindowEx for the animation 
//control. This sidesteps the need to provide an HINSTANCE as the
//wparam when sending the ACM_OPEN message (see below) which is not
//supported for earlier versions of comctl32.dll (_WIN32_IE >= 0x0400)
HINSTANCE hLib=LoadLibrary(_T("shell32.dll"));
if (!hLib)
  {
  ErrMsg(_T("Failed to load shell32.dll.\nQuitting now."));
  return -1;
  }
  
//store the lib instance in the parent window's user data - the following
//will emit a C4244 warning if /wp64 is enabled with ms compilers under win32
//due to how SetWindowLongPtr is typedef'd for 32bit and 64bit compatibility.
//The warning in this context can be safely ignored. Despite this being 
//identified as a glitch under msvc.net 2003, it still exists in the later
//msvc express 2005. A workaround would be to wrap the offending call in 
//#pragma warning directives, or to typedef the fn properly for 32/64 bit
//compatibility. See http://msdn.microsoft.com/msdnmag/issues/01/08/bugslayer/
//for details.
SetWindowLongPtr(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(hLib));

HWND hwndAnimate=CreateAnimation(hwnd,hLib,ACS_TIMER|ACS_AUTOPLAY|
                                 ACS_TRANSPARENT,rc,
                                 IDC_ANIMATION);

SendMessage(hwndAnimate,ACM_OPEN,static_cast<WPARAM>(0),
            reinterpret_cast<LPARAM>(MAKEINTRESOURCE(160)));
SendMessage(hwndAnimate,ACM_PLAY,static_cast<WPARAM>(-1),MAKELONG(0,-1));
return 0;
}
//=============================================================================
HWND CreateAnimation(const HWND hParent,const HINSTANCE hInst,DWORD dwStyle,
                     const RECT& rc,const int id)
{
dwStyle|=WS_CHILD|WS_VISIBLE;
return CreateWindowEx(0,                  //extended styles
                      ANIMATE_CLASS,      //control 'class' name
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
