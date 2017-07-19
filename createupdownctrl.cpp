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
HWND CreateControl(const ustring&,const HWND,DWORD,const HINSTANCE,DWORD,
                   const RECT&,const int);
inline int ErrMsg(const ustring&);                     
void StartCommonControls(DWORD);

//setup some control id's
enum {
  IDC_UPDOWN=200,
  IDC_EDIT
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
                         _T("Common Controls - Updown"), //wnd title
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
RECT rc={20,20,100,24}; //used to set control dimensions
StartCommonControls(ICC_UPDOWN_CLASS);  //ICC_WIN95_CLASSES can also be used

//create a 'buddy' edit cntrl - this is associated with the updown control and
//used to display the updown control's current value.
HWND hEdit=CreateControl(_T("edit"),hwnd,WS_EX_CLIENTEDGE,cs->hInstance,0,rc,
                         IDC_EDIT);

SetRect(&rc,20,60,180,20);
HWND hUpdown=CreateControl(UPDOWN_CLASS,hwnd,0,cs->hInstance,
                           UDS_ALIGNRIGHT|UDS_SETBUDDYINT|UDS_WRAP,rc,
                           IDC_UPDOWN);

//explicitly attach the updown common control to its 'buddy' edit control
SendMessage(hUpdown,UDM_SETBUDDY,reinterpret_cast<WPARAM>(hEdit),0);

return 0;
}
//=============================================================================
HWND CreateControl(const ustring& classname,const HWND hParent,DWORD extstyle,
                   const HINSTANCE hInst,DWORD dwStyle,
                   const RECT& rc,const int id)
{
dwStyle|=WS_CHILD|WS_VISIBLE;
return CreateWindowEx(extstyle,           //extended styles
                      classname.c_str(),  //control 'class' name
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
