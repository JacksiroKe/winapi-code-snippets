#include <windows.h>  //include all the basics
#include <tchar.h>    //string and other mapping macros
#include <richedit.h> //to use richedit control
#include <string>

//define an unicode string type alias
typedef std::basic_string<TCHAR> ustring;
//=============================================================================
//define v4.1 richedit control class name if necessary
#if !defined MSFTEDIT_CLASS
#define MSFTEDIT_CLASS L"RICHEDIT50W"
#endif
//=============================================================================
//message processing function declarartions
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
int OnCreate(const HWND,CREATESTRUCT*);

//non-message function declarations
HWND CreateRichedit(const HWND,const HINSTANCE,DWORD,const RECT&,const int,
                    const ustring&,const ustring&);                    
inline int ErrMsg(const ustring&);          
bool IsWinxpSp1Min();
ustring GetRicheditClassName();

//setup some edit control id's
enum {
  IDCRE_RICHEDIT=200
};
//=============================================================================
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR pStr,int nCmd)
{
ustring classname=_T("SIMPLEWND");
WNDCLASSEX wcx={0};  //used for storing information about the wnd 'class'

wcx.cbSize         = sizeof(WNDCLASSEX);           
wcx.lpfnWndProc    = WndProc;             //wnd procedure pointer
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

HWND hwnd=CreateWindowEx(0,                      //extended styles
                         classname.c_str(),      //name: wnd 'class'
                         _T("Richedit Controls"),//wnd title
                         WS_OVERLAPPEDWINDOW,    //wnd style
                         desktopwidth/4,         //position:left
                         desktopheight/4,        //position: top
                         desktopwidth/2,         //width
                         desktopheight/2,        //height
                         0,                      //parent wnd handle
                         0,                      //menu handle/wnd id
                         hInst,                  //app instance
                         0);                     //user defined info
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
    return OnCreate(hwnd,reinterpret_cast<CREATESTRUCT*>(lParam));
  case WM_DESTROY:
    PostQuitMessage(0);    //signal end of application
    return 0;
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
RECT rc={10,10,220,200};

CreateRichedit(hwnd,cs->hInstance,0,rc,IDCRE_RICHEDIT,_T("Richedit control"),
               GetRicheditClassName());

return 0;
}
//=============================================================================
HWND CreateRichedit(const HWND hParent,const HINSTANCE hInst,DWORD dwStyle,
                    const RECT& rc,const int id,const ustring& caption,
                    const ustring& classname)
{
dwStyle|=WS_CHILD|WS_VISIBLE;
ustring cap=caption + _T(" - ") + classname;
return CreateWindowEx(WS_EX_CLIENTEDGE,             //extended styles
                      classname.c_str(),            //control 'class' name
                      cap.c_str(),                  //control caption
                      dwStyle,                      //control style 
                      rc.left,                      //position: left
                      rc.top,                       //position: top
                      rc.right,                     //width
                      rc.bottom,                    //height
                      hParent,                      //parent window handle
                      //control's ID
                      reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                      hInst,                        //application instance
                      0);                           //user defined info
}
//=============================================================================
inline int ErrMsg(const ustring& s)
{
return MessageBox(0,s.c_str(),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
}
//=============================================================================
bool IsWinxpSp1Min()
{
//return true if operating sytem is winxpsp1(windows xp, service pack 1) or 
//later. Winxpsp1 is the minimum system required to use a richedit v4.1 but 
//only when UNICODE is defined. Use GetVersionEx rather than later system
//determining or verification functions to ensure backward compatibility with
//win9x.
OSVERSIONINFO osvi={0};
osvi.dwOSVersionInfoSize=sizeof(osvi);

if (!GetVersionEx(&osvi))
  {
  ErrMsg(_T("GetVersionEx failed"));
  return false;
  }
//determine if system is winxp minimum
if (osvi.dwMajorVersion>=5 && osvi.dwMinorVersion>=1)
  {
  //now check if system is specifically winxp and, if so, what service
  //pack version
  if (osvi.dwMajorVersion==5 && osvi.dwMinorVersion==1)
    {
    //The following test assumes that the szCSDVersion member of the 
    //OSVERSIONINFO struct's format will always be a string like 
    //"Service Pack x", where 'x' is a number >=1. This is fine for sp1 and 
    //sp2 but future service packs may have a different string descriptor.
    ustring test=_T("Service Pack 1");
    if (osvi.szCSDVersion>=test)
      {
      return true;
      }
    else
      {
      return false; //pre-winxpsp1
      }
    }
  return true;
  }

return false;
}
//=============================================================================
ustring GetRicheditClassName()
{
HINSTANCE hLib;

//try to load latest version of rich edit control. Since v4.1 is available only 
//as an UNICODE control on a minimum of winxp with service pack 1 (sp1) 
//installed, use ugly preprocessor conditional to ensure that an attempt to 
//load Msftedit.dll is only made if UNICODE is defined.
#if defined UNICODE
if (IsWinxpSp1Min())
  {
  //try to get richedit v4.1, explicitly use wide character string as this is 
  //UNICODE only
  hLib=LoadLibrary(L"Msftedit.dll"); 
  if (hLib)
    {
    return MSFTEDIT_CLASS;
    }
  }
#endif

//can't get latest version (v4.1) so try to get earlier one
hLib=LoadLibrary(_T("Riched20.dll")); //Rich Edit Version 2.0/3.0
if (!hLib)
  {
  hLib=LoadLibrary(_T("Riched32.dll")); //Rich Edit Version 1.0
  if (!hLib)
    {
    //can't get this version so inform user
    ErrMsg(_T("Failed to load any richedit library"));
    return _T("");
    }
  else    //version 1.0 is good
    {
    return _T("RichEdit");
    }
  }
//version 2.0/3.0 is good
return RICHEDIT_CLASS;
}
